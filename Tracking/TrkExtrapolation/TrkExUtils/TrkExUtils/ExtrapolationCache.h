/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

///////////////////////////////////////////////////////////////////
// ExtrapolationCache.h 
///////////////////////////////////////////////////////////////////

#ifndef TRKEXUTILS_EXTRAPOLATIONCACHE_H
#define TRKEXUTILS_EXTRAPOLATIONCACHE_H

/** Cache for the extrapolator to keep track of the total X0 traversed
  and the total extended energy loss (Eloss (error) ,Ionization (error), Radiation (error)) */     

#include "TrkMaterialOnTrack/EnergyLoss.h" 

namespace Trk {

class Energyloss;

class ExtrapolationCache {

public:  
  // 
  // Constructor
  //
  ExtrapolationCache();
  ExtrapolationCache(double x0tot);
  ExtrapolationCache(double x0tot, EnergyLoss* eloss);

  //! Copy constructor
  ExtrapolationCache(const ExtrapolationCache& cache);
  //! Destructor 
  virtual ~ExtrapolationCache() = default;
  virtual ExtrapolationCache* clone() const;


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
  EnergyLoss* m_eloss;

}; 

inline ExtrapolationCache* ExtrapolationCache::clone() const
{ return new ExtrapolationCache(*this); }


inline double ExtrapolationCache::x0tot() const 
{ return m_x0tot; }
inline const EnergyLoss* ExtrapolationCache::eloss() const 
{ return m_eloss; }

inline void ExtrapolationCache::reset()  
{ m_x0tot = 0.;  m_eloss->update(-m_eloss->meanIoni(),-m_eloss->sigmaIoni(),-m_eloss->meanRad(),-m_eloss->sigmaRad(),false); }

inline void ExtrapolationCache::updateX0(double x0) 
{ m_x0tot += x0; }

inline void ExtrapolationCache::updateEloss(double ioni, double sigi, double rad, double sigr) 
{ m_eloss->update(ioni,sigi,rad,sigr,false); } 

}

#endif
