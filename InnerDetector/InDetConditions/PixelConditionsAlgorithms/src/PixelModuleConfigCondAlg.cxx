/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelModuleConfigCondAlg.h"
#include "Identifier/IdentifierHash.h"
#include "GaudiKernel/EventIDRange.h"
#include <memory>
#include <sstream>

#include "PathResolver/PathResolver.h"


PixelModuleConfigCondAlg::PixelModuleConfigCondAlg(const std::string& name, ISvcLocator* pSvcLocator):
  ::AthReentrantAlgorithm(name, pSvcLocator)
{
}

StatusCode PixelModuleConfigCondAlg::initialize() {
  ATH_MSG_DEBUG("PixelModuleConfigCondAlg::initialize()");

  ATH_CHECK(m_condSvc.retrieve());
  ATH_CHECK(m_readDeadMapKey.initialize(SG::AllowEmpty));
  ATH_CHECK(m_writeKey.initialize());
  if (m_condSvc->regHandle(this,m_writeKey).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode PixelModuleConfigCondAlg::execute(const EventContext& ctx) const {
  ATH_MSG_DEBUG("PixelModuleConfigCondAlg::execute()");

  SG::WriteCondHandle<PixelModuleData> writeHandle(m_writeKey, ctx);
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid.. In theory this should not be called, but may happen if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS; 
  }

  // Construct the output Cond Object and fill it in
  std::unique_ptr<PixelModuleData> writeCdo(std::make_unique<PixelModuleData>());

  const EventIDBase start{EventIDBase::UNDEFNUM, EventIDBase::UNDEFEVT,                     0,                       
                                              0, EventIDBase::UNDEFNUM, EventIDBase::UNDEFNUM};
  const EventIDBase stop {EventIDBase::UNDEFNUM,   EventIDBase::UNDEFEVT, EventIDBase::UNDEFNUM-1, 
                          EventIDBase::UNDEFNUM-1, EventIDBase::UNDEFNUM, EventIDBase::UNDEFNUM};

  //==============
  // Set dead map
  //==============
  EventIDRange rangeDeadMap{start, stop};
  if (!m_readDeadMapKey.empty()) {
    ATH_MSG_INFO("Obsolate!! It shouldn't be called here..." << m_readDeadMapKey.key());

    SG::ReadCondHandle<CondAttrListCollection> readHandle(m_readDeadMapKey, ctx);
    const CondAttrListCollection* readCdo = *readHandle; 
    if (readCdo==nullptr) {
      ATH_MSG_FATAL("Null pointer to the read conditions object");
      return StatusCode::FAILURE;
    }
    // Get the validitiy range
    if (not readHandle.range(rangeDeadMap)) {
      ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandle.key());
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("Size of CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
    ATH_MSG_INFO("Range of input is " << rangeDeadMap);

    // Read dead map info
    for (CondAttrListCollection::const_iterator attrList=readCdo->begin(); attrList!=readCdo->end(); ++attrList) {
      const CondAttrListCollection::ChanNum &channelNumber = attrList->first;
      const CondAttrListCollection::AttributeList &payload = attrList->second;

      // RUN-1, RUN-2 format
      if (payload.exists("moduleID") and not payload["moduleID"].isNull() && payload.exists("ModuleSpecialPixelMap_Clob") and not payload["ModuleSpecialPixelMap_Clob"].isNull()) {

        int moduleHash = payload["moduleID"].data<int>();
        const std::string &stringStatus = payload["ModuleSpecialPixelMap_Clob"].data<std::string>();

        std::stringstream ss(stringStatus);
        std::vector<std::string> moduleStringStatus;
        std::string buffer;
        while (std::getline(ss,buffer,' ')) { moduleStringStatus.push_back(buffer); }

        if (moduleStringStatus.size()<2) {
          ATH_MSG_FATAL("Not enough moduleStringStatus data " << moduleStringStatus.size() << " < 2 for channel " <<  channelNumber << " read from " << readHandle.fullKey());
          return StatusCode::FAILURE;
        }

        int moduleStatus = std::atoi(moduleStringStatus[0].c_str());
        int chipStatus   = std::atoi(moduleStringStatus[1].c_str());

        if (moduleStatus>0) { writeCdo->setModuleStatus(moduleHash, moduleStatus); }
        if (chipStatus>0)   { writeCdo->setChipStatus(moduleHash, chipStatus); }
      } 
      else {
        ATH_MSG_WARNING("Can not retrieve " << channelNumber);
      }
    }
  }

  // Digitization parameters
  writeCdo -> setBunchSpace(m_bunchSpace);
  writeCdo -> setBarrelNumberOfBCID(m_BarrelNumberOfBCID);
  writeCdo -> setEndcapNumberOfBCID(m_EndcapNumberOfBCID);
  writeCdo -> setBarrelTimeOffset(m_BarrelTimeOffset);
  writeCdo -> setEndcapTimeOffset(m_EndcapTimeOffset);
  writeCdo -> setBarrelTimeJitter(m_BarrelTimeJitter);
  writeCdo -> setEndcapTimeJitter(m_EndcapTimeJitter);
  writeCdo -> setDefaultBarrelAnalogThreshold(m_BarrelAnalogThreshold);
  writeCdo -> setDefaultEndcapAnalogThreshold(m_EndcapAnalogThreshold);
  writeCdo -> setDefaultBarrelAnalogThresholdSigma(m_BarrelAnalogThresholdSigma);
  writeCdo -> setDefaultEndcapAnalogThresholdSigma(m_EndcapAnalogThresholdSigma);
  writeCdo -> setDefaultBarrelAnalogThresholdNoise(m_BarrelAnalogThresholdNoise);
  writeCdo -> setDefaultEndcapAnalogThresholdNoise(m_EndcapAnalogThresholdNoise);
  writeCdo -> setDefaultBarrelInTimeThreshold(m_BarrelInTimeThreshold);
  writeCdo -> setDefaultEndcapInTimeThreshold(m_EndcapInTimeThreshold);
  writeCdo -> setBarrelThermalNoise(m_BarrelThermalNoise);
  writeCdo -> setEndcapThermalNoise(m_EndcapThermalNoise);
  writeCdo -> setFEI4BarrelHitDiscConfig(m_FEI4BarrelHitDiscConfig);
  writeCdo -> setFEI4EndcapHitDiscConfig(m_FEI4EndcapHitDiscConfig);
  writeCdo -> setFEI4ChargScaling(m_chargeScaleFEI4);
  writeCdo -> setUseFEI4SpecialScalingFunction(m_UseFEI4SpecialScalingFunction);

  //FEI3-specific parameters... only write if they are set
  if(m_FEI3BarrelLatency.size()!=0){
    //Use the above as the check... worth adding checks to see that all FEI3 vectors are non-zero?
    writeCdo -> setFEI3BarrelLatency(m_FEI3BarrelLatency);
    writeCdo -> setFEI3BarrelHitDuplication(m_FEI3BarrelHitDuplication);
    writeCdo -> setFEI3BarrelSmallHitToT(m_FEI3BarrelSmallHitToT);
    writeCdo -> setFEI3BarrelTimingSimTune(m_FEI3BarrelTimingSimTune);

    writeCdo -> setEndcapToTThreshold(m_EndcapToTThreshold);
    writeCdo -> setFEI3EndcapLatency(m_FEI3EndcapLatency);
    writeCdo -> setFEI3EndcapHitDuplication(m_FEI3EndcapHitDuplication);
    writeCdo -> setFEI3EndcapSmallHitToT(m_FEI3EndcapSmallHitToT);
    writeCdo -> setFEI3EndcapTimingSimTune(m_FEI3EndcapTimingSimTune);

  }

    //DBM-specific parameters... only write if they are set
  if(m_DBMToTThreshold.size()!=0){
       writeCdo -> setDBMNumberOfBCID(m_DBMNumberOfBCID);
       writeCdo -> setDBMTimeOffset(m_DBMTimeOffset);
       writeCdo -> setDBMTimeJitter(m_DBMTimeJitter);
       writeCdo -> setDefaultDBMAnalogThreshold(m_DBMAnalogThreshold);
       writeCdo -> setDefaultDBMAnalogThresholdSigma(m_DBMAnalogThresholdSigma);
       writeCdo -> setDefaultDBMAnalogThresholdNoise(m_DBMAnalogThresholdNoise);
       writeCdo -> setDefaultDBMInTimeThreshold(m_DBMInTimeThreshold);
       writeCdo -> setDBMThermalNoise(m_DBMThermalNoise);

  }

  // Charge calibration parameters
  writeCdo -> setDefaultQ2TotA(m_CalibrationParameterA);
  writeCdo -> setDefaultQ2TotE(m_CalibrationParameterE);
  writeCdo -> setDefaultQ2TotC(m_CalibrationParameterC);

  // DCS parameters
  writeCdo -> setDefaultBiasVoltage(m_biasVoltage);
  writeCdo -> setDefaultTemperature(m_temperature);

  // Distortion parameters
  writeCdo -> setDistortionInputSource(m_distortionInputSource);
  writeCdo -> setDistortionVersion(m_distortionVersion);
  writeCdo -> setDistortionR1(m_distortionR1);
  writeCdo -> setDistortionR2(m_distortionR2);
  writeCdo -> setDistortionTwist(m_distortionTwist);
  writeCdo -> setDistortionMeanR(m_distortionMeanR);
  writeCdo -> setDistortionRMSR(m_distortionRMSR);
  writeCdo -> setDistortionMeanTwist(m_distortionMeanTwist);
  writeCdo -> setDistortionRMSTwist(m_distortionRMSTwist);
  writeCdo -> setDistortionWriteToFile(m_distortionWriteToFile);
  writeCdo -> setDistortionFileName(m_distortionFileName);

  // Cabling parameters
  writeCdo -> setCablingMapToFile(m_cablingMapToFile);
  writeCdo -> setCablingMapFileName(m_cablingMapFileName);

  // mapping files for radiation damage simulation
  std::vector<std::string> mapsPath_list;
  std::vector<std::string> mapsPath_list3D;

 
    writeCdo -> setBarrelToTThreshold(m_BarrelToTThreshold);
    writeCdo -> setBarrelCrossTalk(m_BarrelCrossTalk);
    writeCdo -> setBarrelNoiseOccupancy(m_BarrelNoiseOccupancy);
    writeCdo -> setBarrelDisableProbability(m_BarrelDisableProbability);
    writeCdo -> setBarrelLorentzAngleCorr(m_BarrelLorentzAngleCorr);
    writeCdo -> setDefaultBarrelBiasVoltage(m_BarrelBiasVoltage);

    writeCdo -> setEndcapToTThreshold(m_EndcapToTThreshold);
    writeCdo -> setEndcapCrossTalk(m_EndcapCrossTalk);
    writeCdo -> setEndcapNoiseOccupancy(m_EndcapNoiseOccupancy);
    writeCdo -> setEndcapDisableProbability(m_EndcapDisableProbability);
    writeCdo -> setEndcapLorentzAngleCorr(m_EndcapLorentzAngleCorr);
    writeCdo -> setDefaultEndcapBiasVoltage(m_EndcapBiasVoltage);

    // This is ad-hoc solution.
    for (size_t i=0; i<m_InnermostNoiseShape.size(); i++)     { writeCdo->setBarrelNoiseShape(0,m_InnermostNoiseShape[i]); }
    for (size_t i=0; i<m_NextInnermostNoiseShape.size(); i++) { writeCdo->setBarrelNoiseShape(1,m_NextInnermostNoiseShape[i]); }
    for (size_t i=0; i<m_PixelNoiseShape.size(); i++)  {
      for (size_t layer:{2,3,4}) { writeCdo->setBarrelNoiseShape(layer,m_PixelNoiseShape[i]); }
    }

    for (size_t i=0; i<m_EndcapToTThreshold.size(); i++) {
      for (size_t j=0; j<m_PixelNoiseShape.size(); j++)  { writeCdo->setEndcapNoiseShape(i,m_PixelNoiseShape[j]); }
    }

    // Radiation damage simulation
    writeCdo -> setFluenceLayer(m_BarrelFluence);
    for (size_t i=0; i<m_BarrelFluenceMap.size(); i++) {
      mapsPath_list.push_back(PathResolverFindCalibFile(m_BarrelFluenceMap[i]));
    }

    // Radiation damage simulation for 3D sensor
    writeCdo -> setFluenceLayer3D(m_3DFluence);
    for (size_t i=0; i<m_3DFluenceMap.size(); i++) {
      mapsPath_list3D.push_back(PathResolverFindCalibFile(m_3DFluenceMap[i]));
    }

  //=======================
  // Combine time interval
  //=======================
  EventIDRange rangeW{start, stop};
  rangeW = rangeDeadMap;

  if (rangeW.stop().isValid() && rangeW.start()>rangeW.stop()) {
    ATH_MSG_FATAL("Invalid intersection rangeW: " << rangeW);
    return StatusCode::FAILURE;
  }

  if (writeHandle.record(rangeW, std::move(writeCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record PixelModuleData " << writeHandle.key() << " with EventRange " << rangeW << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

  return StatusCode::SUCCESS;
}


