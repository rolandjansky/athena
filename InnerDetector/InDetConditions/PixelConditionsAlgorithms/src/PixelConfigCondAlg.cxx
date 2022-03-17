/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelConfigCondAlg.h"
#include "Identifier/IdentifierHash.h"
#include "GaudiKernel/EventIDRange.h"
#include <memory>
#include <sstream>

#include "PathResolver/PathResolver.h"
#include "TFile.h"


PixelConfigCondAlg::PixelConfigCondAlg(const std::string& name, ISvcLocator* pSvcLocator):
  ::AthReentrantAlgorithm(name, pSvcLocator)
{
}

StatusCode PixelConfigCondAlg::initialize() {
  ATH_MSG_DEBUG("PixelConfigCondAlg::initialize()");

  ATH_CHECK(m_writeKey.initialize());

  return StatusCode::SUCCESS;
}

StatusCode PixelConfigCondAlg::execute(const EventContext& ctx) const {
  ATH_MSG_DEBUG("PixelConfigCondAlg::execute()");

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

  // Digitization parameters
  writeCdo -> setBunchSpace(m_bunchSpace);
  writeCdo -> setBarrelNumberOfBCID(m_BarrelNumberOfBCID);
  writeCdo -> setEndcapNumberOfBCID(m_EndcapNumberOfBCID);
  writeCdo -> setDBMNumberOfBCID(m_DBMNumberOfBCID);
  writeCdo -> setBarrelTimeOffset(m_BarrelTimeOffset);
  writeCdo -> setEndcapTimeOffset(m_EndcapTimeOffset);
  writeCdo -> setDBMTimeOffset(m_DBMTimeOffset);
  writeCdo -> setBarrelTimeJitter(m_BarrelTimeJitter);
  writeCdo -> setEndcapTimeJitter(m_EndcapTimeJitter);
  writeCdo -> setDBMTimeJitter(m_DBMTimeJitter);
  writeCdo -> setDefaultBarrelAnalogThreshold(m_BarrelAnalogThreshold);
  writeCdo -> setDefaultEndcapAnalogThreshold(m_EndcapAnalogThreshold);
  writeCdo -> setDefaultDBMAnalogThreshold(m_DBMAnalogThreshold);
  writeCdo -> setDefaultBarrelAnalogThresholdSigma(m_BarrelAnalogThresholdSigma);
  writeCdo -> setDefaultEndcapAnalogThresholdSigma(m_EndcapAnalogThresholdSigma);
  writeCdo -> setDefaultDBMAnalogThresholdSigma(m_DBMAnalogThresholdSigma);
  writeCdo -> setDefaultBarrelAnalogThresholdNoise(m_BarrelAnalogThresholdNoise);
  writeCdo -> setDefaultEndcapAnalogThresholdNoise(m_EndcapAnalogThresholdNoise);
  writeCdo -> setDefaultDBMAnalogThresholdNoise(m_DBMAnalogThresholdNoise);
  writeCdo -> setDefaultBarrelInTimeThreshold(m_BarrelInTimeThreshold);
  writeCdo -> setDefaultEndcapInTimeThreshold(m_EndcapInTimeThreshold);
  writeCdo -> setDefaultDBMInTimeThreshold(m_DBMInTimeThreshold);
  writeCdo -> setBarrelThermalNoise(m_BarrelThermalNoise);
  writeCdo -> setEndcapThermalNoise(m_EndcapThermalNoise);
  writeCdo -> setDBMThermalNoise(m_DBMThermalNoise);
  writeCdo -> setFEI4BarrelHitDiscConfig(m_FEI4BarrelHitDiscConfig);
  writeCdo -> setFEI4EndcapHitDiscConfig(m_FEI4EndcapHitDiscConfig);
  writeCdo -> setFEI4ChargScaling(m_chargeScaleFEI4);
  writeCdo -> setUseFEI4SpecialScalingFunction(m_UseFEI4SpecialScalingFunction);

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

  int currentRunNumber = ctx.eventID().run_number();
  if (currentRunNumber<222222) {
    writeCdo -> setBarrelToTThreshold(m_BarrelToTThresholdRUN1);
    writeCdo -> setFEI3BarrelLatency(m_FEI3BarrelLatencyRUN1);
    writeCdo -> setFEI3BarrelHitDuplication(m_FEI3BarrelHitDuplicationRUN1);
    writeCdo -> setFEI3BarrelSmallHitToT(m_FEI3BarrelSmallHitToTRUN1);
    writeCdo -> setFEI3BarrelTimingSimTune(m_FEI3BarrelTimingSimTuneRUN1);
    writeCdo -> setBarrelCrossTalk(m_BarrelCrossTalkRUN1);
    writeCdo -> setBarrelNoiseOccupancy(m_BarrelNoiseOccupancyRUN1);
    writeCdo -> setBarrelDisableProbability(m_BarrelDisableProbabilityRUN1);
    writeCdo -> setBarrelLorentzAngleCorr(m_BarrelLorentzAngleCorrRUN1);
    writeCdo -> setDefaultBarrelBiasVoltage(m_BarrelBiasVoltageRUN1);

    writeCdo -> setEndcapToTThreshold(m_EndcapToTThresholdRUN1);
    writeCdo -> setFEI3EndcapLatency(m_FEI3EndcapLatencyRUN1);
    writeCdo -> setFEI3EndcapHitDuplication(m_FEI3EndcapHitDuplicationRUN1);
    writeCdo -> setFEI3EndcapSmallHitToT(m_FEI3EndcapSmallHitToTRUN1);
    writeCdo -> setFEI3EndcapTimingSimTune(m_FEI3EndcapTimingSimTuneRUN1);
    writeCdo -> setEndcapCrossTalk(m_EndcapCrossTalkRUN1);
    writeCdo -> setEndcapNoiseOccupancy(m_EndcapNoiseOccupancyRUN1);
    writeCdo -> setEndcapDisableProbability(m_EndcapDisableProbabilityRUN1);
    writeCdo -> setEndcapLorentzAngleCorr(m_EndcapLorentzAngleCorrRUN1);
    writeCdo -> setDefaultEndcapBiasVoltage(m_EndcapBiasVoltageRUN1);

    // This is ad-hoc solution.
    for (size_t i=0; i<m_BLayerNoiseShapeRUN1.size(); i++) { writeCdo->setBarrelNoiseShape(0,m_BLayerNoiseShapeRUN1[i]); }
    for (size_t i=0; i<m_PixelNoiseShapeRUN1.size(); i++)  {
      for (size_t layer:{1,2}) { writeCdo->setBarrelNoiseShape(layer,m_PixelNoiseShapeRUN1[i]); }
    }

    for (size_t i=0; i<m_PixelNoiseShapeRUN1.size(); i++)  {
      for (size_t layer:{0,1,2}) { writeCdo->setEndcapNoiseShape(layer,m_PixelNoiseShapeRUN1[i]); }
    }

    // Radiation damage simulation
    writeCdo -> setFluenceLayer(m_BarrelFluenceRUN1);
    for (size_t i=0; i<m_BarrelFluenceMapRUN1.size(); i++) {
      mapsPath_list.push_back(PathResolverFindCalibFile(m_BarrelFluenceMapRUN1[i]));
    }

  }
  else if (currentRunNumber<240000) {  // RUN2 (mc15)
    writeCdo -> setBarrelToTThreshold(m_BarrelToTThreshold2016);
    writeCdo -> setFEI3BarrelLatency(m_FEI3BarrelLatency2016);
    writeCdo -> setFEI3BarrelHitDuplication(m_FEI3BarrelHitDuplication2016);
    writeCdo -> setFEI3BarrelSmallHitToT(m_FEI3BarrelSmallHitToT2016);
    writeCdo -> setFEI3BarrelTimingSimTune(m_FEI3BarrelTimingSimTune2016);
    writeCdo -> setBarrelCrossTalk(m_BarrelCrossTalk2016);
    writeCdo -> setBarrelNoiseOccupancy(m_BarrelNoiseOccupancy2016);
    writeCdo -> setBarrelDisableProbability(m_BarrelDisableProbability2016);
    writeCdo -> setBarrelLorentzAngleCorr(m_BarrelLorentzAngleCorr2016);
    writeCdo -> setDefaultBarrelBiasVoltage(m_BarrelBiasVoltage2016);

    writeCdo -> setEndcapToTThreshold(m_EndcapToTThreshold2016);
    writeCdo -> setFEI3EndcapLatency(m_FEI3EndcapLatency2016);
    writeCdo -> setFEI3EndcapHitDuplication(m_FEI3EndcapHitDuplication2016);
    writeCdo -> setFEI3EndcapSmallHitToT(m_FEI3EndcapSmallHitToT2016);
    writeCdo -> setFEI3EndcapTimingSimTune(m_FEI3EndcapTimingSimTune2016);
    writeCdo -> setEndcapCrossTalk(m_EndcapCrossTalk2016);
    writeCdo -> setEndcapNoiseOccupancy(m_EndcapNoiseOccupancy2016);
    writeCdo -> setEndcapDisableProbability(m_EndcapDisableProbability2016);
    writeCdo -> setEndcapLorentzAngleCorr(m_EndcapLorentzAngleCorr2016);
    writeCdo -> setDefaultEndcapBiasVoltage(m_EndcapBiasVoltage2016);

    writeCdo -> setDBMToTThreshold(m_DBMToTThreshold2016);
    writeCdo -> setDBMCrossTalk(m_DBMCrossTalk2016);
    writeCdo -> setDBMNoiseOccupancy(m_DBMNoiseOccupancy2016);
    writeCdo -> setDBMDisableProbability(m_DBMDisableProbability2016);
    writeCdo -> setDefaultDBMBiasVoltage(m_DBMBiasVoltage2016);

    /* 
       So far, Gaudi::Property does not support 2D vector.
       for (size_t i=0; i<m_BarrelNoiseShape.size(); i++) {
       for (size_t j=0; j<m_BarrelNoiseShape[i].size(); j++) {
       writeCdo -> setBarrelNoiseShape(i,m_BarrelNoiseShape[i][j]);
       }
       }
     */

    // This is ad-hoc solution.
    for (size_t i=0; i<m_IBLNoiseShape2016.size(); i++)    { writeCdo->setBarrelNoiseShape(0,m_IBLNoiseShape2016[i]); }
    for (size_t i=0; i<m_BLayerNoiseShape2016.size(); i++) { writeCdo->setBarrelNoiseShape(1,m_BLayerNoiseShape2016[i]); }
    for (size_t i=0; i<m_PixelNoiseShape2016.size(); i++)  {
      for (size_t layer:{2,3}) { writeCdo->setBarrelNoiseShape(layer,m_PixelNoiseShape2016[i]); }
    }

    for (size_t i=0; i<m_PixelNoiseShape2016.size(); i++)  {
      for (size_t layer:{0,1,2}) { writeCdo->setEndcapNoiseShape(layer,m_PixelNoiseShape2016[i]); }
    }

    for (size_t i=0; i<m_IBLNoiseShape2016.size(); i++)    {
      for (size_t layer:{0,1,2}) { writeCdo->setDBMNoiseShape(layer,m_IBLNoiseShape2016[i]); }
    }

    // Radiation damage simulation
    writeCdo -> setFluenceLayer(m_BarrelFluence2016);
    for (size_t i=0; i<m_BarrelFluenceMap2016.size(); i++) {
      mapsPath_list.push_back(PathResolverFindCalibFile(m_BarrelFluenceMap2016[i]));
    }

    // Radiation damage simulation for 3D sensor
    writeCdo -> setFluenceLayer3D(m_3DFluence2016);
    for (size_t i=0; i<m_3DFluenceMap2016.size(); i++) {
      mapsPath_list3D.push_back(PathResolverFindCalibFile(m_3DFluenceMap2016[i]));
    }

  }
  else if (currentRunNumber<300000) {  // RUN2 2015/2016 (mc16a)
    writeCdo -> setBarrelToTThreshold(m_BarrelToTThreshold2016);
    writeCdo -> setFEI3BarrelLatency(m_FEI3BarrelLatency2016);
    writeCdo -> setFEI3BarrelHitDuplication(m_FEI3BarrelHitDuplication2016);
    writeCdo -> setFEI3BarrelSmallHitToT(m_FEI3BarrelSmallHitToT2016);
    writeCdo -> setFEI3BarrelTimingSimTune(m_FEI3BarrelTimingSimTune2016);
    writeCdo -> setBarrelCrossTalk(m_BarrelCrossTalk2016);
    writeCdo -> setBarrelNoiseOccupancy(m_BarrelNoiseOccupancy2016);
    writeCdo -> setBarrelDisableProbability(m_BarrelDisableProbability2016);
    writeCdo -> setBarrelLorentzAngleCorr(m_BarrelLorentzAngleCorr2016);
    writeCdo -> setDefaultBarrelBiasVoltage(m_BarrelBiasVoltage2016);

    writeCdo -> setEndcapToTThreshold(m_EndcapToTThreshold2016);
    writeCdo -> setFEI3EndcapLatency(m_FEI3EndcapLatency2016);
    writeCdo -> setFEI3EndcapHitDuplication(m_FEI3EndcapHitDuplication2016);
    writeCdo -> setFEI3EndcapSmallHitToT(m_FEI3EndcapSmallHitToT2016);
    writeCdo -> setFEI3EndcapTimingSimTune(m_FEI3EndcapTimingSimTune2016);
    writeCdo -> setEndcapCrossTalk(m_EndcapCrossTalk2016);
    writeCdo -> setEndcapNoiseOccupancy(m_EndcapNoiseOccupancy2016);
    writeCdo -> setEndcapDisableProbability(m_EndcapDisableProbability2016);
    writeCdo -> setEndcapLorentzAngleCorr(m_EndcapLorentzAngleCorr2016);
    writeCdo -> setDefaultEndcapBiasVoltage(m_EndcapBiasVoltage2016);

    writeCdo -> setDBMToTThreshold(m_DBMToTThreshold2016);
    writeCdo -> setDBMCrossTalk(m_DBMCrossTalk2016);
    writeCdo -> setDBMNoiseOccupancy(m_DBMNoiseOccupancy2016);
    writeCdo -> setDBMDisableProbability(m_DBMDisableProbability2016);
    writeCdo -> setDefaultDBMBiasVoltage(m_DBMBiasVoltage2016);

    // This is ad-hoc solution.
    for (size_t i=0; i<m_IBLNoiseShape2016.size(); i++)    { writeCdo->setBarrelNoiseShape(0,m_IBLNoiseShape2016[i]); }
    for (size_t i=0; i<m_BLayerNoiseShape2016.size(); i++) { writeCdo->setBarrelNoiseShape(1,m_BLayerNoiseShape2016[i]); }
    for (size_t i=0; i<m_PixelNoiseShape2016.size(); i++)  {
      for (size_t layer:{2,3}) { writeCdo->setBarrelNoiseShape(layer,m_PixelNoiseShape2016[i]); }
    }

    for (size_t i=0; i<m_PixelNoiseShape2016.size(); i++)  {
      for (size_t layer:{0,1,2}) { writeCdo->setEndcapNoiseShape(layer,m_PixelNoiseShape2016[i]); }
    }

    for (size_t i=0; i<m_IBLNoiseShape2016.size(); i++)    {
      for (size_t layer:{0,1,2}) { writeCdo->setDBMNoiseShape(layer,m_IBLNoiseShape2016[i]); }
    }

    // Radiation damage simulation
    writeCdo -> setFluenceLayer(m_BarrelFluence2016);
    for (size_t i=0; i<m_BarrelFluenceMap2016.size(); i++) {
      mapsPath_list.push_back(PathResolverFindCalibFile(m_BarrelFluenceMap2016[i]));
    }

    // Radiation damage simulation for 3D sensor
    writeCdo -> setFluenceLayer3D(m_3DFluence2016);
    for (size_t i=0; i<m_3DFluenceMap2016.size(); i++) {
      mapsPath_list3D.push_back(PathResolverFindCalibFile(m_3DFluenceMap2016[i]));
    }

  }
  else if (currentRunNumber<310000) {  // RUN2 2017 (mc16d)
    writeCdo -> setBarrelToTThreshold(m_BarrelToTThreshold2017);
    writeCdo -> setFEI3BarrelLatency(m_FEI3BarrelLatency2017);
    writeCdo -> setFEI3BarrelHitDuplication(m_FEI3BarrelHitDuplication2017);
    writeCdo -> setFEI3BarrelSmallHitToT(m_FEI3BarrelSmallHitToT2017);
    writeCdo -> setFEI3BarrelTimingSimTune(m_FEI3BarrelTimingSimTune2017);
    writeCdo -> setBarrelCrossTalk(m_BarrelCrossTalk2017);
    writeCdo -> setBarrelNoiseOccupancy(m_BarrelNoiseOccupancy2017);
    writeCdo -> setBarrelDisableProbability(m_BarrelDisableProbability2017);
    writeCdo -> setBarrelLorentzAngleCorr(m_BarrelLorentzAngleCorr2017);
    writeCdo -> setDefaultBarrelBiasVoltage(m_BarrelBiasVoltage2017);

    writeCdo -> setEndcapToTThreshold(m_EndcapToTThreshold2017);
    writeCdo -> setFEI3EndcapLatency(m_FEI3EndcapLatency2017);
    writeCdo -> setFEI3EndcapHitDuplication(m_FEI3EndcapHitDuplication2017);
    writeCdo -> setFEI3EndcapSmallHitToT(m_FEI3EndcapSmallHitToT2017);
    writeCdo -> setFEI3EndcapTimingSimTune(m_FEI3EndcapTimingSimTune2017);
    writeCdo -> setEndcapCrossTalk(m_EndcapCrossTalk2017);
    writeCdo -> setEndcapNoiseOccupancy(m_EndcapNoiseOccupancy2017);
    writeCdo -> setEndcapDisableProbability(m_EndcapDisableProbability2017);
    writeCdo -> setEndcapLorentzAngleCorr(m_EndcapLorentzAngleCorr2017);
    writeCdo -> setDefaultEndcapBiasVoltage(m_EndcapBiasVoltage2017);

    writeCdo -> setDBMToTThreshold(m_DBMToTThreshold2017);
    writeCdo -> setDBMCrossTalk(m_DBMCrossTalk2017);
    writeCdo -> setDBMNoiseOccupancy(m_DBMNoiseOccupancy2017);
    writeCdo -> setDBMDisableProbability(m_DBMDisableProbability2017);
    writeCdo -> setDefaultDBMBiasVoltage(m_DBMBiasVoltage2017);

    // This is ad-hoc solution.
    for (size_t i=0; i<m_IBLNoiseShape2017.size(); i++)    { writeCdo->setBarrelNoiseShape(0,m_IBLNoiseShape2017[i]); }
    for (size_t i=0; i<m_BLayerNoiseShape2017.size(); i++) { writeCdo->setBarrelNoiseShape(1,m_BLayerNoiseShape2017[i]); }
    for (size_t i=0; i<m_PixelNoiseShape2017.size(); i++)  {
      for (size_t layer:{2,3}) { writeCdo->setBarrelNoiseShape(layer,m_PixelNoiseShape2017[i]); }
    }

    for (size_t i=0; i<m_PixelNoiseShape2017.size(); i++)  {
      for (size_t layer:{0,1,2}) { writeCdo->setEndcapNoiseShape(layer,m_PixelNoiseShape2017[i]); }
    }

    for (size_t i=0; i<m_IBLNoiseShape2017.size(); i++)    {
      for (size_t layer:{0,1,2}) { writeCdo->setDBMNoiseShape(layer,m_IBLNoiseShape2017[i]); }
    }

    // Radiation damage simulation
    writeCdo -> setFluenceLayer(m_BarrelFluence2017);
    for (size_t i=0; i<m_BarrelFluenceMap2017.size(); i++) {
      mapsPath_list.push_back(PathResolverFindCalibFile(m_BarrelFluenceMap2017[i]));
    }

    // Radiation damage simulation for 3D sensor
    writeCdo -> setFluenceLayer3D(m_3DFluence2017);
    for (size_t i=0; i<m_3DFluenceMap2017.size(); i++) {
      mapsPath_list3D.push_back(PathResolverFindCalibFile(m_3DFluenceMap2017[i]));
    }

  }
  else if (currentRunNumber<330000) {  // RUN2 2018 (mc16e)
    writeCdo -> setBarrelToTThreshold(m_BarrelToTThreshold2018);
    writeCdo -> setFEI3BarrelLatency(m_FEI3BarrelLatency2018);
    writeCdo -> setFEI3BarrelHitDuplication(m_FEI3BarrelHitDuplication2018);
    writeCdo -> setFEI3BarrelSmallHitToT(m_FEI3BarrelSmallHitToT2018);
    writeCdo -> setFEI3BarrelTimingSimTune(m_FEI3BarrelTimingSimTune2018);
    writeCdo -> setBarrelCrossTalk(m_BarrelCrossTalk2018);
    writeCdo -> setBarrelNoiseOccupancy(m_BarrelNoiseOccupancy2018);
    writeCdo -> setBarrelDisableProbability(m_BarrelDisableProbability2018);
    writeCdo -> setBarrelLorentzAngleCorr(m_BarrelLorentzAngleCorr2018);
    writeCdo -> setDefaultBarrelBiasVoltage(m_BarrelBiasVoltage2018);

    writeCdo -> setEndcapToTThreshold(m_EndcapToTThreshold2018);
    writeCdo -> setFEI3EndcapLatency(m_FEI3EndcapLatency2018);
    writeCdo -> setFEI3EndcapHitDuplication(m_FEI3EndcapHitDuplication2018);
    writeCdo -> setFEI3EndcapSmallHitToT(m_FEI3EndcapSmallHitToT2018);
    writeCdo -> setFEI3EndcapTimingSimTune(m_FEI3EndcapTimingSimTune2018);
    writeCdo -> setEndcapCrossTalk(m_EndcapCrossTalk2018);
    writeCdo -> setEndcapNoiseOccupancy(m_EndcapNoiseOccupancy2018);
    writeCdo -> setEndcapDisableProbability(m_EndcapDisableProbability2018);
    writeCdo -> setEndcapLorentzAngleCorr(m_EndcapLorentzAngleCorr2018);
    writeCdo -> setDefaultEndcapBiasVoltage(m_EndcapBiasVoltage2018);

    writeCdo -> setDBMToTThreshold(m_DBMToTThreshold2018);
    writeCdo -> setDBMCrossTalk(m_DBMCrossTalk2018);
    writeCdo -> setDBMNoiseOccupancy(m_DBMNoiseOccupancy2018);
    writeCdo -> setDBMDisableProbability(m_DBMDisableProbability2018);
    writeCdo -> setDefaultDBMBiasVoltage(m_DBMBiasVoltage2018);

    // This is ad-hoc solution.
    for (size_t i=0; i<m_IBLNoiseShape2018.size(); i++)    { writeCdo->setBarrelNoiseShape(0,m_IBLNoiseShape2018[i]); }
    for (size_t i=0; i<m_BLayerNoiseShape2018.size(); i++) { writeCdo->setBarrelNoiseShape(1,m_BLayerNoiseShape2018[i]); }
    for (size_t i=0; i<m_PixelNoiseShape2018.size(); i++)  {
      for (size_t layer:{2,3}) { writeCdo->setBarrelNoiseShape(layer,m_PixelNoiseShape2018[i]); }
    }

    for (size_t i=0; i<m_PixelNoiseShape2018.size(); i++)  {
      for (size_t layer:{0,1,2}) { writeCdo->setEndcapNoiseShape(layer,m_PixelNoiseShape2018[i]); }
    }

    for (size_t i=0; i<m_IBLNoiseShape2018.size(); i++)    {
      for (size_t layer:{0,1,2}) { writeCdo->setDBMNoiseShape(layer,m_IBLNoiseShape2018[i]); }
    }

    // Radiation damage simulation
    writeCdo -> setFluenceLayer(m_BarrelFluence2018);
    for (size_t i=0; i<m_BarrelFluenceMap2018.size(); i++) {
      mapsPath_list.push_back(PathResolverFindCalibFile(m_BarrelFluenceMap2018[i]));
    }

    // Radiation damage simulation for 3D sensor
    writeCdo -> setFluenceLayer3D(m_3DFluence2018);
    for (size_t i=0; i<m_3DFluenceMap2018.size(); i++) {
      mapsPath_list3D.push_back(PathResolverFindCalibFile(m_3DFluenceMap2018[i]));
    }
  }
  else {  // RUN3 2022
    writeCdo -> setBarrelToTThreshold(m_BarrelToTThreshold2022);
    writeCdo -> setFEI3BarrelLatency(m_FEI3BarrelLatency2022);
    writeCdo -> setFEI3BarrelHitDuplication(m_FEI3BarrelHitDuplication2022);
    writeCdo -> setFEI3BarrelSmallHitToT(m_FEI3BarrelSmallHitToT2022);
    writeCdo -> setFEI3BarrelTimingSimTune(m_FEI3BarrelTimingSimTune2022);
    writeCdo -> setBarrelCrossTalk(m_BarrelCrossTalk2022);
    writeCdo -> setBarrelNoiseOccupancy(m_BarrelNoiseOccupancy2022);
    writeCdo -> setBarrelDisableProbability(m_BarrelDisableProbability2022);
    writeCdo -> setBarrelLorentzAngleCorr(m_BarrelLorentzAngleCorr2022);
    writeCdo -> setDefaultBarrelBiasVoltage(m_BarrelBiasVoltage2022);

    writeCdo -> setEndcapToTThreshold(m_EndcapToTThreshold2022);
    writeCdo -> setFEI3EndcapLatency(m_FEI3EndcapLatency2022);
    writeCdo -> setFEI3EndcapHitDuplication(m_FEI3EndcapHitDuplication2022);
    writeCdo -> setFEI3EndcapSmallHitToT(m_FEI3EndcapSmallHitToT2022);
    writeCdo -> setFEI3EndcapTimingSimTune(m_FEI3EndcapTimingSimTune2022);
    writeCdo -> setEndcapCrossTalk(m_EndcapCrossTalk2022);
    writeCdo -> setEndcapNoiseOccupancy(m_EndcapNoiseOccupancy2022);
    writeCdo -> setEndcapDisableProbability(m_EndcapDisableProbability2022);
    writeCdo -> setEndcapLorentzAngleCorr(m_EndcapLorentzAngleCorr2022);
    writeCdo -> setDefaultEndcapBiasVoltage(m_EndcapBiasVoltage2022);

    writeCdo -> setDBMToTThreshold(m_DBMToTThreshold2022);
    writeCdo -> setDBMCrossTalk(m_DBMCrossTalk2022);
    writeCdo -> setDBMNoiseOccupancy(m_DBMNoiseOccupancy2022);
    writeCdo -> setDBMDisableProbability(m_DBMDisableProbability2022);
    writeCdo -> setDefaultDBMBiasVoltage(m_DBMBiasVoltage2022);

    // This is ad-hoc solution.
    for (size_t i=0; i<m_IBLNoiseShape2022.size(); i++)    { writeCdo->setBarrelNoiseShape(0,m_IBLNoiseShape2022[i]); }
    for (size_t i=0; i<m_BLayerNoiseShape2022.size(); i++) { writeCdo->setBarrelNoiseShape(1,m_BLayerNoiseShape2022[i]); }
    for (size_t i=0; i<m_PixelNoiseShape2022.size(); i++)  {
      for (size_t layer:{2,3}) { writeCdo->setBarrelNoiseShape(layer,m_PixelNoiseShape2022[i]); }
    }

    for (size_t i=0; i<m_PixelNoiseShape2022.size(); i++)  {
      for (size_t layer:{0,1,2}) { writeCdo->setEndcapNoiseShape(layer,m_PixelNoiseShape2022[i]); }
    }

    for (size_t i=0; i<m_IBLNoiseShape2022.size(); i++)    {
      for (size_t layer:{0,1,2}) { writeCdo->setDBMNoiseShape(layer,m_IBLNoiseShape2022[i]); }
    }

    // Radiation damage simulation
    writeCdo -> setFluenceLayer(m_BarrelFluence2022);
    for (size_t i=0; i<m_BarrelFluenceMap2022.size(); i++) {
      mapsPath_list.push_back(PathResolverFindCalibFile(m_BarrelFluenceMap2022[i]));
    }

    // Radiation damage simulation for 3D sensor
    writeCdo -> setFluenceLayer3D(m_3DFluence2022);
    for (size_t i=0; i<m_3DFluenceMap2022.size(); i++) {
      mapsPath_list3D.push_back(PathResolverFindCalibFile(m_3DFluenceMap2022[i]));
    }

  }

  writeCdo -> setRadSimFluenceMapList(mapsPath_list);
  writeCdo -> setRadSimFluenceMapList3D(mapsPath_list3D);

  //=======================
  // Combine time interval
  //=======================
  EventIDRange rangeW{start, stop};
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


