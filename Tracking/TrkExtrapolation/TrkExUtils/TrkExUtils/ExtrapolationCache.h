/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

///////////////////////////////////////////////////////////////////
// ExtrapolationCache.h 
///////////////////////////////////////////////////////////////////

#ifndef TRKEXUTILS_EXTRAPOLATIONCACHE_H
#define TRKEXUTILS_EXTRAPOLATIONCACHE_H

/** 
 * Cache for the extrapolator to keep track of the total X0 traversed
  and the total extended energy loss (Eloss (error) ,Ionization (error),
  Radiation (error)) 
    
*/

#include "TrkMaterialOnTrack/EnergyLoss.h" 

namespace Trk {

class Energyloss;

class ExtrapolationCache {

public:  
  // 
  // Constructor
  //
  ExtrapolationCache()=default;
  ExtrapolationCache(const ExtrapolationCache& other) = default;
  ExtrapolationCache(ExtrapolationCache&& other) = default;
  ExtrapolationCache& operator=(const ExtrapolationCache& other)=default;
  ExtrapolationCache& operator=(ExtrapolationCache&& other) = default;

  ExtrapolationCache(double x0tot);
  ExtrapolationCache(double x0tot, EnergyLoss* eloss);
  ~ExtrapolationCache() = default;
  
  //! Destructor 
  ExtrapolationCache* clone() const;


  // total X0 
  double x0tot() const;
  // total Eloss
  const EnergyLoss* eloss() const;
  // reset to zero
  void reset() ;
  // add X0 value
  void updateX0(double x0) ;
  // add Eloss values
  void updateEloss(double ioni, double sigi, double rad, double sigr);

private:
  double m_x0tot;
  EnergyLoss* m_eloss;//We do not own this ptr

};

}
#include "TrkExUtils/ExtrapolationCache.icc"
#endif
