#include "CaloIdentifier/GTower_ID.h"
#include "CaloEvent/CaloCellContainer.h"
#include "xAODTrigL1Calo/JGTower.h"
#include "xAODTrigL1Calo/JGTowerContainer.h"
#include "xAODTrigL1Calo/JGTowerAuxContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "JetCalibTools/IJetCalibrationTool.h"
#include "JetInterface/IJetUpdateJvt.h"
#include "TrigAnalysisInterfaces/IBunchCrossingTool.h"
#include "Identifier/IdentifierHash.h"

#include <TMatrixD.h>
#include <TMath.h>
#include <TLorentzVector.h>
#include <vector>

//Helper class for patches of towers
struct Patch{
  float sumEt{0.};
  float px{0.};
  float py{0.};
  float pt() {return sqrt(px*px + py*py);}
  float cosPhi() {return pt() > 0 ? px/pt() : 1.;}
  float sinPhi() {return pt() > 0 ? py/pt() : 0.;}
  Patch & add(float Et, float Phi){
    float sinPhi = TMath::Sin(Phi);
    float cosPhi = TMath::Cos(Phi);
    sumEt += Et;
    px += Et*cosPhi;
    py += Et*sinPhi;
    return *this;
  };
};

float m_targetTowerWidth = 0.7;
float m_maxEta = 5.;
float m_caloResSqrtTerm = 15.81;
float m_caloResFloor = 50;
float m_nSigma = 3.0;
float m_constraintWeight = 1.;
float m_trimFactor = 0.9;

unsigned int m_nPhiBins = TMath::TwoPi()/m_targetTowerWidth;
unsigned int m_nEtaBins = 2*m_maxEta/m_targetTowerWidth;
unsigned int m_nTowers = m_nPhiBins*m_nEtaBins;

float m_towerEtaWidth = 2*m_maxEta/m_nEtaBins;
float m_towerPhiWidth = TMath::TwoPi()/m_nPhiBins;

float Run_PUfit(const xAOD::JGTowerContainer* towers, float input_sigma = 3., bool useNegTowers=false){
  
  m_nSigma = input_sigma;
  
  std::vector<Patch> towerConfig;
  towerConfig.resize(m_nTowers);
  
  TH2F* pufitGrid = new TH2F("h_pufitGrid", "",  m_nEtaBins, -m_maxEta, m_maxEta, m_nPhiBins, -TMath::Pi(), TMath::Pi());
  
  float sumEtEta = 0; //Eta restricted sum
  for(unsigned int i = 0; i < towers->size(); i++){
    const xAOD::JGTower* tower = towers->at(i);
    float Et_ = tower->et();
    
    if(!useNegTowers && Et_ < 0) Et_ = 0;
    
    float eta = tower->eta();
    float phi = tower->phi();
    
    if(fabs(eta) > m_maxEta) continue;
    
    sumEtEta += Et_;
    
    int ieta = pufitGrid->GetXaxis()->FindBin(eta);
    int iphi = pufitGrid->GetYaxis()->FindBin(phi);
    
    ieta -= 1;  // to deal with the conversion from root histogram indicies to c++ vector indicies
    iphi -= 1;  // histograms start with 1, vectors with 0
    
    int index = ieta*m_nPhiBins + iphi;
    
    towerConfig.at(index).add(Et_, phi);
    
    float temp = pufitGrid->GetBinContent(ieta, iphi);
    pufitGrid->SetBinContent(ieta, iphi, temp + Et_);
  }
  
  int maxConfig;
  double EtTowerTrimMean = 0.;
  double VarEtTower = 0.;
  double ptCut = 0.;
  
  //Calculate trimmed mean, variance
  std::vector<Patch> tower_array = towerConfig;
  std::sort(tower_array.begin(), tower_array.end(), [](const Patch& lhs, const Patch& rhs) { return lhs.sumEt < rhs.sumEt;});
  
  //Number of towers trimmed from each end
  unsigned int nTrimmed = m_nTowers*(1 - m_trimFactor)/2;
  for(unsigned int ii = 0; ii < m_nTowers - nTrimmed; ii++){
    EtTowerTrimMean += tower_array.at(ii).sumEt;
  }
  EtTowerTrimMean /= (m_nTowers - 2*nTrimmed);
  
  for(unsigned int ii = 0; ii < m_nTowers - nTrimmed; ii++){
    double term = (EtTowerTrimMean - tower_array.at(ii).sumEt)*(EtTowerTrimMean - tower_array.at(ii).sumEt);
    
    //Double lower terms to make up for the exclusion of higher terms
    if(ii < nTrimmed) term *= 2;
    VarEtTower += term;
  }
  VarEtTower /= m_nTowers;
  //set minimum value to avoid FPEs
  ptCut = EtTowerTrimMean + m_nSigma*sqrt(VarEtTower);
  std::vector<Patch> hsTowers;
  unsigned int nHardScatter = 0;
  
  //covariance parameters
  double sigma_yy = 0.;
  double sigma_xx = 0.;
  double sigma_xy = 0.;
    
  //pileup vector
  TVector2 ptPileup;
  double sumEtPileup = 0.;
  double varSumEtPileup = 0.;
  
  float maxTowerEt = 0;
    
  for(Patch patch : tower_array){
      
    if(patch.sumEt > maxTowerEt) maxTowerEt = patch.sumEt;
    
    if(patch.sumEt > ptCut){
      nHardScatter++;
      hsTowers.push_back(patch);
    }else{
      ptPileup += TVector2(patch.px, patch.py);
      float sigma2 = m_caloResSqrtTerm*m_caloResSqrtTerm*patch.pt() + (m_caloResFloor*m_caloResFloor);
      sumEtPileup += patch.pt();
      varSumEtPileup += sigma2;
      
      sigma_yy += sigma2*patch.sinPhi()*patch.sinPhi();
      sigma_xx += sigma2*patch.cosPhi()*patch.cosPhi();
      sigma_xy += sigma2*patch.cosPhi()*patch.sinPhi();
    }
  }
  
  //Stop if no patches are classified as hard scatter
  if(nHardScatter == 0) return 0.;
  
  double sigma_det = sigma_yy*sigma_xx - sigma_xy*sigma_xy;
  
  double cosPhiPileup = ptPileup.Px()/ptPileup.Mod();
  double sinPhiPileup = ptPileup.Py()/ptPileup.Mod();

  //The ratio of the area of one tower to the total area covered by pileup patches
  double aRatio = 1/(m_nTowers - nHardScatter);
  double Vk = VarEtTower;
  
  //Construct the matrix equation
  TMatrixD Xij(nHardScatter, nHardScatter);
  TMatrixD Cj(nHardScatter, 1);
  
  for(unsigned int ii = 0; ii < nHardScatter; ii++){
    Cj[ii][0] = -m_constraintWeight*ptPileup.Mod()/sigma_det*(
							      sigma_yy*hsTowers.at(ii).cosPhi()*cosPhiPileup
							      + sigma_xx*hsTowers.at(ii).sinPhi()*sinPhiPileup
							      - sigma_xy*(hsTowers.at(ii).cosPhi()*sinPhiPileup + hsTowers.at(ii).sinPhi()*cosPhiPileup))
      + EtTowerTrimMean/Vk;
    for(unsigned int jj = ii; jj <nHardScatter; jj++){
      Xij[ii][jj] = m_constraintWeight/sigma_det*(
						  sigma_yy*hsTowers.at(ii).cosPhi()*hsTowers.at(jj).cosPhi()
						  + sigma_xx*hsTowers.at(ii).sinPhi()*hsTowers.at(jj).sinPhi()
						  - sigma_xy*(hsTowers.at(ii).cosPhi()*hsTowers.at(jj).sinPhi() + hsTowers.at(ii).sinPhi()*hsTowers.at(jj).cosPhi()));
      if(ii == jj) Xij[ii][ii] += 1./Vk;
      else Xij[jj][ii] = Xij[ii][jj];
    }
  }
  Xij.Invert();
  TMatrixD corrections(Xij*Cj);
  //Apply the corrections to the "hard scatter" tower patches
  
  float pxMiss = 0;
  float pyMiss = 0;
  float sumEt = 0;
  
  for(unsigned int ii = 0; ii < nHardScatter; ii++){
    hsTowers.at(ii).sumEt -= corrections[ii][0];
    float cosPhi = hsTowers.at(ii).cosPhi();
    float sinPhi = hsTowers.at(ii).sinPhi();
    
    hsTowers.at(ii).px -= corrections[ii][0]*cosPhi;
    hsTowers.at(ii).py -= corrections[ii][0]*sinPhi;
    pxMiss -= hsTowers.at(ii).px;
    pyMiss -= hsTowers.at(ii).py;
    sumEt += hsTowers.at(ii).sumEt;
  }
  
  return TMath::Sqrt(pxMiss*pxMiss + pyMiss*pyMiss);

}
