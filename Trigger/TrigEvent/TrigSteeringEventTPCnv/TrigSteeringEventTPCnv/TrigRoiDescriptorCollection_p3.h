/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigSteeringEventTPCnv
 * @Class  : TrigRoiDescriptorCollection_p3
 *
 * @brief persistent partner for TrigRoiDescriptorCollection_p3
 *
 *
 * File and Version Information:
 * $Id: 
 **********************************************************************************/
#ifndef TRIGSTEERINGEVENTTPCNV_TRIGROIDESCRIPTORCOLLECTION_P3_H
#define TRIGSTEERINGEVENTTPCNV_TRIGROIDESCRIPTORCOLLECTION_P3_H

#include "TrigSteeringEventTPCnv/TrigRoiDescriptor_p3.h"
#include <vector>
  
class TrigRoiDescriptorCollection_p3 : public std::vector<TrigRoiDescriptor_p3>
{};// end of class definitions
 
 
#endif
