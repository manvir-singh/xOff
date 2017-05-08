#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>

typedef enum {
	DLL_LOAD,
	GET_FUNC_POWER_OFF,
	GET_FUNC_GET_STATE,
	DEVICE_NOT_CONNECTED
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


typedef int (*POWER_OFF_T)(int);
typedef DWORD (*XInputGetState_T)(DWORD, XINPUT_GAMEPAD_SECRET*);
HANDLE dll;

void on_exit() {
	if (dll) FreeLibrary(dll);
}

void err_and_exit(HANDLE dll, FAIL_CODES code) {
	switch(code) {
		case DEVICE_NOT_CONNECTED:
			printf("No controller found!\n");
			break;
		case GET_FUNC_GET_STATE:
			printf("Unable to find \"XInputGetState\" function!\n");
			break;
		case GET_FUNC_POWER_OFF:
			printf("Unable to find the poweroff function!\n");
			break;
		case DLL_LOAD:
			printf("Unable to load Xinput dll!\n");
			break;
		default:
			printf("Unknown error! :(\n");
	}
	exit(-1);
}

int main() {
	atexit(on_exit);
	dll = LoadLibrary("XInput1_3");
	if (!dll) err_and_exit(dll, DLL_LOAD);

	POWER_OFF_T power_off = (POWER_OFF_T) GetProcAddress(dll, MAKEINTRESOURCE(103));
	// This is a special hidden function for getting the guide button
	XInputGetState_T xinput_get_state = (XInputGetState_T) GetProcAddress(dll, MAKEINTRESOURCE(100));
	if (!power_off) err_and_exit(dll, GET_FUNC_POWER_OFF);
	if (!xinput_get_state) err_and_exit(dll, GET_FUNC_GET_STATE);

	XINPUT_GAMEPAD_SECRET x_state = {0};
	int wasPressed = 0;
	while(1) {
		if (xinput_get_state(0, &x_state) == ERROR_DEVICE_NOT_CONNECTED) {
			Sleep(1);
			continue;
		}
		if (x_state.wButtons == 0x00400) {
			if (wasPressed) {
				wasPressed = 0;
				power_off(0);
				memset(&x_state, 0, sizeof(XINPUT_GAMEPAD_SECRET));
			} else {
				wasPressed = 1;
			}
		}
		Sleep(2000);
	}
	return 0;
}
