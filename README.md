# LSL-LeapMotion

Stream Leap Motion data over LSL

## How to Build
### Dependencies
#### liblsl

Download the appropriate liblsl archive from the [liblsl GitHub release page](https://github.com/sccn/liblsl/releases). After extracting/installing the archive, you may have to provide its path to cmake: 
`-DLSL_INSTALL_ROOT={path/to/lsl/install}`

#### LeapSDK

Download the [tracking software](https://developer.leapmotion.com/tracking-software-download) for your platform.
You will have to provide the path to the extracted `LeapSDK` folder to cmake with the argument `-DLeap_ROOT_DIR={path}`

## How to Use

The application creates

### Linux

The leapd daemon must be running. If you do not have this setup in your `/etc/init.d` then you can run it on demand with `sudo leapd`.

Check its operation by running the diagnostic visualizer inside `LeapControlPanel --showsettings`
