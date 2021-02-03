/*
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#ifndef IDPERFMON_ELECTRONSELECTOR_H
#define IDPERFMON_ELECTRONSELECTOR_H

//==============================================================================
// Include files...
//==============================================================================
#include "InDetPerformanceMonitoring/EventAnalysis.h"

#include <map>
#include "TH1.h"

#include "xAODMuon/Muon.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"

#include "GaudiKernel/ServiceHandle.h"
#include "AsgTools/ToolHandle.h" 
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronContainer.h"
#include "ElectronPhotonSelectorTools/AsgElectronLikelihoodTool.h"

#include <atomic>
//==============================================================================
// Forward class declarations...
//==============================================================================
class ElectronSelector : public EventAnalysis
{
 public:
  ElectronSelector();
  ~ElectronSelector();

  void setDebug (bool debug) {m_doDebug = debug;}
 
  const xAOD::TrackParticle*        GetElecNegTrackParticle (size_t i);
  const xAOD::TrackParticle*        GetElecPosTrackParticle (size_t i);
  inline unsigned int               GetElectronCollectionSize() {return m_goodElecNegTrackParticleList.size() + m_goodElecPosTrackParticleList.size();}
  // Override functions from EventAnalysis
  void                              Init();
  void                              PrepareElectronList (const xAOD::ElectronContainer* pxElecContainer);
  bool                              RecordElectron (const xAOD::Electron *);
  inline void                       SetPtCut (float newpt) {m_ptCut = newpt;}

 protected:
  // virtual void BookHistograms();

 private:
  typedef EventAnalysis PARENT;

  static std::atomic<unsigned int> s_uNumInstances;

  // functions 
  void   Clear();
  bool   OrderElectronList ();
  bool   RetrieveVertices ();

  // message stream
  MsgStream * m_msgStream;

  // Class variables
  const xAOD::Muon*           m_pxElectron;
  std::vector<const xAOD::TrackParticle*>  m_pxElTrackList; 
  std::vector<const xAOD::TrackParticle*>  m_goodElecNegTrackParticleList; 
  std::vector<const xAOD::TrackParticle*>  m_goodElecPosTrackParticleList; 

  // 
  bool m_doDebug;
  // 
  float m_ptCut;
  float m_etaCut;
  float m_deltaXYcut; //cut for vertex distance in X or Y 
  float m_deltaZcut; //cut for vertex distance in Z

  // Electron likelihood tool:
  AsgElectronLikelihoodTool* m_LHTool2015; //!

  // 
  int m_elecneg1;
  int m_elecneg2;
  int m_elecpos1;
  int m_elecpos2;

  // MeV to GeV conversion
  const float m_CGeV =  1.0e-3;  // Conversion factor to remove evil MeV nonsense.

};

#endif

