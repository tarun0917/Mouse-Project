#include <Wire.h>
#include <I2Cdev.h>
#include <MPU6050.h>
#include <Mouse.h>

MPU6050 mpu;
int16_t ax, ay, az, gx, gy, gz; //to get the acceleration and gyroscope values
int vx, vy; //to get the velocity values
int i, j, k; //for the 'for' loop
int count, lcount = 0; //for short and long press, to count the number of left clicks to activate continuous click
int cmill, pmill = 0; //current millisecond, previous millisecond
int values; //for reading values when the sensor is pressed
int flag; // breaking the loop

void setup() {
  Serial.begin(9600); //initialise the Serial Monitor
  Wire.begin();
  mpu.initialize(); //to check if the gyroscope is working or not
  if (!mpu.testConnection())
  {
    while (1);
  }
}

void loop()
{
start: mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  if ((ay <= 32767 && ay >= 15000) || (ay >= -32768 && ay <= -15000))
  {
    for (k = 0;; k++)

    {
      count = 0;
      for (i = 0; i < 35; i++) //to detect whether a short of a long click is done by the user
      {
        values = analogRead(A2);
        if (values > 1022)
        {
          count++;
        }
        mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
        vx = -(gz + 60) / 200;
        vy = (gy - 60) / 200;
        Mouse.move(vx, vy);

        delay(18);

      }
      Serial.println(count);
      if (count <= 10 && count > 4 )  //left click if short tap
      {
        Mouse.click(MOUSE_LEFT);
        Serial.println("L");
        cmill = millis();
        if (cmill - pmill > 2500)
        {
          pmill = cmill;
          lcount = 0;
        }
        else
          lcount++;
  
        if (lcount == 2) //if clicked three times continuously, then the mouse key is continuously pressed
        {
          flag = 0;
          count = 0;
          Mouse.press();
          Serial.println("C");

          do {
            for (i = 0; i < 35; i++)
            {
              values = analogRead(A2);
              if (values > 1020)
              {
                count++;
              }
              mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
              vx = -(gz + 60) / 200;
              vy = (gy - 60) / 200;
              Mouse.move(vx, vy);
              delay(20);
            }
            
                  Serial.println(" ");
                  Serial.println(count);
            // only when the left key is detected, the loop will go back to the intial loop
            if (count <= 10 && count > 4 )
            {
              Mouse.release();
              Serial.println("B");
              break;
            }
          } while (flag == 0);

        }
      }

      else if (count > 10 && count <= 20) //right click if long tap
      {
        Mouse.click(MOUSE_RIGHT);
        Serial.println("R");
      }

      else if (count > 20 && count <= 35)
        goto start;
    }
  }
  delay(500);

}