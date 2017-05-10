#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>

typedef enum {
	DLL_LOAD,
	GET_FUNC_POWER_OFF,
	GET_FUNC_GET_STATE,
	DEVICE_NOT_CONNECTED,
	GET_FUNC_SET_STATE
} FAIL_CODES;

// Hidden struct in xinput dll for getting guide button input
typedef struct {
    DWORD dwPacketNumber;
    WORD wButtons;
    BYTE bLeftTrigger;
    BYTE bRightTrigger;
    SHORT sThumbLX;
    SHORT sThumbLY;
    SHORT sThumbRX;
    SHORT sThumbRY;
} XINPUT_GAMEPAD_SECRET;

typedef struct _XINPUT_VIBRATION {
  WORD wLeftMotorSpeed;
  WORD wRightMotorSpeed;
} XINPUT_VIBRATION, *PXINPUT_VIBRATION;


typedef int (*POWER_OFF_T)(int);
typedef DWORD (*SET_STATE_T)(DWORD, XINPUT_VIBRATION*);
typedef DWORD (*GET_STATE_T)(DWORD, XINPUT_GAMEPAD_SECRET*);
HANDLE dll;

#if DEBUG
	void LOG(char *msg) {
		printf("%s\n", msg);
	}
#else
	void LOG() {}
#endif

void on_exit() {
	if (dll) FreeLibrary(dll);
	LOG("Bye!");
}

void err_and_exit(FAIL_CODES code) {
	switch(code) {
		case DEVICE_NOT_CONNECTED:
			LOG("No controller found!\n");
			break;
		case GET_FUNC_GET_STATE:
			LOG("Unable to find \"XInputGetState\" function!\n");
			break;
		case GET_FUNC_POWER_OFF:
			LOG("Unable to find the poweroff function!\n");
			break;
		case GET_FUNC_SET_STATE:
			LOG("Unable to find setstate function!\n");
			break;
		case DLL_LOAD:
			LOG("Unable to load Xinput dll!\n");
			break;
		default:
			LOG("Unknown error! :(\n");
	}
	exit(-1);
}

int main() {
	LOG("Loading...");
	atexit(on_exit);
	dll = LoadLibrary("XInput1_3");
	if (!dll) err_and_exit(DLL_LOAD);

	POWER_OFF_T power_off = (POWER_OFF_T) GetProcAddress(dll, MAKEINTRESOURCE(103));
	if (!power_off) err_and_exit(GET_FUNC_POWER_OFF);
	SET_STATE_T set_state = (SET_STATE_T) GetProcAddress(dll, "XInputSetState");
	if (!set_state) err_and_exit(GET_FUNC_SET_STATE);
	// This is a special hidden function for getting the guide button
	GET_STATE_T get_state = (GET_STATE_T) GetProcAddress(dll, MAKEINTRESOURCE(100));
	if (!get_state) err_and_exit(GET_FUNC_GET_STATE);

	XINPUT_GAMEPAD_SECRET x_state = {0};
	int wasPressed = 0;
	LOG("Running.");
	while(1) {
		if (get_state(0, &x_state) == ERROR_DEVICE_NOT_CONNECTED) {
			Sleep(1);
			continue;
		}
		LOG("Checking guide button state.");
		if (x_state.wButtons == 0x00400) {
			LOG("Guide button has been pressed.");
			if (wasPressed) {
				LOG("Guide button was pressed before, powering off.");
				wasPressed = 0;
				// Vibrate
				XINPUT_VIBRATION vib_struct = {0, 65535};
				set_state(0, &vib_struct);
				Sleep(500);
				power_off(0);
				x_state = (XINPUT_GAMEPAD_SECRET) {0};
			} else {
				wasPressed = 1;
			}
		}
		Sleep(2000);
	}
	return 0;
}
