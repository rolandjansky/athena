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
//#include "src/TrackTruthSelectionTool.h"

#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"
#include "xAODTracking/TrackParticle.h"
#include "InDetPhysValMonitoring/IInDetPhysValDecoratorTool.h"

//fwd declaration
class IInDetPhysValDecoratorTool;
class InDetRttLargeD0Plots;


/**
 * Tool to book and fill inner detector histograms for physics validation
 */
class InDetPhysValLargeD0Tool:public ManagedMonitorToolBase{
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
  InDetPhysValLargeD0Tool();
  ///TrackParticle container's name
  std::string m_trkParticleName;
  ///TruthParticle container's name
  std::string m_truthParticleName;
  ///Primary vertex container's name
  std::string m_vertexContainerName;
  ///EventInfo container name
  std::string m_eventInfoContainerName;
  
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
  
  ToolHandle<IAsgSelectionTool> m_truthSelectionTool;
  
  //bool m_TrackTruthSelectionTool;
  //ToolHandle<TrackTruthSelectionTool> m_TrackTruthSelectionTool;
  
  ///Jet Things
  std::string m_jetContainerName;
  float m_maxTrkJetDR;
  std::string m_folder;
  bool m_fillTIDEPlots;
  bool m_fillExtraTIDEPlots;
    
  template<class T>
    const T* getContainer( const std::string & containerName);
};

template<class T>
const T* InDetPhysValLargeD0Tool::getContainer(const std::string & containerName){
  const T * ptr = evtStore()->retrieve< const T >( containerName );
  if (!ptr) {
    ATH_MSG_ERROR("Container '"<<containerName<<"' could not be retrieved");
  }
  return ptr;
}

#endif
