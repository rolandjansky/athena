/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//********************************************************************//
// NAME:     TauSubstructureVariables.cxx                             //
// PACKAGE:  offline/Reconstruction/tauRec                            //
// AUTHORS:  M. Trottier-McDonald                                     //
// CREATED:  January 11 2010                                          //
//********************************************************************//

#include <algorithm> 
#include <cmath>
#include <sstream>

#include "xAODJet/Jet.h"
#include "xAODTau/TauJet.h"

#include "tauRecTools/CaloClusterVariables.h"
#include "tauRecTools/TauSubstructureVariables.h"

#include "tauRecTools/KineUtils.h"
#include "tauRecTools/HelperFunctions.h"

#define GeV 1000
const double TauSubstructureVariables::DEFAULT = -1111.;

//**********************************
// Constructor
//**********************************

TauSubstructureVariables::TauSubstructureVariables( const std::string& name )
    : TauRecToolBase(name) {
	declareProperty("maxPileUpCorrection", m_maxPileUpCorrection = 4 * GeV);
	declareProperty("pileUpAlpha", m_pileUpAlpha = 1.0);
	declareProperty("VertexCorrection", m_doVertexCorrection = false);
	declareProperty("IncShowerSubtr", m_incShowerSubtr = true);
}


//**********************************
// Destructor
//**********************************

TauSubstructureVariables::~TauSubstructureVariables() {
}


//***********************************
// Initialize method
//***********************************

StatusCode TauSubstructureVariables::initialize() {
	return StatusCode::SUCCESS;
}

//***********************************
// Finalize method
//***********************************

StatusCode TauSubstructureVariables::finalize() {
	return StatusCode::SUCCESS;
}


//************************************
// Execute method
//************************************

StatusCode TauSubstructureVariables::execute(xAOD::TauJet& pTau) const {

	const xAOD::Jet* taujetseed = (*pTau.jetLink());
    if (!taujetseed) {
        ATH_MSG_ERROR("Tau jet link is invalid.");
        return StatusCode::FAILURE;
    } 

	//*****************************************************
	// calculate some tau substructure variables
	//*****************************************************

	CaloClusterVariables CaloClusterVariablesTool;
	CaloClusterVariablesTool.setVertexCorrection(m_doVertexCorrection);
	CaloClusterVariablesTool.setIncSub(m_incShowerSubtr);

	bool isFilled = CaloClusterVariablesTool.update(pTau);

	if (!isFilled) {
		ATH_MSG_DEBUG("problem in calculating calo cluster variables -> will be set to -1111");

		pTau.setDetail(xAOD::TauJetParameters::numTopoClusters , static_cast<int>(DEFAULT) );
		pTau.setDetail(xAOD::TauJetParameters::numEffTopoClusters , static_cast<float>(DEFAULT) );
		pTau.setDetail(xAOD::TauJetParameters::topoInvMass,  static_cast<float>(DEFAULT) );
		pTau.setDetail(xAOD::TauJetParameters::effTopoInvMass,  static_cast<float>(DEFAULT) );
		pTau.setDetail(xAOD::TauJetParameters::topoMeanDeltaR,  static_cast<float>(DEFAULT) );
		pTau.setDetail(xAOD::TauJetParameters::effTopoMeanDeltaR,  static_cast<float>(DEFAULT) );

	} else {
		// Getting the variables
		//-----------------------
		double TopoInvMass = CaloClusterVariablesTool.totalMass();
		double EffTopoInvMass = CaloClusterVariablesTool.effectiveMass();
		unsigned int NumTopoClusters = CaloClusterVariablesTool.numConstituents();
		double NumEffTopoClusters = CaloClusterVariablesTool.effectiveNumConstituents();
		double TopoMeanDeltaR = CaloClusterVariablesTool.averageRadius();
		double EffTopoMeanDeltaR = CaloClusterVariablesTool.averageEffectiveRadius();

		ATH_MSG_VERBOSE(" Substructure variables: ");
		ATH_MSG_VERBOSE("-------------------------");
		ATH_MSG_VERBOSE("       TopoInvMass: " << TopoInvMass);
		ATH_MSG_VERBOSE("    EffTopoInvMass: " << EffTopoInvMass);
		ATH_MSG_VERBOSE("   NumTopoClusters: " << NumTopoClusters);
		ATH_MSG_VERBOSE("NumEffTopoClusters: " << NumEffTopoClusters);
		ATH_MSG_VERBOSE("    TopoMeanDeltaR: " << TopoMeanDeltaR);
		ATH_MSG_VERBOSE(" EffTopoMeanDeltaR: " << EffTopoMeanDeltaR);

		//Record the variables
		//---------------------

		pTau.setDetail(xAOD::TauJetParameters::numTopoClusters            ,  static_cast<int>  (NumTopoClusters)    );
		pTau.setDetail(xAOD::TauJetParameters::numEffTopoClusters         ,  static_cast<float>(NumEffTopoClusters) );
		pTau.setDetail(xAOD::TauJetParameters::topoInvMass                ,  static_cast<float>(TopoInvMass)	     );
		pTau.setDetail(xAOD::TauJetParameters::effTopoInvMass             ,  static_cast<float>(EffTopoInvMass)     );
		pTau.setDetail(xAOD::TauJetParameters::topoMeanDeltaR             ,  static_cast<float>(TopoMeanDeltaR)     );
		pTau.setDetail(xAOD::TauJetParameters::effTopoMeanDeltaR          ,  static_cast<float>(EffTopoMeanDeltaR)  );

	}

	//*****************************************************
	// calculate some new cluster based ID variables
	//*****************************************************
	// New cluster-based variables
	float totalEnergy(0.);
	float calo_iso(0.);
	float dr(0.);

	unsigned int num_clusters(0);

	TLorentzVector leadClusVec;
	TLorentzVector subLeadClusVec;
	TLorentzVector approxSubstructure4Vec;
	double clusELead = -1111.0;
	double clusESubLead = -1111.0;

	// Loop through jets, get links to clusters
	std::vector<const xAOD::CaloCluster*> vClusters;
	ATH_CHECK(tauRecTools::GetJetClusterList(taujetseed, vClusters, m_incShowerSubtr));

	for (auto incluster : vClusters){
	        ++num_clusters;

		// calc total energy
		totalEnergy += incluster->e();
		
		//apply Vertex correction on a temporary
		TLorentzVector tempclusvec;
		if (m_doVertexCorrection && pTau.vertexLink())
			tempclusvec = CaloVertexedClusterType(*incluster, (*pTau.vertexLink())->position()).p4();
		else
			tempclusvec = CaloVertexedClusterType(*incluster).p4();

		dr = Tau1P3PKineUtils::deltaR(pTau.eta(),pTau.phi(), tempclusvec.Eta(), tempclusvec.Phi());
		if (0.2 <= dr && dr < 0.4) 
		  {
		    calo_iso += tempclusvec.Et();
		  }
		else if (dr < 0.2)
		  {
		    double clusEnergyBE = ( incluster->energyBE(0) + incluster->energyBE(1) + incluster->energyBE(2) );
		    
		    if (clusEnergyBE > clusELead) 
		      {
			//change current leading cluster to subleading
			clusESubLead = clusELead;
			subLeadClusVec = leadClusVec;

			//set energy and 4-vector of leading cluster
			clusELead = clusEnergyBE;
			leadClusVec.SetPtEtaPhiM(clusELead/cosh(tempclusvec.Eta()), tempclusvec.Eta(), tempclusvec.Phi(), 0);
		      }
		    else if (clusEnergyBE > clusESubLead) 
		      {
			//set energy and 4-vector of subleading cluster only
			clusESubLead = clusEnergyBE;
			subLeadClusVec.SetPtEtaPhiM(clusESubLead/cosh(tempclusvec.Eta()), tempclusvec.Eta(), tempclusvec.Phi(), 0);
		      }

		  }
		
	}

	if (clusELead > 0.) {
	  approxSubstructure4Vec += leadClusVec;
	}
	if (clusESubLead > 0.) {
	  approxSubstructure4Vec += subLeadClusVec;
	  }

	// now sort cluster by energy
	std::sort(vClusters.begin(), vClusters.end(), DefCaloClusterCompare());

	// determine energy sum of leading 2 and leading 3 clusters
	float sum2LeadClusterE(0.);
	float sum3LeadClusterE(0.);
	std::vector<const xAOD::CaloCluster*>::const_iterator icluster(vClusters.begin());
	std::vector<const xAOD::CaloCluster*>::const_iterator icluster_end(vClusters.end());

	for (; icluster != icluster_end && icluster != vClusters.begin() + 3; ++icluster) {
		if (icluster < vClusters.begin() + 2) {
			sum2LeadClusterE += (*icluster)->e();
		}
		sum3LeadClusterE += (*icluster)->e();
	}

	//record variables
	if (totalEnergy != 0) {
		ATH_MSG_VERBOSE(" lead2ClusterEOverAllClusterE: " << sum2LeadClusterE / totalEnergy);
		ATH_MSG_VERBOSE(" lead3ClusterEOverAllClusterE: " << sum3LeadClusterE / totalEnergy);

		pTau.setDetail(xAOD::TauJetParameters::lead2ClusterEOverAllClusterE, static_cast<float>(sum2LeadClusterE / totalEnergy)  );
		pTau.setDetail(xAOD::TauJetParameters::lead3ClusterEOverAllClusterE, static_cast<float>(sum3LeadClusterE / totalEnergy)  );
	}

	ATH_MSG_VERBOSE(" caloIso: " << calo_iso);
	pTau.setDetail(xAOD::TauJetParameters::caloIso, static_cast<float>(calo_iso)  );

	// calculate calorimeter energies in different layers
	float PSSEnergy(0.);
	float EMEnergy(0.);
	float HADEnergy(0.);
	icluster = vClusters.begin();
	for (; icluster != icluster_end; ++icluster) {
		float clEnergy = (*icluster)->e();

		//Calculate the fractions of energy in different calorimeter layers
		const xAOD::CaloCluster *cl = *icluster;
		float PreSampler = cl->eSample(CaloSampling::PreSamplerB) + cl->eSample(CaloSampling::PreSamplerE);
		float EMLayer1   = cl->eSample(CaloSampling::EMB1) + cl->eSample(CaloSampling::EME1);
		float EMLayer2   = cl->eSample(CaloSampling::EMB2) + cl->eSample(CaloSampling::EME2);

		float Energy = cl->rawE();
		float PSSF = Energy != 0 ? (PreSampler + EMLayer1) / Energy : 0;
		float EM2F = Energy != 0 ? EMLayer2 / Energy : 0;
		float EMF = PSSF + EM2F;

		PSSEnergy += PSSF * clEnergy;
		EMEnergy  += EMF * clEnergy;
		HADEnergy += (Energy != 0) ? (1 - EMF) * clEnergy : 0;
	}

	// calculate trk momentum
	float trkSysMomentum(0.);
	for (unsigned int i(0); i < pTau.nTracks(); ++i) {
		trkSysMomentum += pTau.track(i)->pt()	* cosh(pTau.track(i)->eta());

		//adding the core tracks to the approximate substructure 4 vector
		approxSubstructure4Vec += pTau.track(i)->p4();
	}

	// set new approximate energy flow variables for tau ID
	pTau.setDetail(xAOD::TauJetParameters::ptRatioEflowApprox, static_cast<float>(approxSubstructure4Vec.Pt()/ pTau.ptDetectorAxis()) );
	pTau.setDetail(xAOD::TauJetParameters::mEflowApprox, static_cast<float>(approxSubstructure4Vec.M()) );


	float fPSSFraction 			= (totalEnergy != 0) ? PSSEnergy / totalEnergy : DEFAULT;
	float fChPIEMEOverCaloEME 	= (EMEnergy != 0) ? (trkSysMomentum - HADEnergy) / EMEnergy : DEFAULT;
	float fEMPOverTrkSysP 		= DEFAULT;
	if (pTau.nTracks() > 0) fEMPOverTrkSysP = (trkSysMomentum != 0) ? EMEnergy / trkSysMomentum : DEFAULT;

	pTau.setDetail(xAOD::TauJetParameters::PSSFraction,		static_cast<float>(fPSSFraction));
	pTau.setDetail(xAOD::TauJetParameters::ChPiEMEOverCaloEME,	static_cast<float>(fChPIEMEOverCaloEME));
	pTau.setDetail(xAOD::TauJetParameters::EMPOverTrkSysP,		static_cast<float>(fEMPOverTrkSysP));

	// jvf and sumPtTrk are now a vector and the old run1-type jvf value is stored in the 0-th element
	// sumPtTrk is calculated wrt Vertices

	float jvf(0.0);
	float sumPtTrk(0.0);

	// for tau trigger: JVF and sumPtTrack are not available
	if (!inTrigger())
	{
		std::vector<float> sumPtTrkvec;
		std::vector<float> jvfvec;

		// ToDo still need to check if the 500MeV threshold is correct
		taujetseed->getAttribute(xAOD::JetAttribute::SumPtTrkPt500, sumPtTrkvec);
		taujetseed->getAttribute(xAOD::JetAttribute::JVF, jvfvec);

		if (!jvfvec.empty() && !sumPtTrkvec.empty()) {
			// ToDo need to check if first vertex is the vertex we want to use here!
			jvf = jvfvec[0];
			sumPtTrk = sumPtTrkvec[0];
		}
		else {
			ATH_MSG_WARNING("jvf value vector and/or sumPtTrk vector returned from seed jet is empty!");
		}
	}

	float pt_pileup = (1.0 - jvf) * sumPtTrk;

	const float max_pileup_correction = m_maxPileUpCorrection;
	const float alpha = m_pileUpAlpha;
	float pileup_correction = alpha * pt_pileup;

	ATH_MSG_VERBOSE("   --------------------------------------");
	ATH_MSG_VERBOSE("   Pile-up correction parameter");
	ATH_MSG_VERBOSE("   -> sumPtTrk:        " << sumPtTrk);
	ATH_MSG_VERBOSE("   -> jvf:             " << jvf);
	ATH_MSG_VERBOSE("   -> pt_pileup:       " << pt_pileup);
	ATH_MSG_VERBOSE("   -> alpha:           " << alpha);
	ATH_MSG_VERBOSE("   -> max pileup corr: " << max_pileup_correction);

	if (pileup_correction > max_pileup_correction) {
		pileup_correction = max_pileup_correction;
	}
	const float calo_iso_corrected = calo_iso - pileup_correction;

	ATH_MSG_VERBOSE("   -> pileup corr:     " << pileup_correction);
	ATH_MSG_VERBOSE("   --------------------------------------");

	//record variable
	ATH_MSG_VERBOSE(" caloIsoCorrected: " << calo_iso_corrected);
	pTau.setDetail(xAOD::TauJetParameters::caloIsoCorrected, static_cast<float>(calo_iso_corrected)  );

	// calculate dRMax
	unsigned int numTrack(pTau.nTracks());
	if (numTrack > 0) {
		float dRmin = -1 * -1111;
		float dRmax = -1111;
		float dR;

		for (unsigned int i(0); i < numTrack; ++i) {
			if (pTau.track(i) == 0) continue;
			dR = Tau1P3PKineUtils::deltaR( pTau.track(i)->eta(), pTau.track(i)->phi(), pTau.eta(), pTau.phi() );
			if (dRmin > dR) dRmin = dR;
			if (dRmax < dR) dRmax = dR;
		}
		//record variable
		ATH_MSG_VERBOSE(" dRmax: " << dRmax);
		pTau.setDetail(xAOD::TauJetParameters::dRmax, static_cast<float>(dRmax)  );
	}

	return StatusCode::SUCCESS;
}
