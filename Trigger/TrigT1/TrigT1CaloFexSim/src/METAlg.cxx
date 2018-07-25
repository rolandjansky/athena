#include "TrigT1CaloFexSim/METAlg.h"
#include "TrigT1CaloFexSim/JGTower.h"
StatusCode METAlg::BuildMET(const xAOD::JGTowerContainer*towers,std::vector<float> noise, METAlg::MET* met){
  float met_x=0;
  float met_y=0;
  for(unsigned t=0; t<towers->size(); t++){
     const xAOD::JGTower* tower = towers->at(t);
     if(tower->et()<noise.at(t)) continue;
     float phi=tower->phi();
     float et =tower->et();
     met_x -= et*cos(phi);
     met_y -= et*sin(phi);
  }

  float et_met = sqrt(met_x*met_x+met_y*met_y);
  float phi_met=TMath::ACos(met_x/et_met);
  if (met_y<0) phi_met = -phi_met;
  met->phi=phi_met;
  met->et = et_met;
  return StatusCode::SUCCESS;
}

