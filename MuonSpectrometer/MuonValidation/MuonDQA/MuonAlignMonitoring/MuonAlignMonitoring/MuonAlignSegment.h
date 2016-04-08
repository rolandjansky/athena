/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONALIGNSEGMENT_H
#define MUONALIGNSEGMENT_H

/* // Gaudi Tools */
/* #include "GaudiKernel/MsgStream.h" */
/* #include "GaudiKernel/Algorithm.h" */
/* #include "GaudiKernel/AlgFactory.h" */
/* #include "GaudiKernel/IToolSvc.h" */
/* #include "GaudiKernel/ToolHandle.h" */
/* #include "GaudiKernel/ITHistSvc.h" */
/* #include "StoreGate/StoreGateSvc.h" */
/* #include "GaudiKernel/ToolHandle.h" */
  
/* #include "CLHEP/Units/SystemOfUnits.h" */

/* #include "TrkSegment/SegmentCollection.h" */
/* #include "TrkTrack/TrackCollection.h" */
/* #include "GeoPrimitives/GeoPrimitives.h" */
/* #include "EventPrimitives/EventPrimitives.h" */

/* // AthenaMonitoring */
/* #include "AthenaMonitoring/ManagedMonitorToolBase.h" */
/* #include "AthenaMonitoring/AthenaMonManager.h" */

/* // MuonDQAUtils */
/* #include "MuonDQAUtils/MuonDQATrackHistTool.h" */
/* #include "MuonDQAUtils/MuonDQAHistTool.h" */

/* #include <string> */
/* #include <map> */
/* #include "TH1.h" */
/* #include "TH2.h" */

/* class StoreGateSvc; */
/* class ActiveStoreSvc; */
/* class AtlasDetectorID; */
/* class Identifier; */
/* class MdtIdHelper; */
/* class CscIdHelper; */

/* namespace Trk{ */
/*   class Track; */
/*   class MeasurementBase; */
/*   class Segment; */
/* } */

/* namespace MuonGM { */
/*   class MuonDetectorManager; */
/* } */

/* namespace Muon { */
/*   class MuonSegment; */
/*   class MuonSegmentCombination; */
/*   class MuonEDMHelperTool; */
/*   class MuonIdHelperTool; */
/* } */



/* class MuonAlignSegment: public ManagedMonitorToolBase { */

/*  public: */


/*   /\** Constructor *\/ */
/*   MuonAlignSegment( const std::string & type, const std::string & name, const IInterface* parent ); */
/*   /\** Destructor *\/ */
/*   ~MuonAlignSegment(); */

/*   /\** Histogram booking method *\/ */
/*   virtual StatusCode bookHistograms ();    */
  
/*   /\** Histogram filling method *\/ */
/*   virtual StatusCode fillHistograms(); */
  
/*   virtual StatusCode procHistograms ();  */

/*  private: */
  
/*   StatusCode initialize(); */

/*   /\** methods called by fillHistograms *\/ */
/*   StatusCode getSegments( int& hIndex  ); */

/*   /\** test if segment is in/near a shower*\/ */
/*   void showeringTest( std::vector<Muon::MuonSegment* > muonSegs, int stationIdx ); */

/*   /\** test quality of segment *\/ */
/*   bool goodSegmentQuality(float& chi2, int& hots, int& holes, bool& isMDT, bool& isEndcap, int& station); */

/*   /\** test if segment pair direction is close enough to be considered *\/ */
/*   bool matchPairSegment( const Amg::Vector3D& globalDir1, const Amg::Vector3D& globalDir2 ); */


/*   /\** a handle on Store Gate for access to the Event Store *\/ */
/*   ActiveStoreSvc*                              m_activeStore; */
/*   StoreGateSvc*                                m_storeGate; */

/*   /\** name of the segment collection to retrieve from StoreGate *\/ */
/*   std::vector<int>                             m_segmCollectionFlag; */
/*   std::vector<std::string>                     m_segmCollectionName;  */
/*   std::vector<std::string>                     m_trackCollectionName; */

/*   /\** pointer to detector manager *\/ */
/*   const MuonGM::MuonDetectorManager*           p_muonMgr;      */

/*   /\** pointers to IdHelpers *\/ */
/*   const MdtIdHelper*                           p_mdtIdHelper; */
/*   const CscIdHelper*                           p_cscIdHelper; */

/*   /\** class member version of retrieving MsgStream *\/ */
/*   ToolHandle<Muon::MuonDQAHistTool>            m_histoTool; */
/*   ToolHandle<Muon::MuonEDMHelperTool>          p_helperTool; */
/*   ToolHandle<Muon::MuonIdHelperTool>           p_idHelperTool; */

/*   /\** a handle on the Hist/TTree registration service *\/ */
/*   ITHistSvc * m_thistSvc; */

/*   /\** Histograms *\/ */
/*   std::vector<TH2F*> m_h_muonalign_linesummary; */

/*   std::vector<TH1F*> m_h_endcapAL_lineZ_segmentZ; */
/*   std::vector<TH1F*> m_h_endcapCL_lineZ_segmentZ; */
/*   std::vector<TH1F*> m_h_endcapAS_lineZ_segmentZ; */
/*   std::vector<TH1F*> m_h_endcapCS_lineZ_segmentZ; */

/*   std::vector<TH1F*> m_h_barrelL_lineZ_segmentZ; */
/*   std::vector<TH1F*> m_h_barrelS_lineZ_segmentZ; */
 
/*   // Individual phi-sectors of barrel */
/*   std::vector<TH1F*> m_h_barrel_lineZ_segmentZ1; */
/*   std::vector<TH1F*> m_h_barrel_lineZ_segmentZ2; */
/*   std::vector<TH1F*> m_h_barrel_lineZ_segmentZ3; */
/*   std::vector<TH1F*> m_h_barrel_lineZ_segmentZ4; */
/*   std::vector<TH1F*> m_h_barrel_lineZ_segmentZ5; */
/*   std::vector<TH1F*> m_h_barrel_lineZ_segmentZ6; */
/*   std::vector<TH1F*> m_h_barrel_lineZ_segmentZ7; */
/*   std::vector<TH1F*> m_h_barrel_lineZ_segmentZ8; */
/*   std::vector<TH1F*> m_h_barrel_lineZ_segmentZ9; */
/*   std::vector<TH1F*> m_h_barrel_lineZ_segmentZ10; */
/*   std::vector<TH1F*> m_h_barrel_lineZ_segmentZ11; */
/*   std::vector<TH1F*> m_h_barrel_lineZ_segmentZ12; */
/*   std::vector<TH1F*> m_h_barrel_lineZ_segmentZ13; */
/*   std::vector<TH1F*> m_h_barrel_lineZ_segmentZ14; */
/*   std::vector<TH1F*> m_h_barrel_lineZ_segmentZ15; */
/*   std::vector<TH1F*> m_h_barrel_lineZ_segmentZ16; */


/*   /\** MDT segment specific cuts *\/ */
/*   double m_bMdtchi2max; */
/*   int    m_bMdtnholemax; */
/*   int    m_bMdtnhotsmin; */

/*   double m_eMdtchi2max; */
/*   int    m_eMdtnholemax; */
/*   int    m_eMdtnhotsmin; */

/*   double m_eCscchi2max; */
/*   double m_eCscnhotsmin; */

/*   double m_mdtdelthetamax; */
/*   double m_mdtdelphimax; */
/*   double m_mdtdeltaZ; */
/*   double m_mdtdeltaR; */
/*   double m_mdtdeltaEta; */

/*   /\** Record if segment is part of shower or not *\/ */
/*   std::vector<int> hasShoweredInn; */
/*   std::vector<int> hasShoweredMid; */
/*   std::vector<int> hasShoweredOut; */

/*   /\** Record the index of the track associated with the segment *\/ */
/*   std::vector<int> hasTrackMatchInn; */
/*   std::vector<int> hasTrackMatchMid; */
/*   std::vector<int> hasTrackMatchOut; */


/*   /\** Entries for summary plot *\/ */
/*   float zLocSumMoore[16][16]; */
/*   int zLocCountMoore[16][16]; */
/*   float zLocSumMboy[16][16]; */
/*   int zLocCountMboy[16][16]; */

/* }; */

#endif

