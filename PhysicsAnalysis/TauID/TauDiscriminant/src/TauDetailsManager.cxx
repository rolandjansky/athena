/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * file: TauDetailsManager.cxx
 *
 * Author: Noel Dawe (end1@sfu.ca)
 */

#include <iostream>

#include "TauDiscriminant/TauDetailsManager.h"
#include "TauDiscriminant/TauPi0Clusters.h"
#include <utility>

#include "xAODTau/TauJet.h"
#include "xAODTracking/TrackParticle.h"
// #include "TrkTrackSummary/TrackSummary.h"
#include "xAODTracking/VertexContainer.h"
#include "TrkEventPrimitives/VertexType.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "FourMomUtils/P4Helpers.h"
#include "AnalysisUtils/AnalysisMisc.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloUtils/CaloVertexedCluster.h"

// redefine the macro to print strings instead of enums as in TauDetails.h
#undef ENUM_OR_STRING
#define ENUM_OR_STRING( x ) #x   

using CLHEP::GeV;
using namespace std;
using namespace xAOD;

// Arrays of the string representations of the detail names
namespace Details {
string IntTauDetailString[]     = { INT_TAU_DETAILS };
string FloatTauDetailString[]   = { FLOAT_TAU_DETAILS };
string IntEventDetailString[]   = { INT_EVENT_DETAILS };
string FloatEventDetailString[] = { FLOAT_EVENT_DETAILS };
}

// The default value for all details
const float TauDetailsManager::LOW_NUMBER = -1111.;

TauDetailsManager::TauDetailsManager(StoreGateSvc* storeGateSvc, bool isTrigger)
{
	this->storeGate = storeGateSvc;
	this->doTrigger = isTrigger;
	//hard-coded for a while
	m_clusterCone = 0.2;

	// Initialize the vector containing the tau-based variables
	this->float_data = vector<float>(Details::__FloatTauDetail__END__ + 1,LOW_NUMBER);
	this->int_data = vector<int>(Details::__IntTauDetail__END__ + 1,int(LOW_NUMBER));

	// Initialize the vector containing the event-based variables
	this->float_event_data = vector<float>( Details::__FloatEventDetail__END__ + 1, LOW_NUMBER);
	this->int_event_data = vector<int>(Details::__IntEventDetail__END__ + 1, int(LOW_NUMBER));

	// Maps of the string representations to the addresses of the values in the above vectors
	unsigned int i;
	for (i = 0; i < this->float_data.size() - 1; ++i) {
		this->float_details.insert( pair<string, float*>(Details::FloatTauDetailString[i], &this->float_data[i]));
	}
	for (i = 0; i < this->float_event_data.size() - 1; ++i) {
		this->float_details.insert(pair<string, float*>(Details::FloatEventDetailString[i],&this->float_event_data[i]));
	}
	for (i = 0; i < this->int_data.size() - 1; ++i) {
		this->int_details.insert( pair<string, int*>(Details::IntTauDetailString[i], &this->int_data[i]));
	}
	for (i = 0; i < this->int_event_data.size() - 1; ++i) {
		this->int_details.insert( pair<string, int*>(Details::IntEventDetailString[i], &this->int_event_data[i]));
	}
}

bool TauDetailsManager::updateEvent()
{
	// Reset the buffers at the beginning of each event
	this->float_event_data.assign(this->float_event_data.size(), LOW_NUMBER);
	this->int_event_data.assign(this->int_event_data.size(), int(LOW_NUMBER));

	if (this->storeGate && this->doTrigger) {
		//vertex information not available at EF, thus use default values to get correct bin; also retrievin VertexContainer will crash for TrigTauDiscriminant
		this->int_event_data[Details::NUMGOODVERTICES] = 1;
		this->int_event_data[Details::NUM_PILEUP_AND_PRIMARY_VERTICES] = 1;
		return true;
	} else if (this->storeGate) {
		const xAOD::VertexContainer* priVertices(0);
		StatusCode sc = this->storeGate->retrieve(priVertices, "PrimaryVertices");
		if (sc.isFailure()) {
			return false;
		}
		if (priVertices) {
			this->int_event_data[Details::NUMVERTICES] = priVertices->size();

			//Martin Flechl, Nov 16, 2010: calculate number of good vertices
			int nGoodVtx(0);
			int nPileupPrimaryVtx(0);
			xAOD::VertexContainer::const_iterator vxIter = priVertices->begin();
			xAOD::VertexContainer::const_iterator vxIterEnd = priVertices->end();
			for (; vxIter != vxIterEnd; ++vxIter) {
				if (!(*vxIter)) continue;
				// FF, March 2014
				// attention: the lines below may work in Athena, but not in xAOD standalone mode, which is used for end-user analysis
				/*
				 std::vector<Trk::VxTrackAtVertex*>* vxTrackAtVertex = (*vxIter)->vxTrackAtVertex();
				 if (!vxTrackAtVertex) continue;
				 if ( vxTrackAtVertex->size() >= 4) nGoodVtx++;

				 if ((vxTrackAtVertex->size() >= 4 && (*vxIter)->vertexType() == xAOD::VxType::PriVtx ) || \
                        (vxTrackAtVertex->size() >= 2 && (*vxIter)->vertexType() == xAOD::VxType::PileUp ) ) nPileupPrimaryVtx++;
				 */

				// this works also in xAOD standalone mode, however, did not checked if results are the same
				int nTrackParticles = (*vxIter)->nTrackParticles();
				if (nTrackParticles >= 4) nGoodVtx++;
				if ( (nTrackParticles >= 4 && (*vxIter)->vertexType() == xAOD::VxType::PriVtx) ||
						(nTrackParticles >= 2 && (*vxIter)->vertexType() == xAOD::VxType::PileUp)
				)
					nPileupPrimaryVtx++;

			}
			this->int_event_data[Details::NUMGOODVERTICES] = nGoodVtx;
			this->int_event_data[Details::NUM_PILEUP_AND_PRIMARY_VERTICES] = nPileupPrimaryVtx;
		}
	}
	return true;
}

bool TauDetailsManager::setNpi0(xAOD::TauJet& tauJet, int nPi0)
{
	this->int_data[Details::TAU_PI0_N] = nPi0;
	tauJet.setDetail(TauJetParameters::nPi0Topo, static_cast<int>(nPi0));

	//ATH_MSG_VERBOSE("setting Npi0 = "<<nPi0);
	//std::cout<<"setting Npi0 = "<<nPi0<<std::endl;
	return true;
}

// const version of TauDetailsManager::update
// calculate variables and store them only in internal memory
bool TauDetailsManager::update(const xAOD::TauJet& tauJet)
{
	// Reset the buffers before setting the variables of each tau
	this->float_data.assign(this->float_data.size(), LOW_NUMBER);
	this->int_data.assign(this->int_data.size(), int(LOW_NUMBER));

	// there is no author flag currently
	// so keeping for backward compatibility only
	this->int_data[Details::AUTHOR] = 3;

	unsigned int numTrack(tauJet.nTracks());

	//==========================================================================================
	// get variables from tau EDM
	//==========================================================================================
	this->int_data[Details::CHARGE]       = tauJet.charge();
	this->int_data[Details::NUMTRACK]     = numTrack;
	this->int_data[Details::NUMWIDETRACK] = tauJet.nWideTracks();

	this->float_data[Details::ETA]     = tauJet.eta();
	this->float_data[Details::ABS_ETA] = fabs(tauJet.eta());

	this->float_data[Details::PHI] = tauJet.phi();
	this->float_data[Details::E]   = tauJet.e();
	this->float_data[Details::ET]  = tauJet.pt();
	this->float_data[Details::PT]  = tauJet.pt();
	this->float_data[Details::M]   = tauJet.m();

	float etOverpTLeadTrk;
	tauJet.detail(TauJetParameters::etOverPtLeadTrk, etOverpTLeadTrk);
	this->float_data[Details::ETOVERPTLEADTRK] = etOverpTLeadTrk;
	this->float_data[Details::PTLEADTRKOVERET] = etOverpTLeadTrk > 0 ? 1. / etOverpTLeadTrk : LOW_NUMBER;

	tauJet.detail(TauJetParameters::ipZ0SinThetaSigLeadTrk, this->float_data[Details::IPZ0SINTHETASIGLEADTRK]);
	tauJet.detail(TauJetParameters::ipSigLeadTrk,           this->float_data[Details::IPSIGLEADTRK]);
	tauJet.detail(TauJetParameters::massTrkSys,	            this->float_data[Details::MASSTRKSYS]);
	tauJet.detail(TauJetParameters::trkWidth2,              this->float_data[Details::TRKWIDTH2]);
	tauJet.detail(TauJetParameters::trFlightPathSig,        this->float_data[Details::TRFLIGHTPATHSIG]);

	float etEflow = 0;
	tauJet.detail(TauJetParameters::etEflow, etEflow);
	this->float_data[Details::ETEFLOWOVERET] =	(tauJet.pt() > 0) ? etEflow / tauJet.pt() : LOW_NUMBER;

	// this variable is not filled anymore by tauRec
	//tauJet.detail(TauJetParameters::mEflow, this->float_data[Details::MEFLOW]);

	tauJet.detail(TauJetParameters::stripWidth2, this->float_data[Details::STRIPWIDTH2]);
	tauJet.detail(TauJetParameters::EMRadius,    this->float_data[Details::EMRADIUS]);
	tauJet.detail(TauJetParameters::hadRadius,   this->float_data[Details::HADRADIUS]);
	tauJet.detail(TauJetParameters::isolFrac,    this->float_data[Details::ISOLFRAC]);
	tauJet.detail(TauJetParameters::centFrac,    this->float_data[Details::CENTFRAC]);
	tauJet.detail(TauJetParameters::nStrip,      this->int_data[Details::NSTRIP]);
	tauJet.detail(TauJetParameters::trkAvgDist,  this->float_data[Details::TRKAVGDIST]);

	float etEMScale, etEMScale1, etEMScale2;
	tauJet.detail(TauJetParameters::etEMAtEMScale,  etEMScale1);
	tauJet.detail(TauJetParameters::etHadAtEMScale, etEMScale2);
	etEMScale = etEMScale1 + etEMScale2;
	this->float_data[Details::EMFRACTIONATEMSCALE] = (etEMScale != 0) ? etEMScale1 / etEMScale : LOW_NUMBER;

	float emradius, hadradius;
	tauJet.detail(TauJetParameters::EMRadius,  emradius);
	tauJet.detail(TauJetParameters::hadRadius, hadradius);
	this->float_data[Details::CALRADIUS] =	(etEMScale != 0) ? (etEMScale1 * emradius + etEMScale2 * hadradius)	/ etEMScale : LOW_NUMBER;

	// New cluster-based variables
	tauJet.detail(TauJetParameters::lead2ClusterEOverAllClusterE,   this->float_data[Details::LEAD2CLUSTEREOVERALLCLUSTERE]);
	tauJet.detail(TauJetParameters::lead3ClusterEOverAllClusterE,   this->float_data[Details::LEAD3CLUSTEREOVERALLCLUSTERE]);
	tauJet.detail(TauJetParameters::caloIso,                        this->float_data[Details::CALO_ISO]);
	tauJet.detail(TauJetParameters::caloIsoCorrected,               this->float_data[Details::CALO_ISO_CORRECTED]);

	// Topocluster variables:
	tauJet.detail(TauJetParameters::topoInvMass, 	this->float_data[Details::TOPOINVMASS]);
	tauJet.detail(TauJetParameters::effTopoInvMass, this->float_data[Details::EFFTOPOINVMASS]);

	tauJet.detail(TauJetParameters::PSSFraction,		this->float_data[Details::PSSFRACTION]);
	tauJet.detail(TauJetParameters::ChPiEMEOverCaloEME,	this->float_data[Details::CHPIEMEOVERCALOEME] );
	tauJet.detail(TauJetParameters::EMPOverTrkSysP,		this->float_data[Details::EMPOVERTRKSYSP]);

	// get intermediate axis
	TLorentzVector tauInterAxis = tauJet.p4(TauJetParameters::IntermediateAxis);
	this->float_data[Details::INTERAXIS_ETA] = tauInterAxis.Eta();
	this->float_data[Details::INTERAXIS_PHI] = tauInterAxis.Phi();

	// this variable is special: when TauDiscriminant is called first time, this variable is empty
	// in case TauDiscriminant::TauDetailsManager is called after a full TauDiscriminant run or on a already full processed AOD/xAOD
	//  this variable is filled properly
	this->float_data[Details::BDTJETSCORE] = tauJet.discriminant(TauJetParameters::BDTJetScore);


	//==========================================================================================
	// calculate now variables needed for TauID not calculated by tauRec
	//==========================================================================================
	const xAOD::Jet* pJetSeed = (*tauJet.jetLink());
	if (!pJetSeed) {
		//ATH_MSG_WARNING("tau does not have jet seed");
		return StatusCode::SUCCESS;
	}

	// JVF and PT_PILEUP
	float pt_pileup = 0.;
	if (!this->doTrigger) {
		// jvf and sumPtTrk are now a vector and the old run1-type jvf value is stored in the 0-th element
		// sumPtTrk is calculated wrt Vertices
		std::vector<float> sumPtTrkvec;
		std::vector<float> jvfvec;

		// ToDo: still need to check if the 500MeV threshold is correct
		pJetSeed->getAttribute(xAOD::JetAttribute::SumPtTrkPt500, sumPtTrkvec);
		pJetSeed->getAttribute(xAOD::JetAttribute::JVF, jvfvec);

		float jvf = 0.0;
		float sumPtTrk = 0.0;
		if (!jvfvec.empty() && !sumPtTrkvec.empty()) {
			// ToDo: need to check if first vertex is the vertex we want to use here!
			jvf = jvfvec[0];
			sumPtTrk = sumPtTrkvec[0];
		} else {
			msg(MSG::WARNING) << "jvf value vector and/or sumPtTrk vector returned from seed jet is empty!"	<< endreq;
		}
		pt_pileup = (1.0 - jvf) * sumPtTrk;
		this->float_data[Details::JVF] = jvf;
		this->float_data[Details::PT_PILEUP] = pt_pileup;
	}

	// track-based variables for pi0 counting and ele veto
	if (numTrack > 0) {
		// variables used by the cut-based e-veto
		tauJet.detail(TauJetParameters::hadLeakEt,                this->float_data[Details::HADLEAKET]);
		tauJet.detail(TauJetParameters::sumEMCellEtOverLeadTrkPt, this->float_data[Details::SUMEMCELLETOVERLEADTRKPT]);
		tauJet.detail(TauJetParameters::secMaxStripEt,            this->float_data[Details::SECMAXSTRIPET]);

		// Used by cut-based eveto:
		if (tauJet.track(0)) {
			this->float_data[Details::ABS_ETA_LEAD_TRACK] = fabs( tauJet.track(0)->eta() );
			this->float_data[Details::TAU_ABSDELTAETA]    = fabs( tauJet.track(0)->eta() - tauJet.eta() );
			this->float_data[Details::TAU_ABSDELTAPHI]    = fabs( tauJet.track(0)->phi() - tauJet.phi() );
			this->float_data[Details::TAU_SEEDTRK_SECMAXSTRIPETOVERPT] = (tauJet.track(0)->pt() != 0) ?	this->float_data[Details::SECMAXSTRIPET] / tauJet.track(0)->pt() : LOW_NUMBER;
			// solve for E3
			float tau_sumETCellsLAr = this->float_data[Details::SUMEMCELLETOVERLEADTRKPT] * tauJet.track(0)->pt();
			float tau_sumEMCellET = etEMScale1;
			float tau_E3 = tau_sumETCellsLAr - tau_sumEMCellET;

			// remove E3
			float tau_seedCalo_etHadAtEMScale_noE3 = etEMScale2 - tau_E3;
			float tau_seedCalo_etEMAtEMScale_yesE3 = etEMScale1 + tau_E3;

			//calculate new EMFraction
			this->float_data[Details::EMFRACTIONATEMSCALE_MOVEE3] =	tau_seedCalo_etEMAtEMScale_yesE3 / (tau_seedCalo_etEMAtEMScale_yesE3 + tau_seedCalo_etHadAtEMScale_noE3);
		}

		// for pi0 counting
		float sumpT3Trk(0.);
		float sumpT(0.);
		float dRmin = -1 * LOW_NUMBER;
		float dR;

		for (unsigned int i(0); i < numTrack; ++i) {
			if (i < 3) sumpT3Trk += tauJet.track(i)->pt();
			sumpT += tauJet.track(i)->pt();
			dR = tauJet.p4().DeltaR(tauJet.track(i)->p4());
			if (dRmin > dR)	dRmin = dR;
		}

		this->float_data[Details::SUMPT3TRK] = sumpT3Trk;
		this->float_data[Details::SUMPT]     = sumpT;
		this->float_data[Details::DRMIN]     = dRmin;
		tauJet.detail(TauJetParameters::dRmax,	this->float_data[Details::DRMAX]);

		if (tauJet.pt() != 0) {
			this->float_data[Details::SUMPT_OVER_ET]     = sumpT / tauJet.pt();
			this->float_data[Details::SUMPT3TRK_OVER_ET] = sumpT3Trk / tauJet.pt();
		}

		if (sumpT3Trk != 0) {
			this->float_data[Details::ETHAD_EM_OVER_SUMPT3TRK] = etEMScale2	/ sumpT3Trk;
			this->float_data[Details::ETEM_EM_OVER_SUMPT3TRK]  = etEMScale1 / sumpT3Trk;
		}
		if (sumpT != 0) {
			this->float_data[Details::ETHAD_EM_OVER_SUMPT] = etEMScale2 / sumpT;
			this->float_data[Details::ETEM_EM_OVER_SUMPT]  = etEMScale1 / sumpT;
		}

		uint8_t numberOfTRTHighThresholdHits;
		tauJet.track(0)->summaryValue(numberOfTRTHighThresholdHits,	xAOD::numberOfTRTHighThresholdHits);
		uint8_t numberOfTRTHits;
		tauJet.track(0)->summaryValue(numberOfTRTHits, xAOD::numberOfTRTHits);
		uint8_t numberOfTRTHighThresholdOutliers;
		tauJet.track(0)->summaryValue(numberOfTRTHighThresholdOutliers,	xAOD::numberOfTRTHighThresholdOutliers);
		uint8_t numberOfTRTOutliers;
		tauJet.track(0)->summaryValue(numberOfTRTOutliers, xAOD::numberOfTRTOutliers);
		this->float_data[Details::TRT_NHT_OVER_NLT] =
				(numberOfTRTHits + numberOfTRTOutliers) > 0 ?
						float( numberOfTRTHighThresholdHits	+ numberOfTRTHighThresholdOutliers)	/ float(numberOfTRTHits + numberOfTRTOutliers) : LOW_NUMBER;
	}

	//Pi0 Cluster finding variables
	TauPi0Clusters tpc = TauPi0Clusters(tauJet);

	this->float_data[Details::PI0CL1_PT]  = tpc.get_cl1_Pt();
	this->float_data[Details::PI0CL1_ETA] = tpc.get_cl1_Eta();
	this->float_data[Details::PI0CL1_PHI] = tpc.get_cl1_Phi();

	this->float_data[Details::PI0CL2_PT]  = tpc.get_cl2_Pt();
	this->float_data[Details::PI0CL2_ETA] = tpc.get_cl2_Eta();
	this->float_data[Details::PI0CL2_PHI] = tpc.get_cl2_Phi();

	this->float_data[Details::VISTAU_PI0CL_PT]  = tpc.get_tau_vis_Pt();
	this->float_data[Details::VISTAU_PI0CL_ETA] = tpc.get_tau_vis_Eta();
	this->float_data[Details::VISTAU_PI0CL_PHI] = tpc.get_tau_vis_Phi();
	this->float_data[Details::VISTAU_PI0CL_M]   = tpc.get_tau_vis_M();
	this->float_data[Details::TAU_PI0_VISTAU_M] = tpc.get_tau_vis_M();
	this->float_data[Details::TAU_PTRATIO]      = (tauJet.pt() != 0) ? tpc.get_tau_vis_Pt() / tauJet.pt() : LOW_NUMBER;
	//	this->int_data[Details::TAU_PI0_N] 	    = 0; //this guy is set elsewhere

	// TRACK_ISO
	float track_iso(0.);
	for (unsigned int i_track(0); i_track < tauJet.nWideTracks(); ++i_track) {
		track_iso += tauJet.wideTrack(i_track)->pt();
	}
	this->float_data[Details::TRACK_ISO] = track_iso;

	//Corrected CENTRALITY FRACTION and FTRK
	float centFrac;
	tauJet.detail(TauJetParameters::centFrac, centFrac);
	float corrFtrk = this->float_data[Details::PTLEADTRKOVERET];

	int nVtx = this->int_event_data[Details::NUM_PILEUP_AND_PRIMARY_VERTICES];

	if (nVtx != int(LOW_NUMBER) && !this->doTrigger) {
		if (tauJet.pt() < 80 * GeV)
			centFrac = centFrac + 0.003 * nVtx;

		if (corrFtrk != float(LOW_NUMBER))
			corrFtrk = corrFtrk + 0.003 * nVtx;
	}

	this->float_data[Details::CORRCENTFRAC] = centFrac;
	this->float_data[Details::CORRFTRK]     = corrFtrk;

	return true;
}

// non-const version of update
bool TauDetailsManager::update_with_edm(xAOD::TauJet& tauJet)
{
	// update first the internal storage of the DetailsManager and calculate variables
	if (!this->update(tauJet)) return false;

	// update now the tau itself
	tauJet.setDetail(TauJetParameters::ptRatioEflowTopo,	static_cast<float>(this->float_data[Details::TAU_PTRATIO]));
	tauJet.setDetail(TauJetParameters::mEflowTopo,			static_cast<float>(this->float_data[Details::TAU_PI0_VISTAU_M]));
	tauJet.setDetail(TauJetParameters::etEflowTopo,			static_cast<float>(this->float_data[Details::VISTAU_PI0CL_PT]));
	//nPi0 saved in TauDetailsManager::setPi0(int nPi0) method
	return true;
}

// Var Getters
const float* TauDetailsManager::getFloatDetailAddress(Details::FloatTauDetail detail) const
{
	return &this->float_data[detail];
}

const int* TauDetailsManager::getIntDetailAddress(Details::IntTauDetail detail) const
{
	return &this->int_data[detail];
}

const float* TauDetailsManager::getFloatDetailAddress(Details::FloatEventDetail detail) const
{
	return &this->float_event_data[detail];
}

const int* TauDetailsManager::getIntDetailAddress(Details::IntEventDetail detail) const
{
	return &this->int_event_data[detail];
}

float TauDetailsManager::getFloatDetailValue(Details::FloatTauDetail detail) const
{
	return this->float_data[detail];
}

int TauDetailsManager::getIntDetailValue(Details::IntTauDetail detail) const
{
	return this->int_data[detail];
}

float TauDetailsManager::getFloatDetailValue(Details::FloatEventDetail detail) const
{
	return this->float_event_data[detail];
}

int TauDetailsManager::getIntDetailValue(Details::IntEventDetail detail) const
{
	return this->int_event_data[detail];
}
