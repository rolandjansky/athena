/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGGING_IBTAGTRACKASSOCIATION_H
#define BTAGGING_IBTAGTRACKASSOCIATION_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODJet/Jet.h"
#include "xAODTracking/TrackParticleContainerFwd.h"
#include "xAODJet/JetContainer.h"
#include "xAODBTagging/BTaggingContainer.h"

#include <vector>

namespace Analysis
{

  static const InterfaceID IID_IBTagTrackAssociation("IBTagTrackAssociation", 1, 0);

  class IBTagTrackAssociation :
                        virtual public IAlgTool 
  {
   public:

       /** Virtual destructor */
       virtual ~IBTagTrackAssociation(){};

       typedef std::vector<xAOD::Jet* > jetcollection_t;

       /** AlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_IBTagTrackAssociation; };

       virtual StatusCode initialize() = 0;
       virtual StatusCode finalize() = 0;
       virtual StatusCode BTagTrackAssociation_exec(jetcollection_t* theJets, const xAOD::TrackParticleContainer* tracks = 0) const = 0;
       virtual StatusCode BTagTrackAssociation_exec(const xAOD::JetContainer * jetContainer, xAOD::BTaggingContainer* btaggingContainer) const = 0;

  };

} // End namespace
#endif
