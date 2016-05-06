/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * PFMatchDistance.cxx
 *
 *  Created on: 28.03.2014
 *      Author: tlodd
 */

#include "eflowRec/PFMatchDistance.h"
#include "eflowRec/PFMatchPositions.h"

namespace PFMatch {

//double EtaPhiSqDistanceCalculator::distanceBetween(IPosition* position1,
//                                                   IPosition* position2) {
//  EtaPhi* etaphi1 = convertPosition<EtaPhi>(position1);
//  EtaPhi* etaphi2 = convertPosition<EtaPhi>(position2);
//
//  return etaphi1->dRSq(*etaphi2);
//}

double EtaPhiSqDistanceCalculator::distanceBetween(EtaPhi* position1, EtaPhi* position2) {
  return position1->dRSq(*position2);
}

double EtaPhiSqSignificanceCalculator::distanceBetween(EtaPhi* position1, EtaPhiWithVariance* position2){
   double dEta = position1->getEta() - position2->getEta();
   double dPhi = position1->getPhi().getAbsDifference(position2->getPhi());
   return dEta*dEta/position2->m_etaVariance + dPhi*dPhi/position2->m_phiVariance;
}
//
//double EtaPhiSqSignificanceCalculator::distanceBetween(IPosition* position1,
//                                                       IPosition* position2) {
//  EtaPhi* etaphi1 = convertPosition<EtaPhi>(position1);
//  EtaPhiWithVariance* etaphi2 = convertPosition<EtaPhiWithVariance>(position2);
//
//  double dEta = etaphi1->getEta() - etaphi2->getEta();
//  double dPhi = etaphi1->getPhi().getAbsDifference(etaphi2->getPhi());
//  return dEta*dEta/etaphi2->_etaVariance + dPhi*dPhi/etaphi2->_phiVariance;
//}

}
