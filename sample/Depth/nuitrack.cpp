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
    tdv::nuitrack::Nuitrack::setConfigValue( "Realsense2Module.Depth.ProcessWidth", std::to_string( depth_width ) );
    tdv::nuitrack::Nuitrack::setConfigValue( "Realsense2Module.Depth.ProcessHeight", std::to_string( depth_height ) );

    // Get Device Config
    max_distance = std::stoi( tdv::nuitrack::Nuitrack::getConfigValue( "Realsense2Module.Depth.ProcessMaxDepth" ) );

    // Create Sensor
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

    // Update Depth
    updateDepth();
}

// Update Frame
inline void NuiTrack::updateFrame()
{
    // Update Frame
    tdv::nuitrack::Nuitrack::update();
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
    // Draw Depth
    drawDepth();
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
    // Show Depth
    showDepth();
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
