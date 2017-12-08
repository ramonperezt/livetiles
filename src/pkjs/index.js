//http://api.openweathermap.org/data/2.5/weather?lat=42.493512&lon=-70.946111&appid=d4029eab65fead998e274001b9b56a8e

var mynews;
// Import the Clay package
var Clay = require('pebble-clay');
// Load our Clay configuration file
var clayConfig = require('./config');
// Initialize Clay
var clay = new Clay(clayConfig);


var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function getNews() {
var urlNews = 'https://newsapi.org/v1/articles?source=bbc-news&sortBy=top&apiKey=db98d60f47524aa99df29130a5f37823';

  
  // Send request to OpenWeatherMap
  xhrRequest(urlNews, 'GET', 
    function(responseText) {
      
      var json = JSON.parse(responseText);

      mynews = json.articles[0].title;      
      console.log('News1 ' + mynews);

    } 
  );
 

}

function locationSuccess(pos) {
 
  // Construct URL
  var url = 'http://api.openweathermap.org/data/2.5/weather?lat=' +
      pos.coords.latitude + '&lon=' + pos.coords.longitude + '&appid=d4029eab65fead998e274001b9b56a8e';
  
  

  // Send request to OpenWeatherMap
  xhrRequest(url, 'GET', 
    function(responseText) {
      // responseText contains a JSON object with weather info
      var json = JSON.parse(responseText);

      // Temperature in Kelvin requires adjustment
      var temperature = Math.round(json.main.temp * 9/5 - 459.67);
      var tempmax = Math.round(json.main.temp_max * 9/5 - 459.67);
      var tempmin = Math.round(json.main.temp_min * 9/5 - 459.67);
      //var temperature=10;
      console.log('Temperature is ' + temperature);
      console.log('Temperature Max ' + tempmax);
      console.log('Temperature Min' + tempmin);

      // Conditions
      var conditions = json.weather[0].main;      
      console.log('Conditions are ' + conditions);
      
      //description
      var description = json.weather[0].description;      
      console.log('Description are ' + description);
      
      // City
      var location = json.name;      
      console.log('location is ' + location);
      

      
      var dictionary = {
  'TEMPERATURE': temperature,
  'TEMPMIN': tempmin,
  'TEMPMAX': tempmax,
  'CONDITIONS': conditions,
  'DESCRIPTION': description,
  'WLOCATION': location,
  'NEWS': mynews
        
};

// Send to Pebble
Pebble.sendAppMessage(dictionary,
  function(e) {
    console.log('Weather info sent to Pebble successfully!');
  },
  function(e) {
    console.log('Error sending weather info to Pebble!');
  }
);
    }      
  );
 
}




function locationError(err) {
  
  console.log('Error requesting location!');
}



function getWeather() {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );
}


Pebble.addEventListener('ready', 
  function(e) {
    console.log('PebbleKit JS ready!');
        // Get the initial weather
    getNews();
    getWeather();
    
    
    
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log('AppMessage received!');
  }                     
);