/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTruthSummaryTool.h"
#include "MuonSegment/MuonSegment.h"
#include "TrkTrack/Track.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "HepMC/GenParticle.h"
#include <iostream>

namespace Muon {

  /** Constructor **/
  MuonTruthSummaryTool::MuonTruthSummaryTool(const std::string& t, const std::string& n, const IInterface* p) : 
    AthAlgTool(t,n,p),
    m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool"),
    m_helper("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
    m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
    m_incidentSvc("IncidentSvc",n),
    m_wasInit(false)
  {
    declareInterface<IMuonTruthSummaryTool>(this);
    declareProperty("CSC_TruthName",    m_CSC_TruthName = "CSC_TruthMap");
    declareProperty("RPC_TruthName",    m_RPC_TruthName = "RPC_TruthMap");
    declareProperty("TGC_TruthName",    m_TGC_TruthName = "TGC_TruthMap");
    declareProperty("MDT_TruthName",    m_MDT_TruthName = "MDT_TruthMap");
    declareProperty("MM_TruthName",     m_MM_TruthName  = "MM_TruthMap");
    declareProperty("STGC_TruthName",   m_STGC_TruthName = "STGC_TruthMap");
  }

  StatusCode  MuonTruthSummaryTool::initialize()
  {
    ATH_MSG_VERBOSE("Initializing ...");
    if (m_idHelper.retrieve().isFailure()) { 
      msg(MSG::FATAL) << "Could not get " << m_idHelper << endreq;
      return StatusCode::FAILURE; 
    }

    if (m_printer.retrieve().isFailure()) { 
      msg(MSG::FATAL) << "Could not get " << m_printer << endreq;
      return StatusCode::FAILURE; 
    }

    if (m_helper.retrieve().isFailure()) { 
      msg(MSG::FATAL) << "Could not get " << m_helper << endreq;
      return StatusCode::FAILURE; 
    }

    // call handle in case of EndEvent
    if( m_incidentSvc.retrieve().isFailure() ) {
      ATH_MSG_ERROR("Could not get " << m_incidentSvc);
      return StatusCode::FAILURE;      
    }
    m_incidentSvc->addListener( this, std::string("EndEvent"));
    return StatusCode::SUCCESS;
  }

                                    

  StatusCode  MuonTruthSummaryTool::finalize() {
    return StatusCode::SUCCESS;
  }

  void MuonTruthSummaryTool::clear() {
    m_wasInit = false;
    m_truthHits.clear();
    m_truthDataPerLevel.clear();
  }

  void MuonTruthSummaryTool::init() {
    getTruth(m_CSC_TruthName);
    getTruth(m_RPC_TruthName);
    getTruth(m_TGC_TruthName);
    getTruth(m_MDT_TruthName);
    getTruth(m_MM_TruthName);
    getTruth(m_STGC_TruthName);
    m_wasInit = true;
    ATH_MSG_DEBUG(" Total collected muon truth hits " << m_truthHits.size() ); 
  }

  void MuonTruthSummaryTool::getTruth(std::string name ) {
    const PRD_MultiTruthCollection* col = 0;

    if( !evtStore()->contains<PRD_MultiTruthCollection>(name) ) return; 

    if( evtStore()->retrieve(col, name).isFailure() || !col ) {
      ATH_MSG_WARNING(  "PRD_MultiTruthCollection " << name << " NOT found");
      return;
    }
    ATH_MSG_DEBUG(  "PRD_MultiTruthCollection " << name << " found");
    PRD_MultiTruthCollection::const_iterator it = col->begin();
    PRD_MultiTruthCollection::const_iterator it_end = col->end();
    for( ;it!=it_end;++it ){
      const HepMcParticleLink& link = it->second;
      if( link.cptr() && abs(link.cptr()->pdg_id()) == 13 ) m_truthHits.insert(it->first);
    }
  }

  void MuonTruthSummaryTool::add( const Identifier& id, int level ) {
    if( !m_wasInit) init();
    if( m_truthHits.count(id) ) m_truthDataPerLevel[level].insert(id);
  }

  void MuonTruthSummaryTool::add( const MuonSegment& seg, int level ) {
    add(seg.containedMeasurements(),level);
  }

  void MuonTruthSummaryTool::add( const Trk::Track& track, int level ) {
    const DataVector<const Trk::MeasurementBase>* meas = track.measurementsOnTrack();                
    if( meas ) add(meas->stdcont(),level);
  }

  void MuonTruthSummaryTool::add( const std::vector<const Trk::MeasurementBase*>& measurements, int level ) {
    for( std::vector<const Trk::MeasurementBase*>::const_iterator it = measurements.begin();it!=measurements.end();++it ){
      Identifier id = m_helper->getIdentifier(**it);
      if( id.is_valid() && m_idHelper->isMuon(id) ) add(id,level);
    }
  }
  
  std::string MuonTruthSummaryTool::printSummary() const {
    if( m_truthHits.empty() )  return "Event without truth hits";
    if( m_truthDataPerLevel.empty() ) return "No hits added";
    ATH_MSG_DEBUG( " printing summary " << m_truthHits.size() << " levels filled " << m_truthDataPerLevel.size() );
    std::ostringstream sout;
    sout << " Summarizing: truth hits " << m_truthHits.size() << " levels filled " << m_truthDataPerLevel.size();
    std::map<int,std::set<Identifier> >::const_iterator it = m_truthDataPerLevel.begin();
    std::map<int,std::set<Identifier> >::const_iterator it_end = m_truthDataPerLevel.end();
    for( ;it!=it_end;++it ){
      sout << " Comparing truth to level " << it->first << std::endl
	   << printSummary( m_truthHits, it->second );
    }
    return sout.str();
  }

  std::string MuonTruthSummaryTool::printSummary( const std::set<Identifier>& truth, const std::set<Identifier>& found ) const {
    std::ostringstream sout;
    if( truth.size() != found.size() ){
      sout << " Some truth hits not found: truth " << truth.size() << " found " << found.size() << std::endl;
      std::vector<Identifier> result(truth.size()-found.size());
      std::vector<Identifier>::iterator pos = std::set_difference(truth.begin(),truth.end(),found.begin(),found.end(),result.begin());
      result.resize(pos-result.begin());
      int nmm = 0;
      for( std::vector<Identifier>::iterator it=result.begin();it!=result.end();++it ){
	if(m_idHelper->isMM(*it)  ) {
	  sout << "  " << m_idHelper->toString(*it) << std::endl;
	  ++nmm;
	}
	if(m_idHelper->issTgc(*it) ) sout << "  " << m_idHelper->toString(*it) << std::endl;
      }
      if( nmm > 4 ) sout << " possible missing MM segment : " << nmm << std::endl;
    }else{
      sout << " All hits found: truth " << truth.size() << " found " << found.size() << std::endl;
    }
    return sout.str();
  }

  void MuonTruthSummaryTool::handle(const Incident& inc) {

    // Only clear cache for EndEvent incident
    if (inc.type() == "EndEvent") {
      ATH_MSG_DEBUG( printSummary() );
      clear();
    }     
  }  
}
