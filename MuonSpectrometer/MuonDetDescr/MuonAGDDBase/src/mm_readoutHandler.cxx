/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

    MM->roParameters.sStripWidth           = getAttributeAsDouble    ("sStripWidth", ret);
    if (! ret) std::cout << "WARNING: sStripWidth is missing" << std::endl;

    MM->roParameters.lStripWidth           = getAttributeAsDouble    ("lStripWidth", ret);
    if (! ret) std::cout << "WARNING: lStripWidth is missing" << std::endl;

    MM->roParameters.activeH               = getAttributeAsDouble    ("activeH", ret);
    if (! ret) std::cout << "WARNING: activeH is missing" << std::endl;

    MM->roParameters.minYPhi               = getAttributeAsDouble    ("minYPhi", ret);
    if (! ret) std::cout << "WARNING: minYPhi is missing" << std::endl;

    MM->roParameters.maxYPhi               = getAttributeAsDouble    ("maxYPhi", ret);
    if (! ret) std::cout << "WARNING: maxYPhi is missing" << std::endl;


}
