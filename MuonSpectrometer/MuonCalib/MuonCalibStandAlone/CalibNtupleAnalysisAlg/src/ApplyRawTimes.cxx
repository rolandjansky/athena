/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//this
#include "CalibNtupleAnalysisAlg/ApplyRawTimes.h"

//MuonCalibStandAloneBase
#include "MuonCalibStandAloneBase/RegionSelectionSvc.h"

//MuonCalibEventBase
#include "MuonCalibEventBase/MuonCalibEvent.h"
#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "MuonCalibEventBase/MuonCalibRawHitCollection.h"
#include "MuonCalibEventBase/MuonCalibRawMdtHit.h"
#include "MuonCalibEventBase/MdtCalibHitBase.h"

//MuonCalibIdentifier
#include "MuonCalibIdentifier/MuonFixedId.h"


#include "map"
#include "set"


namespace MuonCalib {

ApplyRawTimes::ApplyRawTimes(const std::string &t, const std::string &n, const IInterface *p)
  : AthAlgTool(t, n, p), m_reg_sel_svc("RegionSelectionSvc", n) {
  declareInterface< CalibSegmentPreparationTool >(this);
  declareProperty("RegionSelectionSvc", m_reg_sel_svc);
}

StatusCode ApplyRawTimes::initialize(void) {
  ATH_CHECK( m_reg_sel_svc.retrieve() );
  return StatusCode::SUCCESS;
}

void ApplyRawTimes::prepareSegments(const MuonCalibEvent *&event, std::map<NtupleStationId, MuonCalibSegment *> &segments) {
  std::map<MuonFixedId, std::set<int> > raw_times;
  const MuonCalibRawHitCollection *raw_coll(event->rawHitCollection());
  for(MuonCalibRawHitCollection::MuonCalibRawMdtHitVecCit it=raw_coll->rawMdtHitCollectionBegin(); it!=raw_coll->rawMdtHitCollectionEnd(); it++) {
    if(m_reg_sel_svc->isInRegion((*it)->identify()))
      raw_times[(*it)->identify()].insert((*it)->tdcCount());
  }
  std::set<NtupleStationId> to_delete;
  for(std::map<NtupleStationId, MuonCalibSegment *>::iterator it=segments.begin(); it!=segments.end(); it++) {
    MuonCalibSegment::MdtHitCit h_it(it->second->mdtHOTBegin());
    for(; h_it!=it->second->mdtHOTEnd(); h_it++) {
      if(raw_times.find((*h_it)->identify()) == raw_times.end()) {
	ATH_MSG_WARNING( "No Raw time found for this hit!" );
	to_delete.insert(it->first);
	break;
      }
      int tdc(*(raw_times[(*h_it)->identify()].begin()));
      (*h_it)->setDriftTime((25.0/32.0) * tdc);
      (*h_it)->setDriftRadius(0.0, 0.0);
      (*h_it)->setDistanceToTrack(0.0, 0.0);
      (*h_it)->setTimeFromTrackDistance(0.0, 0.0);
      (*h_it)->setSlewingTime(0.0);
      (*h_it)->setBFieldTime(0.0);
      (*h_it)->setPropagationTime(0.0);
      (*h_it)->setTimeOfFlight(0.0);
      (*h_it)->setWiresagTime(0.0);
      (*h_it)->setTemperatureTime(0.0);
      (*h_it)->setBackgroundTime(0.0);
      (*h_it)->setTubeT0(0.0);
      (*h_it)->setTdc(tdc);
    }
    it->second->setFittedT0(0.0);
  }
  for(std::set<NtupleStationId>::iterator it=to_delete.begin(); it!=to_delete.end(); it++) {
    segments.erase(segments.find(*it));
  }
}  //end ApplyRawTimes::prepareSegments

} //namespace MuonCalib
