/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/********************************************************************

NAME:     eflowTrackCaloPoints.cxx
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  R. Duxfield
CREATED:  4th January, 2006

********************************************************************/

#include <utility>
#include <iostream>
#include <cmath>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "eflowRec/eflowCaloRegions.h"
#include "eflowRec/eflowTrackToCaloTrackExtrapolatorTool.h"
#include "eflowRec/eflowTrackCaloPoints.h"
#include "eflowRec/eflowDepthCalculator.h"
#include "eflowRec/cycle.h"

using std::pair;
using std::cout;
using std::endl;

const pair<float, float> eflowTrackCaloPoints::m_defaultEtaPhiPair = pair<float, float>(-999.0, -999.0);
const Amg::Vector3D eflowTrackCaloPoints::m_nullVector = Amg::Vector3D(0.0, 0.0, 0.0);
const eflowEtaPhiPosition eflowTrackCaloPoints::m_defaultEtaPhiPosition = eflowEtaPhiPosition(-999.,-999.);

double eflowTrackCaloPoints::defaultEta()  {return (double)m_defaultEtaPhiPair.first;}
double eflowTrackCaloPoints::defaultPhi()  {return (double)m_defaultEtaPhiPair.second;}

eflowTrackCaloPoints::eflowTrackCaloPoints(const eflowTrackCaloPoints& other) :
    m_isInBarrel(other.m_isInBarrel), m_etaPhiPositions(other.m_etaPhiPositions) { }

eflowTrackCaloPoints::eflowTrackCaloPoints(const ToolHandle<eflowTrackToCaloTrackExtrapolatorTool>& extrapolator, const xAOD::TrackParticle* track) :
  m_isInBarrel(extrapolator->isInBarrel(track))
{
  double etaTrack = track->eta();
  bool isHcalBarrel = (m_isInBarrel || fabs(etaTrack) < 1.8);
  bool isHcalEndcap = (fabs(etaTrack) > 1.35 && etaTrack != m_defaultEtaPhiPair.first);

  CaloCell_ID::CaloSample myEMBSamples[] = {CaloCell_ID::EMB1, CaloCell_ID::EMB2, CaloCell_ID::EMB3};
  CaloCell_ID::CaloSample myEMESamples[] = {CaloCell_ID::EME1, CaloCell_ID::EME2, CaloCell_ID::EME3};
  std::vector<CaloCell_ID::CaloSample> allSamples;
  unsigned int nEMSamples = 3;
  for (unsigned int i = 0; i < nEMSamples; ++i){
    allSamples.push_back(m_isInBarrel ? myEMBSamples[i] : myEMESamples[i]);
  }
  if (isHcalEndcap) {
    CaloCell_ID::CaloSample myHcalSamples[] = {CaloCell_ID::HEC0, CaloCell_ID::HEC1, CaloCell_ID::HEC2, CaloCell_ID::HEC3, CaloCell_ID::Unknown};
    for (CaloCell_ID::CaloSample* thisSample = myHcalSamples; *thisSample != CaloCell_ID::Unknown; ++thisSample){
      allSamples.push_back(*thisSample);
    }
  }

  std::vector<Amg::Vector3D > vec = extrapolator->execute(track, allSamples);
  std::vector<Amg::Vector3D >& vecEM = vec;

  const int firstLayer = (m_isInBarrel) ? eflowCalo::EMB1 : eflowCalo::EME1;
  for (int i = 0; i < 3; i++) {
    setEtaPhi((eflowCaloENUM)(firstLayer + i), vecEM[i]);
  }

  fixFailedEmExtrapolations((eflowCaloENUM)firstLayer);

  if (isHcalEndcap) {
    std::vector<Amg::Vector3D > vecHcal(vec.begin()+3, vec.end());
    for (int i = 0; i < 4; i++) {
        setEtaPhi((eflowCaloENUM)(eflowCalo::HEC1 + i), vecHcal[i]);
    }

    /* TODO: Get rid of this kludge */
    if (vecHcal[1] != m_nullVector && vecHcal[2] != m_nullVector) {
      vecHcal[3] = 2.0 * vecHcal[2] - vecHcal[1];
      setEtaPhi(eflowCalo::HEC4, vecHcal[3]);
    }
  }
  if (isHcalBarrel) {
    std::pair<Amg::Vector3D, Amg::Vector3D >
    lastPointAndDirection = setLastPointAndDirection(vecEM);
    doHcalBarrelLinearExtrapolations(lastPointAndDirection.first, lastPointAndDirection.second);
  }
}

void eflowTrackCaloPoints::doHcalBarrelLinearExtrapolations(const Amg::Vector3D& lastPoint, const Amg::Vector3D& direction) {
  if (lastPoint == m_nullVector || direction == m_nullVector) return;
  const double dR = sqrt(direction.x() * direction.x() + direction.y() * direction.y());
  const double lastR = sqrt(lastPoint.x() * lastPoint.x() + lastPoint.y() * lastPoint.y());
  for (int i = 0; i < 3; i++) {
    double r = (eflowDepthCalculator::m_tileBarLayerR[i] + eflowDepthCalculator::m_tileBarLayerR[i + 1]) / 2.0;
    Amg::Vector3D vec = lastPoint + direction * ((r - lastR) / dR);
    double z = fabs(vec.z());
    if (z < eflowDepthCalculator::m_tileBarZmin || z > eflowDepthCalculator::m_tileBarZmax) {
      bool found = false;
      if (1 == i) {
        r = (eflowDepthCalculator::m_itc1Rmin + eflowDepthCalculator::m_itc1Rmax) / 2.0;
        vec = lastPoint + direction * ((r - lastR) / dR);
        z = fabs(vec.z());
        found = (z > eflowDepthCalculator::m_itc1Zmin && z < eflowDepthCalculator::m_itc1Zmax);
      }
      else
      if (2 == i) {
        r = (eflowDepthCalculator::m_itc2Rmin + eflowDepthCalculator::m_itc2Rmax) / 2.0;
        vec = lastPoint + direction * ((r - lastR) / dR);
        z = fabs(vec.z());
        found = (z > eflowDepthCalculator::m_itc2Zmin || z < eflowDepthCalculator::m_itc2Zmax);
      }

      if (!found) {
        r = (eflowDepthCalculator::m_tileExtLayerR[i] + eflowDepthCalculator::m_tileExtLayerR[i + 1]) / 2.0;
        vec = lastPoint + direction * ((r - lastR) / dR);
      }
    }
    setEtaPhi((eflowCaloENUM)(eflowCalo::Tile1 + i), vec);
  }
}

std::pair<Amg::Vector3D, Amg::Vector3D >
eflowTrackCaloPoints::setLastPointAndDirection(const std::vector<Amg::Vector3D >& vecEM) {
  Amg::Vector3D lastPoint(m_nullVector);
  Amg::Vector3D direction(m_nullVector);

  int nPairs = 0;
  for (int i = 0; i < 3; i++) {
    if (vecEM[i] != m_nullVector)  nPairs++;
  }
  if (3 == nPairs) {
    // 3rd layer sometimes appears to be 'inside' second layer near crack region. small effect, but ruins extrapolation to tile -->
    direction = (fabs(vecEM[2].z()) > fabs(vecEM[1].z())) ? vecEM[2] - vecEM[1] : vecEM[2] - vecEM[0];
    lastPoint = vecEM[2];
  } else if (2 == nPairs) {
    if (vecEM[2] != m_nullVector) {
      lastPoint = vecEM[2];
      direction = vecEM[2];
      if (vecEM[1] != m_nullVector && fabs(vecEM[2].z()) > fabs(vecEM[1].z())) {
        direction -= vecEM[1];
      } else if (fabs(vecEM[2].z()) > fabs(vecEM[0].z())) {
        direction -= vecEM[0];
      }
    } else {
      direction = vecEM[1] - vecEM[0];
      lastPoint = vecEM[1];
    }
  } else if (1 == nPairs) {
    for (int i = 0; i < 3; i++) {
      if (vecEM[i] != m_nullVector) {
        lastPoint = vecEM[i];
        direction = vecEM[i];
        break;
      }
    }
  }

  return std::pair<Amg::Vector3D, Amg::Vector3D >(lastPoint, direction);
}

void eflowTrackCaloPoints::fixFailedEmExtrapolations(const eflowCaloENUM firstLayer) {
  eflowCaloENUM secondLayer = (eflowCaloENUM)(firstLayer+1);
  eflowCaloENUM thirdLayer = (eflowCaloENUM)(secondLayer+1);
  bool haveFirstLayer(haveLayer(firstLayer));
  bool haveSecondLayer(haveLayer(secondLayer));
  bool haveThirdLayer(haveLayer(thirdLayer));

  if ( !haveFirstLayer ) {
    if ( haveSecondLayer ) {
      copyEtaPhi(secondLayer, firstLayer);
    } else {
      copyEtaPhi(thirdLayer, firstLayer);
    }
  }

  if ( !haveSecondLayer ) {
    if ( !haveFirstLayer ) {
      copyEtaPhi(thirdLayer, secondLayer);
    } else if ( !haveThirdLayer ) {
      copyEtaPhi(firstLayer, secondLayer);
    } else {
      pair<float, float> firstLayerEtaPhi((*this)[firstLayer]);
      pair<float, float> thirdLayerEtaPhi((*this)[thirdLayer]);
      double eta = ( firstLayerEtaPhi.first + thirdLayerEtaPhi.first ) / 2.0;
      double phi = ( firstLayerEtaPhi.second + cycle(thirdLayerEtaPhi.second, firstLayerEtaPhi.second) ) / 2.0;
      m_etaPhiPositions[ secondLayer ] = eflowEtaPhiPosition(eta, phi);
    }
  }

  if ( !haveThirdLayer ) {
     if (haveSecondLayer) {
      copyEtaPhi(secondLayer, thirdLayer);
    } else {
      copyEtaPhi(firstLayer, thirdLayer);
    }
  }
}

const pair<float, float> eflowTrackCaloPoints::operator[] (eflowCaloENUM lay) const {
  std::map< eflowCalo::LAYER, eflowEtaPhiPosition>::const_iterator it = m_etaPhiPositions.find(lay);
  return (m_etaPhiPositions.end() == it) ? m_defaultEtaPhiPair
                                         : pair<float, float>(it->second.getEta(), it->second.getPhiD());
}
const eflowEtaPhiPosition& eflowTrackCaloPoints::getEtaPhiPos(eflowCalo::LAYER lay) const {
  std::map< eflowCalo::LAYER, eflowEtaPhiPosition>::const_iterator it = m_etaPhiPositions.find(lay);
  return (m_etaPhiPositions.end() == it) ? m_defaultEtaPhiPosition : it->second;
}
