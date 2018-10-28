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
    tdv::nuitrack::Nuitrack::setConfigValue( "Realsense2Module.RGB.ProcessWidth", std::to_string( color_width ) );
    tdv::nuitrack::Nuitrack::setConfigValue( "Realsense2Module.RGB.ProcessHeight", std::to_string( color_height ) );
    tdv::nuitrack::Nuitrack::setConfigValue( "Realsense2Module.Depth2ColorRegistration", align ? "true" : "false" );

    // Create Sensor
    color_sensor = tdv::nuitrack::ColorSensor::create();

    // Create Tracker
    skeleton_tracker = tdv::nuitrack::SkeletonTracker::create();

    // Create Recognizer
    gesture_recognizer = tdv::nuitrack::GestureRecognizer::create();

    // Register Callback
    gesture_recognizer->connectOnNewGestures( std::bind( &NuiTrack::onNewGestures, this, std::placeholders::_1 ) );
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

    // Update Skeleton
    updateSkeleton();
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

// Update Skeleton
inline void NuiTrack::updateSkeleton()
{
    // Update Tracker
    try{
        tdv::nuitrack::Nuitrack::waitUpdate( skeleton_tracker );
    } catch( const tdv::nuitrack::LicenseNotAcquiredException& ex ){
        throw std::runtime_error( "failed license not acquired" );
    }

    // Retrieve Skeleton Data
    skeleton_data = skeleton_tracker->getSkeletons();
}

// Draw Data
void NuiTrack::draw()
{
    // Draw Color
    drawColor();

    // Draw Skeleton
    drawSkeleton();
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

// Draw Skeleton
inline void NuiTrack::drawSkeleton()
{
    if( color_mat.empty() ){
        return;
    }

    // Copy Color Mat
    color_mat.copyTo( skeleton_mat );

    // Draw Skeleton
    const std::vector<tdv::nuitrack::Skeleton> skeletons = skeleton_data->getSkeletons();

    for( const tdv::nuitrack::Skeleton& skeleton : skeletons ){
        const int32_t id = skeleton.id;
        const std::vector<tdv::nuitrack::Joint> joints = skeleton.joints;

        // Left Hand
        const tdv::nuitrack::Joint left_hand = joints[tdv::nuitrack::JointType::JOINT_LEFT_HAND];
        if( left_hand.confidence > 0.2 ){
            const cv::Point point = { static_cast<int32_t>( left_hand.proj.x * color_width ) , static_cast<int32_t>( left_hand.proj.y * color_height ) };
            cv::circle( skeleton_mat, point, 5, colors[id - 1], -1 );
        }

        // Right Hand
        const tdv::nuitrack::Joint right_hand = joints[tdv::nuitrack::JointType::JOINT_RIGHT_HAND];
        if( right_hand.confidence > 0.2 ){
            const cv::Point point = { static_cast<int32_t>( right_hand.proj.x * color_width ) , static_cast<int32_t>( right_hand.proj.y * color_height ) };
            cv::circle( skeleton_mat, point, 5, colors[id - 1], -1 );
        }
    }
}

// Show Data
void NuiTrack::show()
{
    // Show Skeleton
    showSkeleton();
}

// Show Skeleton
inline void NuiTrack::showSkeleton()
{
    if( skeleton_mat.empty() ){
        return;
    }

    // Show Skeleton Image
    cv::imshow( "Skeleton", skeleton_mat );
}

// On New Gestures
void NuiTrack::onNewGestures( const tdv::nuitrack::GestureData::Ptr gesture_data )
{
    // Show Gestures
    const std::vector<tdv::nuitrack::Gesture> gestures = gesture_data->getGestures();
    for( const tdv::nuitrack::Gesture& gesture : gestures ){
        std::cout << gesture.userId << " " << type2string( gesture.type ) << std::endl;
    }
}

// Convert Gesture Type to String
inline std::string NuiTrack::type2string( const tdv::nuitrack::GestureType gesture_type )
{
    switch( gesture_type ){
        case tdv::nuitrack::GestureType::GESTURE_WAVING:
            return "GESTURE_WAVING";
        case tdv::nuitrack::GestureType::GESTURE_SWIPE_LEFT:
            return "GESTURE_SWIPE_LEFT";
        case tdv::nuitrack::GestureType::GESTURE_SWIPE_RIGHT:
            return "GESTURE_SWIPE_RIGHT";
        case tdv::nuitrack::GestureType::GESTURE_SWIPE_UP:
            return "GESTURE_SWIPE_UP";
        case tdv::nuitrack::GestureType::GESTURE_SWIPE_DOWN:
            return "GESTURE_SWIPE_DOWN";
        case tdv::nuitrack::GestureType::GESTURE_PUSH:
            return "GESTURE_PUSH";
        default:
            throw std::runtime_error( "failed can't convert gesture type to string" );
            return "";
    }
}
