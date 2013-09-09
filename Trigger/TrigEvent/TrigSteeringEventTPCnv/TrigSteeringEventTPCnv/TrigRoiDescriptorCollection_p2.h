/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigSteeringEventTPCnv
 * @Class  : TrigRoiDescriptorCollection_p2
 *
 * @brief persistent partner for TrigRoiDescriptorCollection_p2
 *
 *
 * File and Version Information:
 * $Id: 
 **********************************************************************************/
#ifndef TRIGSTEERINGEVENTTPCNV_TRIGROIDESCRIPTORCOLLECTION_P2_H
#define TRIGSTEERINGEVENTTPCNV_TRIGROIDESCRIPTORCOLLECTION_P2_H

#include "TrigSteeringEventTPCnv/TrigRoiDescriptor_p2.h"
#include <vector>
  
class TrigRoiDescriptorCollection_p2 : public std::vector<TrigRoiDescriptor_p2>
{};// end of class definitions
 
 
#endif
