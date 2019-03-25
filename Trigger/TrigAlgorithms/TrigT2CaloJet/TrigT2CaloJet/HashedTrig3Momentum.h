/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT2CALOJET_HASHEDTRIG3MOMENTUM_H 
#define TRIGT2CALOJET_HASHEDTRIG3MOMENTUM_H

/*! 
  @brief Trig3Momentum with a hash; allows to merge energies from several Trig3Momentum.

  Eta and phi positions are computed as a weighted average of the
  contributions from different locations in (eta,phi), where the
  weight is abs(energy).

  April 2012
  davide.gerbaudo@gmail.com
 */

#include "TrigCaloEvent/Trig3Momentum.h"

typedef unsigned short u_short;
typedef unsigned u_int;

class HashedTrig3Momentum : public Trig3Momentum {
 public:
  HashedTrig3Momentum();
  HashedTrig3Momentum(const Trig3Momentum &t3m,
		      const u_int &hash);
  u_int hash() const {return m_hash;};
  void addE(const double &eta, const double &phi, const double &energy);
  void addE(const Trig3Momentum &t3m);
  void reset();
 private:
  void updateWeightedCoord(const double &eta, const double &phi, const double &energy);
 private:
  u_int m_hash;
};

#endif
