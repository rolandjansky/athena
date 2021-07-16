/* 
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration 
*/

#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TestTrigAFPDijetHypoTool.h"
#include "PathResolver/PathResolver.h"

using TrigCompositeUtils::addDecisionID;

TestTrigAFPDijetHypoTool::TestTrigAFPDijetHypoTool(const std::string &type, const std::string &name, const IInterface *parent) 
  : AthAlgTool(type, name, parent),   
    m_decisionId(HLT::Identifier::fromToolName(name)), 
    m_transportBeamA("AFPProtonTransportTool/TransportBeamA", this),
    m_transportBeamC("AFPProtonTransportTool/TransportBeamC", this)
{
  declareProperty("TransportBeamA",m_transportBeamA,"Proton transport tool side A");
  declareProperty("TransportBeamC",m_transportBeamC,"Proton transport tool side C");
}

StatusCode TestTrigAFPDijetHypoTool::initialize() 
{ 

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
  ATH_MSG_DEBUG("Path to param file side C: "<<filePathC);
  //Defining the parameterization object                                             
  m_transportBeamC->setParamFile(filePathC);
  ATH_CHECK(m_transportBeamC->load());

  return StatusCode::SUCCESS; 
}

StatusCode TestTrigAFPDijetHypoTool::decide(TrigAFPDijetHypoInfo &info) const {

  // Get input jet collection
  const xAOD::JetContainer *jetCollection = info.jetInputCollection;
  if(jetCollection->size()<2){
    ATH_MSG_DEBUG("Number of jets is smaller than 2");
    return StatusCode::SUCCESS;
  }

  // Get two leading jets
  const xAOD::Jet *jet1 = jetCollection->at(0);
  const xAOD::Jet *jet2 = jetCollection->at(1);

  ATH_MSG_DEBUG("Jets pt:"<<jet1->pt()<<";"<<jet2->pt());

  TLorentzVector dijet = jet1->p4() + jet2->p4();

  // Calculate dijet quantities                           
  auto dijetMass = Monitored::Scalar( "DijetMass"   , -1.0 );
  //auto monitorIt    = Monitored::Group( m_monTool, dijetMass);
  auto dijetRapidity = Monitored::Scalar( "DijetRapidity"   , -1.0 );
  dijetMass = dijet.M() * m_GeV;
  dijetRapidity = dijet.Rapidity();
  Monitored::fill(m_monTool, dijetMass);
  Monitored::fill(m_monTool, dijetRapidity);

  // Calculate relative energy loss
  auto xiJet1 = Monitored::Scalar( "XiJet1"   , -1.0 );
  auto xiJet2 = Monitored::Scalar( "XiJet2"   , -1.0 );
  xiJet1 = exp(dijetRapidity) * dijetMass / m_totalEnergy;
  xiJet2 = exp(-dijetRapidity) * dijetMass / m_totalEnergy;
  ATH_MSG_DEBUG("xi Jet 2:"<<xiJet2);
  Monitored::fill(m_monTool, xiJet1);
  Monitored::fill(m_monTool, xiJet2);

  // Predict proton positions on AFP sides A and C
  auto predictProtonAEnergy = Monitored::Scalar( "PredictProtonAEnergy"   , -1.0 );
  auto predictProtonCEnergy = Monitored::Scalar( "PredictProtonCEnergy"   , -1.0 );
  predictProtonAEnergy = m_beamEnergy * (1. - xiJet1); // Side A                           
  predictProtonCEnergy = m_beamEnergy * (1. - xiJet2); // Side C
  ATH_MSG_DEBUG("Predict proton energy A:"<<predictProtonAEnergy);
  ATH_MSG_DEBUG("Predict proton energy C:"<<predictProtonCEnergy);
  Monitored::fill(m_monTool, predictProtonAEnergy);
  Monitored::fill(m_monTool, predictProtonCEnergy);  

  // Side A position prediction
  auto sideA_predictX = Monitored::Scalar( "SideA_predictX" , -1.0 );
  auto sideA_predictY = Monitored::Scalar( "SideA_predictY" , -1.0 );
  sideA_predictX = 1e3 * m_transportBeamA->x(0, 0, 0, 0, 0, predictProtonAEnergy) + m_protonPosShift_x;
  sideA_predictY = 1e3 * m_transportBeamA->x(0, 0, 0, 0, 0, predictProtonAEnergy) + m_protonPosShift_y;

  ATH_MSG_DEBUG("Side A predict X:"<<sideA_predictX);
  Monitored::fill(m_monTool, sideA_predictX);
  Monitored::fill(m_monTool, sideA_predictY);  

  // Side C position prediction
  auto sideC_predictX = Monitored::Scalar( "SideC_predictX" , -1.0 );
  auto sideC_predictY = Monitored::Scalar( "SideC_predictY" , -1.0 );
  sideC_predictX = 1e3 * m_transportBeamC->x(0, 0, 0, 0, 0, predictProtonCEnergy) + m_protonPosShift_x;
  sideC_predictY = 1e3 * m_transportBeamC->x(0, 0, 0, 0, 0, predictProtonCEnergy) + m_protonPosShift_y;

  ATH_MSG_DEBUG("Side C predict X:"<<sideA_predictX);
  Monitored::fill(m_monTool, sideC_predictX);
  Monitored::fill(m_monTool, sideC_predictY);    
  
  // Get input track collection
  const xAOD::AFPTrackContainer *afpTrackCollection = info.afpTracksInputCollection;

  if(afpTrackCollection==nullptr){
    ATH_MSG_DEBUG("AFP track collection is null");
    return StatusCode::SUCCESS;
  }

  if(afpTrackCollection->size()<2){
    ATH_MSG_DEBUG("Less than 2 track reconstructed in AFP");
    return StatusCode::SUCCESS;
  }

  double sideA_minDist = 9e9;
  double sideC_minDist = 9e9;

  int nearestTrackSideAId = 999;
  int nearestTrackSideCId = 999;

  auto xDiff = Monitored::Scalar("XDiff",-1.0);
  auto yDiff = Monitored::Scalar("YDiff",-1.0);
  auto distance = Monitored::Scalar("distance",-1.0);

  for(auto track: (*afpTrackCollection)){

    xDiff = -1.0;
    yDiff = -1.0;
    distance = -1.0;

    if(track->stationID() == 1){

      xDiff = sideA_predictX - (track->xLocal() + m_alignmentCorrection_nearA);
      yDiff = sideA_predictY - track->yLocal();
      distance = sqrt(xDiff * xDiff + yDiff * yDiff);

      if(distance < sideA_minDist){
        sideA_minDist = distance;
        nearestTrackSideAId = track->index();
      }

    }else if(track->stationID() == 2){

      xDiff = sideC_predictX - (track->xLocal() + m_alignmentCorrection_nearC);
      yDiff = sideC_predictY - track->yLocal();
      distance = sqrt(xDiff * xDiff + yDiff * yDiff);

      if(distance < sideC_minDist){
        sideC_minDist = distance;
        nearestTrackSideCId = track->index();
      }
    }// End of if for choosing AFP station     

    ATH_MSG_DEBUG("X diff:"<<xDiff);
    ATH_MSG_DEBUG("Y diff:"<<yDiff);
    Monitored::fill(m_monTool, xDiff);
    Monitored::fill(m_monTool, yDiff);
    Monitored::fill(m_monTool, distance);

  }// End of loop over AFP tracks

  bool passRCutA = false;
  bool passRCutC = false;

  bool passXYCutA = false;
  bool passXYCutC = false;

  auto sideA_trackX = Monitored::Scalar("SideA_trackX",-1.0);
  auto sideA_trackY = Monitored::Scalar("SideA_trackY",-1.0);
  auto sideA_diffX = Monitored::Scalar("SideA_diffX",-1.0);
  auto sideA_diffY = Monitored::Scalar("SideA_diffY",-1.0);

  auto sideC_trackX = Monitored::Scalar("SideC_trackX",-1.0);
  auto sideC_trackY = Monitored::Scalar("SideC_trackY",-1.0);
  auto sideC_diffX = Monitored::Scalar("SideC_diffX",-1.0);
  auto sideC_diffY = Monitored::Scalar("SideC_diffY",-1.0);

  // Cuts on A side        
  if(nearestTrackSideAId != 999){

    sideA_trackX = afpTrackCollection->at(nearestTrackSideAId)->xLocal() + m_alignmentCorrection_nearA;
    sideA_trackY = afpTrackCollection->at(nearestTrackSideAId)->yLocal();
    sideA_diffX = sideA_predictX - sideA_trackX;
    sideA_diffY = sideA_predictY - sideA_trackY;

    if(sideA_minDist < m_maxProtonDist) passRCutA = true;

    if(std::abs(sideA_diffX) < m_maxProtonDiff_x && std::abs(sideA_diffY) < m_maxProtonDiff_y) passXYCutA = true;

  }else{
    passRCutA = false;
    passXYCutA = false;
  }

  // Cuts on C side
  if(nearestTrackSideCId != 999){

    sideC_trackX = afpTrackCollection->at(nearestTrackSideCId)->xLocal() + m_alignmentCorrection_nearC;
    sideC_trackY = afpTrackCollection->at(nearestTrackSideCId)->yLocal();
    sideC_diffX = sideC_predictX - sideC_trackX;
    sideC_diffY = sideC_predictY - sideC_trackY;

    if(sideC_minDist < m_maxProtonDist) passRCutC = true;

    if(std::abs(sideC_diffX) < m_maxProtonDiff_x && std::abs(sideC_diffY) < m_maxProtonDiff_y) passXYCutC = true;

  }else{
    passRCutC = false;
    passXYCutC = false;
  }

  Monitored::fill(m_monTool,sideA_trackX);
  Monitored::fill(m_monTool,sideA_trackY);
  Monitored::fill(m_monTool,sideA_diffX);
  Monitored::fill(m_monTool,sideA_diffY);

  Monitored::fill(m_monTool,sideC_trackX);
  Monitored::fill(m_monTool,sideC_trackY);
  Monitored::fill(m_monTool,sideC_diffX);
  Monitored::fill(m_monTool,sideC_diffY);

  bool pass = false;

  if(passRCutA && passRCutC && passXYCutA && passXYCutC) pass = true;

  if(pass){
    addDecisionID(m_decisionId.numeric(), info.decision); 
    ATH_MSG_DEBUG("Event accepted");
  }else{
    ATH_MSG_DEBUG("Event rejected");
  }
  return StatusCode::SUCCESS;

}
