/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Create by: joao.victor.da.fonseca.pinto@cern.ch, 2014-10-17
#ifndef XAODTRIGRINGER_VERSIONS_TRIGRINGERRINGS_V1_H
#define XAODTRIGRINGER_VERSIONS_TRIGRINGERRINGS_V1_H

// System include(s):
extern "C" {
  #include <stdint.h>
}

// xAOD include(s):
#include "AthContainers/AuxElement.h"
#include <vector>
//#include "CLIDSvc/CLASS_DEF.h"
//#include "GaudiKernel/MsgStream.h"

namespace xAOD{   
  class TrigRingerRings_v1 : public SG::AuxElement{
       
  public:
  
    /**
     * Class default constructor
     */
    TrigRingerRings_v1();
      
    /** Acessor methods **/
    const std::vector< float >& rings() const;
    long RoIword() const; // hold the roi word
 
    void setRings(const std::vector<float> &r);
    void setRoiWord(long w);
    unsigned size() const;
   
  };
} // namespace xAOD


#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::TrigRingerRings_v1, SG::AuxElement );

#endif
