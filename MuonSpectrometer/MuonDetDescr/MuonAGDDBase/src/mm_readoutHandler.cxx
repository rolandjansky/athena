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
    MM->roParameters.gasThickness          = getAttributeAsDouble    ("gasThickness", ret);
    MM->roParameters.pcbThickness          = getAttributeAsDouble    ("pcbThickness", ret);
    MM->roParameters.driftThickness        = getAttributeAsDouble    ("driftThickness", ret);
    MM->roParameters.stereoAngel           = getAttributeAsVector    ("stereoAngle", ret);
    MM->roParameters.readoutSide           = getAttributeAsIntVector ("readoutSide", ret);
    MM->roParameters.zpos                  = getAttributeAsDouble    ("zPos", ret);
    MM->roParameters.distanceFromZAxis     = getAttributeAsDouble    ("distanceFromZAxis", ret);
    MM->roParameters.roLength              = getAttributeAsDouble    ("roLength", ret);

}
