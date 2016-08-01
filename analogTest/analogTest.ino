int sensorPin = A0;    // select the input pin for the potentiometer
int sensorValue = 0;  // variable to store the value coming from the sensor

void setup() {
  // put your setup code here, to run once:
  //pinMode(0,INPUT);
Serial.begin(115200);
  Serial.print("\n");
}

void loop() {
  // put your main code here, to run repeatedly:
Serial.println(analogRead(sensorValue));
}
