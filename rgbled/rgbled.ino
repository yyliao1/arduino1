
const byte ROWS = 4; //four rows
const byte COLS = 3; //four columns
byte rowPins[ROWS] = { 3, 13, 12, 14 }; //connect to the row pinouts of the keypad
byte colPins[COLS] = {4, 5, 2}; //connect to the column pinouts of the keypad
void setup() {
  // put your setup code here, to run once:
  for (int b=0; b<3; b=b+1)
       {
  pinMode(colPins[b],OUTPUT);
  digitalWrite(colPins[b],HIGH);
       }
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
for( int a = 0; a < 4; a = a + 1 )
   {
       pinMode(rowPins[a],OUTPUT);
       digitalWrite(rowPins[a],LOW);      
       for (int b=0; b<3; b=b+1)
       {
         
        Serial.printf("cnt: %u\n",colPins[b]);
         Serial.printf("cnt: %u\n", digitalRead(colPins[b]));
          digitalWrite(colPins[b],LOW);
          delay(200);
          digitalWrite(colPins[b],HIGH);
       }
       digitalWrite(rowPins[a],HIGH); 
   }

  delay(100);

}
