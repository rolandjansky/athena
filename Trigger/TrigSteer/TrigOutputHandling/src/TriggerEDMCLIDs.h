/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGOUTPUTHANDLING_TRIGGEREDMCLIDS_H
#define TRIGOUTPUTHANDLING_TRIGGEREDMCLIDS_H

#include "AthenaKernel/CLASS_DEF.h"
#include "AthLinks/ElementLink.h"

#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

/**
 * @File TriggerEDMCLIDs.h
 * @brief Declaration of additional types which can be serialised.
 * This is primarily here for the benifit of non-trivial Dynamic Aux variabless
 **/

CLASS_DEF( std::vector<ElementLink<TrigRoiDescriptorCollection> >, 78044011, 1 )

#endif //> !TRIGOUTPUTHANDLING_TRIGGEREDMCLIDS_H
