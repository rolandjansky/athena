/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MissingETPerformance/MissingETMuonData.h"

#include "JetEvent/Jet.h"
#include "JetEvent/JetCollection.h"

#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenParticle.h"

#include "TruthHelper/IsGenStable.h"
#include "TruthHelper/IsGenNonInteracting.h"
#include "TruthHelper/GenAccessIO.h"

using namespace TruthHelper; 
using CLHEP::HepLorentzVector;


MissingETMuonData::MissingETMuonData( const std::string& type,
    const std::string& name,
    const IInterface* parent ) : AthAlgTool( type, name, parent ) {
  declareInterface<MissingETMuonData>( this );

  declareProperty("McEventCollectionKey",         m_mcEventColKey           = "GEN_AOD");
  declareProperty("MuonContainerKey",             m_MuonContainerKey        = "StacoMuonCollection");
  declareProperty("CaloMuonContainerKey",         m_CaloMuonContainerKey    = "CaloMuonCollection");
  declareProperty("JetKey",                       m_JetKey                  = "Cone4H1TopoJets");

  m_MET_MuonBoy_v1510 = new MissingET;
  m_MET_MuonBoy_standalone_fix = new MissingET;
}

MissingETMuonData::~MissingETMuonData() {}

StatusCode MissingETMuonData::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode MissingETMuonData::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode MissingETMuonData::retrieveMuons() {

  StatusCode sc = StatusCode::SUCCESS;

  m_truth_Muons.clear();
  m_crack_Muons.clear();
  m_MuonBoy_Spectro_Muons.clear();
  m_MuonBoy_Track_Muons.clear();
  m_all_used_muons.clear();

  m_MuonBoy_Spectro_Muons_standalone_fix.clear();
  m_MuonBoy_Track_Muons_standalone_fix.clear();
  m_all_used_muons_standalone_fix.clear();

  m_MET_MuonBoy_v1510->setEx(0.);
  m_MET_MuonBoy_v1510->setEy(0.);
  m_MET_MuonBoy_v1510->setEtSum(0.);

  m_MET_MuonBoy_standalone_fix->setEx(0.);
  m_MET_MuonBoy_standalone_fix->setEy(0.);
  m_MET_MuonBoy_standalone_fix->setEtSum(0.);

  const Analysis::MuonContainer *muons = 0;
  if (evtStore()->contains<Analysis::MuonContainer>(m_MuonContainerKey)) {
    sc = evtStore()->retrieve( muons, m_MuonContainerKey );
    if ( sc.isFailure() ) { return sc; }
  }
  else {
    msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_MuonContainerKey << endmsg;
    return StatusCode::FAILURE;
  }

  const Analysis::MuonContainer *calo_muons = 0;
  if (evtStore()->contains<Analysis::MuonContainer>(m_CaloMuonContainerKey)) {
    sc = evtStore()->retrieve( calo_muons, m_CaloMuonContainerKey );
    if ( sc.isFailure() ) { return sc; }
  }
  else {
    msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_CaloMuonContainerKey << endmsg;
    return StatusCode::FAILURE;
  }

  const JetCollection *jets = 0;
  if (evtStore()->contains<JetCollection>(m_JetKey)) {
    sc=evtStore()->retrieve( jets, m_JetKey );
    if( sc.isFailure() ) { return sc; }
  }
  else {
    msg() << MSG::WARNING << "No JetCollection found in StoreGate, key:" << m_JetKey << endmsg; 
    return StatusCode::FAILURE;
  }

  if (m_mcEventColKey != "") {
    bool foundMCCOLL = false;
    const McEventCollection * mcEventCol=0;
    if (evtStore()->contains<McEventCollection>(m_mcEventColKey)) {
      sc = evtStore()->retrieve(mcEventCol, m_mcEventColKey);
      if (sc.isFailure() ) { return sc; }
      foundMCCOLL = true;
    }
    else {
      //msg() << MSG::WARNING << "No McEventCollection found in StoreGate, key:" << m_mcEventColKey << endmsg; 
      foundMCCOLL = false;
      //return StatusCode::FAILURE;
    }

    if (foundMCCOLL) {
      IsGenStable istab;
      for (HepMC::GenEvent::particle_const_iterator it = (mcEventCol->at(0))->particles_begin(); it != (mcEventCol->at(0))->particles_end(); ++it) {
	if ( istab(*it) && abs((*it)->pdg_id()) == 13) {
	  m_truth_Muons.push_back(*it);
	}//found truth muon
      }//loop over mc parts
    }//if foundMCCOLL
  }//if we should try to get truth muons

  for (Analysis::MuonContainer::const_iterator it = muons->begin(); it!= muons->end(); ++it) {
    if ((*it)->isCombinedMuon() || (*it)->isStandAloneMuon() ) {	
      if (isIsolated(*it,jets)) {
	m_MuonBoy_Track_Muons.push_back(*it);
      } else {
	m_MuonBoy_Spectro_Muons.push_back(*it);
      }
    }
  }//loop over muons

  double v1510_x = 0.;
  double v1510_y = 0.;
  double v1510_sum = 0.;

  double standalone_fix_x = 0.;
  double standalone_fix_y = 0.;
  double standalone_fix_sum = 0.;

  for (std::vector<const Analysis::Muon*>::const_iterator it = m_MuonBoy_Track_Muons.begin(); it != m_MuonBoy_Track_Muons.end(); ++it ) {
    if ((*it)->isCombinedMuon() || ( (*it)->isStandAloneMuon()  &&  fabs((*it)->eta())>2.5) ) {
      standalone_fix_x -= (*it)->px();
      standalone_fix_y -= (*it)->py();
      standalone_fix_sum += (*it)->pt();

      m_MuonBoy_Track_Muons_standalone_fix.push_back((*it));
      m_all_used_muons_standalone_fix.push_back(new HepLorentzVector((*it)->hlv()));
    }
  }

  findCrackMuons(&m_crack_Muons,muons,calo_muons);
  for (std::vector<const Analysis::Muon*>::iterator it = m_crack_Muons.begin(); it!= m_crack_Muons.end(); ++it) {
    m_MuonBoy_Track_Muons.push_back(*it);

    standalone_fix_x -= (*it)->px();
    standalone_fix_y -= (*it)->py();
    standalone_fix_sum += (*it)->pt();

    m_MuonBoy_Track_Muons_standalone_fix.push_back((*it));
    m_all_used_muons_standalone_fix.push_back(new HepLorentzVector((*it)->hlv()));
  }

  for (std::vector<const Analysis::Muon*>::const_iterator it = m_MuonBoy_Track_Muons.begin(); it != m_MuonBoy_Track_Muons.end(); ++it ) {
    v1510_x -= (*it)->px();
    v1510_y -= (*it)->py();
    v1510_sum += (*it)->pt();
    m_all_used_muons.push_back(new HepLorentzVector((*it)->hlv()));
  }

  for (std::vector<const Analysis::Muon*>::const_iterator it = m_MuonBoy_Spectro_Muons.begin(); it != m_MuonBoy_Spectro_Muons.end(); ++it ) {

    if (!(*it)->muonSpectrometerTrackParticle()) continue;

    m_all_used_muons.push_back(new HepLorentzVector((*it)->muonSpectrometerTrackParticle()->px(),(*it)->muonSpectrometerTrackParticle()->py(),
						   (*it)->muonSpectrometerTrackParticle()->pz(),(*it)->muonSpectrometerTrackParticle()->e()));
    v1510_x -= (*it)->muonSpectrometerTrackParticle()->px();
    v1510_y -= (*it)->muonSpectrometerTrackParticle()->py();
    v1510_sum += (*it)->muonSpectrometerTrackParticle()->pt();

    if ((*it)->isCombinedMuon() || ( (*it)->isStandAloneMuon()  &&  fabs((*it)->eta())>2.5) ) {
      standalone_fix_x -= (*it)->muonSpectrometerTrackParticle()->px();
      standalone_fix_y -= (*it)->muonSpectrometerTrackParticle()->py();
      standalone_fix_sum += (*it)->muonSpectrometerTrackParticle()->pt();

      m_MuonBoy_Spectro_Muons_standalone_fix.push_back((*it));
      m_all_used_muons_standalone_fix.push_back(new HepLorentzVector((*it)->muonSpectrometerTrackParticle()->px(),(*it)->muonSpectrometerTrackParticle()->py(),
								    (*it)->muonSpectrometerTrackParticle()->pz(),(*it)->muonSpectrometerTrackParticle()->e()));
    }
  }

  m_MET_MuonBoy_v1510->setEx(v1510_x);
  m_MET_MuonBoy_v1510->setEy(v1510_y);
  m_MET_MuonBoy_v1510->setEtSum(v1510_sum);

  m_MET_MuonBoy_standalone_fix->setEx(standalone_fix_x);
  m_MET_MuonBoy_standalone_fix->setEy(standalone_fix_y);
  m_MET_MuonBoy_standalone_fix->setEtSum(standalone_fix_sum);

  return sc;
}//end retrieveMuons()

void MissingETMuonData::findCrackMuons(std::vector<const Analysis::Muon*> *crack_Muons,const Analysis::MuonContainer *muonContainer,const Analysis::MuonContainer *calomuonContainer) {

  // Loop on the CaloMuonIdContainer							  
  Analysis::MuonContainer::const_iterator cm_it = calomuonContainer->begin();		  
  Analysis::MuonContainer::const_iterator cm_it_end = calomuonContainer->end(); 	  
  for ( ; cm_it != cm_it_end ; ++cm_it) {	
    if ( (*cm_it)->caloMuonAlsoFoundByMuonReco() == 0 ) { 
      double eta_calomu    = (*cm_it)->eta();						  
      double phi_calomu    = (*cm_it)->phi();						  

      if ( fabs(eta_calomu) < 0.1) {  // calomu in crack region in abs(eta)<0.1 	  
          bool hasmatch = false;							  
          //loop over muons and look for match  					  
          for (Analysis::MuonContainer::const_iterator  tr = (muonContainer)->begin();    
               tr != (muonContainer)->end(); ++tr) {					  
        										  
            // select combined tracks with best match or standalone track (in |eta|>2.5)  
            if ((*tr)->isCombinedMuon()) {
              double etamu = (*tr)->eta();						    	      
              double phimu = (*tr)->phi();						    	      
              if (fabs(etamu) < 0.12 ) {						  
        	double deta = fabs(etamu - eta_calomu); 				  
        	double dphi = fabs(phimu - phi_calomu); 				  
        	if(dphi > M_PI) dphi = fabs(dphi - 2*M_PI);
        	double dR = sqrt(pow(deta,2) + pow(dphi,2));				  
        	if (dR < 0.02) {							  
        	  hasmatch=true;							  
        	}									  
              } //muon cuts								  
            } // combined muon or standalone muon					  
          } //loop over muons								  
          if (!hasmatch) {
	    crack_Muons->push_back(*cm_it);
          }										  
      }//eta calo muon < 0.1		
    }
  }//loop over calo muons								  

  // Add MuTag muons in region 1<abs(eta)<1.3					   
  // Loop on the StacoCombinedMuonContainerLowPt				   
  Analysis::MuonContainer::const_iterator mt_it = muonContainer->begin();	   
  Analysis::MuonContainer::const_iterator mt_it_end = muonContainer->end();	   
  for ( ; mt_it != mt_it_end ; ++mt_it) {			
    //
    //     if ( (*mt_it)->isLowPtReconstructedMuon() ) {       
    // deprecated, replace isLowPtReconstructedMuon with isSegmentTaggedMuon() && inDetTrackParticle()!=0
    if ( (*mt_it)->isSegmentTaggedMuon() && (*mt_it)->inDetTrackParticle()!=0) {
      double eta_mutag = (*mt_it)->eta();					     
      if ( fabs(eta_mutag) > 1.0 && fabs(eta_mutag) < 1.3) {			     
	crack_Muons->push_back(*mt_it);
      }
    }
  }//loop over mutag
}//findCrackMuons()

bool MissingETMuonData::isIsolated(const Analysis::Muon *mu, const JetCollection *jets) {
  bool isIsol = true;

  // select combined tracks with best match or standalone track (in |eta|>2.5)     
  if (mu->isCombinedMuon() || mu->isStandAloneMuon()) { 
    double eta_mu = mu->eta(); 
    double phi_mu = mu->phi();  
    // loop on jets to check if the muon is Isolated or not  
    if (jets->size() > 0) { 
      for (JetCollection::const_iterator jtr = jets->begin(); jtr != jets->end(); ++jtr) {
	double eta_jet =(*jtr)->eta() ; 
	double phi_jet =(*jtr)->phi() ; 
	//calculate dR 
	double deta = fabs(eta_mu - eta_jet);      
	double dphi = fabs(phi_mu - phi_jet);      
	if(dphi > M_PI) dphi = fabs(dphi - 2*M_PI);
	double dR = sqrt(pow(deta,2) + pow(dphi,2)); 
	if (dR < 0.3) {   //muon isolation criteria  
	  isIsol = false;     
	}
      } //jet loop 
    }  //jet container size >0 
  }//is combined or standalone

  return isIsol;
}

