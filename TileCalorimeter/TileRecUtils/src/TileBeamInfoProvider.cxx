/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileCal includes
#include "TileRecUtils/TileBeamInfoProvider.h"
#include "TileIdentifier/TileHWID.h"
#include "TileIdentifier/TileTBFrag.h"
#include "TileIdentifier/TileTTL1Hash.h"
#include "TileEvent/TileTrigger.h"
#include "TileConditions/TileDCSSvc.h"
#include "TileConditions/TileBadChanTool.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"

// Calo include
#include "CaloIdentifier/CaloLVL1_ID.h" 

// Atlas includes
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "AthenaKernel/errorcheck.h"

#include "AthenaKernel/IAtRndmGenSvc.h"

// Gaudi includes
#include "GaudiKernel/IIncidentSvc.h"


//CLHEP includes
#include <CLHEP/Random/Randomize.h>


#include <iomanip>
#include <memory>

// uncomment line below for debug output
// #define TILECELL_DEBUG 1

static const InterfaceID IID_ITileBeamInfoProvider("TileBeamInfoProvider", 1, 0);

const InterfaceID&
TileBeamInfoProvider::interfaceID() {
  return IID_ITileBeamInfoProvider;
}

/**
 * Constructor
 */
TileBeamInfoProvider::TileBeamInfoProvider(const std::string& type,
    const std::string& name, const IInterface* parent)
    : AthAlgTool(type, name, parent)
    , m_tileDCSSvc("TileDCSSvc", name)
    , m_rndmSvc ("AtRndmGenSvc", name)
    , m_tileBadChanTool("TileBadChanTool")
    , m_tileHWID(0)
    , m_trigType(0)
    , m_laserFlag(0xFFFFFFFF)
    , m_calibMode(0xFFFFFFFF)
    , m_digiSize(0U)
    , m_BCID(0U)
    , m_evt(0U)
    , m_digitsCnt(0)
    , m_rcCnt(0)
    , m_beamElemCnt(0)
    , m_pHRengine(0)
    , m_rndmVec(0)
{

  declareInterface < TileBeamInfoProvider > (this);

  declareProperty("CheckDCS", m_checkDCS = false);
  declareProperty("SimulateTrips", m_simulateTrips = false, "Simulate drawer trips (default=false)");
  declareProperty("RndmSvc", m_rndmSvc, "Random Number Service used in TileCondToolTrip");

  m_checkDQ = m_checkDigi = m_checkBeam = false; //until begin of run inibit DQ status

  m_noiseFilterApplied = false;
  m_incompleteDigits = true;
}

/**
 * Destructor
 */
TileBeamInfoProvider::~TileBeamInfoProvider() {
}

/**
 * Initializer
 */
StatusCode TileBeamInfoProvider::initialize() {

  ATH_MSG_INFO("in initialize()");

  // Pass pointer of TileBeamInfoProvider to DQ status to make available some data members
  m_DQstatus.setTBIP(this);

  // retrieve TileID helpers from det store
  CHECK(detStore()->retrieve(m_tileHWID, "TileHWID"));

  //=== TileDCSSvc
  if (m_checkDCS)
    CHECK(m_tileDCSSvc.retrieve());

  m_evt = 0;
  m_trigType = 0;
  m_laserFlag = 0xFFFFFFFF;
  memset(m_cispar, 0, sizeof(m_cispar));

  m_calibMode = 0xFFFFFFFF;


  if (m_simulateTrips) {
    CHECK( m_tileBadChanTool.retrieve() );
    CHECK( m_rndmSvc.retrieve());
    m_pHRengine = m_rndmSvc->GetEngine("Tile_CondToolTrip");
    m_rndmVec = new double[TileCalibUtils::MAX_DRAWER];
    ATH_MSG_INFO("Drawer trips will be simulated");
  }

  if (!(m_beamElemContainerKey.key().empty() 
        && m_digitsContainerKey.key().empty()
        && m_rawChannelContainerKey.key().empty())
      || m_simulateTrips) {

    // Listen for begin of event
    ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", this->name());
    CHECK(incSvc.retrieve());

    int PRIORITY = 100;
    incSvc->addListener(this, "BeginRun", 101);
    incSvc->addListener(this, "BeginEvent", PRIORITY);

    ATH_MSG_DEBUG( "TileBeamInfoProvider created, taking info from '"
                   << m_beamElemContainerKey.key()
                   << "', '" << m_digitsContainerKey.key()
                   << "', and from '" << m_rawChannelContainerKey.key()
                   << ((m_simulateTrips) ? "', and from drawer trips simulation" : "'") );

  } else {
    ATH_MSG_DEBUG("TileBeamInfoProvider created, but BeginOfEvent incident not activated");
  }

  ATH_CHECK( m_eventInfoKey.initialize() );

  if (!m_beamElemContainerKey.key().empty()) {
    ATH_CHECK( m_beamElemContainerKey.initialize() ); 
  }

  if (!m_digitsContainerKey.key().empty()) {
    ATH_CHECK( m_digitsContainerKey.initialize() ); 
  }

  if (!m_rawChannelContainerKey.key().empty()) {
    ATH_CHECK( m_rawChannelContainerKey.initialize() ); 
  }

  if (!m_triggerContainerKey.key().empty()) {
    ATH_CHECK( m_triggerContainerKey.initialize() ); 
  }

  if (!m_laserObjectKey.key().empty()) {
    ATH_CHECK( m_laserObjectKey.initialize() ); 
  }


  return StatusCode::SUCCESS;
}

StatusCode TileBeamInfoProvider::finalize() {
  ATH_MSG_INFO("Finalizing");
  if (m_rndmVec) {
    delete[] m_rndmVec;
    m_rndmVec = NULL;
  }
  return StatusCode::SUCCESS;
}

/****************************************************************************
 * This setContainer is called in event overlay jobs from TileDigitsMaker
 when containers are in non-usual event store
 ****************************************************************************/
void TileBeamInfoProvider::setContainers(const TileDigitsContainer * digitsCnt,
    const TileRawChannelContainer * rcCnt,
    const TileBeamElemContainer * beamElemCnt) {
  m_rcCnt = rcCnt;
  m_digitsCnt = digitsCnt;
  m_beamElemCnt = beamElemCnt;

  m_checkDQ = (m_rcCnt != NULL);
  m_checkDigi = (m_digitsCnt != NULL);
  m_checkBeam = (m_beamElemCnt != NULL);
  m_noiseFilterApplied = false;
  m_incompleteDigits = false;

  m_DQstatus.setAllGood();
  m_DQstatus.setCheckDigi(m_checkDigi);

  if (m_evt == 0) {
    ATH_MSG_DEBUG("setContainer is called in first event");
  }
  ++m_evt;
}

/****************************************************************************
 * Implementation of Incident listener
 ****************************************************************************/
void TileBeamInfoProvider::handle(const Incident& inc) {
  if (inc.type() == "BeginRun") {
    ATH_MSG_DEBUG("Handling BeginRun incident");
    m_evt = 0;
    m_DQstatus.setAllGood();
    m_DQstatus.setFilled(true);
    return;
  }

  ATH_MSG_DEBUG("Handling BeginEvent incident");

  if (m_evt == 0) {
    //enable DQstatus checks
    m_checkDQ = (!m_rawChannelContainerKey.key().empty());
    m_checkDigi = (!m_digitsContainerKey.key().empty());
    m_checkBeam = (!m_beamElemContainerKey.key().empty());
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "Check DQ=" << m_checkDQ << endmsg;
      msg(MSG::DEBUG) << "Check Digi=" << m_checkDigi << endmsg;
      msg(MSG::DEBUG) << "Check Beam=" << m_checkBeam << endmsg;
    }
    m_DQstatus.setCheckDigi(m_checkDigi);
  }

  m_rcCnt = nullptr;
  m_digitsCnt = nullptr;
  m_beamElemCnt = nullptr;
  m_noiseFilterApplied = false;

  // retrive all containers from detector store and cache pointers for future use

  if (m_checkBeam) {
    SG::ReadHandle<TileBeamElemContainer> beamElemContainer(m_beamElemContainerKey);
    if (beamElemContainer.isValid()) {
      m_beamElemCnt = beamElemContainer.cptr();
    } else {
      ATH_MSG_ERROR("can't retrieve BeamElem '" << m_beamElemContainerKey.key() << "' from TDS");
      if (m_evt == 0) {
        m_checkBeam = false;
      }
    }
  }

  if (m_checkDigi) {
    SG::ReadHandle<TileDigitsContainer> digitsContainer(m_digitsContainerKey);
    if (digitsContainer.isValid()) {
      m_digitsCnt = digitsContainer.cptr();
    } else {
      ATH_MSG_ERROR("can't retrieve Digits '" << m_digitsContainerKey.key() << "' from TDS");
      if (m_evt == 0) {
        m_checkDigi = false;
        m_DQstatus.setCheckDigi(false);
      }
    }
  }

  if (m_checkDQ) {
    m_DQstatus.setAllGood();
    SG::ReadHandle<TileRawChannelContainer> rawChannelContainer(m_rawChannelContainerKey);
    if (rawChannelContainer.isValid()) {
      m_rcCnt = rawChannelContainer.cptr();
    } else {
      ATH_MSG_ERROR("can't retrieve RawChannels '" << m_rawChannelContainerKey.key() << "' from TDS");
      if (m_evt == 0) {
        m_checkDQ = false;
        m_DQstatus.setFilled(true);
      }
    }
  }

  m_BCID = 0;
  m_trigType = 0;
  m_laserFlag = 0xFFFFFFFF;
  memset(m_cispar, 0, sizeof(m_cispar));
  memset(m_laspar, 0, sizeof(m_laspar));

  if (m_beamElemCnt != NULL) {

    TileBeamElemContainer::const_iterator collItr = m_beamElemCnt->begin();
    TileBeamElemContainer::const_iterator lastColl = m_beamElemCnt->end();

    for (; collItr != lastColl; ++collItr) {

      int frag = (*collItr)->identify();

      if (m_trigType == 0 && (*collItr)->getLvl1Type() != 0) // take it from the ROD header
        m_trigType = -(*collItr)->getLvl1Type(); // make negative to distinguish from TileCal internal trig types

      switch (frag) {

        case LASE_PTN_FRAG: {

          TileBeamElemCollection::const_iterator beamItr = (*collItr)->begin();
          TileBeamElemCollection::const_iterator lastBeam = (*collItr)->end();

          if (beamItr != lastBeam) {
            std::vector < uint32_t > digits = (*beamItr)->get_digits();

            if (digits.size() > 0) {
              m_laserFlag = digits[0];
              if (m_laserFlag & 0xFF00)
                m_trigType = m_laserFlag >> 8;
            }
          }
        }
          break;

        case DIGI_PAR_FRAG: {

          m_BCID = (*collItr)->getRODBCID();

          TileBeamElemCollection::const_iterator beamItr = (*collItr)->begin();
          TileBeamElemCollection::const_iterator lastBeam = (*collItr)->end();

          for (; beamItr != lastBeam; ++beamItr) {

            HWIdentifier id = (*beamItr)->adc_HWID();
            std::vector < uint32_t > digits = (*beamItr)->get_digits();
            int cha = m_tileHWID->channel(id);

            if (cha < 15) {
              if (digits.size() > 0) {
                m_cispar[cha] = digits[0];
                ATH_MSG_VERBOSE("cispar [" << cha << "] = " << m_cispar[cha]);
              }
            } else if (cha == 15) {
              int siz = 15 + digits.size();
              if (siz > 110)
                siz = 110;
              for (int i = 15; i < siz; ++i) {
                m_cispar[i] = digits[i - 15];
                ATH_MSG_VERBOSE("cispar [" << i << "] = " << m_cispar[i]);
              }
              switch (m_cispar[16]) {

                case 0x02: {
                  int aux_ext = m_cispar[17];
                  m_cispar[17] = (aux_ext & 0x00ff); // dac
                  m_cispar[18] = (aux_ext >> 8) & 0x00ff; // 00
                  m_cispar[19] = (aux_ext >> 16) & 0x00ff; // 00
                  m_cispar[20] = (aux_ext >> 24) & 0x00ff; // small/large cap
                }
                  break;

                case 0x07: {
                  bool badpar = ((m_cispar[16] == m_cispar[17]) || (m_cispar[17] == m_cispar[18]));
                  int aux_ext = m_cispar[18];
                  m_cispar[18] = (aux_ext & 0x00ff) - 1; // pmt ext cispar starts from 1
                  m_cispar[19] = (aux_ext >> 8) & 0x00ff; // tower
                  m_cispar[20] = (aux_ext >> 16) & 0x00ff; // drawer

                  if (badpar || (aux_ext>>24)!=0 || m_cispar[18]>5 || m_cispar[19]>15 || m_cispar[20]>63) {
                    ATH_MSG_WARNING("bad cispar[16,17,18]: " << m_cispar[16] << " " << m_cispar[17] << " " << aux_ext
                                    << "  drawer,tower,pmt: " << m_cispar[20] << " " << m_cispar[19] << " " << (int)m_cispar[18]);
                    m_cispar[16] += 0x100; // flag bad events
                    m_cispar[18] = 5;
                    m_cispar[19] = 0xff;
                    m_cispar[20] = 0xff;
                  }

                  if (m_cispar[16] != m_cispar[17]) {
                    // Warning: nonportable.
                    union {
                      unsigned int i;
                      float f;
                    } chargeCnv;
                    chargeCnv.i = m_cispar[17];
                    m_cispar[17] = chargeCnv.f;
                  }
                }
                  break;
              }
            }
          }
        }
          break;

        case LASER_OBJ_FRAG: {

          m_BCID = (*collItr)->getRODBCID();

          TileBeamElemCollection::const_iterator beamItr = (*collItr)->begin();
          TileBeamElemCollection::const_iterator lastBeam = (*collItr)->end();

          if (beamItr != lastBeam) { // one channel is expected, check that it's really there

            //HWIdentifier id=(*beamItr)->adc_HWID();
            std::vector < uint32_t > digits = (*beamItr)->get_digits();
            int cha = std::min(32, (int) digits.size());

            while (--cha >= 0) {
              m_laspar[cha] = digits[cha];
              ATH_MSG_VERBOSE("laspar [" << cha << "] = " << m_laspar[cha]);
            }
          }
        }
          break;

        case COIN_TRIG1_FRAG:
        case COIN_TRIG2_FRAG:
        case COIN_TRIG3_FRAG:
        case COIN_TRIG4_FRAG:
        case COIN_TRIG5_FRAG:
        case COIN_TRIG6_FRAG:
        case COIN_TRIG7_FRAG:
        case COIN_TRIG8_FRAG: {
          unsigned int board = frag - COIN_TRIG1_FRAG;
          // make sure that we have enough space
          if (board >= m_coincTrig.size()) {
            m_coincTrig.resize(board + 1);
          }

          TileBeamElemCollection::const_iterator beamItr = (*collItr)->begin();
          TileBeamElemCollection::const_iterator lastBeam = (*collItr)->end();

          // loop over 4 integer words for one board
          for (; beamItr != lastBeam; ++beamItr) {

            HWIdentifier id = (*beamItr)->adc_HWID();
            std::vector < uint32_t > digits = (*beamItr)->get_digits();
            uint32_t amplitude = (digits.size() > 0) ? digits[0] : 0;
            int cha = m_tileHWID->channel(id);

            if (cha < 3) {
              int idx = cha * 32;
              for (int ibit = 0; ibit < 32; ++ibit) {
                m_coincTrig[board].trig[idx++] = (amplitude >> ibit) & 1;
              }
            } else if (cha == 3) {
              m_coincTrig[board].amp = amplitude;
            }
          }
        }
          break;

        default:
          break;
      }
    }
  }
  /*
   for (unsigned int i=0; i<m_coincTrig.size(); ++i) {
   std::cout << "board "<<i+1<<" amp="<<m_coincTrig[i].amp<<std::endl;
   for (unsigned int j=0; j<96; ++j) {
   std::cout << m_coincTrig[i].trig[j];
   }
   std::cout << std::endl;
   }
   */
  // we are going to put TileTrigger to StoreGate
  if (!m_triggerContainerKey.key().empty()) {

    SG::WriteHandle<TileTriggerContainer> triggerContainer(m_triggerContainerKey);
    if (triggerContainer.record( std::make_unique<TileTriggerContainer>() ).isSuccess()) {

      int maxboard = m_coincTrig.size();
      if (maxboard > 0) {
        Identifier mtid;
        double mtsum(0.0);
        std::vector < Identifier > boardtid(maxboard);
        std::vector<double> boardtsum(maxboard);
        std::vector < Identifier > backtid(maxboard);
        std::vector<double> backtsum(maxboard);
        
        // FIXME:: convert coincTrig to TileTrigger
        
        TileTrigger* tileTrigger = new TileTrigger(mtid, mtsum, boardtid,
                                                   boardtsum, backtid, backtsum);
        triggerContainer->push_back(tileTrigger);
      }
    } else {
      ATH_MSG_ERROR("failed to register the TileTrigger container: "
                    << m_triggerContainerKey.key() << " in StoreGate");

    }
    
  }

  // we are going to put TileLaserObject to StoreGate
  if (!m_laserObjectKey.key().empty()) {

    SG::WriteHandle<TileLaserObject> laserObject(m_laserObjectKey);
    if(laserObject.record( std::make_unique<TileLaserObject>() ).isSuccess()) {
      laserObject->setBCID(m_BCID);

      // FIXME: a lot of set methods here 
      // to copy m_laspar to TileLaserObject

    } else {
      ATH_MSG_ERROR("failed to register the TileLaserObject: "
                    << m_laserObjectKey.key() << " in StoreGate");
    }

  }

  if (m_calibMode == 0xFFFFFFFF) {

    m_digiSize = 0;

    if (m_digitsCnt != NULL) {
      // Get iterator for all TDColl in TDCont
      TileDigitsContainer::const_iterator itColl = m_digitsCnt->begin();
      TileDigitsContainer::const_iterator itCollEnd = m_digitsCnt->end();

      bool found = false;
      bool isCalib = false;
      bool incomplete = false;
      for (; itColl != itCollEnd; ++itColl) {
        if (m_BCID == 0)
          m_BCID = (*itColl)->getRODBCID();
        incomplete |= ((*itColl)->size() < 48);
        if ((*itColl)->size() > 0) {
          int dsize = (*((*itColl)->begin()))->NtimeSamples();
          if (4 < dsize && dsize < 15) { // don't use strange fragments
            isCalib |= (*itColl)->isCalibMode();
            m_digiSize = dsize;
            found = true;
          }
        }
      }
      m_incompleteDigits = incomplete;
      if (found)
        m_calibMode = (isCalib) ? 1 : 0;
      m_DQstatus.setBiGain(isCalib);
    }
  }

  if (msgLvl(MSG::VERBOSE)) {
    msg(MSG::VERBOSE) << "BCID = " << m_BCID << endmsg;
    msg(MSG::VERBOSE) << "digi size = " << m_digiSize << endmsg;
    msg(MSG::VERBOSE) << "zero-suppressed digi container = " << m_incompleteDigits << endmsg;

    if (m_trigType < 0)
      msg(MSG::VERBOSE) << "trig type = " << m_trigType << " (Level-1 type) " << endmsg;
    else
      msg(MSG::VERBOSE) << "trig type = " << m_trigType
                        << " (TileCal internal type) " << endmsg;

    msg(MSG::VERBOSE) << "calib mode = " << m_calibMode << endmsg;
  }

  // Set BCID in DQstatus object for  explicit BCID check
  m_DQstatus.setRODBCID(m_BCID);

  if (m_simulateTrips) m_DQstatus.setFilled(false);

  if (m_evt == 0) {
    ATH_MSG_DEBUG("End of handling first BeginEvent incident");
  }
  ++m_evt;
}

uint32_t TileBeamInfoProvider::checkCalibMode(void) {
  // on demand - loop over all collection to check if
  // we are running in bi-gain mode
  if (m_digitsCnt != NULL) {
    bool found = false;
    bool calibMode = false;
    bool incomplete = false;
    // Get iterator for all TDColl in TDCont
    TileDigitsContainer::const_iterator itColl = m_digitsCnt->begin();
    TileDigitsContainer::const_iterator itCollEnd = m_digitsCnt->end();

    for (; itColl != itCollEnd; ++itColl) {

      incomplete |= ((*itColl)->size() < 48);
      if ((*itColl)->size() > 0) {
        calibMode |= (*itColl)->isCalibMode();
        found = true;
      }
    }
    m_incompleteDigits = incomplete;
    if (found)
      m_calibMode = (calibMode) ? 1 : 0;
  }
  return m_calibMode;
}

// returns DCS HV channel status
bool TileBeamInfoProvider::isChanDCSgood(int partition, int drawer,
    int channel) const {
  if (m_checkDCS) {
    TileDCSSvc::TileDCSStatus Status = m_tileDCSSvc->getDCSSTATUS(partition,
        drawer, channel);
    if (Status > TileDCSSvc::WARNING) {
      ATH_MSG_DEBUG("Module=" << m_tileDCSSvc->partitionName(partition) << std::setw(2) << std::setfill('0') << drawer + 1
                    << " channel=" << channel
                    << " masking becasue of bad DCS status=" << Status);
      return false;
    } else {
      return true;
    }
  }
  return true;
}

// returns pointer to TileDQstatus and calls function to fill the data members if it's not filled yet
const TileDQstatus * TileBeamInfoProvider::getDQstatus() {

  // return immediately if it's filled already
  if (m_DQstatus.isFilled())
    return &m_DQstatus;

  //Fill BCID from the EventInfo, if available
  SG::ReadHandle<xAOD::EventInfo> eventInfo(m_eventInfoKey);
  if (eventInfo.isValid()) {
    m_DQstatus.setRODBCID(eventInfo->bcid());
  }

  if (m_rcCnt != NULL) {

    if (m_digitsCnt != NULL) {

      TileDigitsContainer::const_iterator collItr = m_digitsCnt->begin();
      TileDigitsContainer::const_iterator lastColl = m_digitsCnt->end();

      for (; collItr != lastColl; collItr++) {// Loop over TileModules
        int frag = (*collItr)->identify();
        int partition = (frag >> 8);
        int drawer = (frag & 0x3F);

        std::vector < uint32_t > data = (*collItr)->getFragChipHeaderWords();
        unsigned int dataSize = std::min(16u, (unsigned int) data.size());
        for (unsigned int dmu = 0; dmu < dataSize; ++dmu) {
          if (data[dmu] == 0xFFFFFFFF)
            m_DQstatus.setEmptyEvent(partition, drawer, dmu, 0, 1);
        }

        data = (*collItr)->getFragChipHeaderWordsHigh();
        dataSize = std::min(16u, (unsigned int) data.size());
        for (unsigned int dmu = 0; dmu < dataSize; ++dmu) {
          if (data[dmu] == 0xFFFFFFFF)
            m_DQstatus.setEmptyEvent(partition, drawer, dmu, 1, 1);
        }
      }
    } // end if m_checkDigi

    TileFragHash::TYPE RChType = m_rcCnt->get_type();
    if (RChType != TileFragHash::OptFilterDsp
        && RChType != TileFragHash::OptFilterDspCompressed) {
      ATH_MSG_INFO("RawChannelContaier didn't come from BS - don't check DQ flags");
      ATH_MSG_INFO("RChType = " << RChType);
      m_DQstatus.setAllGood();
      m_checkDQ = false;
    } else {
      TileRawChannelContainer::const_iterator itColl = m_rcCnt->begin();
      TileRawChannelContainer::const_iterator itCollEnd = m_rcCnt->end();

      // Go through all TileRawChannelCollections
      for (; itColl != itCollEnd; ++itColl) {
        const TileRawChannelCollection *coll = (*itColl);
        ATH_MSG_VERBOSE("RCh collection 0x" << MSG::hex
                       << coll->identify() << MSG::dec
                       << " size=" << coll->size());

        m_DQstatus.fillArrays(coll, 0);
        m_DQstatus.fillArrays(coll, 1);
      }
      if (m_evt == 1 && m_DQstatus.nonZeroCounter() == 0) {
        ATH_MSG_INFO("all DQ elements are empty - don't check DQ flags");
        m_DQstatus.setAllGood();
        m_checkDQ = false;
      } else {
        ATH_MSG_DEBUG("BiGain mode: " << ((m_DQstatus.isBiGain()) ? "true" : "false"));
      }
    }
  }


  if (m_simulateTrips) {
    for (unsigned int partition = 1; partition < TileCalibUtils::MAX_ROS; ++partition) {
      CLHEP::RandFlat::shootArray(m_pHRengine, TileCalibUtils::MAX_DRAWER, m_rndmVec);
      std::vector<float> trips = m_tileBadChanTool->getTripsProbabilities(partition);
      m_DQstatus.fillTrips(partition, trips, m_rndmVec);
    }
  }

  m_DQstatus.setFilled(true);
  return &m_DQstatus;
}

//-----------------------------------------------------------------
// Implementation of TileDQstatus class
//-----------------------------------------------------------------

const int TileDQstatus::s_ch2dmuLB[48] = { 0, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
  0, 0, 0, 0 };

const int TileDQstatus::s_ch2dmuEB[48] = { 0, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 2, 
  2, 2, 2, 2, 2, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 2, 2,
  2, 2, 2, 2 };

const int TileDQstatus::s_ch2dmuEBspecial[48] = { 2, 2, 2, 1, 
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
  0, 2, 2, 2, 2, 2, 2, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0,
  2, 2, 2, 2, 2, 2 };  

// Constructor
TileDQstatus::TileDQstatus():m_TBIP(0) {
  setAllGood();
  m_isBiGain = false;
  m_checkDigi = false;
  m_BCID = 0;
}

void TileDQstatus::setAllGood() {
  // Intitalize all arrays to no errors
  m_isFilled = false;
  //m_isBiGain = false; //BAD!
  m_counter = 0;
  memset(m_EmptyEventArray, 0, sizeof(m_EmptyEventArray));
  memset(m_GlobalCRCErrArray, 0, sizeof(m_GlobalCRCErrArray));
  memset(m_FE_DMUmaskArray, -1, sizeof(m_FE_DMUmaskArray));
  memset(m_ROD_DMUmaskArray, -1, sizeof(m_ROD_DMUmaskArray));
  memset(m_BCIDErrArray, 0, sizeof(m_BCIDErrArray));
  memset(m_BCIDErrArrayDetail, 0, sizeof(m_BCIDErrArrayDetail));
  memset(m_HeaderFormatErrArray, 0, sizeof(m_HeaderFormatErrArray));
  memset(m_HeaderParityErrArray, 0, sizeof(m_HeaderParityErrArray));
  memset(m_SampleFormatErrArray, 0, sizeof(m_SampleFormatErrArray));
  memset(m_SampleParityErrArray, 0, sizeof(m_SampleParityErrArray));
  memset(m_MemoryParityErrArray, 0, sizeof(m_MemoryParityErrArray));
  memset(m_SingleStrobeErrArray, 0, sizeof(m_SingleStrobeErrArray));
  memset(m_DoubleStrobeErrArray, 0, sizeof(m_DoubleStrobeErrArray));
}

// Destructor
TileDQstatus::~TileDQstatus() {
}

// Fucntion to fill error arrays from DQ fragment stores in DSP RawChannelContainer
// If monogain run, both gains contain the same results
void TileDQstatus::fillArrays(const TileRawChannelCollection *coll, int gain) {

  int frag = coll->identify();
  int partition = (frag >> 8); // 0=AUX,1=LBA,2=LBC,3=EBA,4=EBC
  int drawer = (frag & 0x3F);  // 0-63
  bool eb = (frag > 0x2ff);    // true if ext.barrel
  bool ebsp = (frag == 0x30e || frag == 0x411);// EBA15 or EBC18

  m_isBiGain |= (coll->size() > 48);
  // attention! it's assignment below, i.e. only single "=", not "=="
  // LF: ... which is something very dangerous. Does it provide any speed advantage?
  if ((m_GlobalCRCErrArray[partition][drawer][gain] = coll->getFragGlobalCRC()))    ++m_counter;
  if ((m_BCIDErrArray[partition][drawer][gain] = coll->getFragBCID()))              ++m_counter;
  if ((m_MemoryParityErrArray[partition][drawer][gain] = coll->getFragMemoryPar())) ++m_counter;
  if ((m_SingleStrobeErrArray[partition][drawer][gain] = coll->getFragSstrobe()))   ++m_counter;
  if ((m_DoubleStrobeErrArray[partition][drawer][gain] = coll->getFragDstrobe()))   ++m_counter;
  if ((m_HeaderFormatErrArray[partition][drawer][gain] = coll->getFragHeaderBit())) ++m_counter;
  if ((m_HeaderParityErrArray[partition][drawer][gain] = coll->getFragHeaderPar())) ++m_counter;
  if ((m_SampleFormatErrArray[partition][drawer][gain] = coll->getFragSampleBit())) ++m_counter;
  if ((m_SampleParityErrArray[partition][drawer][gain] = coll->getFragSamplePar())) ++m_counter;
  if ((m_ROD_DMUmaskArray[partition][drawer][gain] = coll->getFragRODChipMask()))   ++m_counter;
// special treatment of FE DMU mask - take into account that some DMUs in Ext.barrel do not exist
  if(uint32_t FE_DMUmask = coll->getFragFEChipMask()) {
    if (eb) { // EBA or EBC
      if (ebsp)  FE_DMUmask <<= 1; // shift by one DMU in EBA15 EBC18
      FE_DMUmask = (FE_DMUmask & 0xFF) | ((FE_DMUmask & 0xF00) << 2); // shift upper half by two DMUs
    }
     m_FE_DMUmaskArray[partition][drawer][gain] = FE_DMUmask;
    ++m_counter;
  }

  unsigned short BCIDerr =
      (unsigned short) m_BCIDErrArray[partition][drawer][gain];
  if (BCIDerr & 0x2) { // DMU1 (second DMU) is bad - can not trust others 
    m_BCIDErrArray[partition][drawer][gain] = -1;
  } else {
    // additional check if DQ frag BCID is the same as event BCID 
    uint32_t DSPBCID = coll->getFragDSPBCID();
    if (DSPBCID != 0xDEAD && DSPBCID != m_BCID) { // DSP BCID doesn't match! all wrong
      m_BCIDErrArray[partition][drawer][gain] = -1; // I preferred 0xFFFF; but variable is decleared as signed!
      ++m_counter;
    } else {
      int n_badMB = 0;
      if (eb) { // do not count non-existing DMUs in EB
        if (ebsp) {
          BCIDerr &= 0x3cfe;
        } else {
          BCIDerr &= 0x3cff;
        }
      }
      while (BCIDerr) {
        if (BCIDerr & 0xF)
          ++n_badMB;
        BCIDerr >>= 4;
      }
      if (n_badMB == 4) { // BCID errors in all 4 motherboards - assume that all DMUs are bad
        m_BCIDErrArray[partition][drawer][gain] = -1;
#ifdef TILECELL_DEBUG
        std::cout << "masking whole drawer " << drawer << " in partition " << partition << " because all 4 MB have BCID errors"
                  << std::endl;
#endif
      }
    }
  }

  if ((m_BCIDErrArray[partition][drawer][gain] & 0x2) && m_checkDigi)
      fillBCIDErrDetail(frag, gain);
  
  if (m_HeaderFormatErrArray[partition][drawer][gain]
      || m_HeaderParityErrArray[partition][drawer][gain]
      || m_SampleFormatErrArray[partition][drawer][gain]
      || m_SampleParityErrArray[partition][drawer][gain]) {
    // can not trust FE mask, assume that FE is good
    m_FE_DMUmaskArray[partition][drawer][gain] = -1;
  }

#ifdef TILECELL_DEBUG
  std::cout << std::hex 
            << " " << coll->getFragGlobalCRC()
            << " " << coll->getFragFEChipMask()
            << " " << coll->getFragRODChipMask()
            << " " << coll->getFragDSPBCID()
            << " " << coll->getFragBCID()
            << " " << coll->getFragMemoryPar()
            << " " << coll->getFragSstrobe()
            << " " << coll->getFragDstrobe()
            << " " << coll->getFragHeaderBit()
            << " " << coll->getFragHeaderPar()
            << " " << coll->getFragSampleBit()
            << " " << coll->getFragSamplePar()
            << " counter is " << std::dec << m_counter << std::endl;
#endif
  
}


//  Returns AND of all error check reults
bool TileDQstatus::isAdcDQgood(int partition, int drawer, int ch, int gain) const {

  int dmu = ch/3;

#ifdef TILECELL_DEBUG

  int errorArray[11];
  errorArray[0] = checkGlobalCRCErr    (partition,drawer,gain);
  errorArray[1] = checkROD_CRCErr      (partition,drawer,dmu,gain);
  errorArray[2] = (m_isBiGain) ? 0 : checkFE_CRCErr(partition,drawer,dmu,gain);  // Skip FE CRC for bigain runs
  errorArray[3] = checkBCIDErr         (partition,drawer,dmu,gain);
  errorArray[4] = checkHeaderFormatErr (partition,drawer,dmu,gain);
  errorArray[5] = checkHeaderParityErr (partition,drawer,dmu,gain);
  errorArray[6] = checkSampleFormatErr (partition,drawer,dmu,gain);
  errorArray[7] = checkSampleParityErr (partition,drawer,dmu,gain);
  errorArray[8] = checkMemoryParityErr (partition,drawer,dmu,gain);
  errorArray[9] = checkSingleStrobeErr (partition,drawer,dmu,gain);
  errorArray[10]= checkDoubleStrobeErr (partition,drawer,dmu,gain);

  int nError=0;
  for(int i=0;i<9;++i){ // exclude Strobe errors from decision
    nError += errorArray[i];
  }
  std::cout << std::dec <<"Part: " << partition << " Drawer: " << drawer+1 << " DMU: " << dmu << " ch: " << ch << std::endl;
  std::cout << "IsBigain "        << m_isBiGain << std::endl;
  std::cout << "EmptyEvent (LG) " << m_EmptyEventArray[partition][drawer][dmu][0]  << std::endl;
  std::cout << "EmptyEvent (HG) " << m_EmptyEventArray[partition][drawer][dmu][1]  << std::endl;
  std::cout << "GlobalCRCErr "    << errorArray[0] << std::endl;
  std::cout << "ROD_CRCmask (LG) " << std::hex <<  m_ROD_DMUmaskArray[partition][drawer][0] << std::endl;
  std::cout << "ROD_CRCmask (HG) " << std::hex <<  m_ROD_DMUmaskArray[partition][drawer][1] << std::endl;
  std::cout << "ROD_CRCErr "      << std::dec << errorArray[1] << std::endl;
  std::cout << "FE_CRCmask (LG) "  << std::hex << m_FE_DMUmaskArray[partition][drawer][0] << std::endl;
  std::cout << "FE_CRCmask (HG) "  << std::hex << m_FE_DMUmaskArray[partition][drawer][1] << std::endl;
  std::cout << "FE_CRCErr "       << std::dec << errorArray[2] << std::endl;
  std::cout << "BCIDErr "         << std::dec << errorArray[3] << std::endl;
  std::cout << "HeaderFormatErr " << std::dec << errorArray[4] << std::endl;
  std::cout << "HeaderParityErr " << std::dec << errorArray[5] << std::endl;
  std::cout << "SampleFormatErr " << std::dec << errorArray[6] << std::endl;
  std::cout << "SampleParityErr " << std::dec << errorArray[7] << std::endl;
  std::cout << "MemoryParityErr " << std::dec << errorArray[8] << std::endl;
  std::cout << "SingleStrobeErr " << std::dec << errorArray[9] << std::endl;
  std::cout << "DoubleStrobeErr " << std::dec << errorArray[10] << std::endl;
  std::cout << "Total number of errors: " << std::dec << nError << std::endl;

#endif

  if (checkGlobalCRCErr(partition, drawer, gain)
      || checkROD_CRCErr(partition, drawer, dmu, gain)
      || ((m_isBiGain) ? 0 : checkFE_CRCErr(partition, drawer, dmu, gain)) // Skip FE CRC for bigain runs
      || checkBCIDErr(partition, drawer, dmu, gain)
      || checkHeaderFormatErr(partition, drawer, dmu, gain)
      || checkHeaderParityErr(partition, drawer, dmu, gain)
      || checkSampleFormatErr(partition, drawer, dmu, gain)
      || checkSampleParityErr(partition, drawer, dmu, gain)
      || checkMemoryParityErr(partition, drawer, dmu, gain)) {
    return false;
  }

  return true;
}

// Returns AND of error checks for both adc's for a single channel
bool TileDQstatus::isChanDQgood(int partition, int drawer, int ch) const {
  bool isGood = isAdcDQgood(partition, drawer, ch, 0);

  if (m_isBiGain)
    isGood &= isAdcDQgood(partition, drawer, ch, 1);

  return isGood;
}

void TileDQstatus::fillBCIDErrDetail(int frag, int gain) {

  int partition = (frag >> 8);
  int drawer = (frag & 0x3F);
  m_BCIDErrArrayDetail[partition][drawer][gain] =
      m_BCIDErrArray[partition][drawer][gain]; //initialize
  const TileDigitsContainer* digitsCnt = m_TBIP->m_digitsCnt;
  if (digitsCnt == NULL)
    // by default a conservative return value of 1 (channel Mask)
    m_BCIDErrArrayDetail[partition][drawer][gain] = short(-1);  
  else {
    TileDigitsContainer::const_iterator collItr = digitsCnt->begin();
    TileDigitsContainer::const_iterator lastColl = digitsCnt->end();

    for (; collItr != lastColl; collItr++) { // Loop over TileModules
      if ((*collItr)->identify() != frag)
        continue;

      std::vector < uint32_t > data;
      if ( isBiGain() && gain ) //high Gain in bigain run
        data = (*collItr)->getFragChipHeaderWordsHigh();
      else
        data = (*collItr)->getFragChipHeaderWords();

      unsigned int dataSize = std::min(16u, (unsigned int) data.size());
      short bcidCheck = 0x0;
      uint32_t rodbcid = (*collItr)->getRODBCID();
      for (unsigned int dmu = 0; dmu < dataSize; ++dmu) {
        bcidCheck |= ((data[dmu] & 0xFFF) != rodbcid) << dmu;
#ifdef TILECELL_DEBUG
        std::cout << "Part: " << partition << " Drawer: " << drawer << " DMU: " << dmu << (gain?"HG":"LG") 
                  << " DMU BCID: " << (data[dmu] & 0xFFF) << " ROD BCID: " << rodbcid << std::endl;
#endif
      }
      if (dataSize > 0)
        m_BCIDErrArrayDetail[partition][drawer][gain] = bcidCheck;
      break; // break after the digit is found
    }
  }
}


void TileDQstatus::fillTrips(unsigned int partition, const std::vector<float>& trips, double* rndmVec) {
  for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer) {
    if (trips[drawer] > rndmVec[drawer]) {
      if (m_TBIP->msgLvl(MSG::DEBUG)) {
        m_TBIP->msg(MSG::DEBUG) << "The drawer has been tripped (simulation): "
                                << TileCalibUtils::getDrawerString(partition, drawer) << endmsg;
      }
      m_GlobalCRCErrArray[partition][drawer][0] = -1;
      m_GlobalCRCErrArray[partition][drawer][1] = -1;
      m_counter += 2;
    }
  }
}

