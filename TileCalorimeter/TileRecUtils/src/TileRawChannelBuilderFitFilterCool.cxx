/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TileEvent/TileRawChannel.h"

// Gaudi includes
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IIncidentSvc.h"

// Atlas includes
#include "AthAllocators/DataPool.h"
#include "AthenaKernel/errorcheck.h"

// Tile includes
#include "TileRecUtils/TileRawChannelBuilderFitFilterCool.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileDigits.h"
#include "CaloIdentifier/TileID.h"
#include "TileIdentifier/TileHWID.h"
#include "TileIdentifier/TileTrigType.h"
#include "TileConditions/TileInfo.h"
#include "TileConditions/TileCablingService.h"

// lang include
#include <algorithm>
#include <cmath>


static const InterfaceID IID_ITileRawChannelBuilderFitFilterCool
       ("TileRawChannelBuilderFitFilterCool", 1, 0);

const InterfaceID& TileRawChannelBuilderFitFilterCool::interfaceID( ) { 
  return IID_ITileRawChannelBuilderFitFilterCool;
  //return TileRawChannelBuilderCool::interfaceID();
}

/**
 * Constructor
 */
TileRawChannelBuilderFitFilterCool::TileRawChannelBuilderFitFilterCool(const std::string& type,
    const std::string& name, const IInterface *parent)
  : TileRawChannelBuilder(type, name, parent)
  , m_t0Fit(0.0)
  , m_iPeak0(0)
  , m_minTime(0.0)
  , m_maxTime(0.0)
  , m_minTau(0)
  , m_maxTau(0.0)
  , m_pulseShapes(0)
  , m_shapes(nullptr)
{  
  //declare interfaces
  declareInterface< TileRawChannelBuilder >( this );
  declareInterface< TileRawChannelBuilderFitFilterCool >(this);

  m_rawChannelContainerKey = "TileRawChannelFitCool";

  //declare properties
  declareProperty("FrameLength", m_frameLength = 9);
  declareProperty("MaxIterate", m_maxIterate = 9);
  declareProperty("NoiseLowGain", m_noiseLow = 0.6);
  declareProperty("NoiseHighGain", m_noiseHigh = 1.5);
  declareProperty("RMSChannelNoise", m_channelNoiseRMS = 3);
  declareProperty("ExtraSamplesLeft", m_extraSamplesLeft=0);   // increase window on left side
  declareProperty("ExtraSamplesRight", m_extraSamplesRight=0); // increase window on right side
  declareProperty("SaturatedSampleError", m_saturatedSampleError = 6.0);
  declareProperty("ZeroSampleError", m_zeroSampleError = 100.0);
  declareProperty("NoiseThresholdRMS", m_noiseThresholdRMS = 3.0);
  declareProperty("MaxTimeFromPeak", m_maxTimeFromPeak = 250.0);
}

/**
 * Destructor
 */
TileRawChannelBuilderFitFilterCool::~TileRawChannelBuilderFitFilterCool() {
  if (m_shapes) delete m_shapes;
}

/**
 * Initializer
 */
StatusCode TileRawChannelBuilderFitFilterCool::initialize() {

  ATH_MSG_INFO( "TileRawChannelBuilderFitFilterCool::initialize()" );

  m_rChType = TileFragHash::FitFilterCool;

  // init in superclass
  ATH_CHECK( TileRawChannelBuilder::initialize() );
  
  // Get pulse shapes from TileInfo
  m_pulseShapes = m_tileInfo->getPulseShapes();

  // Determine peak sample position 
  // peak sample position defines t=0 
  m_iPeak0 = (int) (m_frameLength) / 2 + (m_frameLength % 2) - 1;

  // Min and max time are now calculated based on m_framelength - i.e. on the
  // number of 25 ns samples read out. Don't forget t=0 corresponds to 
  // m_ipeak0-th sample
  m_minTime = DTIME * (0 - m_iPeak0 - m_extraSamplesLeft);
  m_maxTime = DTIME * (m_frameLength - 1 - m_iPeak0 + m_extraSamplesRight);
  // maximal jump during one iteration
  m_maxTau = (m_maxTime - m_minTime);
  m_minTau = -m_maxTau;

  ATH_MSG_DEBUG( " ipeak0=" << m_iPeak0
                << " min_time=" << m_minTime
                << " max_time=" << m_maxTime
                << " min_tau=" << m_minTau
                << " max_tau=" << m_maxTau
                << " physize=" << m_pulseShapes->m_ylphys.size()
                << " cissize=" << m_pulseShapes->m_ylcis.size()
                << " xpmin=" << m_pulseShapes->m_tlphys[0]
                << " xpmax=" << m_pulseShapes->m_tlphys[MAX_LO_PULSE_PHYS - 1]
                << " ypmin=" << m_pulseShapes->m_ylphys[0]
                << " ypmax=" << m_pulseShapes->m_ylphys[MAX_LO_PULSE_PHYS - 1] );
  
  //=== get TileCondToolPulseShape
  ATH_CHECK( m_tileToolPulseShape.retrieve() );

  if (m_idocis) {
    ATH_CHECK( m_tileToolLeak100Shape.retrieve() );
    ATH_CHECK( m_tileToolLeak5p2Shape.retrieve() );
    ATH_CHECK( m_tileToolPulse5p2Shape.retrieve() );
  } else {
    m_tileToolLeak100Shape.disable();
    m_tileToolLeak5p2Shape.disable();
    m_tileToolPulse5p2Shape.disable();
  }

  //=== TileCondToolNoiseSample
  ATH_CHECK( m_tileToolNoiseSample.retrieve() );

  // Incident Service:
  ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", this->name());
  ATH_CHECK(incSvc.retrieve());
  //start listening to "BeginRun"
  incSvc->addListener(this, "BeginRun");

  if (msgLvl(MSG::DEBUG)) {
    if (m_channelNoiseRMS) {
      msg(MSG::DEBUG) << " noise for all channels from Conditions DB ";
      if (TileCablingService::getInstance()->getTestBeam())
        msg(MSG::DEBUG) << " rmsLow(LBA01/0) = " << m_tileToolNoiseSample->getHfn(20, 0, TileID::LOWGAIN)
                        << " rmsHi(LBA01/0) = " << m_tileToolNoiseSample->getHfn(20, 0, TileID::HIGHGAIN)
                        << endmsg;
      else
        msg(MSG::DEBUG) << endmsg;
    } else {
      msg(MSG::DEBUG) << " common noise for all channels (OBSOLETE): " << endmsg;
      msg(MSG::DEBUG) << " rmsLow = " << m_noiseLow
                      << " rmsHi = " << m_noiseHigh << endmsg;
    }
  }

  return StatusCode::SUCCESS;
}

void TileRawChannelBuilderFitFilterCool::handle(const Incident& inc) {

  // -------- get pulse shapes from COOL

  ATH_MSG_INFO( "TileRawChannelBuilderFitFilterCool handle" << inc.type() );

  if (inc.type() == "BeginRun") {
    // Do run initializations here...

    ATH_MSG_DEBUG( "m_trigType " << m_trigType
                  << " m_runType " << m_runType
                  << " m_idophys " << m_idophys
                  << " m_idolas " << m_idolas
                  << " m_idoped " << m_idoped
                  << " m_idocis " << m_idocis );

    if (m_shapes) delete m_shapes;

    m_shapes = new TilePulseShapesStruct;
    float y, dy;
    unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(0, 0);

    float phase = 0;
    if (m_idolas) {
      // --- low gain
      m_shapes->m_yllas.reserve(MAX_LO_PULSE_LAS);
      m_shapes->m_tllas.reserve(MAX_LO_PULSE_LAS);
      m_shapes->m_ydllas.reserve(MAX_LO_PULSE_LAS);
      m_shapes->m_tdllas.reserve(MAX_LO_PULSE_LAS);
      phase = LAS_START_T_LO;
      for (int i = 0; i < MAX_LO_PULSE_LAS; i++) {
        //                drawerIdx, channel,gain, phase+25*(i-3), py, pdy
        m_tileToolPulseShape->getPulseShapeYDY(drawerIdx, 0, 0, phase, y, dy);
        m_shapes->m_tllas.push_back(phase);
        m_shapes->m_yllas.push_back(y);
        m_shapes->m_tdllas.push_back(phase);
        m_shapes->m_ydllas.push_back(dy);
        if (msgLvl(MSG::DEBUG)) {
          msg(MSG::DEBUG) << "new  tllas= " << m_shapes->m_tllas[i]
                          << " yllas= " << m_shapes->m_yllas[i]
                          << " ydllas " << m_shapes->m_ydllas[i] << endmsg;

          msg(MSG::DEBUG) << " old  tllas= " << m_pulseShapes->m_tllas[i]
                          << " yllas= " << m_pulseShapes->m_yllas[i]
                          << " ydllas " << m_pulseShapes->m_ydllas[i] << endmsg;
        }
        phase += LAS_DT_LO;
      }
      // --- high gain
      m_shapes->m_yhlas.reserve(MAX_HI_PULSE_LAS);
      m_shapes->m_thlas.reserve(MAX_HI_PULSE_LAS);
      m_shapes->m_ydhlas.reserve(MAX_HI_PULSE_LAS);
      m_shapes->m_tdhlas.reserve(MAX_HI_PULSE_LAS);
      phase = LAS_START_T_HI;
      for (int i = 0; i < MAX_HI_PULSE_LAS; i++) {
        //                drawerIdx, channel,gain, phase+25*(i-3), py, pdy
        m_tileToolPulseShape->getPulseShapeYDY(drawerIdx, 0, 1, phase, y, dy);
        m_shapes->m_thlas.push_back(phase);
        m_shapes->m_yhlas.push_back(y);
        m_shapes->m_tdhlas.push_back(phase);
        m_shapes->m_ydhlas.push_back(dy);
        if (msgLvl(MSG::DEBUG)) {
          msg(MSG::DEBUG) << "n ew  thlas= " << m_shapes->m_thlas[i]
                          << " yhlas= " << m_shapes->m_yhlas[i]
                          << " ydhlas " << m_shapes->m_ydhlas[i] << endmsg;

          msg(MSG::DEBUG) << " old  thlas= " << m_pulseShapes->m_thlas[i]
                          << " yhlas= " << m_pulseShapes->m_yhlas[i]
                          << " ydhlas " << m_pulseShapes->m_ydhlas[i] << endmsg;
        }
        phase += LAS_DT_HI;
      }
    } else if (m_idocis || m_idophys || m_idoped) {
      // --- low gain
      m_shapes->m_ylphys.reserve(MAX_LO_PULSE_PHYS);
      m_shapes->m_tlphys.reserve(MAX_LO_PULSE_PHYS);
      m_shapes->m_ydlphys.reserve(MAX_LO_PULSE_PHYS);
      m_shapes->m_tdlphys.reserve(MAX_LO_PULSE_PHYS);
      phase = PHYS_START_T_LO;
      for (int i = 0; i < MAX_LO_PULSE_PHYS; i++) {
        //                drawerIdx, channel,gain, phase+25*(i-3), py, pdy
        m_tileToolPulseShape->getPulseShapeYDY(drawerIdx, 0, 0, phase, y, dy);
        m_shapes->m_tlphys.push_back(phase);
        m_shapes->m_ylphys.push_back(y);
        m_shapes->m_tdlphys.push_back(phase);
        m_shapes->m_ydlphys.push_back(dy);
        if (msgLvl(MSG::DEBUG)) {
          msg(MSG::DEBUG) << " new  tlphys= " << m_shapes->m_tlphys[i]
                          << " ylphys= " << m_shapes->m_ylphys[i]
                          << " ydlphys " << m_shapes->m_ydlphys[i] << endmsg;

          msg(MSG::DEBUG) << " old  tlphys= " << m_pulseShapes->m_tlphys[i]
                          << " ylphys= " << m_pulseShapes->m_ylphys[i]
                          << " ydlphys " << m_pulseShapes->m_ydlphys[i] << endmsg;
        }
        phase += PHYS_DT_LO;
      }
      // --- high gain
      m_shapes->m_yhphys.reserve(MAX_HI_PULSE_PHYS);
      m_shapes->m_thphys.reserve(MAX_HI_PULSE_PHYS);
      m_shapes->m_ydhphys.reserve(MAX_HI_PULSE_PHYS);
      m_shapes->m_tdhphys.reserve(MAX_HI_PULSE_PHYS);
      phase = PHYS_START_T_HI;
      for (int i = 0; i < MAX_HI_PULSE_PHYS; i++) {
        //                drawerIdx, channel,gain, phase+25*(i-3), py, pdy
        m_tileToolPulseShape->getPulseShapeYDY(drawerIdx, 0, 1, phase, y, dy);
        m_shapes->m_thphys.push_back(phase);
        m_shapes->m_yhphys.push_back(y);
        m_shapes->m_tdhphys.push_back(phase);
        m_shapes->m_ydhphys.push_back(dy);
        if (msgLvl(MSG::DEBUG)) {
          msg(MSG::DEBUG) << " new  thphys= " << m_shapes->m_thphys[i]
                          << " yhphys= " << m_shapes->m_yhphys[i]
                          << " ydhphys " << m_shapes->m_ydhphys[i] << endmsg;

          msg(MSG::DEBUG) << " old  thphys= " << m_pulseShapes->m_thphys[i]
                          << " yhphys= " << m_pulseShapes->m_yhphys[i]
                          << " ydhphys " << m_pulseShapes->m_ydhphys[i] << endmsg;
        }
        phase += PHYS_DT_HI;
      }

      //================================================
      // Speedup for physics processing (max_iter=1):
      //  read initial pulse shapes into arrays
      m_fnParameters[0] = 0.0;
      m_fnParameters[1] = 0.0;
      m_fnParameters[2] = 1.0;
      m_t0Fit = 0.0;
      
      m_gPhysLo.reserve(m_frameLength);
      m_dgPhysLo.reserve(m_frameLength);
      m_gPhysHi.reserve(m_frameLength);
      m_dgPhysHi.reserve(m_frameLength);
      
      double rsamp;
      for (int isamp = 0; isamp < m_frameLength; ++isamp) {
        rsamp = (isamp) * 1.0;
        m_gPhysLo.push_back(scaledPulse(rsamp, &(m_pulseShapes->m_tlphys), &(m_pulseShapes->m_ylphys)));
        m_dgPhysLo.push_back(pulse(rsamp, &(m_pulseShapes->m_tdlphys), &(m_pulseShapes->m_ydlphys)));
        m_gPhysHi.push_back(scaledPulse(rsamp, &(m_pulseShapes->m_thphys), &(m_pulseShapes->m_yhphys)));
        m_dgPhysHi.push_back(pulse(rsamp, &(m_pulseShapes->m_tdhphys), &(m_pulseShapes->m_ydhphys)));
      }
    } else {
      ATH_MSG_ERROR( " Unknown run type "
                    << "idophys=" << m_idophys
                    << " idolas=" << m_idolas
                    << " idocis=" << m_idocis
                    << " idoped= " << m_idoped );
    }

    // -------------- CIS shapes

    if (m_idocis) {
      // --- low gain 100 pF Pulse
      m_shapes->m_ylcis.reserve(MAX_LO_PULSE_CIS);
      m_shapes->m_tlcis.reserve(MAX_LO_PULSE_CIS);
      m_shapes->m_ydlcis.reserve(MAX_LO_PULSE_CIS);
      m_shapes->m_tdlcis.reserve(MAX_LO_PULSE_CIS);
      phase = CIS_START_T_LO;
      for (int i = 0; i < MAX_LO_PULSE_CIS; i++) {
        //                drawerIdx, channel,gain, phase+25*(i-3), py, pdy
        m_tileToolPulseShape->getPulseShapeYDY(drawerIdx, 0, 0, phase, y, dy);
        m_shapes->m_tlcis.push_back(phase);
        m_shapes->m_ylcis.push_back(y);
        m_shapes->m_tdlcis.push_back(phase);
        m_shapes->m_ydlcis.push_back(dy);
        //aa	m_shapes->m_ydlcis.push_back(m_pulsevar->m_ydlcis[i]);
        if (msgLvl(MSG::DEBUG)) {
          msg(MSG::DEBUG) << " new  tlcis= " << m_shapes->m_tlcis[i]
                          << " ylcis= " << m_shapes->m_ylcis[i]
                          << " ydlcis " << m_shapes->m_ydlcis[i] << endmsg;

          msg(MSG::DEBUG) << " old  tlcis= " << m_pulseShapes->m_tlcis[i]
                          << " ylcis= " << m_pulseShapes->m_ylcis[i]
                          << " ydlcis " << m_pulseShapes->m_ydlcis[i] << endmsg;
        }
        phase += CIS_DT_LO;
      }
      // --- high gain 100 pF Pulse
      m_shapes->m_yhcis.reserve(MAX_HI_PULSE_CIS);
      m_shapes->m_thcis.reserve(MAX_HI_PULSE_CIS);
      m_shapes->m_ydhcis.reserve(MAX_HI_PULSE_CIS);
      m_shapes->m_tdhcis.reserve(MAX_HI_PULSE_CIS);
      phase = CIS_START_T_HI;
      for (int i = 0; i < MAX_HI_PULSE_CIS; i++) {
        //                drawerIdx, channel,gain, phase+25*(i-3), py, pdy
        m_tileToolPulseShape->getPulseShapeYDY(drawerIdx, 0, 1, phase, y, dy);
        m_shapes->m_thcis.push_back(phase);
        m_shapes->m_yhcis.push_back(y);
        m_shapes->m_tdhcis.push_back(phase);
        m_shapes->m_ydhcis.push_back(dy);
        //aa	m_shapes->m_ydhcis.push_back(m_pulsevar->m_ydhcis[i]);
        if (msgLvl(MSG::DEBUG)) {
          msg(MSG::DEBUG) << " new  thcis= " << m_shapes->m_thcis[i]
                          << " yhcis= " << m_shapes->m_yhcis[i]
                          << " ydhcis " << m_shapes->m_ydhcis[i] << endmsg;

          msg(MSG::DEBUG) << " old  thcis= " << m_pulseShapes->m_thcis[i]
                          << " yhcis= " << m_pulseShapes->m_yhcis[i]
                          << " ydhcis " << m_pulseShapes->m_ydhcis[i] << endmsg;
        }
        phase += CIS_DT_HI;
      }

      // --- low gain 5.2 pF Pulse
      m_shapes->m_yslcis.reserve(MAX_LO_PULSE_CIS_SMALL);
      m_shapes->m_tslcis.reserve(MAX_LO_PULSE_CIS_SMALL);
      m_shapes->m_ydslcis.reserve(MAX_LO_PULSE_CIS_SMALL);
      m_shapes->m_tdslcis.reserve(MAX_LO_PULSE_CIS_SMALL);
      phase = SCIS_START_T_LO;
      for (int i = 0; i < MAX_LO_PULSE_CIS; i++) {
        //                drawerIdx, channel,gain, phase+25*(i-3), py, pdy
        m_tileToolPulse5p2Shape->getPulseShapeYDY(drawerIdx, 0, 0, phase, y, dy);
        m_shapes->m_tslcis.push_back(phase);
        m_shapes->m_yslcis.push_back(y);
        m_shapes->m_tdslcis.push_back(phase);
        m_shapes->m_ydslcis.push_back(dy);
        //aa	m_shapes->m_ydslcis.push_back(m_pulsevar->m_ydslcis[i]);
        if (msgLvl(MSG::DEBUG)) {
          msg(MSG::DEBUG) << " new  tslcis= " << m_shapes->m_tslcis[i]
                          << " yslcis= " << m_shapes->m_yslcis[i]
                          << " ydslcis " << m_shapes->m_ydslcis[i] << endmsg;

          msg(MSG::DEBUG) << " old  tslcis= " << m_pulseShapes->m_tslcis[i]
                          << " yslcis= " << m_pulseShapes->m_yslcis[i]
                          << " ydslcis " << m_pulseShapes->m_ydslcis[i] << endmsg;
        }
        phase += CIS_DT_LO;
      }
      // --- high gain 5.2 pF Pulse
      m_shapes->m_yshcis.reserve(MAX_HI_PULSE_CIS_SMALL);
      m_shapes->m_tshcis.reserve(MAX_HI_PULSE_CIS_SMALL);
      m_shapes->m_ydshcis.reserve(MAX_HI_PULSE_CIS_SMALL);
      m_shapes->m_tdshcis.reserve(MAX_HI_PULSE_CIS_SMALL);
      phase = SCIS_START_T_HI;
      for (int i = 0; i < MAX_HI_PULSE_CIS; i++) {
        //                drawerIdx, channel,gain, phase+25*(i-3), py, pdy
        m_tileToolPulse5p2Shape->getPulseShapeYDY(drawerIdx, 0, 1, phase, y, dy);
        m_shapes->m_tshcis.push_back(phase);
        m_shapes->m_yshcis.push_back(y);
        m_shapes->m_tdshcis.push_back(phase);
        m_shapes->m_ydshcis.push_back(dy);
        //aa	m_shapes->m_ydshcis.push_back(m_pulsevar->m_ydshcis[i]);
        if (msgLvl(MSG::DEBUG)) {
          msg(MSG::DEBUG) << " new  tshcis= " << m_shapes->m_tshcis[i]
                          << " yshcis= " << m_shapes->m_yshcis[i]
                          << " ydshcis " << m_shapes->m_ydshcis[i] << endmsg;

          msg(MSG::DEBUG) << " old  tshcis= " << m_pulseShapes->m_tshcis[i]
                          << " yshcis= " << m_pulseShapes->m_yshcis[i]
                          << " ydshcis " << m_pulseShapes->m_ydshcis[i] << endmsg;
        }
        phase += CIS_DT_HI;
      }

      // --- low gain 100 pF Leak
      m_shapes->m_leaklo.reserve(MAX_LO_PULSE_CIS);
      m_shapes->m_tleaklo.reserve(MAX_LO_PULSE_CIS);
      m_shapes->m_dleaklo.reserve(MAX_LO_PULSE_CIS);
      m_shapes->m_tdleaklo.reserve(MAX_LO_PULSE_CIS);
      phase = LEAK_START_T_LO;
      for (int i = 0; i < MAX_LO_PULSE_CIS; i++) {
        //                drawerIdx, channel,gain, phase+25*(i-3), py, pdy
        m_tileToolLeak100Shape->getPulseShapeYDY(drawerIdx, 0, 0, phase, y, dy);
        m_shapes->m_tleaklo.push_back(phase);
        m_shapes->m_leaklo.push_back(y);
        m_shapes->m_tdleaklo.push_back(phase);
        //aa m_shapes->m_dleaklo.push_back(m_pulsevar->m_dleaklo[i]);
        m_shapes->m_dleaklo.push_back(dy);
        if (msgLvl(MSG::DEBUG)) {
          msg(MSG::DEBUG) << " new tleaklo = " << m_shapes->m_tleaklo[i]
                          << " leaklo = " << m_shapes->m_leaklo[i]
                          << " dleaklo  " << m_shapes->m_dleaklo[i] << endmsg;

          msg(MSG::DEBUG) << " old  tleaklo= " << m_pulseShapes->m_tleaklo[i]
                          << " leaklo= " << m_pulseShapes->m_leaklo[i]
                          << " dleaklo " << m_pulseShapes->m_dleaklo[i] << endmsg;
        }
        phase += CIS_DT_LO;
      }
      // --- high gain 100 pF Leak
      m_shapes->m_leakhi.reserve(MAX_HI_PULSE_CIS);
      m_shapes->m_tleakhi.reserve(MAX_HI_PULSE_CIS);
      m_shapes->m_dleakhi.reserve(MAX_HI_PULSE_CIS);
      m_shapes->m_tdleakhi.reserve(MAX_HI_PULSE_CIS);
      phase = LEAK_START_T_HI;
      for (int i = 0; i < MAX_HI_PULSE_CIS; i++) {
        //                drawerIdx, channel,gain, phase+25*(i-3), py, pdy
        m_tileToolLeak100Shape->getPulseShapeYDY(drawerIdx, 0, 1, phase, y, dy);
        m_shapes->m_tleakhi.push_back(phase);
        m_shapes->m_leakhi.push_back(y);
        m_shapes->m_tdleakhi.push_back(phase);
        m_shapes->m_dleakhi.push_back(dy);
        //aa	m_shapes->m_dleakhi.push_back(m_pulsevar->m_dleakhi[i]);
        if (msgLvl(MSG::DEBUG)) {
          msg(MSG::DEBUG) << " new tleakhi = " << m_shapes->m_tleakhi[i]
                          << " leakhi= " << m_shapes->m_leakhi[i]
                          << " dleakhi " << m_shapes->m_dleakhi[i] << endmsg;

          msg(MSG::DEBUG) << " old tleakhi = " << m_pulseShapes->m_tleakhi[i]
                          << " leakhi= " << m_pulseShapes->m_leakhi[i]
                          << " dleakhi " << m_pulseShapes->m_dleakhi[i] << endmsg;
        }
        phase += CIS_DT_HI;
      }

      // --- low gain 5.2 pF Leak
      m_shapes->m_sleaklo.reserve(MAX_LO_PULSE_CIS_SMALL);
      m_shapes->m_tsleaklo.reserve(MAX_LO_PULSE_CIS_SMALL);
      m_shapes->m_dsleaklo.reserve(MAX_LO_PULSE_CIS_SMALL);
      m_shapes->m_tdsleaklo.reserve(MAX_LO_PULSE_CIS_SMALL);
      phase = SLEAK_START_T_LO;
      for (int i = 0; i < MAX_LO_PULSE_CIS; i++) {
        //                drawerIdx, channel,gain, phase+25*(i-3), py, pdy
        m_tileToolLeak5p2Shape->getPulseShapeYDY(drawerIdx, 0, 0, phase, y, dy);
        m_shapes->m_tsleaklo.push_back(phase);
        m_shapes->m_sleaklo.push_back(y);
        m_shapes->m_tdsleaklo.push_back(phase);
        m_shapes->m_dsleaklo.push_back(dy);
        //aa	m_shapes->m_dsleaklo.push_back(m_pulsevar->m_dsleaklo[i]);
        if (msgLvl(MSG::DEBUG)) {
          msg(MSG::DEBUG) << " new tsleaklo = " << m_shapes->m_tsleaklo[i]
                          << " sleaklo = " << m_shapes->m_sleaklo[i]
                          << " dsleaklo " << m_shapes->m_dsleaklo[i] << endmsg;

          msg(MSG::DEBUG) << " old tsleaklo = " << m_pulseShapes->m_tslcis[i]
                          << " sleaklo= " << m_pulseShapes->m_sleaklo[i]
                          << " dsleaklo " << m_pulseShapes->m_dsleaklo[i] << endmsg;
        }
        phase += CIS_DT_LO;
      }
      // --- high gain 5.2 pF Leak
      m_shapes->m_sleakhi.reserve(MAX_HI_PULSE_CIS_SMALL);
      m_shapes->m_tsleakhi.reserve(MAX_HI_PULSE_CIS_SMALL);
      m_shapes->m_dsleakhi.reserve(MAX_HI_PULSE_CIS_SMALL);
      m_shapes->m_tdsleakhi.reserve(MAX_HI_PULSE_CIS_SMALL);
      phase = SLEAK_START_T_HI;
      for (int i = 0; i < MAX_HI_PULSE_CIS; i++) {
        //                drawerIdx, channel,gain, phase+25*(i-3), py, pdy
        m_tileToolLeak5p2Shape->getPulseShapeYDY(drawerIdx, 0, 1, phase, y, dy);
        m_shapes->m_tsleakhi.push_back(phase);
        m_shapes->m_sleakhi.push_back(y);
        m_shapes->m_tdsleakhi.push_back(phase);
        m_shapes->m_dsleakhi.push_back(dy);
        //aa	m_shapes->m_dsleakhi.push_back(m_pulsevar->m_dsleakhi[i]);
        if (msgLvl(MSG::DEBUG)) {
          msg(MSG::DEBUG) << " new  tsleakhi= " << m_shapes->m_tsleakhi[i]
                          << " sleakhi = " << m_shapes->m_sleakhi[i]
                          << " dsleakhi " << m_shapes->m_dsleakhi[i] << endmsg;

          msg(MSG::DEBUG) << " old  tsleakhi= " << m_pulseShapes->m_tsleakhi[i]
                          << " sleakhi= " << m_pulseShapes->m_sleakhi[i]
                          << " dsleakhi " << m_pulseShapes->m_dsleakhi[i] << endmsg;
        }
        phase += CIS_DT_HI;
      }
    } // endif(m_idocis)
  } // endif "beginrun"
}

StatusCode TileRawChannelBuilderFitFilterCool::finalize() {

  ATH_MSG_DEBUG( "Finalizing" );

  return StatusCode::SUCCESS;
}

TileRawChannel* TileRawChannelBuilderFitFilterCool::rawChannel(const TileDigits* digits) {

  ++m_chCounter;

  const HWIdentifier adcId = digits->adc_HWID();

  ATH_MSG_VERBOSE ( "Running FitFilter for TileRawChannel with HWID "
                   << m_tileHWID->to_string(adcId) );
  
  // tmp variables for filtering
  double amplitude = 0.0;
  double chi2 = 0.0;
  double time = 0.0;
  double pedestal = 0.0;

  // use fit filter
  pulseFit(digits, amplitude, time, pedestal, chi2);

  unsigned int drawerIdx(0), channel(0), adc(0);
  m_tileIdTransforms->getIndices(adcId, drawerIdx, channel, adc);
  
  // fit filter calib
  // note that when called from TileROD_Decoder, m_calibrateEnergy is set
  // from TileROD_Decoder...
  if (m_calibrateEnergy) {
    amplitude = m_tileToolEmscale->doCalibCis(drawerIdx, channel, adc, amplitude);
  }

  ATH_MSG_VERBOSE ( "Creating RawChannel"
                   << " a=" << amplitude
                   << " t=" << time
                   << " q=" << chi2
                   << " ped=" << pedestal );

  // return new TileRawChannel
  //  TileRawChannel *rawCh = new TileRawChannel(adcId,amplitude,time,chi2,pedestal);

  DataPool<TileRawChannel> tileRchPool(m_dataPoollSize);
  TileRawChannel *rawCh = tileRchPool.nextElementPtr();
  rawCh->assign (adcId,
                 amplitude,
                 time,
                 chi2,
                 pedestal);

  if (m_correctTime && chi2 > 0) {
    time -= m_tileToolTiming->getSignalPhase(drawerIdx, channel, adc);
    rawCh->insertTime(time);
    ATH_MSG_VERBOSE ( "Correcting time, new time=" << rawCh->time() );

  }
  
  int gain = m_tileHWID->adc(adcId);
  if (TileID::HIGHGAIN == gain) {
    ++m_nChH;
    m_RChSumH += amplitude;
  } else {
    ++m_nChL;
    m_RChSumL += amplitude;
  }

  return rawCh;
}

/**
 * Calculate energy, time and chi2 for one channel
 * using fitted pulse shape. Method uses HFITV.
 * @param samples TileDigits
 */
void TileRawChannelBuilderFitFilterCool::pulseFit(const TileDigits *digit, double &amplitude
    , double &time, double &pedestal, double &chi2) {

  amplitude = 0.0;
  time = 0.0;
  pedestal = 0.0;
  chi2 = MAX_CHI2;

  const HWIdentifier adcId = digit->adc_HWID();
  int ros = m_tileHWID->ros(adcId);
  int channel = m_tileHWID->channel(adcId);
  int igain = m_tileHWID->adc(adcId);

  int drawer = m_tileHWID->drawer(adcId);
  unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);

  // Estimate channel noise
  double rms = 0.0;
  int noise_channel = (ros < 3) ? channel : channel + 48;

  if (igain == 0) {
    switch (m_channelNoiseRMS) {
      case 3:
        rms = m_tileToolNoiseSample->getHfn(drawerIdx, channel, igain);
        if (rms > 0.0) break;
        /* FALLTHROUGH */
      case 2:
        rms = m_pulseShapes->m_noiseNkLo[noise_channel];
        if (rms > 0.0) break;
        /* FALLTHROUGH */
      case 1:
        rms = m_pulseShapes->m_noiseOrigLo[noise_channel];
        if (rms > 0.0) break;
        /* FALLTHROUGH */
      default:
        rms = m_noiseLow;
    }
  } else if (igain == 1) {
    switch (m_channelNoiseRMS) {
      case 3:
        rms = m_tileToolNoiseSample->getHfn(drawerIdx, channel, igain);
        if (rms > 0.0) break;
        /* FALLTHROUGH */
      case 2:
        rms = m_pulseShapes->m_noiseNkHi[noise_channel];
        if (rms > 0.0) break;
        /* FALLTHROUGH */
      case 1:
        rms = m_pulseShapes->m_noiseOrigHi[noise_channel];
        if (rms > 0.0) break;
        /* FALLTHROUGH */
      default:
        rms = m_noiseHigh;
    }
  } else {
    // neither low nor hi-gain, (e.g. adder data)
    chi2 = -1.0;
    return;
  }

  ATH_MSG_VERBOSE ( "PulseFit:"
                   << " ROS=" << ros
                   << " Channel=" << channel
                   << " gain=" << igain
                   << " RMS=" << rms
                   << " RMSChNoise=" << m_channelNoiseRMS );

  // First sample to fit, number of samples to fit
  int ifit1 = 0;
  int nfit = std::min(m_frameLength, digit->NtimeSamples());

  int ipeakMax = m_iPeak0;
  int ipeakMin = m_iPeak0;

  ATH_MSG_VERBOSE ( "ipeak0=" << m_iPeak0
                   << " ifit1=" << ifit1
                   << " ifit2=" << ifit1 + nfit - 1
                   << " nfit=" << nfit
                   << " idolas=" << m_idolas
                   << " idocis=" << m_idocis
                   << " CISchan=" << m_cischan
                   << " capdaq=" << m_capdaq );

  std::vector<float> samples = digit->samples();
  double maxsamp = 0.0;
  double minsamp = SATURATED_ADC_VALUE;

  double xvec[MAX_SAMPLES], yvec0[MAX_SAMPLES];
  double yvec[MAX_SAMPLES], eyvec[MAX_SAMPLES];

  bool no_signal = true;
  pedestal = samples[0];
  const double delta = 1.e-6;

  for (int isamp = 0; isamp < nfit; ++isamp) {
    int j = isamp + ifit1;
    xvec[isamp] = j;
    yvec0[isamp] = samples[j];
    if (no_signal) no_signal = (fabs(samples[j] - pedestal) < delta);

    // Check for saturated or zero samples and de-weight accordingly
    if ((yvec0[isamp] < SATURATED_ADC_VALUE) && (yvec0[isamp] > 0)) {
      eyvec[isamp] = rms;
    } else {
      if (yvec0[isamp] >= SATURATED_ADC_VALUE) {
        eyvec[isamp] = m_saturatedSampleError * rms;
        ATH_MSG_VERBOSE ( "Saturated ADC value yvec0[" << isamp << "]=" << yvec0[isamp]
                         << " (MAX=" << SATURATED_ADC_VALUE << " ) RMS=" << eyvec[isamp] );
      } else { // must be yvec0[isamp]==0
        eyvec[isamp] = m_zeroSampleError * rms;
        ATH_MSG_VERBOSE ( "Zero ADC value yvec0[" << isamp << "]=" << yvec0[isamp]
                         << " RMS=" << eyvec[isamp] );
      }
    }
    
    if (yvec0[isamp] > maxsamp) {
      // initial time guess based on
      // sample with maximum value
      maxsamp = yvec0[isamp];
      ipeakMax = j;
    }
    if (yvec0[isamp] < minsamp) {
      minsamp = yvec0[isamp];
      ipeakMin = j;
    }
    ATH_MSG_VERBOSE ( "isamp=" << isamp
                     << ", xvec=" << xvec[isamp]
                     << ", yvec0=" << yvec0[isamp]
                     << ", eyvec=" << eyvec[isamp] );
  }

  if (no_signal) {
    ATH_MSG_VERBOSE ( "No signal detected" );
    return;
  }

  // Make an initial guess about pedestal
  double pedg = (yvec0[0] + yvec0[nfit - 1]) / 2.0;

  // Position of max sample compared to nominal peak position
  int delta_peak = 0;
  // Time offset in pulse functions
  m_t0Fit = 0.0;
  // Flag for fixed time in fit
  bool fixedTime = (m_maxIterate < 0);
  
  if (!fixedTime) {
    if (maxsamp - pedg > m_noiseThresholdRMS * rms) {
      delta_peak = ipeakMax - m_iPeak0;  // Adjust initial phase guess,
      m_t0Fit = (delta_peak) * DTIME;       // positive amplitude
    } else if (pedg - minsamp > m_noiseThresholdRMS * rms) {
      delta_peak = ipeakMin - m_iPeak0;       // Adjust initial phase guess,
      m_t0Fit = (delta_peak) * DTIME;     // negative amplitude
    } else {
      fixedTime = true; // no signal above noise
      m_t0Fit = 0.0;    // fit with fixed time
    }
  }
  
  ATH_MSG_VERBOSE ( " initial value of t0fit=" << m_t0Fit
                   << " ipeakMax=" << ipeakMax
                   << " ipeakMin=" << ipeakMin
                   << " fixedTime=" << ((fixedTime) ? "true" : "false") );

  std::vector<double> *tpulse = &m_dummy, *ypulse = &m_dummy, *tdpulse = &m_dummy, *dpulse = &m_dummy;
  std::vector<double> *tleak = &m_dummy, *yleak = &m_dummy, *tdleak = &m_dummy, *dleak = &m_dummy;
  
  if (m_idocis && ((m_cischan == -1) || (channel == m_cischan))) { // CIS pulse
    if (igain == 0) { // low gain
      if (m_capdaq > 10) { // 100 pF capacitor
        tpulse = &(m_pulseShapes->m_tlcis);
        ypulse = &(m_pulseShapes->m_ylcis);
        tdpulse = &(m_pulseShapes->m_tdlcis);
        dpulse = &(m_pulseShapes->m_ydlcis);
        tleak = &(m_pulseShapes->m_tleaklo);
        yleak = &(m_pulseShapes->m_leaklo);
        tdleak = &(m_pulseShapes->m_tdleaklo);
        dleak = &(m_pulseShapes->m_dleaklo);
      } else { // 5.2 pF capacitor
        tpulse = &(m_pulseShapes->m_tslcis);
        ypulse = &(m_pulseShapes->m_yslcis);
        tdpulse = &(m_pulseShapes->m_tdslcis);
        dpulse = &(m_pulseShapes->m_ydslcis);
        tleak = &(m_pulseShapes->m_tsleaklo);
        yleak = &(m_pulseShapes->m_sleaklo);
        tdleak = &(m_pulseShapes->m_tdsleaklo);
        dleak = &(m_pulseShapes->m_dsleaklo);
      }
    } else { // igain==1 => high-gain
      if (m_capdaq > 10) { // 100 pF capacitor
        tpulse = &(m_pulseShapes->m_thcis);
        ypulse = &(m_pulseShapes->m_yhcis);
        tdpulse = &(m_pulseShapes->m_tdhcis);
        dpulse = &(m_pulseShapes->m_ydhcis);
        tleak = &(m_pulseShapes->m_tleakhi);
        yleak = &(m_pulseShapes->m_leakhi);
        tdleak = &(m_pulseShapes->m_tdleakhi);
        dleak = &(m_pulseShapes->m_dleakhi);
      } else { // 5.2 pF capacitor
        tpulse = &(m_pulseShapes->m_tshcis);
        ypulse = &(m_pulseShapes->m_yshcis);
        tdpulse = &(m_pulseShapes->m_tdshcis);
        dpulse = &(m_pulseShapes->m_ydshcis);
        tleak = &(m_pulseShapes->m_tsleakhi);
        yleak = &(m_pulseShapes->m_sleakhi);
        tdleak = &(m_pulseShapes->m_tdsleakhi);
        dleak = &(m_pulseShapes->m_dsleakhi);
      }
    }
  } else {
    if (m_idolas) { // laser pulse
      if (igain == 0) { // low gain
        tpulse = &(m_pulseShapes->m_tllas);
        ypulse = &(m_pulseShapes->m_yllas);
        tdpulse = &(m_pulseShapes->m_tdllas);
        dpulse = &(m_pulseShapes->m_ydllas);
      } else { // igain==1 => high-gain
        tpulse = &(m_pulseShapes->m_thlas);
        ypulse = &(m_pulseShapes->m_yhlas);
        tdpulse = &(m_pulseShapes->m_tdhlas);
        dpulse = &(m_pulseShapes->m_ydhlas);
      }
    } else { // physics pulse
      if (igain == 0) { // low gain
        tpulse = &(m_pulseShapes->m_tlphys);
        ypulse = &(m_pulseShapes->m_ylphys);
        tdpulse = &(m_pulseShapes->m_tdlphys);
        dpulse = &(m_pulseShapes->m_ydlphys);
      } else { // igain==1 => high-gain
        tpulse = &(m_pulseShapes->m_thphys);
        ypulse = &(m_pulseShapes->m_yhphys);
        tdpulse = &(m_pulseShapes->m_tdhphys);
        dpulse = &(m_pulseShapes->m_ydhphys);
      }
    }
  }
  
  // Variables used for iterative fitting
  double gval, gpval, sy, syg, sygp, sg, sgp, sgg, sgpgp, sggp, serr, err2;
  double dgg0, dgg, dggp, dgpgp, dyg, dygp, dg, dc, xd;
  double sllp, sylp, slplp, dleakage, leakage;
  double fixtau = 0.0, fixped = 0.0, fixampl = 0.0, fixchi2 = MAX_CHI2;
  double leaktau = 0.0, leakped = 0.0, leakampl = 0.0, leakchi2 = MAX_CHI2;
  double cistau = 0.0, cisped = 0.0, cisampl = 0.0, cisatau = 0.0, cischi2 = MAX_CHI2;
  double tau, ped, ampl, atau = 0.0, tempChi2 = MAX_CHI2, oldchi2 = MAX_CHI2 / 2;

  // number of iterations
  int niter = 0;
  do {
    ++niter;
    ATH_MSG_VERBOSE ( "niter=" << niter << " maxIterate=" << m_maxIterate );

    if (tempChi2 < oldchi2) oldchi2 = tempChi2; // best chi2 up to now

    // parameters for pulse shape functions
    // 0. phase
    m_fnParameters[0] = 0.0;
    // 1. pedestal 
    m_fnParameters[1] = 0.0;
    // 2. amplitude
    m_fnParameters[2] = 1.0;

    // CIS events linear fit
    if (m_idocis && ((m_cischan == -1) || (channel == m_cischan))) {
      ATH_MSG_VERBOSE ( "Fit time with leakage" );
      // CIS Part (A): fit for time using leakage pulse
      sllp = 0.0;
      sylp = 0.0;
      slplp = 0.0;
      for (int isamp = 0; isamp < nfit; ++isamp) {
        ATH_MSG_VERBOSE ( "Lo gain leakage xvec[" << isamp << "]=" << xvec[isamp] );

        leakage = pulse(xvec[isamp], tleak, yleak);
        dleakage = derivative(xvec[isamp], tdleak, dleak);

        // Samples with pedestal subtracted
        yvec[isamp] = yvec0[isamp] - pedg;

        ATH_MSG_VERBOSE ( " yvec[" << isamp << "]=" << yvec[isamp]
                         << " yvec0[" << isamp << "]=" << yvec0[isamp]
                         << " pedg=" << pedg );

        sllp += leakage * dleakage;
        sylp += yvec[isamp] * dleakage;
        slplp += dleakage * dleakage;
      }
      // Also allow for fixed-time fit to CIS events
      if (fabs(slplp) > EPS_DG && !fixedTime) {
        leaktau = (sllp - sylp) / slplp;
        // Also have to check the range for leaktau
        if (leaktau > m_maxTau)
          leaktau = m_maxTau;
        else if (leaktau < m_minTau) leaktau = m_minTau;
      } else {
        leaktau = 0.0;
      }
      
      ATH_MSG_VERBOSE ( " sllp=" << sllp
                      << " sylp=" << sylp
                      << " slplp=" << slplp
                      << " leaktau=" << leaktau );

      // CIS Part (B): using phase determined in part (A), 
      // subtract leakage pedestal and fit for amplitude, pedestal
      m_fnParameters[0] = leaktau;
      sy = 0.0;
      sg = 0.0;
      syg = 0.0;
      sgg = 0.0;
      serr = 0.0;
      for (int isamp = 0; isamp < nfit; ++isamp) {
        leakage = pulse(xvec[isamp], tleak, yleak);
        gval = scaledPulse(xvec[isamp], tpulse, ypulse);
        yvec[isamp] = yvec0[isamp] - leakage;

        ATH_MSG_VERBOSE ( " yvec[" << isamp << "]=" << yvec[isamp]
                         << " yvec0[" << isamp << "]=" << yvec0[isamp]
                         << " leakage=" << leakage );

        err2 = eyvec[isamp] * eyvec[isamp];
        sy += yvec[isamp] / err2;
        sg += gval / err2;
        syg += yvec[isamp] * gval / err2;
        sgg += gval * gval / err2;
        serr += 1.0 / err2;
      }
      dgg0 = sg * sg - serr * sgg;
      if (fabs(dgg0) > EPS_DG) {
        leakampl = (sy * sg - serr * syg) / dgg0;
        leakped = (syg * sg - sy * sgg) / dgg0;
      } else {
        leakampl = 0.0;
        leakped = sy / serr;
      }

      // Determine Chi2 for corresponding function  for CIS leakage + pulse
      ATH_MSG_VERBOSE ( " Determine Chi2 for CIS leakage + pulse" );
      
      leakchi2 = 0.0;
      m_fnParameters[0] = leaktau;
      m_fnParameters[1] = leakped;
      m_fnParameters[2] = leakampl;
      for (int isamp = 0; isamp < nfit; ++isamp) {
        gval = scaledPulse(xvec[isamp], tpulse, ypulse);
        leakage = pulse(xvec[isamp], tleak, yleak);
        xd = yvec0[isamp] - (gval + leakage);
        leakchi2 = leakchi2 + (xd * xd) / (eyvec[isamp] * eyvec[isamp]);

        ATH_MSG_VERBOSE ( " isamp=" << isamp
                         << " yvec0[" << isamp << "]=" << yvec0[isamp]
                         << " gval=" << gval
                         << ", leakage=" << leakage
                         << ", xd=" << xd );
      }
      leakchi2 = leakchi2 / (nfit - 3.0);

      ATH_MSG_VERBOSE ( " leaktau=" << leaktau
                       << " leakped=" << leakped
                       << " leakampl=" << leakampl
                       << " leakchi2=" << leakchi2 );
      
      // CIS Part C: Least-squares fit with 3 parameters for pulse+leakage
      if (!fixedTime) {
        m_fnParameters[0] = 0.0;
        m_fnParameters[1] = 0.0;
        m_fnParameters[2] = 0.0;
        for (int isamp = 0; isamp < nfit; ++isamp) {
          leakage = pulse(xvec[isamp], tleak, yleak);

          // Subtract leakage from samples
          yvec[isamp] = yvec0[isamp] - leakage;

          ATH_MSG_VERBOSE ( " isamp=" << isamp
                          << " yvec0[" << isamp << "]=" << yvec0[isamp]
                          << " leakage=" << leakage
                          << " yvec[" << isamp << "]=" << yvec[isamp] );
        }
        m_fnParameters[0] = 0.0;
        m_fnParameters[1] = 0.0;
        m_fnParameters[2] = 1.0;
        sy = 0.0;
        sg = 0.0;
        sgp = 0.0;
        syg = 0.0;
        sygp = 0.0;
        sgg = 0.0;
        sggp = 0.0;
        sgpgp = 0.0;
        serr = 0.0;
        for (int isamp = 0; isamp < nfit; ++isamp) {
          gval = scaledPulse(xvec[isamp], tpulse, ypulse);
          gpval = derivative(xvec[isamp], tdpulse, dpulse);
          err2 = eyvec[isamp] * eyvec[isamp];
          sy += yvec[isamp] / err2;
          sg += gval / err2;
          sgp += gpval / err2;
          syg += yvec[isamp] * gval / err2;
          sygp += yvec[isamp] * gpval / err2;
          sgg += gval * gval / err2;
          sggp += gval * gpval / err2;
          sgpgp += gpval * gpval / err2;
          serr += 1.0 / err2;
        }
        dgg = sgg - sg * sg / serr;
        dggp = sggp - sg * sgp / serr;
        dgpgp = sgpgp - sgp * sgp / serr;
        dyg = syg - sy * sg / serr;
        dygp = sygp - sy * sgp / serr;
        dg = dgg * dgpgp - dggp * dggp;

        if (fabs(dg) > EPS_DG) {
          cisampl = (dyg * dgpgp - dygp * dggp) / dg;
          cisatau = (dyg * dggp - dygp * dgg) / dg;
          cisped = (sy
              - (dyg * dgpgp * sg - dygp * dggp * sg + dyg * dggp * sgp - dygp * dgg * sgp) / dg)
              / serr;

          if (fabs(cisampl) > EPS_DG) {
            cistau = cisatau / cisampl;
            if (cistau > m_maxTau)
              cistau = m_maxTau;
            else if (cistau < m_minTau) cistau = m_minTau;
          } else {
            cistau = 0.0;
          }
        } else {
          cisampl = 0.0;
          cisatau = 0.0;
          cistau = 0.0;
          cisped = sy / serr;
        }

        if (msgLvl(MSG::VERBOSE)) {
          msg(MSG::VERBOSE) << " sy=" << sy
                            << " sg=" << sg
                            << " sgp=" << sgp
                            << " syg=" << syg
                            << " sygp=" << sygp
                            << " sgg=" << sgg
                            << " sggp=" << sggp
                            << " sgpgp=" << sgpgp << endmsg;

          msg(MSG::VERBOSE) << " dgg=" << dgg
                            << " dggp=" << dggp
                            << " sgpgp=" << sgpgp
                            << " dyg=" << dyg
                            << " dygp=" << dygp
                            << " dg=" << dg << endmsg;

          msg(MSG::VERBOSE) << " cistau=" << cistau
                            << " cisped=" << cisped
                            << " cisampl=" << cisampl << endmsg;
        }

        // Determine Chi2 for pulse shape + leakage fit CIS Part C
        cischi2 = 0.0;
        m_fnParameters[0] = cistau;
        m_fnParameters[1] = cisped;
        m_fnParameters[2] = cisampl;
        for (int isamp = 0; isamp < nfit; ++isamp) {
          gval = scaledPulse(xvec[isamp], tpulse, ypulse);
          leakage = pulse(xvec[isamp], tleak, yleak);
          // Subtract leakage from samples
          yvec[isamp] = yvec0[isamp] - leakage;
          xd = yvec[isamp] - gval;
          cischi2 = cischi2 + (xd * xd) / (eyvec[isamp] * eyvec[isamp]);

          ATH_MSG_VERBOSE( " yvec0[" << isamp << "]=" << yvec0[isamp]
                          << " yvec[" << isamp << "]=" << yvec[isamp]
                          << " leakage=" << leakage
                          << " gval=" << gval
                          << " xd=" << xd );
        }
        cischi2 = cischi2 / (nfit - 3.0);
        
        ATH_MSG_VERBOSE ( " cischi2=" << cischi2 );
      }

      // Determine which set of parameters to use from CIS fit methods based on minimum chi2
      if ((cischi2 < leakchi2) && !fixedTime) {
        tau = cistau;
        ped = cisped;
        ampl = cisampl;
        tempChi2 = cischi2;
      } else {
        tau = leaktau;
        ped = leakped;
        ampl = leakampl;
        tempChi2 = leakchi2;
      }
    }
    // End of fit for CIS events

    else {    // Physics and laser events

      if (niter == 1) { /* For first iteration, also calculate 2-Parameter Fit
       for pedestal and amplitude */
        double t0fit_old = m_t0Fit;
        m_t0Fit = 0.0;

        sy = 0.0;
        sg = 0.0;
        sgg = 0.0;
        syg = 0.0;
        serr = 0.0;

        for (int isamp = 0; isamp < nfit; ++isamp) {
          if (!m_idolas) {
            // Use initial values for speeding up the physics events
            int jsamp = (int) xvec[isamp] - delta_peak;
            if (jsamp < 0)
              jsamp = 0;
            else if (jsamp >= nfit) jsamp = nfit - 1;

            if (igain == 0) {
              gval = m_gPhysLo[jsamp];
            } else {
              gval = m_gPhysHi[jsamp];
            }
          } else {
            gval = scaledPulse(xvec[isamp], tpulse, ypulse);
          }
          err2 = eyvec[isamp] * eyvec[isamp];
          sy += yvec0[isamp] / err2;
          sg += gval / err2;
          syg += yvec0[isamp] * gval / err2;
          sgg += gval * gval / err2;
          serr += 1.0 / err2;
        }
        fixtau = 0.0;
        dgg0 = sg * sg - serr * sgg;
        if (fabs(dgg0) > EPS_DG) {
          fixampl = (sy * sg - serr * syg) / dgg0;
          fixped = (syg * sg - sy * sgg) / dgg0;
          ATH_MSG_VERBOSE ( "  2-par fit:"
                           << " fixampl = " << fixampl
                           << " fixped = " << fixped );
        } else {
          fixampl = 0.0;
          fixped = sy / serr;
          ATH_MSG_VERBOSE ( "  2-par fit:"
                          << " small dgg0 = " << dgg0
                          << ", fixampl = " << fixampl
                          << " fixped = " << fixped  );
        }
        m_fnParameters[0] = fixtau; /* 2-Par fit Calculate chi2 for physics events */
        m_fnParameters[1] = fixped;
        m_fnParameters[2] = fixampl;
        fixchi2 = 0.0;
        for (int isamp = 0; isamp < nfit; ++isamp) {
          dc = yvec0[isamp] - scaledPulse(xvec[isamp], tpulse, ypulse);
          fixchi2 = fixchi2 + (dc * dc) / (eyvec[isamp] * eyvec[isamp]);
          ATH_MSG_VERBOSE ( "   isamp= " << isamp
                           << " yvec0[" << isamp << "]= " << yvec0[isamp]
                           << " eyvec[" << isamp << "]= " << eyvec[isamp]
                           << " fixchi2= " << fixchi2  );
        }
        fixchi2 = fixchi2 / (nfit - 2.0);
        ATH_MSG_VERBOSE ( "  fixchi2/(nfit-2.0)=" << fixchi2 << " nfit=" << nfit );

        m_t0Fit = t0fit_old;

        // restore initial parameters for pulse shape functions - to be used in 3-par fit
        m_fnParameters[0] = 0.0;
        m_fnParameters[1] = 0.0;
        m_fnParameters[2] = 1.0;
      } /* end of 2-par fit in first iteration */

      if (fixedTime) {
        m_t0Fit = 0.0;
        tau = fixtau;
        ped = fixped;
        ampl = fixampl;
        tempChi2 = oldchi2 = -fabs(fixchi2);
      } else {

        sy = 0.0;
        sg = 0.0;
        sgp = 0.0;
        syg = 0.0;
        sygp = 0.0;
        sgg = 0.0;
        sggp = 0.0;
        sgpgp = 0.0;
        serr = 0.0;

        for (int isamp = 0; isamp < nfit; ++isamp) {
          if ((niter == 1) && (!m_idolas)) {
            // Use initial function values stored in array for niter=1 physics
            // XXX: double->int
            int jsamp = (int) xvec[isamp] - delta_peak;
            if (jsamp < 0)
              jsamp = 0;
            else if (jsamp >= nfit) jsamp = nfit - 1;

            if (igain == 0) {       // igain ==0 => low-gain
              gval = m_gPhysLo[jsamp];
              gpval = m_dgPhysLo[jsamp];
            } else { 	          // must be igain==1 => high-gain
              gval = m_gPhysHi[jsamp];
              gpval = m_dgPhysHi[jsamp];
            }
          } else {
            // Use the respective function values
            gval = scaledPulse(xvec[isamp], tpulse, ypulse);
            gpval = derivative(xvec[isamp], tdpulse, dpulse);
          }

          err2 = eyvec[isamp] * eyvec[isamp];
          sy += yvec0[isamp] / err2;
          sg += gval / err2;
          sgp += gpval / err2;
          syg += yvec0[isamp] * gval / err2;
          sygp += yvec0[isamp] * gpval / err2;
          sgg += gval * gval / err2;
          sggp += gval * gpval / err2;
          sgpgp += gpval * gpval / err2;
          serr += 1.0 / err2;

          ATH_MSG_VERBOSE ( " isamp=" << isamp
                          << " gval=" << gval
                          << " sg=" << sg
                          << " gpval=" << gpval
                          << " sgp=" << sgp );
        }

        dgg = sgg - sg * sg / serr;
        dggp = sggp - sg * sgp / serr;
        dgpgp = sgpgp - sgp * sgp / serr;
        dyg = syg - sy * sg / serr;
        dygp = sygp - sy * sgp / serr;
        dg = dgg * dgpgp - dggp * dggp;

        // Fit for time, pedestal, and amplitude
        if (fabs(dg) > EPS_DG) {
          // Amplitude           : ampl
          ampl = (dyg * dgpgp - dygp * dggp) / dg;
          // and Amplitude * time: atau
          atau = (dyg * dggp - dygp * dgg) / dg;
          // Pedestal
          ped = (sy - ((dyg * dgpgp - dygp * dggp) * sg + (dyg * dggp - dygp * dgg) * sgp) / dg)
              / serr;

          if (fabs(ampl) > EPS_DG) {
            // Time
            tau = atau / ampl;
            if (tau > m_maxTau)
              tau = m_maxTau;
            else if (tau < m_minTau) tau = m_minTau;
          } else {
            tau = 0.0;
          }
        } else {
          ampl = 0.0;
          atau = 0.0;
          tau = 0.0;
          ped = sy / serr;
        }

        if (msgLvl(MSG::VERBOSE)) {
          msg(MSG::VERBOSE) << " ped=" << ped << endmsg;
          msg(MSG::VERBOSE) << " sy=" << sy
                            << " sg=" << sg
                            << " sgp=" << sgp << endmsg;

          msg(MSG::VERBOSE) << " syg=" << syg
                            << " sygp=" << sygp
                            << " sgg=" << sgg << endmsg;

          msg(MSG::VERBOSE) << " sggp=" << sggp
                            << " sgpgp=" << sgpgp << endmsg;

          msg(MSG::VERBOSE) << " ampl = (dyg*dgpgp - dygp*dggp)= " << ampl << endmsg;

          msg(MSG::VERBOSE) << " dyg=" << dyg
                            << " dgpgp=" << dgpgp
                            << " dyg*dgpgp=" << (dyg * dgpgp) << endmsg;

          msg(MSG::VERBOSE) << " dygp=" << dygp
                            << " dggp=" << dggp
                            << " dygp*dggp=" << (dygp * dggp) << endmsg;

          msg(MSG::VERBOSE) << " dyg=" << dyg
                            << " dggp=" << dggp
                            << " dyg*dggp=" << (dyg * dggp) << endmsg;

          msg(MSG::VERBOSE) << " dygp=" << dygp
                            << " dgg=" << dgg
                            << " dygp*dgg=" << (dygp * dgg) << endmsg;

          msg(MSG::VERBOSE) << " dg=" << dg
                            << " atau=" << atau
                            << " tau=" << tau << endmsg;
        }

        m_fnParameters[0] = tau;
        m_fnParameters[1] = ped;
        m_fnParameters[2] = ampl;

        tempChi2 = 0;
        // Calculate chi2 for physics and laser events
        for (int isamp = 0; isamp < nfit; ++isamp) {
          dc = yvec0[isamp] - scaledPulse(xvec[isamp], tpulse, ypulse);
          tempChi2 = tempChi2 + (dc * dc) / (eyvec[isamp] * eyvec[isamp]);
          ATH_MSG_VERBOSE ( " isamp=" << isamp
                            << " yvec0[" << isamp << "]=" << yvec0[isamp]
                            << " eyvec[" << isamp << "]=" << eyvec[isamp]
                            << " dc=" << dc << " chi2=" << tempChi2 );
        }
        tempChi2 = tempChi2 / (nfit - 3.0);
        ATH_MSG_VERBOSE ( " chi2/(nfit-3.0)=" << tempChi2 << " nfit=" << nfit );
      } // end if fixedTime
    } // end of physics and laser specific part

    if (msgLvl(MSG::VERBOSE))
      msg(MSG::VERBOSE) << " t0fit: " << m_t0Fit << ((tau < 0.0) ? " - " : " + ") << fabs(tau);

    // avoid infinite loop at the boundary
    if (fabs(m_t0Fit - m_maxTime) < 0.001 && tau >= 0.0) { // trying to go outside max boudary second time
      m_t0Fit = fixtau + (m_minTime - fixtau) * niter / m_maxIterate; // jump to negative time
      if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << " jumping to " << m_t0Fit << endmsg;
      tempChi2 = MAX_CHI2;
    } else if (fabs(m_t0Fit - m_minTime) < 0.001 && tau <= 0.0) { // trying to go outside min boudary second time
      m_t0Fit = fixtau + (m_maxTime - fixtau) * niter / m_maxIterate; // jump to positive time
      if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << " jumping to " << m_t0Fit << endmsg;
      tempChi2 = MAX_CHI2;
    } else {

      // Iteration with parameter for time 
      m_t0Fit += tau;
      if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << " = " << m_t0Fit << endmsg;

      // Check if total time does not exceed the limits:
      if (m_t0Fit > m_maxTime) {
        m_t0Fit = m_maxTime;
        tempChi2 = MAX_CHI2;
      } else if (m_t0Fit < m_minTime) {
        m_t0Fit = m_minTime;
        tempChi2 = MAX_CHI2;
      }
    }
    
    // save values of the best iteration
    if (tempChi2 < chi2) {
      time = m_t0Fit;
      pedestal = ped;
      amplitude = ampl;
      chi2 = tempChi2;
    }

    if (!fixedTime && tempChi2 < MAX_CHI2 && fabs(tau) < EPS_DG) { // too small tau
      if (m_t0Fit > fixtau)
        m_t0Fit = fixtau + (m_minTime - fixtau) * niter / m_maxIterate; // jump to negative time
      else
        m_t0Fit = fixtau + (m_maxTime - fixtau) * niter / m_maxIterate; // jump to positive time

      ATH_MSG_VERBOSE ( " too small tau - jump to " << m_t0Fit );
    }
    
    ATH_MSG_VERBOSE ( " iter=" << niter
                     << " t0fit=" << m_t0Fit
                     << " phase=" << tau
                     << " ped=" << ped
                     << " ampl=" << ampl
                     << " chi2=" << tempChi2 );

  } while (fabs(tempChi2 - oldchi2) > DELTA_CHI2 && (niter < m_maxIterate));
  
  //NGO never use the 2par fit result if non-pedestal event was detected!
//   if ((fabs(fixchi2) <= fabs(p_chi2)) && 
//       !(m_idocis && ((m_cischan == -1) || (channel == m_cischan)))) {
//     /* results from 2-par fit */
//     p_time      = fixtau;
//     p_pedestal  = fixped;
//     p_amplitude = fixampl;
//     p_chi2      = - fabs(fixchi2);
//   }

// TD: fit converged, now one extra iteration, leaving out the samples that
// are more then m_MaxTimeFromPeak ns from the peak. We want to avoid to 
// extrapolate the pulse shape beyond the region where it is known.

// Do it only in case when at least last sample is beyond m_MaxTimeFromPeak:
  if ((nfit - 1 - m_iPeak0) * DTIME > time + m_maxTimeFromPeak) {

    ATH_MSG_VERBOSE ( "Result before last iteration:"
                     << " Time=" << time
                     << " Ped=" << pedestal
                     << " Amplitude=" << amplitude
                     << " Chi2=" << chi2);

    m_t0Fit = time;
    int nfit_real;

    // parameters for pulse shape functions
    // 0. phase
    m_fnParameters[0] = 0.0;
    // 1. pedestal 
    m_fnParameters[1] = 0.0;
    // 2. amplitude
    m_fnParameters[2] = 1.0;

    // CIS events linear fit
    if (m_idocis && ((m_cischan == -1) || (channel == m_cischan))) {
      if (!fixedTime) {
        ATH_MSG_VERBOSE ( "Fit time with leakage" );
        // CIS Part (A): fit for time using leakage pulse
        sllp = 0.0;
        sylp = 0.0;
        slplp = 0.0;
        for (int isamp = 0;
            (isamp < nfit) && (DTIME * (isamp - m_iPeak0) - m_t0Fit < m_maxTimeFromPeak); ++isamp) {
          ATH_MSG_VERBOSE ( "Lo gain leakage xvec[" << isamp << "]=" << xvec[isamp] );

          leakage = pulse(xvec[isamp], tleak, yleak);
          dleakage = derivative(xvec[isamp], tdleak, dleak);

          // Samples with pedestal subtracted
          yvec[isamp] = yvec0[isamp] - pedg;

          ATH_MSG_VERBOSE ( " yvec[" << isamp << "]=" << yvec[isamp]
                           << " yvec0[" << isamp << "]=" << yvec0[isamp]
                           << " pedg=" << pedg );

          sllp += leakage * dleakage;
          sylp += yvec[isamp] * dleakage;
          slplp += dleakage * dleakage;
        }
        // Also allow for fixed-time fit to CIS events
        if (fabs(slplp) > EPS_DG && !fixedTime) {
          leaktau = (sllp - sylp) / slplp;
          // Also have to check the range for leaktau
          if (leaktau > m_maxTau)
            leaktau = m_maxTau;
          else if (leaktau < m_minTau) leaktau = m_minTau;
        } else {
          leaktau = 0.0;
        }

        ATH_MSG_VERBOSE ( " sllp=" << sllp
                        << " sylp=" << sylp
                        << " slplp=" << slplp
                        << " leaktau=" << leaktau );

        // CIS Part (B): using phase determined in part (A), 
        // subtract leakage pedestal and fit for amplitude, pedestal
        m_fnParameters[0] = leaktau;
        sy = 0.0;
        sg = 0.0;
        syg = 0.0;
        sgg = 0.0;
        serr = 0.0;
        for (int isamp = 0;
            (isamp < nfit) && (DTIME * (isamp - m_iPeak0) - m_t0Fit < m_maxTimeFromPeak); ++isamp) {
          leakage = pulse(xvec[isamp], tleak, yleak);
          gval = scaledPulse(xvec[isamp], tpulse, ypulse);
          yvec[isamp] = yvec0[isamp] - leakage;

          ATH_MSG_VERBOSE ( " yvec[" << isamp << "]=" << yvec[isamp]
                           << " yvec0[" << isamp << "]=" << yvec0[isamp]
                           << " leakage=" << leakage );

          err2 = eyvec[isamp] * eyvec[isamp];
          sy += yvec[isamp] / err2;
          sg += gval / err2;
          syg += yvec[isamp] * gval / err2;
          sgg += gval * gval / err2;
          serr += 1.0 / err2;
        }
        dgg0 = sg * sg - serr * sgg;
        if (fabs(dgg0) > EPS_DG) {
          leakampl = (sy * sg - serr * syg) / dgg0;
          leakped = (syg * sg - sy * sgg) / dgg0;
        } else {
          leakampl = 0.0;
          leakped = sy / serr;
        }

        // Determine Chi2 for corresponding function  for CIS leakage + pulse
        ATH_MSG_VERBOSE( " Determine Chi2 for CIS leakage + pulse");

        leakchi2 = 0.0;
        nfit_real = 0;
        m_fnParameters[0] = leaktau;
        m_fnParameters[1] = leakped;
        m_fnParameters[2] = leakampl;
        for (int isamp = 0;
            (isamp < nfit) && (DTIME * (isamp - m_iPeak0) - m_t0Fit < m_maxTimeFromPeak); ++isamp) {
          ++nfit_real;
          gval = scaledPulse(xvec[isamp], tpulse, ypulse);
          leakage = pulse(xvec[isamp], tleak, yleak);
          xd = yvec0[isamp] - (gval + leakage);
          leakchi2 = leakchi2 + (xd * xd) / (eyvec[isamp] * eyvec[isamp]);

          ATH_MSG_VERBOSE ( " isamp=" << isamp
                           << " yvec0[" << isamp << "]=" << yvec0[isamp]
                           << " gval=" << gval
                           << ", leakage=" << leakage
                           << ", xd=" << xd );
        }
        leakchi2 = leakchi2 / (nfit_real - 3.0);

        ATH_MSG_VERBOSE ( " leaktau=" << leaktau
                         << " leakped=" << leakped
                         << " leakampl=" << leakampl
                         << " leakchi2=" << leakchi2 );

        // CIS Part C: Least-squares fit with 3 parameters for pulse+leakage
        m_fnParameters[0] = 0.0;
        m_fnParameters[1] = 0.0;
        m_fnParameters[2] = 0.0;
        for (int isamp = 0;
            (isamp < nfit) && (DTIME * (isamp - m_iPeak0) - m_t0Fit < m_maxTimeFromPeak); ++isamp) {

          leakage = pulse(xvec[isamp], tleak, yleak);

          // Subtract leakage from samples
          yvec[isamp] = yvec0[isamp] - leakage;

          ATH_MSG_VERBOSE ( " isamp=" << isamp
                           << " yvec0[" << isamp << "]=" << yvec0[isamp]
                           << " leakage=" << leakage
                           << " yvec[" << isamp << "]=" << yvec[isamp] );
        }
        m_fnParameters[0] = 0.0;
        m_fnParameters[1] = 0.0;
        m_fnParameters[2] = 1.0;
        sy = 0.0;
        sg = 0.0;
        sgp = 0.0;
        syg = 0.0;
        sygp = 0.0;
        sgg = 0.0;
        sggp = 0.0;
        sgpgp = 0.0;
        serr = 0.0;
        for (int isamp = 0;
            (isamp < nfit) && (DTIME * (isamp - m_iPeak0) - m_t0Fit < m_maxTimeFromPeak); ++isamp) {
          gval = scaledPulse(xvec[isamp], tpulse, ypulse);
          gpval = derivative(xvec[isamp], tdpulse, dpulse);
          err2 = eyvec[isamp] * eyvec[isamp];
          sy += yvec[isamp] / err2;
          sg += gval / err2;
          sgp += gpval / err2;
          syg += yvec[isamp] * gval / err2;
          sygp += yvec[isamp] * gpval / err2;
          sgg += gval * gval / err2;
          sggp += gval * gpval / err2;
          sgpgp += gpval * gpval / err2;
          serr += 1.0 / err2;
        }
        dgg = sgg - sg * sg / serr;
        dggp = sggp - sg * sgp / serr;
        dgpgp = sgpgp - sgp * sgp / serr;
        dyg = syg - sy * sg / serr;
        dygp = sygp - sy * sgp / serr;
        dg = dgg * dgpgp - dggp * dggp;

        if (fabs(dg) > EPS_DG) {
          cisampl = (dyg * dgpgp - dygp * dggp) / dg;
          cisatau = (dyg * dggp - dygp * dgg) / dg;
          cisped = (sy
              - (dyg * dgpgp * sg - dygp * dggp * sg + dyg * dggp * sgp - dygp * dgg * sgp) / dg)
              / serr;

          if (fabs(cisampl) > EPS_DG) {
            cistau = cisatau / cisampl;
            if (cistau > m_maxTau)
              cistau = m_maxTau;
            else if (cistau < m_minTau) cistau = m_minTau;
          } else {
            cistau = 0.0;
          }
        } else {
          cisampl = 0.0;
          cisatau = 0.0;
          cistau = 0.0;
          cisped = sy / serr;
        }

        if (msgLvl(MSG::VERBOSE)) {
          msg(MSG::VERBOSE) << " sy=" << sy
                            << " sg=" << sg
                            << " sgp=" << sgp
                            << " syg=" << syg
                            << " sygp=" << sygp
                            << " sgg=" << sgg
                            << " sggp=" << sggp
                            << " sgpgp=" << sgpgp << endmsg;

          msg(MSG::VERBOSE) << " dgg=" << dgg
                            << " dggp=" << dggp
                            << " sgpgp=" << sgpgp
                            << " dyg=" << dyg
                            << " dygp=" << dygp
                            << " dg=" << dg << endmsg;

          msg(MSG::VERBOSE) << " cistau=" << cistau
                            << " cisped=" << cisped
                            << " cisampl=" << cisampl << endmsg;
        }

        // Determine Chi2 for pulse shape + leakage fit CIS 
        cischi2 = 0.0;
        nfit_real = 0;
        m_fnParameters[0] = cistau;
        m_fnParameters[1] = cisped;
        m_fnParameters[2] = cisampl;
        for (int isamp = 0;
            (isamp < nfit) && (DTIME * (isamp - m_iPeak0) - m_t0Fit < m_maxTimeFromPeak); ++isamp) {
          ++nfit_real;
          gval = scaledPulse(xvec[isamp], tpulse, ypulse);
          leakage = pulse(xvec[isamp], tleak, yleak);
          // Subtract leakage from samples
          yvec[isamp] = yvec0[isamp] - leakage;
          xd = yvec[isamp] - gval;
          cischi2 = cischi2 + (xd * xd) / (eyvec[isamp] * eyvec[isamp]);

          ATH_MSG_VERBOSE ( " yvec0[" << isamp << "]=" << yvec0[isamp]
                           << " yvec[" << isamp << "]=" << yvec[isamp]
                           << " leakage=" << leakage
                           << " gval=" << gval
                           << " xd=" << xd );
        }
        cischi2 = cischi2 / (nfit_real - 3.0);

        ATH_MSG_VERBOSE ( " cischi2=" << cischi2 );

        // Determine which set of parameters to use from CIS fit methods based on minimum chi2
        if ((cischi2 < leakchi2) && !fixedTime) {
          tau = cistau;
          ped = cisped;
          ampl = cisampl;
          tempChi2 = cischi2;
        } else {
          tau = leaktau;
          ped = leakped;
          ampl = leakampl;
          tempChi2 = leakchi2;
        }
        // End of fit for CIS events
      }
    } else {    // Physics and laser events
      if (!fixedTime) {

        // restore initial parameters for pulse shape functions - to be used in 3-par fit
        m_fnParameters[0] = 0.0;
        m_fnParameters[1] = 0.0;
        m_fnParameters[2] = 1.0;

        sy = 0.0;
        sg = 0.0;
        sgp = 0.0;
        syg = 0.0;
        sygp = 0.0;
        sgg = 0.0;
        sggp = 0.0;
        sgpgp = 0.0;
        serr = 0.0;

        for (int isamp = 0;
            (isamp < nfit) && (DTIME * (isamp - m_iPeak0) - m_t0Fit < m_maxTimeFromPeak); ++isamp) {
          // Use the respective function values
          gval = scaledPulse(xvec[isamp], tpulse, ypulse);
          gpval = derivative(xvec[isamp], tdpulse, dpulse);

          err2 = eyvec[isamp] * eyvec[isamp];
          sy += yvec0[isamp] / err2;
          sg += gval / err2;
          sgp += gpval / err2;
          syg += yvec0[isamp] * gval / err2;
          sygp += yvec0[isamp] * gpval / err2;
          sgg += gval * gval / err2;
          sggp += gval * gpval / err2;
          sgpgp += gpval * gpval / err2;
          serr += 1.0 / err2;

          ATH_MSG_VERBOSE ( " isamp=" << isamp
                           << " gval=" << gval
                           << " sg=" << sg
                           << " gpval=" << gpval
                           << " sgp=" << sgp );
        }

        dgg = sgg - sg * sg / serr;
        dggp = sggp - sg * sgp / serr;
        dgpgp = sgpgp - sgp * sgp / serr;
        dyg = syg - sy * sg / serr;
        dygp = sygp - sy * sgp / serr;
        dg = dgg * dgpgp - dggp * dggp;

        // Fit for time, pedestal, and amplitude
        if (fabs(dg) > EPS_DG) {
          // Amplitude           : ampl 
          ampl = (dyg * dgpgp - dygp * dggp) / dg;
          // and Amplitude * time: atau
          atau = (dyg * dggp - dygp * dgg) / dg;
          // Pedestal
          ped = (sy - ((dyg * dgpgp - dygp * dggp) * sg + (dyg * dggp - dygp * dgg) * sgp) / dg)
              / serr;

          if (fabs(ampl) > EPS_DG) {
            // Time
            tau = atau / ampl;
            if (tau > m_maxTau)
              tau = m_maxTau;
            else if (tau < m_minTau) tau = m_minTau;
          } else {
            tau = 0.0;
          }
        } else {
          ampl = 0.0;
          atau = 0.0;
          tau = 0.0;
          ped = sy / serr;
        }

        if (msgLvl(MSG::VERBOSE)) {
          msg(MSG::VERBOSE) << " ped=" << ped << endmsg;
          msg(MSG::VERBOSE) << " sy=" << sy
                            << " sg=" << sg
                            << " sgp=" << sgp << endmsg;

          msg(MSG::VERBOSE) << " syg=" << syg
                            << " sygp=" << sygp
                            << " sgg=" << sgg << endmsg;

          msg(MSG::VERBOSE) << " sggp=" << sggp
                            << " sgpgp=" << sgpgp << endmsg;

          msg(MSG::VERBOSE) << " ampl = (dyg*dgpgp - dygp*dggp)= " << ampl << endmsg;
          msg(MSG::VERBOSE) << " dyg=" << dyg
                            << " dgpgp=" << dgpgp
                            << " dyg*dgpgp=" << (dyg * dgpgp) << endmsg;

          msg(MSG::VERBOSE) << " dygp=" << dygp
                            << " dggp=" << dggp
                            << " dygp*dggp=" << (dygp * dggp) << endmsg;

          msg(MSG::VERBOSE) << " dyg=" << dyg
                            << " dggp=" << dggp
                            << " dyg*dggp=" << (dyg * dggp) << endmsg;

          msg(MSG::VERBOSE) << " dygp=" << dygp
                            << " dgg=" << dgg
                            << " dygp*dgg=" << (dygp * dgg) << endmsg;

          msg(MSG::VERBOSE) << " dg=" << dg
                            << " atau=" << atau
                            << " tau=" << tau << endmsg;
        }

        m_fnParameters[0] = tau;
        m_fnParameters[1] = ped;
        m_fnParameters[2] = ampl;

        tempChi2 = 0;
        nfit_real = 0;
        // Calculate chi2 for physics and laser events
        for (int isamp = 0;
            (isamp < nfit) && (DTIME * (isamp - m_iPeak0) - m_t0Fit < m_maxTimeFromPeak); ++isamp) {
          ++nfit_real;
          dc = yvec0[isamp] - scaledPulse(xvec[isamp], tpulse, ypulse);
          tempChi2 = tempChi2 + (dc * dc) / (eyvec[isamp] * eyvec[isamp]);
          ATH_MSG_VERBOSE ( " isamp=" << isamp
                           << " yvec0[" << isamp << "]=" << yvec0[isamp]
                           << " eyvec[" << isamp << "]=" << eyvec[isamp]
                           << " dc=" << dc
                           << " chi2=" << tempChi2 );
        }
        tempChi2 = tempChi2 / (nfit_real - 3.0);
        ATH_MSG_VERBOSE ( " chi2/(nfit_real-3.0)=" << tempChi2
                         << " nfit_real=" << nfit_real );
      } // end if fixedTime
    } // end of physics and laser specific part

    if (msgLvl(MSG::VERBOSE))
      msg(MSG::VERBOSE) << " t0fit: " << m_t0Fit << ((tau < 0.0) ? " - " : " + ") << fabs(tau);
    // Iteration with parameter for time 
    m_t0Fit += tau;
    ATH_MSG_VERBOSE ( " = " << m_t0Fit );

    // Check if total time does not exceed the limits:
    if (m_t0Fit > m_maxTime) {
      m_t0Fit = m_maxTime;
      tempChi2 = MAX_CHI2;
    } else if (m_t0Fit < m_minTime) {
      m_t0Fit = m_minTime;
      tempChi2 = MAX_CHI2;
    }

    if (tempChi2 < MAX_CHI2) {
      time = m_t0Fit;
      pedestal = ped;
      amplitude = ampl;
      chi2 = tempChi2;
    } // otherwise using the previous iteration
    
  } // end if to use extra iteration
  
  ATH_MSG_VERBOSE ( "Result:"
                   << " Time=" << time
                   << " Ped=" << pedestal
                   << " Amplitude=" << amplitude
                   << " Chi2=" << chi2 );
}


/**
 * pulse interpolation
 */
double TileRawChannelBuilderFitFilterCool::pulse(double x, const std::vector<double> * xvec
    , const std::vector<double> * yvec, bool zeroOutside) const {

  int size1 = xvec->size() - 1;
  if (size1 < 1) return 0.0;

  const double delta = 1.e-6;
  
  double xpmin = xvec->at(0);
  double xpmax = xvec->at(size1);

  double xp = (x - m_iPeak0) * DTIME - m_t0Fit - m_fnParameters[0];

  double val = 0.0;
  double tdiv = (xpmax - xpmin) / size1;

  if (xp < xpmin + delta) {
    if (zeroOutside && xp < xpmin - delta)
      val = 0.0;
    else
      val = yvec->at(0);
#ifdef EXTRAPOLATE_TO_ZERO
    if (xp < xpmin - delta && val != 0.0) {
      double newval = val + ((yvec->at(1) - val) / tdiv) * (xp - xpmin);
      if (val * newval < 0.0) {
        val = 0.0;
      } else if (fabs(newval) < fabs(val)) {
        val = newval;
      }
    }
#endif
  } else if (xp > xpmax - delta) {
    if (zeroOutside && xp > xpmax + delta)
      val = 0.0;
    else
      val = yvec->at(size1);
#ifdef EXTRAPOLATE_TO_ZERO
    if (xp > xpmax + delta && val != 0.0) {
      double newval = val + ((yvec->at(size1 - 1) - val) / tdiv) * (xp - xpmax);
      if (val * newval < 0.0) {
        val = 0.0;
      } else if (fabs(newval) < fabs(val)) {
        val = newval;
      }
    }
#endif
  } else {
    int j = (int) ((xp - xpmin) / tdiv);
    val = yvec->at(j) + ((yvec->at(j + 1) - yvec->at(j)) / tdiv) * (xp - xvec->at(j));
  }
  
  return val;
}
