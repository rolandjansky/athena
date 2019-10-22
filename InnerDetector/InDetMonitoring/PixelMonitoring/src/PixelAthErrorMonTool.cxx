/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelMonitoring/PixelAthErrorMonTool.h"
#include "PixelMonitoring/PixelAthMonitoringBase.h"
#include "InDetIdentifier/PixelID.h"

PixelAthErrorMonTool::PixelAthErrorMonTool( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthMonitorAlgorithm(name, pSvcLocator),
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


PixelAthErrorMonTool::~PixelAthErrorMonTool() {}


StatusCode PixelAthErrorMonTool::initialize() {

  ATH_CHECK( detStore()->retrieve(m_pixelid, "PixelID") );
  ATH_CHECK( m_pixelCondSummaryTool.retrieve() );
  ATH_CHECK( m_pixelErrorTool.retrieve() );

  return AthMonitorAlgorithm::initialize();
}


StatusCode PixelAthErrorMonTool::fillHistograms( const EventContext& ctx ) const {
  using namespace Monitored;

  int lb = GetEventInfo(ctx)->lumiBlock();

  //*******************************************************************************
  //************************** Begin of filling Error Histograms ********************
  //*******************************************************************************


  ATH_MSG_DEBUG("Filling Error Monitoring Histograms");

  float num_errors[PixLayers::COUNT] = {0,1,2,3,4,5,6,7};
  // filling tbd

  CHECK( fill1DProfLumiLayers("errors_per_lumi", lb, num_errors) );


  //*******************************************************************************
  //************************** End of filling Error Histograms **********************
  //*******************************************************************************

  return StatusCode::SUCCESS;
}

