#ifndef __NUITRACK__
#define __NUITRACK__

#include "parser.h"

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

    // Face Tracker
    parser::JSON json;
    cv::Mat face_mat;
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

    // Update Face
    inline void updateFace();

    // Draw Data
    void draw();

    // Draw Color
    inline void drawColor();

    // Draw Face
    inline void drawFace();

    // Draw Attributes
    inline void drawAttributes( cv::Mat& image, const parser::Face& face, const cv::Point& org, const double fontScale, const cv::Vec3b& color, const int32_t thickness = 2 );

    // Show Data
    void show();

    // Show Face
    inline void showFace();
};


#endif // __NUITRACK__
