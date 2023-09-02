
eel.expose(js_random);
function js_random(){
    return Math.random();
}

eel.expose(Alert);
function Alert(message){
    alert(message);
}