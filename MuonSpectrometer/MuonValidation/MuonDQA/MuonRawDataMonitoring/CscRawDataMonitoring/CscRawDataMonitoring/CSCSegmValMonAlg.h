/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCSegmValMonAlg_H
#define CSCSegmValMonAlg_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "AthenaMonitoringKernel/Monitored.h"

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

namespace Trk {
  class MeasurementBase;
}

class CSCSegmValMonAlg : public AthMonitorAlgorithm {

 public:
  /** Constructor */
  CSCSegmValMonAlg( const std::string& name, ISvcLocator* pSvcLocator );
  /** Destructor */
  virtual ~CSCSegmValMonAlg()=default;
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

 private:

  typedef std::vector<TH1 *> SegmHistType;
  typedef std::vector<TH1 *>::const_iterator SegmHistIter;

  typedef std::vector<const Trk::MeasurementBase*> TrkSegmType;
  typedef std::vector<const Trk::MeasurementBase*>::const_iterator TrkSegmIter;

  typedef enum EndCaps { ECA = 0, ECC } EndCapType;

  bool isCscSegment( const Muon::MuonSegment* seg ) const;
  unsigned int cscHits( const Muon::MuonSegment* seg ) const;
  bool segmSlopeCut(const float csc_x, const float csc_ax, const float cut ) const;
  //Trigger aware monitoring
  bool evtSelTriggersPassed() const; 

  ServiceHandle<Muon::IMuonEDMHelperSvc> m_edmHelperSvc {this, "edmHelper", 
    "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc", 
    "Handle to the service providing the IMuonEDMHelperSvc interface" };
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  SG::ReadHandleKey<Trk::SegmentCollection> m_segmKey{this,"SegmentKey","MuonSegments","muon segments"};

  Gaudi::Property<std::vector<std::string>> m_sampSelTriggers{this,"EventSelTriggers",{}};
  Gaudi::Property<bool> m_doEvtSel{this,"DoEventSelection",false};
  Gaudi::Property<double> m_segmSlope{this,"SegmentSlopeCut",0};

};

#endif
