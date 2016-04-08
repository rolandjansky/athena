/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAGDDBase/sTGC_readoutHandler.h"
#include "MuonAGDDDescription/sTGCDetectorDescription.h"
#include <iostream>


sTGC_readoutHandler::sTGC_readoutHandler(std::string s):XMLHandler(s)
{
}

void sTGC_readoutHandler::ElementHandle()
{
    sTGCDetectorDescription* sTGC=sTGCDetectorDescription::GetCurrent();
    // std::cout<<" this is sTGC_readoutHandler::Handle"<<sTGC->GetName()<<std::endl;

    bool ret=true;
    sTGC->roParameters.sPadWidth           = getAttributeAsDouble    ("sPadWidth", ret);
    sTGC->roParameters.lPadWidth           = getAttributeAsDouble    ("lPadWidth", ret);
    sTGC->roParameters.padH                = getAttributeAsVector    ("padH", ret);
    sTGC->roParameters.nPadX               = getAttributeAsIntVector ("nPadX", ret);
    sTGC->roParameters.anglePadX           = getAttributeAsDouble    ("anglePadX", ret);
    sTGC->roParameters.firstPadPhi         = getAttributeAsVector    ("firstPadPhi", ret);
    sTGC->roParameters.PadPhiShift         = getAttributeAsVector    ("PadPhiShift", ret);
    sTGC->roParameters.nPadH               = getAttributeAsVector    ("nPadH", ret);
    sTGC->roParameters.firstPadH           = getAttributeAsVector    ("firstPadH", ret);
    sTGC->roParameters.firstPadRow         = getAttributeAsIntVector ("firstPadRow", ret);
    sTGC->roParameters.nWires              = getAttributeAsIntVector ("nWires", ret);
    sTGC->roParameters.firstWire           = getAttributeAsVector    ("firstWire", ret);
    sTGC->roParameters.wireGroupWidth      = getAttributeAsDouble    ("wireGroupWidth", ret);
    sTGC->roParameters.nStrips             = getAttributeAsInt       ("nStrips", ret);
    sTGC->roParameters.firstTriggerBand    = getAttributeAsIntVector ("firstTriggerBand", ret);
    sTGC->roParameters.nTriggerBands       = getAttributeAsIntVector ("nTriggerBands", ret);
    sTGC->roParameters.firstStripInTrigger = getAttributeAsIntVector ("firstStripInTrigger", ret);
    sTGC->roParameters.firstStripWidth     = getAttributeAsVector    ("firstStripWidth", ret);
    sTGC->roParameters.StripsInBandsLayer1 = getAttributeAsIntVector ("StripsInBandsLayer1", ret);
    sTGC->roParameters.StripsInBandsLayer2 = getAttributeAsIntVector ("StripsInBandsLayer2", ret);
    sTGC->roParameters.StripsInBandsLayer3 = getAttributeAsIntVector ("StripsInBandsLayer3", ret);
    sTGC->roParameters.StripsInBandsLayer4 = getAttributeAsIntVector ("StripsInBandsLayer4", ret);
    sTGC->roParameters.nWireGroups         = getAttributeAsIntVector ("nWireGroups", ret);
    sTGC->roParameters.firstWireGroup      = getAttributeAsIntVector ("firstWireGroup", ret);
    sTGC->roParameters.leftmostPadColumn   = getAttributeAsIntVector ("leftmostPadColumn", ret);
    sTGC->roParameters.rightmostPadColumn  = getAttributeAsIntVector ("rightmostPadColumn", ret);
}
