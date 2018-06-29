#include "nuitrack.h"

#include <string>
#include <vector>

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

    // Initalize Color Table for Visualization
    colors[0] = cv::Vec3b( 255,   0,   0 ); // Blue
    colors[1] = cv::Vec3b(   0, 255,   0 ); // Green
    colors[2] = cv::Vec3b(   0,   0, 255 ); // Red
    colors[3] = cv::Vec3b( 255, 255,   0 ); // Cyan
    colors[4] = cv::Vec3b( 255,   0, 255 ); // Magenta
    colors[5] = cv::Vec3b(   0, 255, 255 ); // Yellow
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

    // Create Tracker
    user_tracker = tdv::nuitrack::UserTracker::create();
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

    // Update User
    updateUser();
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

// Update User
inline void NuiTrack::updateUser()
{
    // Update Tracker
    try{
        tdv::nuitrack::Nuitrack::waitUpdate( user_tracker );
    }
    catch( const tdv::nuitrack::LicenseNotAcquiredException& ex ){
        throw std::runtime_error( "failed license not acquired" );
    }

    // Retrieve User Frame
    user_frame = user_tracker->getUserFrame();
}

// Draw Data
void NuiTrack::draw()
{
    // Draw Depth
    drawDepth();

    // Draw User
    drawUser();
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

// Draw User
inline void NuiTrack::drawUser()
{
    if( depth_mat.empty() ){
        return;
    }

    // Copy Depth Mat
    cv::cvtColor( depth_mat, user_mat, CV_GRAY2BGR );
    user_mat.convertTo( user_mat, CV_8U, -255.0 / max_distance, 255.0 ); // 0-max_distance -> 255(white)-0(black)
    //user_mat.convertTo( user_mat, CV_8U, 255.0 / max_distance, 0.0 ); // 0-max_distance -> 0(black)-255(white)

    // Draw User Area
    const uint16_t* labels = user_frame->getData();
    #pragma omp parallel for
    for( int32_t y = 0; y < depth_height; y++ ){
        for( int32_t x = 0; x < depth_width; x++ ){
            const uint32_t index = y * depth_width + x;
            const uint16_t label = labels[index];
            if( label ){
                user_mat.at<cv::Vec3b>( y, x ) = colors[label - 1];
            }
        }
    }

    // Draw Bounding Box
    const std::vector<tdv::nuitrack::User> users = user_frame->getUsers();
    for( const tdv::nuitrack::User& user : users ){
        const int32_t id = user.id;
        const cv::Point point1 = { static_cast<int32_t>( user.box.left * depth_width ), static_cast<int32_t>( user.box.top * depth_height ) };
        const cv::Point point2 = { static_cast<int32_t>( user.box.right * depth_width ), static_cast<int32_t>( user.box.bottom * depth_height ) };
        cv::rectangle( user_mat, point1, point2, colors[id - 1] );
    }
}

// Show Data
void NuiTrack::show()
{
    // Show User
    showUser();
}

// Show User
inline void NuiTrack::showUser()
{
    if( user_mat.empty() ){
        return;
    }

    // Show User Image
    cv::imshow( "User", user_mat );
}
