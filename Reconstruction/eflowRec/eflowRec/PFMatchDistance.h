/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*
 * PFMatchDistance.h
 *
 *  Created on: 28.03.2014
 *      Author: tlodd
 */

#ifndef PFMATCHDISTANCE_H_
#define PFMATCHDISTANCE_H_

#include "eflowRec/PFMatchInterfaces.h"

namespace PFMatch {

class EtaPhi;
class EtaPhiWithVariance;

/* Distance calculator classes */

/**
Calculates distance between two EtaPhi objects
*/
class EtaPhiSqDistanceCalculator: public DistanceCalculator<EtaPhi, EtaPhi> {
public:
  EtaPhiSqDistanceCalculator() { }
  virtual ~EtaPhiSqDistanceCalculator() { }

  virtual double distanceBetween(const EtaPhi& position1, const EtaPhi& position2) const;
};

/**
Calculates distance between EtaPhi and EtaPhiWithVariance object
*/
class EtaPhiSqSignificanceCalculator: public DistanceCalculator<EtaPhi, EtaPhiWithVariance> {
public:
  EtaPhiSqSignificanceCalculator() { }
  virtual ~EtaPhiSqSignificanceCalculator() { }

  virtual double distanceBetween(const EtaPhi& position1, const EtaPhiWithVariance& position2) const;
};


/* Distance calculator factory */

/**
Factory to create IDistanceProvider of the type requested.
*/
class DistanceFactory {
public:
  static std::unique_ptr<IDistanceProvider> Get(std::string distanceType, std::unique_ptr<IPositionProvider> trackPosition,
						std::unique_ptr<IPositionProvider> clusterPosition) {
    if (distanceType == "EtaPhiSquareDistance") {
      return std::make_unique<DistanceProvider<EtaPhi, EtaPhi> >(std::move(trackPosition), std::move(clusterPosition), std::make_unique<EtaPhiSqDistanceCalculator>());
    } else if (distanceType == "EtaPhiSquareSignificance") {
      return std::make_unique<DistanceProvider<EtaPhi, EtaPhiWithVariance> >(std::move(trackPosition), std::move(clusterPosition), std::make_unique<EtaPhiSqSignificanceCalculator>());
    } else {
      std::cerr << "DistanceFactory\tERROR\tInvalid distance type: \"" << distanceType << "\"" << std::endl;
      assert(false);
      return 0;
    }
  }
};

}

#endif /* PFMATCHDISTANCE_H_ */
