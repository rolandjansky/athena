/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Create by: joao.victor.da.fonseca.pinto@cern.ch, 2014-10-17
#ifndef XAODTRIGRINGER_VERSIONS_TRIGRNNOUTPUT_V2_H
#define XAODTRIGRINGER_VERSIONS_TRIGRNNOUTPUT_V2_H

// System include(s):
extern "C" {
  #include <stdint.h>
}

// xAOD include(s):
#include "AthLinks/ElementLink.h"
#include "AthContainers/AuxElement.h"
#include "xAODTrigRinger/TrigRingerRings.h"
#include "xAODTrigRinger/TrigRingerRingsContainer.h"
// std
#include <vector>


namespace xAOD{   
  class TrigRNNOutput_v2 : public SG::AuxElement{
    
  public:
    /**
     * Class default constructor
     */
    TrigRNNOutput_v2();
      
    /** Acessor methods **/
    const std::vector<float>& rnnDecision() const;    
    void setRnnDecision(const std::vector<float> &d);
    void setRnnDecision(float d);
    /// Type of the EM cluster link
    typedef ElementLink< TrigRingerRingsContainer > RingerLink_t; 
    /// The associated EM cluster, as a simple pointer
    const TrigRingerRings* ringer() const;
    /// The associated EM cluster, as an ElementLink
    const RingerLink_t& ringerLink() const;
    /// Set the ElementLink pointing to the EM cluster constituent
    void setRingerLink( const RingerLink_t& value );

  };
} // namespace xAOD

#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::TrigRNNOutput_v2, SG::AuxElement );

#endif // XAODTRIGRINGER_VERSIONS_TRIGRNNOUTPUT_V2_H


