/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

// Gaudi Tools
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/MsgStream.h"

// Athena Monitoring 
#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "AthenaBaseComps/AthAlgorithm.h"

// Muon Segment 
#include "MuonSegment/MuonSegment.h"
#include "TrkSegment/SegmentCollection.h"

// Muon Track 
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

//TDT
#include "TrigDecisionTool/TrigDecisionTool.h"

// STL
#include <vector>
#include <string>
#include <map>

class TH1;
class TH1F;
class TH2F;
class CscIdHelper;
class TgcIdHelper;

namespace Muon {
  class MuonEDMHelperTool;
  class MuonIdHelperTool;
}


namespace Trk {
  class IUpdator;
  class RIO_OnTrack;
  class Track;
  class TrackStateOnSurface;
  class MeasurementBase;
}
 
class CSCSegmValAlg : public ManagedMonitorToolBase {

 public:
  /** Constructor */
  CSCSegmValAlg( const std::string & type, const std::string & name, const IInterface* parent ); 
  /** Destructor */
  virtual ~CSCSegmValAlg();
  /** Histogram booking method */
  virtual StatusCode bookHistograms();
  /** Histogram filling method */
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();

  StatusCode initialize();
  StatusCode finalize(); 

 private:

  typedef std::map<std::string, int> MuonSegmType;
  typedef std::map<std::string, int>::const_iterator MuonSegmIter;

  typedef std::vector<TH1 *> SegmHistType;
  typedef std::vector<TH1 *>::const_iterator SegmHistIter;

  typedef std::vector<const Trk::MeasurementBase*> TrkSegmType;
  typedef std::vector<const Trk::MeasurementBase*>::const_iterator TrkSegmIter;
   
  std::vector<MonGroup *> m_segmDetail_EA, m_segmDetail_EC;
  std::vector<MonGroup *> m_segmOview_EA, m_segmOview_EC;

  typedef enum EndCaps { ECA = 0, ECC } EndCapType;

  void bookSegmentHistograms();
  void clearHistogramVectors();
  bool isCscSegment( const Muon::MuonSegment* seg ) const;
  unsigned int cscHits( const Muon::MuonSegment* seg ) const;
  void setCSCLayerLabels(TH1 *h, int m_side);
  void regCSCHist(TH1 *h, MonGroup *mg );
  bool segmSlopeCut(float& csc_x, float& csc_ax, float& cut );
  //Trigger aware monitoring
  bool evtSelTriggersPassed();
    

  StoreGateSvc* m_storeGate;
    
  //!< TDT handle
  ToolHandle<Trig::TrigDecisionTool> m_trigDec;
        
  std::vector<std::string> m_sampSelTriggers;
  bool m_doEvtSel;

  const CscIdHelper* m_cscIdHelper;
  const TgcIdHelper* m_tgcIdHelper;
  MuonSegmType m_segmKey;                //!< Reconstructed segm collection name
  std::map<std::string, std::string> m_segmPrefix;
  std::map<std::string, double> m_segmSlope;

  //std::vector<int> m_segmCollectionFlag;
  const Trk::SegmentCollection* m_segms;           //container for segms

  bool m_debuglevel;           //!< private member to control debug messages
  bool m_bookedhistos;

  std::vector<std::string> m_ecap, m_segcoll; 

  std::vector<TH2F*> m_h2CSC_Segm_NumOfSegs_EA;
  std::vector<TH2F*> m_h2CSC_Segm_NumOfSegs_EC;

  //unsigned int m_ncoll;
  std::vector<std::string> m_clusStatWord;
  std::vector<std::string> m_NClusWord;

  std::vector<TH2F*> m_h2CSC_Segm_NumOfNClusSegs_Eta_EA;
  std::vector<TH2F*> m_h2CSC_Segm_NumOfNClusSegs_Eta_EC;

  std::vector<TH1F*> m_h1CSC_Segm_Efficiency_Eta_EA;
  std::vector<TH1F*> m_h1CSC_Segm_Efficiency_Eta_EC;

  std::vector<TH2F*> m_h2CSC_Segm_QsumOfGoodClusMap_Eta_EA;
  std::vector<TH2F*> m_h2CSC_Segm_QsumOfGoodClusMap_Eta_EC;
    
  std::vector<TH2F*> m_h2CSC_Segm_TimeOfGoodClusMap_Eta_EA;
  std::vector<TH2F*> m_h2CSC_Segm_TimeOfGoodClusMap_Eta_EC;
    
  std::vector<TH1F*> m_h1CSC_Segm_StatOfClus_Eta_EA;
  std::vector<TH1F*> m_h1CSC_Segm_NumOfClus_Eta_EA;
  std::vector<TH1F*> m_h1CSC_Segm_NumOfGoodClus_Eta_EA;
  std::vector<TH1F*> m_h1CSC_Segm_QsumOfClus_Eta_EA;
  std::vector<TH1F*> m_h1CSC_Segm_QsumOfGoodClus_Eta_EA;
  std::vector<TH1F*> m_h1CSC_Segm_TimeOfClus_Eta_EA;
  std::vector<TH1F*> m_h1CSC_Segm_TimeOfGoodClus_Eta_EA;

  std::vector<TH1F*> m_h1CSC_Segm_StatOfClus_Eta_EC;
  std::vector<TH1F*> m_h1CSC_Segm_NumOfClus_Eta_EC;
  std::vector<TH1F*> m_h1CSC_Segm_NumOfGoodClus_Eta_EC;
  std::vector<TH1F*> m_h1CSC_Segm_QsumOfClus_Eta_EC;
  std::vector<TH1F*> m_h1CSC_Segm_QsumOfGoodClus_Eta_EC;
  std::vector<TH1F*> m_h1CSC_Segm_TimeOfClus_Eta_EC;
  std::vector<TH1F*> m_h1CSC_Segm_TimeOfGoodClus_Eta_EC;
    
  std::vector<TH2F*> m_h2CSC_Segm_QsumOfGoodClusMap_Phi_EA;
  std::vector<TH2F*> m_h2CSC_Segm_QsumOfGoodClusMap_Phi_EC;
    
  std::vector<TH2F*> m_h2CSC_Segm_TimeOfGoodClusMap_Phi_EA;
  std::vector<TH2F*> m_h2CSC_Segm_TimeOfGoodClusMap_Phi_EC;
    
  std::vector<TH1F*> m_h1CSC_Segm_StatOfClus_Phi_EA;
  std::vector<TH1F*> m_h1CSC_Segm_NumOfClus_Phi_EA;
  std::vector<TH1F*> m_h1CSC_Segm_NumOfGoodClus_Phi_EA;
  std::vector<TH1F*> m_h1CSC_Segm_QsumOfClus_Phi_EA;
  std::vector<TH1F*> m_h1CSC_Segm_QsumOfGoodClus_Phi_EA;
  std::vector<TH1F*> m_h1CSC_Segm_TimeOfClus_Phi_EA;
  std::vector<TH1F*> m_h1CSC_Segm_TimeOfGoodClus_Phi_EA;

  std::vector<TH1F*> m_h1CSC_Segm_StatOfClus_Phi_EC;
  std::vector<TH1F*> m_h1CSC_Segm_NumOfClus_Phi_EC;
  std::vector<TH1F*> m_h1CSC_Segm_NumOfGoodClus_Phi_EC;
  std::vector<TH1F*> m_h1CSC_Segm_QsumOfClus_Phi_EC;
  std::vector<TH1F*> m_h1CSC_Segm_QsumOfGoodClus_Phi_EC;
  std::vector<TH1F*> m_h1CSC_Segm_TimeOfClus_Phi_EC;
  std::vector<TH1F*> m_h1CSC_Segm_TimeOfGoodClus_Phi_EC;
    
  std::vector<TH2F*> m_h2CSC_Segm_QsumOfGoodClus_PhiVsEta_EA;
  std::vector<TH2F*> m_h2CSC_Segm_QsumOfGoodClus_PhiVsEta_EC;

  std::vector< SegmHistType* > m_cscSegmDetailEA, m_cscSegmDetailEC;

  /*
    std::vector<TH1*> //m_cscSegmExpert  , 
    //m_cscSegmShift   , 
    m_cscSegmDetailEA_Moore ,
    m_cscSegmDetailEA_MuBoy , 
    m_cscSegmDetailEC_Moore ,
    m_cscSegmDetailEC_MuBoy ,
    m_cscSegmOviewEA_Moore ,
    m_cscSegmOviewEA_MuBoy , 
    m_cscSegmOviewEC_Moore ,
    m_cscSegmOviewEC_MuBoy ;
  */

  // Tool handles
  ToolHandle<Muon::MuonEDMHelperTool> m_helperTool;
  ToolHandle<Muon::MuonIdHelperTool> m_idHelperTool;
};

#endif
