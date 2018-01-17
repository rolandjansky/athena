/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGGING_IBTAGSECVERTEXING_H
#define BTAGGING_IBTAGSECVERTEXING_H

#include "GaudiKernel/IAlgTool.h"
#include "VxVertex/RecVertex.h"            // Reconstructed vertex RecVertex
#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODTracking/VertexFwd.h"
#include "xAODTracking/VertexContainerFwd.h"
#include "xAODBTagging/BTagging.h"
#include "xAODBTagging/BTagVertexContainer.h"
#include "xAODBTagging/BTaggingContainer.h"

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
       virtual StatusCode BTagSecVtx_exec(xAOD::Jet& myJet, xAOD::BTagging*, xAOD::VertexContainer*, xAOD::BTagVertexContainer*, const xAOD::Vertex* vtx=0) const = 0;
       virtual StatusCode BTagSecVtx_exec(const xAOD::JetContainer * jetContainer, xAOD::BTaggingContainer * btaggingContainer) const = 0;
       virtual StatusCode finalize() = 0;
      

  };

} // End namespace
#endif
