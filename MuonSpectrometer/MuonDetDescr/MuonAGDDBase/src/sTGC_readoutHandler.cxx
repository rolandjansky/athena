/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAGDDBase/sTGC_readoutHandler.h"
#include "MuonAGDDDescription/sTGCDetectorDescription.h"
#include "GaudiKernel/Bootstrap.h"
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
    if (! ret) std::cout << "WARNING: sPadWidth is missing" << std::endl;

    sTGC->roParameters.lPadWidth           = getAttributeAsDouble    ("lPadWidth", ret);
    if (! ret) std::cout << "WARNING: lPadWidth is missing" << std::endl;

    sTGC->roParameters.padH                = getAttributeAsVector    ("padH", ret);
    if (! ret) std::cout << "WARNING: padH is missing" << std::endl; 

    sTGC->roParameters.nPadPhi         = getAttributeAsIntVector ("nPadPhi", ret);
    if (! ret) { // Check for backwards compatibility
      std::cout << "INFO: nPadPhi is missing trying older format nPadX" << std::endl;
      sTGC->roParameters.nPadPhi       = getAttributeAsIntVector ("nPadX", ret);
      if (! ret) std::cout << "WARNING: nPadX is missing" << std::endl;
    }

    sTGC->roParameters.anglePadPhi            = getAttributeAsDouble    ("anglePadPhi", ret);
    if (! ret){
      std::cout << "INFO: anglePadPhi is missing trying older format anglePadX" << std::endl;
      sTGC->roParameters.anglePadPhi            = getAttributeAsDouble    ("anglePadX", ret);
      if (! ret) std::cout << "WARNING: anglePadX is missing" << std::endl;
    }

    sTGC->roParameters.firstPadPhiDivision_A = getAttributeAsVector    ("firstPadPhiDivision_A", ret);
    sTGC->roParameters.firstPadPhiDivision_C         = getAttributeAsVector    ("firstPadPhiDivision_C", ret);
    if (! ret){
      std::cout << "INFO: firstPadPhiDivision_C/A is missing trying older format firstPadPhi" << std::endl; 
      sTGC->roParameters.firstPadPhiDivision_A        = getAttributeAsVector    ("firstPadPhi", ret);
      sTGC->roParameters.firstPadPhiDivision_C        = getAttributeAsVector    ("firstPadPhi", ret);
      if (! ret) std::cout << "WARNING: firstPadPhi is missing" << std::endl;
    }



      sTGC->roParameters.PadPhiShift_A = getAttributeAsVector    ("PadPhiShift_A", ret);
      sTGC->roParameters.PadPhiShift_C = getAttributeAsVector    ("PadPhiShift_C", ret);
    if (! ret){
      std::cout << "INFO: PadPhiShift_C/A is missing trying older format firstPadPhi" << std::endl; 
      sTGC->roParameters.PadPhiShift_A        = getAttributeAsVector    ("firstPadPhi", ret);
      sTGC->roParameters.PadPhiShift_C        = getAttributeAsVector    ("firstPadPhi", ret);
      if (! ret) std::cout << "WARNING: firstPadPhi is missing" << std::endl;
    }

    sTGC->roParameters.nPadH               = getAttributeAsVector    ("nPadH", ret);
    if (! ret) std::cout << "WARNING: nPadH is missing" << std::endl;

    sTGC->roParameters.firstPadH           = getAttributeAsVector    ("firstPadH", ret);
    if (! ret) std::cout << "WARNING: firstPadH is missing" << std::endl;

    sTGC->roParameters.firstPadRow         = getAttributeAsIntVector ("firstPadRow", ret);
    if (! ret) std::cout << "WARNING: firstPadRow is missing" << std::endl;

    sTGC->roParameters.nWires              = getAttributeAsIntVector ("nWires", ret);
    if (! ret) std::cout << "WARNING: nWires is missing" << std::endl;

    sTGC->roParameters.wireCutout          = getAttributeAsVector ("wireCutout", ret);
    if (! ret) std::cout << "WARNING: wireCutout is missing" << std::endl;

    sTGC->roParameters.firstWire           = getAttributeAsVector    ("firstWire", ret);
    if (! ret) std::cout << "WARNING: firstWire is missing" << std::endl;

    sTGC->roParameters.wireGroupWidth      = getAttributeAsDouble    ("wireGroupWidth", ret);
    if (! ret) std::cout << "WARNING: wireGroupWidth is missing" << std::endl;

    sTGC->roParameters.nStrips             = getAttributeAsInt       ("nStrips", ret);
    if (! ret) std::cout << "WARNING: nStrips is missing" << std::endl;

    sTGC->roParameters.firstTriggerBand    = getAttributeAsIntVector ("firstTriggerBand", ret);
    if (! ret) std::cout << "WARNING: firstTriggerBand is missing" << std::endl;

    sTGC->roParameters.nTriggerBands       = getAttributeAsIntVector ("nTriggerBands", ret);
    if (! ret) std::cout << "WARNING: nTriggerBands is missing" << std::endl;

    sTGC->roParameters.firstStripInTrigger = getAttributeAsIntVector ("firstStripInTrigger", ret);
    if (! ret) std::cout << "WARNING: firstStripInTrigger is missing" << std::endl;

    sTGC->roParameters.firstStripWidth     = getAttributeAsVector    ("firstStripWidth", ret);
    if (! ret) std::cout << "WARNING: firstStripWidth is missing" << std::endl;

    sTGC->roParameters.StripsInBandsLayer1 = getAttributeAsIntVector ("StripsInBandsLayer1", ret);
    if (! ret) std::cout << "WARNING: StripsInBandsLayer1 is missing" << std::endl;

    sTGC->roParameters.StripsInBandsLayer2 = getAttributeAsIntVector ("StripsInBandsLayer2", ret);
    if (! ret) std::cout << "WARNING: StripsInBandsLayer2 is missing" << std::endl;

    sTGC->roParameters.StripsInBandsLayer3 = getAttributeAsIntVector ("StripsInBandsLayer3", ret);
    if (! ret) std::cout << "WARNING: StripsInBandsLayer3 is missing" << std::endl;

    sTGC->roParameters.StripsInBandsLayer4 = getAttributeAsIntVector ("StripsInBandsLayer4", ret);
    if (! ret) std::cout << "WARNING: StripsInBandsLayer4 is missing" << std::endl;

    sTGC->roParameters.nWireGroups         = getAttributeAsIntVector ("nWireGroups", ret);
    if (! ret) std::cout << "WARNING: nWireGroups is missing" << std::endl;

    sTGC->roParameters.firstWireGroup      = getAttributeAsIntVector ("firstWireGroup", ret);
    if (! ret) std::cout << "WARNING: firstWireGroup is missing" << std::endl;

}
