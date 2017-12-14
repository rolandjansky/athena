/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRKPHYSMONITORING_MUONGENERICTRACKSMON_H
#define MUONTRKPHYSMONITORING_MUONGENERICTRACKSMON_H

// MuonGenericTracksMon.cxx
// AUTHORS: N. Benekos, E. Christidi, A. Eppig, Tony Liss

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGate.h"
 
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

#include "MuonTrackMonitoring/RecoMuonPlots.h"
#include "MuonTrackMonitoring/RecoMuonSegmentPlots.h"
#include "MuonTrackMonitoring/RecoMuonTrackPlots.h"
#include "MuonTrackMonitoring/RecoMuonIDTrackPlots.h"
#include "MuonTrackMonitoring/RecoLumiPlots.h"
#include "MuonTrackMonitoring/RecoPhysPlots.h"
#include "MuonTrackMonitoring/RecoVertexPlots.h"
#include "MuonHistUtils/MuonEnumDefs.h"
 
#include "TrigConfL1Data/TriggerItem.h"
//#include "TrigDecisionInterface/ITrigDecisionTool.h"
 
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"

#include "xAODTracking/TrackParticle.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "FourMomUtils/P4Helpers.h"

#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "MuonSelectorTools/IMuonSelectionTool.h"
#include "MuonResonanceTools/IMuonResonanceSelectionTool.h"
#include "MuonResonanceTools/IMuonResonancePairingTool.h"

#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonSegment.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "xAODEventInfo/EventInfo.h"

#include "TProfile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TMath.h"
#include "TLorentzVector.h"
#include <vector>
#include <string>
#include <algorithm>
 
class MuonGenericTracksMon : public ManagedMonitorToolBase
{
 
 public:

  MuonGenericTracksMon( const std::string & type, const std::string & name, const IInterface* parent ); 
  virtual ~MuonGenericTracksMon(){;}
  
  StatusCode initialize();
  virtual StatusCode bookHistograms();   
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();  
  StatusCode finalize();

  //second argument is the souce type
  void plot_lumi(   std::vector<std::pair<const xAOD::Muon*, const xAOD::Muon*> > resonances_Z, 
    std::vector<std::pair<const xAOD::Muon*, const xAOD::Muon*> > resonances_jpsi, 
    const xAOD::MuonContainer* Muons,
    const xAOD::TrackParticleContainer*   tracksMS, 
    const xAOD::MuonSegmentContainer* MuonSegments);
  void plot_lumi_notrig(const xAOD::MuonContainer* Muons, 
    const xAOD::TrackParticleContainer*   tracksMS, 
    const xAOD::MuonSegmentContainer* MuonSegments);
  //other plots
  void plot_muon(   const xAOD::Muon&          muon,    int source);
  void plot_muon_notrig(const xAOD::Muon&        muon,    int source);
  void plot_segment(const xAOD::MuonSegment&   segment, int source);
  void plot_track(  const xAOD::TrackParticle& track,   int source);
  void plot_vertex( const xAOD::Vertex&        aVx,     int source);
  void plot_resonances(std::vector<std::pair<const xAOD::Muon*, const xAOD::Muon*> > resonances, int source);

  void FillPullResid(RecoMuonTrackPlots *, const xAOD::TrackParticle*);
  
  float m_inst_lumi_bcid;
  float m_inst_lumi_lb;
  int   m_current_lb;

  TH1* m_hNEvent;//a sample histogram to count the number of events

  // plot classes in vectors (one class per selection: Z, JPsi, all)
  std::vector<RecoLumiPlots*>         m_oRecoLumiPlots;
  std::vector<RecoMuonSegmentPlots*>  m_oRecoMuonSegmentPlots;
  std::vector<RecoMuonTrackPlots*>    m_oRecoMuonMSTrackPlots;
  std::vector<RecoMuonTrackPlots*>    m_oRecoMuonMETrackPlots;
  std::vector<RecoMuonIDTrackPlots*>  m_oRecoMuonIDTrackPlots;
  std::vector<RecoMuonPlots*>         m_oRecoMuonPlots;
  //std::vector<RecoMuonPlots*>         m_oRecoMuonForwPlots;
  //std::vector<RecoMuonPlots*>         m_oRecoMuonCaloPlots;
  std::vector<RecoPhysPlots*>         m_oRecoPhysPlots;
  std::vector<RecoVertexPlots*>       m_oRecoVertexPlots;

  
 protected:

  //
    
 private:
  
  StoreGateSvc* m_storeGate;
  std::string m_muonsName;
  std::string m_muonSegmentsName;
  std::string m_muonTracksName;
  std::string m_msVertexCollection;
  std::string m_muonExtrapTracksName;
  std::string m_innerTracksName;
            
  StatusCode setupTools();
  StatusCode bookInMongroup(TH1* hist, MonGroup& mongroup);
  StatusCode bookInMongroup(HistData& hist, MonGroup& mongroup, std::string source);
  StatusCode bookInMongroup(PlotBase& valPlots, MonGroup& mongroup, std::string source);
  StatusCode bookInMongroup(PlotBase& valPlots, MonGroup& mongroup, std::string source, TString Montype);

  // define the different classes of plots;
  enum SOURCE {Z = 0, JPSI, CBMUONS, NONCBMUONS, CONTAINER, N_SOURCE};
  std::string sources[SOURCE::N_SOURCE + 1] = {"Z", "Jpsi", "CBMuons", "NonCBMuons", "Container", "N_SOURCE"};
  enum MUON_COMPONENT {TRACK_MS=0, TRACK_ME, TRACK_ID, N_COMPONENTS};
  // Trigger items
  bool m_useTrigger; 
  std::string m_MuonTriggerChainName;
  std::vector<std::string> m_muon_triggers;  
        
  // ATLAS Detector Description
  // Handle for the trig decision tool
  ToolHandle<Trig::ITrigDecisionTool> m_trigDecTool;
  // ToolHandle idHelper
  //ToolHandle<Trk::IResidualPullCalculator> m_pullCalculator;     //<! tool to calculate residuals and pulls
  //ToolHandle<Muon::MuonEDMHelperTool> m_helperTool;
  //ToolHandle<Muon::MuonIdHelperTool> m_idHelperTool;
  // MCP muon quality tool
  ToolHandle<CP::IMuonSelectionTool> m_muonSelectionTool;
  // MCP T&P helpers
  ToolHandle<IMuonResonanceSelectionTool> m_ZmumuResonanceSelectionTool;
  ToolHandle<IMuonResonancePairingTool>   m_ZmumuResonancePairingTool;
  ToolHandle<IMuonResonanceSelectionTool> m_JpsimumuResonanceSelectionTool;
  ToolHandle<IMuonResonancePairingTool>   m_JpsimumuResonancePairingTool;
    
  std::string pathToHistName(std::string str){
    std::replace( str.begin(), str.end(), '/', '_');
    return str;
  }
  // isMC required by MCP tools. 
  // only matters for scalefactors, i.e., keep isMC=false for SF=1.
  bool m_isMC;
  
};

#endif
 


