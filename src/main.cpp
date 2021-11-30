/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp8266-nodemcu-async-web-server-espasyncwebserver-library/
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

// Import required libraries
#include <Wifi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AccelStepper.h>

#include <credentials.h>
#include <html.h>
#include <config.h>

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// // Define a stepper and the pins it will use
AccelStepper stepper(AccelStepper::DRIVER, 2, 4);

void moveToPct(const int pct)
{
  stepper.moveTo(getPctOpenPosition(pct));
}

// Replaces placeholder with button section in your web page
String variableProcessor(const String &var)
{
  //Serial.println(var);
  if (var == "CURRENT_POSITION")
  {
    Serial.println("currentPosition: " + String(stepper.currentPosition()));
    return String(stepper.currentPosition());
  }
  else if (var == "OFFSET")
  {
    return String(offset);
  }
  else if (var == "STEPS_PER_ROTATION")
  {
    return String(STEPS_PER_ROTATION);
  }
  else if (var == "ROTATIONS_TO_CLOSE")
  {
    return String(ROTATIONS_TO_CLOSE);
  }
  else if (var == "STEPS_TO_CLOSE")
  {
    return String(STEPS_TO_CLOSE);
  }
  else if (var == "CLOCKWISE_CLOSE")
  {
    return String(CLOCKWISE_CLOSE);
  }
  else if (var == "CURRENT_PCT_OPEN")
  {
    return String(getCurrentPctOpen(stepper.currentPosition()));
  }
  // else if (var == "OPEN_OR_CLOSED")
  // {
  //   return getOpenOrClosed();
  // }
  // else if (var == "ONE_ROTATION")
  // {
  //   return String(oneRotation);
  // }
  return String();
}

void configureServer()
{
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              Serial.println("GET " + request->url());
              request->send_P(200, "text/html", index_html, variableProcessor);
            });

  server.on("/api/position", HTTP_PUT, [](AsyncWebServerRequest *request)
            {
              Serial.println("PUT " + request->url());
              if (request->hasParam("pct"))
              {
                String positionValue = request->getParam("pct")->value();
                Serial.println("pct=" + positionValue);

                int pct = positionValue.toInt();
                moveToPct(pct);
                Serial.println("Moving to " + String(pct) + "%");
              }
              else if (request->hasParam("steps"))
              {
                String positionValue = request->getParam("steps")->value();
                Serial.println("steps=" + positionValue);
              }
              request->send(200, "text/plain", "OK");
            });

  server.on("/api/zero", HTTP_PUT, [](AsyncWebServerRequest *request)
            {
              Serial.println("PUT " + request->url());
              if (request->hasParam("position"))
              {
                String positionValue = request->getParam("position")->value();
                boolean isOpen = positionValue != "closed";
                updateOffset(isOpen, stepper.currentPosition());
              }
              request->send(200, "text/plain", "OK");
            });

  server.on("/api/step", HTTP_PUT, [](AsyncWebServerRequest *request)
            {
              Serial.println("PUT " + request->url());
              if (request->hasParam("distance"))
              {
                String distanceValue = request->getParam("distance")->value();
                stepper.move((CLOCKWISE_CLOSE ? 1 : -1) * distanceValue.toInt());
              }
              request->send(200, "text/plain", "OK");
            });

  server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              Serial.println("PUT " + request->url());
              int pctOpen = getCurrentPctOpen(stepper.currentPosition());
              request->send(200, "text/plain", String(pctOpen));
            });
}

void setup()
{
  // Serial port for debugging purposes
  Serial.begin(115200);
  Serial.println("Starting Up");

  // Connect to Wi-Fi
  Serial.print("Connecting to network ");
  Serial.print(wifi_ssid);
  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  stepper.setPinsInverted(false, false, false, false, true);
  stepper.setEnablePin(5);
  stepper.setMaxSpeed(MAX_SPEED);
  stepper.setAcceleration(ACCELERATION);

  configureServer();

  // Start server
  server.begin();
}

bool stepperEnabled = true;
long timeDisabled = millis();

void loop()
{
  // run the stepper
  stepper.run();

  // if (stepper.distanceToGo() > 0)
  // {
  //   Serial.println("distanceToGo: " + stepper.distanceToGo());
  // }

  if (stepper.distanceToGo() != 0)
  // if stepper needs to move
  {
    if (!stepperEnabled)
    // if stepper is currently disabled
    {
      Serial.println("enabling outputs");
      Serial.println("distance to go: " + String(stepper.distanceToGo()));
      // Enable stepper
      stepper.enableOutputs();
      stepperEnabled = true;
    }
  }
  else
  // else if stepper is done moving
  {
    if (stepperEnabled)
    // if stepper is currently enabled
    {
      Serial.println("disabling outputs");
      Serial.println("distance to go: " + String(stepper.distanceToGo()));
      stepperEnabled = false;
      // set disable time to 2 seconds in the
      timeDisabled = millis() + 2000;
    }
    else
    {
      if (timeDisabled < millis())
      {
        // Disable stepper
        stepper.disableOutputs();
      }
    }
  }
}