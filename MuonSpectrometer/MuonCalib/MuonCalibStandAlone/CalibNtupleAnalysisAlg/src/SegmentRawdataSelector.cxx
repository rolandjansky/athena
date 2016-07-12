/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//this
#include "CalibNtupleAnalysisAlg/SegmentRawdataSelector.h"

//MuonCalibStandAloneBase
#include "MuonCalibStandAloneBase/RegionSelectionSvc.h"

//MuonCalibEventBase
#include "MuonCalibEventBase/MuonCalibEvent.h"
#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "MuonCalibEventBase/MuonCalibEvent.h"
#include "MuonCalibEventBase/MuonCalibPattern.h"
#include "MuonCalibEventBase/MuonCalibRawHitCollection.h"
#include "MuonCalibEventBase/MuonCalibRawMdtHit.h"
#include "MuonCalibEventBase/MuonCalibRawRpcHit.h"

// MuonReadoutGeometry //
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

// AtlasConditions //
#include "MuonIdHelpers/MdtIdHelper.h"

// MuonCalib //
#include "MuonCalibITools/IIdToFixedIdTool.h"
namespace MuonCalib {


///////////////////////////
// SegmentRawdataSelector //
///////////////////////////
SegmentRawdataSelector::SegmentRawdataSelector(const std::string &t, const std::string &n, const IInterface *p):
  AthAlgTool(t, n, p), m_reg_sel_svc("RegionSelectionSvc", n) {
  m_min_hits = 1;
  declareProperty("MinHits", m_min_hits);
  m_max_hits = -1; 
  declareProperty("MaxHits", m_max_hits);
  m_adc_cut = 0;
  declareProperty("AdcCut", m_adc_cut);
  declareInterface< CalibSegmentPreparationTool >(this);
  
  m_MDT_ID_helper = std::string("MDTIDHELPER");
  declareProperty("MDTIdHelper", m_MDT_ID_helper);

  m_idToFixedIdToolType = std::string("MuonCalib::IdToFixedIdTool");
  declareProperty("idToFixedIdToolType", m_idToFixedIdToolType);
  
  m_idToFixedIdToolName = std::string("MuonCalib_IdToFixedIdTool");
  declareProperty("idToFixedIdToolName", m_idToFixedIdToolName);
  declareProperty("RegionSelectionSvc", m_reg_sel_svc);
}  //end SegmentRawdataSelector::SegmentRawdataSelector

////////////////
// initialize //
////////////////
StatusCode SegmentRawdataSelector::initialize(void) {

  ATH_CHECK( m_reg_sel_svc.retrieve() );

// MDT ID helper //
  ATH_CHECK( detStore()->retrieve(m_MdtIdHelper, m_MDT_ID_helper) );

// muon detector manager //
  ATH_CHECK( detStore()->retrieve(m_detMgr) );

// muon fixed id tool //
  ATH_CHECK( toolSvc()->retrieveTool(m_idToFixedIdToolType, m_idToFixedIdToolName, m_id_tool) );

  return StatusCode::SUCCESS;
}  //end SegmentRawdataSelector::initialize

void SegmentRawdataSelector::prepareSegments(const MuonCalibEvent *&event, std::map<NtupleStationId, MuonCalibSegment *> & segments) {
//delete old segments
  for(std::set<MuonCalibSegment *>::iterator it=m_segments.begin(); it!=m_segments.end(); it++) {
    delete (*it);
  }
  m_segments.clear();
  segments.clear();
  double TDC_to_ns(0.78125);
  const MuonCalibRawHitCollection *raw_hits(event->rawHitCollection());
// loop over the raw hits //
  for (MuonCalibRawHitCollection::MuonCalibRawMdtHitVecCit it=
	 raw_hits->rawMdtHitCollectionBegin();
       it!=raw_hits->rawMdtHitCollectionEnd(); ++it)  {
    MuonCalibRawMdtHit *hit(*it);
    //check for adc cut
    if (hit->adcCount() < m_adc_cut) continue;
   // get the raw hit and check whether it is in the calibration region //
    if(!m_reg_sel_svc->isInRegion(hit->identify())) continue;
    NtupleStationId station_identifier(hit->identify());
    station_identifier.SetMultilayer(0);
    Identifier station_id = m_id_tool->fixedIdToId(hit->identify());
    const MuonGM::MdtReadoutElement *MdtRoEl =
      m_detMgr->getMdtReadoutElement(m_MdtIdHelper->channelID(station_id,
	hit->identify().mdtMultilayer(),hit->identify().mdtTubeLayer(),hit->identify().mdtTube()));

    if (MdtRoEl==0) {
      continue;
    }
   // create a segment if necessary //
    if (segments.find(station_identifier)==segments.end()) {
      Amg::Vector3D local_pos=get_raw_position(hit, raw_hits, MdtRoEl-> GlobalToAmdbLRSTransform());
      segments[station_identifier] =
	new MuonCalibSegment(1.0, 
			     local_pos,
			     Amg::Vector3D(0.0, 0.0, 1.0),
			     MdtRoEl->AmdbLRSToGlobalTransform());
    }
    Amg::Vector3D hit_position=hit->localPosition();
    hit_position[0]=(segments[station_identifier]->position()[0]);
    MdtCalibHitBase *MDT_calib_hit(new MdtCalibHitBase(
	hit->tdcCount(),hit->adcCount(),hit->globalPosition(),hit_position));
    MDT_calib_hit->setDriftTime(TDC_to_ns*hit->tdcCount());
    MDT_calib_hit->setTubeT0(0.0);
    MDT_calib_hit->setDriftRadius(0.02*TDC_to_ns*hit->tdcCount(),0.2);
    MDT_calib_hit->setIdentifier(hit->identify());
    MDT_calib_hit->setDistanceToReadout(0.0);
    MDT_calib_hit->setPropagationTime(0.0);
    segments[station_identifier]->addHitOnTrack(MDT_calib_hit);
  }
  std::set<NtupleStationId> to_delte;
  for(std::map<NtupleStationId, MuonCalibSegment *>::iterator it=segments.begin(); it!=segments.end(); it++) {
    if((static_cast<int>(it->second->hitsOnTrack()) > m_max_hits  && m_max_hits>0)|| static_cast<int>(it->second->hitsOnTrack()) < m_min_hits) {
      to_delte.insert(it->first);
    } else {
      m_segments.insert(it->second);
    }
  }
  for(std::set<NtupleStationId> ::iterator it=to_delte.begin(); it!=to_delte.end(); it++) {
    std::map<NtupleStationId, MuonCalibSegment *>::iterator m_it(segments.find(*it));
    if(m_it!=segments.end()) {
      delete m_it->second;
      segments.erase(m_it);
    }
  }
}  //end SegmentRawdataSelector::prepareSegments

inline const Amg::Vector3D SegmentRawdataSelector::get_raw_position(const MuonCalibRawMdtHit *hit, 
const MuonCalibRawHitCollection *coll, const Amg::Transform3D &global_to_local) const {
//	std::cout<<"event"<<std::endl;
  double x_pos=0.0;
  int n_rpc_hits=0;
  int mdt_station=hit->identify().stationName();
  int mdt_eta=hit->identify().eta();
  int mdt_phi=hit->identify().phi();
  for(MuonCalibRawHitCollection::MuonCalibRawRpcHitVecCit it = coll->rawRpcHitCollectionBegin(); it != coll->rawRpcHitCollectionEnd(); it++) {
    MuonCalibRawRpcHit *rpc_hit(*it);
//		const MuonFixedId & id(rpc_hit->identify());
//		std::cout<<id.stationNameString()<<" "<<id.phi()<<" "<<id.eta()<<std::endl;
	//check that rpc and mdt hit are in the same chamber
    if(mdt_station != rpc_hit->identify().stationName() || mdt_eta != rpc_hit->identify().eta() || mdt_phi != rpc_hit->identify().phi()) {
      continue;
    }
    //we need only phi strips
    if(rpc_hit->identify().rpcMeasuresPhi() == 0) continue;
    Amg::Vector3D local_strip_pos = global_to_local*rpc_hit->globalPosition();
    x_pos += local_strip_pos.x();
//		std::cout<< local_strip_pos << std::endl;
    n_rpc_hits++;
  }
  if(n_rpc_hits>0) {
    x_pos /= static_cast<double>(n_rpc_hits);
  }
  return Amg::Vector3D(x_pos, hit->localPosition().y(), hit->localPosition().z());
}  //end SegmentRawdataSelector::get_raw_position

} // namespace Muoncalib
