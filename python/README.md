# LSL LeapMotion in Python

This uses the Leap Motion cffi-based Python API that comes with the Hyperion and Gemini SDKs and the [leapc-python-bindings](https://github.com/ultraleap/leapc-python-bindings) that wraps it.

## Getting Started

Before getting started, you'll need a Leap Motion (tested with model 2), [`uv`](https://docs.astral.sh/uv/getting-started/installation/), and the Leap Motion SDK installed.

1. Clone the repository: `git clone https://github.com/labstreaminglayer/LSL-LeapMotion.git`
2. Change to the python directory: `cd LSL-LeapMotion/python`
3. Run the script: `uv run leap_lsl.py`
    * The first run might take a moment do install the virtual env and dependencies.
    * Add `--quat` to the command to include quaternions for each joint which increasees the channel count from 63 to 147

## Troubleshooting

> Missing required files, Cannot import leapc_cffi, or Unable to find leapc_cffi
 
You do not have the correct UltraLeap SDK installed. This script was tested with the Hyperion SDK. If you cannot install the SDK but you can download and access the files, you can set a `LEAPSDK_INSTALL_LOCATION` environment variable to the path of the "LeapSDK" directory before running this script.
