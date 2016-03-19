/*
  Ben's test for the sound sensor.
 */

int delayTime = 0;
int counter = 0;
int cols = 0;
int maxCols = 50;
int countPrint = 4000;

int soundVal = 1;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 13 as an output.
  pinMode(13, OUTPUT);
  pinMode(2, INPUT);
  Serial.begin(9600);

  delayTime = 50;
}

// the loop function runs over and over again forever
void loop() {
  counter++;

  if (digitalRead(2) == 0) soundVal = 0;
  
  if (counter >= countPrint) {
    Serial.print(soundVal);
    Serial.print(' ');
    cols++;
    counter = 0;
    soundVal = 1;
  }
  
  if (cols >= maxCols) {
    Serial.print("\n");
    cols = 0;
  }
}
