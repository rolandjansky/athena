/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGOUTPUTHANDLING_TRIGGEREDMCLIDS_H
#define TRIGOUTPUTHANDLING_TRIGGEREDMCLIDS_H

#include "AthenaKernel/CLASS_DEF.h"
#include "AthLinks/ElementLink.h"

#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#include "xAODBTagging/BTaggingContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODBTagging/BTagVertexContainer.h"
#include "xAODTrigger/jFexTauRoIContainer.h"

/**
 * @File TriggerEDMCLIDs.h
 * @brief Declaration of additional types which can be serialised.
 * This is primarily here for the benefit of non-trivial Dynamic Aux variables
 **/

CLASS_DEF( std::vector<ElementLink<TrigRoiDescriptorCollection> >, 78044011, 1 )
CLASS_DEF( std::vector<ElementLink<xAOD::BTaggingContainer>>, 1289383075, 1 )
CLASS_DEF( std::vector<std::vector<ElementLink<xAOD::TrackParticleContainer>>>, 1188795373, 1 )
CLASS_DEF( std::vector<std::vector<ElementLink<xAOD::MuonContainer> > >, 1196568576, 1 )
CLASS_DEF( std::vector<ElementLink<xAOD::JetContainer>>, 1210667800, 1 )
CLASS_DEF( std::vector<std::vector<ElementLink<xAOD::VertexContainer>>>, 1164463855, 1 )
CLASS_DEF( std::vector<std::vector<ElementLink<xAOD::BTagVertexContainer>>>, 1289535397, 1 )
CLASS_DEF( std::vector<ElementLink<xAOD::jFexTauRoIContainer>>, 1334654121, 1 )

#endif //> !TRIGOUTPUTHANDLING_TRIGGEREDMCLIDS_H
