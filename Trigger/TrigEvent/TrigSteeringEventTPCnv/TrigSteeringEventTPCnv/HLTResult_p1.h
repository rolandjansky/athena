/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigSteeringEventTPCnv
 * @Class  : HLTResult_p1
 *
 * @brief  persistent representation of all three trigger level information - for now
 *         this is exactly what is in HLTResultData.
 *
 * @author Andrew Hamilton <Andrew.Hamilton@cern.ch> - U. of Geneva, Switzerland
 * @author Francesca Bucci <f.bucci@cern.ch>         - U. of Geneva, Switzerland
 *
 * File and Version Information:
 * $Id: HLTResult_p1.h,v 1.2 2009-04-01 22:14:57 salvator Exp $
 **********************************************************************************/

#ifndef TrigSteeringEventTPCnv_HLTResult_p1_H
#define TrigSteeringEventTPCnv_HLTResult_p1_H

#include <vector>

#include "TrigSteeringEvent/HLTResult.h"


// needed to fix a Gaudi problem in SLC3 (should be removed eventually):
#include <stdint.h>

namespace HLT {

  class HLTResult_p1
  {
    friend class HLTResultCnv;
    
  public:
    
    // default constructor
    HLTResult_p1() { }

    // default destructor
    virtual ~HLTResult_p1() { }

    std::vector<uint32_t>   m_headerResult;     
    std::vector<uint32_t>   m_chainsResult;     
    std::vector<uint32_t>   m_navigationResult; 

  };

} // end of HLT namespace


#endif
