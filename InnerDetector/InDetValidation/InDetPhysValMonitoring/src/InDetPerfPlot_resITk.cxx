/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPerfPlot_resITk.cxx
 * @author me!
**/

#include "InDetPerfPlot_resITk.h"
#include <iostream>
#include <map>

static const int nParams = 5;
std::string m_paraNames[nParams] = { "d0", "z0", "phi", "theta" , "qOverP" };

std::map<TString, float> trkParams;
std::map<TString, float> trueTrkParams;
std::map<TString, float> trkErrParams;


InDetPerfPlot_resITk::InDetPerfPlot_resITk(PlotBase* pParent, const std::string & sDir)  :PlotBase(pParent, sDir)
   {

		m_primTrk = false;
		m_secdTrk = false;
		m_allTrk = false;
   TString tsDir = (TString) sDir;
	 if(tsDir.Contains("Primary")) m_primTrk=true; //control if sec/prim from init
	 else if(tsDir.Contains("Secondary")) m_secdTrk=true;
	 else { m_allTrk=true; }
//
}


void InDetPerfPlot_resITk::initializePlots() {

	int d0bin = 1000; float d0min = -2.0; float d0max = 2.0;
	int z0bin = 2500; float z0min = -5.0; float z0max = 5.0;

  //larger d0 range if secondary tracks
	if(m_secdTrk || m_allTrk) { d0min = -10.; d0max = 10.; }

  //IP resolutions
	

	//Pull plots
	m_resITk_pull_d0 			= Book1D("pull_d0","pull_d0; d_{0}(reco-true)/sigma_{d_{0}}",200,-5.,5.,false);
	m_resITk_pull_z0 			= Book1D("pull_z0","pull_z0; z_{0}(reco-true)/sigma_{z_{0}}",200,-5.,5.,false);
	m_resITk_pull_qoverp		= Book1D("pull_qoverp","pull_qoverp; q/p(reco-true)/sigma_{q/p}",200,-5.,5.,false);
	m_resITk_pull_qoverpt	= Book1D("pull_qoverpt","pull_qoverpt; q/p_{T}(reco-true)/sigma_{q/p_{T}}",200,-5.,5.,false);
	m_resITk_pull_theta		= Book1D("pull_theta","pull_theta; #theta(reco-true)/sigma_{#theta}",200,-5.,5.,false);
	m_resITk_pull_phi			= Book1D("pull_phi","pull_phi; #phi(reco-true)/sigma_{#phi}",200,-5.,5.,false);
	m_resITk_pull_pt				= Book1D("pull_pt","pull_pt; p_{T}(reco-true)/sigma_{p_{T}}",200,-5.,5.,false);


	
}

void InDetPerfPlot_resITk::fill(const xAOD::TrackParticle& trkprt, const xAOD::TruthParticle& truthprt) {
  
	//Check whether the track is primary or secondary
	int trueBC=-9999;
  trueBC = truthprt.barcode();
	int isPrimTrk = 0; int isSecdTrk = 0;
	if((trueBC > 0) && (trueBC < 200000)) isPrimTrk = 1;
	if((trueBC > 200000)) isSecdTrk = 1;
  
	//Move on to the next track incase the wrong track category
	if(!isPrimTrk && !isSecdTrk) return;
	if(!isPrimTrk && m_primTrk) return;
	if(!isSecdTrk && m_secdTrk) return;

	//Get track paramters for truth and reco tracks for easy access
	//store all in track parameter maps trkP/truetrkP
 	getTrackParameters(trkprt);
	getTrackParameters(truthprt);

	//Pull plots to extract uncertainties
	m_resITk_pull_d0->Fill((trkP["d0"]-truetrkP["d0"])/trkErrP["d0"]);
	m_resITk_pull_z0->Fill((trkP["z0"]-truetrkP["z0"])/trkErrP["z0"]);
	m_resITk_pull_qoverp->Fill((trkP["qoP"]-truetrkP["qoP"])/trkErrP["qoP"]);
	m_resITk_pull_qoverpt->Fill((trkP["qoPt"]-truetrkP["qoPt"])/trkErrP["qoPt"]);
	m_resITk_pull_theta->Fill((trkP["theta"]-truetrkP["theta"])/trkErrP["theta"]);
	m_resITk_pull_phi->Fill((trkP["phi"]-truetrkP["phi"])/trkErrP["phi"]);
	m_resITk_pull_pt->Fill((trkP["pt"]-truetrkP["pt"])/trkErrP["pt"]);

	std::cout << "track Pt	" << trkP["pt"] << "	-	" << truetrkP["pt"] << "	/	" << trkErrP["pt"] << std::endl;
}

void InDetPerfPlot_resITk::getTrackParameters(const xAOD::TrackParticle& trkprt){
 
 	//Track perigee, BS?
	trkP["d0"] 				= trkprt.d0();
  trkP["z0"] 				= trkprt.z0();
  trkP["phi"] 			= trkprt.phi0();
  trkP["theta"] 		= trkprt.theta();
  trkP["qoP"] 			= trkprt.qOverP();

  //Track fit errors 	
	trkErrP["d0"] 		= TMath::Sqrt(trkprt.definingParametersCovMatrix()(0,0));
  trkErrP["z0"] 		= TMath::Sqrt(trkprt.definingParametersCovMatrix()(1,1));
	trkErrP["phi"] 		= TMath::Sqrt(trkprt.definingParametersCovMatrix()(2,2));
	trkErrP["theta"] 	= TMath::Sqrt(trkprt.definingParametersCovMatrix()(3,3));
	trkErrP["qoP"] 		= TMath::Sqrt(trkprt.definingParametersCovMatrix()(4,4));
	
	//Extra
	trkP["pt"] 				= trkprt.pt();
	trkP["p"] 				= trkprt.pt()/TMath::Sin(trkprt.theta());
	trkP["eta"] 			= trkprt.eta();
	trkP["qoPt"] 			= trkprt.qOverP()*TMath::Sin(trkprt.theta());
	trkP["q"]					= trkprt.qOverP()/fabs(trkprt.qOverP());
	trkErrP["qoPt"]		= trkErrP["qoP"]*TMath::Sin(trkprt.theta());
  trkErrP["pt"]			= fabs(trkP["pt"]*(1/trkErrP["qoPt"]));

}

void InDetPerfPlot_resITk::getTrackParameters(const xAOD::TruthParticle& truthprt){
		
	//"d0", "z0", "phi", "theta" , "qOverP"
	//Perigee for truth particles are in aux container
  for(int iParams = 0; iParams < nParams; iParams++) {
	    truetrkP[m_paraNames[iParams]] = -9999.;
			if(truthprt.isAvailable<float>(m_paraNames[iParams])) truetrkP[m_paraNames[iParams]] = (truthprt.auxdata<float>(m_paraNames[iParams]));
	}

	truetrkP["pt"] 		= truthprt.pt();
	truetrkP["eta"] 	= truthprt.eta();
	truetrkP["qoP"] 	= truetrkP["qOverP"]; //annoying and unesscary ;)
	truetrkP["q"] 		= truetrkP["qOverP"]/fabs(truetrkP["qOverP"]);
	truetrkP["qoPt"]	= truetrkP["qOverP"]*TMath::Sin(truetrkP["theta"]);
	
}


void InDetPerfPlot_resITk::finalizePlots(){

	

}



