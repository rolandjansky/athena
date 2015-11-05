/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>

#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "AthenaMonitoring/AthenaMonManager.h"
#include "HIMonitoring/HIMonitoringMuonsTool.h"

#include "LWHists/TH1D_LW.h"
#include "LWHists/TH2D_LW.h"
#include "LWHists/TProfile_LW.h"

HIMonitoringMuonsTool::
HIMonitoringMuonsTool( const std::string & type, const std::string & name,
const IInterface* parent ): ManagedMonitorToolBase( type, name, parent )   	     	  
{
	m_FCalEt=0;
}


HIMonitoringMuonsTool::~HIMonitoringMuonsTool()
{
}

// Description: Used for rebooking unmanaged histograms       
StatusCode HIMonitoringMuonsTool::bookHistogramsRecurrent( )
{
	return StatusCode::SUCCESS;
}


// Description: Used for re-booking managed histograms       
StatusCode HIMonitoringMuonsTool::bookHistograms( )
{
	if( m_environment == AthenaMonManager::online ) {
		// book histograms that are only made in the online environment...
	}

	if( m_dataType == AthenaMonManager::cosmics ) {
		// book histograms that are only relevant for cosmics data...
	}
   
	book_hist(); 

 
	return StatusCode::SUCCESS;
}


StatusCode HIMonitoringMuonsTool::fillHistograms()
{
	StatusCode sc; 
	
	/// HI event shape
	const xAOD::HIEventShapeContainer* evtShape;
	sc = evtStore()->retrieve(evtShape, "HIEventShape");
	if (sc.isFailure()) 
	{
		ATH_MSG_ERROR("Could not find HIEventShape");
		return StatusCode::FAILURE;
	} 
	else 
	{
		ATH_MSG_INFO("HIEventShape retrieved from StoreGate");
	}
	
	int size=evtShape->size();
	for(int i=0;i<size;i++)
	{       
		const xAOD::HIEventShape *sh=evtShape->at(i);
		if(sh->layer()==21 || sh->layer()==22 || sh->layer()==23)
		{
			m_FCalEt += sh->et();
		} 
	}
   
	m_FCalEt = m_FCalEt/1000000.;
	  	

        const xAOD::MuonContainer* muons = 0;
	sc = evtStore()->retrieve( muons, "Muons" );
	if (sc.isFailure()) {
	  ATH_MSG_ERROR("Could not find Muons");
	  return StatusCode::FAILURE;
	}
	else {
	  ATH_MSG_INFO("Muons retrieved from StoreGate");
	}

        xAOD::MuonContainer::const_iterator muon_itr = muons->begin();
        xAOD::MuonContainer::const_iterator muon_end = muons->end();

        for( ; muon_itr != muon_end; ++muon_itr ) {

	// pt cut
	if((*muon_itr)->pt()*1.e-3 <6.)continue;

	auth = (*muon_itr)->author();
        type = (*muon_itr)->muonType();



        const xAOD::TrackParticle* primTrackParticle = (*muon_itr)->primaryTrackParticle();
        const xAOD::TrackParticle* MStrackParticle   = (*muon_itr)->trackParticle( xAOD::Muon::MuonSpectrometerTrackParticle );
        const xAOD::TrackParticle* MEtrackParticle   = (*muon_itr)->trackParticle( xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle );
        const xAOD::TrackParticle* IDtrackParticle   = (*muon_itr)->trackParticle( xAOD::Muon::InnerDetectorTrackParticle );


	if(primTrackParticle){
	chi2ndof = primTrackParticle->chiSquared()/primTrackParticle->numberDoF();
	}
	if(IDtrackParticle){
	chi2ndofID = IDtrackParticle->chiSquared()/IDtrackParticle->numberDoF();
	}
	if(MStrackParticle){
	chi2ndofMS = MStrackParticle->chiSquared()/MStrackParticle->numberDoF();
	}
	if(MEtrackParticle){
	chi2ndofME = MEtrackParticle->chiSquared()/MEtrackParticle->numberDoF();
	}

	if(m_FCalEt < 0.2){
	  h_Auth_fcal0->Fill(auth);
	  h_Type_fcal0->Fill(type);
	  h_Chi2ndf_fcal0->Fill(chi2ndof);
	  h_Chi2ndfID_fcal0->Fill(chi2ndofID);
	  h_Chi2ndfMS_fcal0->Fill(chi2ndofMS);
	  h_Chi2ndfME_fcal0->Fill(chi2ndofME);
        } 
        if(m_FCalEt >= 0.2 && m_FCalEt < 1.5){
	  h_Auth_fcal1->Fill(auth);
	  h_Type_fcal1->Fill(type);
	  h_Chi2ndf_fcal1->Fill(chi2ndof);
	  h_Chi2ndfID_fcal1->Fill(chi2ndofID);
	  h_Chi2ndfMS_fcal1->Fill(chi2ndofMS);
	  h_Chi2ndfME_fcal1->Fill(chi2ndofME);
        }
        if(m_FCalEt >= 1.5){
	  h_Auth_fcal2->Fill(auth);
	  h_Type_fcal2->Fill(type);
	  h_Chi2ndf_fcal2->Fill(chi2ndof);
	  h_Chi2ndfID_fcal2->Fill(chi2ndofID);
	  h_Chi2ndfMS_fcal2->Fill(chi2ndofMS);
	  h_Chi2ndfME_fcal2->Fill(chi2ndofME);
        }


        }//for


 	  
	return StatusCode::SUCCESS;
}


StatusCode HIMonitoringMuonsTool::procHistograms( )
{
   
	if( endOfLowStat || endOfLumiBlock ) 
	{
	   
	}


	if( endOfRun ) 
	{
   
	}

	return StatusCode::SUCCESS;
}


/// *** FCal sum Et *** /// 
void HIMonitoringMuonsTool::book_hist()
{
	std::string path = "HeavyIon/Muons"; 

        h_Auth_fcal0 = TH1D_LW::create( "h_Auth_fcal0", ";author;counts" , 12 , 0 , 12);
	regHist(h_Auth_fcal0, path, run).ignore();
        h_Auth_fcal1 = TH1D_LW::create( "h_Auth_fcal1", ";author;counts" , 12 , 0 , 12);
	regHist(h_Auth_fcal1, path, run).ignore();
        h_Auth_fcal2 = TH1D_LW::create( "h_Auth_fcal2", ";author;counts" , 12 , 0 , 12);
	regHist(h_Auth_fcal2, path, run).ignore();

        h_Type_fcal0 = TH1D_LW::create( "h_Type_fcal0", ";type;counts" , 5 , 0 , 5);
	regHist(h_Type_fcal0, path, run).ignore();
        h_Type_fcal1 = TH1D_LW::create( "h_Type_fcal1", ";type;counts" , 5 , 0 , 5);
	regHist(h_Type_fcal1, path, run).ignore();
        h_Type_fcal2 = TH1D_LW::create( "h_Type_fcal2", ";type;counts" , 5 , 0 , 5);
	regHist(h_Type_fcal2, path, run).ignore();

        h_Chi2ndf_fcal0 = TH1D_LW::create( "h_Chi2ndf_fcal0", ";chi2/ndf;counts" , 100 , 0 , 10);
	regHist(h_Chi2ndf_fcal0, path, run).ignore();
        h_Chi2ndf_fcal1 = TH1D_LW::create( "h_Chi2ndf_fcal1", ";chi2/ndf;counts" , 100 , 0 , 10);
	regHist(h_Chi2ndf_fcal1, path, run).ignore();
        h_Chi2ndf_fcal2 = TH1D_LW::create( "h_Chi2ndf_fcal2", ";chi2/ndf;counts" , 100 , 0 , 10);
	regHist(h_Chi2ndf_fcal2, path, run).ignore();

        h_Chi2ndfID_fcal0 = TH1D_LW::create( "h_Chi2ndfID_fcal0", ";ID chi2/ndf;counts" , 100 , 0 , 10);
	regHist(h_Chi2ndfID_fcal0, path, run).ignore();
        h_Chi2ndfID_fcal1 = TH1D_LW::create( "h_Chi2ndfID_fcal1", ";ID chi2/ndf;counts" , 100 , 0 , 10);
	regHist(h_Chi2ndfID_fcal1, path, run).ignore();
        h_Chi2ndfID_fcal2 = TH1D_LW::create( "h_Chi2ndfID_fcal2", ";ID chi2/ndf;counts" , 100 , 0 , 10);
	regHist(h_Chi2ndfID_fcal2, path, run).ignore();

        h_Chi2ndfMS_fcal0 = TH1D_LW::create( "h_Chi2ndfMS_fcal0", ";MS chi2/ndf;counts" , 100 , 0 , 10);
	regHist(h_Chi2ndfMS_fcal0, path, run).ignore();
        h_Chi2ndfMS_fcal1 = TH1D_LW::create( "h_Chi2ndfMS_fcal1", ";MS chi2/ndf;counts" , 100 , 0 , 10);
	regHist(h_Chi2ndfMS_fcal1, path, run).ignore();
        h_Chi2ndfMS_fcal2 = TH1D_LW::create( "h_Chi2ndfMS_fcal2", ";MS chi2/ndf;counts" , 100 , 0 , 10);
	regHist(h_Chi2ndfMS_fcal2, path, run).ignore();

        h_Chi2ndfME_fcal0 = TH1D_LW::create( "h_Chi2ndfME_fcal0", ";ME chi2/ndf;counts" , 100 , 0 , 10);
	regHist(h_Chi2ndfME_fcal0, path, run).ignore();
        h_Chi2ndfME_fcal1 = TH1D_LW::create( "h_Chi2ndfME_fcal1", ";ME chi2/ndf;counts" , 100 , 0 , 10);
	regHist(h_Chi2ndfME_fcal1, path, run).ignore();
        h_Chi2ndfME_fcal2 = TH1D_LW::create( "h_Chi2ndfME_fcal2", ";ME chi2/ndf;counts" , 100 , 0 , 10);
	regHist(h_Chi2ndfME_fcal2, path, run).ignore();



}
