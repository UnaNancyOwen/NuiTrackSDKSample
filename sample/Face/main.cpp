#include <iostream>
#include <sstream>
#include <memory>

#include "nuitrack.h"

int main( int argc, char* argv[] )
{
    try{
        std::shared_ptr<NuiTrack> nuitrack;
        if( argc < 2 ){
            nuitrack = std::make_shared<NuiTrack>();
        }
        else{
            nuitrack = std::make_shared<NuiTrack>( argv[1] );
        }
        nuitrack->run();
    } catch( std::exception& ex ){
        std::cout << ex.what() << std::endl;
    }

    return 0;
}
