//  lab2-GPIO
//  How to access GPIO registers from C-code on the Raspberry-Pi
//  Example program. The program uses the code contained 
//  on the website https://elinux.org/RPi_GPIO_Code_Samples#Direct_register_access" provided by Dom and Gert



// Access from ARM Running Linux

#define BCM2708_PERI_BASE        0x20000000
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>  // to mmap function
#include <unistd.h>
#include <stdint.h>
#include <curl/curl.h>

//#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)

int  mem_fd;
void *gpio_map;

// I/O access
volatile unsigned *gpio;


// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y)
#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))

#define GPIO_SET *(gpio+7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR *(gpio+10) // clears bits which are 1 ignores bits which are 0

#define GET_GPIO(g) (*(gpio+13)&(1<<g)) // 0 if LOW, (1<<g) if HIGH

#define GPIO_PULL *(gpio+37) // Pull up/pull down
#define GPIO_PULLCLK0 *(gpio+38) // Pull up/pull down clock

void setup_io();
void sendRestRequest();


int main(int argc, char **argv)
{
	int gLED = 25;  // GPIO 25 - for diode 
	int gSwitch = 23; // GPIO 23 - for switch

	// Set up gpio pointer for direct register access
	setup_io();

	

	/************************************************************************\
	* You are about to change the GPIO settings of your computer.          *
	* Mess this up and it will stop working!                               *
	* It might be a good idea to 'sync' before running this program        *
	* so at least you still have your code changes written to the SD-card! *
	\************************************************************************/
 
	// Set GPIO pin 25 to output
	INP_GPIO(gLED); // must use INP_GPIO before we can use OUT_GPIO
	OUT_GPIO(gLED);

	INP_GPIO(gSwitch); // Set GPIO pin 25 to input
 
	GPIO_PULL = 2; // Enable Pull Up control
	usleep(1);
	GPIO_PULLCLK0 = 0x800000; // Selects GPIO pin 23. GPIO Pin Pull-up/down Enable Clock 0
	usleep(1);
	
	unsigned int val = 200;
	
 
	
    while(val > 0) 
	{
        usleep(200000); //delay for 0.2 seconds
        if(GET_GPIO(gSwitch) != 0) // read pin state (no debounce to make code more readable)
		{
			
		}
        else
		{
            sendRestRequest();
            printf("Button is pressed ... toggling LED\n"); 
			GPIO_SET = 1<<gLED;
			sleep(1);
			GPIO_CLR = 1<<gLED;
			sleep(1);			
        }
        val--;
    }		
	
	
	return 0;

} // main


//
// Set up a memory regions to access GPIO
//
void setup_io()
{
   /* open /dev/mem */
   if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
      printf("can't open /dev/mem \n");
      exit(-1);
   }

   /* mmap GPIO */
   gpio_map = mmap(
      NULL,             //Any adddress in our space will do
      BLOCK_SIZE,       //Map length
      PROT_READ|PROT_WRITE,// Enable reading & writting to mapped memory
      MAP_SHARED,       //Shared with other processes
      mem_fd,           //File to map
      GPIO_BASE         //Offset to GPIO peripheral
   );

   close(mem_fd); //No need to keep mem_fd open after mmap

   if (gpio_map == MAP_FAILED) {
      printf("mmap error %d\n", (int)gpio_map);//errno also set!
      exit(-1);
   }

   // Always use volatile pointer!
   gpio = (volatile unsigned *)gpio_map;


} // setup_io


void sendRestRequest(){
	CURL *curl;
  CURLcode res;
  char url[]= "http://169.254.95.238:8080/hello/world";
//  char postData[] = "{\"hi\":\"there\"}"}
  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url);
    //curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
  }
  printf("%d", res);
	
}
