/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include "TrigAFPHypo/TrigAFPDijetComboHypoTool.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/Jet.h"

#include "xAODForward/AFPTrackContainer.h"
#include "xAODForward/AFPTrack.h"

#include "PathResolver/PathResolver.h"
#include "xAODTrigger/TrigPassBits.h"
#include <math.h>


TrigAFPDijetComboHypoTool::TrigAFPDijetComboHypoTool(const std::string& type, const std::string& name, const IInterface* parent)
  : ComboHypoToolBase(type, name, parent) {}

TrigAFPDijetComboHypoTool::~TrigAFPDijetComboHypoTool()
{
}

StatusCode TrigAFPDijetComboHypoTool::initialize() {
  ATH_MSG_DEBUG("Initialization. The correct configuration of this algorithm "
                << "requires jets ordered in increasing energy");

  if (!(m_monTool.name().empty())) {
    ATH_CHECK( m_monTool.retrieve() );
    ATH_MSG_DEBUG("m_monTool name: " << m_monTool);
  }

  return StatusCode::SUCCESS;
}

struct DescendingEt: std::binary_function<const xAOD::Jet*,
                                          const xAOD::Jet*,
                                          bool> {
  bool operator () (const xAOD::Jet* l, const xAOD::Jet* r)  const {
    return l->p4().Et() > r->p4().Et();
  }
};

bool TrigAFPDijetComboHypoTool::executeAlg(std::vector<LegDecision> &combination) const {

  bool pass = false;
  
  std::vector<ElementLink<xAOD::JetContainer>> selected_jets;
  
  if(combination.size() != 2) ATH_MSG_ERROR("Number of previous leg decisions is not 2!");
  
  // Loop over leg decisions
  for(auto comb: combination){
    
    auto dec = comb.second;
    auto jet_link = TrigCompositeUtils::findLink<xAOD::JetContainer>(*dec, TrigCompositeUtils::featureString()).link;
    if(!jet_link.isValid()){
      ATH_MSG_ERROR("Expecting to combine exactly two jets, but instead passed "+std::to_string(combination.size())+". Will throw a runtime error");
      throw std::runtime_error("Expecting to combine exactly two jets, but instead passed "+std::to_string(combination.size()));
    }
    selected_jets.push_back(jet_link);

  }
  
  // Get jet pair
  auto jetLink1 = selected_jets[0]; // Need to check if jets are actually ordered in pT
  auto jetLink2 = selected_jets[1];
  
  TLorentzVector jet1_vec = (*jetLink1)->p4();
  TLorentzVector jet2_vec = (*jetLink2)->p4();
  TLorentzVector dijet = jet1_vec + jet2_vec;

  // Calculate dijet quantities
  double dijetMass = dijet.M() * m_GeV;
  //double dijetEta = dijet.Eta();
  double dijetRapidity = dijet.Rapidity();

  // Calculate relative energy loos
  double xiJet1 = exp(dijetRapidity) * dijetMass / m_totalEnergy;
  double xiJet2 = exp(-dijetRapidity) * dijetMass / m_totalEnergy;
  
  // Predict proton positions on AFP sides A and C
  double predictProton1Energy = m_beamEnergy * (1. - xiJet1); // Side A
  double predictProton2Energy = m_beamEnergy * (1. - xiJet2); // Side C

  // Side A position prediction
  double sideA_predictX = 1e3 * m_transportBeam2->x(0, 0, 0, 0, 0, predictProton1Energy) + m_protonPosShift_x;
  double sideA_predictY = 1e3 * m_transportBeam2->x(0, 0, 0, 0, 0, predictProton1Energy) + m_protonPosShift_y;

  // Side C position prediction
  double sideC_predictX = 1e3 * m_transportBeam1->x(0, 0, 0, 0, 0, predictProton2Energy) + m_protonPosShift_x;
  double sideC_predictY = 1e3 * m_transportBeam1->x(0, 0, 0, 0, 0, predictProton2Energy) + m_protonPosShift_y;

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
