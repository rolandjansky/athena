/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// JetQGTaggerVariableTool.h

#ifndef JETMOMENTTOOLS_JETQGTAGGERVARIABLETOOL_H
#define JETMOMENTTOOLS_JETQGTAGGERVARIABLETOOL_H

/// Properties:
///  VertexContainer - name of the vertex container
///  EventInfo - name of EventInfo container
///  AssociatedTracks - name for attribute holding the list of associated tracks
///  TVATool - tool to do track-vertex association
///  TrkSelTool - tool to select tracks (none ==> no selection)

#include "AsgTools/ToolHandle.h"
#include "AsgTools/AsgTool.h"
#include "AsgTools/PropertyWrapper.h"
#include "AsgDataHandles/ReadDecorHandleKey.h"
#include "AsgDataHandles/WriteDecorHandleKey.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "TrackVertexAssociationTool/ITrackVertexAssociationTool.h"

#include "xAODEventInfo/EventInfo.h"

#include "xAODTruth/TruthParticle.h"

#include "JetInterface/IJetDecorator.h"

#include <vector>
#include <string>

class JetQGTaggerVariableTool : public asg::AsgTool,
  virtual public IJetDecorator {
  ASG_TOOL_CLASS(JetQGTaggerVariableTool,IJetDecorator)

public:
  // Constructor from tool name
  JetQGTaggerVariableTool(const std::string& name);

  // Initialization.
  StatusCode initialize() override;

  // Inherited method to decorate a jet container
  virtual StatusCode decorate(const xAOD::JetContainer& jetCont) const override;

  // Local method to return the HS vertex - that of type PriVtx
  const xAOD::Vertex* findHSVertex(const xAOD::VertexContainer*&) const;

private:  // data

  // Configurable parameters
  Gaudi::Property<std::string> m_jetContainerName{this,"JetContainer", "", "SG key for the input jet container"};
  SG::ReadHandleKey<xAOD::VertexContainer> m_vertexContainer_key{this, "VertexContainer", "PrimaryVertices", "SG key for input vertex container"};
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfo_key{this, "EventInfo", "EventInfo", "SG key for input EventInfo"};

  ToolHandle<InDet::IInDetTrackSelectionTool> m_trkSelectionTool{this, "TrkSelTool", "Track selector tool"};
  ToolHandle<CP::ITrackVertexAssociationTool> m_trkVertexAssocTool{this, "TVATool", "Track-vertex association tool"};

  SG::WriteDecorHandleKey<xAOD::JetContainer> m_nTrkKey{this, "NTrksDecorName", "DFCommonJets_QGTagger_NTracks", "SG key for output NTracks decoration"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_trkWidthKey{this, "TracksWidthDecorName", "DFCommonJets_QGTagger_TracksWidth", "SG key for output TracksWidth decoration"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_trkC1Key{this, "TracksC1DecorName", "DFCommonJets_QGTagger_TracksC1", "SG key for output TracksC1 decoration"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_nChargedKey{this, "nChargedDecorName", "DFCommonJets_QGTagger_truthjet_nCharged", "SG key for output truthjet_nCharged decoration"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_truthPtKey{this, "truthPtDecorName", "DFCommonJets_QGTagger_truthjet_pt", "SG key for output truthjet_pt decoration"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_truthEtaKey{this, "truthEtaDecorName", "DFCommonJets_QGTagger_truthjet_eta", "SG key for output truthjet_eta decoration"};

};


#endif

