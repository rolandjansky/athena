/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#include "TrigAFPDijetComboHypoTool.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/Jet.h"

#include "xAODForward/AFPTrackContainer.h"
#include "xAODForward/AFPTrack.h"

#include "PathResolver/PathResolver.h"
#include "xAODTrigger/TrigPassBits.h"
#include <math.h>

TrigAFPDijetComboHypoTool::TrigAFPDijetComboHypoTool(const std::string& type, const std::string& name, const IInterface* parent)
  : ComboHypoToolBase(type, name, parent),
    m_transportBeamA("AFPProtonTransportTool/TransportBeamA", this),
    m_transportBeamC("AFPProtonTransportTool/TransportBeamC", this)
{
  declareProperty("TransportBeamA",m_transportBeamA,"Proton transport tool side A");
  declareProperty("TransportBeamC",m_transportBeamC,"Proton transport tool side C");
}

TrigAFPDijetComboHypoTool::~TrigAFPDijetComboHypoTool()
{
}

StatusCode TrigAFPDijetComboHypoTool::initialize() {
  ATH_MSG_INFO("Initialization. The correct configuration of this algorithm "
                << "requires jets ordered in increasing energy");

  if (!(m_monTool.name().empty())) {
    ATH_CHECK( m_monTool.retrieve() );
    ATH_MSG_DEBUG("m_monTool name: " << m_monTool);
  }

  if(!(m_transportBeamA.name().empty())){
    ATH_CHECK( m_transportBeamA.retrieve() );
    ATH_MSG_DEBUG("m_transportBeamA name: " << m_transportBeamA);
  }

  if(!(m_transportBeamC.name().empty())){
    ATH_CHECK( m_transportBeamC.retrieve() );
    ATH_MSG_DEBUG("m_transportBeamC name: " << m_transportBeamC);
  }

  //Retrieving the parameterization file for A side
  // By default it used file final_parameterization_b1.txt
  ATH_MSG_DEBUG("Parameterization file for A side: "<< m_protonTransportParamFileNameA);
  std::string filePathA = PathResolver::find_file(m_protonTransportParamFileNameA, "DATAPATH", PathResolver::RecursiveSearch);
  ATH_MSG_DEBUG("Path to param file side A: "<<filePathA);
  //Defining the parameterization object   
  m_transportBeamA->setParamFile(filePathA);
  ATH_CHECK(m_transportBeamA->load());
  
  //Retrieving the parameterization file for C side
  // By default it used file final_parameterization_b2.txt
  ATH_MSG_DEBUG("Parameterization file for C side: "<< m_protonTransportParamFileNameC);
  std::string filePathC = PathResolver::find_file(m_protonTransportParamFileNameC, "DATAPATH", PathResolver::RecursiveSearch);
  ATH_MSG_DEBUG("Path to param file side A: "<<filePathA);
  //Defining the parameterization object   
  m_transportBeamC->setParamFile(filePathC);
  ATH_CHECK(m_transportBeamC->load());
  
  return StatusCode::SUCCESS;
}

struct DescendingEt: std::binary_function<const xAOD::Jet*,
                                          const xAOD::Jet*,
                                          bool> {
  bool operator () (const xAOD::Jet* l, const xAOD::Jet* r)  const {
    return l->p4().Et() > r->p4().Et();
  }
};

bool TrigAFPDijetComboHypoTool::executeAlg(const std::vector<Combo::LegDecision>& combination) const {

  ATH_MSG_DEBUG("TrigAFPDijetComboHypoTool::executeAlg Executing algorithm");

  auto dijetMass = Monitored::Scalar( "DijetMass"   , -1.0 );
  auto monitorIt    = Monitored::Group( m_monTool, dijetMass);

  bool pass = false;
  
  std::vector<ElementLink<xAOD::JetContainer>> selected_jets;
  
  // Expecting to run over chains with the signature HLT_afp_2jX, and hence to be supplied with three Decision Objects for each combination.
  if(combination.size() != 3) ATH_MSG_ERROR("Number of leg decisions is not 3!");
  
  // One of these is the AFP Decision Object, which we ignore as we are fetching the AFP reconstruction directly from a ReadHandle.
  // The other two should be Jet Decision Objects.
  for(const auto& comb: combination){
    const auto dec = comb.second;
    const auto jet_link = TrigCompositeUtils::findLink<xAOD::JetContainer>(*dec, TrigCompositeUtils::featureString()).link;
    if (jet_link.isValid()) {
      selected_jets.push_back(jet_link);
    }
  }

  if(selected_jets.size() != 2){
    ATH_MSG_ERROR("Expecting to combine exactly two jets, but instead found "
                  << selected_jets.size() << ". Will throw a runtime error");
    throw std::runtime_error(
      "Expecting to combine exactly two jets, but instead found " +
      std::to_string(selected_jets.size()));
  }

  // Get jet pair
  auto jetLink1 = selected_jets[0]; // Need to check if jets are actually ordered in pT
  auto jetLink2 = selected_jets[1];
  
  TLorentzVector jet1_vec = (*jetLink1)->p4();
  TLorentzVector jet2_vec = (*jetLink2)->p4();
  TLorentzVector dijet = jet1_vec + jet2_vec;

  // Calculate dijet quantities
  dijetMass = dijet.M() * m_GeV;
  ATH_MSG_DEBUG("AFP ComboHypo::DijetMass "<<dijetMass);
  Monitored::fill(m_monTool, dijetMass);
  double dijetRapidity = dijet.Rapidity();

  // Calculate relative energy loos
  double xiJet1 = exp(dijetRapidity) * dijetMass / m_totalEnergy;
  double xiJet2 = exp(-dijetRapidity) * dijetMass / m_totalEnergy;
  
  // Predict proton positions on AFP sides A and C
  double predictProtonAEnergy = m_beamEnergy * (1. - xiJet1); // Side A
  double predictProtonCEnergy = m_beamEnergy * (1. - xiJet2); // Side C

  // Side A position prediction
  double sideA_predictX = 1e3 * m_transportBeamA->x(0, 0, 0, 0, 0, predictProtonAEnergy) + m_protonPosShift_x;
  double sideA_predictY = 1e3 * m_transportBeamA->x(0, 0, 0, 0, 0, predictProtonAEnergy) + m_protonPosShift_y;

  // Side C position prediction
  double sideC_predictX = 1e3 * m_transportBeamC->x(0, 0, 0, 0, 0, predictProtonCEnergy) + m_protonPosShift_x;
  double sideC_predictY = 1e3 * m_transportBeamC->x(0, 0, 0, 0, 0, predictProtonCEnergy) + m_protonPosShift_y;

  // Retrieve AFP track container
  SG::ReadHandle<xAOD::AFPTrackContainer> tracksAFP(m_AFPtrackCollectionReadKey);

  double sideA_minDist = 9e9;
  double sideC_minDist = 9e9;
  
  int nearestTrackSideAId = 999;
  int nearestTrackSideCId = 999;
  
  // Find nearest track
  for(auto track: (*tracksAFP)){
    
    // Side A
    if(track->stationID() == 1){
      
      double xDiff = sideA_predictX - (track->xLocal() + m_alignmentCorrection_nearA);
      double yDiff = sideA_predictY - track->yLocal();
      double distance = sqrt(xDiff * xDiff + yDiff * yDiff);

      if(distance < sideA_minDist){
	sideA_minDist = distance;
	nearestTrackSideAId = track->index();	
      }
      
    }else if(track->stationID() == 2){

      double xDiff = sideC_predictX - (track->xLocal() + m_alignmentCorrection_nearC);
      double yDiff = sideC_predictY - track->yLocal();
      double distance = sqrt(xDiff * xDiff + yDiff * yDiff);

      if(distance < sideC_minDist){
        sideC_minDist = distance;
	nearestTrackSideCId = track->index();
      }
    }// End of if for choosing AFP station
    
  }// End of loop over AFP track

  bool passRCutA = false;
  bool passRCutC = false;
  
  bool passXYCutA = false;
  bool passXYCutC = false;

  // Cuts on A side
  if(nearestTrackSideAId != 999){

    double sideA_trackX = tracksAFP.get()->at(nearestTrackSideAId)->xLocal() + m_alignmentCorrection_nearA;
    double sideA_trackY = tracksAFP.get()->at(nearestTrackSideAId)->yLocal();
    double sideA_diffX = sideA_predictX - sideA_trackX;
    double sideA_diffY = sideA_predictY - sideA_trackY;

    ATH_MSG_DEBUG("TrigAFPDijetComboHypoTool::execute sideA_diffX: " << sideA_diffX);
    
    if(sideA_minDist < m_maxProtonDist) passRCutA = true; 

    if(fabs(sideA_diffX) < m_maxProtonDiff_x && fabs(sideA_diffY) < m_maxProtonDiff_y) passXYCutA = true;

  }else{
    passRCutA = false;
    passXYCutA = false;
  }

  // Cuts on C side
  if(nearestTrackSideCId != 999){

    double sideC_trackX = tracksAFP.get()->at(nearestTrackSideCId)->xLocal() + m_alignmentCorrection_nearC;
    double sideC_trackY = tracksAFP.get()->at(nearestTrackSideCId)->yLocal();
    double sideC_diffX = sideC_predictX - sideC_trackX;
    double sideC_diffY = sideC_predictY - sideC_trackY;
    
    if(sideC_minDist < m_maxProtonDist) passRCutC = true; 

    if(fabs(sideC_diffX) < m_maxProtonDiff_x && fabs(sideC_diffY) < m_maxProtonDiff_y) passXYCutC = true;

  }else{
    passRCutC = false;
    passXYCutC = false;
  }

  if(passRCutA && passRCutC && passXYCutA && passXYCutC) pass = true;

  return pass;

}
