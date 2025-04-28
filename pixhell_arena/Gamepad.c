#include "Gamepad.h"

// This define makes your program compile faster by excluding things we are not using
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
#include <XInput.h>
#pragma comment(lib,"xinput.lib")
#else
#include <XInput.h>
#pragma comment(lib,"xinput9_1_0.lib")
#endif

sfBool isConnected(t_joyNum joyNum)
{
    XINPUT_STATE state;
    ZeroMemory(&state, sizeof(XINPUT_STATE));

    auto result = XInputGetState(joyNum, &state);
    return (result == ERROR_SUCCESS);
}

unsigned int getButtonCount(t_joyNum joyNum)
{
    return 14;
}

sfBool isButtonPressed(t_joyNum joyNum, t_buttonNum buttonNum)
{
    XINPUT_STATE state;
    ZeroMemory(&state, sizeof(XINPUT_STATE));

    XInputGetState(joyNum, &state);
    return  (state.Gamepad.wButtons & buttonNum);
}

sfBool isAnyXBox360ControllerConnected()
{
    return  (isConnected(0) || isConnected(1));
}

sfBool voiceSupported(t_joyNum joyNum)
{
    XINPUT_CAPABILITIES caps;
    ZeroMemory(&caps, sizeof(XINPUT_CAPABILITIES));

    auto result = XInputGetCapabilities(joyNum, XINPUT_FLAG_GAMEPAD, &caps);

    if (result != ERROR_SUCCESS)
        return sfFalse;

    return  (caps.Flags & XINPUT_CAPS_VOICE_SUPPORTED);
}

void getTriggers(t_joyNum joyNum, float* left, float* right)
{
    XINPUT_STATE state;
    ZeroMemory(&state, sizeof(XINPUT_STATE));

    XInputGetState(joyNum, &state);

    *left = (float)(state.Gamepad.bLeftTrigger) / 255;
    *right = (float)(state.Gamepad.bRightTrigger) / 255;
}

void getSticksPosition(t_joyNum joyNum, sfVector2f* left, sfVector2f* right)
{
    XINPUT_STATE state;
    ZeroMemory(&state, sizeof(XINPUT_STATE));

    XInputGetState(joyNum, &state);

    if ((state.Gamepad.sThumbLX < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
        state.Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) &&
        (state.Gamepad.sThumbLY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
            state.Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)) {

        state.Gamepad.sThumbLX = 0;
        state.Gamepad.sThumbLY = 0;

    }

    if ((state.Gamepad.sThumbRX < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
        state.Gamepad.sThumbRX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) &&
        (state.Gamepad.sThumbRY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
            state.Gamepad.sThumbRY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)) {

        state.Gamepad.sThumbRX = 0;
        state.Gamepad.sThumbRY = 0;

    }

    left->x = (float)(state.Gamepad.sThumbLX / 327);
    left->y = (float)(state.Gamepad.sThumbLY / 327);
    right->x = (float)(state.Gamepad.sThumbRX / 327);
    right->y = (float)(state.Gamepad.sThumbRY / 327);
}

void setVibration(t_joyNum joyNum, float leftMotor, float rightMotor)
{
    XINPUT_VIBRATION vib;
    ZeroMemory(&vib, sizeof(XINPUT_VIBRATION));

    vib.wLeftMotorSpeed = (unsigned short)(leftMotor * 65535.0f);
    vib.wRightMotorSpeed = (unsigned short)(rightMotor * 65535.0f);

    XInputSetState(joyNum, &vib);
}
