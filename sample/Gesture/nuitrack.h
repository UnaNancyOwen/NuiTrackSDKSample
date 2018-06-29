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

    // Skeleton Tracker
    tdv::nuitrack::SkeletonTracker::Ptr skeleton_tracker;
    tdv::nuitrack::SkeletonData::Ptr skeleton_data;
    cv::Mat skeleton_mat;
    std::array<cv::Vec3b, USER_COUNT> colors;

    // Align
    bool align = true;

    // Gesture Recognizer
    tdv::nuitrack::GestureRecognizer::Ptr gesture_recognizer;
    tdv::nuitrack::GestureData::Ptr gesture_data;

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

    // Update Skeleton
    inline void updateSkeleton();

    // Draw Data
    void draw();

    // Draw Color
    inline void drawColor();

    // Draw Skeleton
    inline void drawSkeleton();

    // Show Data
    void show();

    // Show Skeleton
    inline void showSkeleton();

    // On New Gestures
    void onNewGestures( const tdv::nuitrack::GestureData::Ptr gesture_data );

    // Convert Gesture Type to String
    inline std::string type2string( const tdv::nuitrack::GestureType gesture_type );
};

#endif // __NUITRACK__