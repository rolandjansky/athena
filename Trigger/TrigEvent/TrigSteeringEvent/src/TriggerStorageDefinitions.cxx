/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "TrigSteeringEvent/TrigSuperRoiCollection.h"
#include "TrigSteeringEvent/TrigOperationalInfoCollection.h"
#include "TrigSteeringEvent/TrigPassBitsCollection.h"
#include "TrigSteeringEvent/TrigPassFlagsCollection.h"
#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigT1Interfaces/RecJetRoI.h"
#include "TrigT1Interfaces/RecJetEtRoI.h"
#include "TrigT1Interfaces/RecEnergyRoI.h"
#include "TrigT1Interfaces/RecMuonRoI.h"

#include "TrigNavigation/TypeRegistration.h"

// Avoid checkreq warning.
#include "xAODTrigger/TrigPassBitsContainer.h"


REGISTER_PACKAGE_WITH_NAVI(TrigSteeringEvent)




REGISTER_PACKAGE_WITH_NAVI(TrigCombinedEvent)


#endif //XAOD_ANALYSIS
