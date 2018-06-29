#ifndef __NUITRACK__
#define __NUITRACK__

#include <nuitrack/Nuitrack.h>
#include <opencv2/opencv.hpp>
#include <array>

#define USER_COUNT 6

class NuiTrack
{
private:
    // Color Sensor
    tdv::nuitrack::ColorSensor::Ptr color_sensor;
    tdv::nuitrack::RGBFrame::Ptr color_frame;
    cv::Mat color_mat;
    uint32_t color_width = 1280;
    uint32_t color_height = 720;

    // Hand Tracker
    tdv::nuitrack::HandTracker::Ptr hand_tracker;
    tdv::nuitrack::HandTrackerData::Ptr hand_data;
    cv::Mat hand_mat;
    std::array<cv::Vec3b, USER_COUNT> colors;

    // Align
    bool align = true;

public:
    // Constructor
    NuiTrack( const std::string& config_json = "" );

    // Destructor
    ~NuiTrack();

    // Processing
    void run();

private:
    // Initialize
    void initialize( const std::string& config_json );

    // Initialize Sensor
    inline void initializeSensor();

    // Finalize
    void finalize();

    // Update Data
    void update();

    // Update Frame
    inline void updateFrame();

    // Update Color
    inline void updateColor();

    // Update Hand
    inline void updateHand();

    // Draw Data
    void draw();

    // Draw Color
    inline void drawColor();

    // Draw Hands
    inline void drawHands();

    // Draw Hand
    inline void drawHand( const tdv::nuitrack::Hand::Ptr hand, const int32_t id );

    // Show Data
    void show();

    // Show Hand
    inline void showHand();
};

#endif // __NUITRACK__
