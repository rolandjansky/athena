/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelConfigCondAlg.h"
#include "Identifier/IdentifierHash.h"
#include "GaudiKernel/EventIDRange.h"
#include <memory>
#include <sstream>

PixelConfigCondAlg::PixelConfigCondAlg(const std::string& name, ISvcLocator* pSvcLocator):
  ::AthAlgorithm(name, pSvcLocator),
  m_BarrelAnalogThreshold({-1,-1,-1,-1,-1,-1,-1}),
  m_EndcapAnalogThreshold({-1,-1,-1,-1,-1,-1,-1}),
  m_DBMAnalogThreshold({-1,-1,-1,-1,-1,-1,-1}),
  m_BarrelToTThreshold({-1,-1,-1,-1,-1,-1,-1}),
  m_EndcapToTThreshold({-1,-1,-1,-1,-1,-1,-1}),
  m_DBMToTThreshold({-1,-1,-1,-1,-1,-1,-1}),
  m_BarrelLatency({256,256,256,256,256,256,256}),
  m_EndcapLatency({256,256,256,256,256,256,256}),
  m_DBMLatency({256,256,256,256,256,256,256}),
  m_BarrelCrossTalk({0.06,0.06,0.06,0.06,0.06,0.06,0.06}),
  m_EndcapCrossTalk({0.06,0.06,0.06,0.06,0.06,0.06,0.06}),
  m_DBMCrossTalk({0.06,0.06,0.06,0.06,0.06,0.06,0.06}),
  m_BarrelThermalNoise({160.0,160.0,160.0,160.0,160.0,160.0,160.0}),
  m_EndcapThermalNoise({160.0,160.0,160.0,160.0,160.0,160.0,160.0}),
  m_DBMThermalNoise({160.0,160.0,160.0,160.0,160.0,160.0,160.0}),
  m_BarrelHitDuplication({false,false,false,false,false,false,false}),
  m_EndcapHitDuplication({false,false,false,false,false,false,false}),
  m_DBMHitDuplication({false,false,false,false,false,false,false}),
  m_BarrelSmallHitToT({-1,-1,-1,-1,-1,-1,-1}),
  m_EndcapSmallHitToT({-1,-1,-1,-1,-1,-1,-1}),
  m_DBMSmallHitToT({-1,-1,-1,-1,-1,-1,-1}),
  m_IBLHitDisConfig(2),
  m_condSvc("CondSvc", name)
{
  declareProperty("BarrelAnalogThreshold", m_BarrelAnalogThreshold);
  declareProperty("EndcapAnalogThreshold", m_EndcapAnalogThreshold);
  declareProperty("DBMAnalogThreshold",    m_DBMAnalogThreshold);
  declareProperty("BarrelToTThreshold",    m_BarrelToTThreshold);
  declareProperty("EndcapToTThreshold",    m_EndcapToTThreshold);
  declareProperty("DBMToTThreshold",       m_DBMToTThreshold);
  declareProperty("BarrelLatency",         m_BarrelLatency);
  declareProperty("EndcapLatency",         m_EndcapLatency);
  declareProperty("DBMLatency",            m_DBMLatency);
  declareProperty("BarrelCrossTalk",       m_BarrelCrossTalk);
  declareProperty("EndcapCrossTalk",       m_EndcapCrossTalk);
  declareProperty("DBMCrossTalk",          m_DBMCrossTalk);
  declareProperty("BarrelThermalNoise",    m_BarrelThermalNoise);
  declareProperty("EndcapThermalNoise",    m_EndcapThermalNoise);
  declareProperty("DBMThermalNoise",       m_DBMThermalNoise);
  declareProperty("BarrelHitDuplication",  m_BarrelHitDuplication);
  declareProperty("EndcapHitDuplication",  m_EndcapHitDuplication);
  declareProperty("DBMHitDuplication",     m_DBMHitDuplication);
  declareProperty("BarrelSmallHitToT",     m_BarrelSmallHitToT);
  declareProperty("EndcapSmallHitToT",     m_EndcapSmallHitToT);
  declareProperty("DBMSmallHitToT",        m_DBMSmallHitToT);
  declareProperty("FEI4HitDiscConfig",     m_IBLHitDisConfig);
}

StatusCode PixelConfigCondAlg::initialize() {
  ATH_MSG_DEBUG("PixelConfigCondAlg::initialize()");

  ATH_CHECK(m_condSvc.retrieve());

  ATH_CHECK(m_writeKey.initialize());
  if (m_condSvc->regHandle(this,m_writeKey).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  if (m_IBLHitDisConfig<0 || m_IBLHitDisConfig>3) {
    ATH_MSG_FATAL("HitDiscConfig should be set [0-3].  (FEI4HitDiscConfig=" << m_IBLHitDisConfig << ")");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode PixelConfigCondAlg::execute() {
  ATH_MSG_DEBUG("PixelConfigCondAlg::execute()");

  SG::WriteCondHandle<PixelModuleData> writeHandle(m_writeKey);
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid.. In theory this should not be called, but may happen if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS; 
  }

  const EventIDBase start{EventIDBase::UNDEFNUM, EventIDBase::UNDEFEVT, 0,                       0,                       EventIDBase::UNDEFNUM, EventIDBase::UNDEFNUM};
  const EventIDBase stop {EventIDBase::UNDEFNUM, EventIDBase::UNDEFEVT, EventIDBase::UNDEFNUM-1, EventIDBase::UNDEFNUM-1, EventIDBase::UNDEFNUM, EventIDBase::UNDEFNUM};
  const EventIDRange rangeW{start, stop};

  ATH_MSG_DEBUG("Range of input is " << rangeW);
 
  // Construct the output Cond Object and fill it in
  std::unique_ptr<PixelModuleData> writeCdo(std::make_unique<PixelModuleData>());

  writeCdo -> setBarrelAnalogThreshold(m_BarrelAnalogThreshold);
  writeCdo -> setEndcapAnalogThreshold(m_EndcapAnalogThreshold);
  writeCdo -> setDBMAnalogThreshold(m_DBMAnalogThreshold);
  writeCdo -> setBarrelToTThreshold(m_BarrelToTThreshold);
  writeCdo -> setEndcapToTThreshold(m_EndcapToTThreshold);
  writeCdo -> setDBMToTThreshold(m_DBMToTThreshold);
  writeCdo -> setBarrelLatency(m_BarrelLatency);
  writeCdo -> setEndcapLatency(m_EndcapLatency);
  writeCdo -> setDBMLatency(m_DBMLatency);
  writeCdo -> setBarrelCrossTalk(m_BarrelCrossTalk);
  writeCdo -> setEndcapCrossTalk(m_EndcapCrossTalk);
  writeCdo -> setDBMCrossTalk(m_DBMCrossTalk);
  writeCdo -> setBarrelThermalNoise(m_BarrelThermalNoise);
  writeCdo -> setEndcapThermalNoise(m_EndcapThermalNoise);
  writeCdo -> setDBMThermalNoise(m_DBMThermalNoise);
  writeCdo -> setBarrelHitDuplication(m_BarrelHitDuplication);
  writeCdo -> setEndcapHitDuplication(m_EndcapHitDuplication);
  writeCdo -> setDBMHitDuplication(m_DBMHitDuplication);
  writeCdo -> setBarrelSmallHitToT(m_BarrelSmallHitToT);
  writeCdo -> setEndcapSmallHitToT(m_EndcapSmallHitToT);
  writeCdo -> setDBMSmallHitToT(m_DBMSmallHitToT);
  writeCdo -> setIBLHitDiscConfig(m_IBLHitDisConfig);

  if (writeHandle.record(rangeW, std::move(writeCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record PixelModuleData " << writeHandle.key() << " with EventRange " << rangeW << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("recorded new CDO " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

  return StatusCode::SUCCESS;
}

StatusCode PixelConfigCondAlg::finalize() {
  ATH_MSG_DEBUG("PixelConfigCondAlg::finalize()");
  return StatusCode::SUCCESS;
}

