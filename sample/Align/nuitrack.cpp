#include "nuitrack.h"

#include <string>

// Constructor
NuiTrack::NuiTrack( const std::string& config_json )
{
    // Initialize
    initialize( config_json );
}

// Destructor
NuiTrack::~NuiTrack()
{
    // Finalize
    finalize();
}

// Processing
void NuiTrack::run()
{
    // Run NuiTrack
    tdv::nuitrack::Nuitrack::run();

    // Main Loop
    while( true ){
        // Update Data
        update();

        // Draw Data
        draw();

        // Show Data
        show();

        // Key Check
        const int32_t key = cv::waitKey( 10 );
        if( key == 'q' ){
            break;
        }
    }
}

// Initialize
void NuiTrack::initialize( const std::string& config_json )
{
    cv::setUseOptimized( true );

    // Initialize NuiTrack
    tdv::nuitrack::Nuitrack::init( config_json );

    // Initialize Sensor
    initializeSensor();
}

// Initialize Sensor
inline void NuiTrack::initializeSensor()
{
    // Set Device Config
    tdv::nuitrack::Nuitrack::setConfigValue( "Realsense2Module.RGB.ProcessWidth", std::to_string( color_width ) );
    tdv::nuitrack::Nuitrack::setConfigValue( "Realsense2Module.RGB.ProcessHeight", std::to_string( color_height ) );
    tdv::nuitrack::Nuitrack::setConfigValue( "Realsense2Module.Depth.ProcessWidth", std::to_string( depth_width ) );
    tdv::nuitrack::Nuitrack::setConfigValue( "Realsense2Module.Depth.ProcessHeight", std::to_string( depth_height ) );
    tdv::nuitrack::Nuitrack::setConfigValue( "Realsense2Module.Depth2ColorRegistration", align ? "true" : "false" );

    // Get Device Config
    max_distance = std::stoi( tdv::nuitrack::Nuitrack::getConfigValue( "Realsense2Module.Depth.ProcessMaxDepth" ) );

    // Create Sensor
    color_sensor = tdv::nuitrack::ColorSensor::create();
    depth_sensor = tdv::nuitrack::DepthSensor::create();
}

// Finalize
void NuiTrack::finalize()
{
    // Close Windows
    cv::destroyAllWindows();

    // Release NuiTrack
    tdv::nuitrack::Nuitrack::release();
}

// Update Data
void NuiTrack::update()
{
    // Update Frame
    updateFrame();

    // Update Color
    updateColor();

    // Update Depth
    updateDepth();
}

// Update Frame
inline void NuiTrack::updateFrame()
{
    // Update Frame
    tdv::nuitrack::Nuitrack::update();
}

// Update Color
inline void NuiTrack::updateColor()
{
    // Retrieve Color Frame
    color_frame = color_sensor->getColorFrame();

    // Retrive Frame Size
    color_width = color_frame->getCols();
    color_height = color_frame->getRows();
}

// Update Depth
inline void NuiTrack::updateDepth()
{
    // Retrieve Depth Frame
    depth_frame = depth_sensor->getDepthFrame();

    // Retrive Frame Size
    depth_width = depth_frame->getCols();
    depth_height = depth_frame->getRows();
}

// Draw Data
void NuiTrack::draw()
{
    // Draw Color
    drawColor();

    // Draw Depth
    drawDepth();
}

// Draw Color
inline void NuiTrack::drawColor()
{
    // Create cv::Mat form Color Data
    const tdv::nuitrack::Color3* color_data = color_frame->getData();
    color_mat = cv::Mat::zeros( color_height, color_width, CV_8UC3 );

    #pragma omp parallel for
    for( int32_t index = 0; index < color_mat.total(); index++ ){
        const tdv::nuitrack::Color3 color = color_data[index];
        color_mat.at<cv::Vec3b>( index ) = cv::Vec3b( color.blue, color.green, color.red );
    }
}

// Draw Depth
inline void NuiTrack::drawDepth()
{
    // Create cv::Mat form Depth Data
    const uint16_t* depth_data = depth_frame->getData();
    depth_mat = cv::Mat::zeros( depth_height, depth_width, CV_16UC1 );

    #pragma omp parallel for
    for( int32_t index = 0; index < depth_mat.total(); index++ ){
        const uint16_t depth = depth_data[index];
        depth_mat.at<ushort>( index ) = depth;
    }
}

// Show Data
void NuiTrack::show()
{
    // Show Color
    showColor();

    // Show Depth
    showDepth();
}

// Show Color
inline void NuiTrack::showColor()
{
    if( color_mat.empty() ){
        return;
    }

    // Show Color Image
    cv::imshow( "Color", color_mat );
}

// Show Depth
inline void NuiTrack::showDepth()
{
    if( depth_mat.empty() ){
        return;
    }

    // Scaling
    cv::Mat scale_mat;
    depth_mat.convertTo( scale_mat, CV_8U, -255.0 / max_distance, 255.0 ); // 0-max_distance -> 255(white)-0(black)
    //depth_mat.convertTo( scale_mat, CV_8U, 255.0 / max_distance, 0.0 ); // 0-max_distance -> 0(black)-255(white)

    // Apply False Colour
    //cv::applyColorMap( scale_mat, scale_mat, cv::COLORMAP_BONE );

    // Show Depth Image
    cv::imshow( "Depth", scale_mat );
}
