#ifndef __NUITRACK__
#define __NUITRACK__

#include <nuitrack/Nuitrack.h>
#include <opencv2/opencv.hpp>
#include <array>

#define USER_COUNT 6

class NuiTrack
{
private:
    // Depth Sensor
    tdv::nuitrack::DepthSensor::Ptr depth_sensor;
    tdv::nuitrack::DepthFrame::Ptr depth_frame;
    cv::Mat depth_mat;
    uint32_t depth_width = 1280;
    uint32_t depth_height = 720;
    uint32_t max_distance = 5000;

    // User Tracker
    tdv::nuitrack::UserTracker::Ptr user_tracker;
    tdv::nuitrack::UserFrame::Ptr user_frame;
    cv::Mat user_mat;
    std::array<cv::Vec3b, USER_COUNT> colors;

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

    // Update Depth
    inline void updateDepth();

    // Update User
    inline void updateUser();

    // Draw Data
    void draw();

    // Draw Depth
    inline void drawDepth();

    // Draw User
    inline void drawUser();

    // Show Data
    void show();

    // Show User
    inline void showUser();
};

#endif // __NUITRACK__
