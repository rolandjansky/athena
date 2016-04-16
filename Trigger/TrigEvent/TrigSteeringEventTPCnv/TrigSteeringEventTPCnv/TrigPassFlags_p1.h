/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigSteeringEventTPCnv
 * @Class  : TrigPassFlags_p1
 *
 * @brief persistent partner for TrigPassFlags
 *
 *
 * File and Version Information:
 * $Id: 
 **********************************************************************************/
#ifndef TRIGSTEERINGEVENTTPCNV_TRIGPASSFLAGS_P1_H
#define TRIGSTEERINGEVENTTPCNV_TRIGPASSFLAGS_P1_H
#include <stdint.h>

class TrigPassFlags_p1
{
public:
   TrigPassFlags_p1() {}
   friend class TrigPassFlagsCnv_p1;

private:
   unsigned int m_nObjects = 0;
   unsigned short int m_nFlags = 0;
   std::vector<uint32_t> m_serialized;
};

#endif
