/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "FwdCell.h"
#include "TMath.h"
#include <math.h>

// Default constructor, should never be used.
// This implementation exists only to print the warning and avoid undefined behavior.
FwdCell::FwdCell(){
  m_eta = -999;
  m_deta= -999;
  m_phi = -999;
  m_dphi= -999;
}

// Constructor
FwdCell::FwdCell(float eta, float deta, float phi, float dphi){
  m_eta = eta;
  m_deta = deta;
  m_phi = phi;
  m_dphi = dphi;
}

//Destructor
FwdCell::~FwdCell(){}

float roundp1(float value){
  return round(value*10.)/10.;
}

//Tools
bool isInBox(float eta1, float eta2, float deta, float phi1, float phi2, float dphi){

  bool inEtaBox = std::fabs(eta1-eta2)<deta;
  float deltaPhi= std::fabs(phi1-phi2);
  if(deltaPhi>TMath::Pi()) deltaPhi = 2*TMath::Pi()-deltaPhi;

  bool inPhiBox = deltaPhi<dphi;

  return inEtaBox&&inPhiBox;
}

bool isInBoxXY(float x1, float x2, float dx, float y1, float y2, float dy, float z1, float z2){

  bool inXBox = std::fabs(x1-x2)<dx;
  bool inYBox = std::fabs(y1-y2)<dy;
  bool sameDirection = ((z1*z2)>0);
  return inXBox&&inYBox&&sameDirection;
}

//Tools
bool inOnBox(float eta1, float eta2, float deta, float phi1, float phi2, float dphi){

  bool inEtaBox = std::fabs(eta1-eta2)<=deta;
  float deltaPhi= std::fabs(phi1-phi2);
  if(deltaPhi>TMath::Pi()) deltaPhi = 2*TMath::Pi()-deltaPhi;

  bool inPhiBox = deltaPhi<=dphi;

  return inEtaBox&&inPhiBox;
}

// Use the Full delta of the window/cell
// This function assumes a rectange in eta/phi
float percentageInWindowEtaPhi(float etaWindow, float etaCell, float detaWindow, float detaCell, float phiWindow, float phiCell, float dphiWindow, float dphiCell){

  float etaDown = 0.,etaUp = 0.,phiDown = 0.,phiUp = 0.;

  if(!isInBox(etaWindow, etaCell, detaWindow/2., phiWindow, phiCell, dphiWindow/2.)) return 0.0;
  else{
    etaDown = std::max(etaWindow-detaWindow/2.,etaCell-detaCell/2.);
    etaUp = std::min(etaWindow+detaWindow/2.,etaCell+detaCell/2.);

    // Transform phi range from [-pi,pi] to [0,2pi]
    // Since we only calculate deltas this does not affect the result
    phiWindow = phiWindow+TMath::Pi();
    phiCell = phiCell+TMath::Pi();

    phiDown = std::max(phiWindow-dphiWindow/2.,phiCell-dphiCell/2.);
    phiUp = std::min(phiWindow+dphiWindow/2.,phiCell+dphiCell/2.);
  }

  float areaOverlap = std::fabs((phiUp-phiDown)*(etaUp-etaDown));
  float areaCell = std::fabs(detaCell*dphiCell);
  float percentageOverlap = areaOverlap/areaCell;

  if(percentageOverlap>1) percentageOverlap=1.;

  return percentageOverlap;
}

// Use the full delta of the window/cell
// This function assumes a rectange in x/y
float percentageInWindowFCAL(float etaWindow, float etaCell, float detaWindow, float detaCell, float phiWindow, float phiCell, float dphiWindow, float dphiCell){

  float etaDown = 0.,etaUp = 0.,phiDown = 0.,phiUp = 0.;

  if(!isInBox(etaWindow, etaCell, detaWindow/2., phiWindow, phiCell, dphiWindow/2.)) return 0.0;
  else{
    etaDown = std::max(etaWindow-detaWindow/2.,etaCell-detaCell/2.);
    etaUp = std::min(etaWindow+detaWindow/2.,etaCell+detaCell/2.);

    // Transform phi range from [-pi,pi] to [0,2pi]
    // Since we only calculate deltas this does not affect the result
    phiWindow = phiWindow+TMath::Pi();
    phiCell = phiCell+TMath::Pi();

    phiDown = std::max(phiWindow-dphiWindow/2.,phiCell-dphiCell/2.);
    phiUp = std::min(phiWindow+dphiWindow/2.,phiCell+dphiCell/2.);
  }

  float areaOverlap = std::fabs((phiUp-phiDown)*(etaUp-etaDown));
  float areaCell = std::fabs(detaCell*dphiCell);
  float percentageOverlap = areaOverlap/areaCell;

  if(percentageOverlap>1) percentageOverlap=1.;

  return percentageOverlap;
}

// Tranform helper function
std::vector<float> fromEtaPhiLayerToXY(float eta, float phi, int layer){
  std::vector<float> rLayers ={493.195,538.915,587.125}; // in cm

  float theta = TMath::ATan(TMath::Exp(-eta))*2;
  float radius = std::fabs(TMath::Tan(theta)*rLayers[layer]);

  float x = TMath::Cos(phi)*radius*10;//To get to mm
  float y = TMath::Sin(phi)*radius*10;// To get to mm

  std::vector<float> xyPair = {x,y};
  return xyPair;

}
// Tranform helper function (taken from M. Weirich)
std::vector<float> fromXYtoEtaPhi(float x, float y, float z){
  float sinTh = std::sqrt((x*x+y*y)/(z*z));
  float phi = 0;
  if(x>0) phi = std::atan2(y,x);
  else if(x==0&&y<0) phi = -M_PI/2.;
  else if(x==0&&y>0) phi = M_PI/2.;
  else if(x<0&&y>=0) phi = std::atan2(y,x)+M_PI;
  else if(x<0&&y<0) phi = std::atan2(y,x)-M_PI;
  else phi = -999;

  float theta = asin(sinTh);
  theta = std::atan2(std::sqrt(x*x+y*y),std::fabs(z));
  float eta = 0;
  if(z>0) eta = -std::log(tan(theta/2.));
  if(z<0) eta = std::log(tan(theta/2.));

  std::vector<float> etaPhiPair = {eta,phi};
  return  etaPhiPair;
}

float deltaPhiAtlas(float phi1,float phi2){

  float dphi = phi1-phi2;
  if(dphi>TMath::Pi()) dphi = TMath::Pi()*2 - dphi;
  if(dphi<-TMath::Pi()) dphi = -TMath::Pi()*2 - dphi;
  return dphi;
}

float deltaR(float phi1, float phi2, float eta1, float eta2){
  float dR = std::sqrt((phi1-phi2)*(phi1-phi2)+(eta1-eta2)*(eta1-eta2));
  return dR;
}

std::vector<CaloCell*> sortCells(std::vector<CaloCell*> cells){
  std::sort(cells.begin(), cells.end(),[] (const CaloCell* a, const CaloCell* b )
    {
      return a->et() < b->et();
    });
  return cells;
}

float sumOfEnergy(std::vector<CaloCell*> cells){
  float et = 0;
  for(unsigned i=0;i<cells.size();i++){
    et +=cells.at(i)->et();
  }
  return et;
}

// Determine the weighted centre of the shower in the first layer
std::vector<float> firstlayerShowerCentre(std::vector<CaloCell*> cells){
  float xCentre = 0;
  float yCentre = 0;
  float zCentre = 0;
  float etSum = 0;
  for(unsigned i=0;i<cells.size();i++){
    float et =cells.at(i)->et();
    float x = cells.at(i)->x();
    float y = cells.at(i)->y();
    float z = cells.at(i)->z();
    bool isFirstLayer = (cells.at(i)->caloDDE()->getLayer()==1);

    if(isFirstLayer){
      xCentre += x*et;
      yCentre += y*et;
      zCentre += z*et;
      etSum +=et;
    }
  }

  xCentre = xCentre/etSum;
  yCentre = yCentre/etSum;
  zCentre = zCentre/etSum;

  std::vector<float> etaPhiCentre = fromXYtoEtaPhi(xCentre,yCentre,zCentre);
  std::vector<float> centres = {xCentre,yCentre,zCentre,etaPhiCentre.at(0),etaPhiCentre.at(1)};

  return centres;

}

// Weighted variance from centre of shower
std::vector<float> varianceFromCentre(std::vector<CaloCell*> cells){

  float rCentre = 0;
  float zCentre = 0;
  float etSum = 0;
  //Background: https://en.wikipedia.org/wiki/Weighted_arithmetic_mean
  for(unsigned i=0;i<cells.size();i++){
    float et =cells.at(i)->et();
    float x = cells.at(i)->x();
    float y = cells.at(i)->y();
    float z = cells.at(i)->z();

    etSum += et;
    rCentre += std::sqrt(x*x+y*y)*et;
    zCentre += z*et;

  }
  rCentre = rCentre/etSum;
  zCentre = zCentre/etSum;

  float varR = 0;
  float varZ = 0;
  for(unsigned i=0;i<cells.size();i++){
    float et =cells.at(i)->et();
    float x = cells.at(i)->x();
    float y = cells.at(i)->y();
    float z = cells.at(i)->z();
    if(etSum!=0 && et!=0){
      varR += (std::sqrt(x*x+y*y)-rCentre)*(std::sqrt(x*x+y*y)-rCentre)*(et/etSum)*(et/etSum);
      varZ += (z-zCentre)*(z-zCentre)*(et/etSum)*(et/etSum);
    }else{
      varR += 0;
      varZ += 0;
    }

  }
  std::vector<float> variance;
  if(cells.size()>0){
    variance.push_back(varR);
    variance.push_back(varZ);
  }else{
    variance.push_back(-999999);
    variance.push_back(-999999);
  }

  return variance;

}

// E_{T,max}/Sum(E_{T,i})
float fractionMaxEnergyCell(std::vector<CaloCell*> cells){

  float etSum = 0;
  float maxEnergy = -999999;
  for(unsigned i=0;i<cells.size();i++){
    float et =cells.at(i)->et();
    etSum += et;
    if(et>maxEnergy)maxEnergy=et;
  }
  float fraction = -999999;
  if(etSum!=0) fraction= maxEnergy/etSum;

  return fraction;

}

// Depth of the shower centre in z
float showerDepthZ(std::vector<CaloCell*> cells){
  // use face of cell, not centre
  // dz returns half width according to https://twiki.cern.ch/twiki/bin/view/AtlasProtected/FCalGeometry

  float minZpos = 999999;
  float maxZneg = -999999;
  float etZ = 0;
  float etSum = 0;
  
  for(unsigned c=0;c<cells.size();c++){
    float et = cells.at(c)->et();
    float z = 0;
    if(cells.at(c)->z()>0) z=cells.at(c)->z()-cells.at(c)->caloDDE()->dz();
    if(cells.at(c)->z()<0) z=cells.at(c)->z()+cells.at(c)->caloDDE()->dz();
    if(z>0 && z<minZpos) minZpos=z;
    if(z<0 && z>maxZneg) maxZneg=z;

    etZ += z*et;
    etSum +=et;
  }
  float centreZ = -999999;
  if(etSum!=0) centreZ = etZ/etSum;

  float depthZ = -99999;
  if(centreZ>0) depthZ = std::fabs(centreZ-minZpos);
  if(centreZ<0) depthZ = std::fabs(maxZneg-centreZ);

  return depthZ;

}

// Overlap between two rectangles (seed <-> cells)
float rectangeOverlapSeedCell(float seed_x, float seed_y, float seed_dx, float seed_dy, CaloCell* cell){

    float cell_x = cell->x();
    float cell_y = cell->y();
    float cell_z = cell->z();
    float cell_dx = cell->caloDDE()->dx();
    float cell_dy = cell->caloDDE()->dy();

    float xlow_cell = cell_x-cell_dx/2.;
    float xup_cell = cell_x+cell_dx/2.;
    float ylow_cell = cell_y-cell_dy/2.;
    float yup_cell = cell_y+cell_dy/2.;
    float xlow_seed = seed_x-seed_dx/2.;
    float xup_seed = seed_x+seed_dx/2.;
    float ylow_seed = seed_y-seed_dy/2.;
    float yup_seed = seed_y+seed_dy/2.;


    std::vector<float> xy_bl = fromXYtoEtaPhi(xlow_cell,ylow_cell,cell_z);
    std::vector<float> projected_bl = projectXYCoordinates(xlow_cell,ylow_cell,cell_z,xy_bl.at(0),xy_bl.at(1));
    xlow_cell = projected_bl.at(0);
    ylow_cell = projected_bl.at(1);

    std::vector<float> xy_ur = fromXYtoEtaPhi(xup_cell,yup_cell,cell_z);
    std::vector<float> projected_ur = projectXYCoordinates(xup_cell,yup_cell,cell_z,xy_ur.at(0),xy_ur.at(1));
    xup_cell = projected_ur.at(0);
    yup_cell = projected_ur.at(1);

    cell_dx = std::fabs(xup_cell-xlow_cell);
    cell_dy = std::fabs(yup_cell-ylow_cell);

    // in these cases the overlap has to be zero
    if(xlow_cell+cell_dx<xlow_seed) return 0.0;
    if(xlow_seed+seed_dx<xlow_cell) return 0.0;
    if(ylow_cell+cell_dy<ylow_seed) return 0.0;
    if(ylow_seed+seed_dy<ylow_cell) return 0.0;

    float xlow_overlap = std::max(xlow_seed,xlow_cell);
    float xup_overlap = std::min(xup_seed,xup_cell);
    float ylow_overlap = std::max(ylow_seed,ylow_cell);
    float yup_overlap = std::min(yup_seed,yup_cell);

    float areaOverlap = std::fabs((xup_overlap-xlow_overlap)*(yup_overlap-ylow_overlap));
    float areaCell = cell_dx*cell_dy;
    float overlapPercentage = 0;
    if(areaCell!=0) overlapPercentage = areaOverlap/areaCell;

    return overlapPercentage;

}

// Project coordinates onto FCAL 1 face
std::vector<float> projectXYCoordinates(float x,float y,float z,float eta,float phi){
  float fcal1_z = 0;
  if(z>0) fcal1_z = 4935.6;
  else fcal1_z = - 4935.6;
  float theta = 2*std::atan(std::exp(-1*eta));

  float diff_z = z-fcal1_z;
  float deltaR = std::tan(theta)*diff_z;
  float deltaX = deltaR*cos(phi);
  float deltaY = deltaR*sin(phi);

  std::vector<float> projected;
  projected.push_back(x-deltaX);
  projected.push_back(y-deltaY);

  return projected;
}
