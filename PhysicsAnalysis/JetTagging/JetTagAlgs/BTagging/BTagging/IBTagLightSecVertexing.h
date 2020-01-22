/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGGING_IBTAGLIGHTSECVERTEXING_H
#define BTAGGING_IBTAGLIGHTSECVERTEXING_H

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

  static const InterfaceID IID_IBTagLightSecVertexing("IBTagLightSecVertexing", 1, 0);

  class IBTagLightSecVertexing : virtual public IAlgTool 
  {
   public:

       /** Virtual destructor */
       virtual ~IBTagLightSecVertexing(){};

       /** AlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_IBTagLightSecVertexing; };

       virtual StatusCode initialize() = 0;
       virtual StatusCode BTagSecVertexing_exec(const xAOD::JetContainer * jetContainer, xAOD::BTaggingContainer * btaggingContainer) const = 0;

  };

} // End namespace
#endif
