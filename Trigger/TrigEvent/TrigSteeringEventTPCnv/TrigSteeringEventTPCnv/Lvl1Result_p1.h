/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigSteeringEventTPCnv
 * @Class  : Lvl1Result_p1
 *
 * @brief  persistent representation of Lvl1ResultData.
 *
 * @author Andrew Hamilton <Andrew.Hamilton@cern.ch> - U. of Geneva, Switzerland
 * @author Francesca Bucci <f.bucci@cern.ch>         - U. of Geneva, Switzerland
 *
 * File and Version Information:
 * $Id: Lvl1Result_p1.h,v 1.2 2009-04-01 22:14:57 salvator Exp $
 **********************************************************************************/

#ifndef TrigSteeringEventTPCnv_Lvl1Result_p1_H
#define TrigSteeringEventTPCnv_Lvl1Result_p1_H

#include <vector>

#include "TrigSteeringEvent/Lvl1Result.h"


// needed to fix a Gaudi problem in SLC3 (should be removed eventually):
#include <stdint.h>

namespace LVL1CTP {

  class Lvl1Result_p1
  {
    friend class Lvl1ResultCnv;
    
  public:
    
    // default constructor
    Lvl1Result_p1() { }

    // default destructor
    virtual ~Lvl1Result_p1() { }

    //private:

    bool m_configured = false;
    std::vector<uint32_t> m_l1_items;           
    std::vector<uint32_t> m_l1_itemsPrescaled;  

  };

} // end of LVL1CTP namespace


#endif
