/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCTSiLorentzAngleCondAlg.cxx
 * @author Susumu.Oda@cern.ch
 **/

// Local include
#include "SCTSiLorentzAngleCondAlg.h"

// Athena includes
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "SiPropertiesSvc/SiliconProperties.h"

// Gaudi include
#include "GaudiKernel/PhysicalConstants.h"

// STL include

SCTSiLorentzAngleCondAlg::SCTSiLorentzAngleCondAlg(const std::string& name, ISvcLocator* pSvcLocator):
  ::AthReentrantAlgorithm(name, pSvcLocator),
  m_condSvc{"CondSvc", name},
  m_magFieldSvc{"AtlasFieldSvc", name},
  m_maxHash{0}
{
  // YOU NEED TO USE THE SAME PROPERTIES AS USED IN SCTLorentzAngleTool!!!
  declareProperty("MagFieldSvc", m_magFieldSvc);
}

StatusCode SCTSiLorentzAngleCondAlg::initialize()
{
  if (m_siConditionsTool.empty()) m_sctDefaults.setValue(true);

  if (not m_sctDefaults.value()) {
    // SCTSiliconConditionsTool
    ATH_CHECK(m_siConditionsTool.retrieve());
    // Read Cond handle
    if (not m_useGeoModel.value()) {
      ATH_CHECK(m_readKeyTemp.initialize());
      ATH_CHECK(m_readKeyHV.initialize());
    }
  } else {
    m_siConditionsTool.disable();
  }

  if (m_useMagFieldSvc.value()) {
    // MagFieldSvc
    ATH_CHECK(m_magFieldSvc.retrieve());
    // Read Cond handle
    if (m_useMagFieldDcs.value()) {
      ATH_CHECK(m_readKeyBFieldSensor.initialize());
    }
  }

  ATH_CHECK(m_SCTDetEleCollKey.initialize());

  // Write Cond Handle
  ATH_CHECK(m_condSvc.retrieve());
  ATH_CHECK(m_writeKey.initialize());
  if (m_condSvc->regHandle(this, m_writeKey).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  // Get maximum hash for vector sizes. We need the idhelper for this.
  const SCT_ID* idHelper{nullptr};
  ATH_CHECK(detStore()->retrieve(idHelper, "SCT_ID"));
  m_maxHash = idHelper->wafer_hash_max();

  return StatusCode::SUCCESS;
}

StatusCode SCTSiLorentzAngleCondAlg::execute(const EventContext& ctx) const
{
  // Write Cond Handle
  SG::WriteCondHandle<SiLorentzAngleCondData> writeHandle{m_writeKey, ctx};
  // Do we have a valid Write Cond Handle for current time?
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
                  << " In theory this should not be called, but may happen"
                  << " if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS;
  }

  EventIDBase eidStart;
  eidStart.set_time_stamp(0);
  eidStart.set_time_stamp_ns_offset(0);
  EventIDBase eidStop;
  eidStop.set_time_stamp(EventIDBase::UNDEFNUM);
  eidStop.set_time_stamp_ns_offset(EventIDBase::UNDEFNUM);
  EventIDRange rangeSCT{eidStart, eidStop};
  EventIDRange rangeBField{eidStart, eidStop};

  // Get SCT_DetectorElementCollection
  SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> sctDetEle(m_SCTDetEleCollKey, ctx);
  const InDetDD::SiDetectorElementCollection* elements(sctDetEle.retrieve());
  if (elements==nullptr) {
    ATH_MSG_FATAL(m_SCTDetEleCollKey.fullKey() << " could not be retrieved");
    return StatusCode::FAILURE;
  }
  EventIDRange rangeDetEle; // Run-LB IOV
  if (not sctDetEle.range(rangeDetEle)) {
    ATH_MSG_FATAL("Failed to retrieve validity range for " << m_SCTDetEleCollKey.key());
    return StatusCode::FAILURE;
  }

  if ((not m_sctDefaults.value()) and (not m_useGeoModel.value())) {
    // Read Cond Handle (temperature)
    SG::ReadCondHandle<SCT_DCSFloatCondData> readHandleTemp{m_readKeyTemp, ctx};
    const SCT_DCSFloatCondData* readCdoTemp{*readHandleTemp};
    if (readCdoTemp==nullptr) {
      ATH_MSG_FATAL("Null pointer to the read conditions object");
      return StatusCode::FAILURE;
    }
    EventIDRange rangeTemp;
    if (not readHandleTemp.range(rangeTemp)) {
      ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandleTemp.key());
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("Input is " << readHandleTemp.fullKey() << " with the range of " << rangeTemp);
    
    // Read Cond Handle (HV)
    SG::ReadCondHandle<SCT_DCSFloatCondData> readHandleHV{m_readKeyHV, ctx};
    const SCT_DCSFloatCondData* readCdoHV{*readHandleHV};
    if (readCdoHV==nullptr) {
      ATH_MSG_FATAL("Null pointer to the read conditions object");
      return StatusCode::FAILURE;
    }
    EventIDRange rangeHV;
    if (not readHandleHV.range(rangeHV)) {
      ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandleHV.key());
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("Input is " << readHandleHV.fullKey() << " with the range of " << rangeHV);
    
    // Combined the validity ranges of temp and HV
    rangeSCT = EventIDRange::intersect(rangeTemp, rangeHV);
    if (rangeSCT.stop().isValid() and rangeSCT.start()>rangeSCT.stop()) {
      ATH_MSG_FATAL("Invalid intersection rangeSCT: " << rangeSCT);
      return StatusCode::FAILURE;
    }
  }
  
  if (m_useMagFieldSvc.value()) {
    if (m_useMagFieldDcs.value()) {
      // Read Cond Handle (B field sensor)
      SG::ReadCondHandle<CondAttrListCollection> readHandleBFieldSensor{m_readKeyBFieldSensor, ctx};
      const CondAttrListCollection* readCdoBFieldSensor{*readHandleBFieldSensor};
      if (readCdoBFieldSensor==nullptr) {
        ATH_MSG_FATAL("Null pointer to the read conditions object");
        return StatusCode::FAILURE;
      }
      EventIDRange rangeBFieldSensor;
      if (not readHandleBFieldSensor.range(rangeBFieldSensor)) {
        ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandleBFieldSensor.key());
        return StatusCode::FAILURE;
      }
      ATH_MSG_INFO("Input is " << readHandleBFieldSensor.fullKey() << " with the range of " << rangeBFieldSensor);

      // Set the validity ranges of sensor
      rangeBField = rangeBFieldSensor;
      if (rangeBField.stop().isValid() and rangeBField.start()>rangeBField.stop()) {
        ATH_MSG_FATAL("Invalid intersection rangeBField: " << rangeBField);
        return StatusCode::FAILURE;
      }
    }
  }

  // Combined the validity ranges of temp and HV
  EventIDRange rangeW{EventIDRange::intersect(rangeSCT, rangeBField)};
  if (rangeW.stop().isValid() and rangeW.start()>rangeW.stop()) {
    ATH_MSG_FATAL("Invalid intersection rangeW: " << rangeW);
    return StatusCode::FAILURE;
  }

  // Construct the output Cond Object and fill it in
  std::unique_ptr<SiLorentzAngleCondData> writeCdo{std::make_unique<SiLorentzAngleCondData>()};
  writeCdo->resize(m_maxHash);
  //  SiLorentzAngleCondData myData;

  for (unsigned int hash{0}; hash<m_maxHash; hash++) {
    const IdentifierHash elementHash{hash};

    double temperatureC{0.0};
    double deplVoltage{0.0};
    double biasVoltage{0.0};

    if (m_sctDefaults.value()) {
      temperatureC = m_temperature.value();
      deplVoltage = m_deplVoltage.value() * CLHEP::volt;
      biasVoltage = m_biasVoltage.value() * CLHEP::volt;
    } else {
      temperatureC = m_siConditionsTool->temperature(elementHash);
      deplVoltage = m_siConditionsTool->depletionVoltage(elementHash) * CLHEP::volt;
      biasVoltage = m_siConditionsTool->biasVoltage(elementHash) * CLHEP::volt;
      ATH_MSG_DEBUG("SCT Hash = " << elementHash << " Temperature = " << temperatureC << " [deg C], BiasV = " << biasVoltage << " DeplV = " << deplVoltage);
    }

    // Protect against invalid temperature
    if (not ((temperatureC>m_temperatureMin) and (temperatureC<m_temperatureMax))) {
      temperatureC = m_temperature.value();
      ATH_MSG_DEBUG("Invalid temperature: " << temperatureC << " C. " << "Setting to " << m_temperature.value() << " C. " << "Detector element hash: " << elementHash);
    }
    double temperature{temperatureC + Gaudi::Units::STP_Temperature}; // C -> K

    // Calculate depletion depth. If biasVoltage is less than depletionVoltage
    // the detector is not fully depleted and we need to take this into account.
    // We take absolute values just in case voltages are signed.
    const InDetDD::SiDetectorElement* element{elements->getDetectorElement(elementHash)};
    double depletionDepth{element->thickness()};
    if (deplVoltage==0.0) {
      ATH_MSG_WARNING("Depletion voltage in "<<__FILE__<<" is zero, which might be a bug.");
    }
    if (std::abs(biasVoltage) < std::abs(deplVoltage)) {
      depletionDepth *= sqrt(std::abs(biasVoltage / deplVoltage));
    }
 
    double meanElectricField{0.};
    if (depletionDepth>0.) { 
      meanElectricField = biasVoltage / depletionDepth;
    }
    InDet::SiliconProperties siProperties;
    siProperties.setConditions(temperature, meanElectricField);
    double mobility{siProperties.signedHallMobility(element->carrierType())};

    // Get magnetic field. This first checks that field cache is valid.
    Amg::Vector3D magneticField{getMagneticField(element)};

    // The angles are in the hit frame. This is because that is what is needed by the digization and also
    // gives a more physical sign of the angle (ie dosen't flip sign when the detector is flipped).
    // The hit depth axis is pointing from the readout side to the backside if  m_design->readoutSide() < 0
    // The hit depth axis is pointing from the backside to the readout side if  m_design->readoutSide() > 0
    double tanLorentzAnglePhi{element->design().readoutSide() * mobility * element->hitDepthDirection() 
        * element->hitPhiDirection() * (element->normal().cross(magneticField)).dot(element->phiAxis())};
    writeCdo->setTanLorentzAngle(elementHash, tanLorentzAnglePhi);

    // This gives the effective correction in the reconstruction frame hence the extra hitPhiDirection()
    // as the angle above is in the hit frame.
    double lorentzCorrectionPhi{-0.5 * element->hitPhiDirection() * tanLorentzAnglePhi * depletionDepth};
    writeCdo->setLorentzShift(elementHash, lorentzCorrectionPhi);
 
    // The Lorentz eta shift very small and so can be ignored, but we include it for completeness.
    // In the SCT its largest in the stereo side of the barrel modules where it is about 0.3 micron along the strip. 
    double tanLorentzAngleEta{element->design().readoutSide() * mobility * element->hitDepthDirection()
        * element->hitEtaDirection() * (element->normal().cross(magneticField)).dot(element->etaAxis())};
    writeCdo->setTanLorentzAngleEta(elementHash, tanLorentzAngleEta);
    double lorentzCorrectionEta{-0.5 * element->hitPhiDirection() * tanLorentzAngleEta * depletionDepth};
    writeCdo->setLorentzShiftEta(elementHash, lorentzCorrectionEta);

    // Monitoring value
    writeCdo->setBiasVoltage(elementHash, biasVoltage/CLHEP::volt);
    writeCdo->setTemperature(elementHash, temperatureC);
    writeCdo->setDepletionVoltage(elementHash, deplVoltage/CLHEP::volt);

    ATH_MSG_DEBUG("Hash = " << elementHash << " tanPhi = " << lorentzCorrectionPhi << " shiftPhi = " << writeCdo->getLorentzShift(elementHash) << "Depletion depth = " << depletionDepth);
    ATH_MSG_VERBOSE("Temperature (C), bias voltage, depletion voltage: " << temperatureC << ", " << biasVoltage/CLHEP::volt << ", " << deplVoltage/CLHEP::volt);
    ATH_MSG_VERBOSE("Depletion depth: " << depletionDepth/CLHEP::mm);
    ATH_MSG_VERBOSE("Mobility (cm2/V/s): " << mobility/(CLHEP::cm2/CLHEP::volt/CLHEP::s));
    ATH_MSG_VERBOSE("Magnetic Field (tesla): " << "(" << magneticField.x()/CLHEP::tesla << "," << magneticField.y()/CLHEP::tesla << "," << magneticField.z()/CLHEP::tesla << ")");
    ATH_MSG_VERBOSE("LorentzShift, tanLorentzAngle = " << writeCdo->getLorentzShift(elementHash) << ", " << writeCdo->getTanLorentzAngle(elementHash));
    ATH_MSG_VERBOSE("LorentzShiftEta, tanLorentzAngleEta = " << writeCdo->getLorentzShiftEta(elementHash) << ", " << writeCdo->getTanLorentzAngleEta(elementHash));
  }

  // Record the output cond object
  if (writeHandle.record(rangeW, std::move(writeCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record SiLorentzAngleCondData " << writeHandle.key() 
                  << " with EventRange " << rangeW << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

  return StatusCode::SUCCESS;
}

StatusCode SCTSiLorentzAngleCondAlg::finalize()
{
  return StatusCode::SUCCESS;
}

Amg::Vector3D SCTSiLorentzAngleCondAlg::getMagneticField(const InDetDD::SiDetectorElement* element) const {
  if (m_useMagFieldSvc.value()) {
    Amg::Vector3D pointvec{element->center()};
    ATH_MSG_VERBOSE("Getting magnetic field from magnetic field service.");
    double point[3];
    point[0] = pointvec[0];
    point[1] = pointvec[1];
    point[2] = pointvec[2];
    double field[3];
    m_magFieldSvc->getField(point, field);
    return Amg::Vector3D(field[0], field[1], field[2]);
  } else {
    ATH_MSG_VERBOSE("Using Nominal Field");
    return Amg::Vector3D(0., 0., m_nominalField);
  }
}
