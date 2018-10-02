//Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileEMScaleCondAlg.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileIdentifier/TileRawChannelUnit.h"
#include "TileConditions/TileCablingService.h"
#include "TileConditions/TileCablingSvc.h"


#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/WriteCondHandle.h"

TileEMScaleCondAlg::TileEMScaleCondAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_condSvc("CondSvc", name),
  m_useOflLasFib(false),
  m_maxChannels(0),
  m_maxGains(0),
  m_onlCacheUnit(TileRawChannelUnit::Invalid)
{
}


TileEMScaleCondAlg::~TileEMScaleCondAlg() {
}


StatusCode TileEMScaleCondAlg::initialize() {

  ATH_MSG_DEBUG( "In initialize()" );

  //=== Configure online cache unit
  if (m_onlCacheUnitString == "Invalid") {
    m_onlCacheUnit = TileRawChannelUnit::Invalid;
  } else if (m_onlCacheUnitString == "OnlineADCcounts") {
    m_onlCacheUnit = TileRawChannelUnit::OnlineADCcounts;
  } else if (m_onlCacheUnitString == "OnlinePicoCoulombs") {
    m_onlCacheUnit = TileRawChannelUnit::OnlinePicoCoulombs;
  } else if (m_onlCacheUnitString == "OnlineCesiumPicoCoulombs") {
    m_onlCacheUnit = TileRawChannelUnit::OnlineCesiumPicoCoulombs;
  } else if (m_onlCacheUnitString == "OnlineMegaElectronVolts") {
    m_onlCacheUnit = TileRawChannelUnit::OnlineMegaElectronVolts;
  } else {
    ATH_MSG_WARNING( "Unrecoginzed jobOption OnlCacheUnit=" << m_onlCacheUnitString << ". "
                     << "Setting it to \"Invalid\"!" );
    m_onlCacheUnit = TileRawChannelUnit::Invalid;
  }


  // CondSvc
  ATH_CHECK( m_condSvc.retrieve() );

  ATH_CHECK( m_oflCisLinProxy.retrieve() );
  ATH_CHECK( m_oflCisNlnProxy.retrieve() );

  ATH_CHECK( m_oflLasLinProxy.retrieve() );
  ATH_CHECK( m_oflLasNlnProxy.retrieve() );

  m_useOflLasFib = !(m_oflLasFibProxy.empty());

  if (m_useOflLasFib) {
    //=== retrieve offline proxy
    CHECK( m_oflLasFibProxy.retrieve() );
    ATH_MSG_INFO("ProxyOflLasFib is set up and can be used");
  } else {
    ATH_MSG_INFO("ProxyOflLasFib is not set up and cannot be used");
    m_oflLasFibProxy.disable();
  }

  ATH_CHECK( m_oflCesProxy.retrieve() );
  ATH_CHECK( m_oflEmsProxy.retrieve() );

  if (m_onlCacheUnit != TileRawChannelUnit::Invalid) {

    if (m_onlCisProxy.empty() || m_onlLasProxy.empty()
        || m_onlCesProxy.empty() || m_onlEmsProxy.empty()) {

      ATH_MSG_ERROR( "Requested valid OnlCacheUnit (" << m_onlCacheUnitString << ") "
                     << "but at least one Onl{Cis|Las|Ces|Ems}Proxy is not set up!" );
      return StatusCode::FAILURE;
    }

    ATH_CHECK( m_onlCisProxy.retrieve() );
    ATH_CHECK( m_onlLasProxy.retrieve() );
    ATH_CHECK( m_onlCesProxy.retrieve() );
    ATH_CHECK( m_onlEmsProxy.retrieve() );


    //=== Resize onlCache to desired size
    ServiceHandle<TileCablingSvc> cablingSvc("TileCablingSvc", name());
    ATH_CHECK( cablingSvc.retrieve());

    const TileCablingService* cabling = cablingSvc->cablingService();
    if (!cabling) {
      ATH_MSG_ERROR( "Unable to retrieve TileCablingService" );
      return StatusCode::FAILURE;
    }


    m_maxChannels = cabling->getMaxChannels();
    m_maxGains = cabling->getMaxGains();

  } else {

    ATH_MSG_INFO( "Loading of online calibration constants is not requested, "
                  << "since OnlCacheUnit=" << m_onlCacheUnitString  );

    m_onlCisProxy.disable();
    m_onlLasProxy.disable();
    m_onlCesProxy.disable();
    m_onlEmsProxy.disable();
  }


  ATH_CHECK( m_calibEmsKey.initialize() );

  // Register write handle
  if (m_condSvc->regHandle(this, m_calibEmsKey).isFailure()) {
    ATH_MSG_ERROR("unable to register WriteCondHandle " << m_calibEmsKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }



  return StatusCode::SUCCESS;
}


StatusCode TileEMScaleCondAlg::execute() {

  SG::WriteCondHandle<TileEMScale> calibEms{m_calibEmsKey};

  if (calibEms.isValid()) {
    ATH_MSG_DEBUG("Found valid TileCalibEms: " << calibEms.key());
    return StatusCode::SUCCESS;
  }


  std::unique_ptr<TileEMScale> emsData = std::make_unique<TileEMScale>();

  emsData->setMaxChannels(m_maxChannels);
  emsData->setMaxGains(m_maxGains);
  emsData->setOnlineCacheUnit(m_onlCacheUnit);


  // Get offline CIS linear calibration constants
  EventIDRange oflCisLinRange;
  std::unique_ptr<TileCalibDataFlt> calibOflCisLin = std::make_unique<TileCalibDataFlt>();

  ATH_CHECK( m_oflCisLinProxy->fillCalibData(*calibOflCisLin, oflCisLinRange) );
  emsData->setCalibOflCisLin(std::move(calibOflCisLin));

  // Get offline CIS non-linear calibration constants
  EventIDRange oflCisNlnRange;
  std::unique_ptr<TileCalibDataFlt> calibOflCisNln = std::make_unique<TileCalibDataFlt>();

  ATH_CHECK( m_oflCisNlnProxy->fillCalibData(*calibOflCisNln, oflCisNlnRange) );
  emsData->setCalibOflCisNln(std::move(calibOflCisNln));


  // Get offline Laser linear calibration constants
  EventIDRange oflLasLinRange;
  std::unique_ptr<TileCalibDataFlt> calibOflLasLin = std::make_unique<TileCalibDataFlt>();

  ATH_CHECK( m_oflLasLinProxy->fillCalibData(*calibOflLasLin, oflLasLinRange) );
  emsData->setCalibOflLasLin(std::move(calibOflLasLin));


  // Get offline Laser non-linear calibration constants
  EventIDRange oflLasNlnRange;
  std::unique_ptr<TileCalibDataFlt> calibOflLasNln = std::make_unique<TileCalibDataFlt>();

  ATH_CHECK( m_oflLasNlnProxy->fillCalibData(*calibOflLasNln, oflLasNlnRange) );
  emsData->setCalibOflLasNln(std::move(calibOflLasNln));


  // Get offline Ces calibration constants
  EventIDRange oflCesRange;
  std::unique_ptr<TileCalibDataFlt> calibOflCes = std::make_unique<TileCalibDataFlt>();

  ATH_CHECK( m_oflCesProxy->fillCalibData(*calibOflCes, oflCesRange) );
  emsData->setCalibOflCes(std::move(calibOflCes));


  // Get offline Ems calibration constants
  EventIDRange oflEmsRange;
  std::unique_ptr<TileCalibDataFlt> calibOflEms = std::make_unique<TileCalibDataFlt>();

  ATH_CHECK( m_oflEmsProxy->fillCalibData(*calibOflEms, oflEmsRange) );
  emsData->setCalibOflEms(std::move(calibOflEms));


  EventIDRange eventRange = EventIDRange::intersect(oflCisLinRange, oflCisNlnRange,
                                                    oflLasLinRange, oflLasNlnRange,
                                                    oflCesRange, oflEmsRange);


  if (m_useOflLasFib) {
    // Get offline Laser fiber calibration constants
    EventIDRange oflLasFibRange;
    std::unique_ptr<TileCalibDataFlt> calibOflLasFib = std::make_unique<TileCalibDataFlt>();

    ATH_CHECK( m_oflLasFibProxy->fillCalibData(*calibOflLasFib, oflLasFibRange) );
    emsData->setCalibOflLasFib(std::move(calibOflLasFib));

    eventRange = EventIDRange::intersect(eventRange, oflLasFibRange);
  }


  if (m_onlCacheUnit != TileRawChannelUnit::Invalid) {
    // Get online CIS calibration constants
    EventIDRange onlCisRange;
    std::unique_ptr<TileCalibDataFlt> calibOnlCis = std::make_unique<TileCalibDataFlt>();

    ATH_CHECK( m_onlCisProxy->fillCalibData(*calibOnlCis, onlCisRange) );
    emsData->setCalibOnlCis(std::move(calibOnlCis));

    // Get online Laser calibration constants
    EventIDRange onlLasRange;
    std::unique_ptr<TileCalibDataFlt> calibOnlLas = std::make_unique<TileCalibDataFlt>();

    ATH_CHECK( m_onlLasProxy->fillCalibData(*calibOnlLas, onlLasRange) );
    emsData->setCalibOnlLas(std::move(calibOnlLas));


    // Get online Ces calibration constants
    EventIDRange onlCesRange;
    std::unique_ptr<TileCalibDataFlt> calibOnlCes = std::make_unique<TileCalibDataFlt>();

    ATH_CHECK( m_onlCesProxy->fillCalibData(*calibOnlCes, onlCesRange) );
    emsData->setCalibOnlCes(std::move(calibOnlCes));


    // Get online Ems calibration constants
    EventIDRange onlEmsRange;
    std::unique_ptr<TileCalibDataFlt> calibOnlEms = std::make_unique<TileCalibDataFlt>();

    ATH_CHECK( m_onlEmsProxy->fillCalibData(*calibOnlEms, onlEmsRange) );
    emsData->setCalibOnlEms(std::move(calibOnlEms));

    eventRange = EventIDRange::intersect(eventRange, onlCisRange, onlLasRange,
                                         onlCesRange, onlEmsRange);
  }


  if(eventRange.start() > eventRange.stop()) {
    ATH_MSG_ERROR("Invalid intersection range: " << eventRange);
    return StatusCode::FAILURE;
  }

  if (emsData->initialize()) {
    ATH_MSG_DEBUG("TileCalibEms object is initialized successfully.");
  } else {
    ATH_MSG_ERROR("Impossible to inizialize TileCalibEms object!");
    return StatusCode::FAILURE;
  }

  if(calibEms.record(eventRange, emsData.release()).isFailure()) {
    ATH_MSG_ERROR("Could not record TileCalibEms object with "
                  << calibEms.key()
                  << " with EventRange " << eventRange
                  << " into Conditions Store");
    return StatusCode::FAILURE;
  } else {

    ATH_MSG_VERBOSE("Recorded TileCalibEms object with "
                   << calibEms.key()
                   << " with EventRange " << eventRange
                   << " into Conditions Store");
  }

  return StatusCode::SUCCESS;

}


StatusCode TileEMScaleCondAlg::finalize() {
  return StatusCode::SUCCESS;
}
