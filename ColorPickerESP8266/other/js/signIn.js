function signIn(){
    var login = document.getElementById("login");
    var password = document.getElementById("password");

    if(!login || !password){
        return;
    }

    var xhr = new XMLHttpRequest();
    xhr.open('GET', 'setWifiCredentials?login='+ login.value + '&password=' + password.value, true);
    xhr.send();
}

function  checkLetters(contect){
    contect.value = contect.value.replace(/[^a-z 0-9]/i, "");
}