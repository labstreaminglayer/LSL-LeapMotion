# LSL LeapMotion in Python

This uses the Leap Motion cffi-based Python API that comes with the Hyperion and Gemini SDKs and the [leapc-python-bindings](https://github.com/ultraleap/leapc-python-bindings) that wraps it.

## Getting Started

Before getting started, you'll need a Leap Motion (tested with model 2), [`uv`](https://docs.astral.sh/uv/getting-started/installation/), and the Leap Motion SDK installed.

1. Clone the repository: `git clone https://github.com/labstreaminglayer/LSL-LeapMotion.git`
2. Change to the python directory: `cd LSL-LeapMotion/python`
4. Run the script: `uv run leap_lsl.py`
    * The first run might take a moment do install the virtual env and dependencies. 
