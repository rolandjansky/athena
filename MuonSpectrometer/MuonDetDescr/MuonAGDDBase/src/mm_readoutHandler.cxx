/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAGDDBase/mm_readoutHandler.h"
#include "MuonAGDDDescription/MMDetectorDescription.h"
#include <iostream>


mm_readoutHandler::mm_readoutHandler(std::string s):XMLHandler(s)
{
}

void mm_readoutHandler::ElementHandle()
{
    MMDetectorDescription* MM=MMDetectorDescription::GetCurrent();
    // std::cout<<" this is mm_readoutHandler::Handle"<<MM->GetName()<<std::endl;

    bool ret=true;
    MM->roParameters.stripPitch            = getAttributeAsDouble    ("stripPitch", ret);
    if (! ret) std::cout << "WARNING: stripPitch is missing" << std::endl;

    MM->roParameters.gasThickness          = getAttributeAsDouble    ("gasThickness", ret);
    if (! ret) std::cout << "WARNING: gasThickness is missing" << std::endl;

    MM->roParameters.pcbThickness          = getAttributeAsDouble    ("pcbThickness", ret);
    if (! ret) std::cout << "WARNING: pcbThickness is missing" << std::endl;

    MM->roParameters.driftThickness        = getAttributeAsDouble    ("driftThickness", ret);
    if (! ret) std::cout << "WARNING: driftThickness is missing" << std::endl;

    MM->roParameters.stereoAngle           = getAttributeAsVector    ("stereoAngle", ret);
    if (! ret) std::cout << "WARNING: stereoAngle is missing" << std::endl;

    MM->roParameters.readoutSide           = getAttributeAsIntVector ("readoutSide", ret);
    if (! ret) std::cout << "WARNING: readoutSide is missing" << std::endl;

    MM->roParameters.zpos                  = getAttributeAsDouble    ("zPos", ret);
    if (! ret) std::cout << "WARNING: zPos is missing" << std::endl;

    MM->roParameters.distanceFromZAxis     = getAttributeAsDouble    ("distanceFromZAxis", ret);
    if (! ret) std::cout << "WARNING: distanceFromZAxis is missing" << std::endl;

    MM->roParameters.roLength              = getAttributeAsDouble    ("roLength", ret);
    if (! ret) std::cout << "WARNING: roLength is missing" << std::endl;

    MM->roParameters.activeBottomLength           = getAttributeAsDouble    ("activeBottomLength", ret);
    if (! ret) std::cout << "WARNING: activeBottomLength is missing" << std::endl;

    MM->roParameters.activeTopLength           = getAttributeAsDouble    ("activeTopLength", ret);
    if (! ret) std::cout << "WARNING: activeTopLength is missing" << std::endl;

    MM->roParameters.activeH               = getAttributeAsDouble    ("activeH", ret);
    if (! ret) std::cout << "WARNING: activeH is missing" << std::endl;

    MM->roParameters.minYPhiL               = getAttributeAsDouble    ("minYPhiL", ret);
    if (! ret) std::cout << "WARNING: minYPhiL is missing" << std::endl;
    
    MM->roParameters.minYPhiR               = getAttributeAsDouble    ("minYPhiR", ret);
    if (! ret) std::cout << "WARNING: minYPhiR is missing" << std::endl;

    MM->roParameters.maxYPhi               = getAttributeAsDouble    ("maxYPhi", ret);
    if (! ret) std::cout << "WARNING: maxYPhi is missing" << std::endl;
    
    MM->roParameters.dlStereoBottom               = getAttributeAsDouble    ("dlStereoBottom", ret);
    if (! ret) std::cout << "WARNING: dlStereoBottom is missing" << std::endl;
    
    MM->roParameters.dlStereoTop               = getAttributeAsDouble    ("dlStereoTop", ret);
    if (! ret) std::cout << "WARNING: dlStereoTop is missing" << std::endl;
    
    MM->roParameters.nMissedTopEta               = getAttributeAsInt    ("nMissedTopEta", ret);
    if (! ret) std::cout << "WARNING: nMissedTopEta is missing" << std::endl;
    
    MM->roParameters.nMissedBottomEta               = getAttributeAsInt    ("nMissedBottomEta", ret);
    if (! ret) std::cout << "WARNING: nMissedBottomEta is missing" << std::endl;
    
    MM->roParameters.nMissedTopStereo               = getAttributeAsInt    ("nMissedTopStereo", ret);
    if (! ret) std::cout << "WARNING: nMissedTopStereo is missing" << std::endl;
    
    MM->roParameters.nMissedBottomStereo              = getAttributeAsInt    ("nMissedBottomStereo", ret);
    if (! ret) std::cout << "WARNING: nMissedBottomStereo is missing" << std::endl;
    
    MM->roParameters.nRoutedTop               = getAttributeAsInt    ("nRoutedTop", ret);
    if (! ret) std::cout << "WARNING: nRoutedTop is missing" << std::endl;
    
    MM->roParameters.nRoutedBottom               = getAttributeAsInt    ("nRoutedBottom", ret);
    if (! ret) std::cout << "WARNING: nRoutedBottom is missing" << std::endl;
    
    MM->roParameters.tStrips             = getAttributeAsInt    ("totalStrips", ret);
    if (! ret) std::cout << "WARNING: totalStrips is missing" << std::endl;
    
}
