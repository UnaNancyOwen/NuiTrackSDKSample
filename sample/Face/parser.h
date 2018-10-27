// This is JSON parser that parses data that retrieved from NuiTrack instance-based API.
// The parsed data is output to the parser::JSON structure. You can easily access these data.
// 
// #include "parser.h"
// 
// const parser::JSON json = parser::parse( tdv::nuitrack::Nuitrack::getInstancesJson() );
// for( const parser::Human& human : json.humans ){
//     if( !human.face ){
//         continue;
//     }
//     
//     const parser::Face& face = human.face.get();
//     /* access face data */
// }
// 
// This parser depends on Boost.PropertyTree and Boost.Optional.
// Those libraries works with header only.
// 
// This source code is licensed under the MIT license.
//
// MIT License
// 
// Copyright (c) 2018 Tsukasa Sugiura
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef __PARSER__
#define __PARSER__

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>

#include <vector>
#include <string>
#include <strstream>
#include <iostream>

#define LANDMARK 31

namespace parser
{
    struct Vec
    {
        double x;
        double y;

        Vec()
            : x( 0.0 ), y( 0.0 ){}

        Vec( const double x, const double y )
            : x( x ), y( y ){}
    };

    struct Rect
    {
        double x;
        double y;
        double width;
        double height;

        Rect()
            : x( 0.0 ), y( 0.0 ), width( 0.0 ), height( 0.0 ){}

        Rect( const double x, const double y, const double width, const double height )
            : x( x ), y( y ), width( width ), height( height ){}
    };

    struct Age
    {
        std::string type;
        double years;

        Age()
            : type( "" ), years( 0.0 ){}

        Age( const std::string type, const double years )
            : type( type ), years( years ){}
    };

    struct Emotions
    {
        double happy;
        double neutral;
        double angry;
        double surprise;

        Emotions()
            : happy( 0.0 ), neutral( 0.0 ), angry( 0.0 ), surprise( 0.0 ){}

        Emotions( const double happy, const double neutral, const double angry, const double surprise )
            : happy( happy ), neutral( neutral ), angry( angry ), surprise( surprise ){}
    };

    struct Angles
    {
        double yaw;
        double pitch;
        double roll;

        Angles()
            : yaw( 0.0 ), pitch( 0.0 ), roll( 0.0 ){}

        Angles( const double yaw, const double pitch, const double roll )
            : yaw( yaw ), pitch( pitch ), roll( roll ){}
    };

    struct Eyes
    {
        parser::Vec left_eye;
        parser::Vec right_eye;

        Eyes()
            : left_eye( parser::Vec() ), right_eye( parser::Vec() ){}

        Eyes( const parser::Vec left_eye, const parser::Vec right_eye )
            : left_eye( left_eye ), right_eye( right_eye ){}
    };

    struct Face
    {
        parser::Rect rectangle;
        std::vector<parser::Vec> landmarks;
        parser::Eyes eyes;
        parser::Angles angles;
        parser::Emotions emotions;
        parser::Age age;
        std::string gender;

        Face(){}
    };

    struct Human
    {
        int32_t id;
        std::string type;
        boost::optional<parser::Face> face;
    };

    struct JSON
    {
        int64_t timestamp;
        std::vector<parser::Human> humans;

        friend std::ostream& operator<<( std::ostream& os, const parser::JSON& json )
        {
            os << json.timestamp << std::endl;
            for( const parser::Human& human : json.humans ){
                os << "id    : " << human.id << std::endl;
                os << "class : " << human.type << std::endl;
                if( !human.face ){
                    continue;
                }

                const parser::Face& face = human.face.get();
                os << "face  : " << std::endl;

                os << "\trectangle : " << std::endl;
                os << "\t\tleft   : " << face.rectangle.x << std::endl;
                os << "\t\ttop    : " << face.rectangle.y << std::endl;
                os << "\t\twidth  : " << face.rectangle.width << std::endl;
                os << "\t\theight : " << face.rectangle.height << std::endl;

                os << "\tlandmark :" << std::endl;
                uint32_t index = 0;
                for( const parser::Vec& landmark : face.landmarks ){
                    os << "\t\t" << index++ << " ( " << landmark.x << ", " << landmark.y << " )" << std::endl;
                }

                os << "\teyes :" << std::endl;
                os << "\t\tleft  : ( " << face.eyes.left_eye.x << ", " << face.eyes.left_eye.y << " )" << std::endl;
                os << "\t\tright : ( " << face.eyes.right_eye.x << ", " << face.eyes.right_eye.y << " )" << std::endl;

                os << "\tangles :" << std::endl;
                os << "\t\tyaw   : " << face.angles.yaw << std::endl;
                os << "\t\tpitch : " << face.angles.pitch << std::endl;
                os << "\t\troll  : " << face.angles.roll << std::endl;

                os << "\temotions :" << std::endl;
                os << "\t\thappy    : " << face.emotions.happy << std::endl;
                os << "\t\tneutral  : " << face.emotions.neutral << std::endl;
                os << "\t\tangry    : " << face.emotions.angry << std::endl;
                os << "\t\tsurprise : " << face.emotions.surprise << std::endl;

                os << "\tage :" << std::endl;
                os << "\t\ttype  : " << face.age.type << std::endl;
                os << "\t\tyears : " << face.age.years << std::endl;

                os << "\tgender : " << face.gender << std::endl;
            }

            return os;
        };
    };

    static parser::JSON parse( const std::string json )
    {
        std::stringstream ss( json );
        boost::property_tree::ptree pt;
        boost::property_tree::read_json( ss, pt );

        parser::JSON j;

        if( boost::optional<int64_t> timestamp = pt.get_optional<int64_t>( "Timestamp" ) ){
            j.timestamp = timestamp.get();
        }

        if( !pt.get_child_optional( "Instances" ) ){
            return j;
        }

        for( const boost::property_tree::ptree::value_type& child : pt.get_child( "Instances" ) ){
            const boost::property_tree::ptree& human = child.second;

            parser::Human h;

            h.id = human.get<int32_t>( "id" );

            h.type = human.get<std::string>( "class" );

            if( boost::optional<const boost::property_tree::ptree&> child = human.get_child_optional( "face" ) ){
                const boost::property_tree::ptree& face = child.get();

                parser::Face f;

                const boost::property_tree::ptree& rectangle = face.get_child( "rectangle" );
                f.rectangle = parser::Rect( rectangle.get<double>( "left" ), rectangle.get<double>( "top" ), rectangle.get<double>( "width" ), rectangle.get<double>( "height" ) );

                for( const boost::property_tree::ptree::value_type& child : face.get_child( "landmark" ) ){
                    const boost::property_tree::ptree& landmark = child.second;
                    f.landmarks.push_back( parser::Vec( landmark.get<double>( "x" ), landmark.get<double>( "y" ) ) );
                }

                const boost::property_tree::ptree& left_eye = face.get_child( "left_eye" );
                const boost::property_tree::ptree& right_eye = face.get_child( "right_eye" );
                f.eyes = parser::Eyes( parser::Vec( left_eye.get<double>( "x" ), left_eye.get<double>( "y" ) ), parser::Vec( right_eye.get<double>( "x" ), right_eye.get<double>( "y" ) ) );

                const boost::property_tree::ptree& angles = face.get_child( "angles" );
                f.angles = parser::Angles( angles.get<double>( "yaw" ), angles.get<double>( "pitch" ), angles.get<double>( "roll" ) );

                const boost::property_tree::ptree& emotions = face.get_child( "emotions" );
                f.emotions = parser::Emotions( emotions.get<double>( "happy" ), emotions.get<double>( "neutral" ), emotions.get<double>( "angry" ), emotions.get<double>( "surprise" ) );

                const boost::property_tree::ptree& age = face.get_child( "age" );
                f.age = parser::Age( age.get<std::string>( "type" ), age.get<double>( "years" ) );

                f.gender = face.get<std::string>( "gender" );

                h.face = f;
            }

            j.humans.push_back( h );
        }

        return j;
    };
}

#endif // __PARSER__