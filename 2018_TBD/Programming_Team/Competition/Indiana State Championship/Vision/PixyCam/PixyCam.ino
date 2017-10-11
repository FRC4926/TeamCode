 /*
  Serial Call and Response
 Language: Wiring/Arduino
 http://www.arduino.cc/en/Tutorial/SerialCallResponse

 FRC4926
 Working Code:  Sends out x-y coordinates of midpoints 
                of detected GEAR vision-target strips.

 Usage: -3= no blocks detected
        -4= fake block detected
        -1 for H2 and W2= one block detected
 */
#include <Wire.h>
#include <PixyI2C.h>

//PixyI2C pixy;
PixyI2C pixy(0x54); // Setting the I2C address through PixyI2C object 

//int inByte = 0;         // incoming serial byte
double midpoint_x,midpoint_y,temph1,temph2,h_diff;//setting variables
int width1;
int height1;
int width2;
int height2;
bool serialReq = false;
int t = 0; //if we detect no blocks for 50 iterations in a row, we print -3s for all the values (see loops below)
int p = 0; //if we detect no blocks for 10 iterations in a row, we print -1 for all the 2nd block values (see loops below)
//int target_flag = 0;//SlateTiger wrong scope; sets default
double aspect_ratioj,aspect_ratiok;
int first_time, atleast_one,correct_distance_blocks;

void setup() {
  // start serial port at 9600 bps:
  Serial.begin(9600);
  while (!Serial) {
    ; //wait for serial port to connect. Needed for native USB port only
  }
  ///Serial.print("Starting...\n");
  pixy.init();//initializing pixy
}

void loop() {//creating a loop
  //making more variables
  static int i = 0; 
  int j; //the first block
  uint16_t blocks; //number of blocks detected
  char buf[32]; 

// FRC PatternDetection vars
  int k; //the second block
  int distance; //distance between the two blocks
  int target_flag = 0;

  blocks = pixy.getBlocks();//looks for targets on Pixy

  atleast_one = 0;
  first_time = 1;
  if (blocks)//begins loop if there are blocks
  {
    t = 0; //every time blocks are detected, the t count is reset to 0
    i++; //add 1 to iteration number every time loop executes
    if (blocks>1)//if the number of blocks detected is greater than 1
    {
      target_flag = 0; //FlagFlag
      p = 0; //resets the p value to zero after detecting more than 1 block
      correct_distance_blocks=0;
      for (j=0; j<blocks; j++) 
      {
          aspect_ratioj = double(pixy.blocks[j].height)/double(pixy.blocks[j].width);
          if ((aspect_ratioj>2)&&(aspect_ratioj<3))
          {
            if(first_time ==1)      // check only the first time to see if atlast one block is correct aspect ratio
            {
              atleast_one = 1;
              first_time = 0;
            }
            for (k=j+1; k<blocks; k++) 
            {
              temph1 = double(pixy.blocks[j].height); //Sets the integer pixy.blocks[j].height (height of the first block) to a double (a decimal)
              temph2 = double(pixy.blocks[k].height); //Sets the integer pixy.blocks[k].height (height of the second block) to a double (a decimal)

              distance=(pixy.blocks[j].x-pixy.blocks[k].x); //distance between two blocks is x-value of first - x-value of second
              distance=abs(distance);
              if ( (distance < 5*pixy.blocks[k].width) && (distance > 3*pixy.blocks[k].width) ) //if the blocks' heights are within 20 percent of each other
              { 
                 h_diff = (double((abs(temph1-temph2))/temph1)); //variable h_diff is height difference between two blocks
                //Serial.println("Test Distance");Serial.print(distance);
//                if (distance<0) distance = -1*distance; //if the distance is less than 0 multiply it by -1 to get it to be positive
                 if(h_diff<0.2) //if distance between blocks is 3-5x the width of block
                 { 
                   aspect_ratiok = double(pixy.blocks[k].height)/double(pixy.blocks[k].width);
                   if( (aspect_ratiok>2)&&(aspect_ratiok<3) )
                   {
                       midpoint_x = (pixy.blocks[j].x + pixy.blocks[k].x)/2; //finds midpoint of x values of blocks
                       midpoint_y = (pixy.blocks[j].y + pixy.blocks[k].y)/2; //finds midpoint of y values of blocks
                       width1   = pixy.blocks[j].width; //variables
                       height1  = pixy.blocks[j].height;
                       width2   = pixy.blocks[k].width;
                       height2  = pixy.blocks[k].height;
                        //Serial.println("TargetDetected_ 2 good blocks");
                        Serial.print("B:"); //Printing the number of blocks detected
                        Serial.print(blocks);
                        Serial.print(",X:");
                        Serial.print(midpoint_x);
                        Serial.print(",Y:");
                        Serial.print(midpoint_y);
                        Serial.print(",W1:");
                        Serial.print(width1);
                        Serial.print(",H1:");
                        Serial.print(height1);
                        Serial.print(",W2:");
                        Serial.print(width2);
                        Serial.print(",H2:");
                        Serial.print(height2);
                        Serial.println("");
                        target_flag = 1; //FlagFlag
                   }
                 }
                 else
                 {
                   correct_distance_blocks = blo
                 }
              }
           }
         }
      }

      if(target_flag != 1){ //FlagFlag
        if (atleast_one ==1)
        {
          //Serial.println("TargetNotDetected_ 1 good block 1 noisy");
          Serial.print("B:");
          Serial.print("1");
          Serial.print(",X:");
          Serial.print(pixy.blocks[0].x);
          Serial.print(",Y:");
          Serial.print(pixy.blocks[0].y);
          Serial.print(",W1:");
          Serial.print(pixy.blocks[0].width);
          Serial.print(",H1:");
          Serial.print(pixy.blocks[0].height);
          Serial.print(",W2:");
          Serial.print("-5");
          Serial.print(",H2:");
          Serial.print("-5");
          Serial.println(""); 
        }
        else
        {
          //Serial.println("TargetNotDetected_ 2 noisy");
          Serial.print("B:");
          Serial.print("0");
          Serial.print(",X:");
          Serial.print("-3");
          Serial.print(",Y:");
          Serial.print("-3");
          Serial.print(",W1:");
          Serial.print("-3");
          Serial.print(",H1:");
          Serial.print("-3");
          Serial.print(",W2:");
          Serial.print("-3");
          Serial.print(",H2:");
          Serial.print("-2");
          Serial.println("");
        }
      }
    }
    else if(blocks == 1){ //if two or more blocks are not detected, skips over that loop
      p++; //adds to the iterations
      if(p%10 ==0){
        aspect_ratioj = double(pixy.blocks[j].height)/double(pixy.blocks[j].width);
        if ((aspect_ratioj>2)&&(aspect_ratioj<3))
        {
          //Serial.println("TargetNotDetected_ 1 good block 0 noisy");
          Serial.print("B:");
          Serial.print(blocks);
          Serial.print(",X:");
          Serial.print(pixy.blocks[j].x);
          Serial.print(",Y:");
          Serial.print(pixy.blocks[j].y);
          Serial.print(",W1:");
          Serial.print(pixy.blocks[j].width);
          Serial.print(",H1:");
          Serial.print(pixy.blocks[j].height);
          Serial.print(",W2:");
          Serial.print("-1");
          Serial.print(",H2:");
          Serial.print("-1");
          Serial.println("");
        }
        else
        {          
          //Serial.println("TargetNotDetected_ 1 noisy");
          Serial.print("B:");
          Serial.print("0");
          Serial.print(",X:");
          Serial.print("-3");
          Serial.print(",Y:");
          Serial.print("-3");
          Serial.print(",W1:");
          Serial.print("-3");
          Serial.print(",H1:");
          Serial.print("-3");
          Serial.print(",W2:");
          Serial.print("-3");
          Serial.print(",H2:");
          Serial.print("-2");
          Serial.println("");
          
        }
      }
    }
  } 
  
  else {
    t++; //adds to iterations
    if(t%50 == 0){ //every 50 iterations, print
//       Serial.print("No blocks");
                  //Serial.println("No  Blocks");
                  Serial.print("B:"); //this
                  Serial.print(blocks);
                  Serial.print(",X:");
                  Serial.print("-3");
                  Serial.print(",Y:");
                  Serial.print("-3");
                  Serial.print(",W1:");
                  Serial.print("-3");
                  Serial.print(",H1:");
                  Serial.print("-3");
                  Serial.print(",W2:");
                  Serial.print("-3");
                  Serial.print(",H2:");
                  Serial.print("-3");
                  Serial.println("");
    }
   
  }
}

