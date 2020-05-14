// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * SCT_ChargeTrappingCondData
 * @file header file for data object for SCT_ChargeTrappingTool, SCT_RadDamageSummaryTool, SCT_SurfaceChargesGenerator
 * @author Susumu Oda
 * @date 14/05/2018
 **/

#ifndef SCT_CHARGETRAPPINGCONDDATA_H
#define SCT_CHARGETRAPPINGCONDDATA_H

/**
 * @class SCT_ChargeTrappingCondData
 * @brief Data object for SCT_ChargeTrappingTool, SCT_RadDamageSummaryTool, SCT_SurfaceChargesGenerator
 **/
class SCT_ChargeTrappingCondData
{
public:
  SCT_ChargeTrappingCondData();
  virtual ~SCT_ChargeTrappingCondData() = default;

  double getElectricField() const;
  double getMeanFreePathElectrons() const;
  double getMeanFreePathHoles() const;
  double getTrappingElectrons() const;
  double getTrappingHoles() const;
  double getTrappingProbability() const;
  double getTrappingTime() const;
  double getTimeToElectrode() const;
  double getTrappingPositionZ() const;
  double getHoleDriftMobility() const;

  void setElectricField(const double electricField);
  void setMeanFreePathElectrons(const double meanFreePathElectrons);
  void setMeanFreePathHoles(const double meanFreePathHoles);
  void setTrappingElectrons(const double trappingElectrons);
  void setTrappingHoles(const double trappingHoles);
  void setTrappingProbability(const double trappingProbability);
  void setTrappingTime(const double trappingTime);
  void setTimeToElectrode(const double electrodeTime);
  void setTrappingPositionZ(const double trappingPosition);
  void setHoleDriftMobility(const double holeDriftMobility);

private:
  double m_electricField;
  double m_meanFreePathElectrons;
  double m_meanFreePathHoles;
  double m_trappingElectrons;
  double m_trappingHoles;
  double m_trappingProbability;
  double m_trappingTime;
  double m_electrodeTime;
  double m_trappingPosition;
  double m_holeDriftMobility;
};

// Class definition for StoreGate
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( SCT_ChargeTrappingCondData , 70980446 , 1 )

#endif // SCT_CHARGETRAPPINGCONDDATA_H
