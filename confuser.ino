#include <Servo.h>

// Sharp IR distance sensors work fine here.
int front_sensor = 18; /* Analog pin 4 */
int rear_sensor = 19; /* Analog pin 5 */

// Parallax continuous rotation servos.
int left_wheel_pin = 9;
int right_wheel_pin = 10;

Servo left_wheel;
Servo right_wheel;

/* In principle anything from 0-1023.  In practice, under 100 means
there is probably nothing in there, > 600 is very close. */

const int SENSOR_MAX = 600; 
const int SENSOR_THRESHOLD = 100;

void setup()
{
  // initialize the serial communications:
  Serial.begin(9600);

  pinMode(front_sensor, INPUT);  
  pinMode(rear_sensor, INPUT);  

  left_wheel.attach(left_wheel_pin);
  right_wheel.attach(right_wheel_pin);
}

void ccw(Servo wheel, float speed)
{
  wheel.writeMicroseconds(1500 + 300 * speed);
}

void cw(Servo wheel, float speed)
{
  wheel.writeMicroseconds(1500 - 300 * speed);
}

void stop_servo(Servo wheel)
{
  wheel.writeMicroseconds(1500);
}

void stop()
{
  stop_servo(left_wheel);
  stop_servo(right_wheel);
}

void forward(float speed)
{
  Serial.print("forward ");
  Serial.print(speed);
  Serial.println();
  ccw(left_wheel, speed);
  cw(right_wheel, speed);
}

void backward(float speed)
{
  Serial.print("backward ");
  Serial.print(speed);
  Serial.println();
  cw(left_wheel, speed);
  ccw(right_wheel, speed);
}

void left(float speed)
{
  Serial.print("left");
  Serial.println();
  cw(right_wheel, speed);
  cw(left_wheel, speed);
}

void right(float speed)
{
  Serial.print("right");
  Serial.println();
  ccw(right_wheel, speed);
  ccw(left_wheel, speed);
}

float normalize_speed(float speed)
{
  if (speed < 0) {
    return 0;
  }

  if (speed > 1) {
    return 1;
  }

  return speed;
}

int get_turn_angle(float forward_speed, float backward_speed)
{
  if (forward_speed < 0.2 || backward_speed < 0.2) {
    // We do not need to turn.
    if (random(100) < 3) {
      // Turn a little bit at random every once in a while.
      return random(40) - 20;
    }
    return 0;
  }

  int angle = random(45) + 45;
  if (random(2) < 1) {
    return angle;
  } else {
    return -angle;
  }
}

void turn(int turn_angle)
{
  Serial.print("Turning ");
  Serial.print(turn_angle);
  Serial.print(" degrees");
  Serial.println();

  if (turn_angle > 0) {
    right(0.5);
  } else {
    left(0.5);
  }
  delay(6 * abs(turn_angle));
}

void loop()
{
  int front_value = analogRead(front_sensor);
  int rear_value = analogRead(rear_sensor);
  
  Serial.print(front_value);
  Serial.print("\t");
  Serial.print(rear_value);
  Serial.println();

  // What this does:
  // 
  // if something in front, back up.
  // if something in back, go forward.
  // if something in both sides, turn and move for a short time.

  // Scale the sensor values to speed.
  float backward_speed = (float)(front_value - SENSOR_THRESHOLD) / 
  (SENSOR_MAX - SENSOR_THRESHOLD);
  float forward_speed = (float)(rear_value - SENSOR_THRESHOLD) / 
  (SENSOR_MAX - SENSOR_THRESHOLD);

  forward_speed = normalize_speed(forward_speed);
  backward_speed = normalize_speed(backward_speed);

  float turn_angle = get_turn_angle(forward_speed, backward_speed);

  if (turn_angle != 0) {
    turn(turn_angle);
    forward(1);
  } else {
    float speed = normalize_speed(forward_speed) - 
       normalize_speed(backward_speed);
    Serial.print("Speed ");
    Serial.print(speed);
    Serial.println();
    
    if (speed < 0) {
      backward(-speed);
    } else {
      forward(speed);
    }
  }
  delay(200);
}
