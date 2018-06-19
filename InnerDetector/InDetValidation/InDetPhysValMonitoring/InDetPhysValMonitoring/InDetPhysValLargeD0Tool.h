/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_InDetPhysValLargeD0Tool_H
#define INDETPHYSVALMONITORING_InDetPhysValLargeD0Tool_H
/**
 * @file InDetPhysValLargeD0Tool.h
 * header file for class of same name
 * @author siin che
 * @date 19 December 2015
**/
//STL includes
#include <string>

//#gaudi includes
#include "GaudiKernel/ToolHandle.h"

//local include
#include "PATCore/IAsgSelectionTool.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "InDetPhysValMonitoring/IAthSelectionTool.h"
//#include "src/TrackTruthSelectionTool.h"

#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODEventInfo/EventInfo.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"
#include "xAODTracking/TrackParticle.h"

//fwd declaration
class IInDetPhysValDecoratorTool;
class InDetRttLargeD0Plots;


/**
 * Tool to book and fill inner detector histograms for physics validation
 */
class InDetPhysValLargeD0Tool:public ManagedMonitorToolBase{
  InDetPhysValLargeD0Tool() = delete;
 public:
  ///Constructor with parameters
  InDetPhysValLargeD0Tool(const std::string & type, const std::string & name, const IInterface* parent);
  ///Destructor
  virtual ~InDetPhysValLargeD0Tool();
  /** \name BaseclassMethods Baseclass methods reimplemented 
      }**/
  //@{
  virtual StatusCode initialize();
  virtual StatusCode bookHistograms();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();
  
  // * @asogaard
  bool isSignal       (const xAOD::TruthParticle* p);
  bool isLargeD0Track (const xAOD::TrackParticle* p);
  bool isPrimaryTrack (const xAOD::TruthParticle* p);
  bool MinTrackSelection (const xAOD::TrackParticle* p);
  // * Remove the following.
  bool isCameFromLLP(const xAOD::TruthParticle* p);
  bool isLLP(const int absPdgId);
  bool hasNoLLP(const xAOD::TruthParticle* p);
  //@}
 private:
  ///prevent default construction
  ///TrackParticle container's name
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_trkParticleName
    {this, "TrackParticleContainerName", "InDetTrackParticles", "" };
  ///TruthParticle container's name
  SG::ReadHandleKey<xAOD::TruthParticleContainer> m_truthParticleName
    {this, "TruthParticleContainerName",  "TruthParticles", "" };
  ///Primary vertex container's name
  SG::ReadHandleKey<xAOD::VertexContainer> m_vertexContainerName
    {this, "VertexContainerName", "PrimaryVertices", "" };
  ///EventInfo container name
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoContainerName
    {this, "EventInfoContainerName", "EventInfo", "" };

  /* @asogaard */
  std::string      m_LLP;
  std::vector<int> m_signalIds; 
  
  ///histograms
  std::unique_ptr< InDetRttLargeD0Plots > m_LargeD0Plots;
  ///Tool for selecting tracks
  bool m_useTrackSelection;
  bool m_onlyInsideOutTracks;
  //ToolHandle<InDet::IInDetTrackSelectionTool> m_trackSelectionTool;
  ToolHandle<IAsgSelectionTool> m_trackSelectionTool; // @asogaard
  ToolHandle<IAthSelectionTool> m_truthSelectionTool;
  
  //bool m_TrackTruthSelectionTool;
  //ToolHandle<TrackTruthSelectionTool> m_TrackTruthSelectionTool;
  
  ///Jet Things
  std::string m_jetContainerName;
  float m_maxTrkJetDR;
  std::string m_folder;
  bool m_fillTIDEPlots;
  bool m_fillExtraTIDEPlots;
};

#endif
