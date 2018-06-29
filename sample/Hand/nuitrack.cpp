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
    hand_tracker = tdv::nuitrack::HandTracker::create();
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

    // Update Hand
    updateHand();
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

// Update Hand
inline void NuiTrack::updateHand()
{
    // Update Tracker
    try{
        tdv::nuitrack::Nuitrack::waitUpdate( hand_tracker );
    }
    catch( const tdv::nuitrack::LicenseNotAcquiredException& ex ){
        throw std::runtime_error( "failed license not acquired" );
    }

    // Retrieve Skeleton Data
    hand_data = hand_tracker->getData();
}

// Draw Data
void NuiTrack::draw()
{
    // Draw Color
    drawColor();

    // Draw Hands
    drawHands();
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

// Draw Hands
inline void NuiTrack::drawHands()
{
    if( color_mat.empty() ){
        return;
    }

    // Copy Color Mat
    color_mat.copyTo( hand_mat );

    // Draw Hands
    const std::vector<tdv::nuitrack::UserHands> users_hands = hand_data->getUsersHands();

    for( const tdv::nuitrack::UserHands& user_hands : users_hands ){
        const int32_t id = user_hands.userId;

        // Left Hand
        const tdv::nuitrack::Hand::Ptr left_hand = user_hands.leftHand;
        drawHand( left_hand, id );

        // Right Hand
        const tdv::nuitrack::Hand::Ptr right_hand = user_hands.rightHand;
        drawHand( right_hand, id );
    }
}

// Draw Hand
inline void NuiTrack::drawHand( const tdv::nuitrack::Hand::Ptr hand, const int32_t id )
{
    if( hand == nullptr ){
        return;
    }

    // Check Click
    int32_t thickness = 2;
    if( hand->click ){
        thickness = -1;
    }

    // Draw Hand Pointer on Window
    const cv::Point point = { static_cast<int32_t>( hand->x * color_width ), static_cast<int32_t>( hand->y * color_height ) };
    cv::circle( hand_mat, point, 20, colors[id - 1], thickness );
}

// Show Data
void NuiTrack::show()
{
    // Show Hand
    showHand();
}

// Show Hand
inline void NuiTrack::showHand()
{
    if( hand_mat.empty() ){
        return;
    }

    // Show Hand Image
    cv::imshow( "Hand", hand_mat );
}
