function changeColor(){
    var redSlider = document.getElementById("red");
    var greenSlider = document.getElementById("green");
    var blueSlider = document.getElementById("blue");

    if(!redSlider || !greenSlider || !blueSlider){
        return;
    }

    var buttons = Array.from(document.getElementsByTagName("button"));
    var color = "rgb(" +
        parseInt(redSlider.value) + ", " +
        parseInt(greenSlider.value) + ", " +
        parseInt(blueSlider.value) + ")";

    buttons.forEach(function(item){
        item.style.backgroundColor = color;
    });

    var xhr = new XMLHttpRequest();
    xhr.open('GET', 'setColor?red=' + redSlider.value + '&green=' + greenSlider.value + '&blue=' + blueSlider.value, true);
    xhr.send();
}

function getColor(){
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function()
    {
        if (xhr.readyState === 4 && xhr.status === 200)
        {
            var colors = JSON.parse(xhr.responseText);

            if(!colors){
                return;
            }

            var redSlider = document.getElementById("red");
            var greenSlider = document.getElementById("green");
            var blueSlider = document.getElementById("blue");

            if(!redSlider || !greenSlider || !blueSlider){
                return;
            }

            redSlider.value = colors.red;
            greenSlider.value = colors.green;
            blueSlider.value = colors.blue;
        }
    };
    xhr.open('GET', 'getColor', true);
    xhr.send();
}

function changeSpeed(){
    var speedSlider = document.getElementById("speed");

    if(!speedSlider){
        return;
    }

    var xhr = new XMLHttpRequest();
    xhr.open('GET', 'setEffectSpeed?effectSpeed='+ speedSlider.value / 1000, true);
    xhr.send();
}

function getSpeed(){
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function()
    {
        if (xhr.readyState === 4 && xhr.status === 200)
        {
            var speed = JSON.parse(xhr.responseText);

            if(!speed){
                return;
            }

            var speedSlider = document.getElementById("speed");

            if(!speedSlider){
                return;
            }

            speedSlider.value = speed.effectSpeed * 1000;
        }
    };
    xhr.open('GET', 'getEffectSpeed', true);
    xhr.send();
}

function changeEffect(effect){

    switch (effect) {
        case 1:
            document.getElementById("speed").max = 300;
            break;
        case 2:
            document.getElementById("speed").max = 100;
            break;
    }

    var xhr = new XMLHttpRequest();
    xhr.open('GET', '/setEffect?effect='+ effect, true);
    xhr.send();
}

getColor();
getSpeed();