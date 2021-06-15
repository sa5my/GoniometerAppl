/*#include <avr/interrupt.h>
  #include <avr/io.h>*/

#define dirPin_Rot 9
#define stepPin_Rot 8
#define dirPin_Gonio 7
#define stepPin_Gonio 6

#define AXIS 0
#define CMD 1
#define DIR 2   /*Payload Data - Direction*/

/* Terminate frame*/
#define TERMINATE 'Z'

#define ROT_AXIS    'A'
#define ANGLE_AXIS  'B'

#define JOG_MODE     'C'
#define INCREMENTAL  'D'
#define MANUAL_MOVE  'E'
#define STOP         'F'

/* Direction*/
#define CW        'G'   /* Motor Direction - Clockwise*/
#define CCW       'H'  /* Motor Direction - Counter Clockwise*/

#define ACK       'Y'

extern const unsigned long MtrSpdRot_Table[];
extern const unsigned long MtrSpdGonio_Table[];

unsigned long deg;

signed long pos;
unsigned char rx_data[16]= {0}, lastDirState;
volatile unsigned int move_flag;
boolean newData = false;
volatile int j = 0;
unsigned int spd, quot, rem;
char speed_arr[5], quot_arr[5], rem_arr[5];
int acq_speed, acq_rem, acq_quot;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(dirPin_Rot, OUTPUT);
  pinMode(stepPin_Rot, OUTPUT);
  pinMode(dirPin_Gonio, OUTPUT);
  pinMode(stepPin_Gonio, OUTPUT);
  digitalWrite(dirPin_Gonio, HIGH);
  pinMode(13, OUTPUT);
}

int i = 0;

void loop() {
#if 0
  static byte i = 0;
  int endMarker = TERMINATE;
  String rc = "";
  unsigned long indata = 0;
  while ((Serial.available() > 0) && (newData == false))
  {
    rc = "";
    rc = Serial.read();
    /*Serial1.print("Received:");*/
    /*Serial1.println(rc.toInt());*/
    indata = rc.toInt();
    if ((indata != '\n') && (indata != '\r') && (indata != '\0'))
    {
      if ((indata != endMarker))
      {
        rx_data[i] = indata;
        i++;
        spd = rx_data[3]
              Serial1.println(atoi(&rx_data[3]));
      }
      else
      {
        i = 0;
        newData = true;
      }
    }
    else
    {

    }
  }
#endif
  PC_Read();
  /* Serial1.println(rx_data[CMD]);*/
  switch (rx_data[CMD])
  {
    case JOG_MODE:
      if ((newData == true) || (move_flag == 1))
      {
        Move_Motor(rx_data[DIR], 1);
        move_flag = 1;
      }
      break;

    case INCREMENTAL:
      if ((newData == true))
      {
        Move_Motor(rx_data[DIR], 1);
      }
      break;

    case MANUAL_MOVE:
      if ((newData == true))
      {
        deg = (quot * 256) + (rem);
        Serial1.print("Degrees - ");
        Serial1.println(deg);
        Move_Motor(rx_data[DIR], deg);
        Serial.println(ACK);
        newData =false;
      }
      break;

    case STOP:
      if (newData == true) {
        /*Serial.println("Motor stop");*/
        move_flag = 0;
      }
      else
      {
        /*Serial.println("Reaching here");*/
        /* Do Nothing*/
      }
      newData = false;
      break;

    default:
      break;
  }
}

void Move_Motor(const int mtr_dir, int degree)
{
  int steps;
  steps = 0;
  if (rx_data[AXIS] == ROT_AXIS)
  {
    steps = 870;
  }
  else if (rx_data[AXIS] == ANGLE_AXIS)
  {
    steps = 6400;
  }
  newData = false;
  if (lastDirState != mtr_dir)
  {
    if (mtr_dir == CW)
    {
      if (rx_data[AXIS] == ROT_AXIS)
      {
        /*PORTA |= 0x01;*/  /* Dir Pin clockwise */
        digitalWrite(dirPin_Rot, HIGH);
      }
      else if (rx_data[AXIS] == ANGLE_AXIS)
      {
        digitalWrite(dirPin_Gonio, HIGH);
      }
    }
    else if (mtr_dir == CCW)
    {
      if (rx_data[AXIS] == ROT_AXIS)
      {
        /*PORTA |= 0x01;*/  /* Dir Pin clockwise */
        digitalWrite(dirPin_Rot, LOW);
      }
      else if (rx_data[AXIS] == ANGLE_AXIS)
      {
        digitalWrite(dirPin_Gonio, LOW);
      }
    }
  }
  lastDirState = mtr_dir;

  if (rx_data[AXIS] == ROT_AXIS)
  {
    for (int i = 0; i < (degree); i++)
    {
      for (int i = 0; i < steps; i++) /* 870 steps to move 1 degree */
      {
        /*PORTA |= 0x02; */     /* stepPin HIGH*/
        digitalWrite(stepPin_Rot, HIGH);
        delayMicroseconds(MtrSpdRot_Table[spd]);
        /*PORTA &= ~0x02;*/    /* stepPin LOW*/
        digitalWrite(stepPin_Rot, LOW);
        delayMicroseconds(MtrSpdRot_Table[spd]);
      }
    }
  }
  else if (rx_data[AXIS] == ANGLE_AXIS)
  {
    for (int i = 0; i < (degree); i++)
    {
      for (int i = 0; i < steps; i++) /* 870 steps to move 1 degree */
      {
        /*PORTA |= 0x02; */     /* stepPin HIGH*/
        digitalWrite(stepPin_Gonio, HIGH);
        delayMicroseconds(MtrSpdGonio_Table[spd]);
        /*PORTA &= ~0x02;*/    /* stepPin LOW*/
        digitalWrite(stepPin_Gonio, LOW);
        delayMicroseconds(MtrSpdGonio_Table[spd]);
        /*Serial.println("Moving Motor1");*/
      }
    }
    /*Serial1.println("Moving Motor2");*/
  }
  else
  {
  }
}

void PC_Read()
{
  static byte i = 0;
  String pc_strng = "";
  char indata = 0;
  while ((Serial.available() > 0) && (newData == false))
  {
    pc_strng = "";
    pc_strng = Serial.read();
    /*indata = pc_strng;*/
    /*Serial2.write(pc_strng.toInt());*/
    indata = pc_strng.toInt();
    Serial1.print(indata);
    if (indata != TERMINATE)
    {
      if ((indata != '\n') &&  (indata != '\r'))
      {
        if (indata != 'X')
        {
          if (acq_speed == 1)
          {
            speed_arr[j] = indata;
            j++;
          }
          else if (acq_quot == 1)
          {
            quot_arr[j] = indata;
            j++;
          }
          else if (acq_rem == 1)
          {
            rem_arr[j] = indata;
            j++;
          }
          else
          {
            /*Do Nothing*/
          }
        }
        else
        {
          if (acq_speed == 1)
          {
            spd = atoi(speed_arr);
            Serial1.print("Speed -");
            Serial1.println(spd);
          }
          else if (acq_quot == 1)
          {
            quot = atoi(quot_arr);
            Serial1.print("Quotient -");
            Serial1.println(quot);
          }
          else if (acq_rem == 1)
          {
            rem = atoi(rem_arr);
            Serial1.print("Remainder-");
            Serial1.println(rem);
          }
          else
          {
            /*Do Nothing*/
          }
        }

        switch (indata)
        {
          case 'S':
            acq_speed = 1;

            break;

          case 'R':
            acq_rem = 1;

            break;

          case 'Q':
            acq_quot = 1;

            break;

          case 'X':
            j = 0;
            for (int k = 0; k < 5; k++)
            {
              speed_arr[k] = 0;
              quot_arr[k] = 0;
              rem_arr[k] = 0;
            }
            acq_speed = 0;
            acq_rem = 0;
            acq_quot = 0;

          default:
            rx_data[i] = indata;
           /* Serial1.print("i :");
            Serial1.print(i);
            Serial1.print(" - ");
            Serial1.write(rx_data[i]);
            Serial1.println();*/
            i++;
            break;
        }
      }
    }
    else
    {
      i = 0;
      newData = true;
    }
  }
}
