/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCSegmValidation_H
#define CSCSegmValidation_H

// **********************************************************************
// CSCSegmValidation
// AUTHOR: I. Christidi - N. Benekos (UIUC)  
// **********************************************************************

/* // Gaudi Tools */
/* #include "GaudiKernel/StatusCode.h" */
/* #include "GaudiKernel/ToolHandle.h" */
/* #include "GaudiKernel/MsgStream.h" */
 
/* #include "AthenaMonitoring/AthenaMonManager.h" */
/* #include "AthenaMonitoring/ManagedMonitorToolBase.h" */
/* #include "AthenaBaseComps/AthAlgorithm.h" */

/* // Track  */
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

/* //  */
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
 
/* class CSCSegmValidation : public ManagedMonitorToolBase */
/* { */
/*  public: */
/*   /\** Constructor *\/ */
/*   CSCSegmValidation( const std::string & type, const std::string & name, const IInterface* parent );  */
/*   /\** Destructor *\/ */
/*   virtual ~CSCSegmValidation(); */
 
/*   /\** Histogram booking method *\/ */
/*   virtual StatusCode bookHistograms();    */
  
/*   /\** Histogram filling method *\/ */
/*   virtual StatusCode fillHistograms(); */
  
/*   virtual StatusCode procHistograms();  */
   
/*   StatusCode initialize(); */
/*   StatusCode finalize();  */

/*  protected: */
 
 
/*  private: */

/*   static const int COLLECTIONS = 2; */
/*   static const int ENDCAPS = 2; */
/*   static const int SECTORS = 16; */

/*   void bookSegmentHistograms(); */
/*   //  void deleteUnwantedHistograms( Muon::MuonDQATrackHistTool::TrackHistos histos_from_tool, std::string m_segmCollectionName, int j); */
/*   bool isCscSegment( const Muon::MuonSegment* seg ) const; */
/*   unsigned int cscHits( const Muon::MuonSegment* seg ) const; */
         
/*   StoreGateSvc* m_storeGate; */
/*   ActiveStoreSvc* m_activeStore;  */

/*   const MuonGM::MuonDetectorManager* m_muonMgr; */
/*   const CscIdHelper* m_cscIdHelper; */
/*   const TgcIdHelper* m_tgcIdHelper; */
/*   std::vector<std::string> m_segmCollectionName;   //!< Reconstructed segm collection name */
/*   std::vector<int> m_segmCollectionFlag; */
/*   bool m_doMDT; // subsystem flags */
/*   bool m_doRPC; */
/*   bool m_doTGC; */
/*   bool m_doCSC; */
/*   const Trk::SegmentCollection* m_segms;           //container for segms */
 
/*   int                    nsegm_Counter[COLLECTIONS][ENDCAPS]; */
/*   //int                    nsegm_ECA_Counter[COLLECTIONS]; */
/*   //int                    nsegm_ECC_Counter[COLLECTIONS]; */
/*   int                    nsegm_sect_Counter[COLLECTIONS][ENDCAPS][SECTORS]; */

/*   int                    m_eventCounter; */
/*   int                    m_eventNumber; */
/*   int                    m_rec_nSoS; */
/*   static const int       s_maxEvents;            //!< Maximum number of events per run */
/*   static const int       s_maxSoS;               //!< Maximum number of SegmStateOnSurface objects associated to the segm */

/*   bool                   m_debuglevel;           //!< private member to control debug messages  */
/*   bool                   b_status; */

/*   std::vector<std::string> ecap, coll;  */

/*   //  Muon Monitoring Histograms  */
/*   Muon::MuonDQATrackHistTool::TrackHistos histos_from_tool_Moore; */
/*   Muon::MuonDQATrackHistTool::TrackHistos histos_from_tool_MBoy; */
/*   std::vector<Muon::MuonDQATrackHistTool::TrackHistos> histos_from_tool; */

/*   TH1F* h_nsegm[COLLECTIONS][ENDCAPS]; */
/*   TH1F* h_nsegm_oview[COLLECTIONS][ENDCAPS]; */
/*   TH1F* h_nsegm_sect[COLLECTIONS][ENDCAPS][SECTORS]; */

/*   //TH1F* h_nclust_per_segm; */

/*   /\* */
/*     TH1F* nthreehitseg_sect; */
/*     TH1F* nfourhitseg_sect; */
/*     TH1F* seg_eff_sect; */
/*   *\/ */

/*   //TH1F* chi2_histo; */
  
/*   std::vector<TH1F*> m_num_segms; */
/*   std::vector<TH2F*> m_num_hits_per_segmvsphi; */
/*   std::vector<TH2F*> m_num_hits_per_segm_vs_station; */
/*   std::vector<TH2F*> chi2_vs_station; */

/*   std::vector<TH2F*> m_segm_etavsphi; */
/*   std::vector<TH2F*> m_segm_eta_vs_phi; */
/*   std::vector<TH2F*> m_segm_eta_vs_phiInner; */
/*   std::vector<TH2F*> m_segm_eta_vs_phiMiddle; */
/*   std::vector<TH2F*> m_segm_eta_vs_phiOuter; */

/*   MonGroup *cscsegm_detailEA_Moore, *cscsegm_detailEA_MuBoy, */
/*     *cscsegm_detailEC_Moore, *cscsegm_detailEC_MuBoy, */
/*     *cscsegm_oviewEA_Moore, *cscsegm_oviewEA_MuBoy, */
/*     *cscsegm_oviewEC_Moore, *cscsegm_oviewEC_MuBoy; */

/*   std::vector<TH1*> //m_cscSegmExpert  ,  */
/*     //m_cscSegmShift   ,  */
/*     m_cscSegmDetailEA_Moore , */
/*     m_cscSegmDetailEA_MuBoy ,  */
/*     m_cscSegmDetailEC_Moore , */
/*     m_cscSegmDetailEC_MuBoy , */
/*     m_cscSegmOviewEA_Moore , */
/*     m_cscSegmOviewEA_MuBoy ,  */
/*     m_cscSegmOviewEC_Moore , */
/*     m_cscSegmOviewEC_MuBoy ; */


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
