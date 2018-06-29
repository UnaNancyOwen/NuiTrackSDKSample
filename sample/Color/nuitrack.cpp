#include "nuitrack.h"

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

    // Create Sensor
    color_sensor = tdv::nuitrack::ColorSensor::create();
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

// Draw Data
void NuiTrack::draw()
{
    // Draw Color
    drawColor();
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
        color_mat.at<cv::Vec3b>( index ) = cv::Vec3b( color.red, color.green, color.blue ); // issues #485 on forum
    }
}

// Show Data
void NuiTrack::show()
{
    // Show Color
    showColor();
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
