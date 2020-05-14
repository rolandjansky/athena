/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//----------------------------------------------------------------------
// Implementation file for the data object class for SCT_ChargeTrappingTool
//----------------------------------------------------------------------

#include "SCT_ConditionsData/SCT_ChargeTrappingCondData.h"

//----------------------------------------------------------------------
// Constructor
SCT_ChargeTrappingCondData::SCT_ChargeTrappingCondData():
  m_electricField{0.},
  m_meanFreePathElectrons{0.},
  m_meanFreePathHoles{0.},
  m_trappingElectrons{0.},
  m_trappingHoles{0.},
  m_trappingProbability{0.},
  m_trappingTime{0.},
  m_electrodeTime{0.},
  m_trappingPosition{0.},
  m_holeDriftMobility{0.}
{}

double SCT_ChargeTrappingCondData::getElectricField() const {
  return m_electricField;
}

double SCT_ChargeTrappingCondData::getMeanFreePathElectrons() const {
  return m_meanFreePathElectrons;
}

double SCT_ChargeTrappingCondData::getMeanFreePathHoles() const {
  return m_meanFreePathHoles;
}

double SCT_ChargeTrappingCondData::getTrappingElectrons() const {
  return m_trappingElectrons;
}

double SCT_ChargeTrappingCondData::getTrappingHoles() const {
  return m_trappingHoles;
}

double SCT_ChargeTrappingCondData::getTrappingProbability() const {
  return m_trappingProbability;
}

double SCT_ChargeTrappingCondData::getTrappingTime() const {
  return m_trappingTime;
}

double SCT_ChargeTrappingCondData::getTimeToElectrode() const {
  return m_electrodeTime;
}

double SCT_ChargeTrappingCondData::getTrappingPositionZ() const {
  return m_trappingPosition;
}

double SCT_ChargeTrappingCondData::getHoleDriftMobility() const {
  return m_holeDriftMobility;
}

void SCT_ChargeTrappingCondData::setElectricField(const double electricField) {
  m_electricField = electricField;
}

void SCT_ChargeTrappingCondData::setMeanFreePathElectrons(const double meanFreePathElectrons) {
  m_meanFreePathElectrons = meanFreePathElectrons;
}

void SCT_ChargeTrappingCondData::setMeanFreePathHoles(const double meanFreePathHoles) {
  m_meanFreePathHoles = meanFreePathHoles;
}

void SCT_ChargeTrappingCondData::setTrappingElectrons(const double trappingElectrons) {
  m_trappingElectrons = trappingElectrons;
}

void SCT_ChargeTrappingCondData::setTrappingHoles(const double trappingHoles) {
  m_trappingHoles = trappingHoles;
}

void SCT_ChargeTrappingCondData::setTrappingProbability(const double trappingProbability) {
  m_trappingProbability = trappingProbability;
}

void SCT_ChargeTrappingCondData::setTrappingTime(const double trappingTime) {
  m_trappingTime = trappingTime;
}

void SCT_ChargeTrappingCondData::setTimeToElectrode(const double electrodeTime) {
  m_electrodeTime = electrodeTime;
}

void SCT_ChargeTrappingCondData::setTrappingPositionZ(const double trappingPosition) {
  m_trappingPosition = trappingPosition;
}

void SCT_ChargeTrappingCondData::setHoleDriftMobility(const double holeDriftMobility) {
  m_holeDriftMobility = holeDriftMobility;
}
