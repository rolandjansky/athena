/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAGDDBase/sTGC_readoutHandler.h"
#include "MuonAGDDBase/sTGCHandler.h"
#include "MuonAGDDDescription/sTGCDetectorDescription.h"
#include "AGDDControl/AGDDController.h"
#include "AGDDControl/XMLHandlerStore.h"
#include "GaudiKernel/Bootstrap.h"
#include <iostream>


sTGC_readoutHandler::sTGC_readoutHandler(const std::string& s,
                                         AGDDController& c)
  : XMLHandler(s, c)
{
}

void sTGC_readoutHandler::ElementHandle(AGDDController& c,
                                        xercesc::DOMNode *t)
{
    sTGCHandler* sTGCHand = dynamic_cast<sTGCHandler*>
      (c.GetHandlerStore().GetHandler("sTGC"));
    if (!sTGCHand) std::abort();

    sTGCDetectorDescription* sTGC = sTGCHand->GetsTGCCurrent();
    // std::cout<<" this is sTGC_readoutHandler::Handle"<<sTGC->GetName()<<std::endl;

    bool ret=true;
    sTGC->roParameters.sPadWidth           = getAttributeAsDouble    (c, t, "sPadWidth", ret);
    if (! ret) std::cout << "WARNING: sPadWidth is missing" << std::endl;

    sTGC->roParameters.lPadWidth           = getAttributeAsDouble    (c, t, "lPadWidth", ret);
    if (! ret) std::cout << "WARNING: lPadWidth is missing" << std::endl;

    sTGC->roParameters.sStripWidth           = getAttributeAsDouble    (c, t, "sStripWidth", ret);
    if (! ret) std::cout << "WARNING: sStripWidth is missing" << std::endl;

    sTGC->roParameters.lStripWidth           = getAttributeAsDouble    (c, t, "lStripWidth", ret);
    if (! ret) std::cout << "WARNING: lStripWidth is missing" << std::endl;

    sTGC->roParameters.padH                = getAttributeAsVector    (c, t, "padH", ret);
    if (! ret) std::cout << "WARNING: padH is missing" << std::endl; 

    sTGC->roParameters.nPadPhi         = getAttributeAsIntVector (c, t, "nPadPhi", ret);
    if (! ret) { // Check for backwards compatibility
      std::cout << "INFO: nPadPhi is missing trying older format nPadX" << std::endl;
      sTGC->roParameters.nPadPhi       = getAttributeAsIntVector (c, t, "nPadX", ret);
      if (! ret) std::cout << "WARNING: nPadX is missing" << std::endl;
    }

    sTGC->roParameters.anglePadPhi            = getAttributeAsDouble    (c, t, "anglePadPhi", ret);
    if (! ret){
      std::cout << "INFO: anglePadPhi is missing trying older format anglePadX" << std::endl;
      sTGC->roParameters.anglePadPhi            = getAttributeAsDouble    (c, t, "anglePadX", ret);
      if (! ret) std::cout << "WARNING: anglePadX is missing" << std::endl;
    }

    sTGC->roParameters.firstPadPhiDivision_A = getAttributeAsVector    (c, t, "firstPadPhiDivision_A", ret);
    sTGC->roParameters.firstPadPhiDivision_C         = getAttributeAsVector    (c, t, "firstPadPhiDivision_C", ret);
    if (! ret){
      std::cout << "INFO: firstPadPhiDivision_C/A is missing trying older format firstPadPhi" << std::endl; 
      sTGC->roParameters.firstPadPhiDivision_A        = getAttributeAsVector    (c, t, "firstPadPhi", ret);
      sTGC->roParameters.firstPadPhiDivision_C        = getAttributeAsVector    (c, t, "firstPadPhi", ret);
      if (! ret) std::cout << "WARNING: firstPadPhi is missing" << std::endl;
    }



      sTGC->roParameters.PadPhiShift_A = getAttributeAsVector    (c, t, "PadPhiShift_A", ret);
      sTGC->roParameters.PadPhiShift_C = getAttributeAsVector    (c, t, "PadPhiShift_C", ret);
    if (! ret){
      std::cout << "INFO: PadPhiShift_C/A is missing trying older format firstPadPhi" << std::endl; 
      sTGC->roParameters.PadPhiShift_A        = getAttributeAsVector    (c, t, "firstPadPhi", ret);
      sTGC->roParameters.PadPhiShift_C        = getAttributeAsVector    (c, t, "firstPadPhi", ret);
      if (! ret) std::cout << "WARNING: firstPadPhi is missing" << std::endl;
    }

    sTGC->roParameters.nPadH               = getAttributeAsVector    (c, t, "nPadH", ret);
    if (! ret) std::cout << "WARNING: nPadH is missing" << std::endl;

    sTGC->roParameters.firstPadH           = getAttributeAsVector    (c, t, "firstPadH", ret);
    if (! ret) std::cout << "WARNING: firstPadH is missing" << std::endl;

    sTGC->roParameters.firstPadRow         = getAttributeAsIntVector (c, t, "firstPadRow", ret);
    if (! ret) std::cout << "WARNING: firstPadRow is missing" << std::endl;

    sTGC->roParameters.nWires              = getAttributeAsIntVector (c, t, "nWires", ret);
    if (! ret) std::cout << "WARNING: nWires is missing" << std::endl;

    sTGC->roParameters.wireCutout          = getAttributeAsVector (c, t, "wireCutout", ret);
    if (! ret) std::cout << "WARNING: wireCutout is missing" << std::endl;

    sTGC->roParameters.firstWire           = getAttributeAsVector    (c, t, "firstWire", ret);
    if (! ret) std::cout << "WARNING: firstWire is missing" << std::endl;

    sTGC->roParameters.wireGroupWidth      = getAttributeAsDouble    (c, t, "wireGroupWidth", ret);
    if (! ret) std::cout << "WARNING: wireGroupWidth is missing" << std::endl;

    sTGC->roParameters.nStrips             = getAttributeAsInt       (c, t, "nStrips", ret);
    if (! ret) std::cout << "WARNING: nStrips is missing" << std::endl;

    sTGC->roParameters.firstTriggerBand    = getAttributeAsIntVector (c, t, "firstTriggerBand", ret);
    if (! ret) std::cout << "WARNING: firstTriggerBand is missing" << std::endl;

    sTGC->roParameters.nTriggerBands       = getAttributeAsIntVector (c, t, "nTriggerBands", ret);
    if (! ret) std::cout << "WARNING: nTriggerBands is missing" << std::endl;

    sTGC->roParameters.firstStripInTrigger = getAttributeAsIntVector (c, t, "firstStripInTrigger", ret);
    if (! ret) std::cout << "WARNING: firstStripInTrigger is missing" << std::endl;

    sTGC->roParameters.firstStripWidth     = getAttributeAsVector    (c, t, "firstStripWidth", ret);
    if (! ret) std::cout << "WARNING: firstStripWidth is missing" << std::endl;

    sTGC->roParameters.StripsInBandsLayer1 = getAttributeAsIntVector (c, t, "StripsInBandsLayer1", ret);
    if (! ret) std::cout << "WARNING: StripsInBandsLayer1 is missing" << std::endl;

    sTGC->roParameters.StripsInBandsLayer2 = getAttributeAsIntVector (c, t, "StripsInBandsLayer2", ret);
    if (! ret) std::cout << "WARNING: StripsInBandsLayer2 is missing" << std::endl;

    sTGC->roParameters.StripsInBandsLayer3 = getAttributeAsIntVector (c, t, "StripsInBandsLayer3", ret);
    if (! ret) std::cout << "WARNING: StripsInBandsLayer3 is missing" << std::endl;

    sTGC->roParameters.StripsInBandsLayer4 = getAttributeAsIntVector (c, t, "StripsInBandsLayer4", ret);
    if (! ret) std::cout << "WARNING: StripsInBandsLayer4 is missing" << std::endl;

    sTGC->roParameters.nWireGroups         = getAttributeAsIntVector (c, t, "nWireGroups", ret);
    if (! ret) std::cout << "WARNING: nWireGroups is missing" << std::endl;

    sTGC->roParameters.firstWireGroup      = getAttributeAsIntVector (c, t, "firstWireGroup", ret);
    if (! ret) std::cout << "WARNING: firstWireGroup is missing" << std::endl;
}
