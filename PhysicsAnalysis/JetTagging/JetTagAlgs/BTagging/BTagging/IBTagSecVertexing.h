/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGGING_IBTAGSECVERTEXING_H
#define BTAGGING_IBTAGSECVERTEXING_H

#include "GaudiKernel/IAlgTool.h"
#include "VxVertex/RecVertex.h"            // Reconstructed vertex RecVertex
#include "xAODJet/Jet.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODBTagging/BTagging.h"
#include "xAODBTagging/BTagVertexContainer.h"

class Jet;

namespace Analysis
{

  static const InterfaceID IID_IBTagSecVertexing("IBTagSecVertexing", 1, 0);

  class IBTagSecVertexing : virtual public IAlgTool 
  {
   public:

       /** Virtual destructor */
       virtual ~IBTagSecVertexing(){};

       /** AlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_IBTagSecVertexing; };

       virtual StatusCode initialize() = 0;
       virtual StatusCode BTagSecVtx_exec(xAOD::Jet& myJet, xAOD::BTagging*, xAOD::VertexContainer*, xAOD::BTagVertexContainer*) = 0;
       virtual StatusCode finalize() = 0;
      

  };

} // End namespace
#endif
