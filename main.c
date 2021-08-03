
#include <pspkernel.h>
#include <pspusb.h>

PSP_MODULE_INFO("UsbSlimPwr", 0x1000, 1, 1);
PSP_MAIN_THREAD_ATTR(0);

int main_thread(SceSize args, void *argp)
{
	int connected = 0;
	int state = 0;

	int version = sceKernelDevkitVersion();

	// Need to create a loop to ensure that once the cable is removed
	// and reconnected the charging is restarted
	for(;;)
	{
		// Not very nice, but means that the PSP can still charge
		// after going into USB mode, otherwise the USB module
		// stops USB, guess this will probably be patched out in
		// a future custom firmware, but until then always restart it
		sceUsbStart(PSP_USBBUS_DRIVERNAME, 0, 0);

		state = sceUsbGetState();

		state &= PSP_USB_CABLE_CONNECTED;

		if(connected && (state == 0))
		{
			// Stop the Charging of the PSP otherwise it is not
			// possible to start charging again
			if(version == 0x03060010)
			{
				scePower_driver_90285886(0);
			}
			else
			{
				scePower_driver_191A3848(0);
			}
			connected = 0;
		}
		
		if((connected==0) && state)
		{
			// Start the charging of the PSP, if an app wants to allow
			// charging then below is all that is required to be added
			// to start it
			if(version == 0x03060010)
			{
				scePower_driver_733F973B(1);
			}
			else
			{
				scePower_driver_A09FC577(1);
			}
			connected = 1;
		}
		
		sceKernelDelayThread(500000);
	}
}

int module_start(SceSize args, void *argp)
{
		// Create a thread
	SceUID thid = sceKernelCreateThread("UsbSlimPwr", main_thread, 25, 64*1024, 0, NULL);
	if(thid >= 0)
	{
		sceKernelStartThread(thid, args, argp);
	}

	return 0;
}
