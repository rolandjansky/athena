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
  p_SGevent(0),
  m_statisticsTool("MuonTrackStatisticsTool"),
  m_writeToFile (false),
  m_doTruth     (false),
  m_log(0)  
{
  declareProperty("TrackLocationList",          m_trackLocationList);
  declareProperty("doTruth",         m_doTruth);
  declareProperty("writeToFile",     m_writeToFile);
  // printout level (0= no printout, 5=max printout)
  declareProperty("print_level", m_print_level);
  declareProperty("FileName",               m_fileName);


}

// INITIALIZE METHOD:
 
StatusCode MuonTrackStatisticsAlg::initialize()
{
  // MSGStream object to output messages from your sub algorithm
  m_log = new MsgStream(messageService(), name());
 
  // Locate the StoreGateSvc and initialize our local ptr
  StatusCode sc = service("StoreGateSvc", p_SGevent);
  if (!sc.isSuccess() || 0 == p_SGevent) 
    {
      *m_log << MSG::ERROR << "MuonTrackStatisticsAlg::initialize() :  Could not find StoreGateSvc" << endreq;
      return	sc;
    }

  sc = m_statisticsTool.retrieve();
  if (sc.isSuccess()){
    *m_log<<MSG::INFO << "Retrieved " << m_statisticsTool << endreq;
  }else{
    *m_log<<MSG::ERROR<<"Could not get " << m_statisticsTool <<endreq; 
    return sc; 
  }
  
  int listLength = m_trackLocationList.size();
  for (int i=0; i<listLength ; i++) m_statisticsTool->addTrackCounters(m_trackLocationList[i]);
    
  return StatusCode::SUCCESS;
}
 
StatusCode MuonTrackStatisticsAlg::execute()
{
  
  *m_log << MSG::DEBUG << "MuonTrackStatisticsAlg in execute() ..." << endreq;

  StatusCode sc = m_statisticsTool->updateTrackCounters();
  sc.ignore();
  
  return StatusCode::SUCCESS;
  
}


/////////////////////////////////////////////////////////////////
// ATHENA FINALIZE:


StatusCode MuonTrackStatisticsAlg::finalize() 

{

  *m_log << MSG::INFO << std::endl << m_statisticsTool->printTrackCounters() << endreq;


  //write to file
  if (m_writeToFile){
    m_fileOutput.open(m_fileName.c_str(), std::ios::trunc);

    m_fileOutput <<  m_statisticsTool->printTrackCounters() << std::endl;
        
    m_fileOutput.close();
  }


  
  delete m_log;
  
  
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
    *m_log << MSG::ERROR << "Could not find the McEventCollection" << endreq;
    return;
    }

    const TrackRecordCollection* recordCollection = 0;
    std::string recordKey = "MuonEntryLayer";
    if (!(p_SGevent->retrieve(recordCollection, recordKey))) {
    *m_log << MSG::WARNING << "Could not find the TrackRecordCollection" << endreq;
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
