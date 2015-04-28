/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIEventUtils/HICaloRange.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include <TH2.h>

//set true explicitly in initialization even though object may not be initialized
//initial construction of object will ensure correct state
//prevents sensitivity to order of initialization of static memebers
bool HICaloRange::s_init=true; 
HICaloRange HICaloRange::s_range=HICaloRange();


HICaloRange::HICaloRange() 
{
  s_init=false;
  initializeRange_Internal();
  s_init=true;
}

HICaloRange::HICaloRange(const HICaloRange& cr)
{
  m_range=cr.m_range;
}

const HICaloRange& HICaloRange::getRange()
{
  if(!s_init) s_range.initializeRange_Internal();
  return s_range;
}

void HICaloRange::initializeRange(const TH2* h2)
{
  s_range.initializeRange_Internal(h2);
}


void HICaloRange::initializeRange(const xAOD::HIEventShapeContainer* shape)
{
  s_range.initializeRange_Internal(shape);
}

void HICaloRange::initializeRange() 
{
  s_range.initializeRange_Internal();
}



bool HICaloRange::LayerInRange(float eta, int layer)
{
  //check on initialization
  if(!s_init)
  {
    //ATH_MSG_WARNING( "Warning tool used before initialized. initializing on the fly using defaults");
    initializeRange_Internal();
  }
  if(layer >= static_cast<int>(m_range.size()))
  {
    //ATH_MSG_ERROR("Requesting range for layer that does not exist. Layer " << layer << " not in range");
    //throw
  }

  float eta_abs=std::abs(eta);
  return ( (eta_abs > m_range[layer].first) && (eta_abs < m_range[layer].second) );
}

void HICaloRange::initializeRange_Internal(const TH2* h2)
{
  //if(s_init) ATH_MSG_WARNING("Reinitializing");
  if(h2==nullptr)
  {
    //ATH_MSG_ERROR("Could not initialize, bad histogram pointer");
    return;
  }
  s_init=false;

  for(int xbin=1; xbin<=h2->GetNbinsX(); xbin++)
  {
    float eta_low=h2->GetXaxis()->GetBinLowEdge(xbin);
    float eta_up=h2->GetXaxis()->GetBinUpEdge(xbin);
    m_range[xbin-1]=range_t(-1,-1);
    for(int ybin=1; ybin<=h2->GetNbinsY(); ybin++)
    {
      int layer=h2->GetYaxis()->GetBinCenter(ybin);
      float content=h2->GetBinContent(xbin,ybin);
      if(content > 0) updateRange(eta_low,eta_up,layer);
    }
  }
  s_init=true;

}


void HICaloRange::initializeRange_Internal(const xAOD::HIEventShapeContainer* shape)
{
  //if(s_init) ATH_MSG_WARNING( "Reinitializing");
  if(shape==nullptr)
  {
    //ATH_MSG_ERROR("Could not initialize, bad xAOD::HIEventShapeContainer pointer");
    return;
  }
  s_init=false;
  for(const auto itr : *shape) updateRange(itr->etaMin(),itr->etaMax(),itr->layer());
  s_init=true;

}

void HICaloRange::initializeRange_Internal()
{
  
  m_range[CaloSampling::PreSamplerB]=range_t(0,1.6); //0
  m_range[CaloSampling::EMB1]=range_t(0,1.5); //1
  m_range[CaloSampling::EMB2]=range_t(0,1.5); //2
  m_range[CaloSampling::EMB3]=range_t(0,1.4); //3

  m_range[CaloSampling::PreSamplerE]=range_t(1.5,1.8); //4
  m_range[CaloSampling::EME1]=range_t(1.3,2.5); //5
  m_range[CaloSampling::EME2]=range_t(1.3,3.2); //6
  m_range[CaloSampling::EME3]=range_t(1.5,3.2); //7

  m_range[CaloSampling::HEC0]=range_t(1.5,3.3); //8
  m_range[CaloSampling::HEC1]=range_t(1.5,3.1); //9
  m_range[CaloSampling::HEC2]=range_t(1.6,3.1); //10
  m_range[CaloSampling::HEC3]=range_t(1.7,3.3); //11

  m_range[CaloSampling::TileBar0]=range_t(0,1); //12
  m_range[CaloSampling::TileBar1]=range_t(0,0.9); //13
  m_range[CaloSampling::TileBar2]=range_t(0,0.7); //14

  m_range[CaloSampling::TileGap1]=range_t(0.9,1); //15
  m_range[CaloSampling::TileGap2]=range_t(0.8,0.9); //16
  m_range[CaloSampling::TileGap3]=range_t(1,1.6); //17

  m_range[CaloSampling::TileExt0]=range_t(1.1,1.6); //18
  m_range[CaloSampling::TileExt1]=range_t(1,1.5); //19
  m_range[CaloSampling::TileExt2]=range_t(0.9,1.3); //20

  m_range[CaloSampling::FCAL0]=range_t(3,5); //21
  m_range[CaloSampling::FCAL1]=range_t(3.1,5); //22
  m_range[CaloSampling::FCAL2]=range_t(3.2,5); //23

  // for(auto p : m_range)
  // { 
  //   std::cout << p.first << "\t" << p.second.first << "\t" << p.second.second << std::endl;
  // }
  
}

void HICaloRange::updateRange(float eta_low, float eta_up, int layer)
{
  //assume layers are eta symmetric
  //necessary to handle non-contiguous layers
  float eta_low_abs=std::abs(eta_low);
  float eta_up_abs=std::abs(eta_up);
  
  range_t current;
  auto mItr=m_range.find(layer);
  if(mItr==m_range.end())
  {
    //ATH_MSG_ERROR( "Trying to initialize invalid layer. Layer " << layer << " not in range.");
    return;
  }
  if(current.first < 0) mItr->second=range_t(eta_low_abs,eta_up_abs);
  else
  {
    if(current.first > eta_low_abs) mItr->second.first=eta_low_abs;
    if(current.second < eta_low_abs) mItr->second.second=eta_up_abs;
  }
}

 
