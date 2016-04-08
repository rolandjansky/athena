/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Create by: joao.victor.da.fonseca.pinto@cern.ch, 2015-01-10
#ifndef XAODTRIGRINGER_VERSIONS_TRIGRINGERRINGS_V2_H
#define XAODTRIGRINGER_VERSIONS_TRIGRINGERRINGS_V2_H

// System include(s):
extern "C" {
  #include <stdint.h>
}

// xAOD include(s):
#include "AthLinks/ElementLink.h"
#include "AthContainers/AuxElement.h"
#include <vector>
//#include "CLIDSvc/CLASS_DEF.h"
//#include "GaudiKernel/MsgStream.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"

namespace xAOD{   
  class TrigRingerRings_v2 : public SG::AuxElement{
       
  public:
  
    /**
     * Class default constructor
     */
    TrigRingerRings_v2();
      
    /** Acessor methods **/
    const std::vector< float >& rings() const;
    void setRings(const std::vector<float> &r);
    unsigned size() const;

    /// Type of the EM cluster link
    typedef ElementLink< TrigEMClusterContainer > EMClusterLink_t; 
    /// The associated EM cluster, as a simple pointer
    const TrigEMCluster* emCluster() const;
    /// The associated EM cluster, as an ElementLink
    const EMClusterLink_t& emClusterLink() const;
    /// Set the ElementLink pointing to the EM cluster constituent
    void setEmClusterLink( const EMClusterLink_t& value );

  };
} // namespace xAOD

#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::TrigRingerRings_v2, SG::AuxElement );

#endif // XAODTRIGRINGER_VERSIONS_TRIGRINGERRINGS_V2_H

