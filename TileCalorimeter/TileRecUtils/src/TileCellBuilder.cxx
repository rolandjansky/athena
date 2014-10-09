/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// small hack to enable datapool usage
#define private public
#define protected public
#include "TileEvent/TileCell.h"
#undef private
#undef protected
 
// Gaudi includes
#include "GaudiKernel/Bootstrap.h"

// Atlas includes
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "DataModel/DataPool.h"
// access all RawChannels inside container
#include "EventContainers/SelectAllObject.h" 
#include "xAODEventInfo/EventInfo.h"
#include "AthenaKernel/errorcheck.h"

// Calo includes
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloConditions/CaloAffectedRegionInfoVec.h"
#include "Identifier/IdentifierHash.h"

// Tile includes
#include "TileRecUtils/TileCellBuilder.h"
#include "CaloIdentifier/TileID.h"
#include "CaloIdentifier/TileTBID.h"
#include "TileIdentifier/TileHWID.h"
#include "CaloDetDescr/MbtsDetDescrManager.h"
#include "TileDetDescr/TileDetDescrManager.h"
#include "TileConditions/ITileBadChanTool.h"
#include "TileEvent/TileRawChannel.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileCellCollection.h"
#include "TileRecUtils/ITileRawChannelTool.h"

using xAOD::EventInfo;	  	
using CLHEP::MeV;
 
// uncomment line below for debug output
// #define ALLOW_DEBUG_COUT 1

static const InterfaceID IID_ITileCellBuilder("TileCellBuilder", 1, 0); 	 

const InterfaceID& TileCellBuilder::interfaceID( ) {
  return IID_ITileCellBuilder; 	 
}

//Constructor
TileCellBuilder::TileCellBuilder(const std::string& type, const std::string& name,
    const IInterface* parent)
  : AthAlgTool(type, name, parent)
  , m_rawChannelContainer("TileRawChannelCnt")
  , m_MBTSContainer("MBTSContainer")
  , m_E4prContainer("E4prContainer")
  , m_eneForTimeCut(35. * MeV) // keep time only for cells above 70 MeV (more than 35 MeV in at least one PMT to be precise)
  , m_eneForTimeCutMBTS(0.03675) // the same cut for MBTS, but in pC, corresponds to 3 ADC counts or 35 MeV
  , m_qualityCut(254) // cut on overflow in quality (if quality is 255 - assume that channel is bad)
  , m_eThreshold(-100000.)
  , m_maxTimeDiff(100000.)
  , m_maxTime (100000.)
  , m_minTime(-100000.)
  , m_maxChi2(100000.)
  , m_minChi2(-100000.)
  , m_thresholdNotSet(true)
  , m_fullSizeCont(true)
  , m_maskBadChannels(true)
  , m_fakeCrackCells(false)
  , m_tileID(0)
  , m_tileTBID(0)
  , m_tileHWID(0)
  , theDQstatus(0)
  , m_tileBadChanTool("TileBadChanTool")
  , m_tileToolEmscale("TileCondToolEmscale")
  , m_tileToolTiming("TileCondToolTiming")
  , m_beamInfo("TileBeamInfoProvider/TileBeamInfoProvider")
  , m_noiseFilterTools() // "TileRawChannelNoiseFilter/TileRawChannelNoiseFilter")
  , m_tileMgr(0)
  , m_mbtsMgr(0)
  , m_MBTSCells(0)
  , m_E4prCells(0)
  , m_RChType(TileFragHash::Default)
  , m_RChUnit(TileRawChannelUnit::ADCcounts)
  , m_maxTimeCorr(75.0)
{
  declareInterface<ICaloCellMakerTool>( this );
  declareInterface<TileCellBuilder>( this );

  memset(m_drawerEvtStatus, 0, sizeof(m_drawerEvtStatus));
  memset(m_drawerRunStatus, 0, sizeof(m_drawerRunStatus));
  memset(m_eventErrorCounter, 0, sizeof(m_eventErrorCounter));

  // never set energy to zero, but set it to some small number
  // this will help TopoCluster to assign proper weight to the cell if needed
  m_zeroEnergy = 0.5 * MeV; // half a MeV in both PMTs i.e. one MeV in a cell

  //!< normal channel energy threshold for masking
  m_minEneChan[0] = -5000. * MeV;
  //!< gap channel energy threshold for masking
  m_minEneChan[1] = -10000. * MeV;
  //!< MBTS channel energy threshold for masking (not used currently(
  m_minEneChan[2] = -999999. * MeV;

  declareProperty("TileRawChannelContainer", m_rawChannelContainer);  // Raw channel to convert
  declareProperty("MBTSContainer",           m_MBTSContainer);        // Where to put MBTS cells
  declareProperty("E4prContainer",           m_E4prContainer);        // Where to put E4'  cells
  declareProperty("TileDSPRawChannelContainer", m_dspRawChannelContainer = "TileRawChannelCnt");
  declareProperty("TileBadChanTool"        , m_tileBadChanTool);
  declareProperty("TileCondToolEmscale"    , m_tileToolEmscale);
  declareProperty("TileCondToolTiming"     , m_tileToolTiming);
  declareProperty("BeamInfo", m_beamInfo);
  declareProperty("NoiseFilterTools", m_noiseFilterTools);

  declareProperty( "MinEnergyChan", m_minEneChan[0]);
  declareProperty( "MinEnergyGap",  m_minEneChan[1]);
  declareProperty( "MinEnergyMBTS", m_minEneChan[2]);

  // Energy threshold in MeV that the Cell must exceed to be considered:
  declareProperty("EThreshold",m_eThreshold);

  // Maximum difference between times of two PMTs in cell:
  declareProperty("MaxTimeDiff", m_maxTimeDiff);

  // Maximum and minimum time for a cell to be included:
  declareProperty("MaxTime", m_maxTime);
  declareProperty("MinTime", m_minTime);

  // Maximum and Minimum fit quality for cell to be considered:
  declareProperty("MaxChi2", m_maxChi2);
  declareProperty("MinChi2", m_minChi2);

  declareProperty("fullSizeCont", m_fullSizeCont);

  // put zero energy in bad channels and recover from single-channel failure using second PMT is a cell
  declareProperty("maskBadChannels", m_maskBadChannels);

  // create fake E3/E4 crack scintillators with zero energy when they do not exist 
  declareProperty("fakeCrackCells", m_fakeCrackCells);

  // PMT energy will be set to this value if channel is bad
  declareProperty("BadChannelZeroEnergy", m_zeroEnergy);
  // PMT with energy above cut will preserve time info in ESD
  declareProperty("EneForTimeCut", m_eneForTimeCut);
  declareProperty("EneForTimeCutMBTS", m_eneForTimeCutMBTS);
  // PMT with quality greater than this cut will be masked
  declareProperty("QualityCut", m_qualityCut);

  // apply time correction taking numbers from CondDB (if not yet done in OF)
  declareProperty("correctTime", m_correctTime = false);

  // apply parabolic amplitude correction (if not yet done in OF without iterations)
  declareProperty("correctAmplitude", m_correctAmplitude = false);

  // merge DSP results with offline reco results
  declareProperty("mergeChannels", m_mergeChannels = false);

  // thresholds for parabolic amplitude correction 
  declareProperty("AmpMinForAmpCorrection", m_ampMinThresh = 15.0);
  declareProperty("TimeMinForAmpCorrection", m_timeMinThresh = -25.0);
  declareProperty("TimeMaxForAmpCorrection", m_timeMaxThresh = 25.0);

  declareProperty("SkipGain", m_skipGain = -1); // never skip any gain by default
}

/**
 * Destructor
 */
TileCellBuilder::~TileCellBuilder(){ 
}

/**
 * Initializer
 */
StatusCode TileCellBuilder::initialize() {
  
  const IGeoModelSvc *geoModel = 0;
  CHECK( service("GeoModelSvc", geoModel) );
  
  // dummy parameters for the callback:
  int dummyInt = 0;
  std::list<std::string> dummyList;
  
  if (geoModel->geoInitialized()) {
    return geoInit(dummyInt, dummyList);
  } else {
    CHECK( detStore()->regFcn(&IGeoModelSvc::geoInit, geoModel, &TileCellBuilder::geoInit, this) );
  }

  return StatusCode::SUCCESS;
}

StatusCode TileCellBuilder::geoInit(IOVSVC_CALLBACK_ARGS) {
  
  // retrieve MBTS and Tile detector manager, TileID helper and TileIfno from det store
  if (m_MBTSContainer.size() > 0) {
    if (detStore()->retrieve(m_mbtsMgr).isFailure()) {
      ATH_MSG_WARNING( "Unable to retrieve MbtsDetDescrManager from DetectorStore" );
      m_mbtsMgr = 0;
    }
  } else {
    m_mbtsMgr = 0;
  }

  CHECK( detStore()->retrieve(m_tileMgr) );
  CHECK( detStore()->retrieve(m_tileID) );
  CHECK( detStore()->retrieve(m_tileTBID) );
  CHECK( detStore()->retrieve(m_tileHWID) );

  //=== get TileBadChanTool
  CHECK( m_tileBadChanTool.retrieve() );

  //=== get TileBeamInfo
  CHECK( m_beamInfo.retrieve() );

  // access tools and store them
  CHECK( m_noiseFilterTools.retrieve() );

  //=== get TileCondToolEmscale
  CHECK( m_tileToolEmscale.retrieve() );

  //=== get TileCondToolTiming
  CHECK( m_tileToolTiming.retrieve() );

  reset(true, false);

  if (m_MBTSContainer.size() > 0)
    ATH_MSG_INFO( "Storing MBTS cells in " << m_MBTSContainer );

  if ( (TileCablingService::getInstance())->getCablingType() != TileCablingService::RUN2Cabling)
    m_E4prContainer = ""; // no E4' container for RUN1
  else if (m_E4prContainer.size() > 0)
    ATH_MSG_INFO( "Storing E4'  cells in " << m_E4prContainer );

  ATH_MSG_INFO( "TileCellBuilder initialization completed" );

  return StatusCode::SUCCESS;
}

void TileCellBuilder::reset(bool /* fullSizeCont */, bool printReset) {

  if (printReset) ATH_MSG_INFO( "Resetting options in " << name() );
  
  // check if any threshold was set in jobOptions
  m_thresholdNotSet = ((fabs(m_eThreshold + 100000.) < 1)
                       && (fabs(m_maxTimeDiff - 100000.) < 1)
                       && (fabs(m_maxTime - 100000.) < 1)
                       && (fabs(m_minTime + 100000.) < 1)
                       && (fabs(m_maxChi2 - 100000.) < 1)
                       && (fabs(m_minChi2 + 100000.) < 1));

  if (m_thresholdNotSet) {
    ATH_MSG_INFO( "none of thresholds set, all RawChannels will be converted to Cells");
  } else {
    ATH_MSG_INFO( "Ene threshold " << m_eThreshold << " MeV" );
    ATH_MSG_INFO( "max time diff " << m_maxTimeDiff << " ns" );
    ATH_MSG_INFO( "max time thr  " << m_maxTime << " ns" );
    ATH_MSG_INFO( "min time thr  " << m_minTime << " ns" );
    ATH_MSG_INFO( "max qual thr  " << m_maxChi2 );
    ATH_MSG_INFO( "min qual thr  " << m_minChi2 );
  }
  
  // prepare empty vector for all cell pointers
  m_fullSizeCont = true;
  m_allCells.resize(m_tileID->cell_hash_max(), 0);
  
  m_MBTSCells = NULL;
  m_E4prCells = NULL;

  ATH_MSG_INFO( "size of temp vector set to " << m_allCells.size() );
  ATH_MSG_INFO( "taking RawChannels from '" << m_rawChannelContainer << "'" );
}

void TileCellBuilder::set_type_and_unit(TileFragHash::TYPE type
    , TileRawChannelUnit::UNIT unit) {

  // this method might be called from TileROD_Decoder
  // otherwise it's not needed - type and unit are available from 
  // TileRawChannelContainer itself (see TileCellBuilder::process() below)

  m_RChType = type;
  m_RChUnit = unit;

  ATH_MSG_INFO( "type of container is '" << m_RChType << "'" );
  ATH_MSG_INFO( "RawChannel unit [0=ADC, 1=pCb, 2=CspCb, 3=MeV] are '" << m_RChUnit << "'" );
}

StatusCode TileCellBuilder::finalize() {

  ATH_MSG_INFO( "Finalizing" );

  return StatusCode::SUCCESS;
}

StatusCode TileCellBuilder::process(CaloCellContainer * theCellContainer) {

  //**
  //* Get TileRawChannels
  //**

  memset(m_drawerEvtStatus, 0, sizeof(m_drawerEvtStatus));

  const TileRawChannelContainer* rawChannels;
  if (evtStore()->retrieve(rawChannels, m_rawChannelContainer).isFailure()) {

    ATH_MSG_WARNING( " Could not find container " << m_rawChannelContainer );
    ATH_MSG_WARNING( " do not fill CaloCellContainer " );

  } else {
    
    ATH_MSG_DEBUG( "Container " << m_rawChannelContainer << " with TileRawChannles found ");

    m_RChType = rawChannels->get_type();
    m_RChUnit = rawChannels->get_unit();
    unsigned int m_bsflags = rawChannels->get_bsflags();
    if (m_correctAmplitude || m_correctTime) {
      int DataType = (m_bsflags & 0x30000000) >> 28;
      if (DataType < 3) { // real data
        m_maxTimeCorr = 63.9375; // 64-1/16 ns is hard limit in DSP
        if (m_correctAmplitude && ((m_bsflags & 0x3000000) != 0)) {
          ATH_MSG_WARNING( "Using results of Opt filter with interations from DSP, disabling amplitude correction" );
          m_correctAmplitude = false;
        }
        if (m_correctTime && ((m_bsflags & 0x3000000) == 0)) {
          ATH_MSG_WARNING( "Using results of Opt filter without interations from DSP, disabling time correction" );
          m_correctTime = false;
        }
      } else {
        m_maxTimeCorr = ((m_bsflags >> 27) & 1) ? 100.0 : 75.0; // 100 or 75 ns is the limit for 9 or 7 samples
        if (m_correctAmplitude && ((m_bsflags & 0x6000) != 0)) {
          ATH_MSG_WARNING( "Amplitude correction was done already in optimal filter, disabling it here" );
          m_correctAmplitude = false;
        }
        if (m_correctTime && ((m_bsflags & 0x1000) != 0)) {
          ATH_MSG_WARNING( "Time correction was done already in optimal filter, disabling it here" );
          m_correctTime = false;
        }
      }
    }

    if (m_MBTSContainer.size() > 0)
      m_MBTSCells = new TileCellContainer(SG::VIEW_ELEMENTS);
    else
      m_MBTSCells = NULL;

    if (m_E4prContainer.size() > 0)
      m_E4prCells = new TileCellContainer(SG::VIEW_ELEMENTS);
    else
      m_E4prCells = NULL;

    SelectAllObject<TileRawChannelContainer> selAll(rawChannels);
    SelectAllObject<TileRawChannelContainer>::const_iterator begin = selAll.begin();
    SelectAllObject<TileRawChannelContainer>::const_iterator end = selAll.end();

    if (m_mergeChannels
        && m_dspRawChannelContainer != m_rawChannelContainer
        && m_dspRawChannelContainer.size() > 0) {

      ATH_MSG_DEBUG( "Merging " << m_rawChannelContainer
                    << " and " << m_dspRawChannelContainer );
      const TileRawChannelContainer* dspChannels;
      if (evtStore()->retrieve(dspChannels, m_dspRawChannelContainer).isFailure()) {
        // no DSP channels, build cells from primary container
        ATH_MSG_DEBUG( " No " << m_dspRawChannelContainer << " found, nothing to merge " );

      } else {

        if (m_beamInfo->noiseFilterApplied()) {
          ATH_MSG_DEBUG( " Noise filter for " << m_dspRawChannelContainer 
                         << " was already applied, use it directly " );
        } else if (m_noiseFilterTools.size() > 0) {
          ATH_MSG_DEBUG( " Running noise filter on " << m_dspRawChannelContainer 
                         << " (i.e. on second container only) " );
          // apply noise filter on dsp container before merging it with offline contaier
          ToolHandleArray<ITileRawChannelTool>::iterator itrTool=m_noiseFilterTools.begin();
          ToolHandleArray<ITileRawChannelTool>::iterator endTool=m_noiseFilterTools.end();

          for (; itrTool != endTool; ++itrTool) {
            if ((*itrTool)->process(dspChannels).isFailure()) {
              ATH_MSG_ERROR( " Error status returned from noise filter " );
            } else {
              ATH_MSG_DEBUG( "Noise filter applied to the container" );
            }
          }
          m_beamInfo->setNoiseFilterApplied(true);
        }
        
        TileFragHash::TYPE dspType = dspChannels->get_type();
        TileRawChannelUnit::UNIT dspUnit = dspChannels->get_unit();
        unsigned int dspFlags = dspChannels->get_bsflags();
        int DataType = (dspFlags & 0x30000000) >> 28;
        float dspTimeCut = m_maxTimeCorr;
        bool dspCorrectAmplitude = false, dspCorrectTime = false;
        if (DataType < 3) { // real data
          dspTimeCut = 63.9375; // 64-1/16 ns is hard limit in DSP
          dspCorrectAmplitude = ((dspFlags & 0x3000000) == 0);
          dspCorrectTime = ((dspFlags & 0x3000000) != 0);
        } else { // dsp container contains resluts of offline reco
          dspTimeCut = ((dspFlags >> 27) & 1) ? 100.0 : 75.0; // 100 or 75 ns is the limit for 9 or 7 samples
        }

        SelectAllObject<TileRawChannelContainer> selAllDsp(dspChannels);
        SelectAllObject<TileRawChannelContainer>::const_iterator beginDsp = selAllDsp.begin();
        SelectAllObject<TileRawChannelContainer>::const_iterator endDsp = selAllDsp.end();

        std::vector<const TileRawChannel *> oflVec;
        std::vector<const TileRawChannel *> dspVec;

        SelectAllObject<TileRawChannelContainer>::const_iterator oflItr = begin;
        SelectAllObject<TileRawChannelContainer>::const_iterator dspItr = beginDsp;
        
        if (oflItr != end) {
          const TileRawChannel *p1 = (*oflItr);
          HWIdentifier id1 = p1->adc_HWID();

          for (; dspItr != endDsp; ++dspItr) {

            const TileRawChannel *p2 = (*dspItr);
            HWIdentifier id2 = p2->adc_HWID();

            if (id2 < id1) {
              dspVec.push_back(p2);
            } else if (id2 == id1) {
              oflVec.push_back(p1);
              ++oflItr;
              if (oflItr != end) {
                p1 = (*oflItr);
                id1 = p1->adc_HWID();
              } else {
                ++dspItr;
                break;
              }
            } else {
              while (id2 >= id1) {
                oflVec.push_back(p1);
                ++oflItr;
                if (oflItr != end) {
                  p1 = (*oflItr);
                  bool id2gtid1 = (id2 > id1);
                  id1 = p1->adc_HWID();
                  if (id2gtid1 && (id2 < id1)) {
                    dspVec.push_back(p2); // id2 is in the gap between old and new id1 - keep it
                  }
                } else {
                  if (id2 == id1) ++dspItr;
                  break;
                }
              }
              if (id2 >= id1) break;
            }
          }
          // copy all remaining channels 
          for (; oflItr != end; ++oflItr) {
            oflVec.push_back(*oflItr);
          }
        }
        for (; dspItr != endDsp; ++dspItr) {
          dspVec.push_back(*dspItr);
        }

        // build here with special iterator over 2 vectors
        DoubleVectorIterator<std::vector<const TileRawChannel *>, const TileRawChannel *> vecBeg(
            &oflVec, m_RChType, m_RChUnit, m_maxTimeCorr, m_correctAmplitude, m_correctTime,
            &dspVec, dspType, dspUnit, dspTimeCut, dspCorrectAmplitude, dspCorrectTime, this, 0);
        DoubleVectorIterator<std::vector<const TileRawChannel *>, const TileRawChannel *> vecEnd(
            &oflVec, m_RChType, m_RChUnit, m_maxTimeCorr, m_correctAmplitude, m_correctTime,
            &dspVec, dspType, dspUnit, dspTimeCut, dspCorrectAmplitude, dspCorrectTime, this, 2);

        if (msgLvl(MSG::DEBUG)) {
          msg(MSG::DEBUG) << " Calling build() method for rawChannels from two vectors" << endmsg;
          msg(MSG::DEBUG) << " offline vector size = " << oflVec.size()
                          << " dsp vector size = " << dspVec.size() << endmsg;
        }
        build(vecBeg, vecEnd, theCellContainer);
        begin = end;
      }

    }

    if (begin != end) { // no merging applied, use original raw channel container
        
      if (m_noiseFilterTools.size() > 0) {
        ATH_MSG_DEBUG( " Running noise filter on " << m_rawChannelContainer );
        // apply noise filter on input container before sending it to the build() method
        ToolHandleArray<ITileRawChannelTool>::iterator itrTool = m_noiseFilterTools.begin();
        ToolHandleArray<ITileRawChannelTool>::iterator endTool = m_noiseFilterTools.end();

        for (; itrTool != endTool; ++itrTool) {
          if ((*itrTool)->process(rawChannels).isFailure()) {
            ATH_MSG_ERROR( " Error status returned from noise filter " );
          } else {
            ATH_MSG_DEBUG( "Noise filter applied to the container" );
          }
        }
      }

      ATH_MSG_DEBUG( " Calling build() method for rawChannels from " << m_rawChannelContainer );
      build(begin, end, theCellContainer);
    }
    
    if (m_MBTSContainer.size() > 0) {
      if (evtStore()->record(m_MBTSCells, m_MBTSContainer, false).isFailure()) {
        ATH_MSG_ERROR( " Could not register TileCellContainer for MBTS with name " << m_MBTSContainer );
      }
    }

    if (m_E4prContainer.size() > 0) {
      if (evtStore()->record(m_E4prCells, m_E4prContainer, false).isFailure()) {
        ATH_MSG_ERROR( " Could not register TileCellContainer for E4'  with name " << m_E4prContainer );
      }
    }
    
    CaloCell_ID::SUBCALO caloNum = CaloCell_ID::TILE;
    //specify that a given calorimeter has been filled
    if (theCellContainer->hasCalo(caloNum)) {
      // log << MSG::WARNING << "CaloCellContainer has already been filled with TileCells (caloNum = " 
      //  <<	caloNum << ")" << endreq ;    
    }
    theCellContainer->setHasCalo(caloNum);
  }
  
  //enum EventFlagErrorState { NotSet, Warning, Error };
  EventInfo::EventFlagErrorState error = EventInfo::NotSet;
  // flag will contain status of a given event
  // every 4 bits - status of partitions LBA,LBC,EBA,EBC
  // bits 0-3   - there is a signal above threshold in partitions
  // bits 4-7   - there are channels with underflow (sample=0) in partition (since rel 17.2.6.4)
  // bits 8-11  - there are channels with overflow (sample=1023) in partition (since rel 17.2.6.4)
  // bits 12-15 - there are at least 16 drawers with bad quality in partition
  // bits 16-19 - maximal length of consecutive bad area (since rel 17.2.6.5)
  // bits 20-23 - there are at least 16 drawers which are completely masked in partition
  // bits 24-27 - there are at least 16 drawers which do not send data in partition
  // bits 28-31 - reserved for global good/warning/bad status
  // bits 20-27 are also used for module number which gives warning status (since release 17.2.6.5)
  //            in case of warning we are sure that bits which indicates error are not filled
  unsigned int flag = 0;
  
  int drConsecMaxMax = 0;
  int drConsecNum = 0;

  for (int p = 1; p < 5; ++p) {
    TileDrawerEvtStatus * evt = m_drawerEvtStatus[p];
    TileDrawerRunStatus * run = m_drawerRunStatus[p];
    int drAbsent = 0;
    int drMasked = 0;
    int drConsec = 0;
    int drConsecMax = 0;
    int hasBadQ = 0;
    int hasOver = 0;
    int hasUnder = 0;
    int hasSig = 0;
    for (int d = 0; d < 64; ++d) {
      if (evt[d].nChannels == 0) {
        ++drConsec;
        ++drAbsent;
        ++(run[d].drawerAbsent);
      } else if (evt[d].nMaskedChannels >= evt[d].nChannels) {
        ++drConsec;
        ++drMasked;
        ++(run[d].drawerMasked);
      } else {
        if (drConsec > drConsecMax) {
          drConsecMax = drConsec;
          if (drConsecMax > drConsecMaxMax) {
            drConsecMaxMax = drConsecMax;
            drConsecNum = ((p - 1) << 6) | (d - drConsec);
          }
        }
        drConsec = 0;
        if (evt[d].nMaskedChannels > 0) {
          ++(run[d].channelsMasked);
        }
        if (evt[d].nBadQuality) ++hasBadQ;
        if (evt[d].nOverflow) ++hasOver;
        if (evt[d].nUnderflow) ++hasUnder;
        if (evt[d].nSomeSignal) ++hasSig;
      }
    }
    if (drConsec != 0 && drConsecMax < 64) { // 64th drawer is bad - check transition from 64th to 1st drawer
      for (int d = 0; d < drConsecMax; ++d) {
        if (evt[d].nChannels == 0 || evt[d].nMaskedChannels >= evt[d].nChannels) {
          ++drConsec;
        } else {
          break;
        }
      }
      if (drConsec > drConsecMax) {
        drConsecMax = drConsec;
      }
    }
    unsigned int fl = 0;
    if (drAbsent > 15) {
      fl |= 0x01000000;
      error = EventInfo::Error;
    }
    if (drMasked > 15) {
      fl |= 0x00100000;
      error = EventInfo::Error;
    }
    //if (drConsecMax > 1)fl |= 0x00010000; // want to use these bits for length of consecutive area
    if (hasBadQ > 15) fl |= 0x00001000;
    if (hasOver) fl |= 0x00000100;
    if (hasUnder) fl |= 0x00000010;
    if (hasSig) fl |= 0x00000001;

#ifdef ALLOW_DEBUG_COUT
    std::cout<<"partition "<<p<<" drAbsent "<<drAbsent<<" drMasked "<<drMasked<<" drConsec "<<drConsecMax
    <<" hasBadQ "<<hasBadQ<<" hasOver "<<hasOver<<" hasUnder "<<hasUnder<<" hasSig "<<hasSig<<std::endl;
#endif
    flag |= fl << (p - 1);
  }

  // number of cosecutively masked modules (if it's > 15 we have error already set)
  flag |= (std::min(15, drConsecMaxMax) << 16);

  if (drConsecMaxMax > 1 && error < EventInfo::Warning) {
    // setting warning flag
    error = EventInfo::Warning;
    // putting starting module number of consecutive bad area
    // instead of bits which indicates 16 masked or 16 absent modules in partition
    flag |= (drConsecNum << 20);
#ifdef ALLOW_DEBUG_COUT
    std::cout<<"warning in partition " << (drConsecNum>>6)+1 << " for modules "
    <<(drConsecNum)%64 <<" - " <<(drConsecNum+drConsecMaxMax-1)%64 <<std::endl;
#endif
  }
  
#ifdef ALLOW_DEBUG_COUT
  std::cout<<"partition flag 0x0"<<std::hex<<flag<<std::dec<<" error "<<error<<std::endl;
#endif

  ++m_eventErrorCounter[error]; // error index is 0 or 1 or 2 here
  ++m_eventErrorCounter[3]; // count separately total number of events
  
  // retrieve EventInfo
  const EventInfo* eventInfo_c = 0;
  if (evtStore()->retrieve(eventInfo_c).isFailure()) {
    ATH_MSG_WARNING( " cannot retrieve EventInfo, will not set Tile information " );
  }
  EventInfo* eventInfo = 0;
  if (eventInfo_c) {
    eventInfo = const_cast<EventInfo*>(eventInfo_c);
  }

  if (eventInfo) {

    if (flag != 0) {
      ATH_MSG_DEBUG( " set eventInfo for Tile for this event to 0x" << MSG::hex << flag << MSG::dec );
      if (!eventInfo->setEventFlags(EventInfo::Tile, flag)) {
        ATH_MSG_WARNING( " cannot set eventInfo for Tile " );
      }
    }
    
    if (error != EventInfo::NotSet) {
      ATH_MSG_DEBUG( " set error bits for Tile for this event to " << error );
      if (!eventInfo->setErrorState(EventInfo::Tile, error)) {
        ATH_MSG_WARNING( " cannot set error state for Tile " );
      }
    }

  }
  
  // Execution completed.
  ATH_MSG_DEBUG( "TileCellBuilder execution completed." );

  return StatusCode::SUCCESS;
}

//************************************************************************
void TileCellBuilder::correctCell(TileCell* pCell, int correction, int pmt, int gain
    , float ener, float time, unsigned char iqual, unsigned char qbit, int ch_type) {
//************************************************************************

// Merge two pmts in one cell if needed
// and apply corrections

  // do not trust to energies below certain threshold
  if (ener < m_minEneChan[ch_type]) {
#ifdef ALLOW_DEBUG_COUT
    std::cout << "channel with negative energy " << ener << " => setting quality to 255" << std::endl;
#endif
    iqual = 255;
  }

  switch (correction) {
    case 1: // first pmt for this cell
      pCell->addEnergy(ener, pmt, gain);
      pCell->setTime(time); // overwrite time completely
      pCell->setQuality(iqual, qbit, pmt);
      pCell->setQuality(0, 0, 1 - pmt);
      break;
    case 2: // second pmt for this cell
      pCell->addEnergy(ener, pmt, gain);
      pCell->setTime(time, pmt); // calculate average time and timeDiff
      pCell->setQuality(iqual, qbit, pmt);
      break;
  }
}

unsigned char TileCellBuilder::qbits(int ros, int drawer, bool count_over
    , bool good_time, bool good_ener, bool overflow, bool underflow, bool overfit) {

  ++m_drawerEvtStatus[ros][drawer].nChannels;
  // new feature in rel 17.2.7 - count underflows and overflows
  if (count_over) {
    if (overflow) ++m_drawerEvtStatus[ros][drawer].nOverflow;
    if (underflow) ++m_drawerEvtStatus[ros][drawer].nUnderflow;
  }
#ifdef ALLOW_DEBUG_COUT
  if (overflow)  std::cout << "channel with overflow " << ((count_over)?"":"MBTS") << std::endl;
  if (underflow) std::cout << "channel with underflow " << ((count_over)?"":"MBTS") << std::endl;
  if (overfit)   std::cout << "channel with corrected overflow " << ((count_over)?"":"MBTS") << std::endl;
#endif

  unsigned char qbit = (overfit) ? (TileFragHash::FitFilter & TileCell::MASK_ALGO)
                                 : (m_RChType & TileCell::MASK_ALGO);
  if (good_time) qbit |= TileCell::MASK_TIME;
  if (overflow || underflow) qbit |= TileCell::MASK_OVER;
      
  if (good_ener) {
    qbit |= TileCell::MASK_AMPL;
    if (count_over) {
      ++m_drawerEvtStatus[ros][drawer].nSomeSignal;
    }
  }

  return qbit;
}

// masking for MBTS with single channel
bool TileCellBuilder::maskBadChannel(TileCell* pCell, HWIdentifier hwid) {
  int ros = m_tileHWID->ros(hwid);
  int drawer = m_tileHWID->drawer(hwid);
  int chan = m_tileHWID->channel(hwid);
  int gain = m_tileHWID->adc(hwid);
  int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
  TileBchStatus chStatus = m_tileBadChanTool->getAdcStatus(drawerIdx, chan, gain);

  // check quality first
  bool bad = ((int) pCell->qual1() > m_qualityCut);
  if (bad) {
    ++m_drawerEvtStatus[ros][drawer].nBadQuality;

  } else {
    // check bad status in DB
    bad = chStatus.isBad();

    // Now checking the DQ status
    if (!bad) {
      bad = !(theDQstatus->isAdcDQgood(ros, drawer, chan, gain)
            && m_beamInfo->isChanDCSgood(ros, drawer, chan));
    }
  }

  if (bad) {
    // only one channel in this cell and it is bad
    ++m_drawerEvtStatus[ros][drawer].nMaskedChannels;

    //pCell->setEnergy(m_zeroEnergy,0.0,TileID::LOWGAIN,CaloGain::INVALIDGAIN); // reset energy completely, indicate problem putting low gain
    //pCell->setTime(0.0); // reset time completely
    //pCell->setQuality(255,TileCell::MASK_BADCH,0); // reset quality flag for first pmt

    if (gain == CaloGain::INVALIDGAIN) {
      pCell->setEnergy(0.0, 0.0, TileID::LOWGAIN, CaloGain::INVALIDGAIN); // reset energy completely, indicate problem putting low gain
    } else {
      pCell->setEnergy(0.0, 0.0); // reset energy completely without changing the gain
    }
    pCell->setTime(-100.0); // reset time to big negative number to distinguish this bad cell from good cells
    pCell->setQuality(255, (TileCell::MASK_BADCH | (pCell->qbit1() & TileCell::MASK_ALGO)), 0); // reset quality flag for first pmt
    pCell->setQuality(0, TileCell::MASK_BADCH, 1); // reset quality flag for second pmt

    return true;

  } else if (chStatus.isBadTiming()) {
    pCell->setTime(0.0); // channel with bad timing - no cell time
    uint8_t qbit1 = pCell->qbit1() & (~(TileCell::MASK_TIME)); // clear time bit for first pmt
    pCell->setQuality(pCell->qual1(), qbit1, 0); // update qbits for first pmt
  }

  return false;
}
  

// masking for normal cells
bool TileCellBuilder::maskBadChannels(TileCell* pCell) {
  bool single_PMT_C10 = false;

  const CaloDetDescrElement* caloDDE = pCell->caloDDE();

  IdentifierHash hash1 = caloDDE->onl1();
  IdentifierHash hash2 = caloDDE->onl2();

  int gain1 = pCell->gain1();

  HWIdentifier ch_id1 = m_tileHWID->channel_id(hash1);

  int ros1 = m_tileHWID->ros(ch_id1);
  int drawer1 = m_tileHWID->drawer(ch_id1);
  int chan1 = m_tileHWID->channel(ch_id1);
  int drawerIdx1 = TileCalibUtils::getDrawerIdx(ros1, drawer1);
  const TileBchStatus& chStatus1 = m_tileBadChanTool->getAdcStatus(drawerIdx1, chan1, (gain1 < 0) ? 1 : gain1);
  
  // check quality first
  bool bad1 = ((int) pCell->qual1() > m_qualityCut);
  if (bad1) {
    ++m_drawerEvtStatus[ros1][drawer1].nBadQuality;

  } else {
    // check bad status in DB
    bad1 = (gain1 < 0) || chStatus1.isBad();

    // Now checking the DQ status
    if (!bad1) {
      bad1 = !(theDQstatus->isAdcDQgood(ros1, drawer1, chan1, gain1)
              && m_beamInfo->isChanDCSgood(ros1, drawer1, chan1));
    }
  }

  if (hash2 == TileHWID::NOT_VALID_HASH) {
    // gap/crack scintillators with one PMT per cell

    if (bad1) {
      // only one channel in this cell and it is bad
      ++m_drawerEvtStatus[ros1][drawer1].nMaskedChannels;

      if (gain1 == CaloGain::INVALIDGAIN) {
        pCell->setEnergy(m_zeroEnergy, 0.0, TileID::LOWGAIN, CaloGain::INVALIDGAIN); // reset energy completely, indicate problem putting low gain
      } else {
        pCell->setEnergy(m_zeroEnergy, 0.0); // reset energy completely without changing gain
      }
      pCell->setTime(0.0); // reset time completely
      pCell->setQuality(255, (TileCell::MASK_BADCH | (pCell->qbit1() & TileCell::MASK_ALGO)), 0); // reset quality flag for first pmt
      pCell->setQuality(0, TileCell::MASK_BADCH, 1); // reset quality flag for second pmt

      return true;

    } else if (chStatus1.isBadTiming()) {
      pCell->setTime(0.0); // channel with bad timing - no cell time
      uint8_t qbit1 = pCell->qbit1() & (~(TileCell::MASK_TIME)); // clear time bit for first pmt
      pCell->setQuality(pCell->qual1(), qbit1, 0); // update qbits for first pmt
    }
    
  } else { //cell has both PMTs

    int gain2 = pCell->gain2();
    
    HWIdentifier ch_id2 = m_tileHWID->channel_id(hash2);

    int ros2 = m_tileHWID->ros(ch_id2);
    int drawer2 = m_tileHWID->drawer(ch_id2);
    int chan2 = m_tileHWID->channel(ch_id2);
    int drawerIdx2 = TileCalibUtils::getDrawerIdx(ros2, drawer2);
    const TileBchStatus& chStatus2 = m_tileBadChanTool->getAdcStatus(drawerIdx2, chan2, (gain2 < 0) ? 1 : gain2);

    // check quality first
    bool bad2 = ((int) pCell->qual2() > m_qualityCut);
    if (bad2) {
      ++m_drawerEvtStatus[ros2][drawer2].nBadQuality;

    } else {
      // check bad status in DB
      bad2 = (gain2 < 0) || chStatus2.isBad();

      // Now checking the DQ status
      if (!bad2) {
        bad2 = !(theDQstatus->isAdcDQgood(ros2, drawer2, chan2, gain2)
                && m_beamInfo->isChanDCSgood(ros2, drawer2, chan2));
      }
    }

    static const TileCablingService * s_cabling = TileCablingService::getInstance();
    static const bool run2 = (s_cabling->getCablingType() == TileCablingService::RUN2Cabling);
    single_PMT_C10 = (((ros2 == TileHWID::EXTBAR_POS && chan1 == 4)
                      || (ros2 == TileHWID::EXTBAR_NEG && chan2 == 4))
                      && !s_cabling->C10_connected(drawer2));
    if (single_PMT_C10) {
      // for special C10 disconnected channel might be masked in DB
      // and energy of good channel is taken twice with correct weight
      // but if this channel is not masked in DB - set its bad status
      // equal to bad status of real channel, so that cell is masked correctly
      // if real channel connected to a cell is bad
#ifdef ALLOW_DEBUG_COUT
      static int cnt=0;
      if (++cnt < 17) {
        std::cout << "special C10 in " << ((ros2==TileHWID::EXTBAR_POS) ? "EBA" : "EBC")
        << drawer2+1 << " status " << chan1 << "/" << chan2 << " "
        << (chStatus1.isBad()?"bad":"good") << "/"
        << (chStatus2.isBad()?"bad":"good") << "/"
        << ((run2)?" RUN2 cabling": "RUN1 cabling")
        << std::endl;
      }
#endif
      if (chan1 == 4) {
        if (run2 || !chStatus1.isBad()) {
#ifdef ALLOW_DEBUG_COUT
          if (cnt < 17) {
            std::cout << "Ene of chan1 was " << pCell->ene1() << " changing to half of " << pCell->ene2()
            << " and setting bad1=true" << std::endl;
          }
#endif
          pCell->setEnergy(pCell->ene2()/2., pCell->ene2()/2., gain2, gain2);
          //bad1 = bad2;
          bad1 = true;
          --m_drawerEvtStatus[ros1][drawer1].nMaskedChannels; // since it's fake masking, decrease counter by 1 in advance
        }
      } else {
        if (run2 || !chStatus2.isBad()) {
#ifdef ALLOW_DEBUG_COUT
          if (cnt < 17) {
            std::cout << "Ene of chan2 was " << pCell->ene2() << " changing to half of " << pCell->ene1()
            << " and setting bad2=true" << std::endl;
          }
#endif
          pCell->setEnergy(pCell->ene1()/2., pCell->ene1()/2., gain1, gain1);
          //bad2 = bad1;
          bad2 = true;
          --m_drawerEvtStatus[ros2][drawer2].nMaskedChannels; // since it's fake masking, decrease counter by 1 in advance
        }
      }
    }
    if (bad1 && bad2) {
      // both channels are bad
      ++m_drawerEvtStatus[ros1][drawer1].nMaskedChannels;
      ++m_drawerEvtStatus[ros2][drawer2].nMaskedChannels;

      if (gain1 == CaloGain::INVALIDGAIN || gain2 == CaloGain::INVALIDGAIN) {
        if (gain1 == CaloGain::INVALIDGAIN) gain1 = (CaloGain::CaloGain) TileID::LOWGAIN;
        if (gain2 == CaloGain::INVALIDGAIN) gain2 = (CaloGain::CaloGain) TileID::LOWGAIN;
        pCell->setEnergy(m_zeroEnergy, m_zeroEnergy, gain1, gain2); // reset energy completely, indicate problem putting low gain
      } else {
        pCell->setEnergy(m_zeroEnergy, m_zeroEnergy); // reset energy completely without changing gain
      }
      pCell->setTime(0.0);  // reset time completely
      pCell->setQuality(255, (TileCell::MASK_BADCH | (pCell->qbit1() & TileCell::MASK_ALGO)), 0); // reset quality flag for first pmt
      pCell->setQuality(255, (TileCell::MASK_BADCH | (pCell->qbit2() & TileCell::MASK_ALGO)), 1); // reset quality flag for second pmt

      return true;

    } else if (bad1 && !bad2) {
      // first channel is bad
      ++m_drawerEvtStatus[ros1][drawer1].nMaskedChannels;

      float ene2 = pCell->ene2();
      pCell->setEnergy(ene2, ene2, gain2, gain2); // use energy/gain from second pmt for both pmts

      if (chStatus2.isBadTiming()) {
        pCell->setTime(0.0); // time in second pmt is bad - no cell time
        uint8_t qual2 = pCell->qual2();
        uint8_t qbit2 = pCell->qbit2() & (~(TileCell::MASK_TIME)); // clear time bit for second pmt
        uint8_t qbit1 = qbit2 | TileCell::MASK_BADCH; // set bad channel bit for first pmt
        uint8_t qual1 = qual2 + (gain1 - gain2); // if gains are different, qua11 and qual2 will be different
        if (qual1 > m_qualityCut && gain1 > gain2) qual1 = qual2 - (gain1 - gain2); // new feature in release 17.2
        pCell->setQuality(qual1, qbit1, 0); // change quality and qbits for first pmt
        pCell->setQuality(qual2, qbit2, 1); // update qbits for second pmt
      } else {
        pCell->setTime(pCell->time2());  // use time from second pmt as cell time
        pCell->setQuality(pCell->qual2(), (pCell->qbit2() | TileCell::MASK_BADCH), 0); // change quality flag for first pmt
      }
      
      return true;

    } else if (!bad1 && bad2) {
      // second channel is bad
      ++m_drawerEvtStatus[ros2][drawer2].nMaskedChannels;

      float ene1 = pCell->ene1();
      pCell->setEnergy(ene1, ene1, gain1, gain1);  // use energy/gain from first pmt for both pmts

      if (chStatus1.isBadTiming()) {
        pCell->setTime(0.0); // time in first pmt is bad - no cell time
        uint8_t qual1 = pCell->qual1();
        uint8_t qbit1 = pCell->qbit1() & (~(TileCell::MASK_TIME)); // clear time bit for first pmt
        uint8_t qbit2 = qbit1 | TileCell::MASK_BADCH; // set bad channel bit for second pmt
        uint8_t qual2 = qual1 + (gain2 - gain1); // if gains are different, qua11 and qual2 will be different
        if (qual2 > m_qualityCut && gain2 > gain1) qual2 = qual1 - (gain2 - gain1); // new feature in release 17.2
        pCell->setQuality(qual1, qbit1, 0); // update qbits for first pmt
        pCell->setQuality(qual2, qbit2, 1); // change quality and qbits for second pmt
      } else {
        pCell->setTime(pCell->time1());  // use time from first pmt as cell time
        pCell->setQuality(pCell->qual1(), (pCell->qbit1() | TileCell::MASK_BADCH), 1); // change quality flag for second pmt
      }
      
      return true;

    } else {

      if (chStatus1.isBadTiming()) {

        if (chStatus2.isBadTiming()) {
          pCell->setTime(0.0); // time in both pmts is bad - no cell time
          uint8_t qbit2 = pCell->qbit2() & (~(TileCell::MASK_TIME)); // clear time bit for second pmt
          pCell->setQuality(pCell->qual2(), qbit2, 1); // update qbits for second pmt
        } else {
          pCell->setTime(pCell->time2());  // use time from second pmt as cell time
        }
        uint8_t qbit1 = pCell->qbit1() & (~(TileCell::MASK_TIME)); // clear time bit for first pmt
        pCell->setQuality(pCell->qual1(), qbit1, 0); // update qbits for first pmt

      } else if (chStatus2.isBadTiming()) {

        pCell->setTime(pCell->time1());  // use time from first pmt as cell time
        uint8_t qbit2 = pCell->qbit2() & (~(TileCell::MASK_TIME)); // clear time bit for second pmt
        pCell->setQuality(pCell->qual2(), qbit2, 1); // update qbits for second pmt
      }
    }
    
  }
  
  return single_PMT_C10;
}
