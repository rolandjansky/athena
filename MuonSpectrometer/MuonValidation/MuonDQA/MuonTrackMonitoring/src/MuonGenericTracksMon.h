/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonGenericTracksMon_H
#define MuonGenericTracksMon_H

// MuonGenericTracksMon.cxx
// AUTHORS: N. Benekos, E. Christidi, A. Eppig, Tony Liss

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolHandle.h"
//#include "GaudiKernel/SmartDataPtr.h"
#include "StoreGate/StoreGate.h"
 
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

#include "RecoMuonPlots.h"
#include "RecoMuonSegmentPlots.h"
#include "RecoMuonTrackPlots.h"
#include "MuonHistUtils/MuonEnumDefs.h"
 
#include "TrigConfL1Data/TriggerItem.h"
#include "TrigDecisionInterface/ITrigDecisionTool.h"
 
#include "TrkToolInterfaces/IResidualPullCalculator.h"
 	
//#include "MuonReadoutGeometry/MuonDetectorManager.h"
//#include "MuonReadoutGeometry/MdtReadoutElement.h"
//#include "MuonReadoutGeometry/RpcReadoutElement.h"
//#include "MuonReadoutGeometry/TgcReadoutElement.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

#include "xAODTracking/TrackParticle.h"

//#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"

//#include "MuonDQAUtils/MuonDQAFitFunc.h"
#include "TProfile.h"
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

  void FillPullResid(RecoMuonTrackPlots *, const xAOD::TrackParticle*);

  TH1* m_hNSegments;
  
  TH1* m_hNMuonTracks;
  TH1* m_hNExtrapMuonTracks;
  TH1* m_hNMuons;

  //plot classes in vectors (one class per detector region, Global, BA, BC, EA, EC)
  std::vector<RecoMuonSegmentPlots*> m_oRecoMuonSegmentPlots;
  std::vector<RecoMuonTrackPlots*>   m_oRecoMuonTrackPlots;
  std::vector<RecoMuonTrackPlots*>   m_oRecoMuonExtrapTrackPlots;
  std::vector<RecoMuonPlots*>        m_oRecoMuonPlots;
  std::vector<RecoMuonPlots*>        m_oRecoMuonForwPlots;
  std::vector<RecoMuonPlots*>        m_oRecoMuonCaloPlots;
  
  
 
 private:

  std::string m_muonsName;                 //!< Reconstructed muon collection name
  std::string m_muonSegmentsName;                 //!< Reconstructed muon collection name
  std::string m_muonTracksName;                 //!< Reconstructed track collection name
  std::string m_muonExtrapTracksName;                 //!< Reconstructed track collection name
            
  StatusCode setupTools();
  StatusCode bookInMongroup(TH1* hist, MonGroup& mongroup);
  StatusCode bookInMongroup(HistData& hist, MonGroup& mongroup);
  StatusCode bookInMongroup(PlotBase& valPlots, MonGroup& mongroup); 
 
  bool isGoodSegment(const xAOD::MuonSegment* muSeg);
  Muon::EnumDefs::DetRegion getDetRegion(const xAOD::MuonSegment* muSeg);

  // Handle for the trig decision tool
  ToolHandle<Trig::ITrigDecisionTool> m_trigDecTool;
  // Trigger items
  bool m_useTrigger;  // user flag to switch trigger on or off
  //std::vector<std::string> m_MuonTriggerChainName;
  std::string m_MuonTriggerChainName;
  std::vector<std::string> m_muon_triggers;  


  StoreGateSvc* m_storeGate;
  //  ActiveStoreSvc* m_activeStore; 
 
  //AtlasDetectorID*  m_idHelper;          //!< Used to find out the sub-det from PRD->identify().
  
  /* const MuonGM::MuonDetectorManager* m_muonMgr; */
  /* const MdtIdHelper* m_mdtIdHelper;   */
  /* const RpcIdHelper* m_rpcIdHelper; */
  /* const TgcIdHelper* m_tgcIdHelper;   */
  
  
     
  // Tool handles
  ToolHandle<Trk::IResidualPullCalculator> m_pullCalculator;     //<! tool to calculate residuals and pulls
  ToolHandle<Muon::MuonEDMHelperTool> m_helperTool;
  //  ToolHandle<Muon::MuonEDMPrinterTool> m_printer;
  ToolHandle<Muon::MuonIdHelperTool> m_idHelperTool;
  //  ToolHandle<Muon::MuonDQAFitFunc> m_muondqafitfunc;

  /** MDT segment specific cuts for barrel*/
  const double m_bMdtchi2max = 12.0;
  const int    m_bMdtnhitsmin = 5;
  /** MDT segment specific cuts for endcap*/
  const double m_eMdtchi2max = 20.0;
  const int    m_eMdtnhitsmin = 5;
  /** CSC segment specific cuts for endcap*/
  const double m_eCscchi2max = 20.0;
  const int    m_eCscnhitsmin = 4;

  std::string pathToHistName(std::string str){
    std::replace( str.begin(), str.end(), '/', '_');
    return str;
  }
  
};

#endif
 


