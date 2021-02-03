/*
 * Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#ifndef IDPERFMON_MUONSELECTOR_H
#define IDPERFMON_MUONSELECTOR_H

//==============================================================================
// Include files...
//==============================================================================
#include "InDetPerformanceMonitoring/EventAnalysis.h"

#include <map>
#include "TH1.h"

#include "xAODMuon/Muon.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include <atomic>
#include "GaudiKernel/ServiceHandle.h"
#include "AsgTools/ToolHandle.h" 
#include "MuonAnalysisInterfaces/IMuonSelectionTool.h"
#include "MuonMomentumCorrections/MuonCalibrationAndSmearingTool.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include "InDetPerformanceMonitoring/PerfMonServices.h"
//==============================================================================
// Forward class declarations...
//==============================================================================
class MuonSelector : public EventAnalysis
{
 public:
  MuonSelector();
  ~MuonSelector();


  // for some cases one may need ATLAS_NOT_REENTRANT 
  bool passSelection ( const xAOD::Muon* pxMuon );
  void setDebug(bool debug){m_doDebug = debug;}
  ToolHandle<CP::IMuonSelectionTool> m_muonSelectionTool;
 
  // Override functions from EventAnalysis
  virtual void  Init();
  inline void   doIsoSelection (bool doIso) {m_doIsoSelection = doIso;}
  inline void   doIPSelection (bool doIPsel) {m_doIPSelection = doIPsel;}
  inline void   doMCPSelection (bool domcp) {m_doMCPSelection = domcp;}
  virtual void  finalize();
  inline void   SetPtCut (double newvalue) {m_combPtCut = newvalue;}
  inline double GetPtCut ()                         {return m_combPtCut;}
  virtual bool  Reco();
  void          SetMuonQualityRequirement (std::string newname);

 protected:
  virtual void BookHistograms();

 private:
  typedef EventAnalysis PARENT;

  //static unsigned int s_uNumInstances;
  unsigned int m_uNumInstances;

  bool passQualCuts ();
  bool passPtCuts   ();
  bool passIsolCuts ();
  bool passIPCuts   ();

  // message stream
  MsgStream * m_msgStream;

  // Class variables
  const xAOD::Muon*   m_pxMuon;

  // requested muon tag (tight, medium, loose..)
  int m_requestedMuonQuality{};

  //  TrackIsolationTool* m_isolationTool;
  double m_coneSize{};


  // Cut variables. To go into a struct.
  unsigned char m_ucJMuon_Cut;
  double        m_IsoCut;

  bool  m_bCutOnCombKine;
  float m_fEtaCut;
  float m_fPtCut;

  double m_combPtCut;
  float m_ptMSCut;
  float m_diffZCut;  // mm
  float m_diffPtCut;
  float m_pVZCut;  // mm


  float m_fIDPtCut;
  float m_fIDChiPerDofCut;
  int   m_IDSiHitsCut;
  unsigned char m_ucID_PIXCut;
  unsigned char m_ucID_SCTCut;
  unsigned char m_ucID_TRTCut;

  bool m_doDebug;
  bool m_doQualSelection;
  bool m_doIsoSelection;
  bool m_doPtSelection;
  bool m_doIPSelection;
  bool m_doMCPSelection;

  // Lock cut selection after first muon.
  bool m_bLock;

  // stats
  unsigned int m_testedmuons{};
  unsigned int m_passqual{};
  unsigned int m_passiso{};
  unsigned int m_passpt{};
  unsigned int m_passip{};
  unsigned int m_passmcp{};
  unsigned int m_passall{};

  // Graphs
  enum HISTOS_1D
  {
    NMDT, NCSC, ECONE,
    NUM_1HISTOS
  };

};

#endif
