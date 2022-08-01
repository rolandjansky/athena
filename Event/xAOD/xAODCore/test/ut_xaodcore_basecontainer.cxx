#include "xAODCore/BaseContainer.h"
#include "xAODCore/AuxContainerBase.h"

int main() {
    xAOD::BaseContainer interface;
    xAOD::AuxContainerBase aux;
    interface.setStore(&aux);

    SG::AuxElement* obj = nullptr;
    
    obj = new SG::AuxElement();
    interface.push_back( obj );
    obj->auxdata< int >( "AnInt" ) = 7;
    obj->auxdata< float >( "AFloat" ) = 0.7;


    obj = new SG::AuxElement();
    interface.push_back( obj );
    obj->auxdata< int >( "AnInt" ) = 9;
    obj->auxdata< float >( "AFloat" ) = 1.9;

    for ( auto el: interface) {
        std::cout << el->auxdata<int>("AnInt") << " " << el->auxdata<float>("AFloat") << std::endl;
    }



}