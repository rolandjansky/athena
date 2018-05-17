/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ChargeTrappingTool.cxx
 * @author Peter Vankov (peter.vankov@cern.ch)
 * @author Marco Filipuzzi (marco.filipuzzi@cern.ch)
 **/

#include "SCT_ChargeTrappingTool.h"

#include <algorithm>
#include <cmath>

#include "CLHEP/Random/RandFlat.h"

#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h"

#include "Identifier/IdentifierHash.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "SiPropertiesSvc/SiliconProperties.h"

SCT_ChargeTrappingTool::SCT_ChargeTrappingTool(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent),
  m_detManager{nullptr},
  m_isSCT{true},
  m_conditionsToolValid{false},
  m_conditionsToolWarning{false},
  m_PotentialValue{{0.}}
{
  
  declareProperty("DetectorName", m_detectorName="SCT",  "Detector name");
  // Temperature and voltages from job options only used if  SiConditionsServices is None or
  // if value read from database is out of range.
  declareProperty("Temperature",m_temperature = -2., "Default temperature in Celcius.");
  declareProperty("TemperatureMin",m_temperatureMin = -80.,  "Minimum temperature allowed in Celcius.");
  declareProperty("TemperatureMax",m_temperatureMax = 100.,  "Maximum temperature allowed in Celcius.");
  declareProperty("BiasVoltage", m_biasVoltage = 150., "Default  bias voltage in Volt.");
  declareProperty("DepletionVoltage", m_deplVoltage = -30.,  "Default depletion voltage in Volt.");
  
  // -- Radiation damage specific
  declareProperty("CalcHoles", m_calcHoles=true, "Default is to consider holes in signal formation.");
  // -- Fluence: Need to make it layer-dependent
  declareProperty("Fluence", m_fluence=3.0E13, "Fluence received by the detector.");
  declareProperty("BetaElectrons",m_betaElectrons=3.1E-16,"Constant for the trapping model for electrons, in [cm^2/ns] "
                  "-- average value from Table 2 in ATL-INDET-2003-014");
  declareProperty("BetaHoles",m_betaHoles=(double)5.1E-16,"Constant for the trapping model for holes in [cm^2/ns] "
                  "-- average value from Table 2 in ATL-INDET-2003-014");
}

StatusCode 
SCT_ChargeTrappingTool::initialize()
{
  if (m_detectorName!="SCT") {
    ATH_MSG_FATAL("Invalid detector name: " << m_detectorName  << ". Must be SCT.");
    return StatusCode::FAILURE;
  }
  m_isSCT = (m_detectorName=="SCT");
  
  // Get conditions summary tool
  m_conditionsToolValid = false;
  if (not m_siConditionsTool.empty()) {
    ATH_CHECK(m_siConditionsTool.retrieve());
    m_conditionsToolValid = true;
  } else {
    m_siConditionsTool.disable();
    m_conditionsToolWarning = true;
  }
  
  // Get the detector manager
  ATH_CHECK(detStore()->retrieve(m_detManager, m_detectorName));
  
  return StatusCode::SUCCESS;
}

StatusCode SCT_ChargeTrappingTool::finalize()
{
  return StatusCode::SUCCESS;
}

SCT_ChargeTrappingCondData SCT_ChargeTrappingTool::getCondData(const IdentifierHash& elementHash, const double& pos) const
{
  return calculate(elementHash, pos);
}

void SCT_ChargeTrappingTool::getHoleTransport(double& x0, double& y0, double& xfin, double& yfin, double& Q_m2, double& Q_m1, double& Q_00, double& Q_p1, double& Q_p2) const
{
  holeTransport(x0, y0, xfin, yfin, Q_m2, Q_m1, Q_00, Q_p1, Q_p2);
}

void SCT_ChargeTrappingTool::getInitPotentialValue()
{
  initPotentialValue();
}

SCT_ChargeTrappingCondData SCT_ChargeTrappingTool::calculate(const IdentifierHash& elementHash, const double& pos) const
{
  ATH_MSG_VERBOSE("Updating cache  for elementHash = " << elementHash);
  
  if (m_conditionsToolWarning) {
    // Only print the warning once.
    m_conditionsToolWarning = false;
    ATH_MSG_WARNING("Conditions Summary Tool is not used. Will use temperature and voltages from job options. "
                    << "Effects of radiation damage may be wrong!");
  }
  
  SCT_ChargeTrappingCondData condData;

  const InDetDD::SiDetectorElement* element{m_detManager->getDetectorElement(elementHash)};
  
  double temperature{0.};
  double deplVoltage{0.};
  double biasVoltage{0.};
  if (not m_conditionsToolValid) {
    temperature = m_temperature + Gaudi::Units::STP_Temperature;
    deplVoltage = m_deplVoltage * Gaudi::Units::volt;
    biasVoltage = m_biasVoltage * Gaudi::Units::volt;
  } else {
    temperature = m_siConditionsTool->temperature(elementHash) + Gaudi::Units::STP_Temperature;
    deplVoltage = m_siConditionsTool->depletionVoltage(elementHash) * Gaudi::Units::volt;
    biasVoltage = m_siConditionsTool->biasVoltage(elementHash) * Gaudi::Units::volt;
  }
  double totalFluence{m_fluence};
  double betaElectrons{m_betaElectrons};
  double betaHoles{m_betaHoles};
  
  // Protect against invalid temperature
  double temperatureC{temperature - Gaudi::Units::STP_Temperature};
  if (not (temperatureC > m_temperatureMin and temperatureC <  m_temperatureMax)) {
    ATH_MSG_WARNING("Invalid  temperature: " << temperatureC << " C. "
                    << "Setting to "  << m_temperature << " C.");
    temperature = m_temperature + Gaudi::Units::STP_Temperature;
  }
  
  // Calculate depletion depth. If biasVoltage is less than  depletionVoltage
  // the detector is not fully depleted and we need to take this  into account.
  // We take absolute values just in case voltages are signed .
  double depletionDepth{element->thickness()};
  if (std::abs(biasVoltage) < std::abs(deplVoltage)) {
    depletionDepth *= sqrt(std::abs(biasVoltage / deplVoltage));
    // -- if this was the case would need to re-calculate the Ramo Potential and other parameters.
  }
  
  double electricField{m_electricFieldTool->getElectricField(pos,//posZ
                                                             totalFluence,
                                                             deplVoltage,
                                                             element->thickness(),
                                                             std::abs(biasVoltage))};
  //electric field will be a function of bias voltage and fluence...

  condData.setElectricField(electricField);

  InDet::SiliconProperties siProperties;
  siProperties.setConditions(temperature, electricField);

  // -- Calculate electron and holes drift mobility and velocity for these conditions (temperature, electricField)
  //    using parametrizations in SiliconProperties (SiPropertiesSvc). These will be used later for the trapping model.
  //    In the SCT we collect holes. 
  double electronDriftMobility{0.};
  double holeDriftMobility{0.};
  double electronDriftVelocity{0.};
  double holeDriftVelocity{0.};
  if(element->carrierType()==InDetDD::electrons) {
    //    electronDriftMobility = siProperties.calcElectronDriftMobility(temperature,electricField);
    //    electronDriftVelocity = electronDriftMobility*electricField;
  } else {
    if (m_calcHoles){
      holeDriftMobility = siProperties.calcHoleDriftMobility(temperature,electricField*Gaudi::Units::volt)*Gaudi::Units::volt; //in this way you could put the electric field in V/mm and the mobility will be in [V mm^2 ns^-1]
      condData.setHoleDriftMobility(holeDriftMobility);
      holeDriftVelocity = holeDriftMobility*electricField;
    }
  }
  
  // -- Calculate Trapping Times
  double trappingElectrons{1./(totalFluence*betaElectrons)};
  condData.setTrappingElectrons(trappingElectrons);

  double trappingHoles{0.};
  if (m_calcHoles) {
    trappingHoles = 1./(totalFluence*betaHoles);
    condData.setTrappingHoles(trappingHoles);
  }

  // -- Calculate Mean Free Path
  double meanFreePathElectrons{electronDriftVelocity*trappingElectrons};
  condData.setMeanFreePathElectrons(meanFreePathElectrons);

  double meanFreePathHoles{0.};
  if (m_calcHoles) {
    meanFreePathHoles = holeDriftVelocity*trappingHoles;
    condData.setMeanFreePathHoles(meanFreePathHoles);
  }
  
  // -- Trapping probability
  double trappingProbability_electron{0.0};
  double trappingProbability_hole{0.0};
  double trappingProbability{0.0};
  double zpos{pos};
  if (element->carrierType()==InDetDD::electrons) {
    trappingProbability = 1.0 - std::exp(-std::abs(zpos/meanFreePathElectrons));
    trappingProbability_electron = trappingProbability;
  } else {
    if (m_calcHoles){
      trappingProbability = 1.0 - std::exp(-std::abs(zpos/meanFreePathHoles));
      trappingProbability_hole = trappingProbability;
    } else {
      trappingProbability = 0.0;
    }
  }
  condData.setTrappingProbability(trappingProbability);

  // -- Drift time without being trapped
  double u{CLHEP::RandFlat::shoot(0., 1.)};
  double drift_time{-std::log(u)*trappingHoles};
  condData.setTrappingTime(drift_time);
  
  // -- Time to arrive to the electrode
  double t_electrode_hole{zpos/holeDriftVelocity};
  condData.setTimeToElectrode(t_electrode_hole);

  // -- Position at which the trapping happened
  double trappingPosition_hole{holeDriftVelocity*drift_time};
  condData.setTrappingPositionZ(trappingPosition_hole);

  //-------------------

  ATH_MSG_VERBOSE("Temperature (C), bias voltage, depletion  voltage: "
                  << temperature - Gaudi::Units::STP_Temperature << ", "
                  << biasVoltage/Gaudi::Units::volt << ", "
                  << deplVoltage/Gaudi::Units::volt);
  ATH_MSG_VERBOSE("Depletion depth: " << depletionDepth/Gaudi::Units::mm);
  ATH_MSG_VERBOSE("Electric Field: " << electricField/(Gaudi::Units::volt/Gaudi::Units::mm));
  ATH_MSG_VERBOSE("Electron drift mobility (cm2/V/s): " <<  electronDriftMobility/(Gaudi::Units::cm2/Gaudi::Units::volt/Gaudi::Units::s));
  ATH_MSG_VERBOSE("Electron drift velocity (cm/s): " <<   electronDriftVelocity);
  ATH_MSG_VERBOSE("Electron mean free path (cm): " <<  condData.getMeanFreePathElectrons());
  ATH_MSG_VERBOSE("Electron trapping probability: " <<  trappingProbability_electron);

  if (m_calcHoles) {
    ATH_MSG_VERBOSE("Hole drift mobility (cm2/V/s): " <<  holeDriftMobility/(Gaudi::Units::cm2/Gaudi::Units::volt/Gaudi::Units::s));
    ATH_MSG_VERBOSE("Hole drift velocity (cm/s): " <<   holeDriftVelocity);
    ATH_MSG_VERBOSE("Hole mean free path (cm): " <<  condData.getMeanFreePathHoles());
    ATH_MSG_VERBOSE("Hole trapping probability: " <<  trappingProbability_hole);
  }

  return condData;
}



//-------------------------------------------------------------------------------------------------------------------
//    RAMO POTENTIAL 
//-------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------
//   initialize PotentialValue
//--------------------------------------------------------------
void SCT_ChargeTrappingTool::initPotentialValue() {
  for (int ix{0}; ix <81; ix++) {
    for (int iy{0}; iy<115; iy++) {
      m_PotentialValue[ix][iy] = getPotentialValue(ix, iy);
    }
  }
}


//-------------------------------------------------------------------
//    calculation of induced charge using Weighting (Ramo) function
//-------------------------------------------------------------------
double SCT_ChargeTrappingTool::induced (const int istrip, const double x, const double y) const {
  // x and y are the coorlocation of charge (e or hole)
  // induced chardege on the strip "istrip" situated at the height y = d
  // the center of the strip (istrip=0) is x = 0.004 [cm]
  double deltax{0.0005};
  double deltay{0.00025};
  
  double bulk_depth{0.0285}; // in [cm]
  double strip_pitch{0.0080}; // in [cm]
  // x is width, y is depth

  if ((y < 0.) or (y > bulk_depth)) return 0.;
  double xc{strip_pitch * (istrip + 0.5)};
  double dx{std::abs(x-xc)};
  int ix{static_cast<int>(dx/deltax)};
  if (ix > 79) return 0.;
  int iy{static_cast<int>(y/deltay)};
  double fx{(dx - ix*deltax) / deltax};
  double fy{(y - iy*deltay) / deltay};
  int ix1{ix + 1};
  int iy1{iy + 1};
  double P{m_PotentialValue[ix ][iy ] * (1.-fx) * (1.-fy)
         + m_PotentialValue[ix1][iy ] *     fx  * (1.-fy)
         + m_PotentialValue[ix ][iy1] * (1.-fx) *     fy
         + m_PotentialValue[ix1][iy1] *     fx  *     fy};
  ATH_MSG_DEBUG("induced: x,y,iy="<<x<<" "<<y<<" "<<iy<<" istrip,xc,dx,ix="
                <<istrip<<" "<<xc<<" " <<dx<<" "<<ix<<" fx,fy="<<fx <<" " <<fy<< ", P="<<P);
  
  return P;
}


//---------------------------------------------------------------------
//  holeTransport
//---------------------------------------------------------------------
void SCT_ChargeTrappingTool::holeTransport(double& x0, double& y0, double& xfin, double& yfin, double& Q_m2, double& Q_m1, double& Q_00, double& Q_p1, double& Q_p2) const {
  // transport holes in the bulk 
  // T. Kondo, 2010.9.9
  // External parameters to be specified
  //     m_transportTimeMax  [nsec]
  //     m_transportTimeStep [nsec]     
  //     bulk_depth        [cm]
  // Induced currents are added to
  //     Q_m2,Q_m1,Q_00,Q_p1,Q_p2   
  // 
  // initPotentialValue(); // <-this has to go into the main

  // x is width, y is depth

  double x{x0/10.};  // original hole position [cm]
  double y{y0/10.};  // original hole position [cm]
  double qstrip[5];

  for (int istrip{-2}; istrip < 3 ; istrip++) {
    qstrip[istrip+2] = induced(istrip, x, y);
  }
  ATH_MSG_DEBUG("h:qstrip=" << qstrip[0] << " " << qstrip[1] << " " << qstrip[2] << " " << qstrip[3] << " " << qstrip[4]);
  
  // Get induced current by subtracting induced charges
  for (int istrip{-2}; istrip < 3 ; istrip++) {
    x = xfin/10.;
    y = yfin/10.;
    double qnew{induced(istrip, x, y)};
    int jj{istrip + 2};
    double dq{qnew - qstrip[jj]};
    qstrip[jj] = qnew ;
    ATH_MSG_DEBUG("dq= " << dq);
    switch(istrip) {
    case -2: Q_m2 += dq ; break;
    case -1: Q_m1 += dq ; break;
    case  0: Q_00 += dq ; break;
    case +1: Q_p1 += dq ; break;
    case +2: Q_p2 += dq ; break;
    default: break;
    }
  }  
  ATH_MSG_DEBUG("h:qstrip=" << qstrip[0] << " " << qstrip[1] << " " << qstrip[2] << " " << qstrip[3] << " " << qstrip[4]);
  // end of hole tracing 
  return;
}
