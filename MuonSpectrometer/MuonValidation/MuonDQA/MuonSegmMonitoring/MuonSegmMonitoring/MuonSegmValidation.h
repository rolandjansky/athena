/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonSegmValidation_H
#define MuonSegmValidation_H

// **********************************************************************
// MuonSegmValidation
// AUTHOR: I. Christidi - N. Benekos (UIUC)  
// **********************************************************************

/* // Gaudi Tools */
/* #include "GaudiKernel/StatusCode.h" */
/* #include "GaudiKernel/ToolHandle.h" */
/* #include "GaudiKernel/MsgStream.h" */
 
/* #include "AthenaMonitoring/AthenaMonManager.h" */
/* #include "AthenaMonitoring/ManagedMonitorToolBase.h" */
/* #include "AthenaBaseComps/AthAlgorithm.h" */

/* // Track */
/* #include "TrkParameters/TrackParameters.h" */
 
/* #include "MuonReadoutGeometry/MuonDetectorManager.h" */
/* #include "MuonReadoutGeometry/MdtReadoutElement.h" */
/* #include "MuonReadoutGeometry/RpcReadoutElement.h" */
/* #include "MuonReadoutGeometry/TgcReadoutElement.h" */

/* #include "TrkTrack/Track.h" */
/* #include "TrkTrack/TrackCollection.h" */

/* // ROOT Classes */
/* #include "TH1F.h" */
/* #include "TH2F.h" */
/* #include "TProfile.h" */

/* // */
/* #include "MuonSegment/MuonSegment.h" */
/* #include "TrkSegment/SegmentCollection.h" */

/* #include "MuonDQAUtils/MuonDQATrackHistTool.h" */
/* #include "MuonDQAUtils/MuonDQAFitFunc.h" */
/* #include "MuonDQAUtils/MuonDQAHistTool.h" */

/* // C++ */
/* #include <vector> */

/* class ActiveStoreSvc; */

/* namespace Trk { */
/*   class IResidualPullCalculator; */
/* } */

/* namespace Muon { */
/*   class MuonEDMPrinterTool; */
/*   class MuonEDMHelperTool; */
/*   class MuonIdHelperTool; */
/* } */

/* namespace Trk { */
/*   class IUpdator; */
/*   class IPropagator; */
/*   class RIO_OnTrack; */
/*   class Track; */
/*   class TrackStateOnSurface; */
/* } */
 
/* class MuonSegmValidation : public ManagedMonitorToolBase */
/* { */
/*  public: */
/*   /\** Constructor *\/ */
/*   MuonSegmValidation( const std::string & type, const std::string & name, const IInterface* parent ); */
/*   /\** Destructor *\/ */
/*   virtual ~MuonSegmValidation(); */
/*   /\** Histogram booking method *\/ */
/*   //virtual StatusCode bookHistograms (bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun ); */
/*   virtual StatusCode bookHistogramsRecurrent (); */
  
/*   /\** Histogram filling method *\/ */
/*   virtual StatusCode fillHistograms(); */
  
/*   //  virtual StatusCode procHistograms( bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun ); */
/*   virtual StatusCode procHistograms (); */
    
/*   StatusCode initialize(); */
/*   StatusCode finalize(); */

/*  protected: */
 
 
/*  private: */
         
/*   StoreGateSvc* m_storeGate; */
/*   ActiveStoreSvc* m_activeStore; */

/*   const MuonGM::MuonDetectorManager* m_muonMgr; */
/*   const MdtIdHelper* m_mdtIdHelper; */
/*   const RpcIdHelper* m_rpcIdHelper; */
/*   const CscIdHelper* m_cscIdHelper; */
/*   const TgcIdHelper* m_tgcIdHelper; */
/*   std::vector<std::string> m_segmCollectionName;   //!< Reconstructed segm collection name */
/*   std::vector<int> m_segmCollectionFlag; */
/*   bool m_doMDT; // subsystem flags */
/*   bool m_doRPC; */
/*   bool m_doTGC; */
/*   bool m_doCSC; */
/*   const Trk::SegmentCollection* m_segms;           //container for segms */
  
/*   int                    m_eventCounter; */
/*   int                    m_eventNumber; */
/*   int                    m_rec_nSoS; */
/*   static const int       s_maxEvents;            //!< Maximum number of events per run */
/*   static const int       s_maxSoS;               //!< Maximum number of SegmStateOnSurface objects associated to the segm */

/*   bool                   m_debuglevel;           //!< private member to control debug messages */
/*   bool                   b_status; */


/*   //  Muon Monitoring Histograms */
/*   Muon::MuonDQATrackHistTool::TrackHistos histos_from_tool_Moore; */
/*   Muon::MuonDQATrackHistTool::TrackHistos histos_from_tool_MBoy; */
/*   std::vector<Muon::MuonDQATrackHistTool::TrackHistos> histos_from_tool; */

/*   std::vector<TH1F*> mdt_pulls_BarrelA; */
/*   std::vector<TH1F*> mdt_pulls_BarrelC; */
/*   std::vector<TH1F*> mdt_pulls_ECA; */
/*   std::vector<TH1F*> mdt_pulls_ECC; */

/*   std::vector<TH2F*> mdt2_pulls_BarrelA; */
/*   std::vector<TH2F*> mdt2_pulls_BarrelC; */
/*   std::vector<TH2F*> mdt2_pulls_ECA; */
/*   std::vector<TH2F*> mdt2_pulls_ECC; */

/*   std::vector<TProfile*> mdt_hits_BarrelA; */
/*   std::vector<TProfile*> mdt_hits_BarrelC; */
/*   std::vector<TProfile*> mdt_hits_ECA; */
/*   std::vector<TProfile*> mdt_hits_ECC; */

/*   std::vector<TProfile*> chi2_BarrelA; */
/*   std::vector<TProfile*> chi2_BarrelC; */
/*   std::vector<TProfile*> chi2_ECA; */
/*   std::vector<TProfile*> chi2_ECC; */

/*   std::vector<TH1F*> nsegm_BarrelA; */
/*   std::vector<TH1F*> nsegm_BarrelC; */
/*   std::vector<TH1F*> nsegm_ECA; */
/*   std::vector<TH1F*> nsegm_ECC; */

/*   std::vector<TH1F*> m_num_segms; */
/*   std::vector<TH2F*> m_num_hits_per_segmvsphi; */
/*   std::vector<TH2F*> m_num_hits_per_segm_vs_station; */
/*   std::vector<TH2F*> chi2_vs_station; */

/*   std::vector<TH2F*> m_segm_etavsphi; */
/*   std::vector<TH2F*> m_segm_eta_vs_phi; */
/*   std::vector<TH2F*> m_segm_eta_vs_phiInner; */
/*   std::vector<TH2F*> m_segm_eta_vs_phiMiddle; */
/*   std::vector<TH2F*> m_segm_eta_vs_phiOuter; */
  
/*   // Tool handles */
/*   ToolHandle<Trk::IResidualPullCalculator> m_pullCalculator;     //<! tool to calculate residuals and pulls */
/*   ToolHandle<Muon::MuonEDMHelperTool> m_helperTool; */
/*   ToolHandle<Muon::MuonEDMPrinterTool> m_printer; */
/*   ToolHandle<Muon::MuonIdHelperTool> m_idHelperTool; */
/*   ToolHandle<Trk::IPropagator> m_propagator; */
/*   ToolHandle<Muon::MuonDQATrackHistTool> m_TrackHistoTool; */
/*   ToolHandle<Muon::MuonDQAFitFunc> m_muondqafitfunc; */
/*   ToolHandle<Muon::MuonDQAHistTool> m_histoTool; */
/* }; */

#endif
