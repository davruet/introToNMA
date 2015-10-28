function setup() {
  // uncomment this line to make the canvas the full size of the window
  createCanvas(windowWidth, windowHeight);
  lastSensorCheckTime = millis();

  updateSensorValue();
}

var sensorValue;

var maxSensorReadRate = 1000;
var lastSensorCheckTime;



function draw() {
	frameRate((sensorValue * .1) + 1);
	updateSensorValue();
	background(200);
	// draw stuff here
	// ellipse(width/2, height/2, 50, 50);
	var random = Math.random() * 300;
	color(0);
	line(0);
	ellipse(width/2, height/2, sensorValue, sensorValue);
	textSize(32);
	text("Pulse: " + sensorValue, 50,50);
}

function updateSensorValue(){
	url = "analogRead";
	var timeNow = millis();
	if (timeNow > (lastSensorCheckTime + maxSensorReadRate)){

		lastSensorCheckTime = timeNow;
		loadJSON(url, sensorUpdate);
	}
	
}

function sensorUpdate(update){
	sensorValue = update.sensorValue;
}