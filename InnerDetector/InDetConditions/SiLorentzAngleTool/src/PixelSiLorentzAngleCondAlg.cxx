/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelSiLorentzAngleCondAlg.h"

#include "GaudiKernel/PhysicalConstants.h"

#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "PixelReadoutGeometry/PixelModuleDesign.h"
#include "SiPropertiesTool/SiliconProperties.h"

PixelSiLorentzAngleCondAlg::PixelSiLorentzAngleCondAlg(const std::string& name, ISvcLocator* pSvcLocator):
  ::AthReentrantAlgorithm(name, pSvcLocator),
  m_condSvc("CondSvc", name)
{
}

StatusCode PixelSiLorentzAngleCondAlg::initialize() {
  ATH_MSG_DEBUG("PixelSiLorentzAngleCondAlg::initialize()");

  const PixelID* idHelper{nullptr};
  ATH_CHECK(detStore()->retrieve(idHelper,"PixelID"));
  m_maxHash = idHelper->wafer_hash_max();

  ATH_CHECK(m_condSvc.retrieve());

  ATH_CHECK(m_readKeyTemp.initialize());
  ATH_CHECK(m_readKeyHV.initialize());
  ATH_CHECK(m_writeKey.initialize());
  if (m_condSvc->regHandle(this, m_writeKey).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  ATH_CHECK(m_siPropertiesTool.retrieve());

  ATH_CHECK(m_fieldCondObjInputKey.initialize( m_useMagFieldCache ));
  ATH_CHECK(m_readKeyBFieldSensor.initialize( m_useMagFieldCache & m_useMagFieldDcs ));

  ATH_CHECK(m_pixelDetEleCollKey.initialize());
  
  return StatusCode::SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode
PixelSiLorentzAngleCondAlg::execute(const EventContext& ctx) const {
////////////////////////////////////////////////////////////////////////////////////////////////

  ATH_MSG_DEBUG("PixelSiLorentzAngleCondAlg::execute()");

  SG::WriteCondHandle<SiLorentzAngleCondData> writeHandle{m_writeKey, ctx};
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid." << " In theory this should not be called, but may happen" << " if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS;
  }

  // Read Cond Handle (temperature)
  SG::ReadCondHandle<PixelDCSTempData> readHandleTemp(m_readKeyTemp, ctx);
  const PixelDCSTempData* readCdoTemp(*readHandleTemp);
  if (readCdoTemp==nullptr) {
    ATH_MSG_FATAL("Null pointer to the read conditions object");
    return StatusCode::FAILURE;
  }
  writeHandle.addDependency(readHandleTemp);
  ATH_MSG_DEBUG("Input is " << readHandleTemp.fullKey() << " with the range of " << readHandleTemp.getRange());

  // Read Cond Handle (HV)
  SG::ReadCondHandle<PixelDCSHVData> readHandleHV(m_readKeyHV, ctx);
  const PixelDCSHVData* readCdoHV(*readHandleHV);
  if (readCdoHV==nullptr) {
    ATH_MSG_FATAL("Null pointer to the read conditions object");
    return StatusCode::FAILURE;
  }
  writeHandle.addDependency(readHandleHV);
  ATH_MSG_DEBUG("Input is " << readHandleHV.fullKey() << " with the range of " << readHandleHV.getRange());

  // Field cache object for field calculations
  MagField::AtlasFieldCache    fieldCache;
  if (m_useMagFieldCache) {
////////////////////////////////////////////////////////////////////////////////////////////////////
    // Get field cache object
    SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandleField{m_fieldCondObjInputKey, ctx};
    const AtlasFieldCacheCondObj* fieldCondObj{*readHandleField};

    if (fieldCondObj == nullptr) {
        ATH_MSG_ERROR("PixelSiLorentzAngleCondAlg: Failed to retrieve AtlasFieldCacheCondObj with key " << m_fieldCondObjInputKey.key());
        return StatusCode::FAILURE;
    }
    //WL: The previous implementation didn't intersect the range of this readHanlde.
    //I suspect that was a bug. 
    writeHandle.addDependency(readHandleField);
    fieldCondObj->getInitializedCache (fieldCache);
////////////////////////////////////////////////////////////////////////////////////////////////////

    if (m_useMagFieldDcs) {
      // Read Cond Handle (B field sensor)
      SG::ReadCondHandle<CondAttrListCollection> readHandleBFieldSensor(m_readKeyBFieldSensor, ctx);
      const CondAttrListCollection* readCdoBFieldSensor(*readHandleBFieldSensor);
      if (readCdoBFieldSensor==nullptr) {
        ATH_MSG_FATAL("Null pointer to the read conditions object");
        return StatusCode::FAILURE;
      }
      writeHandle.addDependency(readHandleBFieldSensor);
      ATH_MSG_DEBUG("Input is " << readHandleBFieldSensor.fullKey() << " with the range of " << readHandleBFieldSensor.getRange() );

    }//end if useMagFieldDcs
  }//end if useMagFieldSvc


  SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> pixelDetEle(m_pixelDetEleCollKey, ctx);
  const InDetDD::SiDetectorElementCollection* elements(pixelDetEle.retrieve());
  if (elements==nullptr) {
    ATH_MSG_FATAL(m_pixelDetEleCollKey.fullKey() << " could not be retrieved");
    return StatusCode::FAILURE;
  }
  writeHandle.addDependency(pixelDetEle);

  // Construct the output Cond Object and fill it in
  std::unique_ptr<SiLorentzAngleCondData> writeCdo{std::make_unique<SiLorentzAngleCondData>()};
  const PixelID::size_type wafer_hash_max = m_maxHash;
  writeCdo->resize(wafer_hash_max);
  for (PixelID::size_type hash = 0; hash < wafer_hash_max; hash++) {
    const IdentifierHash elementHash = static_cast<IdentifierHash::value_type>(hash);

    double temperature = readCdoTemp->getTemperature(elementHash)+273.15;
    double deplVoltage = 0.0*CLHEP::volt;
    double biasVoltage = readCdoHV->getBiasVoltage(elementHash)*CLHEP::volt;

    ATH_MSG_DEBUG("Pixel Hash = " << elementHash << " Temperature = " << temperature << " [deg K], BiasV = " << biasVoltage << " DeplV = " << deplVoltage);

    const InDetDD::SiDetectorElement* element = elements->getDetectorElement(elementHash);
    double depletionDepth = element->thickness();
    if (std::fabs(biasVoltage) < std::fabs(deplVoltage)) {
      depletionDepth *= std::sqrt(std::fabs(biasVoltage/deplVoltage));
    }

    const InDetDD::PixelModuleDesign* p_design = dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design());
    if (not p_design){
      ATH_MSG_FATAL("Dynamic cast to PixelModuleDesign* failed in PixelSiLorentzAngleCondAlg::execute");
      return StatusCode::FAILURE;
    }
    double forceLorentzToZero = 1.0;
    if (p_design->getReadoutTechnology()==InDetDD::PixelModuleDesign::FEI4 && p_design->numberOfCircuits()==1 && p_design->rowsPerCircuit()>100) {  // IBL 3D
      forceLorentzToZero = 0.0;
    }

    const InDet::SiliconProperties &siProperties = m_siPropertiesTool->getSiProperties(elementHash);
    double mobility = siProperties.signedHallMobility(element->carrierType());

    // Get magnetic field. This first checks that field cache is valid.
    Amg::Vector3D magneticField = getMagneticField(fieldCache,element);

    // The angles are in the hit frame. This is because that is what is needed by the digization and also
    // gives a more physical sign of the angle (ie dosen't flip sign when the detector is flipped).
    // The hit depth axis is pointing from the readout side to the backside if  m_design->readoutSide() < 0
    // The hit depth axis is pointing from the backside to the readout side if  m_design->readoutSide() > 0
    double tanLorentzAnglePhi = forceLorentzToZero*element->design().readoutSide()*mobility*element->hitDepthDirection()*element->hitPhiDirection()*(element->normal().cross(magneticField)).dot(element->phiAxis());
    writeCdo->setTanLorentzAngle(elementHash, m_correctionFactor*tanLorentzAnglePhi);

    // This gives the effective correction in the reconstruction frame hence the extra hitPhiDirection()
    // as the angle above is in the hit frame.
    double lorentzCorrectionPhi = -0.5*element->hitPhiDirection()*tanLorentzAnglePhi*depletionDepth;
    writeCdo->setLorentzShift(elementHash, m_correctionFactor*lorentzCorrectionPhi);
 
    // The Lorentz eta shift very small and so can be ignored, but we include it for completeness.
    double tanLorentzAngleEta = forceLorentzToZero*element->design().readoutSide()*mobility*element->hitDepthDirection()*element->hitEtaDirection()*(element->normal().cross(magneticField)).dot(element->etaAxis());
    writeCdo->setTanLorentzAngleEta(elementHash, m_correctionFactor*tanLorentzAngleEta);
    double lorentzCorrectionEta = -0.5*element->hitPhiDirection()*tanLorentzAngleEta*depletionDepth;
    writeCdo->setLorentzShiftEta(elementHash, m_correctionFactor*lorentzCorrectionEta);

    // Monitoring value
    writeCdo->setBiasVoltage(elementHash, biasVoltage/CLHEP::volt);
    writeCdo->setTemperature(elementHash, temperature-273.15);
    writeCdo->setDepletionVoltage(elementHash, deplVoltage/CLHEP::volt);

    ATH_MSG_DEBUG("Hash = " << elementHash << " tanPhi = " << lorentzCorrectionPhi << " shiftPhi = " << writeCdo->getLorentzShift(elementHash) << " Factor = " << m_correctionFactor << "Depletion depth = " << depletionDepth);
    ATH_MSG_DEBUG("Hash = " << elementHash << " tanPhi = " << lorentzCorrectionPhi << " shiftPhi = " << writeCdo->getLorentzShift(elementHash) << "Depletion depth = " << depletionDepth);
    ATH_MSG_VERBOSE("Temperature (C), bias voltage, depletion voltage: " << temperature-273.15 << ", " << biasVoltage/CLHEP::volt << ", " << deplVoltage/CLHEP::volt);
    ATH_MSG_VERBOSE("Depletion depth: " << depletionDepth/CLHEP::mm);
    ATH_MSG_VERBOSE("Mobility (cm2/V/s): " << mobility/(CLHEP::cm2/CLHEP::volt/CLHEP::s));
    ATH_MSG_VERBOSE("Magnetic Field (tesla): " << "(" << magneticField.x()/CLHEP::tesla << "," << magneticField.y()/CLHEP::tesla << "," << magneticField.z()/CLHEP::tesla << ")");
    ATH_MSG_VERBOSE("LorentzShift, tanLorentzAngle = " << writeCdo->getLorentzShift(elementHash) << ", " << writeCdo->getTanLorentzAngle(elementHash));
    ATH_MSG_VERBOSE("LorentzShiftEta, tanLorentzAngleEta = " << writeCdo->getLorentzShiftEta(elementHash) << ", " << writeCdo->getTanLorentzAngleEta(elementHash));
  }

  // Record the output cond object
  if (writeHandle.record(std::move(writeCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record SiLorentzAngleCondData " << writeHandle.key() << " with EventRange " << writeHandle.getRange() << " into Conditions Store");
    return StatusCode::FAILURE;
    }
    
  ATH_MSG_DEBUG("recorded new CDO " << writeHandle.key() << " with range " << writeHandle.getRange() << " into Conditions Store");

  return StatusCode::SUCCESS;
}

StatusCode PixelSiLorentzAngleCondAlg::finalize() {
  ATH_MSG_DEBUG("PixelSiLorentzAngleCondAlg::finalize()");
  return StatusCode::SUCCESS;
}

Amg::Vector3D PixelSiLorentzAngleCondAlg::getMagneticField(MagField::AtlasFieldCache& fieldCache, const InDetDD::SiDetectorElement* element) const {
  if (m_useMagFieldCache) {
    Amg::Vector3D pointvec = element->center();

    ATH_MSG_VERBOSE("Getting magnetic field from MT magnetic field service.");
    double point[3];
    point[0] = pointvec[0];
    point[1] = pointvec[1];
    point[2] = pointvec[2];
    double field[3];
    fieldCache.getField(point, field);
    return Amg::Vector3D(field[0], field[1], field[2]);
  } 
  else {
    ATH_MSG_VERBOSE("Using Nominal Field");
    return Amg::Vector3D(0., 0., m_nominalField);
  }
}
