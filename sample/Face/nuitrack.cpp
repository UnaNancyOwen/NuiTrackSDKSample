#include "nuitrack.h"

#include <string>
#include <vector>
#include <iomanip>
#include <ostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>

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

    // Enable Face Module
    tdv::nuitrack::Nuitrack::setConfigValue( "Faces.ToUse", "true" );
    tdv::nuitrack::Nuitrack::setConfigValue( "DepthProvider.Depth2ColorRegistration", "true" );

    // Create Sensor
    color_sensor = tdv::nuitrack::ColorSensor::create();

    // Create Tracker
    skeleton_tracker = tdv::nuitrack::SkeletonTracker::create();
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

    // Update Face
    updateFace();
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

// Update Face
inline void NuiTrack::updateFace()
{
    // Update Tracker
    json = parser::parse( tdv::nuitrack::Nuitrack::getInstancesJson() );
}

// Draw Data
void NuiTrack::draw()
{
    // Draw Color
    drawColor();

    // Draw Face
    drawFace();
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
        color_mat.at<cv::Vec3b>( index ) = cv::Vec3b( color.blue, color.green, color.red ); // issues #485 on forum
    }
}

// Draw Face
inline void NuiTrack::drawFace()
{
    if( color_mat.empty() ){
        return;
    }

    // Copy Color Mat
    color_mat.copyTo( face_mat );

    // Draw Face
    for( const parser::Human& human : json.humans ){
        if( !human.face ){
            continue;
        }

        const parser::Face& face = human.face.get();
        const cv::Vec3b color = colors[human.id - 1];

        // Rectangle
        const cv::Rect rectangle = { 
            static_cast<int32_t>( face.rectangle.x * color_width       ), // X
            static_cast<int32_t>( face.rectangle.y * color_height      ), // Y
            static_cast<int32_t>( face.rectangle.width * color_width   ), // Width
            static_cast<int32_t>( face.rectangle.height * color_height )  // Height
        };
        cv::rectangle( face_mat, rectangle, color );

        // Landmarks
        for( const parser::Vec& landmark : face.landmarks ){
            const cv::Point point = { 
                static_cast<int32_t>( landmark.x * color_width  ), // X
                static_cast<int32_t>( landmark.y * color_height )  // Y
            };
            cv::circle( face_mat, point, 5, color, -1 );
        }

        // Attributes
        drawAttributes( face_mat, face, cv::Point( rectangle.x + rectangle.width, rectangle.y ), 1.0, color );
    }

    // Draw Parsed JSON
    std::cout << json << std::endl;
}

// Draw Attributes
inline void NuiTrack::drawAttributes( cv::Mat& image, const parser::Face& face, const cv::Point& org, const double fontScale, const cv::Vec3b& color, const int32_t thickness )
{
    if( image.empty() ){
        return;
    }

    // Attributes
    const std::string age    = face.age.type;
    std::ostringstream oss;
    oss << std::fixed << std::setprecision( 1 ) << face.age.years;
    const std::string years  = oss.str();
    const std::string gender = face.gender;

    const int32_t offset = static_cast<int32_t>( 30 * fontScale );
    cv::putText( image, "age: "    + age   , cv::Point( org.x, org.y + ( offset * 1 ) ), cv::FONT_HERSHEY_SIMPLEX, fontScale, color, thickness );
    cv::putText( image, "years: "  + years , cv::Point( org.x, org.y + ( offset * 2 ) ), cv::FONT_HERSHEY_SIMPLEX, fontScale, color, thickness );
    cv::putText( image, "gender: " + gender, cv::Point( org.x, org.y + ( offset * 3 ) ), cv::FONT_HERSHEY_SIMPLEX, fontScale, color, thickness );

    // Emotion
    const int32_t bar_width = static_cast<int32_t>( 100 * fontScale ), bar_height = static_cast<int32_t>( 20 * fontScale );
    const cv::Rect neutral  = { org.x, org.y + ( offset * 4 ) - ( offset / 2 ), static_cast<int32_t>( face.emotions.neutral  * bar_width ), bar_height };
    const cv::Rect angry    = { org.x, org.y + ( offset * 5 ) - ( offset / 2 ), static_cast<int32_t>( face.emotions.angry    * bar_width ), bar_height };
    const cv::Rect surprise = { org.x, org.y + ( offset * 6 ) - ( offset / 2 ), static_cast<int32_t>( face.emotions.surprise * bar_width ), bar_height };
    const cv::Rect happy    = { org.x, org.y + ( offset * 7 ) - ( offset / 2 ), static_cast<int32_t>( face.emotions.happy    * bar_width ), bar_height };
    cv::rectangle( image, neutral , cv::Vec3b( 255,   0,   0 ), -1 );
    cv::rectangle( image, angry   , cv::Vec3b(   0,   0, 255 ), -1 );
    cv::rectangle( image, surprise, cv::Vec3b(   0, 255, 255 ), -1 );
    cv::rectangle( image, happy   , cv::Vec3b(   0, 255,   0 ), -1 );
    cv::putText( image, "neutral" , cv::Point( org.x + bar_width, org.y + ( offset * 4 ) ), cv::FONT_HERSHEY_SIMPLEX, fontScale, cv::Vec3b( 255,   0,   0 ), thickness );
    cv::putText( image, "angry"   , cv::Point( org.x + bar_width, org.y + ( offset * 5 ) ), cv::FONT_HERSHEY_SIMPLEX, fontScale, cv::Vec3b(   0,   0, 255 ), thickness );
    cv::putText( image, "surprise", cv::Point( org.x + bar_width, org.y + ( offset * 6 ) ), cv::FONT_HERSHEY_SIMPLEX, fontScale, cv::Vec3b(   0, 255, 255 ), thickness );
    cv::putText( image, "happy"   , cv::Point( org.x + bar_width, org.y + ( offset * 7 ) ), cv::FONT_HERSHEY_SIMPLEX, fontScale, cv::Vec3b(   0, 255,   0 ), thickness );
}

// Show Data
void NuiTrack::show()
{
    // Show Face
    showFace();
}

// Show Face
inline void NuiTrack::showFace()
{
    if( face_mat.empty() ){
        return;
    }

    // Show Face Image
    cv::imshow( "Face", face_mat );
}
