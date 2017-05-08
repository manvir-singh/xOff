# xOff
This utility allows you to power off the controller by holding down the guide[1] button for ~2 seconds.

[1] Guide button: The button with the xbox logo on it, found in the center of the controller.

# Install
1) Download the binary from https://github.com/Programming4life/xOff/releases, or build it yourself `make release`. You'll need msys2 intalled in order to build it.

2) Place it into the windows startup folder located at `C:\Users\<user name>\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup\`

# Note
You may want to disable steam big picture mode or else it'll open up everytime you press the guide button.
1) Close steam completely, right click the tray icon -> exit
2) Open: `C:\Program Files (x86)\Steam\config\config.vdf`
3) Find `"Controller_CheckGuideButton"		"1"`
4) Change the `"1"` to `"0"`
