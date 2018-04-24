/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include "TrigAFPHypo/TrigAFPJetAllTE.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/Jet.h"

#include "xAODForward/AFPTrackContainer.h"
#include "xAODForward/AFPTrack.h"

#include "PathResolver/PathResolver.h"
#include "xAODTrigger/TrigPassBits.h"
#include <math.h>


TrigAFPJetAllTE::TrigAFPJetAllTE(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::AllTEAlgo(name, pSvcLocator),
  m_decision(false),
  m_useCachedResult(false),
  m_transportBeam1(nullptr),
  m_transportBeam2(nullptr) {
  //Radius and rectangular thresholds
  declareProperty("maxProtonDist", m_maxProtonDist = 2.0, "Cut on distance between measured and extrapolated proton");
  declareProperty("maxProtonDiff_x", m_maxProtonDiff_x = 2.5,
                  "Cut on x difference between measured and extrapolated proton");
  declareProperty("maxProtonDiff_y", m_maxProtonDiff_y = 2.0,
                  "Cut on y difference between measured and extrapolated proton");
  declareProperty("beamEnergy", m_beamEnergy = 6500.0, "Energy of one beam i.e. half of centre-of-mass energy");
  declareProperty("protonTransportParamFileName1", m_protonTransportParamFileName1 = "final_parameterization_b1.txt",
                  "Name of the file with proton transport parametrisation");
  declareProperty("protonTransportParamFileName2", m_protonTransportParamFileName2 = "final_parameterization_b2.txt",
                  "Name of the file with proton transport parametrisation");
  declareProperty("protonPosShift_x", m_protonPosShift_x = -97.0,
                  "Expected difference in x position between measured and predicted proton.");
  declareProperty("protonPosShift_y", m_protonPosShift_y = 5.0,
                  "Expected difference in y position between measured and predicted proton.");

  declareProperty("alignmentCorrection_nearA", m_alignmentCorrection_nearA = 0,
                  "Shift due to alignment in x for protons in near station A .");
  declareProperty("alignmentCorrection_nearC", m_alignmentCorrection_nearC = 0,
		  "Shift due to alignment in x for protons in near station C .");
  declareProperty("requireDoubleTag", m_requireDoubleTag = true,
		  "Determine if a single or double tag is required in the hypo decision");


  declareMonitoredVariable("NJets", m_jetsN);
  declareMonitoredVariable("Etajj", m_dijetEta);
  declareMonitoredVariable("Yjj", m_dijetRapidity);
  declareMonitoredVariable("Mjj", m_dijetMass);
  declareMonitoredVariable("Diff_pos_A", m_sideA_minDist);
  declareMonitoredVariable("Diff_pos_C", m_sideC_minDist);
  declareMonitoredVariable("N_trk_C", m_sideC_tracksN);
  declareMonitoredVariable("N_trk_A", m_sideA_tracksN);
  declareMonitoredVariable("Diff_x_A", m_sideA_diffX);
  declareMonitoredVariable("Diff_x_C", m_sideC_diffX);
  declareMonitoredVariable("Diff_y_A", m_sideA_diffY);
  declareMonitoredVariable("Diff_y_C", m_sideC_diffY);
  declareMonitoredVariable("x_A", m_sideA_trkX);
  declareMonitoredVariable("x_C", m_sideC_trkX);
  declareMonitoredVariable("y_A", m_sideA_trkY);
  declareMonitoredVariable("y_C", m_sideC_trkY);
  declareMonitoredVariable("param_x_A", m_sideA_predictX);
  declareMonitoredVariable("param_x_C", m_sideC_predictX);
  declareMonitoredVariable("param_y_A", m_sideA_predictY);
  declareMonitoredVariable("param_y_C", m_sideC_predictY);
  }

HLT::ErrorCode TrigAFPJetAllTE::hltInitialize() {
	ATH_MSG_DEBUG("Initialization. The correct configuration of this algorithm "
			<< "requires jets ordered in increasing energy");

	//Retrieving the parameterization file
	std::string filePath1 = PathResolver::find_file(
			m_protonTransportParamFileName1.c_str(), "DATAPATH", PathResolver::RecursiveSearch);
	if (filePath1.length() == 0) {
		ATH_MSG_WARNING(
				"The parameterization file " << m_protonTransportParamFileName1.c_str() <<
				" was not found in optics. Using default file.");
		filePath1 = PathResolver::find_file("final_parameterization_b1.txt", "DATAPATH", PathResolver::RecursiveSearch);
		m_protonTransportParamFileName1 = "final_parameterization_b1.txt";
	} else ATH_MSG_DEBUG("The parameterization file " << m_protonTransportParamFileName1.c_str() << " was found");

	//Defining the parameterization object
	m_transportBeam1 = new AFPProtonTransportTool(filePath1);

	//Retrieving the parameterization file
	std::string filePath2 = PathResolver::find_file(
			m_protonTransportParamFileName2.c_str(), "DATAPATH", PathResolver::RecursiveSearch);
	if (filePath1.length() == 0) {
		ATH_MSG_WARNING(
				"The parameterization file " << m_protonTransportParamFileName2.c_str() <<
				" was not found in optics. Using default file.");
		filePath2 = PathResolver::find_file("final_parameterization_b2.txt", "DATAPATH", PathResolver::RecursiveSearch);
		m_protonTransportParamFileName2 = "final_parameterization_b2.txt";
	} else ATH_MSG_DEBUG("The parameterization file " << m_protonTransportParamFileName2.c_str() << " was found");

	//Defining the parameterization object
	m_transportBeam2 = new AFPProtonTransportTool(filePath2);


	ATH_MSG_INFO("Cut on distance between measured and extrapolated proton: " << m_maxProtonDist);
	ATH_MSG_INFO("Cut on x difference between measured and extrapolated proton: " << m_maxProtonDiff_x);
	ATH_MSG_INFO("Cut on y difference between measured and extrapolated proton: " << m_maxProtonDiff_y);
	ATH_MSG_INFO("Name of the file with proton transport parametrisation beam 1: " << m_protonTransportParamFileName1);
	ATH_MSG_INFO("Name of the file with proton transport parametrisation beam 2: " << m_protonTransportParamFileName2);

	// total energy is twice the beam energy
	m_totalEnergy = 2 * m_beamEnergy;

	return HLT::OK;
}

struct DescendingEt: std::binary_function<const xAOD::Jet*,
	const xAOD::Jet*,
	bool> {
		bool operator () (const xAOD::Jet* l, const xAOD::Jet* r)  const {
			return l->p4().Et() > r->p4().Et();
		}
	};

TrigAFPJetAllTE::~TrigAFPJetAllTE() {
}

HLT::ErrorCode TrigAFPJetAllTE::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& tes_in,
		unsigned int output) {
	ATH_MSG_DEBUG("***  Executing this TrigJetHypo : " << name());

	// do calculations only if result is not cached
	if (!m_useCachedResult) {
		beforeExecMonitors().ignore();
		// Retrieving jets from tes_in[0]
		const xAOD::JetContainer* outJets(nullptr);
		HLT::ErrorCode statJets = getFeature(tes_in[0].front(), outJets);

		if (statJets != HLT::OK) {
			ATH_MSG_WARNING("Failed to get JetCollections");
			return HLT::OK;
		}

		if (outJets == nullptr) {
			ATH_MSG_DEBUG("Got no JetCollections associated to the TE!");
			return HLT::MISSING_FEATURE;
		}

		std::vector<const xAOD::Jet*> theJets(outJets->begin(), outJets->end());

		if (theJets.size() == 0) {
			ATH_MSG_DEBUG("Size of JetCollection is 0!");
			return HLT::OK;
		}

		//Resetting selection variables
		bool passRCutA = false;
		bool passRCutC = false;
		bool passXYCutA = false;
		bool passXYCutC = false;

		//Resetting dijet system variables
		m_dijetMass = -999;
		m_dijetEta = -999;
		m_dijetRapidity = -999;

		m_jetsN = theJets.size();

		if (theJets.size() >= 2) {
			//Sorting jets by energy
			std::sort(theJets.begin(), theJets.end(), DescendingEt());

			const TLorentzVector jet1(theJets[0]->p4());
			const TLorentzVector jet2(theJets[1]->p4());
			const TLorentzVector dijet = jet1 + jet2;

			//To calculate relative energy loss
			m_dijetMass = dijet.M() * m_GeV;
			m_dijetEta = dijet.Eta();
			m_dijetRapidity = dijet.Rapidity();

			const double xiJet1 = exp(m_dijetRapidity) * m_dijetMass / (m_totalEnergy); //As in
			// https://arxiv.org/abs/1503.00699
			const double xiJet2 = exp(-m_dijetRapidity) * m_dijetMass / (m_totalEnergy);

			//A side proton energy prediction
			const double predictProton1_e = m_beamEnergy * (1. - xiJet1);
			//C side proton energy prediction
			const double predictProton2_e = m_beamEnergy * (1. - xiJet2);

			m_sideA_predictX = 1e3 * m_transportBeam2->x(0, 0, 0, 0, 0, predictProton1_e) + m_protonPosShift_x;
			m_sideA_predictY = 1e3 * m_transportBeam2->y(0, 0, 0, 0, 0, predictProton1_e) + m_protonPosShift_y;
			m_sideC_predictX = 1e3 * m_transportBeam1->x(0, 0, 0, 0, 0, predictProton2_e) + m_protonPosShift_x;
			m_sideC_predictY = 1e3 * m_transportBeam1->y(0, 0, 0, 0, 0, predictProton2_e) + m_protonPosShift_y;
			ATH_MSG_DEBUG("x1: " << m_sideA_predictX);
			ATH_MSG_DEBUG("y1: " << m_sideA_predictY);
			ATH_MSG_DEBUG("x2: " << m_sideC_predictX);
			ATH_MSG_DEBUG("y2: " << m_sideC_predictY);

			ATH_MSG_DEBUG("Mjj: " << m_dijetMass
					<< " Etajj: " << m_dijetEta
					<< " Yjj: " << m_dijetRapidity);


			xAOD::AFPTrackContainer* tracks = nullptr;
			if (evtStore()->retrieve(tracks, "AFPTrackContainer").isFailure()) {
				ATH_MSG_WARNING("Could not retrieve AFP Tracks");
				return HLT::OK;
			}
			ATH_MSG_DEBUG("Tracks size: " << tracks->size());

			//Resetting counters and variables
			m_sideA_minDist = 999.;
			m_sideC_minDist = 999.;
			m_sideA_tracksN = 0;
			m_sideC_tracksN = 0;
			const xAOD::AFPTrack* nearestTrkSideA = nullptr;
			const xAOD::AFPTrack* nearestTrkSideC = nullptr;
			// find nearest tracks
			for (const xAOD::AFPTrack* theTrack : *tracks) {
				//Tracks from A side near station
				if (theTrack->stationID() == 1) {
					m_sideA_tracksN++;
					const double xDiff = m_sideA_predictX - (theTrack->xLocal() + m_alignmentCorrection_nearA);
					const double yDiff = m_sideA_predictY - theTrack->yLocal();
					const double distance = sqrt(xDiff * xDiff + yDiff * yDiff);
					//To get the closest track from prediction
					if (distance < m_sideA_minDist) {
						m_sideA_minDist = distance;
						nearestTrkSideA = theTrack;
					}
				}
				//Tracks from C side near station
				else if (theTrack->stationID() == 2) {
					m_sideC_tracksN++;
					const double xDiff = m_sideC_predictX - (theTrack->xLocal() + m_alignmentCorrection_nearC);
					const double yDiff = m_sideC_predictY - theTrack->yLocal();
					const double distance = sqrt(xDiff * xDiff + yDiff * yDiff);
					if (distance < m_sideC_minDist) {
						m_sideC_minDist = distance;
						nearestTrkSideC = theTrack;
					}
				}
			} // close loop over afp tracks

			// check cuts on A side
			if (nearestTrkSideA) {
				m_sideA_trkX = nearestTrkSideA->xLocal() + m_alignmentCorrection_nearA;
				m_sideA_trkY = nearestTrkSideA->yLocal();
				m_sideA_diffX = m_sideA_predictX - m_sideA_trkX;
				m_sideA_diffY = m_sideA_predictY - m_sideA_trkY;

				//Cutting on distance relative to the mean
				if (m_sideA_minDist < m_maxProtonDist) passRCutA = true;

				//Cutting on x and y relative to the mean
				if (fabs(m_sideA_diffX) < m_maxProtonDiff_x && fabs(m_sideA_diffY) < m_maxProtonDiff_y) passXYCutA = true;
			} else {
				m_sideA_trkX = 999;
				m_sideA_trkY = 999;
				m_sideA_diffX = 999;
				m_sideA_diffY = 999;
			}

			// check cuts on C side
			if (nearestTrkSideC) {
				m_sideC_trkX = nearestTrkSideC->xLocal() + m_alignmentCorrection_nearC;
				m_sideC_trkY = nearestTrkSideC->yLocal();
				m_sideC_diffX = m_sideC_predictX - m_sideC_trkX;
				m_sideC_diffY = m_sideC_predictY - m_sideC_trkY;

				//Cutting on distance relative to the mean
				if (m_sideC_minDist < m_maxProtonDist) passRCutC = true;

				//Cutting on x and y relative to the mean
				if (fabs(m_sideC_diffX) < m_maxProtonDiff_x && fabs(m_sideC_diffY) < m_maxProtonDiff_y) passXYCutC = true;
			} else {
				m_sideC_trkX = 999;
				m_sideC_trkY = 999;
				m_sideC_diffX = 999;
				m_sideC_diffY = 999;
			}

			// make decision
			const bool passSideA = passRCutA && passXYCutA;
			const bool passSideC = passRCutC && passXYCutC;
			if (m_requireDoubleTag) m_decision = passSideA && passSideC;
			else m_decision = passSideA || passSideC;

			if(m_decision) ATH_MSG_DEBUG("AFPJetAllTE: Event passed! SUCCESS!");

			afterExecMonitors().ignore();
			ATH_MSG_DEBUG("Before the end of TrigAFPJetAllTE::Execute");
		} // close if (theJets.size() >= 2)

		m_useCachedResult = true;
	} // close if (!m_useCachedResult)

	// save decision
	HLT::TEVec allTEs;
	HLT::TriggerElement* outputTE = config()->getNavigation()->addNode(allTEs, output);
	outputTE->setActiveState(m_decision);

	return HLT::OK;
}

HLT::ErrorCode TrigAFPJetAllTE::hltEndEvent() {
	m_useCachedResult = false;
	m_decision = false;
	return HLT::OK;
}

HLT::ErrorCode TrigAFPJetAllTE::hltFinalize() {
	if (m_transportBeam1) {
		delete m_transportBeam1;
		m_transportBeam1 = nullptr;
	}

	if (m_transportBeam2) {
		delete m_transportBeam2;
		m_transportBeam2 = nullptr;
	}

	return HLT::OK;
}
