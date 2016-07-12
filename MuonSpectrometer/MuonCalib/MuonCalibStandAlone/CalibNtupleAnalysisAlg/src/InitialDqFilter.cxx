/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//this
#include "CalibNtupleAnalysisAlg/InitialDqFilter.h"

//MuonCalibStandAloneBase
#include "MuonCalibStandAloneBase/RegionSelectionSvc.h"

//MuonCalibEventBase
#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "MuonCalibEventBase/MdtCalibHitBase.h"

//MuonCalibIdentifier
#include "MuonCalibIdentifier/MuonFixedId.h"

//c- c++
#include "fstream"


namespace MuonCalib {

InitialDqFilter::InitialDqFilter(const std::string &t, const std::string &n, const IInterface *p): AthAlgTool(t, n, p), m_reg_sel_svc("RegionSelectionSvc", n) {
  m_initial_dq_path = "initial_dq.txt";
  declareProperty("RegionSelectionSvc", m_reg_sel_svc);
  declareProperty("InitialDqFile", m_initial_dq_path);
  declareInterface< CalibSegmentPreparationTool >(this);
}
	
StatusCode InitialDqFilter::initialize(void) {
//get region selection svc
  ATH_CHECK( m_reg_sel_svc.retrieve() );
//read initial dq list
  std::ifstream lst(m_initial_dq_path.c_str());
  if(!lst.good()) return StatusCode::FAILURE;
  std::string station, s_name;
  int ml, ly, tb, severity, dummy, eta, phi;
  //read line
  while(1) {
    lst>>station;
    if(!lst.good()) break;
    lst>>ml;
    if(!lst.good()) break;
    lst>>ly;
    if(!lst.good()) break;
    lst>>tb;
    if(!lst.good()) break;
    lst>>severity;
    if(!lst.good()) break;
    lst>>dummy;
    if(!lst.good()) break;
    lst>>dummy;
    if(!lst.good()) break;
    if(severity<2) continue;
    MuonFixedId id;
    id.set_mdt();
    for(unsigned int i=0; i<station.size(); i++)
      if(station[i]=='_') station[i]=' ';
    std::istringstream st_stream(station);
    st_stream>>s_name;
    st_stream>>phi;
    st_stream>>eta;
    id.setStationName(id.stationStringToFixedStationNumber(s_name));
    id.setStationEta(eta);
    id.setStationPhi(phi);
    id.setMdtMultilayer(ml);
    id.setMdtTubeLayer(ly);
    id.setMdtTube(tb);
    //only insert relevant tubes
    if(!m_reg_sel_svc->isInRegion(id)) continue;
    ATH_MSG_INFO( "Suppressing "<<s_name<<","<<phi<<","<<eta<<" ml="<<ml<<" ly="<<ly<<" tb="<<tb );
    m_suppressed_tubes.insert(id);
  }
  return StatusCode::SUCCESS;
}  //end InitialDqFilter::initialize

void InitialDqFilter::prepareSegments(const MuonCalibEvent *& /*event*/, 
std::map<NtupleStationId, MuonCalibSegment *> &segments) {
//reject empty segments
  std::set<NtupleStationId> empty_segments;	
  for(std::map<NtupleStationId, MuonCalibSegment *>::iterator it=segments.begin(); it!=segments.end(); it++) {
    std::vector<unsigned int> new_selection;
    bool is_empty(true);
    for(MuonCalibSegment::MdtHitIt h_it=(it->second)->mdtHOTBegin(); h_it!=(it->second)->mdtHOTEnd(); h_it++) {
      if (m_suppressed_tubes.find((*h_it)->identify()) == m_suppressed_tubes.end()) {
	is_empty = false;
	new_selection.push_back(0);
      }	else {
	new_selection.push_back(1);
      }
    }
    if(is_empty) {
      empty_segments.insert(it->first);
    } else {
      it->second->refineMdtSelection(new_selection);
    }
  }
  for(std::set<NtupleStationId> ::iterator it=empty_segments.begin(); it!=empty_segments.end(); it++) {
    if(segments.find(*it)!=segments.end())
      segments.erase(segments.find(*it));
  }
}  //end InitialDqFilter::prepareSegments

}//namespace MuonClib
