/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//  TopoInputEvent.h
//  L1TopoCoreSimulation
//
//  Created by Joerg Stelzer on 11/21/12.
//

#ifndef __L1TopoCoreSimulation__TopoInputEvent__
#define __L1TopoCoreSimulation__TopoInputEvent__

#include <iostream>
#include <vector>

#include "L1TopoCoreSimulation/StatusCode.h"
#include "L1TopoCoreSimulation/ClusterTOBArray.h"
#include "L1TopoCoreSimulation/JetTOBArray.h"
#include "L1TopoCoreSimulation/Types.h"

namespace TCS {
   
   class ClusterTOB;
   class JetTOB;
   
   class TopoInputEvent {
   public:
      
      TopoInputEvent();
      ~TopoInputEvent();
      
      const ClusterTOBArray & clusters() const { return m_clusters; }
      const JetTOBArray & jets() const { return m_jets; }
      
      StatusCode add(const ClusterTOB & cluster);

      StatusCode add(const JetTOB & jet);

      const InputTOBArray * inputTOBs(TCS::inputTOBType_t) const;
      
      StatusCode clear();

   private:
      // data is owned by the TopoInputEvent
      ClusterTOBArray  m_clusters;
      JetTOBArray      m_jets;
   };
   
} // end of namespace TCS

std::ostream & operator<<(std::ostream &, const TCS::TopoInputEvent &);


#endif /* defined(__L1TopoCoreSimulation__TopoInputEvent__) */
