/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Tile includes
#include "TileConditions/TileCondToolEmscale.h"
#include "TileConditions/TileCablingService.h"
#include "TileConditions/TileCablingSvc.h"

#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileCalibBlobObjs/Exception.h"
#include "TileConditions/TileCondProxyWrapper.h"

// Athena includes
#include "AthenaKernel/errorcheck.h"


//
//____________________________________________________________________
static const InterfaceID IID_TileCondToolEmscale("TileCondToolEmscale", 1, 0);

const InterfaceID& TileCondToolEmscale::interfaceID() {
  return IID_TileCondToolEmscale;
}

//
//____________________________________________________________________
TileCondToolEmscale::TileCondToolEmscale(const std::string& type, const std::string& name,
    const IInterface* parent)
    : AthAlgTool(type, name, parent)
    , m_onlCacheUnit( TileRawChannelUnit::Invalid)
    , m_onlCache(std::vector<float>(0))
    , m_OflLasLinUsed(true)
    , m_OflLasNlnUsed(true)
    //=== Offline folders are by default read from ASCII file
    , m_pryOflCisLin("TileCondProxyFile_TileCalibDrawerFlt_/TileCondProxyDefault_OflCisLin", this)
    , m_pryOflCisNln("TileCondProxyFile_TileCalibDrawerFlt_/TileCondProxyDefault_OflCisNln", this)
    , m_pryOflLasLin("TileCondProxyFile_TileCalibDrawerFlt_/TileCondProxyDefault_OflLasLin", this)
    , m_pryOflLasNln("TileCondProxyFile_TileCalibDrawerFlt_/TileCondProxyDefault_OflLasNln", this)
    , m_pryOflLasFib("TileCondProxyFile_TileCalibDrawerFlt_/TileCondProxyDefault_OflLasFib", this)
    , m_pryOflCes("TileCondProxyFile_TileCalibDrawerFlt_/TileCondProxyDefault_OflCes", this)
    , m_pryOflEms("TileCondProxyFile_TileCalibDrawerFlt_/TileCondProxyDefault_OflEms", this)
    //=== Online folders can only be read from the COOL DB
    , m_pryOnlCis("TileCondProxyCool_TileCalibDrawerFlt_/TileCondProxyDefault_OnlCis", this)
    , m_pryOnlLas("TileCondProxyCool_TileCalibDrawerFlt_/TileCondProxyDefault_OnlLas", this)
    , m_pryOnlCes("TileCondProxyCool_TileCalibDrawerFlt_/TileCondProxyDefault_OnlCes", this)
    , m_pryOnlEms("TileCondProxyCool_TileCalibDrawerFlt_/TileCondProxyDefault_OnlEms", this)
    , m_useOflLasFib(false)
    , m_maxChannels(0)
    , m_maxGains(0)
    , m_drawerCacheSize(0)
{
  declareInterface<TileCondToolEmscale>(this);

  declareProperty("OnlCacheUnit", m_onlCacheUnitStr = "Invalid");
  //=== "offline" proxies
  declareProperty("ProxyOflCisLin", m_pryOflCisLin);
  declareProperty("ProxyOflCisNln", m_pryOflCisNln);
  declareProperty("ProxyOflLasLin", m_pryOflLasLin);
  declareProperty("ProxyOflLasNln", m_pryOflLasNln);
  declareProperty("ProxyOflLasFib", m_pryOflLasFib);
  declareProperty("ProxyOflCes", m_pryOflCes);
  declareProperty("ProxyOflEms", m_pryOflEms);
  //=== "Online" proxies
  declareProperty("ProxyOnlCis", m_pryOnlCis);
  declareProperty("ProxyOnlLas", m_pryOnlLas);
  declareProperty("ProxyOnlCes", m_pryOnlCes);
  declareProperty("ProxyOnlEms", m_pryOnlEms);
}

//
//____________________________________________________________________
TileCondToolEmscale::~TileCondToolEmscale() {
}

//
//____________________________________________________________________
StatusCode TileCondToolEmscale::initialize() {

  ATH_MSG_DEBUG( "In initialize()" );

  //=== Configure online cache unit
  if (m_onlCacheUnitStr == "Invalid") {
    m_onlCacheUnit = TileRawChannelUnit::Invalid;
  } else if (m_onlCacheUnitStr == "OnlineADCcounts") {
    m_onlCacheUnit = TileRawChannelUnit::OnlineADCcounts;
  } else if (m_onlCacheUnitStr == "OnlinePicoCoulombs") {
    m_onlCacheUnit = TileRawChannelUnit::OnlinePicoCoulombs;
  } else if (m_onlCacheUnitStr == "OnlineCesiumPicoCoulombs") {
    m_onlCacheUnit = TileRawChannelUnit::OnlineCesiumPicoCoulombs;
  } else if (m_onlCacheUnitStr == "OnlineMegaElectronVolts") {
    m_onlCacheUnit = TileRawChannelUnit::OnlineMegaElectronVolts;
  } else {
    ATH_MSG_WARNING( "Unrecoginzed jobOption OnlCacheUnit=" << m_onlCacheUnitStr << ". "
                     << "Setting it to \"Invalid\"!" );
    m_onlCacheUnit = TileRawChannelUnit::Invalid;
  }

  //=== Retrieve offline proxies
  CHECK( m_pryOflCisLin.retrieve() );
  CHECK( m_pryOflCisNln.retrieve() );
  CHECK( m_pryOflLasLin.retrieve() );
  CHECK( m_pryOflLasNln.retrieve() );
  CHECK( m_pryOflCes.retrieve() );
  CHECK( m_pryOflEms.retrieve() );

  m_useOflLasFib = !(m_pryOflLasFib.empty());
  if (m_useOflLasFib) {
    //=== retrieve offline proxy
    CHECK( m_pryOflLasFib.retrieve() );
    ATH_MSG_INFO("ProxyOflLasFib is set up and can be used");
  } else {
    ATH_MSG_INFO("ProxyOflLasFib is not set up and cannot be used");
  }

  //=== Attempt to load online folders only if online cache unit is valid
  if (m_onlCacheUnit != TileRawChannelUnit::Invalid) {

    //=== Retrieve offline proxies
    CHECK( m_pryOnlCis.retrieve() );
    CHECK( m_pryOnlLas.retrieve() );
    CHECK( m_pryOnlCes.retrieve() );
    CHECK( m_pryOnlEms.retrieve() );

    //=== Undoing online corrections works only with connection to COOL DB
    //=== i.e. ensure that we have only TileCondProxyCool tools
    const TileCondProxyCoolFlt* ptrPryOnlCis = dynamic_cast<const TileCondProxyCoolFlt*>(&(*m_pryOnlCis));
    const TileCondProxyCoolFlt* ptrPryOnlLas = dynamic_cast<const TileCondProxyCoolFlt*>(&(*m_pryOnlLas));
    const TileCondProxyCoolFlt* ptrPryOnlCes = dynamic_cast<const TileCondProxyCoolFlt*>(&(*m_pryOnlCes));
    const TileCondProxyCoolFlt* ptrPryOnlEms = dynamic_cast<const TileCondProxyCoolFlt*>(&(*m_pryOnlEms));

    if (!ptrPryOnlCis || !ptrPryOnlLas || !ptrPryOnlCes || !ptrPryOnlEms) {
      ATH_MSG_ERROR( "Requested valid OnlCacheUnit (" << m_onlCacheUnitStr << ") "
                    << "but at least one ProxyOnl{Cis|Las|Ces|Ems} is not connected to a COOL folder!" );
      return StatusCode::FAILURE;
    }

    // 
    const TileCondProxyCoolFlt* ptrPryOflLasLin = dynamic_cast<const TileCondProxyCoolFlt*>(&(*m_pryOflLasLin));
    const TileCondProxyCoolFlt* ptrPryOflLasNln = dynamic_cast<const TileCondProxyCoolFlt*>(&(*m_pryOflLasNln));
    if (!ptrPryOflLasLin || !ptrPryOflLasNln) {
      ATH_MSG_ERROR( "At least one ProxyOfl{LasLin|LasNln} is not connected to a COOL folder!" );
      return StatusCode::FAILURE;
    }

    //=== Register resetCache callback function for online folders
    //---
    //--- This below is a bit tricky:
    //--- The regFcn needs a pointer to the function actually called, a pointer to a callback function in a base class 
    //--- (for example a purely virtual function in ITileCondProxy) does NOT work. 
    //--- Therefore we need to provide a pointer to the callback function of the derived class. Hence we ensured
    //--- with above code that we really have a TileCondProxyCool connected to the ToolHandle.
    //---
    CHECK( detStore()->regFcn(&TileCondProxyCoolFlt::callback, ptrPryOnlCis, &TileCondToolEmscale::resetOnlCache, this, true) );
    CHECK( detStore()->regFcn(&TileCondProxyCoolFlt::callback, ptrPryOnlLas, &TileCondToolEmscale::resetOnlCache, this, true) );
    CHECK( detStore()->regFcn(&TileCondProxyCoolFlt::callback, ptrPryOnlCes, &TileCondToolEmscale::resetOnlCache, this, true) );
    CHECK( detStore()->regFcn(&TileCondProxyCoolFlt::callback, ptrPryOnlEms, &TileCondToolEmscale::resetOnlCache, this, true) );

    //=== Register checkIfOflLasLinCalibUsed callback function for offline linear laser folder
    CHECK( detStore()->regFcn(&TileCondProxyCoolFlt::callback, ptrPryOflLasLin, &TileCondToolEmscale::checkIfOflLasLinCalibUsed, this, true) );

    //=== Register checkIfOflLasNlnCalibUsed callback function for offline nonlinear laser folder
    CHECK( detStore()->regFcn(&TileCondProxyCoolFlt::callback, ptrPryOflLasNln, &TileCondToolEmscale::checkIfOflLasNlnCalibUsed, this, true) );

    //=== Resize onlCache to desired size

    ServiceHandle<TileCablingSvc> cablingSvc("TileCablingSvc", name());
    CHECK( cablingSvc.retrieve());
    
    const TileCablingService* cabling = cablingSvc->cablingService();
    if (!cabling) {
      ATH_MSG_ERROR( "Unable to retrieve TileCablingService" );
      return StatusCode::FAILURE;
    }
    
    m_maxChannels = cabling->getMaxChannels();
    m_maxGains = cabling->getMaxGains();
    m_drawerCacheSize = m_maxChannels * m_maxGains;
    
    m_onlCache.resize(m_drawerCacheSize * TileCalibUtils::MAX_DRAWERIDX);
    
  } else {
    ATH_MSG_INFO( "Loading of online calibration folders not requested, "
                  << "since OnlCacheUnit=" << m_onlCacheUnitStr  );
  }

  return StatusCode::SUCCESS;
}

//
//____________________________________________________________________
StatusCode TileCondToolEmscale::finalize() {
  ATH_MSG_DEBUG( "finalize called" );
  return StatusCode::SUCCESS;
}

//
//____________________________________________________________________
StatusCode TileCondToolEmscale::resetOnlCache( IOVSVC_CALLBACK_ARGS_K(keys)) {
  msg(MSG::INFO) << "TileCondToolEmscale::resetOnlCache() has been triggered by: ";
  std::list<std::string>::const_iterator itr;
  for (itr = keys.begin(); itr != keys.end(); ++itr) {
    msg(MSG::INFO) << "\'" << *itr << "' ";
  }
  msg(MSG::INFO) << endmsg;

  //=== recalculate online cache
  for (unsigned int drawerIdx = 0; drawerIdx < TileCalibUtils::MAX_DRAWERIDX; ++ drawerIdx) {
    for (unsigned int channel = 0; channel < m_maxChannels; ++channel) {
      for (unsigned int adc = 0; adc < m_maxGains; ++adc) {
        m_onlCache[cacheIndex(drawerIdx, channel, adc)] = getOnlCalib(drawerIdx, channel, adc, m_onlCacheUnit);      
      }
    }
  }

  return StatusCode::SUCCESS;
}

//
//____________________________________________________________________
float TileCondToolEmscale::getOnlCalib(unsigned int drawerIdx, unsigned int channel, unsigned int adc
                                       , TileRawChannelUnit::UNIT onlUnit) const {

  if (drawerIdx >= TileCalibUtils::MAX_DRAWERIDX) {
    throw TileCalib::IndexOutOfRange("TileCondToolEmscale::getOnlineCalib", drawerIdx, TileCalibUtils::MAX_DRAWERIDX);
  }

  ATH_MSG_VERBOSE( "Recalculating total online constant for "
                  << drawerIdx << "/" << channel << "/" << adc << " , " << onlUnit );

  float val(1.);
  //=== CIS calibration
  if (onlUnit > TileRawChannelUnit::OnlineADCcounts) {
    val = m_pryOnlCis->getCalibDrawer(drawerIdx)->getCalib(channel, adc, val);
    ATH_MSG_VERBOSE( "---> val after CIS: " << val );

    //=== LAS+CES calibration
    if (onlUnit > TileRawChannelUnit::OnlinePicoCoulombs) {
      val = m_pryOnlLas->getCalibDrawer(drawerIdx)->getCalib(channel, adc, val);
      ATH_MSG_VERBOSE(  "---> val after LAS: " << val );

      val = m_pryOnlCes->getCalibDrawer(drawerIdx)->getCalib(channel, adc, val);
      ATH_MSG_VERBOSE( "---> val after CES: " << val );
    }
    //=== EMSCALE calibration
    if (onlUnit > TileRawChannelUnit::OnlineCesiumPicoCoulombs) {
      val = m_pryOnlEms->getCalibDrawer(drawerIdx)->getCalib(channel, adc, val);
      ATH_MSG_VERBOSE( "---> val after EMS: " << val );
    }
  }
  //=== Convert float to 16 bit fixed point DSP precision
  val = TileCalibUtils::fixedPointPrecision(val);
  ATH_MSG_VERBOSE( "---> val in fixed point precision: " << val );

  return val;
}

//
//____________________________________________________________________
float TileCondToolEmscale::undoOnlCalib(unsigned int drawerIdx, unsigned int channel, unsigned int adc
                                        , float amplitude, TileRawChannelUnit::UNIT onlUnit) const {

  //=== Check if online folders are available
  if (m_onlCacheUnit == TileRawChannelUnit::Invalid) {
    ATH_MSG_FATAL( "Trying to undo online calibration, but COOL folders were not loaded" );
    std::abort();
  }

  //=== Check for valid unit request
  if (onlUnit < TileRawChannelUnit::OnlineADCcounts
      || onlUnit > TileRawChannelUnit::OnlineMegaElectronVolts) {
    throw TileCalib::InvalidRawChanUnit("TileCondToolEmscale::channelCalib(onl)", onlUnit);
  }

  float val(0.);
  //=== Look up total calib constant in cache if possible ...
  if (onlUnit == m_onlCacheUnit) {
    unsigned int idx = cacheIndex(drawerIdx, channel, adc);
    if (idx >= m_onlCache.size()) {
      throw TileCalib::IndexOutOfRange("TileCondToolEmscale::undoOnlineCalib", idx, m_onlCache.size());
    }

    val = m_onlCache[idx];
  }
  //=== ... otherwise compute on the fly  
  else {
    val = getOnlCalib(drawerIdx, channel, adc, onlUnit);
  }

  //=== Sanity check
  if (val == 0.) {
    throw TileCalib::InvalidValue("TileCondToolEmscale::undoOnlCalib", val);
  }

  return amplitude / val;
}

//
//____________________________________________________________________
float TileCondToolEmscale::channelCalib(unsigned int drawerIdx, unsigned int channel, unsigned int adc
                                        , float amplitude, TileRawChannelUnit::UNIT rawDataUnitIn
                                        , TileRawChannelUnit::UNIT rawDataUnitOut) const {

  //=== Undo online calibration, result is offline ADC counts
  if (rawDataUnitIn >= TileRawChannelUnit::OnlineADCcounts) {
    if (rawDataUnitIn > TileRawChannelUnit::OnlineMegaElectronVolts) {
      throw TileCalib::InvalidRawChanUnit("TileCondToolEmscale::channelCalib(onl)", rawDataUnitIn);
    } else if (rawDataUnitIn > TileRawChannelUnit::OnlineADCcounts) {
      amplitude = undoOnlCalib(drawerIdx, channel, adc, amplitude, rawDataUnitIn);
    }
    rawDataUnitIn = TileRawChannelUnit::ADCcounts;
  }

  //=== Allow only "forward" calibration from here on
  if (rawDataUnitOut < rawDataUnitIn || rawDataUnitOut > TileRawChannelUnit::MegaElectronVolts) {
    ATH_MSG_ERROR( "Invalid unit combination: "
                  << "in=" << rawDataUnitIn << " ,"
                  << "out=" << rawDataUnitOut );

    throw TileCalib::InvalidRawChanUnit("TileCondToolEmscale::channelCalib(out)", rawDataUnitOut);
  }

  //=== decide which calibrations to apply
  switch (rawDataUnitIn) {

    case TileRawChannelUnit::ADCcounts:
    case TileRawChannelUnit::OnlineADCcounts:
      if (rawDataUnitOut > TileRawChannelUnit::ADCcounts) {
        amplitude = doCalibCis(drawerIdx, channel, adc, amplitude);
        if (rawDataUnitOut > TileRawChannelUnit::PicoCoulombs) {
          amplitude = doCalibLas(drawerIdx, channel, amplitude);
          amplitude = doCalibCes(drawerIdx, channel, amplitude);
          if (rawDataUnitOut > TileRawChannelUnit::CesiumPicoCoulombs) {
            amplitude = doCalibEms(drawerIdx, channel, amplitude);
          }
        }
      }
      break;

    case TileRawChannelUnit::PicoCoulombs:
      if (rawDataUnitOut > TileRawChannelUnit::PicoCoulombs) {
        amplitude = doCalibLas(drawerIdx, channel, amplitude);
        amplitude = doCalibCes(drawerIdx, channel, amplitude);
        if (rawDataUnitOut > TileRawChannelUnit::CesiumPicoCoulombs) {
          amplitude = doCalibEms(drawerIdx, channel, amplitude);
        }
      }
      break;

    case TileRawChannelUnit::CesiumPicoCoulombs:
      if (rawDataUnitOut > TileRawChannelUnit::CesiumPicoCoulombs) {
        amplitude = doCalibEms(drawerIdx, channel, amplitude);
      }
      break;

    case TileRawChannelUnit::MegaElectronVolts:
      break;

    default:
      ATH_MSG_ERROR( "We should never come here: "
                    << "in=" << rawDataUnitIn << " ,"
                    << "out=" << rawDataUnitOut );

      throw TileCalib::InvalidRawChanUnit("TileCondToolEmscale::channelCalib", rawDataUnitIn);

  }

  return amplitude;
}

//
//____________________________________________________________________
float TileCondToolEmscale::doCalibEms(unsigned int drawerIdx, unsigned int channel, float amplitude) const {
  if (drawerIdx >= TileCalibUtils::MAX_DRAWERIDX) {
    throw TileCalib::IndexOutOfRange("TileCondToolEmscale::doCalibEms", drawerIdx, TileCalibUtils::MAX_DRAWERIDX);
  }

  return m_pryOflEms->getCalibDrawer(drawerIdx)->getCalib(channel, 0, amplitude);
}

//
//____________________________________________________________________
float TileCondToolEmscale::doCalibCes(unsigned int drawerIdx, unsigned int channel, float amplitude, bool applyLasCorr) const {

  if (drawerIdx >= TileCalibUtils::MAX_DRAWERIDX) {
    throw TileCalib::IndexOutOfRange("TileCondToolEmscale::doCalibCes", drawerIdx, TileCalibUtils::MAX_DRAWERIDX);
  }

  amplitude = m_pryOflCes->getCalibDrawer(drawerIdx)->getCalib(channel, 0, amplitude);

  if (applyLasCorr) {
    //=== At some point a correction relative to the mono-energy laser reference
    //=== taken with each Cesium calibration run needs to be implemented here. 
  }

  return amplitude;
}

//
//____________________________________________________________________
float TileCondToolEmscale::doCalibLas(unsigned int drawerIdx, unsigned int channel, float amplitude) const {

  if (m_OflLasLinUsed || m_OflLasNlnUsed) {
    if (drawerIdx >= TileCalibUtils::MAX_DRAWERIDX) {
      throw TileCalib::IndexOutOfRange("TileCondToolLascale::doCalibLas", drawerIdx, TileCalibUtils::MAX_DRAWERIDX);
    }

    //=== Linear correction
    if (m_OflLasLinUsed)
      amplitude = m_pryOflLasLin->getCalibDrawer(drawerIdx)->getCalib(channel, 0, amplitude);
    //=== Non-linear correction
    if (m_OflLasNlnUsed)
      amplitude = m_pryOflLasNln->getCalibDrawer(drawerIdx)->getCalib(channel, 0, amplitude);
  }

  return amplitude;
}

//
//____________________________________________________________________
float TileCondToolEmscale::doCalibCis(unsigned int drawerIdx, unsigned int channel, unsigned int adc, float amplitude) const {

  if (drawerIdx >= TileCalibUtils::MAX_DRAWERIDX) {
    throw TileCalib::IndexOutOfRange("TileCondToolCiscale::doCalibCis", drawerIdx, TileCalibUtils::MAX_DRAWERIDX);
  }
  //=== Linear correction
  amplitude = m_pryOflCisLin->getCalibDrawer(drawerIdx)->getCalib(channel, adc, amplitude);

  //=== Non-linear correction
  return m_pryOflCisNln->getCalibDrawer(drawerIdx)->getCalib(channel, adc, amplitude);
}

//
//____________________________________________________________________
float TileCondToolEmscale::channelCalibOnl(unsigned int drawerIdx, unsigned int channel, unsigned int adc
                                           , float amplitude, TileRawChannelUnit::UNIT onlUnit) const {

  if (drawerIdx >= TileCalibUtils::MAX_DRAWERIDX) {
    throw TileCalib::IndexOutOfRange("TileCondToolEmscale::channelCalibOnl", drawerIdx, TileCalibUtils::MAX_DRAWERIDX);
  }

  ATH_MSG_VERBOSE( "Recalculating total online constant for "
                  << drawerIdx << "/" << channel << "/" << adc << " , " << onlUnit );


  //=== CIS calibration
  if (onlUnit > TileRawChannelUnit::OnlineADCcounts) {
    amplitude = m_pryOnlCis->getCalibDrawer(drawerIdx)->getCalib(channel, adc, amplitude);
    ATH_MSG_VERBOSE( "---> amplitude after CIS: " << amplitude );

    //=== LAS+CES calibration
    if (onlUnit > TileRawChannelUnit::OnlinePicoCoulombs) {
      amplitude = m_pryOnlLas->getCalibDrawer(drawerIdx)->getCalib(channel, adc, amplitude);
      ATH_MSG_VERBOSE( "---> amplitude after LAS: " << amplitude );

      amplitude = m_pryOnlCes->getCalibDrawer(drawerIdx)->getCalib(channel, adc, amplitude);
      ATH_MSG_VERBOSE( "---> amplitude after CES: " << amplitude );
    }

    //=== EMSCALE calibration
    if (onlUnit > TileRawChannelUnit::OnlineCesiumPicoCoulombs) {
      amplitude = m_pryOnlEms->getCalibDrawer(drawerIdx)->getCalib(channel, adc, amplitude);
      ATH_MSG_VERBOSE( "---> amplitude after EMS: " << amplitude );
    }
  }

  //=== convert float to 16 bit fixed point DSP precision
  amplitude = TileCalibUtils::fixedPointPrecision(amplitude);
  ATH_MSG_VERBOSE( "---> amplitude in fixed point precision: " << amplitude );

  return amplitude;

}

//
//____________________________________________________________________
float TileCondToolEmscale::doCalibEmsOnl(unsigned int drawerIdx, unsigned int channel, float amplitude) const {
  if (drawerIdx >= TileCalibUtils::MAX_DRAWERIDX) {
    throw TileCalib::IndexOutOfRange("TileCondToolEmscale::doCalibEmsOnl", drawerIdx, TileCalibUtils::MAX_DRAWERIDX);
  }

  return m_pryOnlEms->getCalibDrawer(drawerIdx)->getCalib(channel, 0, amplitude);
}

//
//____________________________________________________________________
float TileCondToolEmscale::doCalibCesOnl(unsigned int drawerIdx, unsigned int channel, float amplitude, bool applyLasCorr) const {

  if (drawerIdx >= TileCalibUtils::MAX_DRAWERIDX) {
    throw TileCalib::IndexOutOfRange("TileCondToolEmscale::doCalibCesOnl", drawerIdx, TileCalibUtils::MAX_DRAWERIDX);
  }

  amplitude = m_pryOnlCes->getCalibDrawer(drawerIdx)->getCalib(channel, 0, amplitude);
  if (applyLasCorr) {
    //=== At some point a correction relative to the mono-energy laser reference
    //=== taken with each Cesium calibration run needs to be implemented here. 
  }

  return amplitude;
}

//
//____________________________________________________________________
float TileCondToolEmscale::doCalibLasOnl(unsigned int drawerIdx, unsigned int channel, float amplitude) const {

  if (drawerIdx >= TileCalibUtils::MAX_DRAWERIDX) {
    throw TileCalib::IndexOutOfRange("TileCondToolLascale::doCalibLasOnl", drawerIdx, TileCalibUtils::MAX_DRAWERIDX);
  }
  //=== Linear correction only
  return m_pryOnlLas->getCalibDrawer(drawerIdx)->getCalib(channel, 0, amplitude);
}

//
//____________________________________________________________________
float TileCondToolEmscale::doCalibCisOnl(unsigned int drawerIdx, unsigned int channel, unsigned int adc, float amplitude) const {

  if (drawerIdx >= TileCalibUtils::MAX_DRAWERIDX) {
    throw TileCalib::IndexOutOfRange("TileCondToolCiscale::doCalibCisOnl", drawerIdx, TileCalibUtils::MAX_DRAWERIDX);
  }

  //=== Linear correction only
  return m_pryOnlCis->getCalibDrawer(drawerIdx)->getCalib(channel, adc, amplitude);
}

//
//____________________________________________________________________
float TileCondToolEmscale::getCesRefLas(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const {

  if (drawerIdx >= TileCalibUtils::MAX_DRAWERIDX) {
    throw TileCalib::IndexOutOfRange("TileCondToolEmscale::getCesRefLas", drawerIdx, TileCalibUtils::MAX_DRAWERIDX);
  }

  return m_pryOflCes->getCalibDrawer(drawerIdx)->getData(channel, adc, 1);
}

//
//____________________________________________________________________
float TileCondToolEmscale::getCesRefHv(unsigned int drawerIdx, unsigned int channel) const {

  if (drawerIdx >= TileCalibUtils::MAX_DRAWERIDX) {
    throw TileCalib::IndexOutOfRange("TileCondToolEmscale::getCesRefHv", drawerIdx, TileCalibUtils::MAX_DRAWERIDX);
  }

  return m_pryOflCes->getCalibDrawer(drawerIdx)->getData(channel, 0, 2);
}

//
//____________________________________________________________________
float TileCondToolEmscale::getCesRefTemp(unsigned int drawerIdx, unsigned int channel) const {

  if (drawerIdx >= TileCalibUtils::MAX_DRAWERIDX) {
    throw TileCalib::IndexOutOfRange("TileCondToolEmscale::getCesRefTemp", drawerIdx, TileCalibUtils::MAX_DRAWERIDX);
  }

  return m_pryOflCes->getCalibDrawer(drawerIdx)->getData(channel, 0, 3);
}

//
//____________________________________________________________________
float TileCondToolEmscale::getLasFiber(unsigned int drawerIdx, unsigned int channel) const {

  if (drawerIdx >= TileCalibUtils::MAX_DRAWERIDX) {
    throw TileCalib::IndexOutOfRange("TileCondToolEmscale::getLasFiber", drawerIdx, TileCalibUtils::MAX_DRAWERIDX);
  }

  return m_pryOflLasFib->getCalibDrawer(drawerIdx)->getData(channel, 0, 0);
}

//
//____________________________________________________________________
float TileCondToolEmscale::getLasPartition(unsigned int drawerIdx) const {

  if (drawerIdx >= TileCalibUtils::MAX_DRAWERIDX) {
    throw TileCalib::IndexOutOfRange("TileCondToolEmscale::getLasFiber", drawerIdx, TileCalibUtils::MAX_DRAWERIDX);
  }

  return m_pryOflLasFib->getCalibDrawer(TileCalibUtils::getFirstDrawerInPartitionIdx(drawerIdx))->getData( TileCalibUtils::LAS_PART_CHAN, 0, 0);
}

//
//____________________________________________________________________
float TileCondToolEmscale::getLasRefHv(unsigned int drawerIdx, unsigned int channel) const {

  float val = -4.;

  if (drawerIdx >= TileCalibUtils::MAX_DRAWERIDX) {
    throw TileCalib::IndexOutOfRange("TileCondToolEmscale::getLasRefHv", drawerIdx, TileCalibUtils::MAX_DRAWERIDX);
  }

  const TileCalibDrawerFlt* calibDrawer = m_pryOflLasLin->getCalibDrawer(drawerIdx);
  int size = calibDrawer->getObjSizeUint32();
  if (size > 1) {
    val = m_pryOflLasLin->getCalibDrawer(drawerIdx)->getData(channel, 0, 1);
  }

  return val;
}
//

StatusCode TileCondToolEmscale::checkIfOflLasLinCalibUsed( IOVSVC_CALLBACK_ARGS_K(keys)) {

  msg(MSG::INFO) << "TileCondToolEmscale::checkIfOflLasLinCalibUsed() has been triggered by: ";
  std::list<std::string>::const_iterator itr;
  for (itr = keys.begin(); itr != keys.end(); ++itr) {
    msg(MSG::INFO) << "\'" << *itr << "' ";
  }
  msg(MSG::INFO) << endmsg;

  const float defval = 1.0;
  const float epsilon = 1.e-5;
  float val(defval);
  for (unsigned int drawerIdx = 0; drawerIdx < TileCalibUtils::MAX_DRAWERIDX; ++drawerIdx) {
    for (unsigned int chn = 0; chn < TileCalibUtils::MAX_CHAN; ++chn) {
      val = m_pryOflLasLin->getCalibDrawer(drawerIdx)->getCalib(chn, 0, val);
      if (fabs(val - defval) > epsilon) {
        ATH_MSG_INFO( "The offline linear laser calibration will be used" );
        m_OflLasLinUsed = true;
        return StatusCode::SUCCESS;
      }
    }
  }

  ATH_MSG_INFO( "The offline linear laser calibration will not be used" );
  m_OflLasLinUsed = false;

  return StatusCode::SUCCESS;
}

StatusCode TileCondToolEmscale::checkIfOflLasNlnCalibUsed( IOVSVC_CALLBACK_ARGS_K(keys)) {

  msg(MSG::INFO) << "TileCondToolEmscale::checkIfOflLasNlnCalibUsed() has been triggered by: ";
  std::list<std::string>::const_iterator itr;
  for (itr = keys.begin(); itr != keys.end(); ++itr) {
    msg(MSG::INFO) << "\'" << *itr << "' ";
  }
  msg(MSG::INFO) << endmsg;

  const float defval = 100.0;
  const float epsilon = 1.e-3;
  float val(defval);

  for (unsigned int drawerIdx = 0; drawerIdx < TileCalibUtils::MAX_DRAWERIDX; ++drawerIdx) {
    for (unsigned int chn = 0; chn < TileCalibUtils::MAX_CHAN; ++chn) {
      val = m_pryOflLasNln->getCalibDrawer(drawerIdx)->getCalib(chn, 0, val);
      if (fabs(val - defval) > epsilon) {
        ATH_MSG_INFO( "The offline nonlinear laser calibration will be used" );
        m_OflLasNlnUsed = true;
        return StatusCode::SUCCESS;
      }
    }
  }

  ATH_MSG_INFO( "The offline nonlinear laser calibration will not be used" );
  m_OflLasNlnUsed = false;

  return StatusCode::SUCCESS;
}
