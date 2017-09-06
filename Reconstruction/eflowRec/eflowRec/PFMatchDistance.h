/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

class EtaPhiSqDistanceCalculator: public DistanceCalculator<EtaPhi, EtaPhi> {
public:
  EtaPhiSqDistanceCalculator() { }
  virtual ~EtaPhiSqDistanceCalculator() { }

  virtual double distanceBetween(EtaPhi* position1, EtaPhi* position2);
};

class EtaPhiSqSignificanceCalculator: public DistanceCalculator<EtaPhi, EtaPhiWithVariance> {
public:
  EtaPhiSqSignificanceCalculator() { }
  virtual ~EtaPhiSqSignificanceCalculator() { }

  virtual double distanceBetween(EtaPhi* position1, EtaPhiWithVariance* position2);
};


/* Distance calculator factory */

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
