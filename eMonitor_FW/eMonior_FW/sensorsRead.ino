
//----------------------------
char *readSound(char result[], size_t result_sz) {

  const int sampleWindow = 150; // Sample window width in mS (50 mS = 20Hz)
  unsigned int sample;
  double scale = 10;

  unsigned long startMillis = millis(); // Start of sample window
  unsigned int peakToPeak = 0;   // peak-to-peak level

  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;

  // collect data
  while (millis() - startMillis < sampleWindow)
  {
    sample = analogRead(0);
    if (sample < 1024)  // toss out spurious readings
    {
      if (sample > signalMax)
      {
        signalMax = sample;  // save just the max levels
      }
      else if (sample < signalMin)
      {
        signalMin = sample;  // save just the min levels
      }
    }
  }
  peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
  double level = (peakToPeak * scale);  // convert to volts;

  char buf01[100];
  dtostrf(level, 1, 2, buf01);

  strcpy(result, "SOUND, ");
  strcat(result, buf01);

  return result;
}


//----------------------------
char *readPressure(char result[], size_t result_sz) {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float w = 154.25;
  // Read temperature as Celsius (the default)
  float e = 197.24;
  char buf01[100], buf02[100];
  dtostrf(w, 1, 2, buf01);
  dtostrf(e, 1, 2, buf02);

  strcpy(result, "PRESSURE, ");
  strcat(result, buf01);

  return result;
}


//----------------------------
char *readLight(char result[], size_t result_sz) {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  char buf01[100];

  float sensorValue = 0;
  // print out the value you read:

  for (int i = 0; i < lightReadingSamples; i++) {

    sensorValue = + analogRead(PHOTOPIN);
    delay(lightReadingDelay);
  }

  sensorValue = sensorValue / lightReadingSamples;

  dtostrf(sensorValue, 1, 2, buf01);

  strcpy(result, "LIGHT, ");
  strcat(result, buf01);
  
  return result;
}


//----------------------------
char *readHumidity(char result[], size_t result_sz) {

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) ) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  char buf01[6];
  dtostrf(h, 1, 2, buf01);

  strcpy(result, "HUMIDITY, ");
  strcat(result, buf01);
  strcpy(buf01, "");

  return result;

}

//----------------------------
char *readTemperature(char result[], size_t result_sz) {

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(t) ) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  char buf01[6];
  dtostrf(t, 1, 2, buf01);

  strcpy(result, "TEMPERATURE, ");
  strcat(result, buf01);
  strcpy(buf01, "");
  
  return result;

}

//----------------------------
char *readGases(char result[], size_t result_sz) {
 
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  char buf01[20];

  float sensorValue = 0;
  // print out the value you read:

  for (int i = 0; i < gasesReadingSamples; i++) {

    sensorValue = + analogRead(TGS2602PIN);
    delay(gasesReadingDelay);
  }

  sensorValue = sensorValue / gasesReadingSamples;

  dtostrf(sensorValue, 1, 2, buf01);

  strcpy(result, "GAS, ");
  strcat(result, buf01);
  strcpy(buf01, "");
  return result;
}


