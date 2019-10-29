/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#ifndef REC_HEADCALOEXTRAPOLATIONTOOL_H
#define REC_HEADCALOEXTRAPOLATIONTOOL_H
/**
  @class HeadCaloExtensionBuilder
  Based on EMGSFCaloExtensionBuilder
  Algorithm which creates calo extension for all
  Track Particles
  */
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include "TrkCaloExtension/CaloExtensionCollection.h"
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/Vertex.h"

#include "VxVertex/RecVertex.h"
#include "ITrackToVertex/ITrackToVertex.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"

namespace Trk 
{
  class HeadCaloExtensionBuilder : public AthAlgorithm//, virtual public ITrackSelectorTool
  {
  public:
  /** @brief Default constructor*/
  HeadCaloExtensionBuilder(const std::string& name, ISvcLocator* pSvcLocator);
  /** Default destructor */
  ~HeadCaloExtensionBuilder() {};

  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;
  virtual StatusCode execute() override final;

  private:
  /** @brief the Calo Extension tool*/
  ToolHandle<Trk::IParticleCaloExtensionTool> m_particleCaloExtensionTool {this,
      "LastCaloExtentionTool", "Trk::HeadCaloExtensionBuilderTool"};
  
  //Manages the track selection. It should be able to handle both pflow and tau selections
  ToolHandle<InDet::IInDetTrackSelectionTool> m_TrkSelection {this,
      "TrkSelection", "TrkSelectionHeadCalo", "Tool that handles the track selection"};

  ToolHandle<Trk::ITrackSelectorTool> m_TrkDetailedSelection {this,
      "TrkDetailedSelection", "TrkDetailedSelectionHeadCalo", "Tool that handles the detailed track selection"};

  //output particle calo extension collection
  SG::WriteHandleKey<CaloExtensionCollection>  m_ParticleCacheKey{this,
      "ParticleCache", "ParticleCaloExtension", "Name of the particle measurement extrapolation cache"};

  //input Track collection and vertex
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_TrkPartContainerKey {this,
      "TrkPartContainerName", "InDetTrackParticles", "Container of tracks"};

  //input Vertex collection
  SG::ReadHandleKey<xAOD::VertexContainer> m_vertexInputContainer{this,
      "vertexInputContainer", "PrimaryVertices", "input vertex container key"};

  };
}

#endif //