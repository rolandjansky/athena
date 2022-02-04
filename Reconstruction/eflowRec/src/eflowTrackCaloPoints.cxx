/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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

#include "eflowRec/eflowTrackCaloPoints.h"

using std::pair;

const pair<float, float> eflowTrackCaloPoints::m_defaultEtaPhiPair = pair<float, float>(-999.0, -999.0);
const Amg::Vector3D eflowTrackCaloPoints::m_nullVector = Amg::Vector3D(0.0, 0.0, 0.0);
const eflowEtaPhiPosition eflowTrackCaloPoints::m_defaultEtaPhiPosition = eflowEtaPhiPosition(-999.,-999.);

double eflowTrackCaloPoints::defaultEta()  {return (double)m_defaultEtaPhiPair.first;}
double eflowTrackCaloPoints::defaultPhi()  {return (double)m_defaultEtaPhiPair.second;}

eflowTrackCaloPoints::eflowTrackCaloPoints(const std::map<eflowCalo::LAYER, const Trk::TrackParameters*> & trackParameters):
  m_isEM1Barrel(trackParameters.begin()->first == eflowCalo::EMB1) {

  /* Fill etaPhiPositions map */
  std::map<eflowCalo::LAYER, const Trk::TrackParameters*>::const_iterator itPars = trackParameters.begin();
  std::map<eflowCalo::LAYER, const Trk::TrackParameters*>::const_iterator endPars = trackParameters.end();
  m_isEM2Barrel = false;
  for (; itPars != endPars; ++itPars) {
    setEtaPhi(itPars->first, parToPosition(itPars->second));
    if (itPars->first == eflowCalo::EMB2) m_isEM2Barrel = true;
    m_positions[itPars->first] = parToPosition(itPars->second);
    m_directions[itPars->first] = parToDirection(itPars->second);
  }

}

eflowTrackCaloPoints::~eflowTrackCaloPoints() {

}

void eflowTrackCaloPoints::setEtaPhi(eflowCalo::LAYER lay, const Amg::Vector3D& vec) {
  m_etaPhiPositions[lay] = (vec != m_nullVector) ? eflowEtaPhiPosition(vec.eta(), vec.phi())
                                                 : m_defaultEtaPhiPosition;
}

//only used in eflowTrackExtrapolatorTool
void eflowTrackCaloPoints::setEtaPhi(eflowCaloENUM layer, double eta, double phi) {
  m_etaPhiPositions[layer] = eflowEtaPhiPosition(eta, phi);
}

const pair<float, float> eflowTrackCaloPoints::operator[] (eflowCaloENUM layer) const {
  eflowEtaPhiPosition thisPosition = getEtaPhiPos(layer);
  return pair<float, float>(thisPosition.getEta(), thisPosition.getPhiD());
}
const eflowEtaPhiPosition& eflowTrackCaloPoints::getEtaPhiPos(eflowCalo::LAYER layer) const {
  std::map< eflowCalo::LAYER, eflowEtaPhiPosition>::const_iterator it = m_etaPhiPositions.find(layer);
  return (it == m_etaPhiPositions.end()) ? m_defaultEtaPhiPosition : it->second;
}

Amg::Vector3D eflowTrackCaloPoints::getPosition(eflowCalo::LAYER layer) {
  std::map<eflowCalo::LAYER, Amg::Vector3D>::const_iterator it = m_positions.find(layer);
  return (it == m_positions.end()) ? m_nullVector : it->second;
}

Amg::Vector3D eflowTrackCaloPoints::getDirection(eflowCalo::LAYER layer) {
  std::map<eflowCalo::LAYER, Amg::Vector3D>::const_iterator it = m_directions.find(layer);
  return (it == m_directions.end()) ? m_nullVector : it->second;
}

Amg::Vector3D eflowTrackCaloPoints::parToPosition(const Trk::TrackParameters* trackPars) {
  return (trackPars != nullptr) ? trackPars->position() : m_nullVector;
}

Amg::Vector3D eflowTrackCaloPoints::parToDirection(const Trk::TrackParameters* trackPars) {
  return (trackPars != nullptr) ? trackPars->momentum() : m_nullVector;
}
