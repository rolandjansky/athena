/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkTrackSummary/DetailedHitInfo.h"


Trk::DetailedHitInfo::DetailedHitInfo( const DetailedHitInfo& ph )
    : m_detailedHitInfo(ph.m_detailedHitInfo) {}

 Trk::DetailedHitInfo& Trk::DetailedHitInfo::operator=(const DetailedHitInfo& ph) 
{
  if (&ph !=this){
    m_detailedHitInfo = ph.m_detailedHitInfo;
  }
  return *this;
}

std::vector < std::pair < Trk::DetectorRegion, std::vector < std::tuple <int , int , int> > > > Trk::DetailedHitInfo::getHitInfo()
{
  return m_detailedHitInfo;
}

void Trk::DetailedHitInfo::addHit(Trk::DetectorRegion region, int layer, int eta_module, int hit) 
{
  for (auto& el : m_detailedHitInfo) {
    if (el.first==region) {
      for (auto& count : el.second) {
        if (std::get<0>(count) == layer) {
          // increase the constributing layers only for inclined/ring/endcap modules
          if (region!=pixelBarrelFlat and region!=stripBarrel and eta_module!=m_prevEta) { 
            std::get<1>(count)++;
            m_prevEta=eta_module;  
          }
          std::get<2>(count)+=hit;
          return;
        }
      }
    }
  }
  // if reaches this point the element is not found, then you add it to m_detailedHitInfo  
  m_prevEta=eta_module;
  std::vector < std::tuple <int, int, int> > counts = {{layer, 1, hit}};
  m_detailedHitInfo.push_back(std::make_pair(region, counts));  
  return;
}

int Trk::DetailedHitInfo::getHits(Trk::DetectorRegion region, int layer)
{
  for (auto& el : m_detailedHitInfo) {
    if (el.first==region) {
      for (auto& count : el.second) {
        if (std::get<0>(count) == layer) {
          return std::get<2>(count);
        }
      }
    }
  }
  return 0;  
}

int Trk::DetailedHitInfo::getContributionFromRegion(Trk::DetectorRegion region)
{
  int counts = 0;
  for (auto& el : m_detailedHitInfo) {
    if (el.first==region) {
      for (auto& count : el.second) {
        counts+=std::get<1>(count);
      }
    }
  }  
  return counts;
}

int Trk::DetailedHitInfo::getAllContributions()
{
  int counts = 0;
  for (auto& el : m_detailedHitInfo) {
    for (auto& count : el.second) {
      counts+=std::get<1>(count);
    }
  } 
  return counts;
}

int Trk::DetailedHitInfo::getHitsFromRegion(Trk::DetectorRegion region)
{
  int hits = 0;
  for (auto& el : m_detailedHitInfo) {
    if (el.first==region) {
      for (auto& count : el.second) {
        hits+=std::get<2>(count);
      }
    }
  }
  return hits;
}

int Trk::DetailedHitInfo::getAllHits()
{
  int hits = 0;
  for (auto& el : m_detailedHitInfo) {
    for (auto& count : el.second) {
      hits+=std::get<2>(count);
    }
  }
  return hits;
}

int Trk::DetailedHitInfo::getPixelContributions() {
  int counts = 0;
  for (auto& el : m_detailedHitInfo) {
    if (el.first==stripBarrel or el.first==stripEndcap) {
      continue;
    }
    for (auto& count : el.second) {
      counts+=std::get<1>(count);
    }
  }
  return counts;
}

int Trk::DetailedHitInfo::getPixelHits() {
  int hits = 0;
  for (auto& el : m_detailedHitInfo) {
    if (el.first==stripBarrel or el.first==stripEndcap) {
      continue;
    }
    for (auto& count : el.second) {
      hits+=std::get<2>(count);
    }
  }
  return hits;
}

int Trk::DetailedHitInfo::getStripContributions() {
  int counts = 0;
  for (auto& el : m_detailedHitInfo) {
    if (el.first==pixelBarrelFlat or 
        el.first==pixelBarrelInclined or
        el.first==pixelBarrelRing or
        el.first==pixelEndcap) {
      continue;
    }
    for (auto& count : el.second) {
      counts+=std::get<1>(count);
    }
  }
  return counts;
}

int Trk::DetailedHitInfo::getStripHits() {
  int hits = 0;
  for (auto& el : m_detailedHitInfo) {
    if (el.first==pixelBarrelFlat or 
        el.first==pixelBarrelInclined or
        el.first==pixelBarrelRing or
        el.first==pixelEndcap) {
      continue;
    }
    for (auto& count : el.second) {
      hits+=std::get<2>(count);
    }
  }
  return hits;
}
