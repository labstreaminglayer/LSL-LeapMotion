/******************************************************************************\
* Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.               *
* Leap Motion proprietary and confidential. Not for distribution.              *
* Use subject to the terms of the Leap Motion SDK Agreement available at       *
* https://developer.leapmotion.com/sdk_agreement, or another agreement         *
* between Leap Motion and you, your company or other organization.             *
\******************************************************************************/

#include <iostream>
#include <cstring>
#include "Leap.h"
#include "lsl_cpp.h"

class SampleListener : public Leap::Listener {
    public:
        virtual void onInit(const Leap::Controller&);
        virtual void onConnect(const Leap::Controller&);
        virtual void onDisconnect(const Leap::Controller&);
        virtual void onExit(const Leap::Controller&);
        virtual void onFrame(const Leap::Controller&);
        virtual void onFocusGained(const Leap::Controller&);
        virtual void onFocusLost(const Leap::Controller&);
        virtual void onDeviceChange(const Leap::Controller&);
        virtual void onServiceConnect(const Leap::Controller&);
        virtual void onServiceDisconnect(const Leap::Controller&);

    private:
        std::vector<lsl::stream_outlet> outlets;
};

std::array<const std::string, 5> fingerNames = {"Thumb", "Index", "Middle", "Ring", "Pinky"};
std::array<const std::string, 4> boneNames = {"Metacarpal", "Proximal", "Middle", "Distal"};
std::array<const std::string, 3> dimNames = {"X", "Y", "Z"};
std::array<const std::string, 4> stateNames = {"STATE_INVALID", "STATE_START", "STATE_UPDATE", "STATE_END"};

void SampleListener::onInit(const Leap::Controller& controller) {
    std::cout << "Initialized" << std::endl;
}

void SampleListener::onConnect(const Leap::Controller& controller) {
    std::cout << "Connected" << std::endl;
//    controller.enableGesture(Leap::Gesture::TYPE_CIRCLE);
//    controller.enableGesture(Leap::Gesture::TYPE_KEY_TAP);
//    controller.enableGesture(Leap::Gesture::TYPE_SCREEN_TAP);
//    controller.enableGesture(Leap::Gesture::TYPE_SWIPE);
}

void SampleListener::onDisconnect(const Leap::Controller& controller) {
    // Note: not dispatched when running in a debugger.
    std::cout << "Disconnected" << std::endl;
}

void SampleListener::onExit(const Leap::Controller& controller) {
    outlets.clear();
    std::cout << "Exited" << std::endl;
}

void SampleListener::onFrame(const Leap::Controller& controller) {
    // Get the most recent frame and report some basic information
    const Leap::Frame frame = controller.frame();
    // TODO: Use int64_t frame.timestamp() either as 2 channels or convert to timestamp.

    std::vector<float> currsample(3 * (1 + fingerNames.size() * boneNames.size()));
    lsl::stream_outlet* p_out;
    Leap::HandList hands = frame.hands();
    for (Leap::HandList::const_iterator hl = hands.begin(); hl != hands.end(); ++hl) {
        const Leap::Hand hand = *hl;
        if (hand.isLeft() && outlets.size() > 0)
            p_out = &outlets[0];
        else if (hand.isRight() && outlets.size() > 1)
            p_out = &outlets[1];

        std::fill(currsample.begin(), currsample.end(), 0);

        // Fill the palm values
        Leap::Vector palmpos = hand.palmPosition();
        currsample[0] = palmpos.x;
        currsample[1] = palmpos.y;
        currsample[2] = palmpos.z;

        // Fill the finger values
        const Leap::FingerList fingers = hand.fingers();
        size_t f_ix = 0;
        for (Leap::FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); ++f_ix, ++fl) {
            const Leap::Finger finger = *fl;
            // Get finger bones
            for (int b = 0; b < 4; ++b) {
                Leap::Bone::Type boneType = static_cast<Leap::Bone::Type>(b);
                Leap::Bone bone = finger.bone(boneType);
                Leap::Vector pos = bone.nextJoint();
                currsample[3 * (1 + f_ix * 4 + b) + 0] = pos.x;
                currsample[3 * (1 + f_ix * 4 + b) + 1] = pos.y;
                currsample[3 * (1 + f_ix * 4 + b) + 2] = pos.z;
            }
        }

        p_out->push_sample(currsample);
    }

}

void SampleListener::onFocusGained(const Leap::Controller& controller) {
    std::cout << "Focus Gained" << std::endl;
}

void SampleListener::onFocusLost(const Leap::Controller& controller) {
    std::cout << "Focus Lost" << std::endl;
}

void SampleListener::onDeviceChange(const Leap::Controller& controller) {
    std::cout << "Device Changed" << std::endl;
    const Leap::DeviceList devices = controller.devices();
    outlets.clear();
    int n_channels = dimNames.size() * (1 + fingerNames.size() * boneNames.size());  // palm + each finger bone
    for (int i = 0; i < devices.count(); ++i) {
        if (devices[i].isStreaming()) {
            for (int hand_ix = 0; hand_ix < 2; ++hand_ix) {
                std::string hand_suffix = hand_ix == 0 ? "-Left" : "-Right";
                lsl::stream_info info(devices[i].toString() + hand_suffix, "MoCap", n_channels, 0, lsl::cf_float32, devices[i].toString() + hand_suffix);
                info.desc()
                    .append_child("acquisition")
                    .append_child_value("manufacturer", "UltraLeap")
                    .append_child_value("model", "Leap Motion")
                    .append_child_value("serial", devices[i].toString());
                lsl::xml_element chans = info.desc().append_child("channels");
                // Palm
                for (auto& dim:dimNames)
                    chans.append_child("channel").append_child_value("label", "Palm-none-" + dim);
                for(auto& finger:fingerNames)
                    for (auto& bone:boneNames)
                        for (auto& dim:dimNames)
                            chans.append_child("channel").append_child_value("label", finger + "-" + bone + "-" + dim);
                outlets.emplace_back(lsl::stream_outlet(info));
            }
        }
        std::cout << "id: " << devices[i].toString() << std::endl;
        std::cout << "  isStreaming: " << (devices[i].isStreaming() ? "true" : "false") << std::endl;
    }
}

void SampleListener::onServiceConnect(const Leap::Controller& controller) {
    std::cout << "Service Connected" << std::endl;
}

void SampleListener::onServiceDisconnect(const Leap::Controller& controller) {
    std::cout << "Service Disconnected" << std::endl;
}

int main(int argc, char** argv) {
    // Create a sample listener and controller
    SampleListener listener;
    Leap::Controller controller;

    // Have the sample listener receive events from the controller
    controller.addListener(listener);

    if (argc > 1 && strcmp(argv[1], "--bg") == 0)
        controller.setPolicy(Leap::Controller::POLICY_BACKGROUND_FRAMES);

    // Keep this process running until Enter is pressed
    std::cout << "Press Enter to quit..." << std::endl;
    std::cin.get();

    // Remove the sample listener when done
    controller.removeListener(listener);

    return 0;
}
