/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/* **********************************************************************
 *     Program        : CSCSegmValAlg                                
 *     Package        : MuonSegmMonitoring                           
 *     Author         : Jason Veatch <jason.veatch@cern.ch>
 *                      Venkat Kaushik <Venkat.Kaushik@CERN.CH>         
 *     Purpose        : CSC segment DQ Monitoring                       
 *----------------------------------------------------------------------
 *  #   Date       Comments                                  By         
 *----------------------------------------------------------------------
 *  0 28/08/2008                                                VSK     
 *  1 11/03/2009   Improved Software. Removed CSCSegmentTree    VSK     
 *  2 17/03/2011   Access CSC segments from MuonSegments        Jason
 ************************************************************************/

#ifndef CSCSegmValAlg_H
#define CSCSegmValAlg_H

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonSegment/MuonSegment.h"
#include "TrkSegment/SegmentCollection.h"
#include "TrkTrack/Track.h"

#include "TrigDecisionTool/TrigDecisionTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

#include <vector>
#include <string>
#include <memory>

class TH1;
class TH1F;
class TH2F;

namespace Trk {
  class MeasurementBase;
}

class CSCSegmValAlg : public ManagedMonitorToolBase {

 public:
  /** Constructor */
  CSCSegmValAlg( const std::string & type, const std::string & name, const IInterface* parent ); 
  /** Destructor */
  virtual ~CSCSegmValAlg()=default;
  /** Histogram booking method */
  virtual StatusCode bookHistograms();
  /** Histogram filling method */
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();

  StatusCode initialize();

 private:

  typedef std::vector<TH1 *> SegmHistType;
  typedef std::vector<TH1 *>::const_iterator SegmHistIter;

  typedef std::vector<const Trk::MeasurementBase*> TrkSegmType;
  typedef std::vector<const Trk::MeasurementBase*>::const_iterator TrkSegmIter;
   
  std::unique_ptr<MonGroup> m_segmDetail_EA;
  std::unique_ptr<MonGroup>  m_segmDetail_EC;
  std::unique_ptr<MonGroup> m_segmOview_EA;
  std::unique_ptr<MonGroup> m_segmOview_EC;

  typedef enum EndCaps { ECA = 0, ECC } EndCapType;

  void bookSegmentHistograms();
  bool isCscSegment( const Muon::MuonSegment* seg ) const;
  unsigned int cscHits( const Muon::MuonSegment* seg ) const;
  void setCSCLayerLabels(TH1 *h, int m_side);
  void regCSCHist(TH1 *h, MonGroup *mg );
  bool segmSlopeCut(float& csc_x, float& csc_ax, float& cut );
  //Trigger aware monitoring
  bool evtSelTriggersPassed();
    

  //!< TDT handle
  ToolHandle<Trig::TrigDecisionTool> m_trigDec;
        
  std::vector<std::string> m_sampSelTriggers;
  bool m_doEvtSel;

  SG::ReadHandleKey<Trk::SegmentCollection> m_segmKey{this,"SegmentKey","MuonSegments","muon segments"};
  double m_segmSlope;

  bool m_debuglevel;           //!< private member to control debug messages
  bool m_bookedhistos;

  std::vector<std::string> m_ecap, m_segcoll; 

  TH2F* m_h2CSC_Segm_NumOfSegs_EA;
  TH2F* m_h2CSC_Segm_NumOfSegs_EC;

  std::vector<std::string> m_clusStatWord;
  std::vector<std::string> m_NClusWord;

  TH2F* m_h2CSC_Segm_NumOfNClusSegs_Eta_EA;
  TH2F* m_h2CSC_Segm_NumOfNClusSegs_Eta_EC;

  TH1F* m_h1CSC_Segm_Efficiency_Eta_EA;
  TH1F* m_h1CSC_Segm_Efficiency_Eta_EC;

  TH2F* m_h2CSC_Segm_QsumOfGoodClusMap_Eta_EA;
  TH2F* m_h2CSC_Segm_QsumOfGoodClusMap_Eta_EC;
    
  TH2F* m_h2CSC_Segm_TimeOfGoodClusMap_Eta_EA;
  TH2F* m_h2CSC_Segm_TimeOfGoodClusMap_Eta_EC;
    
  TH1F* m_h1CSC_Segm_StatOfClus_Eta_EA;
  TH1F* m_h1CSC_Segm_NumOfClus_Eta_EA;
  TH1F* m_h1CSC_Segm_NumOfGoodClus_Eta_EA;
  TH1F* m_h1CSC_Segm_QsumOfClus_Eta_EA;
  TH1F* m_h1CSC_Segm_QsumOfGoodClus_Eta_EA;
  TH1F* m_h1CSC_Segm_TimeOfClus_Eta_EA;
  TH1F* m_h1CSC_Segm_TimeOfGoodClus_Eta_EA;

  TH1F* m_h1CSC_Segm_StatOfClus_Eta_EC;
  TH1F* m_h1CSC_Segm_NumOfClus_Eta_EC;
  TH1F* m_h1CSC_Segm_NumOfGoodClus_Eta_EC;
  TH1F* m_h1CSC_Segm_QsumOfClus_Eta_EC;
  TH1F* m_h1CSC_Segm_QsumOfGoodClus_Eta_EC;
  TH1F* m_h1CSC_Segm_TimeOfClus_Eta_EC;
  TH1F* m_h1CSC_Segm_TimeOfGoodClus_Eta_EC;
    
  TH2F* m_h2CSC_Segm_QsumOfGoodClusMap_Phi_EA;
  TH2F* m_h2CSC_Segm_QsumOfGoodClusMap_Phi_EC;
    
  TH2F* m_h2CSC_Segm_TimeOfGoodClusMap_Phi_EA;
  TH2F* m_h2CSC_Segm_TimeOfGoodClusMap_Phi_EC;
    
  TH1F* m_h1CSC_Segm_StatOfClus_Phi_EA;
  TH1F* m_h1CSC_Segm_NumOfClus_Phi_EA;
  TH1F* m_h1CSC_Segm_NumOfGoodClus_Phi_EA;
  TH1F* m_h1CSC_Segm_QsumOfClus_Phi_EA;
  TH1F* m_h1CSC_Segm_QsumOfGoodClus_Phi_EA;
  TH1F* m_h1CSC_Segm_TimeOfClus_Phi_EA;
  TH1F* m_h1CSC_Segm_TimeOfGoodClus_Phi_EA;

  TH1F* m_h1CSC_Segm_StatOfClus_Phi_EC;
  TH1F* m_h1CSC_Segm_NumOfClus_Phi_EC;
  TH1F* m_h1CSC_Segm_NumOfGoodClus_Phi_EC;
  TH1F* m_h1CSC_Segm_QsumOfClus_Phi_EC;
  TH1F* m_h1CSC_Segm_QsumOfGoodClus_Phi_EC;
  TH1F* m_h1CSC_Segm_TimeOfClus_Phi_EC;
  TH1F* m_h1CSC_Segm_TimeOfGoodClus_Phi_EC;
    
  TH2F* m_h2CSC_Segm_QsumOfGoodClus_PhiVsEta_EA;
  TH2F* m_h2CSC_Segm_QsumOfGoodClus_PhiVsEta_EC;

  ServiceHandle<Muon::IMuonEDMHelperSvc> m_edmHelperSvc {this, "edmHelper", 
    "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc", 
    "Handle to the service providing the IMuonEDMHelperSvc interface" };
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
};

#endif
