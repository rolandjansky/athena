/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


// Tile includes
#include "TileRecUtils/TileCellBuilderFromHit.h"
#include "TileEvent/TileCell.h"
#include "CaloIdentifier/TileID.h"
#include "CaloIdentifier/TileTBID.h"
#include "TileIdentifier/TileHWID.h"
#include "CaloDetDescr/MbtsDetDescrManager.h"
#include "TileDetDescr/TileDetDescrManager.h"
#include "TileConditions/TileInfo.h"
#include "TileSimEvent/TileHit.h"
#include "TileSimEvent/TileHitVector.h"
#include "TileEvent/TileHitContainer.h"
#include "TileEvent/TileCellCollection.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"

// Calo includes
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloInterface/ICalorimeterNoiseTool.h"
#include "CaloConditions/CaloAffectedRegionInfoVec.h"
#include "Identifier/IdentifierHash.h"

// Atlas includes
#include "AthAllocators/DataPool.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

// access all Hits inside container
#include "EventContainers/SelectAllObject.h" 
#include "AthenaKernel/errorcheck.h"
// For the Athena-based random numbers.
#include "AthenaKernel/IAtRndmGenSvc.h"


//CLHEP includes
#include <CLHEP/Random/Randomize.h>
#include <CLHEP/Units/SystemOfUnits.h>
#include <cmath>


#ifndef ATHENAHIVE
#define USE_TILECELLS_DATAPOOL
#endif

#ifdef USE_TILECELLS_DATAPOOL
#define NEWTILECELL tileCellsP.nextElementPtr
#else
#define NEWTILECELL new TileCell
#endif


using CLHEP::RandGaussQ;
using CLHEP::MeV;
 
// uncomment line below for debug output
// #define ALLOW_DEBUG_COUT 1

static const InterfaceID IID_ITileCellBuilderFromHit("TileCellBuilderFromHit", 1, 0); 	 

const InterfaceID& TileCellBuilderFromHit::interfaceID( ) {
  return IID_ITileCellBuilderFromHit; 	 
}

//Constructor
TileCellBuilderFromHit::TileCellBuilderFromHit(const std::string& type, const std::string& name,
    const IInterface* parent)
  : base_class(type, name, parent)
  , m_infoName("TileInfo")
  , m_eneForTimeCut(35. * MeV) // keep time only for cells above 70 MeV (more than 35 MeV in at least one PMT to be precise)
  , m_eneForTimeCutMBTS(0.03675) // the same cut for MBTS, but in pC, corresponds to 3 ADC counts or 35 MeV
  , m_qualityCut(254) // cut on overflow in quality (if quality is 255 - assume that channel is bad)
  , m_maxTime (25.)
  , m_minTime(-25.)
  , m_maskBadChannels(false)
  , m_useNoiseTool(true)
  , m_noiseSigma(20.*MeV)
  , m_tileID(0)
  , m_tileTBID(0)
  , m_tileHWID(0)
  , m_tileInfo(0)
  , m_pHRengine(0)
  , m_rndmSvc ("AtRndmGenSvc", name)
  , m_noiseTool("CaloNoiseTool")
  , m_tileMgr(0)
  , m_mbtsMgr(0)
  , m_RChType(TileFragHash::Default)
{
  declareInterface<TileCellBuilderFromHit>( this );

  //memset(m_drawerRunStatus, 0, sizeof(m_drawerRunStatus));
  //memset(m_eventErrorCounter, 0, sizeof(m_eventErrorCounter));

  // never set energy to zero, but set it to some small number
  // this will help TopoCluster to assign proper weight to the cell if needed
  m_zeroEnergy = 0.5 * MeV; // half a MeV in both PMTs i.e. one MeV in a cell

  declareProperty("TileInfoName"                ,m_infoName);        // Name of TileInfo store (default=TileInfo);
  declareProperty("RndmSvc"             ,m_rndmSvc, "Random Number Service used in TileCellBuildetFromHit");

  // Noise Sigma
  declareProperty("NoiseSigma",m_noiseSigma);

  // NoiseTool
  declareProperty("UseCaloNoiseTool",m_useNoiseTool);
  declareProperty("CaloNoiseTool",m_noiseTool,"Tool Handle for noise tool");

  // Maximum and minimum time for a cell to be included:
  declareProperty("MaxTime", m_maxTime);
  declareProperty("MinTime", m_minTime);

  // put zero energy in bad channels and recover from single-channel failure using second PMT is a cell
  declareProperty("maskBadChannels", m_maskBadChannels);

  // PMT energy will be set to this value if channel is bad
  declareProperty("BadChannelZeroEnergy", m_zeroEnergy);
  // PMT with energy above cut will preserve time info in ESD
  declareProperty("EneForTimeCut", m_eneForTimeCut);
  declareProperty("EneForTimeCutMBTS", m_eneForTimeCutMBTS);
  // PMT with quality greater than this cut will be masked
  declareProperty("QualityCut", m_qualityCut);
}

/**
 * Destructor
 */
TileCellBuilderFromHit::~TileCellBuilderFromHit(){ 
}

/**
 * Initializer
 */
StatusCode TileCellBuilderFromHit::initialize() {
  
  // retrieve MBTS and Tile detector manager, TileID helper and TileIfno from det store
  // retrieve MBTS and Tile detector manager, TileID helper and TileIfno from det store
  if (m_MBTSContainerKey.key().empty()) {
    m_mbtsMgr = nullptr;
  } else {

    ATH_CHECK( m_MBTSContainerKey.initialize() );
    ATH_MSG_INFO( "Storing MBTS cells in " << m_MBTSContainerKey.key() );
    
    if (detStore()->retrieve(m_mbtsMgr).isFailure()) {
      ATH_MSG_WARNING( "Unable to retrieve MbtsDetDescrManager from DetectorStore" );
      m_mbtsMgr = nullptr;
    }
  }

  ATH_CHECK( m_eventInfoKey.initialize() );
  ATH_CHECK( m_hitContainerKey.initialize() );

  ATH_CHECK( detStore()->retrieve(m_tileMgr) );
  ATH_CHECK( detStore()->retrieve(m_tileID) );
  ATH_CHECK( detStore()->retrieve(m_tileTBID) );
  ATH_CHECK( detStore()->retrieve(m_tileHWID) );
  ATH_CHECK( detStore()->retrieve(m_tileInfo, m_infoName) );

  //=== get TileBadChanTool
  ATH_CHECK( m_tileBadChanTool.retrieve() );

  //=== get TileCondToolEmscale
  ATH_CHECK( m_tileToolEmscale.retrieve() );

  //---- retrieve the noise tool ----------------
  if (m_useNoiseTool) {
    ATH_MSG_INFO( "Reading electronic noise from DB" );
    ATH_CHECK( m_noiseTool.retrieve() );
  } else {
    ATH_MSG_INFO( "Noise Sigma " << m_noiseSigma << " MeV is selected!" );
    m_noiseTool.disable();
  }

  ATH_CHECK( m_rndmSvc.retrieve());
  m_pHRengine = m_rndmSvc->GetEngine("Tile_DigitsMaker");

  ATH_MSG_INFO( "max time thr  " << m_maxTime << " ns" );
  ATH_MSG_INFO( "min time thr  " << m_minTime << " ns" );
  
  m_E1_TOWER = (m_tileID->cell_hash_max() < 10000) ? 10 : 40;

  ATH_MSG_INFO( "taking hits from '" << m_hitContainerKey.key() << "'" );

  m_cabling = TileCablingService::getInstance();
  m_RUN2 = (m_cabling->getCablingType() == TileCablingService::RUN2Cabling);

  if (m_RUN2 && !m_E4prContainerKey.key().empty()) {
    ATH_CHECK( m_E4prContainerKey.initialize() );
    ATH_MSG_INFO( "Storing E4'  cells in " << m_E4prContainerKey.key() );
  } else {
    m_E4prContainerKey = ""; // no E4' container for RUN1
  }


  ATH_MSG_INFO( "TileCellBuilderFromHit initialization completed" );

  return StatusCode::SUCCESS;
}

StatusCode TileCellBuilderFromHit::finalize() {

  ATH_MSG_INFO( "Finalizing" );

  return StatusCode::SUCCESS;
}

StatusCode TileCellBuilderFromHit::process (CaloCellContainer * theCellContainer,
                                            const EventContext& ctx) const
{
  //**
  //* Get TileHits
  //**

  TileDrawerEvtStatusArray drawerEvtStatus;

  SG::ReadHandle<TileHitContainer> hitContainer(m_hitContainerKey, ctx);

  if (!hitContainer.isValid()) {

    ATH_MSG_WARNING( " Could not find container " << m_hitContainerKey.key() );
    ATH_MSG_WARNING( " do not fill CaloCellContainer " );

  } else {
    
    ATH_MSG_DEBUG( "Container " << m_hitContainerKey.key() << " with TileHits found ");

    std::unique_ptr<TileCellContainer> MBTSCells;
    if (!m_MBTSContainerKey.key().empty()) {
      MBTSCells = std::make_unique<TileCellContainer>(SG::VIEW_ELEMENTS);
    }

    std::unique_ptr<TileCellContainer> E4prCells;
    if (!m_E4prContainerKey.key().empty()) {
      E4prCells = std::make_unique<TileCellContainer>(SG::VIEW_ELEMENTS);
    }
    
    SelectAllObject<TileHitContainer> selAll(hitContainer.cptr());
    SelectAllObject<TileHitContainer>::const_iterator begin = selAll.begin();
    SelectAllObject<TileHitContainer>::const_iterator end = selAll.end();

    if (begin != end) {
      ATH_MSG_DEBUG( " Calling build() method for hits from " << m_hitContainerKey.key() );
      build (drawerEvtStatus, begin, end, theCellContainer,
             MBTSCells.get(), E4prCells.get());
    }
    
    if (!m_MBTSContainerKey.key().empty()) {
      SG::WriteHandle<TileCellContainer> MBTSContainer(m_MBTSContainerKey, ctx);
      ATH_CHECK( MBTSContainer.record(std::move(MBTSCells)) );
    }
    
    if (!m_E4prContainerKey.key().empty()) {
      SG::WriteHandle<TileCellContainer> E4prContainer(m_E4prContainerKey, ctx);
      ATH_CHECK( E4prContainer.record(std::move(E4prCells)) );
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
  xAOD::EventInfo::EventFlagErrorState error = xAOD::EventInfo::NotSet;
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
    TileDrawerEvtStatus * evt = drawerEvtStatus[p];
    //TileDrawerRunStatus * run = m_drawerRunStatus[p];
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
        //++(run[d].drawerAbsent);
      } else if (evt[d].nMaskedChannels >= evt[d].nChannels) {
        ++drConsec;
        ++drMasked;
        //++(run[d].drawerMasked);
      } else {
        if (drConsec > drConsecMax) {
          drConsecMax = drConsec;
          if (drConsecMax > drConsecMaxMax) {
            drConsecMaxMax = drConsecMax;
            drConsecNum = ((p - 1) << 6) | (d - drConsec);
          }
        }
        drConsec = 0;
        //if (evt[d].nMaskedChannels > 0) {
        // ++(run[d].channelsMasked);
        //}
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
      error = xAOD::EventInfo::Error;
    }
    if (drMasked > 15) {
      fl |= 0x00100000;
      error = xAOD::EventInfo::Error;
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

  // number of consecutively masked modules (if it's > 15 we have error already set)
  flag |= (std::min(15, drConsecMaxMax) << 16);

  if (drConsecMaxMax > 1 && error < xAOD::EventInfo::Warning) {
    // setting warning flag
    error = xAOD::EventInfo::Warning;
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

  //++m_eventErrorCounter[error]; // error index is 0 or 1 or 2 here
  //++m_eventErrorCounter[3]; // count separately total number of events
  
  // retrieve EventInfo
  SG::ReadHandle<xAOD::EventInfo> eventInfo(m_eventInfoKey, ctx);

  if (eventInfo.isValid()) {

    if (flag != 0) {
      ATH_MSG_DEBUG( " set eventInfo for Tile for this event to 0x" << MSG::hex << flag << MSG::dec );
      if (!eventInfo->updateEventFlags(xAOD::EventInfo::Tile, flag)) {
        ATH_MSG_WARNING( " cannot set eventInfo for Tile " );
      }
    }
    
    if (error != xAOD::EventInfo::NotSet) {
      ATH_MSG_DEBUG( " set error bits for Tile for this event to " << error );
      if (!eventInfo->updateErrorState(xAOD::EventInfo::Tile, error)) {
        ATH_MSG_WARNING( " cannot set error state for Tile " );
      }
    }

  }
  else {
    ATH_MSG_WARNING( " cannot retrieve EventInfo, will not set Tile information " );
  }
  
  // Execution completed.
  ATH_MSG_DEBUG( "TileCellBuilderFromHit execution completed." );

  return StatusCode::SUCCESS;
}

//************************************************************************
void TileCellBuilderFromHit::correctCell(TileCell* pCell, int correction, int pmt, int gain
                                         , float ener, float time, unsigned char iqual, unsigned char qbit) const {
//************************************************************************

// Merge two pmts in one cell if needed
// and apply corrections

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

unsigned char TileCellBuilderFromHit::qbits(TileDrawerEvtStatusArray& drawerEvtStatus,
                                            int ros, int drawer, bool count_over
    , bool good_time, bool good_ener, bool overflow, bool underflow, bool overfit) const {

  ++drawerEvtStatus[ros][drawer].nChannels;
  // new feature in rel 17.2.7 - count underflows and overflows
  if (count_over) {
    if (overflow) ++drawerEvtStatus[ros][drawer].nOverflow;
    if (underflow) ++drawerEvtStatus[ros][drawer].nUnderflow;
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
      ++drawerEvtStatus[ros][drawer].nSomeSignal;
    }
  }

  return qbit;
}

// masking for MBTS with single channel
bool
TileCellBuilderFromHit::maskBadChannel (TileDrawerEvtStatusArray& drawerEvtStatus,
                                        TileCell* pCell) const
{
  Identifier cell_id = pCell->ID();

  HWIdentifier channel_id = m_cabling->s2h_channel_id(cell_id);
  int ros = m_tileHWID->ros(channel_id);
  int drawer = m_tileHWID->drawer(channel_id);
  int channel = m_tileHWID->channel(channel_id);
  int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
  int gain = pCell->gain1();

  TileBchStatus chStatus = m_tileBadChanTool->getAdcStatus(drawerIdx, channel, gain);

  // check quality first
  bool bad = ((int) pCell->qual1() > m_qualityCut);
  if (bad) {
    ++drawerEvtStatus[ros][drawer].nBadQuality;

  } else {
    // check bad status in DB
    bad = chStatus.isBad();

  }

  if (bad) {
    // only one channel in this cell and it is bad
    ++drawerEvtStatus[ros][drawer].nMaskedChannels;

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
bool
TileCellBuilderFromHit::maskBadChannels (TileDrawerEvtStatusArray& drawerEvtStatus,
                                         TileCell* pCell, bool single_PMT_C10, bool Ecell) const {

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
    ++drawerEvtStatus[ros1][drawer1].nBadQuality;

  } else {
    // check bad status in DB
    bad1 = (gain1 < 0) || chStatus1.isBad();

  }

  if ( Ecell || hash2 == TileHWID::NOT_VALID_HASH) {
    // gap/crack scintillators with one PMT per cell

    if (bad1) {
      // only one channel in this cell and it is bad
      ++drawerEvtStatus[ros1][drawer1].nMaskedChannels;

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
      ++drawerEvtStatus[ros2][drawer2].nBadQuality;

    } else {
      // check bad status in DB
      bad2 = (gain2 < 0) || chStatus2.isBad();

    }

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
        << ((m_RUN2)?" RUN2 cabling": "RUN1 cabling")
        << std::endl;
      }
#endif
      if (chan1 == 4) {
        if (m_RUN2 || !chStatus1.isBad()) {
#ifdef ALLOW_DEBUG_COUT
          if (cnt < 17) {
            std::cout << "Ene of chan1 was " << pCell->ene1() << " changing to half of " << pCell->ene2()
            << " and setting bad1=true" << std::endl;
          }
#endif
          pCell->setEnergy(pCell->ene2()/2., pCell->ene2()/2., gain2, gain2);
          //bad1 = bad2;
          bad1 = true;
          --drawerEvtStatus[ros1][drawer1].nMaskedChannels; // since it's fake masking, decrease counter by 1 in advance
        }
      } else {
        if (m_RUN2 || !chStatus2.isBad()) {
#ifdef ALLOW_DEBUG_COUT
          if (cnt < 17) {
            std::cout << "Ene of chan2 was " << pCell->ene2() << " changing to half of " << pCell->ene1()
            << " and setting bad2=true" << std::endl;
          }
#endif
          pCell->setEnergy(pCell->ene1()/2., pCell->ene1()/2., gain1, gain1);
          //bad2 = bad1;
          bad2 = true;
          --drawerEvtStatus[ros2][drawer2].nMaskedChannels; // since it's fake masking, decrease counter by 1 in advance
        }
      }
    }
    if (bad1 && bad2) {
      // both channels are bad
      ++drawerEvtStatus[ros1][drawer1].nMaskedChannels;
      ++drawerEvtStatus[ros2][drawer2].nMaskedChannels;

      if (gain1 == CaloGain::INVALIDGAIN || gain2 == CaloGain::INVALIDGAIN) {
        if (gain1 == CaloGain::INVALIDGAIN) gain1 = 0; // this is TileID::LOWGAIN; - commented out to make Coverity happy
        if (gain2 == CaloGain::INVALIDGAIN) gain2 = 0; // this is TileID::LOWGAIN; - commented out to make Coverity happy
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
      ++drawerEvtStatus[ros1][drawer1].nMaskedChannels;

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
      ++drawerEvtStatus[ros2][drawer2].nMaskedChannels;

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


template<class ITERATOR, class COLLECTION>
void TileCellBuilderFromHit::build(TileDrawerEvtStatusArray& drawerEvtStatus,
                                   const ITERATOR & begin, const ITERATOR & end, COLLECTION * coll,
                                   TileCellContainer* MBTSCells,
                                   TileCellContainer* E4prCells) const
{
  // disable checks for TileID and remember previous state
  // FIXME: const violation; MT problem.
  bool do_checks = m_tileID->do_checks();
  m_tileID->set_do_checks(false);
  bool do_checks_tb = m_tileID->do_checks();
  m_tileTBID->set_do_checks(false);

  /* zero all counters and sums */
  int nTwo = 0;
  int nCell = 0;
  int nFake = 0;
  int nMBTS = 0;
  int nE4pr = 0;
  int nChan = 0;
  float eCh = 0.0;
  float eCellTot = 0.0;
  float eMBTSTot = 0.0;
  float eE4prTot = 0.0;
  bool EBdrawerPresent[128];
  memset(EBdrawerPresent, 0, sizeof(EBdrawerPresent));
#ifdef USE_TILECELLS_DATAPOOL
  DataPool<TileCell> tileCellsP(5217);
#endif
  //**
  //* Iterate over hits, creating new TileCells (or incrementing
  //* existing ones). Add each new TileCell to the output collection
  //**

  bool overflow = false;
  bool underflow = false;
  bool overfit = false;
  float ener_min = (m_useNoiseTool) ? 1.0E-30F : 0.0;

  std::vector<TileCell*> allCells (m_tileID->cell_hash_max(), nullptr);
  std::vector<TileCell*> MBTSVec;   //!< vector to of pointers to MBTS cells
  if (MBTSCells) {
    MBTSVec.resize (NCELLMBTS);
  }
  std::vector<TileCell*> E4prVec;   //!< vector to of pointers to E4' cells
  if (E4prCells) {
    E4prVec.resize (NCELLE4PR);
  }

  for (ITERATOR hitItr = begin; hitItr != end; ++hitItr) {

    const TileHit* pHit = (*hitItr);

    // Get hit id (logical pmt id)
    Identifier cell_id = pHit->identify();

    double ehit = 0.;
    double thit = 0.;

    int hitsize = pHit->size();
    for (int ind = 0; ind < hitsize; ++ind) { // loop over sub-hits 
      float tim = pHit->time(ind);
      if (m_minTime < tim && tim < m_maxTime) {
        float ene = pHit->energy(ind);
        ehit += ene;
        thit += tim*ene;
      }
    }

    float ener = ener_min; 
    float time = 0.;
    float qual = 0.;
    int gain = TileID::HIGHGAIN;
    bool good_time = false;
    bool non_zero_time = (ehit!=0.0);

    if (non_zero_time) {
      time = thit / ehit;
      // Convert hit energy deposit to cell energy
      ener = ehit * m_tileInfo->HitCalib(cell_id);
      if (ener > 10000. * MeV) { // very simple check - if above 10 GeV assume low gain
        gain = TileID::LOWGAIN;
      }
      good_time = (fabs(time) < 25.);
    }
    
    bool MBTS = false, E4pr = false, E1_CELL = false;
    int section,side,tower,sample,pmt,index,ros,drawer,channel;

    // Get logical ID of cell
    if (m_tileTBID->is_tiletb(cell_id)) {

      pmt = 0;
      side = m_tileTBID->type(cell_id);
      int phi = m_tileTBID->module(cell_id);
      int eta = m_tileTBID->channel(cell_id);
      if (eta<2) {
        MBTS = true;
        index = mbts_index(std::max(0,side),phi,eta);
      } else {
        E4pr = true;
        index = e4pr_index(phi);
      }

      HWIdentifier channel_id = m_cabling->s2h_channel_id(cell_id);
      ros = m_tileHWID->ros(channel_id);
      drawer = m_tileHWID->drawer(channel_id);
      channel = m_tileHWID->channel(channel_id);

    } else {

      MBTS = false;
      E4pr = false;

      pmt = m_tileID->pmt(cell_id); // 0 or 1 - first or second PMT of the cell

      cell_id = m_tileID->cell_id(cell_id); // remove pmt field from cell_id
      index = m_tileID->cell_hash(cell_id);

      section = m_tileID->section(cell_id);
      side = m_tileID->side(cell_id);
      drawer = m_tileID->module(cell_id);
      tower = m_tileID->tower(cell_id);
      sample = m_tileID->sample(cell_id);

      // instead of using cabling (which might be wrong for upgrade geometry
      // try to figure out ros/drawer/channel 
      ros = ( TileID::POSITIVE == side ) ? 1 : 2;
      if ( TileID::BARREL != section ) ros += 2;
      else if (tower == 0 && sample == TileID::SAMP_D && pmt == 1) ros = 2; // Cell D0, second PMT in LBC
      channel = -1; // don't know channel
      E1_CELL = (section == TileID::GAPDET && sample == TileID::SAMP_E && tower == m_E1_TOWER);
    }

    ++nChan;
    eCh += ener;

    if (E4pr) { // E4' cells

      if (E4prCells) { // do something with them only if contaier existst
        ++nE4pr;

        eE4prTot += ener;
        unsigned char iqual = iquality(qual);
        // for E4' cell qbit use only non_zero_time flag and check that energy is above standatd energy threshold in MeV
        unsigned char qbit = qbits(drawerEvtStatus,
                                   ros, drawer, true, non_zero_time, (fabs(ener) > m_eneForTimeCut)
                                   , overflow, underflow, overfit);
        CaloGain::CaloGain cgain = (gain == TileID::HIGHGAIN)
                                   ? CaloGain::TILEONEHIGH
                                   : CaloGain::TILEONELOW;

        TileCell* pCell = NEWTILECELL();
        // no CaloDDE
        // Cell ID is set explicitly
        pCell->set(NULL, cell_id);
        pCell->setEnergy_nonvirt(ener, 0, cgain, 3);
        pCell->setTime_nonvirt(time);
        pCell->setQual1(iqual);
        pCell->setQual2(0);
        pCell->setQbit1(qbit);
        pCell->setQbit2(0);

        if (msgLvl(MSG::VERBOSE)) {
          msg(MSG::VERBOSE) << " E4'  cell_id=" << m_tileTBID->to_string(cell_id)
                            << " adc_id=" << "5/0/" << ros << "/" << drawer << "/" << channel << "/" << gain 
                            << " ene= " << ener
                            << " ehit= " << ehit
                            << " time= " << time
                            << " iqual= " << (int) iqual
                            << " qbit = 0x" << MSG::hex << (int) qbit << MSG::dec << endmsg;
        }

        if (E4prVec[index]) {
          msg(MSG::WARNING) << " double E4' cell_id=" << m_tileTBID->to_string(cell_id)
                            << "  ignoring previous value" << endmsg;
        }
        E4prVec[index] = pCell;
      }

    } else if (MBTS) { // MBTS cells

      if (MBTSCells) { // do something with them only if contaier existst
        ++nMBTS;

        // convert energy to pCb
        ener /= m_tileToolEmscale->channelCalib(TileCalibUtils::getDrawerIdx(ros, drawer), channel, gain, 1., 
                                                TileRawChannelUnit::PicoCoulombs,
                                                TileRawChannelUnit::MegaElectronVolts);
        gain = (ener>12.) ? TileID::LOWGAIN : TileID::HIGHGAIN; // assume low gain above 12 pC
        eMBTSTot += ener;
        unsigned char iqual = iquality(qual);
        // for MBTS qbit use AND of good_time and non_zero_time and check that energy is above MBTS energy threshold in pC
        unsigned char qbit = qbits(drawerEvtStatus,
                                   ros, drawer, false, (good_time && non_zero_time)
           , (fabs(ener) > m_eneForTimeCutMBTS), overflow, underflow, overfit);
        CaloGain::CaloGain cgain = (gain == TileID::HIGHGAIN)
                                   ? CaloGain::TILEONEHIGH
                                   : CaloGain::TILEONELOW;

        TileCell* pCell = NEWTILECELL();
        // MBTS CaloDDE
        // Cell ID is set explicitly
        pCell->set((m_mbtsMgr) ? m_mbtsMgr->get_element(cell_id) : NULL, cell_id);
        pCell->setEnergy_nonvirt(ener, 0, cgain, 3);
        pCell->setTime_nonvirt(time);
        pCell->setQual1(iqual);
        pCell->setQual2(0);
        pCell->setQbit1(qbit);
        pCell->setQbit2(0);

        if (msgLvl(MSG::VERBOSE)) {
          msg(MSG::VERBOSE) << " MBTS cell_id=" << m_tileTBID->to_string(cell_id)
                            << " adc_id=" << "5/0/" << ros << "/" << drawer << "/" << channel << "/" << gain 
                            << " ene= " << ener
                            << " ehit= " << ehit
                            << " time= " << time
                            << " iqual= " << (int) iqual
                            << " qbit = 0x" << MSG::hex << (int) qbit << MSG::dec << endmsg;
        }

        if (MBTSVec[index]) {
          msg(MSG::WARNING) << " double MBTS cell_id=" << m_tileTBID->to_string(cell_id)
                            << "  ignoring previous value" << endmsg;
        }
        MBTSVec[index] = pCell;
      }
    } else {

      eCellTot += ener;

      unsigned char iqual = iquality(qual);
      // for normal cell qbit use only non_zero_time flag and check that energy is above standard energy threshold in MeV
     unsigned char qbit = qbits(drawerEvtStatus,
                                ros, drawer, true, non_zero_time, (fabs(ener) > m_eneForTimeCut)
          , overflow, underflow, overfit);

      if (E1_CELL && m_RUN2) {

        int drawer2 = m_cabling->E1_merged_with_run2(ros,drawer);
        if (drawer2 != 0) { // Raw channel split into two E1 cells for Run 2.
          Identifier cell_id2 = m_tileID->cell_id(TileID::GAPDET, side, drawer2, m_E1_TOWER, TileID::SAMP_E);
          int index2 = m_tileID->cell_hash(cell_id2);
          TileCell* pCell2 = NEWTILECELL();
          ++nCell;
          allCells[index2] = pCell2;
          const CaloDetDescrElement* dde2 = m_tileMgr->get_cell_element(index2);
          pCell2->set(dde2, cell_id2);
          pCell2->setEnergy_nonvirt(0.0, 0.0, CaloGain::INVALIDGAIN, CaloGain::INVALIDGAIN);
          int pmt2(0);
          ener *= 0.5F;
          correctCell(pCell2, 1, pmt2, gain, ener, time, iqual, qbit);

          ATH_MSG_DEBUG("E1 cell Id => " << m_tileID->to_string(cell_id)
                       << " split into " << m_tileID->to_string(cell_id2));
        }
      }

      TileCell* pCell = allCells[index];
      if (pCell) {
        ++nTwo;
        correctCell(pCell, 2, pmt, gain, ener, time, iqual, qbit); // correct & merge 2 PMTs in one cell
      } else {
        ++nCell;
        allCells[index] = pCell = NEWTILECELL();
        const CaloDetDescrElement* dde = m_tileMgr->get_cell_element(index);
        pCell->set(dde, cell_id);
        pCell->setEnergy_nonvirt(0, 0, CaloGain::INVALIDGAIN, CaloGain::INVALIDGAIN);
        correctCell(pCell, 1, pmt, gain, ener, time, iqual, qbit); // correct & save e,t,q in new cell
      }

      if (msgLvl(MSG::VERBOSE)) {
        float calib1 = (ehit != 0.0) ? ener / ehit : 0.0;
        msg(MSG::VERBOSE) << " cell_id=" << m_tileID->to_string(cell_id, -2)
                          << " adc_id=" << "5/0/" << ros << "/" << drawer << "/" << channel << "/" << gain 
                          << " calib=" << calib1
                          << " nCell=" << nCell
                          << " energy=" << ener << " (" << pCell->energy() << ", " << pCell->eneDiff() << ")" << endmsg;

        msg(MSG::VERBOSE) << " ehit= " << ehit
                          << " time= " << time
                          << " iqual= " << (int) iqual
                          << " qbit = 0x" << MSG::hex << (int) qbit << MSG::dec << endmsg;
      }
    }
  }



  unsigned char iqual = 0;
  unsigned char qbit = m_RChType;

  //**
  // Now store all TileCells
  //**
  for (unsigned int index = 0; index < allCells.size(); ++index) {

    TileCell * pCell = allCells[index];
    const CaloDetDescrElement* dde = m_tileMgr->get_cell_element(index);
    if(!dde)
    {
      ATH_MSG_VERBOSE ("skipping dummy tower with id=" << m_tileID->to_string(m_tileID->cell_id((IdentifierHash)index), -2) << " hash " << index);
      continue;
    }
    Identifier cell_id = dde->identify();

    int section = m_tileID->section(cell_id);
    int side = m_tileID->side(cell_id);
    int module = m_tileID->module(cell_id);
    int sample = m_tileID->sample(cell_id);

    bool single_PMT_C10 = ( section == TileID::GAPDET && sample == TileID::SAMP_B && !m_cabling->C10_connected(module) );
    bool missing_D4 = ( section == TileID::GAPDET && sample == TileID::SAMP_D && (module == ((side>0)?14:17)) );
    bool Ecell = (sample == TileID::SAMP_E);
    bool single_PMT = Ecell || single_PMT_C10;
    
    if (!pCell) {

      if ( ! missing_D4 ) {

        ++nCell;
        if (!single_PMT) ++nTwo;

        allCells[index] = pCell = NEWTILECELL();
        pCell->set(dde, cell_id);

        pCell->setEnergy_nonvirt(ener_min, 0.0, TileID::HIGHGAIN, (Ecell)?3:TileID::HIGHGAIN); // reset energy completely
        pCell->setTime_nonvirt(0.0); // reset time completely
        pCell->setQuality(iqual, qbit, 0); // reset quality flag for first pmt
        pCell->setQuality(iqual, qbit, 1); // reset quality flag for second pmt

        ATH_MSG_VERBOSE ( "adding cell with no signal with id=" << m_tileID->to_string(pCell->ID(), -2));
      }
    }
    
    if (pCell) {      // cell exists

      if (m_maskBadChannels)
        if (maskBadChannels (drawerEvtStatus, pCell,single_PMT_C10,Ecell))
          ATH_MSG_VERBOSE ( "cell with id=" << m_tileID->to_string(pCell->ID(), -2)
                           << " bad channels masked, new energy=" << pCell->energy() );

      bool bad1=pCell->badch1();
      bool bad2=pCell->badch2();
      
      if (! (bad1 && bad2) ) {

        float noiseSigma = (m_useNoiseTool)
            ? m_noiseTool->getEffectiveSigma(pCell,ICalorimeterNoiseTool::MAXSYMMETRYHANDLING,ICalorimeterNoiseTool::ELECTRONICNOISE)
            : m_noiseSigma;

        if (bad1 || bad2 || single_PMT) {
            
          float ene = RandGaussQ::shoot(m_pHRengine, 0.0, noiseSigma);

          ATH_MSG_VERBOSE ( "Cell noise for cell id=" << m_tileID->to_string(pCell->ID(), -2)
                            << " sigma " << noiseSigma << " noise " << ene );
          if (Ecell)
            pCell->addEnergy(ene,0,pCell->gain1());
          else
            pCell->addEnergy(ene);
          
        } else {

          int gain1=pCell->gain1();
          int gain2=pCell->gain2();
            
          if (gain1!=gain2) {

            float ene = RandGaussQ::shoot(m_pHRengine, 0.0, noiseSigma);
            ATH_MSG_VERBOSE ( "Cell noise for cell id=" << m_tileID->to_string(pCell->ID(), -2)
                              << " sigma " << noiseSigma << " noise " << ene );

            if (gain1==TileID::LOWGAIN)
              pCell->addEnergy(ene,0,gain1);
            else
              pCell->addEnergy(ene,1,gain2);

          } else {

            noiseSigma *= M_SQRT1_2;
            float ene1 = RandGaussQ::shoot(m_pHRengine, 0.0, noiseSigma);
            float ene2 = RandGaussQ::shoot(m_pHRengine, 0.0, noiseSigma);

            ATH_MSG_VERBOSE ( "Cell noise for cell id=" << m_tileID->to_string(pCell->ID(), -2)
                              << " sigma " << noiseSigma*M_SQRT2 << " noise " << ene1+ene2 << " noise1 " << ene1 << " noise2 " << ene2 );

            pCell->addEnergy(ene1,0,gain1);
            pCell->addEnergy(ene2,0,gain2);
          }
        }
      }
      coll->push_back(pCell); // store cell in container
      allCells[index] = 0; // clear pointer for next event
    }
  }


  if (MBTSCells) {

    for (int side = 0; side < NSIDE; ++side) {
      for (int phi = 0; phi < NPHI; ++phi) {
        for (int eta = 0; eta < NETA; ++eta) {

          int index=mbts_index(side,phi,eta);
          TileCell * pCell = MBTSVec[index];

          bool merged_MBTS = ( eta == 1 && (phi&1) == 1 && m_RUN2); // in RUN2 every second outer MBTS does not exist

          if (!pCell && !merged_MBTS) {

            ++nMBTS;
            MBTSVec[index] = pCell = NEWTILECELL();

            Identifier cell_id = m_tileTBID->channel_id((side > 0) ? 1 : -1, phi, eta);
            pCell->set((m_mbtsMgr) ? m_mbtsMgr->get_element(cell_id) : NULL, cell_id);
            pCell->setEnergy_nonvirt(0.0, 0.0, CaloGain::TILEONEHIGH, CaloGain::INVALIDGAIN); // reset energy completely
            pCell->setTime_nonvirt(0.0); // reset time completely
            pCell->setQuality(iqual, qbit, 0); // reset quality flag for first pmt
            pCell->setQuality(iqual, qbit, 1); // reset quality flag for second pmt
          }
          
          if (pCell) {
            if (m_maskBadChannels && maskBadChannel (drawerEvtStatus, pCell))
                ATH_MSG_VERBOSE ( "MBTS cell with id=" << m_tileTBID->to_string(pCell->ID())
                                  << " bad channel masked, new energy=" << pCell->energy() );

            MBTSCells->push_back(pCell); // store cell in container
            MBTSVec[index] = 0; // clear pointer for next event
          }
        }
      }
    }
  }

  if (E4prCells) {

    for (int phi = 0; phi < E4NPHI; ++phi) {

      int index = e4pr_index(phi);
      TileCell * pCell = E4prVec[index];

      if (!pCell) {

        ++nE4pr;
        E4prVec[index] = pCell = NEWTILECELL();

        pCell->set(NULL, m_tileTBID->channel_id(E4SIDE, phi, E4ETA));
        pCell->setEnergy_nonvirt(0.0, 0.0, CaloGain::TILEONEHIGH, CaloGain::INVALIDGAIN); // reset energy completely
        pCell->setTime_nonvirt(0.0); // reset time completely
        pCell->setQuality(iqual, qbit, 0); // reset quality flag for first pmt
        pCell->setQuality(iqual, qbit, 1); // reset quality flag for second pmt
      }

      if (pCell) {
        if (m_maskBadChannels && maskBadChannel (drawerEvtStatus, pCell))
          ATH_MSG_VERBOSE ( "E4pr cell with id=" << m_tileTBID->to_string(pCell->ID())
                             << " bad channel masked, new energy=" << pCell->energy() );

        E4prCells->push_back(pCell); // store cell in container
        E4prVec[index] = 0; // clear pointer for next event
      }
    }
  }
  
  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << " nChan=" << nChan
                    << " RawChSum=" << eCh
                    << " nCell=" << nCell
                    << " n2=" << nTwo
                    << " nFake=" << nFake
                    << " eneTot=" << eCellTot;

    if (MBTSCells)
      msg(MSG::DEBUG) << " nMBTS=" << nMBTS
                      << " eMBTS=" << eMBTSTot;
    if (E4prCells)
      msg(MSG::DEBUG) << " nE4pr=" << nE4pr
                      << " eE4pr=" << eE4prTot;

    msg(MSG::DEBUG) << endmsg;
  }

  m_tileID->set_do_checks(do_checks);
  // set back this flag to TileID
  m_tileTBID->set_do_checks(do_checks_tb);

}

#undef NEWTILECELL
