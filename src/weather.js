// Listen for when the watchface is opened
Pebble.addEventListener('ready', function(e){
	console.log('pebbleKit is ready');
	
	getWeather();
});

// Listen for when an app message is recieved
Pebble.addEventListener('appmessage', function(e){
	console.log('AppMessage received');
	
	getWeather();
});



var xhrRequest = function(url, type, callback){
	var xhr = new XMLHttpRequest();
	xhr.onload = function(){
		callback(this.responseText);
	};
	xhr.open(type, url);
	xhr.send();
};

function locationSuccess(pos){
	// request weather data

	var url = 'http://api.openweathermap.org/data/2.5/weather?lat=' + pos.coords.latitude + '&lon=' + pos.coords.longitude;
	
	xhrRequest(url, 'GET', function(responseText){
		
		//Get Data 
		
		var json = JSON.parse(responseText);
		console.log(JSON.stringify(json))
		//change temp from Kelvin
		var temperature = Math.round(json.main.temp - 273.15);
		console.log('Temperature is ' + temperature);
		//conditions
		var conditions = json.weather[0].main;
		console.log('Conditions are ' + conditions);
		
		
		//Send data back to pebble
		
		var dictionary = {"KEY_TEMPERATURE": temperature, "KEY_CONDITIONS": conditions };
		
		Pebble.sendAppMessage(dictionary, function(e){
			console.log('Weather info sent to Pebble successfully.');
		}, function(e){
			console.log('Error sending weather info to Pebble.');
		});
	});
	
	
}

function locationError(err){
	console.log('Error requestion location');
}

function getWeather(){
	navigator.geolocation.getCurrentPosition(
		locationSuccess,
		locationError,
		{timeout: 15000, maximumAge: 60000}
	);
}

