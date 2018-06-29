#ifndef __NUITRACK__
#define __NUITRACK__

#include <nuitrack/Nuitrack.h>
#include <opencv2/opencv.hpp>

class NuiTrack
{
private:
    // Color Sensor
    tdv::nuitrack::ColorSensor::Ptr color_sensor;
    tdv::nuitrack::RGBFrame::Ptr color_frame;
    cv::Mat color_mat;
    uint32_t color_width = 1280;
    uint32_t color_height = 720;

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

    // Draw Data
    void draw();

    // Draw Color
    inline void drawColor();

    // Show Data
    void show();

    // Show Color
    inline void showColor();
};

#endif // __NUITRACK__
