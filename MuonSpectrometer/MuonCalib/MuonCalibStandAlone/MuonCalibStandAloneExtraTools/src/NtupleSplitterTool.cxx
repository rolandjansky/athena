/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//c - c++
#include "iostream"
#include "fstream"
#include "sstream"
#include <string.h>

//root
#include "TFile.h"
#include "TTree.h"


//MuonCalibEventBase
#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "MuonCalibEventBase/MuonCalibEvent.h"
#include "MuonCalibEventBase/MuonCalibPattern.h"
#include "MuonCalibEventBase/MuonCalibTruthCollection.h"
#include "MuonCalibEventBase/MuonCalibTruth.h"
#include "MuonCalibEventBase/MuonCalibMdtTruthHit.h"
#include "MuonCalibEventBase/MuonCalibRpcTruthHit.h"
#include "MuonCalibEventBase/MuonCalibTgcTruthHit.h"
#include "MuonCalibEventBase/MuonCalibCscTruthHit.h"
#include "MuonCalibEventBase/MuonCalibRawHitCollection.h"
#include "MuonCalibEventBase/MuonCalibRawMdtHit.h"
#include "MuonCalibEventBase/MuonCalibRawRpcHit.h"
#include "MuonCalibEventBase/MuonCalibRawTgcHit.h"

//MuonCalibExtraTreeAlg
#include "MuonCalibExtraTreeAlg/MuonCalibHit_EBranch.h"
#include "MuonCalibExtraTreeAlg/MuonCalibTrack_EBranch.h"

//MuonCalibExtraTreeEvent
#include "MuonCalibExtraTreeEvent/MuonCalibEvent_E.h"
#include "MuonCalibExtraTreeEvent/MuonCalibHit_E.h"
#include "MuonCalibExtraTreeEvent/MuonCalibTrack_E.h"


//MuonCalibIdentifier
#include "MuonCalibIdentifier/MuonFixedId.h"

//MuonCalibNtuple
#include "MuonCalibNtuple/MuonCalibBranchNtuple.h"
#include "MuonCalibNtuple/RootFileManager.h"

//MuonCalibStandAloneBase
#include "MuonCalibStandAloneBase/RegionSelectionSvc.h"

//this
#include "NtupleSplitterTool.h"


namespace MuonCalib {

NtupleSplitterTool::NtupleSplitterTool(const std::string &t, const std::string &n, const IInterface *p): 
  AthAlgTool(t, n, p), m_split_level(1), m_ntuple_split_level(2), 
  m_calib_tool_handle("MuonCalib::NtupleRunScanTool"), m_file_number(0), m_stored_patterns(0) {
  declareInterface< NtupleCalibrationTool >(this);
  declareProperty("FileSplitLevel",m_split_level );
  declareProperty("NtupleSplitLevel", m_ntuple_split_level);
  declareProperty("ToolToRun", m_calib_tool_handle);
  m_max_patterns=0;
  declareProperty("MaxPatterns", m_max_patterns);
  m_filename_prefix="Ntuple";
  declareProperty("FilenamePrefix", m_filename_prefix);
  m_store_truth = false;
  declareProperty("StoreTruth", m_store_truth);
  m_store_mdt_rawdata = false;
  declareProperty("StoreMdtRawdata", m_store_mdt_rawdata);
  m_adc_cut =0;
  declareProperty("AdcCut", m_adc_cut);
  m_store_rpc_rawdata=false;
  declareProperty("StoreRpcRawdata", m_store_rpc_rawdata);
  m_store_tgc_rawdata=false;
  declareProperty("StoreTgcRawdata", m_store_tgc_rawdata);
  m_trigger_only_segment_stations = true;
  declareProperty("TriggerForSegmentStationOnly", m_trigger_only_segment_stations);
  m_store_raw_triger = false;
  declareProperty("StoreRawTriggerData", m_store_raw_triger);
  declareProperty("FillTrackAuthors", m_track_authors);
  m_all_fill_tracks=false;
  m_track_p_cut=-3.;
  declareProperty("TrackPCut", m_track_p_cut);
}  //end NtupleSplitterTool::NtupleSplitterTool

StatusCode NtupleSplitterTool::initialize() {
//get region selection service
  StatusCode sc=service("RegionSelectionSvc", p_reg_sel_svc);
  if(!sc.isSuccess()) {
    ATH_MSG_ERROR("Cannot retrieve RegionSelectionSvc!");;
    return sc;
  }
  sc = m_calib_tool_handle.retrieve();
  if (m_ntuple_split_level<m_split_level) {
    ATH_MSG_WARNING("Setting ntuple split level to file split level!");
    m_ntuple_split_level = m_split_level;
  }
  for(std::vector<int>::iterator it=m_track_authors.begin(); it!=m_track_authors.end(); it++) {
    if ((*it)<0) {
      m_all_fill_tracks=true;
      break;
    }			
    m_track_authors_set.insert(*it);
  }
  return sc;
}  //end NtupleSplitterTool::initialize

StatusCode NtupleSplitterTool::finalize() {
  RootFileManager * rfm=RootFileManager::getInstance();
  rfm->closeFile();
  return StatusCode::SUCCESS;
}

StatusCode NtupleSplitterTool::handleEvent(const MuonCalibEvent &event, int evnt_nr, const std::vector<MuonCalibSegment *> &segments, unsigned int position) {
//loop over al lnew segments
  m_stations_with_seg_hits.clear();
  std::map<NtupleStationId, MuonCalibPattern *> region_patterns;
  for (unsigned int i=position; i<segments.size(); i++)	{
    MuonCalibSegment *segment=segments[i];
    //get list of regions
    std::set<NtupleStationId> current_regions;
    for(MuonCalibSegment::MdtHitCit it=segment->mdtHOTBegin(); it!=segment->mdtHOTEnd(); it++) {
      NtupleStationId c_id((*it)->identify());
      c_id.SetMultilayer(0);
      m_stations_with_seg_hits.insert(c_id);
      c_id=ntuple_region_id((*it)->identify());
      //create new pattern if needed
      if(region_patterns[c_id]==NULL) {
	region_patterns[c_id] = new MuonCalibPattern();
      }
      //fill segment in patttern, if it is not already inserted
      if(current_regions.find(c_id)==current_regions.end()) {
	region_patterns[c_id]->addMuonSegment(new MuonCalibSegment(*segment));
	current_regions.insert(c_id);
      }
    }
  }
		
//fill patterns
  m_ntuples_initialized.clear();
  for(std::map<NtupleStationId,MuonCalibPattern * >::iterator it=region_patterns.begin(); it!=region_patterns.end(); it++) {
    m_stored_patterns++;
    MuonCalibBranchNtuple *ntpl(get_ntuple(it->first));
    m_ntuples_initialized.insert(it->first);
    ntpl->fillEventToTree(event.eventInfo());
    ntpl->handlePattern(*(it->second));
  }

//fill rawdata collections
  const MuonCalibRawHitCollection *raw_hit_coll(event.rawHitCollection());
  if(raw_hit_coll!=NULL) {
    m_rawdata_collections.clear();
    //mdt hits
    if(m_store_mdt_rawdata)
      process_rawdata<MuonCalibRawMdtHit>( raw_hit_coll->rawMdtHitCollectionBegin(), raw_hit_coll->rawMdtHitCollectionEnd());
    if(m_store_rpc_rawdata)
      process_rawdata<MuonCalibRawRpcHit>( raw_hit_coll->rawRpcHitCollectionBegin(), raw_hit_coll->rawRpcHitCollectionEnd());
    if(m_store_tgc_rawdata)
      process_rawdata<MuonCalibRawTgcHit>( raw_hit_coll->rawTgcHitCollectionBegin(), raw_hit_coll->rawTgcHitCollectionEnd());
    
    for(std::map<NtupleStationId, MuonCalibRawHitCollection *>::iterator it = m_rawdata_collections.begin(); it!= m_rawdata_collections.end(); it++) {
      MuonCalibBranchNtuple *ntpl(get_ntuple(it->first));
      if(m_ntuples_initialized.find(it->first) == m_ntuples_initialized.end()) {
	m_ntuples_initialized.insert(it->first);
	ntpl->fillEventToTree(event.eventInfo());
      }
      ntpl->handleRawHits(*(it->second));
    }
  }

//loop on truth
  const MuonCalibTruthCollection *truth(event.calibTruthCollection());
  if(truth != NULL && m_store_truth) {
    m_truth_ntuples.clear();
//fill mdt truth
    fill_truth(truth->mdtTruthCollectionBegin(), truth->mdtTruthCollectionEnd(), event);
//fill rpc truth
    fill_truth(truth->rpcTruthCollectionBegin(), truth->rpcTruthCollectionEnd(), event);
//fill tgc truth
    fill_truth(truth->tgcTruthCollectionBegin(), truth->tgcTruthCollectionEnd(), event);
//fill csc truth
    fill_truth(truth->cscTruthCollectionBegin(), truth->cscTruthCollectionEnd(), event);
  }//if(truth != NULL && m_store_truth)
//fill ntuples
	
//fill tracks
  if(m_track_authors.size()) {
    try {
      const MuonCalibEvent_E & extra_event = dynamic_cast<const MuonCalibEvent_E &>(event);
      fillTrackNtuple(extra_event);
    }
    catch(std::bad_cast exp) {
      ATH_MSG_WARNING("Not reading extra tree, but track writing is requested!");
      m_track_authors.clear();
    }
  }	
		
  for(std::set<NtupleStationId>::const_iterator it=m_ntuples_initialized.begin();  it != m_ntuples_initialized.end(); it++) {
    MuonCalibBranchNtuple *ntpl(m_ntuples[*it]);
    if(!ntpl) continue;
    if(m_store_raw_triger)
      ntpl->handleRawTriggerHits( *event.rawTriggerHitCollection() );
    ntpl->finishEvent();
  }
//clear
  for(std::map<NtupleStationId,MuonCalibPattern * >::iterator it=region_patterns.begin(); it!=region_patterns.end(); it++) {
    delete it->second;
  }
  for(std::map<NtupleStationId, MuonCalibRawHitCollection *>::iterator it = m_rawdata_collections.begin(); it!= m_rawdata_collections.end(); it++) {
    delete it->second;
  }
  return m_calib_tool_handle->handleEvent(event, evnt_nr, segments, position);
}  //end NtupleSplitterTool::handleEvent

inline void NtupleSplitterTool::fillTrackNtuple(const MuonCalibEvent_E &extra_event) {
  for(std::map<NtupleStationId, MuonCalibHit_EBranch *>::iterator it=m_track_hit_branch.begin(); it!=m_track_hit_branch.end(); it++)
    (it->second)->reset();
  for(std::map<NtupleStationId, MuonCalibTrack_EBranch *>::iterator it=m_track_branch.begin(); it!=m_track_branch.end(); it++)
    (it->second)->reset();
	
  for(MuonCalibEvent_E::TrackVec::const_iterator t_it = extra_event.beginTrack(); t_it!=extra_event.endTrack(); t_it++)	{
    const MuonCalibTrack_E *track(*t_it);
    if(!m_all_fill_tracks &&  m_track_authors_set.find(track->author()) == m_track_authors_set.end()) {
      //			std::cout<<"."<<std::endl;
      continue;
    }
    if (m_track_p_cut>-1) {
      //			std::cout<<".."<<std::endl;
      if(track->qOverP()==0 || track->p()>1e8) {
	continue;
      }
      if(m_track_p_cut>=0 && std::fabs(track->p())<m_track_p_cut) {
	continue;
      }
    }
    std::list<MuonCalibHit_EBranch *> hit_branches;
    std::list<MuonCalibTrack_EBranch *> track_branches;
    std::set<NtupleStationId> station_ids;
		
    for(MuonCalibTrack_E::HitVector::const_iterator h_it=track->beginHit(); h_it!=track->endHit(); h_it++) {
      const MuonCalibHit_E *hit(*h_it);
      if(hit->type()!=1) continue;
      const MuonFixedId &id(hit->identify());
      if(!p_reg_sel_svc->isInRegion(id)) continue;
      for(std::map<NtupleStationId, MuonCalibHit_EBranch *>::const_iterator m_it=m_track_hit_branch.begin(); m_it!=m_track_hit_branch.end(); m_it++) {
	if(station_ids.find(m_it->first) != station_ids.end())
	  continue;
//				std::cout<<m_it->first.regionId()<<std::endl;
	if(m_it->first == id) {
	  station_ids.insert(m_it->first);
	  hit_branches.push_back(m_it->second);
	  track_branches.push_back( m_track_branch[m_it->first] );
	  m_ntuples_initialized.insert(m_it->first);
	}
      }
    }
    std::list<MuonCalibHit_EBranch *>::iterator hb_it=hit_branches.begin();
    std::list<MuonCalibTrack_EBranch *>::iterator tb_it=track_branches.begin();
//		std::cout<<":"<<std::endl;
    while (hb_it!=hit_branches.end() && tb_it!=track_branches.end()) {
      //			std::cout<<"."<<std::endl;
      if(!(*tb_it)->fillBranch(*track))	{
	ATH_MSG_WARNING("Overfull track branch!");
	hb_it++;
	tb_it++;
	continue;
      }
      for(MuonCalibTrack_E::HitVector::const_iterator h_it=track->beginHit(); h_it!=track->endHit(); h_it++) {
	const MuonCalibHit_E *hit(*h_it);
	if(!((*hb_it)->fillBranch(*hit, 0., (*tb_it)->getBranchEntries() - 1))) {
	  ATH_MSG_WARNING("Overfull hit branch! "<<(*hb_it)->getBranchEntries());
	}
      }	
      hb_it++;
      tb_it++;
    }
  }
}  //end NtupleSplitterTool::fillTrackNtuple

inline MuonCalibBranchNtuple* NtupleSplitterTool::get_ntuple(const NtupleStationId &id)	{
  if(m_max_patterns>0 && m_stored_patterns>=m_max_patterns) {
    RootFileManager *rfm=RootFileManager::getInstance();
    rfm->closeFile(); 
    for(std::map<NtupleStationId, MuonCalibBranchNtuple *>::iterator it=m_ntuples.begin(); it!=m_ntuples.end(); it++)
      delete it->second;
    m_ntuples.clear();
    m_stored_patterns=0;
    m_file_number++;
  }
  if(m_ntuples[id]==NULL) {
    RootFileManager *rfm=RootFileManager::getInstance();
    //generate file name
    MuonFixedId fixid;
    std::ostringstream filename;
    filename<<m_filename_prefix;
    if (m_split_level>0) {
      filename<<"_"<<fixid.stationNumberToFixedStationString( id.GetStation());
    }
    if (m_split_level>1) {
      filename<<"_"<<id.GetPhi();
    }
    if (m_split_level>2) {
      filename<<"_"<<id.GetEta();
    }
    filename<<"."<<m_file_number<<".root";
    rfm->openFile(filename.str());
    //generate region id
    std::ostringstream region_id;
    if (m_ntuple_split_level<1)	{
      region_id<<"PatternNtupleMaker";
    } else {
      region_id<<fixid.stationNumberToFixedStationString( id.GetStation());
    }
    if(m_ntuple_split_level>1) {
      region_id<<"_"<<id.GetPhi();
    }
    if(m_ntuple_split_level>2) {
      region_id<<"_"<<id.GetEta();
    }
    MuonCalibBranchNtuple *ntup=new MuonCalibBranchNtuple(region_id.str());
    m_ntuples[id] = ntup;
    MuonCalibHit_EBranch *h_br=new MuonCalibHit_EBranch();
    h_br->createBranch(ntup->getTTree());
    m_track_hit_branch[id] = h_br;
    MuonCalibTrack_EBranch *t_br=new MuonCalibTrack_EBranch();
    t_br->createBranch(ntup->getTTree());
    m_track_branch[id] = t_br;
  }
  return m_ntuples[id];
}  //end NtupleSplitterTool::get_ntuple

inline NtupleStationId NtupleSplitterTool::ntuple_region_id(const MuonFixedId &id) {
  if (m_ntuple_split_level<1) {
    NtupleStationId c_id;
    return c_id;
  }
  NtupleStationId c_id(id);
  c_id.SetMultilayer(0);
  if (m_ntuple_split_level<3)
    c_id.SetEta(-99);
  if (m_ntuple_split_level<2)
    c_id.SetPhi(-1);
  return c_id;
}

inline void NtupleSplitterTool::add_truth(const MuonCalibTruthCollection *truth, MuonCalibBranchNtuple *ntpl) {
  for(MuonCalibTruthCollection::TruthVecCit it=truth->truthCollectionBegin(); it!=truth->truthCollectionEnd(); it++) {
    ntpl->fillTruthToTree(**it);
  }
}

inline MuonCalibBranchNtuple * NtupleSplitterTool ::get_ntuple_for_truth(const NtupleStationId &c_id, const MuonCalibEvent &event) {
  const MuonCalibTruthCollection *truth(event.calibTruthCollection());
  MuonCalibBranchNtuple *ntpl=m_truth_ntuples[c_id];
  if(ntpl==NULL) {
    ntpl = get_ntuple(c_id);
    m_truth_ntuples[c_id] = ntpl;
    add_truth(truth, ntpl);
    if(m_ntuples_initialized.find(c_id) == m_ntuples_initialized.end())	{
      ntpl->fillEventToTree(event.eventInfo());
      m_ntuples_initialized.insert(c_id);
      m_truth_ntuples[c_id] = ntpl;
    }
  }
  return ntpl;
}

template<typename T, typename Tit>
void  NtupleSplitterTool ::process_rawdata(Tit  begin, Tit  end) {
  for(Tit it = begin; it != end; it++) {
    if(!p_reg_sel_svc->isInRegion((*it)->identify()))
      continue;
    if(!rawdata_selector(**it)) continue;
    NtupleStationId c_id(ntuple_region_id((*it)->identify()));
    MuonCalibRawHitCollection * coll(m_rawdata_collections[c_id]);
    if (coll==NULL) {
      coll = new MuonCalibRawHitCollection();
      m_rawdata_collections[c_id] =coll;
    }
    coll->addMuonCalibRawHit(new T(**it));
  }
}

inline bool NtupleSplitterTool ::rawdata_selector(const MuonCalibRawMdtHit &hit) {
  return hit.adcCount() >= m_adc_cut;
}

inline bool NtupleSplitterTool ::rawdata_selector(const MuonCalibRawRpcHit &hit) {
  if(!m_trigger_only_segment_stations)
    return true;
	
  const MuonFixedId &id(hit.identify());
  NtupleStationId s_id(id.stationNameString(), id.eta(), id.phi());
  return m_stations_with_seg_hits.find(s_id) != m_stations_with_seg_hits.end();
}

inline bool NtupleSplitterTool ::rawdata_selector(const MuonCalibRawTgcHit &hit) {
  if(!m_trigger_only_segment_stations)
    return true;
	
  const MuonFixedId &id(hit.identify());
  NtupleStationId s_id(id.stationNameString(), id.eta(), id.phi());
  return m_stations_with_seg_hits.find(s_id) != m_stations_with_seg_hits.end();
}
	
template<typename Tit>
void NtupleSplitterTool ::fill_truth(Tit begin, Tit end, const MuonCalibEvent &event) {
  for (Tit it=begin; it!=end; it++) {
    if(!p_reg_sel_svc->isInRegion((*it)->identify()))
      continue;
    NtupleStationId c_id(ntuple_region_id((*it)->identify()));
    MuonCalibBranchNtuple *ntpl=get_ntuple_for_truth(c_id, event);
    fill_truth_hit(**it, ntpl);
  }
}

inline void NtupleSplitterTool ::fill_truth_hit(const MuonCalibMdtTruthHit &it, MuonCalibBranchNtuple *ntpl) {
  ntpl->fillMdtTruthToTree(it);
}
	
inline void NtupleSplitterTool ::fill_truth_hit(const MuonCalibRpcTruthHit &it, MuonCalibBranchNtuple *ntpl) {
  ntpl->fillRpcTruthToTree(it);
}
	
inline void NtupleSplitterTool ::fill_truth_hit(const MuonCalibTgcTruthHit &it, MuonCalibBranchNtuple *ntpl) {
  ntpl->fillTgcTruthToTree(it);
}
	
inline void NtupleSplitterTool ::fill_truth_hit(const MuonCalibCscTruthHit &it, MuonCalibBranchNtuple *ntpl) {
  ntpl->fillCscTruthToTree(it);
}

}  //namespace MuonCalib
