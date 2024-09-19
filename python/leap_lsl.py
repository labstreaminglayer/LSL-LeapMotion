import time
import typing

import pylsl
import typer
import leap
import leap.datatypes
import leap.device
import leap.events


class MyListener(leap.Listener):

    digit_names = ["thumb", "index", "middle", "ring", "pinky"]
    bone_names = ["metacarpal", "proximal", "intermediate", "distal"]
    dim_names = ["x", "y", "z"]
    quat_names = ["x", "y", "z", "w"]

    def __init__(self, quat: bool = False):
        self._bquat = quat
        self._outlets: typing.Dict[str, typing.Optional[pylsl.StreamOutlet]] = {"left": None, "right": None}
        n_dim = len(self.dim_names) + (len(self.quat_names) if self._bquat else 0)
        n_ch = n_dim * (1 + len(self.digit_names) * len(self.bone_names))
        self._data: typing.Dict[str, typing.List[float]] = {"left": [0.0] * n_ch, "right": [0.0] * n_ch}

    def __del__(self):
        for hand in ["left", "right"]:
            if hand in self._outlets and self._outlets[hand] is not None:
                del self._outlets[hand]

    def on_connection_event(self, event: leap.events.Event):
        print("Connected")

    def on_device_event(self, event: leap.events.Event):
        try:
            with event.device.open():
                dev_info: leap.device.DeviceInfo = event.device.get_info()
                dev_name = str(dev_info.pid).split(".")[-1]
                dev_ser = str(dev_info.serial)
                for hand in ["left", "right"]:
                    n_dim = len(self.dim_names) + (len(self.quat_names) if self._bquat else 0)
                    # palm and all the bones * x,y,z
                    n_joints = 1 + len(self.digit_names) * len(self.bone_names)
                    n_channels = n_joints * n_dim
                    strm_info = pylsl.stream_info(
                        name=f"{dev_name}-{hand}",
                        type="MoCap",
                        channel_count=n_channels,
                        nominal_srate=pylsl.IRREGULAR_RATE,
                        channel_format=pylsl.cf_float32,
                        source_id="-".join((dev_name, dev_ser, hand)),
                        handle=None
                    )
                    acq = strm_info.desc().append_child("acquisition")
                    acq.append_child_value("manufacturer", "UltraLeap")
                    acq.append_child_value("model", dev_name)
                    acq.append_child_value("serial", dev_ser)
                    chans = strm_info.desc().append_child("channels")
                    for dim_name in self.dim_names:
                        chans.append_child("channel") \
                            .append_child_value("label", f"palm-{dim_name}")
                    if self._bquat:
                        for quat_name in self.quat_names:
                            chans.append_child("channel") \
                                .append_child_value("label", f"palm-rot{quat_name}")
                    for dig_name in self.digit_names:
                        for bone_name in self.bone_names:
                            for dim_name in self.dim_names:
                                chans.append_child("channel") \
                                    .append_child_value("label", f"{dig_name}-{bone_name}-{dim_name}")
                            if self._bquat:
                                for quat_name in self.quat_names:
                                    chans.append_child("channel") \
                                        .append_child_value("label", f"{dig_name}-{bone_name}-rot{quat_name}")
                    self._outlets[hand] = pylsl.StreamOutlet(strm_info)
                    self._data[hand] = [0.0] * n_channels
        except leap.LeapError as e:
            dev_info = event.device.get_info()
            dev_ser = str(dev_info.serial)

        print(f"Found device {dev_ser}")

    def on_tracking_event(self, event: leap.events.Event):
        lsl_time = event.timestamp / 1_000_000  # Leap timestamps are in microseconds using same clock as pylsl!
        for hand in event.hands:
            chan_ix = 0
            hand_name = str(hand.type).split(".")[-1].lower()
            if hand_name not in self._outlets or self._outlets[hand_name] is None:
                continue

            # Add palm data
            self._data[hand_name][chan_ix:chan_ix+3] = list(hand.palm.position)
            chan_ix += 3
            if self._bquat:
                self._data[hand_name][chan_ix:chan_ix+4] = list(hand.palm.orientation)
                chan_ix += 4

            # Add digit data
            for dig_name, digit in zip(self.digit_names, hand.digits):
                for bone_name, bone in zip(self.bone_names, digit.bones):
                    joint = bone.next_joint
                    self._data[hand_name][chan_ix:chan_ix+3] = list(joint)
                    chan_ix += 3
                    if self._bquat:
                        self._data[hand_name][chan_ix:chan_ix + 4] = list(bone.rotation)
                        chan_ix += 4

            self._outlets[hand_name].push_sample(self._data[hand_name], lsl_time)

def main(quat: bool = False):
    my_listener = MyListener(quat=quat)

    connection = leap.Connection()
    connection.add_listener(my_listener)

    running = True

    with connection.open():
        connection.set_tracking_mode(leap.TrackingMode.Desktop)
        try:
            while running:
                time.sleep(1)
        except KeyboardInterrupt:
            running = False
            connection.remove_listener(my_listener)
            del my_listener
            connection.disconnect()


if __name__ == "__main__":
    typer.run(main)
