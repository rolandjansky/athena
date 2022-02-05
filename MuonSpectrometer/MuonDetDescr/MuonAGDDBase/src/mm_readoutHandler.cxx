/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAGDDBase/micromegasHandler.h"
#include "MuonAGDDBase/mm_readoutHandler.h"
#include "MuonAGDDDescription/MMDetectorDescription.h"
#include "AGDDControl/AGDDController.h"
#include "AGDDControl/XMLHandlerStore.h"
#include <iostream>


mm_readoutHandler::mm_readoutHandler(const std::string& s,
                                     AGDDController& c)
  : XMLHandler(s, c)
{
}

void mm_readoutHandler::ElementHandle(AGDDController& c,
                                      xercesc::DOMNode *t)
{
    micromegasHandler* mmHand = dynamic_cast<micromegasHandler*>
      (c.GetHandlerStore().GetHandler("micromegas"));
    if (!mmHand) std::abort();

    MMDetectorDescription* MM = mmHand->GetMMCurrent();
    // std::cout<<" this is mm_readoutHandler::Handle"<<MM->GetName()<<std::endl;

    bool ret=true;
    MM->roParameters.stripPitch            = getAttributeAsDouble    (c, t, "stripPitch", ret);
    if (! ret) std::cout << "WARNING: stripPitch is missing" << std::endl;

    MM->roParameters.gasThickness          = getAttributeAsDouble    (c, t, "gasThickness", ret);
    if (! ret) std::cout << "WARNING: gasThickness is missing" << std::endl;

    MM->roParameters.pcbThickness          = getAttributeAsDouble    (c, t, "pcbThickness", ret);
    if (! ret) std::cout << "WARNING: pcbThickness is missing" << std::endl;

    MM->roParameters.driftThickness        = getAttributeAsDouble    (c, t, "driftThickness", ret);
    if (! ret) std::cout << "WARNING: driftThickness is missing" << std::endl;

    MM->roParameters.stereoAngle           = getAttributeAsVector    (c, t, "stereoAngle", ret);
    if (! ret) std::cout << "WARNING: stereoAngle is missing" << std::endl;

    MM->roParameters.readoutSide           = getAttributeAsIntVector (c, t, "readoutSide", ret);
    if (! ret) std::cout << "WARNING: readoutSide is missing" << std::endl;

    MM->roParameters.zpos                  = getAttributeAsDouble    (c, t, "zPos", ret);
    if (! ret) std::cout << "WARNING: zPos is missing" << std::endl;

    MM->roParameters.distanceFromZAxis     = getAttributeAsDouble    (c, t, "distanceFromZAxis", ret);
    if (! ret) std::cout << "WARNING: distanceFromZAxis is missing" << std::endl;

    MM->roParameters.roLength              = getAttributeAsDouble    (c, t, "roLength", ret);
    if (! ret) std::cout << "WARNING: roLength is missing" << std::endl;

    MM->roParameters.activeBottomLength           = getAttributeAsDouble    (c, t, "activeBottomLength", ret);
    if (! ret) std::cout << "WARNING: activeBottomLength is missing" << std::endl;

    MM->roParameters.activeTopLength           = getAttributeAsDouble    (c, t, "activeTopLength", ret);
    if (! ret) std::cout << "WARNING: activeTopLength is missing" << std::endl;

    MM->roParameters.activeH               = getAttributeAsDouble    (c, t, "activeH", ret);
    if (! ret) std::cout << "WARNING: activeH is missing" << std::endl;

    MM->roParameters.minYPhiL               = getAttributeAsDouble    (c, t, "minYPhiL", ret);
    if (! ret) std::cout << "WARNING: minYPhiL is missing" << std::endl;
    
    MM->roParameters.minYPhiR               = getAttributeAsDouble    (c, t, "minYPhiR", ret);
    if (! ret) std::cout << "WARNING: minYPhiR is missing" << std::endl;

    MM->roParameters.maxYPhi               = getAttributeAsDouble    (c, t, "maxYPhi", ret);
    if (! ret) std::cout << "WARNING: maxYPhi is missing" << std::endl;
    
    MM->roParameters.dlStereoBottom               = getAttributeAsDouble    (c, t, "dlStereoBottom", ret);
    if (! ret) std::cout << "WARNING: dlStereoBottom is missing" << std::endl;
    
    MM->roParameters.dlStereoTop               = getAttributeAsDouble    (c, t, "dlStereoTop", ret);
    if (! ret) std::cout << "WARNING: dlStereoTop is missing" << std::endl;
    
    MM->roParameters.nMissedTopEta               = getAttributeAsInt    (c, t, "nMissedTopEta", ret);
    if (! ret) std::cout << "WARNING: nMissedTopEta is missing" << std::endl;
    
    MM->roParameters.nMissedBottomEta               = getAttributeAsInt    (c, t, "nMissedBottomEta", ret);
    if (! ret) std::cout << "WARNING: nMissedBottomEta is missing" << std::endl;
    
    MM->roParameters.nMissedTopStereo               = getAttributeAsInt    (c, t, "nMissedTopStereo", ret);
    if (! ret) std::cout << "WARNING: nMissedTopStereo is missing" << std::endl;
    
    MM->roParameters.nMissedBottomStereo              = getAttributeAsInt    (c, t, "nMissedBottomStereo", ret);
    if (! ret) std::cout << "WARNING: nMissedBottomStereo is missing" << std::endl;
    
    MM->roParameters.nRoutedTop               = getAttributeAsInt    (c, t, "nRoutedTop", ret);
    if (! ret) std::cout << "WARNING: nRoutedTop is missing" << std::endl;
    
    MM->roParameters.nRoutedBottom               = getAttributeAsInt    (c, t, "nRoutedBottom", ret);
    if (! ret) std::cout << "WARNING: nRoutedBottom is missing" << std::endl;
    
    MM->roParameters.tStrips             = getAttributeAsInt    (c, t, "totalStrips", ret);
    if (! ret) std::cout << "WARNING: totalStrips is missing" << std::endl;


}
