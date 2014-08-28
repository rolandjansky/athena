/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "StoreGate/StoreGateSvc.h"

#include "egammaEvent/PhotonContainer.h"
#include "PhotonAnalysisUtils/DiphotonAnalysis.h"
#include "PhotonAnalysisUtils/PAUDiPhoton.h"

#include "egammaInterfaces/IegammaBaseTool.h"
#include "egammaEvent/egamma.h"
#include "egammaAnalysisUtils/FudgeMCTool.h"
#include "egammaAnalysisUtils/PhotonIDTool.h"
#include "CaloEvent/CaloCluster.h"
#include "xAODEgamma/Egamma.h"

#include "D3PDMakerUtils/TypeNameConversions.h"

#include "AthenaKernel/errorcheck.h"

#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>

#include <iomanip>
#include <fstream>
#include <string>


using CLHEP::Hep3Vector;
using CLHEP::HepLorentzVector;


//####################################################################################
DiphotonAnalysis::DiphotonAnalysis(const std::string& name, ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator), 
    m_PrimaryVertex              ( "PAUprimaryVertexFinder"     ) ,
    m_PAUhggFitter               ( "PAUhggFitter"               ) ,
    m_PhotonCorrTool             ( "PAUphotonCorrectionTool"    ) ,
    m_DiPhotonContainer          ( 0 ),
    m_DiPhotonVertexContainer    ( 0 ),
    m_resolver                   ( 0 ),
    m_egammaOQFlagsBuilder       ( "egammaOQFlagsBuilder" )
{ 
  declareProperty("HggFitterTool"          , m_PAUhggFitter               , "H->gamma gamma fitter." );
  declareProperty("PhotonContainer"        , m_photonContainerName        = "PhotonAODCollection"    );
  declareProperty("ClusterContainer"       , m_clusterContainerName       = "CaloCalTopoCluster"     );
  declareProperty("PhotonCorrectionTool"   , m_PhotonCorrTool             , "Photon Correction Tool" );

  declareProperty("DiPhotonPtCut"          , m_dipho_PtCut = 25000);
  declareProperty("VxPrimaryCandidateName" , m_VxPrimaryCandidateName="VxPrimaryCandidate");
  declareProperty("egammaOQFlagsBuilder"   , m_egammaOQFlagsBuilder, "Tool for calculating the OQ flag");
  declareProperty("isData"                 , m_isData=true);
}


StatusCode DiphotonAnalysis::initialize() {

  MsgStream log( messageService(), name() );

  CHECK ( m_PhotonCorrTool.retrieve() );
  CHECK ( m_PAUhggFitter.retrieve() );

  ServiceHandle<IClassIDSvc> clidsvc ("ClassIDSvc", this->name());
  m_resolver = new D3PD::SGKeyResolver(this->name(), evtStore(), m_photonContainerName);
  CLID clid;
  CHECK( D3PD::nameToCLID ("PhotonContainer", clid, this->name(), clidsvc) );
  CHECK( m_resolver->initialize (clid, "PhotonContainer") );

  CHECK( m_egammaOQFlagsBuilder.retrieve() );

  return StatusCode::SUCCESS;
}		 

StatusCode DiphotonAnalysis::finalize() {
  delete m_resolver;

  return StatusCode::SUCCESS;
}


StatusCode DiphotonAnalysis::execute() {
  MsgStream log( messageService(), name() );
  log << MSG::VERBOSE << "in execute()" << endreq; 

  if ( PAUTwoGamma() == StatusCode::FAILURE )  return StatusCode::FAILURE ;

  return StatusCode::SUCCESS;
}


StatusCode DiphotonAnalysis::PAUTwoGamma() {
  MsgStream log( messageService(), name() );
  log << MSG::VERBOSE << "in PAUTwoGamma()" << endreq; 
  
  //-----------------------------------------------
  // Create the new collection in SG
  //
  m_DiPhotonContainer = new PAU::DiPhotonContainer;
  CHECK( evtStore()->record( m_DiPhotonContainer , "PAUDiphotonCollection" ));
  //-----------------------------------------------

  m_DiPhotonVertexContainer = new ConstDataVector<VxContainer>(SG::VIEW_ELEMENTS); 
  CHECK( evtStore()->record( m_DiPhotonVertexContainer , "PAUDiphotonVertexCollection" ));


  //-----------------------------------------------
  // Get the photons
  //
  const PhotonContainer* photons = 0;
  CHECK( evtStore()->retrieve( photons, m_resolver->key() ) );

  PhotonContainer::const_iterator photIt  = photons->begin();
  PhotonContainer::const_iterator photItE = photons->end();
  if(photons->size() < 2) {
    if (m_DiPhotonVertexContainer->size()==0) {
      log << MSG::VERBOSE << "DiPhotonVertexContainer with no element..." << endreq;
      const VxContainer* vxTES = 0;
      CHECK( evtStore()->retrieve( vxTES , m_VxPrimaryCandidateName ));
      VxContainer::const_iterator vtxItr  = vxTES->begin();
      VxContainer::const_iterator vtxItrE = vxTES->end();
      for (; vtxItr != vtxItrE; vtxItr++) {   // loop over the event's vertices
	const Trk::VxCandidate* vxcand = (*vtxItr);
	m_DiPhotonVertexContainer->push_back( vxcand ) ;
      }      
    }
    return StatusCode::SUCCESS;
  }

  std::vector<const PAU::egamma*> allGammas;
  for ( ; photIt != photItE ; ++photIt ) {
    const Analysis::Photon* phot = *photIt ;

    int convFlag = 0 ;
    if ( phot->conversion() ) {
      convFlag = phot->conversion()->vxTrackAtVertex()->size() ;
      if ( convFlag > 2 )  log << MSG::WARNING << "conversion detected, with >2 tracks!" << endreq ;
    }
    if ( phot->author()&egammaParameters::AuthorRConv ) convFlag += 10;


    PAU::egamma* newphot = new PAU::egamma(phot, convFlag);
    
    allGammas.push_back(newphot);
  }
  std::sort( allGammas.begin() , allGammas.end() , PAUutils::High2LowByPt<PAU::egamma>() ) ;
  //-----------------------------------------------


  //-----------------------------------------------
  // Make a local copy of the cluster list.
  std::vector<const CaloCluster*> clusts;
  const CaloClusterContainer* cluscoll = 0;
  CHECK( evtStore()->retrieve( cluscoll, m_clusterContainerName ) );
  CaloClusterContainer::const_iterator itr  = cluscoll->begin();
  CaloClusterContainer::const_iterator itrE = cluscoll->end();
  for ( ; itr!=itrE ; ++itr ){
    const CaloCluster * cl = (*itr);
    clusts.push_back (cl);
  }

  //-----------------------------------------------
  // find leading and subleading loose photons
  int nloose = 0;
  std::vector<float> loosePhotonPt;
  std::vector<int>   loosePhotonIndex;
  if (loosePhotonPt.size()>0)            loosePhotonPt.clear();
  if (loosePhotonIndex.size()>0)         loosePhotonIndex.clear();

  for(unsigned int i=0; i<allGammas.size(); i++){
    
    const Trk::VxCandidate* conv = allGammas[i]->photon()->conversion();
    int convFlag = -1;
    if (conv && conv->vxTrackAtVertex()->size()<=2) {
      convFlag = conv->vxTrackAtVertex()->size();
    } else
      convFlag = 0;
    
    double eCluster = allGammas[i]->cluster()->e();
    double Ecorr   = eCluster;
    double etas2   = allGammas[i]->cluster()->etaBE(2);
    double eT_corr = Ecorr*1./cosh(etas2);
    double energy_correction_photon = Ecorr / eCluster;
    
    const EMShower* shower = allGammas[i]->photon()->detail<EMShower>();
    if(!shower) continue;
    
    double rHad1   = shower->parameter(egammaParameters::ethad1 ) * 1./eT_corr;    
    double rHad    = shower->parameter(egammaParameters::ethad  ) * 1./eT_corr;    
    double e277    = shower->parameter(egammaParameters::e277   ) * energy_correction_photon;    
    double e237    = shower->parameter(egammaParameters::e237   ) * energy_correction_photon;    
    double e233    = shower->parameter(egammaParameters::e233   ) * energy_correction_photon;    
    double weta2   = shower->parameter(egammaParameters::weta2  );   
    double f1      = shower->parameter(egammaParameters::f1     );      
    double Emax2   = shower->parameter(egammaParameters::e2tsts1); 
    double emaxs1  = shower->parameter(egammaParameters::emaxs1 );  
    double Emins1  = shower->parameter(egammaParameters::emins1 );  
    double fSide   = shower->parameter(egammaParameters::fracs1 );  
    double wstot   = shower->parameter(egammaParameters::wtots1 );  
    double ws3     = shower->parameter(egammaParameters::weta1  );   
    
    double rEta    = (e277 == 0 ? 0 : e237/e277);
    double rPhi    = (e237 == 0 ? 0 : e233/e237);
    double eRatio  = fabs(emaxs1 + Emax2)>0. ? (emaxs1 - Emax2) / (emaxs1 + Emax2) : 0;
    double deltaE  = ( Emax2 - Emins1 ) * energy_correction_photon;
    
    int isConv     = (convFlag > 0);
    
    // removed stuff for OQ, doesn't work with xAOD yet
    /*
    unsigned int oq = 0;
    if (allGammas[i]->cluster() && allGammas[i]->cluster()->isCellLinkValid()) {
      egamma neweg (*(allGammas[i]->photon()));
      CHECK( m_egammaOQFlagsBuilder->execute (&neweg) );
      oq = static_cast<unsigned int> (neweg.egammaID (egammaPID::IsGoodOQ));
    }
    if ( !( (oq & 34214)==0) ) continue; //bad photon
    if ( (oq & 134217728)!=0 && ( rEta>0.98 || rPhi>1.0 || (oq & 67108864)!=0 ) ) continue;
    */

    if (!m_isData) {
      FudgeMCTool fudger( eT_corr, etas2, isConv, 0 );
      int mySelection = 14;
      fudger.SetPreselection( mySelection );	
      fudger.FudgeShowers(eT_corr, etas2, rHad1, rHad, e277, rEta, rPhi, weta2, f1, fSide, wstot, ws3, deltaE, eRatio, isConv);
    }
    
    PhotonIDTool idTool = PhotonIDTool( eCluster/cosh(etas2), etas2, rHad1, rHad, e277, rEta, rPhi, 
					weta2, f1, fSide, wstot, ws3, deltaE, eRatio, isConv);
    bool PassLoose = idTool.PhotonCutsLoose(4);
    
    if (!PassLoose) continue;
    if (Ecorr/cosh(etas2)<m_dipho_PtCut) continue;

    loosePhotonPt.push_back(eT_corr);
    loosePhotonIndex.push_back(i);
    ++nloose;
  }

  int iLead = -1;
  int iSublead = -1;
  bool duplicatedBestVtx = false;
  std::vector<const PAU::egamma*> allPairedGammas;

  if (nloose>=2) { // 2 loose photons found
    
    float looseptlead = -999999.;
    for (uint k=0 ; k<loosePhotonPt.size() ; k++) {
      if (loosePhotonPt.at(k)>looseptlead) {
	looseptlead = loosePhotonPt.at(k);
	iLead = k;//int(loosePhotonIndex.at(k));
      }
    }
    float dptmin = 99999999999999.;
    for (uint k=0 ; k<loosePhotonPt.size() ; k++) {
      if (int(k)==iLead) continue;
      if (fabs(loosePhotonPt.at(k)-loosePhotonPt.at(iLead))<dptmin) {
	dptmin = fabs(loosePhotonPt.at(k)-loosePhotonPt.at(iLead));
	iSublead = k;//int(loosePhotonIndex.at(k));
      }
    }
    
    //-----------------------------------------------
    // Fill all photons pairs
    //
    for(unsigned int i=0; i<loosePhotonPt.size()-1; i++){
      if (loosePhotonIndex.at(i)<0) continue;
      
      for(unsigned int j=i+1; j<loosePhotonIndex.size(); j++){
	if (loosePhotonIndex.at(j)<0) continue;
	      
	PAU::DiPhoton* dp = new PAU::DiPhoton(const_cast<PAU::egamma*>(allGammas[loosePhotonIndex.at(i)]),
					      const_cast<PAU::egamma*>(allGammas[loosePhotonIndex.at(j)]));
	const std::vector<PAU::egamma*>* ph = dp->getPhotons() ;
	dp->setIndex(i,j);
	
	m_PhotonCorrTool->set_photons(ph->at(0), ph->at(1));
	PAUphotonCorrectionTool::correction_id    corr_id[3] ;
	PAUphotonCorrectionTool::corrected_set_t  corr_set[3] ;
	corr_id[0] = PAUphotonCorrectionTool::correction_id( PAUphotonCorrectionTool::angle_none ,       // use conversion or track in fit
							     0 ,                                         // no energy correction
							     false ,                                     // don't use primary vertex
							     false                                       // don't correct eta for conv photons
							     );
	corr_id[1] = PAUphotonCorrectionTool::correction_id( PAUphotonCorrectionTool::PAUhggFitter_Calo ,// use calo pointing only
							     0 ,                                         // no energy correction
							     true ,                                      // use primary vertex
							     true                                        // correct eta for conv photons
							     );
	corr_id[2] = PAUphotonCorrectionTool::correction_id( PAUphotonCorrectionTool::PAUhggFitter_Conv ,// use conversion vertex in fit
							     0 ,                                         // no energy correction
							     true ,                                      // use primary vertex
							     true                                        // correct eta for conv photons
							     );
	//----------------------------------------------
	for ( unsigned u=0 ; u<3 ; ++u ) {
	  corr_set[u] = m_PhotonCorrTool->compute_single_correction( corr_id[u] );
	  if ( u>0 && !corr_set[u].isCorrected ) {
	    log << MSG::DEBUG << "photon correction failed at level " << u << endreq ;
	    break ;
	  }
	  fillHggQuantities( corr_set[u] , u , dp) ;
	  if (corr_set[u].Higgs) delete corr_set[u].Higgs;
	}
	
	log << MSG::VERBOSE << "Higgs mass = " << dp->invmass << "\t Accuracy level: " << dp->invmass_flag << endreq ;
	log << MSG::VERBOSE << "Higgs Pt = " << dp->ptgg << endreq ;
	log << MSG::VERBOSE << "Eta1 = " << dp->etaLead            << "\t Eta2 = " << dp->etaSublead << endreq ;
	
	m_DiPhotonContainer->push_back(dp);
	allPairedGammas.push_back(allGammas[loosePhotonIndex.at(i)]);
	allPairedGammas.push_back(allGammas[loosePhotonIndex.at(j)]);
	
	m_DiPhotonVertexContainer->push_back( dp->hgg_HPV_vertex  ) ;
	
	// put the loose photon pair with highest pT at the first position
	if ( (iLead>=0 && iSublead>=0 && ((int(i)==iLead && int(j)==iSublead)||(int(i)==iSublead && int(j)==iLead)) ) ) {
	  duplicatedBestVtx = true;
	  m_DiPhotonVertexContainer->insert(m_DiPhotonVertexContainer->begin(), const_cast<Trk::VxCandidate*>(dp->hgg_HPV_vertex) );
	}
      }
    }

  } 


  if (!duplicatedBestVtx) {
    log << MSG::VERBOSE << "!!!!!   --------   Best vertex not found during the loop   --------   !!!!!" << endreq;
    if (m_DiPhotonVertexContainer->size()>0)
      m_DiPhotonVertexContainer->insert(m_DiPhotonVertexContainer->begin(), m_DiPhotonVertexContainer->at(0) );
    else 
      log << MSG::VERBOSE << "!!!!!   --------   DiPhotonVertex container has no element   --------   !!!!!" << endreq;
  }

  // if the diphoton vertex container is still empty at that point, fill it with original PV container
  if (m_DiPhotonVertexContainer->size()==0) {

    log << MSG::VERBOSE << "DiPhotonVertexContainer with no element..." << endreq;
    const VxContainer* vxTES = 0;
    CHECK( evtStore()->retrieve( vxTES , m_VxPrimaryCandidateName ));
    VxContainer::const_iterator vtxItr  = vxTES->begin();
    VxContainer::const_iterator vtxItrE = vxTES->end();
    for (; vtxItr != vtxItrE; vtxItr++) {   // loop over the event's vertices
      const Trk::VxCandidate* vxcand = (*vtxItr);
      m_DiPhotonVertexContainer->push_back( vxcand ) ;
    }

  }

  // get rid of the photons that weren't used
  for(unsigned int i=0; i<allGammas.size(); i++){
    if(std::find(allPairedGammas.begin(),allPairedGammas.end(),allGammas[i]) == allPairedGammas.end())
      delete allGammas[i];
  }


  allGammas.clear();
  //-----------------------------------------------



  // Done!
  return StatusCode::SUCCESS ;
}


float DiphotonAnalysis::PAUInvMass(const std::vector<float>& E, const std::vector<float>& eta, const std::vector<float>& phi) const {
  if ( E.size() < 2 ) return -99999 ;
  if ( eta.size() < 2 ) return -99999 ;
  if ( phi.size() < 2 ) return -99999 ;
  if ( fabs(eta[0]) > 2.47 || fabs(eta[1]) > 2.47 || fabs(eta[0]-eta[1]) > 5 ) return -99999;

  float Et0 = (E[0]/cosh(eta[0]));
  float Et1 = (E[1]/cosh(eta[1]));
  float angle = ( cosh(eta[0]-eta[1]) - cos(phi[0]-phi[1]) );

  float invMass2 = 2*Et0*Et1*angle;

  if(invMass2 > 0)
    return sqrt( invMass2 ) ;
  else
    return -99999;
}



void DiphotonAnalysis::fillHggQuantities( const PAUphotonCorrectionTool::corrected_set_t & corr , 
					  int accuracy,
					  PAU::DiPhoton* dp) {
  MsgStream log(msgSvc(), name());

  dp->invmass     .push_back( corr.Higgs->m() );
  if (dp->invmass.size()>1) { dp->invmass.erase(dp->invmass.begin()); dp->invmass.insert(dp->invmass.begin(), dp->invmass.at(dp->invmass.size()-1) ); }
  else { dp->invmass.insert(dp->invmass.begin(), dp->invmass.at(0)); }

  dp->ptgg        .push_back( corr.Higgs->pt() );
  if (dp->ptgg.size()>1) { dp->ptgg.erase(dp->ptgg.begin()); dp->ptgg.insert(dp->ptgg.begin(), dp->ptgg.at(dp->ptgg.size()-1) ); }
  else { dp->ptgg.insert(dp->ptgg.begin(), dp->ptgg.at(0)); }

  dp->detagg      .push_back( corr.Ph1->eta() - corr.Ph2->eta() );
  if (dp->detagg.size()>1) { dp->detagg.erase(dp->detagg.begin()); dp->detagg.insert(dp->detagg.begin(), dp->detagg.at(dp->detagg.size()-1) ); }
  else { dp->detagg.insert(dp->detagg.begin(), dp->detagg.at(0)); }

  // Changed by MH obo JFM
  //dp->invmass_flag      .push_back( corr.Ph1->eta() - corr.Ph2->eta() );
  dp->invmass_flag.push_back( accuracy ); 
  if (dp->invmass_flag.size()>1) { dp->invmass_flag.erase(dp->invmass_flag.begin()); dp->invmass_flag.insert(dp->invmass_flag.begin(), dp->invmass_flag.at(dp->invmass_flag.size()-1) ); }
  else { dp->invmass_flag.insert(dp->invmass_flag.begin(), dp->invmass_flag.at(0)); }
  
  // cos(theta*) :
  Hep3Vector       boostVector = -(corr.Higgs->hlv().boostVector());
  HepLorentzVector initialObj  = corr.Ph1->hlv() ;
  HepLorentzVector boostedObj  = initialObj.boost(boostVector) ;
  dp->cosTh .push_back( cos( boostedObj.angle( - boostVector ) ) );
  if (dp->cosTh.size()>1) { dp->cosTh.erase(dp->cosTh.begin()); dp->cosTh.insert(dp->cosTh.begin(), dp->cosTh.at(dp->cosTh.size()-1) ); }
  else { dp->cosTh.insert(dp->cosTh.begin(), dp->cosTh.at(0)); }
  
  dp->etaLead      .push_back( corr.Ph1->eta() );
  if (dp->etaLead.size()>1) { dp->etaLead.erase(dp->etaLead.begin()); dp->etaLead.insert(dp->etaLead.begin(), dp->etaLead.at(dp->etaLead.size()-1) ); }
  else { dp->etaLead.insert(dp->etaLead.begin(), dp->etaLead.at(0)); }

  dp->etaSublead   .push_back( corr.Ph2->eta() );
  if (dp->etaSublead.size()>1) { dp->etaSublead.erase(dp->etaSublead.begin()); dp->etaSublead.insert(dp->etaSublead.begin(), dp->etaSublead.at(dp->etaSublead.size()-1) ); }
  else { dp->etaSublead.insert(dp->etaSublead.begin(), dp->etaSublead.at(0)); }

  dp->z            .push_back( corr.vtx_beforePVF_z );
  if (dp->z.size()>1) { dp->z.erase(dp->z.begin()); dp->z.insert(dp->z.begin(), dp->z.at(dp->z.size()-1) ); }
  else { dp->z.insert(dp->z.begin(), dp->z.at(0)); }

  dp->zres         .push_back( corr.vtx_beforePVF_zres );
  if (dp->zres.size()>1) { dp->zres.erase(dp->zres.begin()); dp->zres.insert(dp->zres.begin(), dp->zres.at(dp->zres.size()-1) ); }
  else { dp->zres.insert(dp->zres.begin(), dp->zres.at(0)); }

  dp->vtxLikelihood.push_back( corr.vtx_afterPVF_zlike );
  if (dp->vtxLikelihood.size()>1) { dp->vtxLikelihood.erase(dp->vtxLikelihood.begin()); dp->vtxLikelihood.insert(dp->vtxLikelihood.begin(), dp->vtxLikelihood.at(dp->vtxLikelihood.size()-1) ); }
  else { dp->vtxLikelihood.insert(dp->vtxLikelihood.begin(), dp->vtxLikelihood.at(0)); }

  // dump truth info
  std::vector<float> t_eta, t_phi, t_E, t_etaMC, t_phiMC, t_EMC ;
  const std::vector<PAU::egamma*>* ph = dp->getPhotons() ;
  t_eta.push_back( corr.Ph1->eta() );
  t_eta.push_back( corr.Ph2->eta() );
  for (unsigned int i=0 ; i<2 ; i++) {
    t_phi.push_back( ph->at(i)->phi() ) ;
    t_E.push_back( ph->at(i)->e() ) ;
    t_etaMC.push_back( ph->at(i)->etaMC ) ;
    t_phiMC.push_back( ph->at(i)->phiMC ) ;
    t_EMC.push_back( ph->at(i)->eMC ) ;
  }
  dp->invmassTrueDir .push_back( PAUInvMass( t_E , t_etaMC , t_phiMC ) );
  if (dp->invmassTrueDir.size()>1) { dp->invmassTrueDir.erase(dp->invmassTrueDir.begin()); dp->invmassTrueDir.insert(dp->invmassTrueDir.begin(), dp->invmassTrueDir.at(dp->invmassTrueDir.size()-1) ); }
  else { dp->invmassTrueDir.insert(dp->invmassTrueDir.begin(), dp->invmassTrueDir.at(0)); }
  
  dp->invmassTrueE   .push_back( PAUInvMass( t_EMC , t_eta , t_phi ) );
  if (dp->invmassTrueE.size()>1) { dp->invmassTrueE.erase(dp->invmassTrueE.begin()); dp->invmassTrueE.insert(dp->invmassTrueE.begin(), dp->invmassTrueE.at(dp->invmassTrueE.size()-1) ); }
  else { dp->invmassTrueE.insert(dp->invmassTrueE.begin(), dp->invmassTrueE.at(0)); }

  t_eta.clear(); t_phi.clear(); t_E.clear(); t_etaMC.clear(); t_phiMC.clear(); t_EMC.clear();
  

  if (accuracy==0) { // only fill once
    
    dp->onejet_cutStatus = PAU::UNKNOWN;
    dp->onejet_ok        = PAU::UNKNOWN;
    dp->onejet_mgg       = PAU::UNKNOWN;
    dp->onejet_mggj      = PAU::UNKNOWN;

    dp->twojet_cutStatus = PAU::UNKNOWN;
    dp->twojet_ok        = PAU::UNKNOWN;
    dp->twojet_mgg       = PAU::UNKNOWN;
    dp->twojet_mjj       = PAU::UNKNOWN;
  
    dp->hggZWjj_SumTracksPT    = PAU::UNKNOWN;
    dp->hggZWjj_mjjZW_tracks   = PAU::UNKNOWN;
    dp->hggZWjj_mjjZW          = PAU::UNKNOWN;
    dp->hggZWjj_ZW7Njet        = PAU::UNKNOWN;
    dp->hggZWjj_PTjjZW         = PAU::UNKNOWN;
    dp->hggZWjj_PTjjZW_tracks  = PAU::UNKNOWN;
    dp->hggZWjj_hggZWjj_ok     = PAU::UNKNOWN;
    dp->hggZWjj_hggZWdeltaRjj  = PAU::UNKNOWN;
    dp->hggZWjj_PTjZW.push_back( PAU::UNKNOWN );
    dp->hggZWjj_PTjZW.push_back( PAU::UNKNOWN );
    dp->hggZWjj_PTjZWTracks.push_back( PAU::UNKNOWN );
    dp->hggZWjj_PTjZWTracks.push_back( PAU::UNKNOWN );
    dp->hggZWjj_EtajZW.push_back( PAU::UNKNOWN );
    dp->hggZWjj_EtajZW.push_back( PAU::UNKNOWN );
      
    dp->hggZWjj_SumTracksPT4    = PAU::UNKNOWN;
    dp->hggZWjj_mjjZW4_tracks   = PAU::UNKNOWN;
    dp->hggZWjj_mjjZW4          = PAU::UNKNOWN;
    dp->hggZWjj_ZW4Njet         = PAU::UNKNOWN;
    dp->hggZWjj_PTjjZW4         = PAU::UNKNOWN;
    dp->hggZWjj_PTjjZW4_tracks  = PAU::UNKNOWN;
    dp->hggZWjj_DeltaRphotons   = PAU::UNKNOWN;
    dp->hggZWjj_PTjZW4.push_back( PAU::UNKNOWN );
    dp->hggZWjj_PTjZW4.push_back( PAU::UNKNOWN );
    dp->hggZWjj_PTjZW4Tracks.push_back( PAU::UNKNOWN );
    dp->hggZWjj_PTjZW4Tracks.push_back( PAU::UNKNOWN );
    dp->hggZWjj_EtajZW4.push_back( PAU::UNKNOWN );
    dp->hggZWjj_EtajZW4.push_back( PAU::UNKNOWN );
      
    dp->hggZWjj_hggetmiss_ok    = PAU::UNKNOWN;
    dp->hggZWjj_hgglepton_ok    = PAU::UNKNOWN;
    dp->hggZWjj_DeltaRmujet     = PAU::UNKNOWN;
    dp->hggZWjj_LeptonJets_pt   = std::vector<float>(0);
    dp->hggZWjj_LeptonJetsSumpt = PAU::UNKNOWN;
    dp->hgg_indexGoodMuon       = std::vector<int>(0);
    dp->hgg_indexGoodElectron   = std::vector<int>(0);

  }

}
