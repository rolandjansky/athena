/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Create by: joao.victor.da.fonseca.pinto@cern.ch, 2014-10-17
#ifndef XAODTRIGRINGER_VERSIONS_TRIGRINGERRINGS_V1_H
#define XAODTRIGRINGER_VERSIONS_TRIGRINGERRINGS_V1_H

// local includes(s):
//#include "xAODRingerRings/RingerRings.h"
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
    std::vector< float > rings() const;
    long  RoIword() const; // hold the roi word
 
    void setRings(const std::vector<float> &r);
    void setRoIword(long w);
   unsigned size() const;
   
    //float &at(const unsigned int i);
    //const float &at(const unsigned int i) const;
    //const float &operator [](const unsigned int i) const;
    //float &operator [](const unsigned int i);
    
  };
} // namespace xAOD


#endif // XAODTRIGCALO_VERSIONS_RINGERRINGS_V1_H


