/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelMonitoring/PixelAthHitMonTool.h"
#include "PixelMonitoring/PixelAthMonitoringBase.h"
#include "InDetIdentifier/PixelID.h"

#include "PixelCabling/IPixelCablingSvc.h"

PixelAthHitMonTool::PixelAthHitMonTool( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthMonitorAlgorithm(name, pSvcLocator),
  m_pixelCableSvc("PixelCablingSvc", name),
  m_pixelid(nullptr)
{
  //jo flags
  declareProperty("doOnline", m_doOnline = false);
  declareProperty("doModules", m_doModules = false);
  declareProperty("doLumiBlock", m_doLumiBlock = false);
  declareProperty("doLowOccupancy", m_doLowOccupancy = false);
  declareProperty("doHighOccupancy", m_doHighOccupancy = false);
  declareProperty("doHeavyIonMon", m_doHeavyIonMon = false);
}


PixelAthHitMonTool::~PixelAthHitMonTool() {}


StatusCode PixelAthHitMonTool::initialize() {

  ATH_CHECK( detStore()->retrieve(m_pixelid, "PixelID") );
  ATH_CHECK( m_pixelCondSummaryTool.retrieve() );
  ATH_CHECK( m_pixelCableSvc.retrieve() );
  ATH_CHECK( m_pixelRDOName.initialize() );

  return AthMonitorAlgorithm::initialize();
}


StatusCode PixelAthHitMonTool::fillHistograms( const EventContext& ctx ) const {
  using namespace Monitored;

  //int lb = GetEventInfo(ctx)->lumiBlock();
  //unsigned int bcid = GetEventInfo(ctx)->bcid();
  auto rdocontainer = SG::makeHandle(m_pixelRDOName, ctx);
  if ( !(rdocontainer.isValid()) ) {
    ATH_MSG_ERROR("Pixel Monitoring: Pixel RDO container "<< m_pixelRDOName << " could not be found.");
    return StatusCode::RECOVERABLE;
  } else {
    ATH_MSG_DEBUG("Pixel Monitoring: Pixel RDO container "<< rdocontainer.name() <<" is found.");
  }

  int nGood_mod[PixLayers::COUNT] = {0};
  int nActive_mod[PixLayers::COUNT] = {0};

  PixelID::const_id_iterator idIt = m_pixelid->wafer_begin();
  PixelID::const_id_iterator idItEnd = m_pixelid->wafer_end();
  for (; idIt != idItEnd; ++idIt) {
    Identifier waferID = *idIt;
    IdentifierHash id_hash = m_pixelid->wafer_hash(waferID);
    int pixlayer = getPixLayersID(m_pixelid->barrel_ec(waferID), m_pixelid->layer_disk(waferID) );
    if (pixlayer == 99) continue;

    if (m_pixelCondSummaryTool->isActive(id_hash) == true) {
      if (pixlayer == PixLayers::kIBL && m_pixelid->eta_module(waferID)>-7 && m_pixelid->eta_module(waferID)<6) nActive_mod[pixlayer]+=2;
      else nActive_mod[pixlayer]++;
 
      if (m_pixelCondSummaryTool->isGood(id_hash) == true) {
	if (pixlayer == PixLayers::kIBL && m_pixelid->eta_module(waferID)>-7 && m_pixelid->eta_module(waferID)<6) nGood_mod[pixlayer]+=2;
	else nGood_mod[pixlayer]++;
      }
    }
  }

  //*******************************************************************************
  //************************** Begin of filling Hit Histograms ********************
  //*******************************************************************************


  ATH_MSG_DEBUG("Filling Raw Hit (RDO) Monitoring Histograms");


  //*******************************************************************************
  //************************** End of filling Hit Histograms **********************
  //*******************************************************************************

  return StatusCode::SUCCESS;
}

