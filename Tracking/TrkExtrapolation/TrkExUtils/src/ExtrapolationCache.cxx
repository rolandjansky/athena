/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ExtrapolationCache.cxx 
///////////////////////////////////////////////////////////////////

#include "TrkExUtils/ExtrapolationCache.h" 
#include "TrkMaterialOnTrack/EnergyLoss.h" 


Trk::ExtrapolationCache::ExtrapolationCache() : 
   m_x0tot(0), m_eloss(nullptr) { }

Trk::ExtrapolationCache::ExtrapolationCache(double x0tot) : 
   m_x0tot(x0tot), m_eloss(nullptr) { }

Trk::ExtrapolationCache::ExtrapolationCache(double x0tot, EnergyLoss* eloss): 
   m_x0tot(x0tot),
   m_eloss(eloss) { }

Trk::ExtrapolationCache::ExtrapolationCache(const Trk::ExtrapolationCache& cache)  
   = default;


