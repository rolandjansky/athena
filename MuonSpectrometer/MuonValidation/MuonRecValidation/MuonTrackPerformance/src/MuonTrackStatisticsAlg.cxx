/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
MuonTrackStatisticsAlg
***************************************************************************/

#include "MuonTrackPerformance/MuonTrackStatisticsAlg.h"
#include "MuonTrackPerformance/MuonTrackStatisticsTool.h"

#include "StoreGate/StoreGateSvc.h"


// INCLUDE GAUDI HEADER FILES:
#include "GaudiKernel/MsgStream.h"

#include "TMath.h"

/////////////////////////////////////////////////////////////////

//  CONSTRUCTOR:
MuonTrackStatisticsAlg::MuonTrackStatisticsAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : 
  AthAlgorithm(name, pSvcLocator),
  m_statisticsTool("MuonTrackStatisticsTool"),
  m_writeToFile (false),
  m_doTruth     (false)
{
  declareProperty("doTruth",         m_doTruth);
  declareProperty("writeToFile",     m_writeToFile);
  declareProperty("FileName",               m_fileName);


}

// INITIALIZE METHOD:
 
StatusCode MuonTrackStatisticsAlg::initialize()
{

  ATH_CHECK(m_statisticsTool.retrieve());
  ATH_CHECK(m_trackKeys.initialize());
  ATH_CHECK(m_truthKeys.initialize());
  
  for (unsigned int i=0; i<m_trackKeys.size() ; i++) m_statisticsTool->addTrackCounters(m_trackKeys[i].key());
  for (unsigned int i=0; i<m_truthKeys.size() ; i++) m_statisticsTool->addTrackCounters(m_truthKeys[i].key());
    
  return StatusCode::SUCCESS;
}
 
StatusCode MuonTrackStatisticsAlg::execute()
{
  
  ATH_MSG_DEBUG("MuonTrackStatisticsAlg in execute() ...");

  for(SG::ReadHandle<TrackCollection>& trackColl : m_trackKeys.makeHandles()){
    if(!trackColl.isValid()){
      ATH_MSG_WARNING("track collection "<<trackColl.key()<<" not valid!");
      return StatusCode::FAILURE;
    }
    if(!trackColl.isPresent()){
      ATH_MSG_DEBUG("track collection "<<trackColl.key()<<" not present");
      continue;
    }
    m_statisticsTool->updateTrackCounters(trackColl.key(),trackColl.cptr());
  }

  for(SG::ReadHandle<DetailedTrackTruthCollection>& truthMap : m_truthKeys.makeHandles()){
    if(!truthMap.isValid()){
      ATH_MSG_WARNING("truth map "<<truthMap.key()<<" not valid!");
      return StatusCode::FAILURE;
    }
    if(!truthMap.isPresent()){
      ATH_MSG_DEBUG("truth map "<<truthMap.key()<<" not present");
      continue;
    }
    m_statisticsTool->updateTruthTrackCounters(truthMap.key(),truthMap.cptr());
  }
  
  return StatusCode::SUCCESS;
  
}


/////////////////////////////////////////////////////////////////
// ATHENA FINALIZE:


StatusCode MuonTrackStatisticsAlg::finalize() 

{

  ATH_MSG_INFO(std::endl << m_statisticsTool->printTrackCounters());


  //write to file
  if (m_writeToFile){
    m_fileOutput.open(m_fileName.c_str(), std::ios::trunc);

    m_fileOutput <<  m_statisticsTool->printTrackCounters() << std::endl;
        
    m_fileOutput.close();
  }

  return StatusCode::SUCCESS;
}

 



void
MuonTrackStatisticsAlg::storeTruthTracks(void)
{
  /*
    const McEventCollection* mcCollection = 0;
    std::string key = "TruthEvent";
    StatusCode sc = p_SGevent->retrieve(mcCollection,key);
    if (sc.isFailure()) {
    *m_log << MSG::ERROR << "Could not find the McEventCollection" << endmsg;
    return;
    }

    const TrackRecordCollection* recordCollection = 0;
    std::string recordKey = "MuonEntryLayer";
    if (!(p_SGevent->retrieve(recordCollection, recordKey))) {
    *m_log << MSG::WARNING << "Could not find the TrackRecordCollection" << endmsg;
    }

    m_nkine = 0;

    const HepMC::GenEvent* event = *mcCollection->begin();   
    HepMC::GenEvent::particle_const_iterator particle = event->particles_begin();
    HepMC::GenEvent::particle_const_iterator particle_end = event->particles_end();

    for ( ; particle != particle_end; ++particle) {	

    // select final-state muons above min calo penetration energy 
    // and in MS acceptance
    if (abs((*particle)->pdg_id()) != 13) continue;
    if ((*particle)->status() != 1) continue;
    if ((*particle)->momentum().e() < m_minProductionEnergy) continue;	    
    if (fabs((*particle)->momentum().pseudoRapidity()) > m_maxEtaMS) continue;
    //    if ((*particle)->barcode() > 10000) continue;

    const Trk::TrackParameters* genPerigee = m_truthToTrack->makePerigeeParameters(*particle);
    if( !genPerigee ) continue;

    const Trk::TrackParameters* genEntry = m_helperTool->extrapolateToMuonEntryRecord(*genPerigee,Trk::muon);
    if( !genEntry ){
    delete genPerigee;
    continue;
    }
    HepPoint3D  gen_position = genPerigee->position();
    HepVector3D	gen_momentum = genPerigee->momentum();
    double      gen_charge   = (*particle)->pdg_id() < 0 ? 1. : -1.;

    HepPoint3D  extr_position = genEntry->position();
    HepVector3D	extr_momentum = genEntry->momentum();
    double      extr_charge   = (*particle)->pdg_id() < 0 ? 1. : -1.;

    ++m_nkine;

    // store the muon generated parameters
    m_xvtxg->push_back((*particle)->production_vertex()->point3d().x());
    m_yvtxg->push_back((*particle)->production_vertex()->point3d().y());
    m_zvtxg->push_back((*particle)->production_vertex()->point3d().z());
    m_a0g->push_back(genPerigee->parameters()[Trk::d0]);
    m_z0g->push_back(genPerigee->parameters()[Trk::z0]);
    m_phig->push_back((*particle)->momentum().phi());
    m_thetag->push_back((*particle)->momentum().theta());
    m_qpig->push_back(gen_charge/(*particle)->momentum().mag());
    m_etag->push_back((*particle)->momentum().pseudoRapidity());
    m_barcode->push_back((*particle)->barcode());
    m_status->push_back((*particle)->status());



  */
  
}
