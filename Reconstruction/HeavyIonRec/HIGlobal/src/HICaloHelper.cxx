/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIGlobal/HICaloHelper.h"
#include "CaloEvent/CaloCell.h"

#include <iomanip>
#include <sstream>
#include <cmath>
#include <TMath.h>

unsigned int HICaloHelper::NumEtaBins=100;
float HICaloHelper::EtaMin=-5.;
float HICaloHelper::EtaMax=5.;
unsigned int HICaloHelper::NumPhiBins=64;
float HICaloHelper::PhiMin=-TMath::Pi();
float HICaloHelper::PhiMax=TMath::Pi();

float HICaloHelper::GetRho(const xAOD::HIEventShapeContainer* shape, float eta, unsigned int sampling)
{
  float ETsum=(*shape)[FindEtaLayerBin(eta,sampling)]->Et();
  float area=(*shape)[FindEtaLayerBin(eta,sampling)]->area();
  if(area==0.) return 0.;
  return ETsum/area;
}

//float HICaloHelper::GetHarmonicModulation(const xAOD::HIEventShapeContainer* shape, unsigned int nn, float eta, float phi, unsigned int sampling)
float HICaloHelper::GetHarmonicModulation(const xAOD::HIEventShapeContainer*, unsigned int, float, float, unsigned int)
{
  //return 2.*GetVN(shape,nn,eta,sampling)*std::cos(nn*(phi-GetPsiN(shape,nn,eta,sampling)));
  return 1.;
}


float HICaloHelper::GetAreaEtaPhi(const CaloCell* theCell)
{
  float deta=theCell->caloDDE()->deta();
  float dphi=theCell->caloDDE()->dphi();
  return std::abs(deta*dphi);
}

float HICaloHelper::GetEtDensity(const CaloCell* theCell, float geoWeight)
{
  float density=theCell->et();
  float area=HICaloHelper::GetAreaEtaPhi(theCell);
  if(area<1e-9) area=0.2*TMath::Pi()/geoWeight;
  density/=area;
  return density;
}

std::string HICaloHelper::DumpCell(const CaloCell* theCell) 
{
  std::stringstream ss;
  ss << "CELLINFO:" 
     << std::setw(10) << (CaloSampling::CaloSample) theCell->caloDDE()->getSampling()
     << std::setw(20) << theCell->ID().get_compact()
     << std::setw(10) << theCell->et()
     << std::setw(10) << theCell->caloDDE()->eta()
     << std::setw(10) << theCell->caloDDE()->phi()
     << std::setw(15) << theCell->caloDDE()->deta()
     << std::setw(15) << theCell->caloDDE()->dphi()
     << std::setw(15) << HICaloHelper::GetAreaEtaPhi(theCell);
  return ss.str();
}



void HICaloHelper::UpdateShape(xAOD::HIEventShapeContainer* shape, const CaloCell* theCell, float geoWeight, float eta0, float phi0, bool isNeg)
{
  float sgn=(isNeg) ? -1 : 1;

  int layer = theCell->caloDDE()->getSampling();
  float cell_et = theCell->et();

  //FIX better lookup
  int eta_bin = HICaloHelper::FindBinEta(eta0); 
  int index = eta_bin*24 + layer;

  //use this to check?
  //int sh_layer = (shape->at(index))->layer();

  xAOD::HIEventShape* slice=shape->at(index);
  //update members
  slice->setNcells(slice->nCells()+sgn);
  slice->setEt(slice->Et()+sgn*cell_et*geoWeight);
  float area=HICaloHelper::GetAreaEtaPhi(theCell);
  float rho=0;
  if(area!=0.) rho=cell_et/area;
  slice->setArea(slice->area() + sgn*area);
  slice->setRho(slice->rho() + sgn*rho);

  //FIX remove hard coded value, implement harmonics
  int N_ORDERS = 4;
  for(int i=0; i<N_ORDERS; i++)
  {
    float tmp_cos = shape->at(index)->Et_cos().at(i);
    shape->at(index)->Et_cos()[i] = tmp_cos + cell_et*cos(i*phi0)*geoWeight;
    
    float tmp_sin = shape->at(index)->Et_sin().at(i);
    shape->at(index)->Et_sin()[i] = tmp_sin + cell_et*sin(i*phi0)*geoWeight;
  }
  
}

