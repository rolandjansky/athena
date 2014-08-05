/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBRec/TBBeamQualityMC.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"

#include "TBEvent/TBEventInfo.h"
#include "TBEvent/TBTrack.h"
#include "LArG4TBSimEvent/LArG4H6FrontHitCollection.h"
#include "CaloEvent/CaloClusterContainer.h"
#include <fstream>
#include <string>
#include <math.h>
#include "PathResolver/PathResolver.h"


TBBeamQualityMC::TBBeamQualityMC(const std::string & name, ISvcLocator * pSvcLocator) :
  Algorithm(name, pSvcLocator),
  m_readFileforXcryo(true),
  m_nRun(0)
{
   declareProperty("ReadFileforXcryo", m_readFileforXcryo);
   declareProperty("CheckTrackParams",m_check_trackpar=false);
   declareProperty("TrackCutX",m_bm_cut_x=35.);
   declareProperty("TrackCutY",m_bm_cut_y=35.);
   declareProperty("CheckPrimaryTrack",m_check_primary=true);
   declareProperty("ScintForPrimaryTrack",m_scint_prim);
   declareProperty("CheckVetoScint",m_check_veto=true);
   declareProperty("CheckClusters",m_check_clus=false);
   declareProperty("CheckTrackReco",m_check_trackreco=false);
   declareProperty("ClusterCollectionName",m_clusterCollName="CaloTopoCluster");

   m_eventStore = 0;
}

StatusCode TBBeamQualityMC::initialize() {
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "in initialize()"  << endreq;

  StatusCode sc = service("StoreGateSvc", m_eventStore);
  if (sc.isFailure()) {
    log << MSG::ERROR << "Unable to retrieve pointer to StoreGate Service" << endreq;
    return sc;
  }

  return StatusCode::SUCCESS; 
}

StatusCode TBBeamQualityMC::execute() {
  /// Print an informatory message:
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "in execute()" << endreq;
  
  StatusCode sc;
  
  // Retrieve Event Info
  const TBEventInfo* theEventInfo;
  sc = m_eventStore->retrieve(theEventInfo,"TBEventInfo");
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "Cannot retrieve TBEventInfo from StoreGate" << endreq;
    return StatusCode::FAILURE;
  }
  else {
    log << MSG::VERBOSE << "retrieved theEventInfo" << endreq;
  }

  // Fill run header
  m_nRun = theEventInfo->getRunNum();
  float beamMom = theEventInfo->getBeamMomentum();
  float xCryo   = -9999;
  float yTable  = -9999;
  if(m_readFileforXcryo) {
    if (!this->getXcryoYtable(xCryo, yTable, beamMom)) {
      log << MSG::ERROR << "xCryo and yTable are not found" << endreq;
      return StatusCode::FAILURE;
    }
  } else {
     xCryo = theEventInfo->getCryoX();
     yTable = theEventInfo->getTableY();
  }
  log << MSG::VERBOSE << "xCryo   = " << xCryo   << endreq;
  log << MSG::VERBOSE << "yTable  = " << yTable  << endreq;
  log << MSG::VERBOSE << "nRun    = " << m_nRun    << endreq;
  log << MSG::VERBOSE << "beamMom = " << beamMom << endreq;


  if(m_check_trackpar) { // check for track parameters
     
     TBTrack *track;
     sc = m_eventStore->retrieve(track,"Track");
     if (sc.isFailure()){
       log << MSG::ERROR << "Retrieval of beam track failed" << endreq;
       return StatusCode::FAILURE;
     }

     const float zCalo = 30000.;
     float beam_coor_x = track->getUslope()*zCalo + track->getUintercept() + xCryo;
     float beam_coor_y = track->getVslope()*zCalo + track->getVintercept();
     
     bool sx1 = (beam_coor_x > xCryo - m_bm_cut_x) && (beam_coor_x < xCryo + m_bm_cut_x); 
     bool sx2 = (std::log(track->getChi2_u())>-10 && track->getChi2_u()!=1000);
     bool sx3 = (beam_coor_x!=0 || track->getUintercept()!=0 || track->getUslope()!=0 );

     bool sy1 = (beam_coor_y > yTable - m_bm_cut_y) && (beam_coor_y < yTable + m_bm_cut_y); 
     bool sy2 = (std::log(track->getChi2_v())>-10 && track->getChi2_v()!=1000);
     bool sy3 = (beam_coor_y!=0 || track->getVintercept()!=0 || track->getVslope()!=0 );

     if(! (sx1 && sx2 && sx3 && sy1 && sy2 && sy3)) {
        log << MSG::DEBUG << "CheckTrackParams failed: "<<sx1<<" "<<sx2<<" "<<sx3<<" / "<<sy1<<" "<<sy2<<" "<<sy3<<endreq; 
        //return StatusCode::FAILURE;
        //return StatusCode::RECOVERABLE;
        setFilterPassed(false);
        return StatusCode::SUCCESS;
     }
  }

  if(m_check_primary) { // check for primary energy deposition in vector of scintilators

     std::vector<bool> has_energy (m_scint_prim.size());
     for(int ll=0; ll<(int)m_scint_prim.size(); ++ll) has_energy[ll]=false;

     //LArG4H6FrontHitCollection *frontcoll;
     const DataHandle<LArG4H6FrontHitCollection> frontcoll;
     sc = m_eventStore->retrieve(frontcoll,"Front::Hits");
     if (sc.isFailure()){
       log << MSG::ERROR << "Retrieval of FrontHits failed" << endreq;
       return StatusCode::FAILURE;
     }
     int scnum;
     unsigned i;
     LArG4H6FrontHitConstIterator f_it = frontcoll->begin();
     LArG4H6FrontHitConstIterator f_end = frontcoll->end();
     for ( ; f_it!=f_end; ++f_it) {
         LArG4H6FrontHit* hit = (*f_it);
         scnum = hit->GetSC();
         if(scnum  <= 0) continue; // not a scintilator hit
         for(i=0; i<m_scint_prim.size(); ++i) {
            log << MSG::DEBUG << "Scint: "<<scnum<<", track: "<<hit->GetTrackID()<<", energy: "<<hit->GetEdep()<<endreq;
            if( (scnum==m_scint_prim[i]) && (hit->GetTrackID() == 1) && (hit->GetEdep() > 0.)) {
               has_energy[i] = true;
            }
         }
     }

     //LArG4H6FrontHitCollection *movecoll;
     const DataHandle<LArG4H6FrontHitCollection> movecoll;
     sc = m_eventStore->retrieve(movecoll,"Movable::Hits");
     if (sc.isFailure()){
       log << MSG::ERROR << "Retrieval of MovableHits failed" << endreq;
       return StatusCode::FAILURE;
     }
     LArG4H6FrontHitConstIterator m_it = movecoll->begin();
     LArG4H6FrontHitConstIterator m_end = movecoll->end();
     for ( ; m_it!=m_end; ++m_it) {
         LArG4H6FrontHit* hit = (*m_it);
         scnum = hit->GetSC();
         if(scnum  <= 0) continue; // not a scintilator hit
         for(i=0; i<m_scint_prim.size(); ++i) {
            log << MSG::DEBUG << "Scint: "<<scnum<<", track: "<<hit->GetTrackID()<<", energy: "<<hit->GetEdep()<<endreq;
            if( (scnum==m_scint_prim[i]) && (hit->GetTrackID() == 1) && (hit->GetEdep() > 0.)) {
               has_energy[i] = true;
            }
         }
     }
     for(int ll=0; ll<(int)m_scint_prim.size(); ++ll) has_energy[0] = has_energy[0] && has_energy[ll];      
     if(!has_energy[0]) {
        log << MSG::DEBUG << "CheckPrimaryTrack failed, no energy deposit in all asked scint." <<endreq; 
        //return StatusCode::FAILURE;
        //return StatusCode::RECOVERABLE;
        setFilterPassed(false);
        return StatusCode::SUCCESS;
     }
  }

  if(m_check_veto) { // check if there is a hit in veto scint.
     int scnum;
     // LArG4H6FrontHitCollection *movecoll;
     const DataHandle<LArG4H6FrontHitCollection> movecoll;
     sc = m_eventStore->retrieve(movecoll,"Movable::Hits");
     if (sc.isFailure()){
       log << MSG::ERROR << "Retrieval of FrontHits failed" << endreq;
       return StatusCode::FAILURE;
     }
     LArG4H6FrontHitConstIterator m_it = movecoll->begin();
     LArG4H6FrontHitConstIterator m_end = movecoll->end();
     for ( ; m_it!=m_end; ++m_it) {
         LArG4H6FrontHit* hit = (*m_it);
         scnum = hit->GetSC();
         if(scnum  <= 0) continue; 
         if(hit->GetSC() == 5 &&  (hit->GetTrackID() == 1)) {
            log << MSG::DEBUG << "CheckVeto failed "<<endreq; 
            //return StatusCode::FAILURE;
            //return StatusCode::RECOVERABLE;
            setFilterPassed(false);
            return StatusCode::SUCCESS;
          }
     }
  }

  if(m_check_clus) { // Rejected if no good cluster
     const DataHandle<CaloClusterContainer> cc ;
     sc = m_eventStore->retrieve(cc,m_clusterCollName);
     if(sc != StatusCode::SUCCESS) {
            log << MSG::ERROR << "Could not retrieve ClusterContainer " 
                << m_clusterCollName << " from StoreGate" << endreq;
            return sc;
     }
     bool haveit=false;
     CaloClusterContainer::const_iterator clusIter = cc->begin();
     CaloClusterContainer::const_iterator clusIterEnd = cc->end();
     for( ;clusIter!=clusIterEnd;clusIter++) {
           const CaloCluster * theCluster = (*clusIter);
           double cL = theCluster->getMomentValue(CaloClusterMoment::CENTER_LAMBDA);
           double eD = theCluster->getMomentValue(CaloClusterMoment::FIRST_ENG_DENS);
           unsigned int cSize = theCluster->getClusterSize();
           if(cL != 0 && eD > 0. && cSize > 1) {
              haveit = true;
              break;
           }
     }
     if (!haveit) {
        log << MSG::DEBUG << "CheckClusters failed "<<endreq; 
        setFilterPassed(false);
         return StatusCode::SUCCESS;
     }
  }


  if(m_check_trackreco) { // Rejected if bad track reco
      const DataHandle< TBTrack > mytrack;
      const DataHandle< TBEventInfo > mytbeinfo;
      sc = m_eventStore->retrieve(mytrack,"Track" ) ;
      if (sc.isFailure()){
          log << MSG::WARNING << "Retrieval of TBTrack failed" << endreq; 
          return sc;
      }
      sc = m_eventStore->retrieve(mytbeinfo,"TBEventInfo" ) ;
      if (sc.isFailure()){
          log << MSG::WARNING << "Retrieval of TBEventInfo failed" << endreq; 
          return sc;
      }
      double chi2 = mytrack->getChi2_u();
      if(chi2 <= 0 ||  std::log(chi2)<=-10 || chi2  == 1000 || mytrack->getUslope() == 0 || mytrack->getUintercept() == 0 || mytbeinfo->getCryoX() == mytrack->getCryoHitu()) {
        log << MSG::DEBUG << "CheckTrackReco in X failed "<<endreq; 
        log << MSG::DEBUG << chi2 << " / " << std::log(chi2) <<  " :: " << mytrack->getUslope() <<  " / " << mytrack->getUintercept()  <<  " / " << mytbeinfo->getCryoX()  <<  " / " << mytrack->getCryoHitu() << endreq;
         setFilterPassed(false);
         return StatusCode::SUCCESS;
      }       
      chi2 = mytrack->getChi2_v();
      if(chi2 <= 0 ||  std::log(chi2)<=-10 || chi2  == 1000 || mytrack->getVslope() == 0 || mytrack->getVintercept() == 0 || mytbeinfo->getTableY() == mytrack->getCryoHitv()) {
        log << MSG::DEBUG << "CheckTrackReco in Y failed "<<endreq; 
        log << MSG::DEBUG << chi2 << " / " << std::log(chi2) <<  " :: " << mytrack->getVslope() <<  " / " << mytrack->getVintercept()  <<  " / " << mytbeinfo->getTableY()  <<  " / " << mytrack->getCryoHitv() << endreq;
         setFilterPassed(false);
         return StatusCode::SUCCESS;
      }       

  }
  setFilterPassed(true);
  return StatusCode::SUCCESS;
}

StatusCode TBBeamQualityMC::finalize() {
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "in finalize()" << endreq;
  return StatusCode::SUCCESS;
}


StatusCode TBBeamQualityMC::getXcryoYtable(float &x, float &y, float &e) {

  // not good to put here (just a workaround) - joh
  std::string m_txtFileWithXY = "xcryo_ytable.txt";

  MsgStream log(messageService(),name());
  log << MSG::DEBUG << "in getXcryoYtable(float x, float y)" << endreq;
  std::ifstream xyFile;
  std::string line;
  std::string filename = PathResolver::find_file(m_txtFileWithXY, "DATAPATH");
  xyFile.open(filename.c_str());
  if (!xyFile.is_open()) {
    log << MSG::ERROR << "File " << m_txtFileWithXY << " fail to open in $DATAPATH" 
	<< endreq;
    return StatusCode::FAILURE;
  }
  while ( getline(xyFile, line, '\n') ) {
    int run;
    std::istringstream buf(line);
    e = 0;
    buf >> run >> x >> y >> e;
    log << MSG::DEBUG << "m_nRun,run,x,y,e= "<<m_nRun<<" "<<run<<" "<<x<<" "<<y<<" "<<e<<endreq;
    if (run==m_nRun && xyFile.good()) return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}
