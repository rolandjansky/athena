/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "SiLorentzAngleSvc/SiLorentzAngleCHSvc.h"

#include <algorithm>
#include <limits>

#include "GaudiKernel/SystemOfUnits.h"

#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "Identifier/IdentifierHash.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "StoreGate/StoreGate.h"

SiLorentzAngleCHSvc::SiLorentzAngleCHSvc(const std::string& name, ISvcLocator* pSvcLocator) :
  AthService(name, pSvcLocator),
  m_isPixel{false},
  m_condData{"SCTSiLorentzAngleCondData"},
  m_magFieldSvc{"AtlasFieldSvc", name},
  m_detStore{"StoreGateSvc/DetectorStore", name},
  m_detManager{nullptr}
{
  declareProperty("IgnoreLocalPos", m_ignoreLocalPos = false, 
                  "Treat methods that take a local position as if one called the methods without a local position");
  declareProperty("DetStore", m_detStore);
  // IF SCT, YOU NEED TO USE THE SAME PROPERTIES AS USED IN SCTSiLorentzAngleCondAlg!!!
  declareProperty("DetectorName", m_detectorName="Pixel", "Detector name (Pixel or SCT)");
  declareProperty("MagFieldSvc", m_magFieldSvc);
  declareProperty("NominalField", m_nominalField = 2.0834*Gaudi::Units::tesla);
  declareProperty("UseMagFieldSvc", m_useMagFieldSvc = true);
  declareProperty("SiLorentzAngleCondData", m_condData, "Key of input SiLorentzAngleCondData");
}

StatusCode SiLorentzAngleCHSvc::initialize() { 

  ATH_MSG_DEBUG("SiLorentzAngleCHSvc Initialized");

  // Detector store
  CHECK(m_detStore.retrieve());

  if ((m_detectorName not_eq "Pixel") and (m_detectorName not_eq "SCT")) {
    ATH_MSG_FATAL("Invalid detector name: " << m_detectorName << ". Must be Pixel or SCT.");
    return StatusCode::FAILURE;
  }
  m_isPixel = (m_detectorName == "Pixel");

  if (m_isPixel) {
    ATH_MSG_FATAL("SiLorentzAngleCHSvc is not yet implemented for Pixel!");
    return StatusCode::FAILURE;
  }

  // Read Cond Handle
  ATH_CHECK(m_condData.initialize());

  // Get the detector manager
  CHECK(m_detStore->retrieve(m_detManager, m_detectorName));

  // MagneticFieldSvc handles updates itself
  if (not m_useMagFieldSvc) {
    ATH_MSG_DEBUG("Not using MagneticFieldSvc - Will be using Nominal Field!");
  } else if (m_magFieldSvc.retrieve().isFailure()) {
    ATH_MSG_WARNING("Could not retrieve MagneticFieldSvc - Will be using Nominal Field!");
    m_useMagFieldSvc = false;
  }
  
  return StatusCode::SUCCESS;
}

StatusCode SiLorentzAngleCHSvc::finalize() {
  return StatusCode::SUCCESS;
}

// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!
StatusCode SiLorentzAngleCHSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if (ISiLorentzAngleSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface = dynamic_cast<ISiLorentzAngleSvc*>(this);
  } else {
    // Interface is not directly available: try out a base class
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

StatusCode SiLorentzAngleCHSvc::corrFolderCallBack(IOVSVC_CALLBACK_ARGS) {
  ATH_MSG_FATAL("SiLorentzAngleCHSvc::corrFolderCallBack should not be used!!!");
  return StatusCode::FAILURE;
}


StatusCode SiLorentzAngleCHSvc::callBack(IOVSVC_CALLBACK_ARGS) {
  ATH_MSG_FATAL("SiLorentzAngleCHSvc::callBack should not be used!!!");
  return StatusCode::FAILURE;
}


double SiLorentzAngleCHSvc::getLorentzShift(const IdentifierHash& elementHash) {
  const SiLorentzAngleCondData* condData{getCondData()};
  if (condData) {
    return condData->getLorentzShift(elementHash);
  }
  return s_invalidValue;
}

double SiLorentzAngleCHSvc::getLorentzShift(const IdentifierHash& elementHash, const Amg::Vector2D& locPos) {
  if (m_ignoreLocalPos) return getLorentzShift(elementHash);
  // The cache is used to store the results. The cache is therefore invalidated if we specify a position.
  return getValue(elementHash, locPos, LorentzShift);
}

double SiLorentzAngleCHSvc::getLorentzShiftEta(const IdentifierHash& elementHash) {
  const SiLorentzAngleCondData* condData{getCondData()};
  if (condData) {
    return condData->getLorentzShiftEta(elementHash);
  }
  return s_invalidValue;
}

double SiLorentzAngleCHSvc::getLorentzShiftEta(const IdentifierHash& elementHash, const Amg::Vector2D& locPos) {
  if (m_ignoreLocalPos) return getLorentzShiftEta(elementHash);
  // The cache is used to store the results. The cache is therefore invalidated if we specify a position.
  return getValue(elementHash, locPos, LorentzShiftEta);
}

double SiLorentzAngleCHSvc::getTanLorentzAngle(const IdentifierHash& elementHash) {
  const SiLorentzAngleCondData* condData{getCondData()};
  if (condData) {
    return condData->getTanLorentzAngle(elementHash);
  }
  return s_invalidValue;
}

double SiLorentzAngleCHSvc::getTanLorentzAngle(const IdentifierHash& elementHash, const Amg::Vector2D& locPos) {
  if (m_ignoreLocalPos) return getTanLorentzAngle(elementHash);
  // The cache is used to store the results. The cache is therefore invalidated if we specify a position.
  return getValue(elementHash, locPos, TanLorentzAngle);
}

double SiLorentzAngleCHSvc::getTanLorentzAngleEta(const IdentifierHash& elementHash) {
  const SiLorentzAngleCondData* condData{getCondData()};
  if (condData) {
    return condData->getTanLorentzAngleEta(elementHash);
  }
  return s_invalidValue;
}

double SiLorentzAngleCHSvc::getTanLorentzAngleEta(const IdentifierHash& elementHash, const Amg::Vector2D& locPos) {
  if (m_ignoreLocalPos) return getTanLorentzAngleEta(elementHash);
  // The cache is used to store the results. The cache is therefore invalidated if we specify a position.
  return getValue(elementHash, locPos, TanLorentzAngleEta);
}

double SiLorentzAngleCHSvc::getBiasVoltage(const IdentifierHash& elementHash) {
  const SiLorentzAngleCondData* condData{getCondData()};
  if (condData) {
    return condData->getBiasVoltage(elementHash);
  }
  return s_invalidValue;
}

double SiLorentzAngleCHSvc::getTemperature(const IdentifierHash& elementHash) {
  const SiLorentzAngleCondData* condData{getCondData()};
  if (condData) {
    return condData->getTemperature(elementHash);
  }
  return s_invalidValue;
}

double SiLorentzAngleCHSvc::getDepletionVoltage(const IdentifierHash& elementHash) {
  const SiLorentzAngleCondData* condData{getCondData()};
  if (condData) {
    return condData->getDepletionVoltage(elementHash);
  }
  return s_invalidValue;
}
   
double SiLorentzAngleCHSvc::getValue(const IdentifierHash& elementHash, const Amg::Vector2D& locPos, Variable variable) {
  if (not (variable==TanLorentzAngle or variable==LorentzShift or variable==TanLorentzAngleEta or variable==LorentzShiftEta)) {
    ATH_MSG_WARNING("getValue with Variable=" << variable << " is not available");
    return s_invalidValue;
  }

  double temperature{getTemperature(elementHash)};
  double deplVoltage{getDepletionVoltage(elementHash)};
  double biasVoltage{getBiasVoltage(elementHash)};

  // Calculate depletion depth. If biasVoltage is less than depletionVoltage
  // the detector is not fully depleted and we need to take this into account.
  // We take absolute values just in case voltages are signed.
  const InDetDD::SiDetectorElement* element{m_detManager->getDetectorElement(elementHash)};
  double depletionDepth{element->thickness()};
  if (deplVoltage==0.0) ATH_MSG_WARNING("Depletion voltage in "<<__FILE__<<" is zero, which might be a bug.");
  if (std::abs(biasVoltage) < std::abs(deplVoltage)) {
    depletionDepth *= sqrt(std::abs(biasVoltage / deplVoltage));
  }
  double meanElectricField{0.};
  if (depletionDepth) { 
    meanElectricField = biasVoltage / depletionDepth;
  }
  double mobility{0.};
  m_siProperties.setConditions(temperature, meanElectricField);
  mobility = m_siProperties.signedHallMobility(element->carrierType());
  // Get magnetic field.
  Amg::Vector3D magneticField{getMagneticField(elementHash, locPos)};

  double correctionFactor{getCorrectionFactor()};

  // The angles are in the hit frame. This is because that is what is needed by the digization and also
  // gives a more physical sign of the angle (ie dosen't flip sign when the detector is flipped).
  // The hit depth axis is pointing from the readout side to the backside if  m_design->readoutSide() < 0
  // The hit depth axis is pointing from the backside to the readout side if  m_design->readoutSide() > 0
  if (variable==TanLorentzAngle and variable==LorentzShift) {
    double tanLorentzAnglePhi{element->design().readoutSide()*mobility*element->hitDepthDirection()*element->hitPhiDirection()*(element->normal().cross(magneticField)).dot(element->phiAxis())};
    if (variable==TanLorentzAngle) {
      return correctionFactor*tanLorentzAnglePhi;
    }
    // This gives the effective correction in the reconstruction frame hence the extra hitPhiDirection()
    // as the angle above is in the hit frame.
    double lorentzCorrectionPhi{-0.5*element->hitPhiDirection()*tanLorentzAnglePhi*depletionDepth};
    return correctionFactor*lorentzCorrectionPhi;
  }

  // The Lorentz eta shift very small and so can be ignored, but we include it for completeness.
  // It is < ~0.1 um in the pixel.
  // In the SCT its largest in the stereo side of the barrel modules where it is about 0.3 micron along the strip. 
  if (variable==TanLorentzAngleEta and variable==LorentzShiftEta) {
    double tanLorentzAngleEta{element->design().readoutSide()*mobility*element->hitDepthDirection()*element->hitEtaDirection()*(element->normal().cross(magneticField)).dot(element->etaAxis())};
    if (variable==TanLorentzAngleEta) {
      return correctionFactor*tanLorentzAngleEta;
    }
    double lorentzCorrectionEta{-0.5*element->hitPhiDirection()*tanLorentzAngleEta*depletionDepth};
    return correctionFactor*lorentzCorrectionEta;
  }

  ATH_MSG_WARNING("You should not see this message. Something is wrong in getValue");
  return s_invalidValue;
}

double SiLorentzAngleCHSvc::getCorrectionFactor() const
{
  const SiLorentzAngleCondData* condData{getCondData()};
  if (condData) {
    return condData->getCorrectionFactor();
  }
  return s_invalidValue;
}

Amg::Vector3D SiLorentzAngleCHSvc::getMagneticField(const IdentifierHash& elementHash, const Amg::Vector2D& locPos) {
  // Get the magnetic field.
  const InDetDD::SiDetectorElement* element{m_detManager->getDetectorElement(elementHash)};
  Amg::Vector3D pointvec{element->globalPosition(locPos)};

  if (m_useMagFieldSvc) {
    ATH_MSG_VERBOSE("Getting magnetic field from magnetic field service.");
    double field[3];
    double point[3];
    point[0] = pointvec[0];
    point[1] = pointvec[1];
    point[2] = pointvec[2];
    m_magFieldSvc->getField(point, field);
    return Amg::Vector3D(field[0], field[1], field[2]);
  } else {
    ATH_MSG_VERBOSE("Using Nominal Field");
    return Amg::Vector3D(0., 0., m_nominalField);
  }
}

const SiLorentzAngleCondData* SiLorentzAngleCHSvc::getCondData() const {
  SG::ReadCondHandle<SiLorentzAngleCondData> handle{m_condData};
  if (handle.isValid()) {
    const SiLorentzAngleCondData* data{*handle};
    return data;
  }
  ATH_MSG_WARNING(m_condData.key() << " cannot be retrieved.");
  return nullptr;
}

const double SiLorentzAngleCHSvc::s_invalidValue{std::numeric_limits<double>::quiet_NaN()};
