// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGRINGER_VERSIONS_TRIGRNNOUTPUTAUXCONTAINER_V2_H
#define XAODTRIGRINGER_VERSIONS_TRIGRNNOUTPUTAUXCONTAINER_V2_H

// System include(s):
extern "C" {
  #include <stdint.h>
}

// STL include(s):
#include <vector>
#include "AthLinks/ElementLink.h"

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"
#include "xAODTrigRinger/TrigRingerRingsContainer.h"

namespace xAOD {
/// This is a fixed implementation of the trigger TrigRNNOutput
/// auxiliary store. Defining every possible property that TrigRNNOutput
///
/// @author Joao victor da Fonseca Pinto <joao.victor.da.fonseca.pinto@cern.ch>
///
/// $Revsision$
/// $Date: 2015-01-10$
///
   class TrigRNNOutputAuxContainer_v2: public AuxContainerBase{

   public:
      /// Default constructor
      TrigRNNOutputAuxContainer_v2();

   private:
      std::vector< std::vector<float> > rnnDecision; // Hold the decision
      std::vector< ElementLink<xAOD::TrigRingerRingsContainer_v2> > ringerLink;
   
   }; 
} // namespace xAOD

#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::TrigRNNOutputAuxContainer_v2, xAOD::AuxContainerBase );


#endif // XAODTRIGRINGER_VERSIONS_TRIGRNNOUTPUTAUXCONTAINER_V2_H
