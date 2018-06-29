#ifndef __NUITRACK__
#define __NUITRACK__

#include <nuitrack/Nuitrack.h>
#include <opencv2/opencv.hpp>

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

    // Draw Data
    void draw();

    // Draw Depth
    inline void drawDepth();

    // Show Data
    void show();

    // Show Depth
    inline void showDepth();
};

#endif // __NUITRACK__
