/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//this
#include "MuonCalibStandAloneExtraTools/CalibExtraNtupleLoader.h"

//CalibNtupleReader
#include "MuonCalibExtraNtuple/NTReader_E.h"

//MuonCalibExtraTreeEvent
#include "MuonCalibExtraTreeEvent/MuonCalibEvent_E.h"

//MuonCalibStandAloneBase
#include "MuonCalibStandAloneBase/RegionSelectionSvc.h"

//root
#include "TChain.h"
#include "TFile.h"
#include "TKey.h"

//c - c++
#include "fstream"

namespace MuonCalib {

///////////////////////
// CalibExtraNtupleLoader //
///////////////////////
CalibExtraNtupleLoader::CalibExtraNtupleLoader(const std::string &t, const std::string &n, const IInterface *p): 
  AthAlgTool(t, n, p), p_reg_sel_svc(NULL) {
  m_filelist="file_list.txt";
  declareProperty("FileList", m_filelist);
  m_first = 0;
  declareProperty("FirstEvent", m_first);
  m_last = -1;
  declareProperty("LastEvent", m_last);
  m_ntuple_type="AUTO";
  declareProperty("NtupleType", m_ntuple_type);
  declareInterface< CalibSegmentPreparationTool >(this);
}

////////////////
// initialize //
////////////////
StatusCode CalibExtraNtupleLoader::initialize(void) {
  ATH_MSG_INFO( "Filelist is '"<<m_filelist<<"'" );
  //convert Ntuple Type
  if(m_ntuple_type == "AUTO") {
    m_ntuple_type_num = 0;
  } else if (m_ntuple_type == "NORMAL")	{
    m_ntuple_type_num = 1;
  } else if (m_ntuple_type == "REGION") {
    m_ntuple_type_num = 2;
  } else {
    ATH_MSG_FATAL("Invalid ntuple type. Must be AUTO/NORMAL/REGION!");
    return StatusCode::FAILURE;
  }
//read filelist an build chain
  m_chain = new TChain("/PatternNtupleMaker/Segments");
  std::string sdummy;
  int count(0);
  std::ifstream fl(m_filelist.c_str());
  if(fl.fail())	{
    ATH_MSG_INFO("Cannot open file '"<<m_filelist<<"' for reading!");
    return  StatusCode::FAILURE;
  }
  while (!fl.eof()) {
    fl >> sdummy;
    if (fl.eof()) 
      break;
    bool is_region(false);
    if(m_ntuple_type_num == 3)
      is_region = true;
    if(m_ntuple_type_num == 0) {
      TFile testf(sdummy.c_str());
      if(testf.GetKey("PatternNtupleMaker") == NULL){
	is_region = true;
      }	else {
	is_region = false;
      }
    }
    if (is_region) {
      if(p_reg_sel_svc == NULL)	{
	StatusCode sc=service("RegionSelectionSvc", p_reg_sel_svc);
	if(!sc.isSuccess()) {
	  ATH_MSG_ERROR("Cannot retrieve RegionSelectionSvc!");
	  return sc;
	}	
      }
      ATH_MSG_INFO("Added "<<p_reg_sel_svc->AddRegionNtuples(sdummy.c_str(), m_chain)<<" regions from file "<<sdummy);
    } else {
      m_chain->Add(sdummy.c_str());
      ATH_MSG_INFO("Added file '"<<sdummy<<"' to filelist!");
    }
    count++;
  }
  if(count==0){
    ATH_MSG_FATAL("No files in filelist!");
    return StatusCode::FAILURE;
  }
  m_reader = new NTReader_E(m_chain);
  return StatusCode::SUCCESS;	
}  //end CalibExtraNtupleLoader::initialize

/////////////////////
// prepareSegments //
/////////////////////
void CalibExtraNtupleLoader::prepareSegments(const MuonCalibEvent *&event, std::map<NtupleStationId, MuonCalibSegment *> & /*segments*/){
  if(m_last>0 && m_last<=m_first) {
    ATH_MSG_INFO("Enough events read!");
    event=NULL;
    return;
  }
  const MuonCalibEvent_E &evt = m_reader->getEvent(m_first);	
  event = static_cast<const MuonCalibEvent *>(&evt);
  if( event->rawHitCollection() == NULL ) {
    ATH_MSG_INFO("End of file reached");
    event=NULL;
    return;
  }
  m_first++;
}  //end CalibExtraNtupleLoader::prepareSegments

} //namespace MuonCalib
