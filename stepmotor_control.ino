#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Stepper.h>

// this motor has 32 steps per revolution
#define STEPS_PER_REV 32
#define GEAR_REDUC 64

const float STEPS_PER_OUT_REV = STEPS_PER_REV * GEAR_REDUC;
// feel free to change the value;
const int stepsRequired = 10;
const int pot = 14;
// keypad variables declaring
const byte rows = 4;
const byte columns = 4;
const char keys[rows][columns] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[rows] = {2, 3, 4, 5};
byte columnPins[columns] = {6, 7, 8, 9};
Keypad keypad(makeKeymap(keys), rowPins, columnPins, rows, columns);
Stepper stepper(stepsRequired, 10, 12, 11, 13);
LiquidCrystal_I2C lcd(0x27, 16, 2);
// arrays for motor numbers
int speed_array[99] = {};
int task_array[99] = {};
int seconds_array[99] = {};
int counter = 0;
String motor_status = "Sleep";
int current_task = 0;
int current_speed = 0;
int current_seconds = 0;
bool isPaused = true;

void create_task() {
  lcd.clear();
  lcd.setCursor(0, 0);
  // setting task number
  task_array[counter] = counter;
  lcd.print("Number is: " + String(counter));
  delay(500);
  lcd.clear();
  // adding speed
  lcd.print("Type speed number: ");
  int task_speed = analogRead(pot);
  lcd.setCursor(0, 1);
  lcd.print(task_speed);
  speed_array[counter] = task_speed;
  delay(500);
  lcd.clear();
  // adding seconds
  lcd.setCursor(0, 0);
  lcd.print("print seconds: ");
  char num1 = keypad.waitForKey();
  char num2 = keypad.waitForKey();
  int final_num = (num1 - '0') * 10 + (num2 - '0');
  lcd.setCursor(0, 1);
  lcd.print(final_num);
  seconds_array[counter] = final_num;
  delay(500);
  counter++;
}

void print_status() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(motor_status);
  lcd.setCursor(8, 0);
  lcd.print("Ntask:" + String(current_task));
  lcd.setCursor(0, 1);
  lcd.print("Speed:" + String(current_speed));
  lcd.setCursor(10, 1);
  lcd.print("Sec:" + String(current_seconds));
  delay(500);
}

void motor_stop() {
  motor_status = "Stop";
  current_task = 0;
  current_speed = 0;
  current_seconds = 0;
}

void motor_pause() {
  motor_status = "Pause";
  keypad.waitForKey();
}

void start_motor() {
  motor_status = "Start";
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Choose task: ");
  char num1 = keypad.waitForKey();
  char num2 = keypad.waitForKey();
  int final_num = (num1 - '0') * 10 + (num2 - '0');
  current_task = task_array[final_num];
  current_speed = speed_array[final_num];
  current_seconds = seconds_array[final_num];
  lcd.setCursor(0, 1);
  lcd.print(final_num);
  delay(500);
}

void setup() {
  pinMode(pot, INPUT);
  lcd.init();
  lcd.backlight();
  print_status();
}

void loop() {
  char action_key = keypad.getKey();
  if (action_key == 'D') {
    create_task();
    print_status();
  }
  if (action_key == 'C') {
    motor_stop();
    print_status();
  }
  if (action_key == 'A') {
    if (isPaused == true) {
      start_motor();
      print_status();
      //delay(current_seconds);
      isPaused = !isPaused;
    } else {
      motor_pause();
      print_status();
      isPaused = !isPaused;
    }
  } if (action_key == '#') {
    print_status();
  }
   if (isPaused == false) {
    stepper.setSpeed(current_speed);
    for (int i = 0; i < current_seconds*10; i++) {
       stepper.step(stepsRequired);
       if (keypad.getState() == HOLD){
         motor_pause();
       }
    }
    motor_stop();
    isPaused = !isPaused;
   }
}
