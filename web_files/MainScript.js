var GlobalStations={}
var newStationForm=false;
var sortByName_Reverse=false;
var sortByFreq_Reverse=false;

function toggleStationList(){
    document.getElementById('radio-stations').toggleAttribute('hidden');
    if (document.getElementById('radio-stations').hasAttribute('hidden')){
        document.getElementById('RadioList_menu_btn').setAttribute('class', "menu-button");
    }else{
        document.getElementById('RadioList_menu_btn').setAttribute('class', "menu-button-active");
    }
}

function toggleConnectionSetup(){
    document.getElementById('connection-menu').toggleAttribute('hidden');
    if (document.getElementById('connection-menu').hasAttribute('hidden')){
        document.getElementById('Connection_menu_btn').setAttribute('class', "menu-button");
    }else{
        document.getElementById('Connection_menu_btn').setAttribute('class', "menu-button-active");
    }
}

function radioOn(){
    eel.py_radioOn();
}

function radioOff(){
    eel.py_radioOff();
}

document.getElementById('com_refresh').addEventListener("click", ()=>{
    eel.py_comListUpdate();
}, false);
document.getElementById("com_connect").addEventListener("click", ()=>{
    var selectedCOM=document.getElementById('com_ports').value;
    eel.py_com_connect(selectedCOM);
},false);
document.getElementById('com_disconnect').addEventListener("click",()=>{eel.py_com_disconnect()},false)

eel.expose(set_com_list)
function set_com_list(Ports, ComConnected){
    var index=0;
    var InnerHTML=""
    for (index=0; index<Ports.length; index++){
        InnerHTML+="<option value='"+Ports[index]+"'>"+Ports[index]+"</option>\n\r";
    }

    document.getElementById('com_connect').disabled=false;
    if (InnerHTML==""){
        InnerHTML="<option value='-select-'>-No COM-</option>";
        //disable Connect Button
        document.getElementById('com_connect').disabled=true;
    }
    document.getElementById("com_ports").innerHTML=InnerHTML;
    if (ComConnected){
        document.getElementById('com_connect').disabled=true;
    }
}

eel.expose(update_connection_status);
function update_connection_status(ComConnected, InfoText){
    if (ComConnected){
        document.getElementById('com_connect').disabled=true;
        document.getElementById('com_disconnect').disabled=false;
        document.getElementById('com_ports').disabled=true;
        document.getElementById('com_refresh').disabled=true;
        document.getElementById('com_connected').innerHTML="<strong>CONNECTED ("+InfoText+")</strong>";
        document.getElementById('com_connected').style="color:green";
    }else{
        document.getElementById('com_connect').disabled=false;
        document.getElementById('com_disconnect').disabled=true;
        document.getElementById('com_ports').disabled=false;
        document.getElementById('com_refresh').disabled=false;
        document.getElementById('com_connected').innerHTML="<strong>Disconnected</strong>";
        document.getElementById('com_connected').style="color:darkred";
    }
}

eel.expose(printAlert);
function printAlert(message){
    alert(message);
}

eel.expose(stationsUpdate);
function stationsUpdate(Stations){
    GlobalStations=Stations;
    var StationsList=Stations["Stations"];
    var listLength=StationsList.length;
    var table=document.getElementById("StationsTable");
    table.innerHTML=""; //erase current content of the table
    table.innerHTML+="<tr>\
                    <th>No.</th>\
                    <th>Station Name</th>\
                    <th>Frequency</th>\
                    <th>Actions</th>\
                </tr>";
    if (listLength>0){
        for (var i=0; i<listLength; i++){
            table.innerHTML+="<tr>\
                    <td>"+(i+1)+"</td>\
                    <td id='td_stationName_"+i+"'>"+StationsList[i]["Name"]+"</td>\
                    <td id='td_stationFreq_"+i+"'>"+StationsList[i]["Frequency"]+"</td>\
                    <td class='right'>\
                        <button id='stat_play_btn_"+i+"' class='options-button green-button' onclick=stationPlay("+i+")>Play</button>\
                        <button id='stat_edit_btn_"+i+"'  class='options-button' onclick=stationEdit("+i+")>Edit</button>\
                        <button id='stat_delete_btn_"+i+"'  class='options-button red-button' onclick=stationDelete("+i+")>Delete</button>\
                        <button id='stat_save_btn_"+i+"'  class='options-button green-button' onclick=stationEditSave("+i+") hidden>Save</button>\
                        <button id='stat_cancel_btn_"+i+"'  class='options-button red-button' onclick=stationEditCancel("+i+") hidden>Cancel</button>\
                    </td>\
                </tr>";
        }
    }else{
        //no item to list
        table.innerHTML+="<tr>\
                    <td colspan=4><strong>No item in list</strong></td>\
                </tr>";
    }
}

function addStation(){
    var table=document.getElementById("NewStationForm");
    if (newStationForm){
        newStationForm=false;
        table.innerHTML="";
        document.getElementById("newStationBtn").setAttribute("class", "options-button");
        var stationTable=document.getElementById("radio-stations");
        stationTable.scrollTo(0,stationTable.scrollHeight);
    }else{
        newStationForm=true;
        table.innerHTML="<br/><table>\
                        <tr><td colspan=4 style='background-color:rgb(246, 252, 255)'>New Station Form</td></tr>\
                        <tr>\
                        <th>No.</th>\
                        <th>Station Name</th>\
                        <th>Frequency</th>\
                        <th>Actions</th>\
                    </tr>\
                    <tr>\
                        <td>"+1+"</td>\
                        <td><input name='stationName' id='stationName' type='text' style='width:150px; margin-left:0px'></td>\
                        <td><input name='stationFreq' id='stationFreq' type='text' style='width:90px; margin-left:0px' inputmode='numeric'></td>\
                        <td class='right'>\
                            <button class='options-button green-button' onclick=stationSave()>Save</button>\
                            <button class='options-button red-button' onclick=stationCancel()>Cancel</button>\
                        </td>\
                    </tr></table>";
        document.getElementById("newStationBtn").setAttribute("class", "options-button-active");
        var stationTable=document.getElementById("radio-stations");
        stationTable.scrollTo(0,stationTable.scrollHeight);
    }
}

function stationCancel(){
    var table=document.getElementById("NewStationForm");
    if (newStationForm){
        newStationForm=false;
        table.innerHTML="";
        document.getElementById("newStationBtn").setAttribute("class", "options-button");
    }
}

function stationSave(){
    var table=document.getElementById("NewStationForm");
    if (newStationForm){
        var stationName=document.getElementById("stationName").value;
        var stationFreq=document.getElementById("stationFreq").value;
        eel.py_addStation(stationName, stationFreq);
        newStationForm=false;
        table.innerHTML="";
        document.getElementById("newStationBtn").setAttribute("class", "options-button");
    }
}

function stationDelete(stationNumber){
    eel.py_deleteStation(stationNumber);
}

function stationEdit(index){
    var nameCell = document.getElementById("td_stationName_"+index);
    var freqCell = document.getElementById("td_stationFreq_"+index);
    nameCell.innerHTML="<input type='text' id='editStationName_"+index+"'  style='width:150px; margin-left:0px'>"
    freqCell.innerHTML="<input type='text' id='editStationFreq_"+index+"'  style='width:90px; margin-left:0px' inputmode='numeric'>"
    document.getElementById('editStationName_'+index).value=GlobalStations["Stations"][index]["Name"];
    document.getElementById('editStationFreq_'+index).value=GlobalStations["Stations"][index]["Frequency"];
    //hide regular buttons
    document.getElementById('stat_play_btn_'+index).setAttribute('hidden', true);
    document.getElementById('stat_edit_btn_'+index).setAttribute('hidden', true);
    document.getElementById('stat_delete_btn_'+index).setAttribute('hidden', true);
    //show edit related buttons
    document.getElementById('stat_save_btn_'+index).removeAttribute('hidden');
    document.getElementById('stat_cancel_btn_'+index).removeAttribute('hidden');
}

function stationPlay(index){
    eel.py_stationPlay(index);
}

function setSSL(){
    eel.py_setSSL(document.getElementById("ssl_option").value);
}

function stationEditCancel(index){
    var nameCell = document.getElementById("td_stationName_"+index);
    var freqCell = document.getElementById("td_stationFreq_"+index);
    nameCell.innerHTML=GlobalStations["Stations"][index]["Name"];
    freqCell.innerHTML=GlobalStations["Stations"][index]["Frequency"];
   //show regular buttons
    document.getElementById('stat_play_btn_'+index).removeAttribute('hidden');
    document.getElementById('stat_edit_btn_'+index).removeAttribute('hidden');
    document.getElementById('stat_delete_btn_'+index).removeAttribute('hidden');
    //hide edit related buttons
    document.getElementById('stat_save_btn_'+index).setAttribute('hidden', true);
    document.getElementById('stat_cancel_btn_'+index).setAttribute('hidden', true);
}

function stationEditSave(index){
    var stationName=document.getElementById("editStationName_"+index).value;
    var stationFreq=document.getElementById("editStationFreq_"+index).value;
    eel.py_updateStation_atIndex(index, stationName, stationFreq);
}

function stationSortByName(){
    if (sortByName_Reverse==false){
        eel.py_stationListSort("byName", "asc");
    }else{
        eel.py_stationListSort("byName", "desc");
    }
    sortByName_Reverse=~sortByName_Reverse;
}

function stationSortByFreq(){
    if (sortByFreq_Reverse==false){
        eel.py_stationListSort("byFreq", "asc");
    }else{
        eel.py_stationListSort("byFreq", "desc");
    }
    sortByFreq_Reverse=~sortByFreq_Reverse;
}

function saveList(){
    eel.py_saveList();
}

function loadList(){
    eel.py_loadList();
}

document.getElementById("serch-mode").addEventListener("click", ()=>{
    eel.py_toggleSerchMode();
}, false);

document.getElementById("plus-button").addEventListener("click", ()=>{
    eel.py_getNextStation();
}, false);

document.getElementById("minus-button").addEventListener("click", ()=>{
    eel.py_getPrevStation();
}, false);


window.addEventListener("load", ()=>{
    eel.py_init();
    eel.getRadioStations();
},false);


eel.expose(updateRadioStatus)
function updateRadioStatus(statusRegs, controlRegs){
    if (controlRegs[0]&0x40){
        document.getElementById("sm-indicator").setAttribute("class", "indicator-on");
    }else{
        document.getElementById("sm-indicator").setAttribute("class", "indicator-off");
    }

    if (statusRegs[0]&0x80){
        document.getElementById("rf-indicator").setAttribute("class", "indicator-on");
    }else{
        document.getElementById("rf-indicator").setAttribute("class", "indicator-off");
    }

    if (statusRegs[0]&0x40){
        document.getElementById("blf-indicator").setAttribute("class", "indicator-on");
    }else{
        document.getElementById("blf-indicator").setAttribute("class", "indicator-off");
    }

    document.getElementById("freq-indicator").value=(32.768*(((statusRegs[0]&0x3F)<<8)+statusRegs[1])/4_000 + 0.225).toFixed(1);

}

document.getElementById('serch-save-new').addEventListener("click", ()=>{
    document.getElementById('radio-stations').removeAttribute('hidden');
    document.getElementById('RadioList_menu_btn').setAttribute('class', "menu-button-active");
    newStationForm=false; //the form is not already displayed
    addStation();
    //set default value for frequency
    document.getElementById("stationFreq").value=document.getElementById("freq-indicator").value;
}, false)

function timeout_handler(){
    // call python function that gets values from uC
    eel.py_getRadioStatus();
}
setInterval(timeout_handler, 1000); //every 1000ms