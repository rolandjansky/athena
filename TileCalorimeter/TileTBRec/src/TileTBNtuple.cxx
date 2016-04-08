/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileTBNtuple.cxx
//  Author   : Paulo da Silva
//  Created  : Oct, 2003
//
//  DESCRIPTION:
//     Implement the algorithm to save TB Beam ROD dataX
//
//  HISTORY:
//
//  BUGS:
//
//*****************************************************************************

// Athena includes
# include "AthenaKernel/errorcheck.h"

//Event info
#include "xAODEventInfo/EventInfo.h"
 
#include "PathResolver/PathResolver.h"
#include <cmath>

//Calo includes
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloID.h"

//TileCalo includes
#include "CaloIdentifier/TileID.h"
#include "TileIdentifier/TileHWID.h"
#include "TileIdentifier/TileTBFrag.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileConditions/TileCablingService.h"
#include "TileConditions/TileCondToolEmscale.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileBeamElemContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileRecUtils/TileRawChannelBuilderFlatFilter.h"
#include "TileRecUtils/TileBeamInfoProvider.h"
#include "TileByteStream/TileBeamElemContByteStreamCnv.h"
#include "TileTBRec/TileTBNtuple.h"

#include <fstream>

#define WRONG_SAMPLE(frag,chan,size)                                  \
msg(MSG::ERROR) << "Wrong no. of samples (" << size                   \
                << ") for channel " << chan                           \
                << " in frag 0x" << MSG::hex << frag << MSG::dec      \
                << " - " << BeamFragName[frag&0x1F] << endmsg;

#define WRONG_CHANNEL(frag,chan)                                      \
msg(MSG::ERROR) << "Wrong channel " << chan                           \
                << " in frag 0x" << MSG::hex << frag << MSG::dec      \
                << " - " << BeamFragName[frag&0x1F] << endmsg;

#define FRAG_FOUND(frag,chan,size)                                    \
if (msgLvl(MSG::DEBUG))                                               \
  msg(MSG::DEBUG) << "Found channel " << chan                         \
                  << " in frag 0x" << MSG::hex << frag << MSG::dec    \
                  << " - " << BeamFragName[frag&0x1F]                 \
                  << " of size " << size << endmsg;


#define SIGNAL_FOUND(frag,chan,amplitude)                             \
if (msgLvl(MSG::DEBUG))                                               \
  msg(MSG::DEBUG) << "Found channel " << chan                         \
                  << " in frag 0x" << MSG::hex << frag << MSG::dec    \
                  << " - " << BeamFragName[frag&0x1F]                 \
                  << " with amp=" << amplitude << endmsg;

// Constructor & deconstructor
/// OBSLETE CLASS: Please, use TileTBAANtuple for rel > 13.0.30.
// Constructor & deconstructor
/** @class TileTBNtuple
 *  @brief Obsolete class to produce commissioning ntuples. use TileTBAANtuple for rel > 13.0.30.
 */
TileTBNtuple::TileTBNtuple(std::string name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_calibMode(false)
  , m_ntuplePtr(0)
  , m_ntupleCreated(false)
  , m_evtNr(0)
  , m_beamBN2X1(0.0F)
  , m_beamBN2X2(0.0F)
  , m_beamBN2Y1(0.0F)
  , m_beamBN2Y2(0.0F)
  , m_beamBN2Z(0.0F)
  , m_beamBN1X1(0.0F)
  , m_beamBN1X2(0.0F)
  , m_beamBN1Y1(0.0F)
  , m_beamBN1Y2(0.0F)
  , m_beamBN1Z(0.0F)
  , m_beamBC0X1(0.0F)
  , m_beamBC0X2(0.0F)
  , m_beamBC0Y1(0.0F)
  , m_beamBC0Y2(0.0F)
  , m_beamBC0Z(0.0F)
  , m_beamBC1X1(0.0F)
  , m_beamBC1X2(0.0F)
  , m_beamBC1Y1(0.0F)
  , m_beamBC1Y2(0.0F)
  , m_beamBC1Z(0.0F)
  , m_beamBC2X1(0.0F)
  , m_beamBC2X2(0.0F)
  , m_beamBC2Y1(0.0F)
  , m_beamBC2Y2(0.0F)
  , m_beamBC2Z(0.0F)
  , m_beamBC2Zperiod1(0.0F)
  , m_beamBC2Zperiod2(0.0F)
  , m_runNumber(0)
  , m_eta(0.0F)
  , m_theta(0.0F)
  , m_radius(0.0F)
  , m_calibrateEnergyThisEvent(false)
  , m_calibrateEnergy(false)
  , m_useDspUnits(false)
  , m_unpackAdder(false)
  , m_completeNtuple(false)
  , m_commitNtuple(false)
  , m_bsInput(false)
  , m_finalUnit(0)
  , m_treeSize(0)
  , m_nSamples(0)
  , m_nDrawers(0)
  , m_adderFilterAlgTool("TileRawChannelBuilderFlatFilter/TileAdderFlatFilter",this)
  , m_tileID(0)
  , m_tileHWID(0)
  , m_cabling(0)
  , m_tileToolEmscale("TileCondToolEmscale")
  , m_beamInfo("TileBeamInfoProvider/TileBeamInfoProvider")
  , m_beamCnv(0)
  , m_currentFileNum(0)
{
  declareProperty("TileCondToolEmscale", m_tileToolEmscale);
  declareProperty("TileDigitsContainer", m_digitsContainer = "TileDigitsCnt");    
  declareProperty("TileBeamElemContainer", m_beamElemContainer = "TileBeamElemCnt");
  declareProperty("TileRawChannelContainerFlat", m_flatRawChannelContainer = "TileRawChannelFlat");
  declareProperty("TileLaserObject", m_laserObject = "");
  declareProperty("TileRawChannelContainerFit", m_fitRawChannelContainer = ""); // don't create
  declareProperty("TileRawChannelContainerFitCool", m_fitcRawChannelContainer = ""); // don't create
  declareProperty("TileRawChannelContainerOpt", m_optRawChannelContainer = ""); // by default
  declareProperty("TileRawChannelContainerDsp", m_dspRawChannelContainer = ""); //
  declareProperty("CalibrateEnergy", m_calibrateEnergy = true);
  declareProperty("UseDspUnits", m_useDspUnits = false);
  declareProperty("OfflineUnits", m_finalUnit = 4);
  declareProperty("CalibMode", m_calibMode = false);  
  declareProperty("UnpackAdder", m_unpackAdder = false);  
  declareProperty("CompleteNtuple", m_completeNtuple = true);  
  declareProperty("CommitNtuple", m_commitNtuple = true);
  declareProperty("BSInput", m_bsInput = true);
    
  declareProperty("NTupleLoc", m_ntupleLoc = "/NTUPLES/FILE%d/TileRec");
  declareProperty("NTupleID", m_ntupleID = "h1000");
  declareProperty("TreeSize", m_treeSize = 16000000000LL); // dummy

  declareProperty("NSamples", m_nSamples = 9);
  declareProperty("NDrawers", m_nDrawers = 6);

  declareProperty("EventsPerFile", m_eventsPerFile = 200000);
  declareProperty("StreamName", m_streamName = ""); // dummy

//  m_drawerList.resize(6);    m_drawerType.resize(6);
//  m_drawerList[0] = "0x200"; m_drawerType[0] = 2; // barrel neg
//  m_drawerList[1] = "0x201"; m_drawerType[1] = 2; // barrel neg
//  m_drawerList[2] = "0x202"; m_drawerType[2] = 4; // ext barrel neg
//  m_drawerList[3] = "0x100"; m_drawerType[3] = 1; // barrel pos
//  m_drawerList[4] = "0x101"; m_drawerType[4] = 1; // barrel pos
//  m_drawerList[5] = "0x102"; m_drawerType[5] = 3; // ext barrel pos

  m_drawerList.resize(1);  m_drawerList[0] = "-1"; // setup drawer list from data
  m_drawerType.clear();    // take drawer type from Frag ID (doesn't work for 2003)
  m_drawerMap.clear();     // for 2003 need to use mapping given above

  char frg[6] = "0x000";
  m_beamFragList.clear();
  for (unsigned int i = 0; i < sizeof(m_beamIdList) / sizeof(bool); ++i) {
    m_beamIdList[i] = false;
    // no coins trig by default (this is the case for 2004)
    if (i <= ECAL_ADC_FRAG || i >= COMMON_ADC1_FRAG) {
      sprintf(frg, "0x%3.3x", i);
      m_beamFragList.push_back((std::string) frg);
    }
  }

  m_rosName.push_back("B"); // 0 - this is beam rod name
  m_rosName.push_back("A"); m_rosName.push_back("C");
  m_rosName.push_back("D"); m_rosName.push_back("E");

  declareProperty("rosName", m_rosName);
  declareProperty("drawerList", m_drawerList);
  declareProperty("drawerType", m_drawerType);  
  declareProperty("beamFragList", m_beamFragList);

  declareProperty("BN2X1",m_beamBN2X1 = 0.0);
  declareProperty("BN2X2",m_beamBN2X2 = 0.2);
  declareProperty("BN2Y1",m_beamBN2Y1 = 0.0);
  declareProperty("BN2Y2",m_beamBN2Y2 = 0.2);
  declareProperty("BN2Z", m_beamBN2Z  = 1.0);

  declareProperty("BN1X1",m_beamBN1X1 = 0.0);
  declareProperty("BN1X2",m_beamBN1X2 = 0.2);
  declareProperty("BN1Y1",m_beamBN1Y1 = 0.0);
  declareProperty("BN1Y2",m_beamBN1Y2 = 0.2);
  declareProperty("BN1Z", m_beamBN1Z  = 1.0);

  declareProperty("BC0X1",m_beamBC0X1 = 0.0);
  declareProperty("BC0X2",m_beamBC0X2 = 0.2);
  declareProperty("BC0Y1",m_beamBC0Y1 = 0.0);
  declareProperty("BC0Y2",m_beamBC0Y2 = 0.2);
  declareProperty("BC0Z", m_beamBC0Z  = 17138.0);

  declareProperty("BC1X1",m_beamBC1X1 =-0.938);
  declareProperty("BC1X2",m_beamBC1X2 = 0.1747);
  declareProperty("BC1Y1",m_beamBC1Y1 = 0.125);
  declareProperty("BC1Y2",m_beamBC1Y2 = 0.1765);
  declareProperty("BC1Z", m_beamBC1Z  = 13788.0);
  
  declareProperty("BC2X1",m_beamBC2X1 =-0.9369);
  declareProperty("BC2X2",m_beamBC2X2 = 0.191);
  declareProperty("BC2Y1",m_beamBC2Y1 =-1.29);
  declareProperty("BC2Y2",m_beamBC2Y2 = 0.187);
  declareProperty("BC2Z", m_beamBC2Z  = 9411.0);

  // for CTB 2004 only
  declareProperty("BC2Zperiod1", m_beamBC2Zperiod1 = 9411.0);
  declareProperty("BC2Zperiod2", m_beamBC2Zperiod2 = 12188.0);

  declareProperty("Radius", m_radius = 2280.0);
  declareProperty("EtaFileName", m_etaFileName = "TileEtaCTB.txt");

  m_eta=0.0;
  m_theta=0.0;
  m_runNumber=0;
}


TileTBNtuple::~TileTBNtuple() {
}

// Alg standard interface function
StatusCode TileTBNtuple::initialize() {

  // find TileCablingService
  m_cabling = TileCablingService::getInstance();

  // retrieve TileID helper from det store

  CHECK( detStore()->retrieve(m_tileID) );

  CHECK( detStore()->retrieve(m_tileHWID) );

  //=== get TileCondToolEmscale
  CHECK( m_tileToolEmscale.retrieve() );

  CHECK( m_beamInfo.retrieve() );

  CHECK( m_beamInfo->setProperty("TileBeamElemContainer", m_beamElemContainer) );
  CHECK( m_beamInfo->setProperty("TileDigitsContainer", m_digitsContainer) );

  if (m_unpackAdder) {
    // get TileRawChannelBuilderFlatFilter for adder energy calculation
    CHECK( m_adderFilterAlgTool.retrieve() );

    m_adderFilterAlgTool->setProperty("TileRawChannelContainer", "TileAdderFlat");
    m_adderFilterAlgTool->setProperty("calibrateEnergy", "true");

    m_adderFilterAlgTool->setProperty("PedStart", "0");
    m_adderFilterAlgTool->setProperty("PedLength", "1");
    m_adderFilterAlgTool->setProperty("PedOffset", "0");
    m_adderFilterAlgTool->setProperty("SignalStart", "1");
    m_adderFilterAlgTool->setProperty("SignalLength", "15");
    m_adderFilterAlgTool->setProperty("FilterLength", "5");
    m_adderFilterAlgTool->setProperty("FrameLength", "16");
    m_adderFilterAlgTool->setProperty("DeltaCutLo", "9.5");
    m_adderFilterAlgTool->setProperty("DeltaCutHi", "9.5");
    m_adderFilterAlgTool->setProperty("RMSCutLo", "1.0");
    m_adderFilterAlgTool->setProperty("RMSCutHi", "1.0");

  }

  ATH_MSG_INFO( "calibMode " << m_calibMode );
  ATH_MSG_INFO( "calibrateEnergy " << m_calibrateEnergy );
  ATH_MSG_INFO( "number of samples " << m_nSamples );


  msg(MSG::INFO) << "drawerList " << MSG::hex;
  unsigned int size = m_drawerList.size();
  for (unsigned int dr = 0; dr < size; ++dr) {
    int frag = strtol(m_drawerList[dr].data(), NULL, 0);
    if (frag >= 0) {
      m_drawerMap[frag] = dr;
      if (dr == m_drawerType.size()) m_drawerType.push_back(frag >> 8);
      msg(MSG::INFO) << " 0x" << frag;
    } else {
      msg(MSG::INFO) << " " << m_drawerList[dr];
      // put negative number in first element (flag to read frag ID from data)
      m_drawerList[0] = m_drawerList[dr];
      m_drawerList.resize(1);
      // m_drawerType.clear();
      m_drawerMap.clear();
      size = 0;
      break;
    }
  }

  if (size == 0) {
    if (m_drawerList.size() > 0)
      msg(MSG::INFO) << " - negative number, will read frag IDs from the data" << MSG::dec << endmsg;
    else
      msg(MSG::INFO) << "is empty, no drawer fragments in ntuple" << MSG::dec << endmsg;
  } else {
    msg(MSG::INFO) << MSG::dec << endmsg;

    size = m_drawerType.size();
    if (size < m_nDrawers) {
      m_drawerType.resize(m_nDrawers);
      for (; size < m_nDrawers; ++size)
        m_drawerType[size] = 0;
    }

    msg(MSG::INFO) << "drawerType ";
    for (unsigned int dr = 0; dr < size; ++dr)
      msg(MSG::INFO) << " " << m_drawerType[dr];
    msg(MSG::INFO) << endmsg;

    if (size > m_nDrawers) {
      ATH_MSG_INFO( "increasing m_nDrawers from " << m_nDrawers << " to " << size );
      m_nDrawers = size;
    }

  }

  msg(MSG::INFO) << "Beam Frag List " << MSG::hex;
  size = m_beamFragList.size();
  for (unsigned int dr = 0; dr < size; ++dr) {
    int frag = strtol(m_beamFragList[dr].data(), NULL, 0);
    if (frag >= 0) {
      m_beamIdList[frag & 0x1F] = true;
      msg(MSG::INFO) << " 0x" << frag;
    }
  }

  if (size == 0) {
    msg(MSG::INFO) << "is empty, no beam fragments in ntuple" << MSG::dec << endmsg;
  } else {
    msg(MSG::INFO) << MSG::dec << endmsg;
  }

  // set event number to 0 before first event
  m_evtNr = 0;
  m_currentFileNum = 0;

  m_calibrateEnergyThisEvent = m_calibrateEnergy;

  if (connectFile().isFailure()) {
    ATH_MSG_ERROR( " Can't open file for Ntuple " );
  }

  ATH_MSG_INFO( "initialization completed" );

  return StatusCode::SUCCESS;
}

StatusCode TileTBNtuple::execute() {

  if (m_evtNr % 1000 == 0)
    ATH_MSG_INFO( m_evtNr << " events processed so far" );

  if (m_eventsPerFile > 0 && m_evtNr % m_eventsPerFile == 0 && m_evtNr > 0) {
    ATH_MSG_INFO( "Booking ntuple in new file" );

    if (connectFile().isFailure()) {
      ATH_MSG_ERROR( " Can't open file for Ntuple " );
    } else if (!m_ntupleCreated) {
      if (initNTuple().isFailure()) {
        ATH_MSG_ERROR( " Error during ntuple initialization" );
      }
    }
  }

  /// find event and beam ROD header, calib mode for digitizers

  /// if first event: determine mode (normal or calib)
  /// by examining the first TileDigitCollection
  /// initialize NTuple accordingly
  if (m_evtNr == 0) {

    if (m_bsInput) {
      ServiceHandle<IConversionSvc> cnvSvc("ByteStreamCnvSvc", "");
      if (cnvSvc.retrieve().isFailure()) {
        ATH_MSG_ERROR( " Can't get ByteStreamCnvSvc " );
        m_beamCnv = NULL;
      } else {

        m_beamCnv = dynamic_cast<TileBeamElemContByteStreamCnv *>(cnvSvc->converter(ClassID_traits<TileBeamElemContainer>::ID()));

        if (m_beamCnv == NULL) {
          ATH_MSG_ERROR( " Can't get TileBeamElemContByteStreamCnv " );
        }
      }
    } else {
      m_beamCnv = NULL;
    }

    if (!m_ntupleCreated) {

      bool calibMode = (m_beamInfo->calibMode() == 1);
      if (calibMode != m_calibMode) {
        ATH_MSG_INFO( "Calib mode from data is " );
        ATH_MSG_INFO( "  Overwriting calib mode " );
        m_calibMode = calibMode;
      }

      if (initList().isFailure()) {
        ATH_MSG_ERROR( " Error during drawer list initialization" );
      }

      if (initNTuple().isFailure()) {
        ATH_MSG_ERROR( " Error during ntuple initialization" );
      }
    }

    if (m_unpackAdder) // in 2003 event number starts from 1
      ++m_evtNr;
  }

  // store BeamElements
  bool empty = storeBeamElements().isFailure();

  if (m_drawerMap.size() > 0) {

    // store TileDigits
    if (m_nSamples > 0) {
      empty &= (storeDigits().isFailure());
    }

    // store TileRawChannels
    empty &= (storeRawChannels(m_fitRawChannelContainer, &m_efitVec, &m_tfitVec, &m_chi2Vec, &m_pedfitVec).isFailure());
    empty &= (storeRawChannels(m_flatRawChannelContainer, &m_eneVec, &m_timeVec, 0, 0).isFailure());
    empty &= (storeRawChannels(m_optRawChannelContainer, &m_eOptVec, &m_tOptVec, &m_chi2OptVec, &m_pedOptVec).isFailure());
    empty &= (storeRawChannels(m_dspRawChannelContainer, &m_eDspVec, &m_tDspVec, &m_chi2DspVec, 0).isFailure());
  }

  if (m_completeNtuple) {
    // store energy per sampling from all calorimeters
    empty &= (storeCells().isFailure());
  }

  if (m_beamCnv) {
    m_evTime = m_beamCnv->eventFragment()->bc_time_seconds();
    m_evt = m_beamCnv->eventFragment()->global_id();
    // m_runNo = m_beamCnv->eventFragment()->run_no();  // run_no is 0 here
    if (m_beamCnv->validBeamFrag())
      m_run = m_beamCnv->robFragment()->rod_run_no();   // take it from beam ROD header
    else
      m_run = 0;
  } else {
    m_evTime = 0;
    m_evt = m_evtNr;
    m_run = 0;
  }

  if (m_commitNtuple) {
    /// Commit ntuple if collections are nonempty
    /// or we share ntuple with somebody else and have to commit in any case
    if (!empty || m_ntupleCreated) {
      ATH_MSG_DEBUG( "Commiting Ntuple" );
      CHECK( ntupleSvc()->writeRecord(m_ntuplePtr) );
    } else
      ATH_MSG_DEBUG( "No data stored for event " << m_evtNr << ", collections are empty" );
  }
  /// increase event nr
  /// this number can be different from real event number if we skip events
  ++m_evtNr;

  // Execution completed.
  ATH_MSG_DEBUG( "execute() completed successfully" );

  return StatusCode::SUCCESS;
}

StatusCode TileTBNtuple::storeBeamElements() {


  if (m_beamElemContainer.size() == 0) return StatusCode::SUCCESS;

  const TileBeamElemContainer* BeamElemCnt;
  CHECK( evtStore()->retrieve(BeamElemCnt, m_beamElemContainer) );

  TileBeamElemContainer::const_iterator collItr = BeamElemCnt->begin();
  TileBeamElemContainer::const_iterator lastColl = BeamElemCnt->end();

  if (m_completeNtuple) {
    // Store ROD header info from collection (just from first one)
    if (collItr != lastColl) {
      m_l1ID[m_nDrawers] = (*collItr)->getLvl1Id();
      m_l1Type[m_nDrawers] = (*collItr)->getLvl1Type();
      m_evType[m_nDrawers] = (*collItr)->getDetEvType();
      m_evBCID[m_nDrawers] = (*collItr)->getRODBCID();
    } else {
      m_l1ID[m_nDrawers] = 0xFFFFFFFF;
      m_l1Type[m_nDrawers] = 0xFFFFFFFF;
      m_evType[m_nDrawers] = 0xFFFFFFFF;
      m_evBCID[m_nDrawers] = 0xFFFFFFFF;
    }
  }

  m_trigType = 0;

  for (; collItr != lastColl; ++collItr) {

    TileBeamElemCollection::const_iterator beamItr = (*collItr)->begin();
    TileBeamElemCollection::const_iterator lastBeam = (*collItr)->end();

    if (msgLvl(MSG::VERBOSE)) {

      for (; beamItr != lastBeam; ++beamItr) {
        HWIdentifier id = (*beamItr)->adc_HWID();
        std::vector<uint32_t> digits = (*beamItr)->get_digits();
        msg(MSG::VERBOSE) << " --- TileBeamElem -- Identifier " << m_tileHWID->to_string(id) << " "
                          << (*beamItr)->get_digits().size() << " " << digits.size() << endmsg;

        msg(MSG::VERBOSE) << MSG::VERBOSE << " --- TileBeamElem -- BeamElem : " << MSG::dec;
        for (unsigned int k = 0; k < digits.size(); k++)
          msg(MSG::VERBOSE) << digits[k] << " ";
        msg(MSG::VERBOSE) << endmsg;
      }

      //restore iterator
      beamItr = (*collItr)->begin();
    }
  
    int frag = (*collItr)->identify();
    ATH_MSG_DEBUG( " frag: " << frag );
    ATH_MSG_DEBUG( " trigType " << (*collItr)->getLvl1Type() );
    
    if ( m_trigType == 0 && (*collItr)->getLvl1Type() != 0 ) // take it from the ROD header
      m_trigType = (*collItr)->getLvl1Type();

    // unpack only fragments which we want to store in ntuple
    if (m_beamIdList[frag & 0x1F]) {

      for (; beamItr != lastBeam; ++beamItr) {

        HWIdentifier id = (*beamItr)->adc_HWID();
        std::vector<uint32_t> digits = (*beamItr)->get_digits();
        int cha = m_tileHWID->channel(id);
        int dsize = digits.size();

        if (dsize <= 0) {

          WRONG_SAMPLE(frag, cha, dsize);

        } else if (dsize != 16 && frag == ADD_FADC_FRAG) {

          WRONG_SAMPLE(frag, cha, dsize);

        } else if (dsize != 1 && frag != ADD_FADC_FRAG
                    && frag != BEAM_TDC_FRAG && frag != COMMON_TDC1_FRAG
                    && frag != COMMON_TOF_FRAG && frag != COMMON_TDC2_FRAG) {

          WRONG_SAMPLE(frag, cha, dsize);

        } else {

          int amplitude = digits[0];
          SIGNAL_FOUND(frag, cha, amplitude);

          switch (frag) {

            case BEAM_TDC_FRAG:

              FRAG_FOUND(frag, cha, dsize);
              if (cha < 8) m_btdc1[cha] = amplitude;
              else if (cha < 16) m_btdc2[cha - 8] = amplitude;
              else  WRONG_CHANNEL(frag, cha);
              break;

            case BEAM_ADC_FRAG:

              if (m_unpackAdder) {
                switch (cha) {
                  // BEAM
                  case 0:
                    m_s1cou = amplitude;
                    break;
                  case 1:
                    m_s2cou = amplitude;
                    break;
                  case 2:
                    m_s3cou = amplitude;
                    break;
                  case 3:
                    m_cher1 = amplitude;
                    break; // swap of Cher1
                  case 4:
                    m_muTag = amplitude;
                    break; // and S4 in 2003 data
                  case 5:
                    m_cher2 = amplitude;
                    break;
                  case 6:
                    m_muHalo = amplitude;
                    break;
                  case 7:
                    m_muVeto = amplitude;
                    break;
                    // LASER
                  case 8:
                    m_las0 = amplitude;
                    break;
                  case 9:
                    m_las1 = amplitude;
                    break;
                  case 10:
                    m_las2 = amplitude;
                    break;
                  case 11:
                    m_las3 = amplitude;
                    break;
                  case 12:
                    m_lasExtra[0] = amplitude;
                    break;
                  case 13:
                    m_lasExtra[1] = amplitude;
                    break;
                  case 14:
                    m_lasExtra[2] = amplitude;
                    break;
                  case 15:
                    m_lasExtra[3] = amplitude;
                    break;
                  default:
                    WRONG_CHANNEL(frag, cha);
                }
              } else { // 2004 data
                switch (cha) {
                  // BEAM
                  case 0:
                    m_sc1 = amplitude;
                    break;
                  case 1:
                    m_sc2 = amplitude;
                    break;
                  case 2:
                  case 3:
                  case 4:
                  case 5:
                  case 6:
                  case 7:
                    break;
                  default:
                    WRONG_CHANNEL(frag, cha);
                }
              }
              break;

            case MUON_ADC_FRAG:

              // first 14 are m_muBack
              if (cha < 14) m_muBack[cha] = amplitude;
              // last 2 are m_muCalib
              else if (cha < 16) m_muCalib[cha - 14] = amplitude;
              else  WRONG_CHANNEL(frag, cha);
              break;

            case ADDR_ADC_FRAG:

              // second half of Muon Wall in 2004
              if (cha < 6) m_muBack[cha + 8] = amplitude;
              // last 2 are m_muCalib
              else if (cha < 8) m_muCalib[cha - 6] = amplitude;
              else WRONG_CHANNEL(frag, cha);
              break;

            case LASE_PTN_FRAG:

              if (cha == 0) {
                // laser pattern unit
                m_lasFlag = amplitude;
                if (amplitude & 0xFF00) m_trigType = amplitude >> 8;
              } else {
                WRONG_CHANNEL(frag, cha);
              }
              break;

            case LASE_ADC_FRAG:

              // laser in 2004
              switch (cha) {
                case 0:
                  m_las0 = amplitude;
                  break;
                case 1:
                  m_las1 = amplitude;
                  break;
                case 2:
                  m_las2 = amplitude;
                  break;
                case 3:
                  m_las3 = amplitude;
                  break;
                case 4:
                  m_lasExtra[0] = amplitude;
                  break;
                case 5:
                  m_lasExtra[1] = amplitude;
                  break;
                case 6:
                  m_lasExtra[2] = amplitude;
                  break;
                case 7:
                  m_lasExtra[3] = amplitude;
                  break;
                default:
                  WRONG_CHANNEL(frag, cha);
              }
              break;

            case ADD_FADC_FRAG:

              if (m_unpackAdder) {

                for (int k = 0; k < dsize; k++) {
                  //m_addx[k]=k;
                  m_adder[cha][k] = digits[k];
                }
                // FlatFiler adders
                double ene, tim;
                m_adderFilterAlgTool->flatFilter(digits, 0, ene, tim);
                m_eneAdd[cha] = ene;
                m_timeAdd[cha] = tim;
              }
              break;

            case ECAL_ADC_FRAG:

              if (cha < 8)
                m_ecal[cha] = amplitude;
              else
                WRONG_CHANNEL(frag, cha);
              break;

            case DIGI_PAR_FRAG:

              if (cha < 16)
                m_cispar[cha] = amplitude;
              else
                WRONG_CHANNEL(frag, cha);
              break;

            case COMMON_ADC1_FRAG:

              switch (cha) {
                // BEAM
                case 0:
                  m_s1cou = amplitude;
                  break;
                case 1:
                  m_s2cou = amplitude;
                  break;
                case 2:
                  m_s3cou = amplitude;
                  break;
                case 3:
                  m_muTag = amplitude;
                  break;
                case 4:
                  m_cher1 = amplitude;
                  break;
                case 5:
                  m_cher2 = amplitude;
                  break;
                case 6:
                  m_muHalo = amplitude;
                  break;
                case 7:
                  m_muVeto = amplitude;
                  break;
                default:
                  WRONG_CHANNEL(frag, cha);
              }
              break;

            case COMMON_ADC2_FRAG:

              if (!m_unpackAdder) {
                switch (cha) {
                  // BEAM
                  case 0:
                    break;
                  case 1:
                    m_s2extra = amplitude;
                    break;
                  case 2:
                    m_s3extra = amplitude;
                    break;
                  case 3:
                  case 4:
                  case 5:
                  case 6:
                  case 7:
                    break;
                  default:
                    WRONG_CHANNEL(frag, cha);
                }
              }
              break;

            case COMMON_PTN_FRAG:

              if (cha == 0)
                m_commonPU = amplitude;
              else
                WRONG_CHANNEL(frag, cha);
              break;

            case COMMON_TOF_FRAG:

              if (cha < 8)
                m_tof[cha] = amplitude;
              else
                WRONG_CHANNEL(frag, cha);
              break;

            case COMMON_TDC1_FRAG:

              FRAG_FOUND(frag, cha, dsize);
              if (cha < 16) m_btdc1[cha] = amplitude;
              else WRONG_CHANNEL(frag, cha);
              break;

            case COMMON_TDC2_FRAG:

              FRAG_FOUND(frag, cha, dsize);
              if (cha < 16) m_btdc2[cha] = amplitude;
              else WRONG_CHANNEL(frag, cha);
              break;

            case COIN_TRIG1_FRAG:

              if (cha < 3) {
                int idx = cha * 32;
                for (int ibit = 0; ibit < 32; ++ibit) {
                  m_coincTrig1[idx++] = (amplitude >> ibit) & 1;
                }
              } else if (cha == 3) {
                m_coincFlag1 = amplitude;
              } else
                WRONG_CHANNEL(frag, cha);
              break;

            case COIN_TRIG2_FRAG:

              if (cha < 3) {
                int idx = cha * 32;
                for (int ibit = 0; ibit < 32; ++ibit) {
                  m_coincTrig2[idx++] = (amplitude >> ibit) & 1;
                }
              } else if (cha == 3) {
                m_coincFlag2 = amplitude;
              } else
                WRONG_CHANNEL(frag, cha);
              break;

            case COIN_TRIG3_FRAG:

              if (cha < 3) {
                int idx = cha * 32;
                for (int ibit = 0; ibit < 32; ++ibit) {
                  m_coincTrig3[idx++] = (amplitude >> ibit) & 1;
                }
              } else if (cha == 3) {
                m_coincFlag3 = amplitude;
              } else
                WRONG_CHANNEL(frag, cha);
              break;

            case COIN_TRIG4_FRAG:

              if (cha < 3) {
                int idx = cha * 32;
                for (int ibit = 0; ibit < 32; ++ibit) {
                  m_coincTrig4[idx++] = (amplitude >> ibit) & 1;
                }
              } else if (cha == 3) {
                m_coincFlag4 = amplitude;
              } else
                WRONG_CHANNEL(frag, cha);
              break;

            case COIN_TRIG5_FRAG:

              if (cha < 3) {
                int idx = cha * 32;
                for (int ibit = 0; ibit < 32; ++ibit) {
                  m_coincTrig5[idx++] = (amplitude >> ibit) & 1;
                }
              } else if (cha == 3) {
                m_coincFlag5 = amplitude;
              } else
                WRONG_CHANNEL(frag, cha);
              break;

            case COIN_TRIG6_FRAG:

              if (cha < 3) {
                int idx = cha * 32;
                for (int ibit = 0; ibit < 32; ++ibit) {
                  m_coincTrig6[idx++] = (amplitude >> ibit) & 1;
                }
              } else if (cha == 3) {
                m_coincFlag6 = amplitude;
              } else
                WRONG_CHANNEL(frag, cha);
              break;

            case COIN_TRIG7_FRAG:

              if (cha < 3) {
                int idx = cha * 32;
                for (int ibit = 0; ibit < 32; ++ibit) {
                  m_coincTrig7[idx++] = (amplitude >> ibit) & 1;
                }
              } else if (cha == 3) {
                m_coincFlag7 = amplitude;
              } else
                WRONG_CHANNEL(frag, cha);
              break;

            case COIN_TRIG8_FRAG:

              if (cha < 3) {
                int idx = cha * 32;
                for (int ibit = 0; ibit < 32; ++ibit) {
                  m_coincTrig8[idx++] = (amplitude >> ibit) & 1;
                }
              } else if (cha == 3) {
                m_coincFlag8 = amplitude;
              } else
                WRONG_CHANNEL(frag, cha);
              break;

            default:
              break;
          }
        }
      }
    }
  }

  // calculate beam coords in Beam Chambers
  if (m_unpackAdder) { // this is 2003 data

    if (m_beamIdList[BEAM_TDC_FRAG]) {
      m_xCha1 = m_beamBC1X1 + m_beamBC1X2 * (m_btdc1[6] - m_btdc1[7]); // last two channels of TDC !!!
      m_yCha1 = m_beamBC1Y1 + m_beamBC1Y2 * (m_btdc1[2] - m_btdc1[3]);
      m_xCha2 = m_beamBC2X1 + m_beamBC2X2 * (m_btdc2[0] - m_btdc2[1]);
      m_yCha2 = m_beamBC2Y1 + m_beamBC2Y2 * (m_btdc2[2] - m_btdc2[3]);

      m_xImp = m_xCha2 + (m_xCha2 - m_xCha1) * m_beamBC2Z / (m_beamBC1Z - m_beamBC2Z);
      m_yImp = m_yCha2 + (m_yCha2 - m_yCha1) * m_beamBC2Z / (m_beamBC1Z - m_beamBC2Z);
    }

  } else { // this is 2004 data

    if (m_beamIdList[COMMON_TDC1_FRAG]) {
      m_yChN2 = m_beamBN2Y1 + m_beamBN2Y2 * (m_btdc1[0] - m_btdc1[1]);
      m_xChN2 = m_beamBN2X1 + m_beamBN2X2 * (m_btdc1[2] - m_btdc1[3]);
      m_yChN1 = m_beamBN1Y1 + m_beamBN1Y2 * (m_btdc1[4] - m_btdc1[5]);
      m_xChN1 = m_beamBN1X1 + m_beamBN1X2 * (m_btdc1[6] - m_btdc1[7]);

      m_yCha0 = m_beamBC0Y1 + m_beamBC0Y2 * (m_btdc1[8] - m_btdc1[9]);
      m_xCha0 = m_beamBC0X1 + m_beamBC0X2 * (m_btdc1[10] - m_btdc1[11]);

      if (msgLvl(MSG::DEBUG)) {
        msg(MSG::DEBUG) << "BC-2x : ( " << m_btdc1[2] << " - " << m_btdc1[3] << " )\t" << static_cast<float>(m_xChN2) << endmsg;
        msg(MSG::DEBUG) << "BC-2y : ( " << m_btdc1[0] << " - " << m_btdc1[1] << " )\t" << static_cast<float>(m_yChN2) << endmsg;
        msg(MSG::DEBUG) << "BC-1x : ( " << m_btdc1[6] << " - " << m_btdc1[7] << " )\t" << static_cast<float>(m_xChN1) << endmsg;
        msg(MSG::DEBUG) << "BC-1y : ( " << m_btdc1[4] << " - " << m_btdc1[5] << " )\t" << static_cast<float>(m_yChN1) << endmsg;
        msg(MSG::DEBUG) << "BC0x  : ( " << m_btdc1[10] << " - " << m_btdc1[11] << " )\t" << static_cast<float>(m_xCha0) << endmsg;
        msg(MSG::DEBUG) << "BC0y  : ( " << m_btdc1[8] << " - " << m_btdc1[9] << " )\t" << static_cast<float>(m_yCha0) << endmsg;
      }

    }
    
    if (m_beamIdList[COMMON_TDC2_FRAG]) {
      m_yCha1 = m_beamBC1Y1 + m_beamBC1Y2 * (m_btdc2[0] - m_btdc2[1]);
      m_xCha1 = m_beamBC1X1 + m_beamBC1X2 * (m_btdc2[2] - m_btdc2[3]);
      m_yCha2 = m_beamBC2Y1 + m_beamBC2Y2 * (m_btdc2[4] - m_btdc2[5]);
      m_xCha2 = m_beamBC2X1 + m_beamBC2X2 * (m_btdc2[6] - m_btdc2[7]);

      m_xImp = 0.0;
      m_yImp = 0.0;
      if (msgLvl(MSG::DEBUG)) {
        msg(MSG::DEBUG) << "BC1x  : ( " << (int) m_btdc2[2] << " - " << (int) m_btdc2[3] << " )\t" << (float) m_xCha1 << endmsg;
        msg(MSG::DEBUG) << "BC1y  : ( " << (int) m_btdc2[0] << " - " << (int) m_btdc2[1] << " )\t" << (float) m_yCha1 << endmsg;
        msg(MSG::DEBUG) << "BC2x  : ( " << (int) m_btdc2[6] << " - " << (int) m_btdc2[7] << " )\t" << (float) m_xCha2 << endmsg;
        msg(MSG::DEBUG) << "BC2y  : ( " << (int) m_btdc2[4] << " - " << (int) m_btdc2[5] << " )\t" << (float) m_yCha2 << endmsg;
      }
    }
    
    if ( m_beamIdList[COMMON_TDC1_FRAG] && m_beamIdList[COMMON_TDC2_FRAG] ) {

      ////////////////////////////////////////////////
      // Get run number and eta  
      if ( 0==m_runNumber ){
        const xAOD::EventInfo* eventInfo(0);
        if (evtStore()->retrieve(eventInfo).isFailure()){
          ATH_MSG_ERROR( "No EventInfo object found! Can't read run number!"  );
          m_runNumber = -1;
        } else {
          m_runNumber = eventInfo->runNumber();
          getEta();
        }  
      } 
      ////////////////////////////////////////////////

      // Computation of X,Y imp on TileCal/LAr front
      float tanBx = (m_xCha1 - m_xCha0) / (m_beamBC0Z - m_beamBC1Z);
      float tanBy = (m_yCha1 - m_yCha0) / (m_beamBC0Z - m_beamBC1Z);

      float Ximp = m_xCha1 + m_beamBC1Z * tanBx;
      float Yimp = m_yCha1 + m_beamBC1Z * tanBy;

      if (0.0 != cos(m_theta) * (1 + tanBx * tan(m_theta))) {
        m_xImp = (Ximp + m_radius * (tanBx * (cos(m_theta) - 1) - sin(m_theta))) / (cos(m_theta) * (1 + tanBx * tan(m_theta)));
        m_yImp = Yimp + tanBy * (m_radius * (1 - cos(m_theta)) - Ximp * sin(m_theta)) / (cos(m_theta) * (1 + tanBx * tan(m_theta)));
      }
    }
  }


  // do not apply Cesium and Laser calibration for CIS events
  m_calibrateEnergyThisEvent = m_calibrateEnergy && (m_trigType != 8);

  return StatusCode::SUCCESS;
}

/**
 * Fill ntuple with data from TRC.
 * Default TRC container contains flat filtered and
 * named container fitted/opt filtered
 */
StatusCode TileTBNtuple::storeRawChannels(std::string containerId
                                           , std::vector<NTuple::Array<float>*> *eneVec
                                           , std::vector<NTuple::Array<float>*> *timeVec
                                           , std::vector<NTuple::Array<float>*> *chi2Vec
                                           , std::vector<NTuple::Array<float>*> *pedVec) {


  if (containerId.size() == 0) // empty name, nothing to do
    return StatusCode::FAILURE;

  // get named container
  const TileRawChannelContainer* rcCnt;
  CHECK( evtStore()->retrieve(rcCnt, containerId) );

  TileRawChannelUnit::UNIT rChUnit = rcCnt->get_unit();

  // Get iterator for all TRCColl in TRCCont
  TileRawChannelContainer::const_iterator itColl = (*rcCnt).begin(); 
  TileRawChannelContainer::const_iterator itCollEnd = (*rcCnt).end(); 

  TileRawChannelCollection::const_iterator it, itEnd;

  // type is 0 - m_nDrawers-1, fragType is 1-4 B+/B-/EB+/EB-
  int type, fragType;
  
  // Go through all TileRawChannelCollections
  for (; itColl != itCollEnd; ++itColl) {
    // determine type
    int fragId = (*itColl)->identify();
    int drawerIdx = TileCalibUtils::getDrawerIdxFromFragId(fragId);
    drawerMap_iterator itr = m_drawerMap.find(fragId);
    if (itr != m_drawerMap.end()) {
      type = (*itr).second;
    } else {
      type = -1;
    }

    if (type < 0) {
      if ((*itColl)->begin() != (*itColl)->end())
        ATH_MSG_WARNING( "frag id 0x" << MSG::hex << fragId << MSG::dec << " was not found among valid frag IDs when storing TRC!"  );

    } else {
      fragType = m_drawerType[type];
      ATH_MSG_DEBUG( "TRC (" << containerId
                     << ") Event# " << m_evtNr
                     << " Frag id 0x" << MSG::hex << fragId << MSG::dec
                     << " index " << type );

      // go through all TileRawChannels in collection
      it = (*itColl)->begin();
      itEnd = (*itColl)->end();

      for (; it != itEnd; ++it) {
        HWIdentifier hwid = (*it)->adc_HWID();
        int index = type;

        // determine channel and gain
        int channel = m_tileHWID->channel(hwid);
        int gain = m_tileHWID->adc(hwid);

        if (m_calibMode) {
          // gain, if hi add m_nDrawers to index
          if (gain == 1) index += m_nDrawers;
        }

        /// Cesium and Laser calibration
        /// we assume that CIS calibration was done already in Opt. filtering
        /// and we have already energy in pCb
        /// after cesium calibration we still have the same pCb for A and BC cells
        /// what we do is just set the same scale for D-cells
        double energy = (*it)->amplitude();
        if (m_calibrateEnergyThisEvent) { // convert to GeV and not MeV !
          energy = m_tileToolEmscale->channelCalib(drawerIdx, channel, gain, energy, rChUnit, TileRawChannelUnit::MegaElectronVolts) / 1000.;
        }

        // cabling for testbeam (convert to pmt#-1)
        if (fragType > 0) channel = digiChannel2PMT(fragType, channel);

        (*(*eneVec)[index])[channel] = energy;
        (*(*timeVec)[index])[channel] = (*it)->time();
        if (chi2Vec != 0) (*(*chi2Vec)[index])[channel] = (*it)->quality();
        if (pedVec != 0) (*(*pedVec)[index])[channel] = (*it)->pedestal();

        ATH_MSG_DEBUG( "TRC ene=" << energy
                      << " time=" << (*it)->time()
                      << " chi2=" << (*it)->quality()
                      << " ped=" << (*it)->pedestal()
                      << " pmt-1=" << channel
                      << " index " << index );

      }
    }
  }

  return StatusCode::SUCCESS;
}

/**
 * Fill Ntuple with info from TileDigits
 * Return true if the collection is empty, 
 * which means that there are no RawChanels either.
 */
StatusCode TileTBNtuple::storeDigits() {

  bool emptyColl = true;

  // Read Digits from TDS
  const TileDigitsContainer *digitsCnt;
  CHECK( evtStore()->retrieve(digitsCnt, m_digitsContainer) );

  // Get iterator for all TDColl in TDCont
  TileDigitsContainer::const_iterator itColl = (*digitsCnt).begin();
  TileDigitsContainer::const_iterator itCollEnd = (*digitsCnt).end();

  TileDigitsCollection::const_iterator it, itEnd;

  // type is 0 - m_nDrawers-1, fragType is 1-4 B+/B-/EB+/EB-
  int n, type, fragType, channel;

  std::vector<float> sampleVec;
  std::vector<uint32_t> headerVec;
  std::vector<uint32_t> headerVecHi;
  uint32_t CRCmask;
  uint32_t fe_crc;
  uint32_t rod_crc;

  // Go through all TileDigitsCollections
  for (; itColl != itCollEnd; itColl++) {
    // determine type of frag
    int fragId = (*itColl)->identify();
    drawerMap_iterator itr = m_drawerMap.find(fragId);
    if (itr != m_drawerMap.end()) {
      type = (*itr).second;
    } else {
      type = -1;
    }

    if (type < 0) {
      if ((*itColl)->begin() != (*itColl)->end())
        ATH_MSG_WARNING( "frag id 0x" << MSG::hex << fragId << MSG::dec << " was not found among valid frag IDs when storing TRC!" );
    } else {
      fragType = m_drawerType[type];
      if (msgLvl(MSG::DEBUG)) {

        msg(MSG::DEBUG) << "Event# " << m_evtNr
                        << " Frag id 0x"<< MSG::hex << fragId <<MSG::dec
                        << " index " << type
                        << " Calib " << m_calibMode << endmsg;

        msg(MSG::DEBUG) << "       Size=" << (*itColl)->getFragSize()
                        << " BCID=" << (*itColl)->getFragBCID() << MSG::hex
                        << " CRC=0x" << ((*itColl)->getFragCRC() & 0xffff)
                        << " DMUMask=0x" << ((*itColl)->getFragDMUMask() & 0xffff) << MSG::dec << endmsg;

        msg(MSG::DEBUG) << "       Lvl1ID=" << (*itColl)->getLvl1Id()
                        << " Lvl1Type=" << (*itColl)->getLvl1Type()
                        << " EvBCID=" << (*itColl)->getRODBCID()
                        << " EvType=" << (*itColl)->getDetEvType() << endmsg;

        msg(MSG::DEBUG) << "       Header=" << (*itColl)->getFragChipHeaderWords() << endmsg;
      }

      if (m_completeNtuple) {
        /// Store ROD header info from collection 
        /// (should be just one per ROD,
        /// but we don't know how many RODs we have,
        /// so store it for every collection)
        m_l1ID[type] = (*itColl)->getLvl1Id();
        m_l1Type[type] = (*itColl)->getLvl1Type();
        m_evType[type] = (*itColl)->getDetEvType();
        m_evBCID[type] = (*itColl)->getRODBCID();
        // store FrBCID
        m_frBCID[type] = (*itColl)->getFragBCID();
      }
      
      if(m_calibMode) {
        // Digits in calib mode
        // check gain for first digits in collection
        
        it = (*itColl)->begin();
        itEnd = (*itColl)->end();
 
        // non empty collection
        if(it != itEnd) {
          // store evtnr, bcid,crc, size
          // Same for lo and hi, because they come from the same fragment
          *m_rodBCIDVec[type] = (*itColl)->getRODBCID();
          *m_sizeVec[type] = (*itColl)->getFragSize();
          *m_sizeVec[type+m_nDrawers] = (*itColl)->getFragSize();
          *m_evtVec[type] = m_evtNr;
          *m_evtVec[type+m_nDrawers] = m_evtNr;

          headerVec = (*itColl)->getFragChipHeaderWords();
          headerVecHi = (*itColl)->getFragChipHeaderWordsHigh();
          CRCmask = (*itColl)->getFragDMUMask(); //mask of FE+ROD DMU crc check (16bit+16bit) 0xffffffff == All ok
          fe_crc = CRCmask & 0xFFFF;
          rod_crc = CRCmask >> 16;

          unsigned int headsize = std::min(16U, static_cast<unsigned int>(headerVec.size()));
          unsigned int headsizehi = std::min(16U, static_cast<unsigned int>(headerVecHi.size()));

          for (unsigned int ih = 0; ih < headsize; ++ih) {
            (*m_bcidVec[type])[ih] = (headerVec[ih] & 0xFFF);
            (*m_feCRCVec[type])[ih] = (fe_crc >> ih & 0x1);
            (*m_rodCRCVec[type])[ih] = (rod_crc >> ih & 0x1);
          }

          for (unsigned int ihhi = 0; ihhi < headsizehi; ++ihhi) {
            (*m_bcidVec[type + m_nDrawers])[ihhi] = (headerVecHi[ihhi] & 0xFFF);
          }

          (*m_slinkCRCVec[type])[0] = ((*itColl)->getFragCRC() >> 16) & 0xffff;
          (*m_dmuMaskVec[type])[0] = ((*itColl)->getFragDMUMask() >> 16) & 0xffff;
          (*m_slinkCRCVec[type])[1] = (*itColl)->getFragCRC() & 0xffff;
          (*m_dmuMaskVec[type])[1] = (*itColl)->getFragDMUMask() & 0xffff;

          (*m_slinkCRCVec[type + m_nDrawers])[0] = ((*itColl)->getFragCRC() >> 16) & 0xffff;
          (*m_dmuMaskVec[type + m_nDrawers])[0] = ((*itColl)->getFragDMUMask() >> 16) & 0xffff;
          (*m_slinkCRCVec[type + m_nDrawers])[1] = (*itColl)->getFragCRC() & 0xffff;
          (*m_dmuMaskVec[type + m_nDrawers])[1] = (*itColl)->getFragDMUMask() & 0xffff;
      
          // go through all TileDigits in collection
          for (; it != itEnd; it++) {
            emptyColl = false;

            HWIdentifier hwid = (*it)->adc_HWID();
            // determine gain
            int gain = m_tileHWID->adc(hwid);
            // add m_nDrawers to index if hi gain
            int index = type;
            if (gain == 1) index += m_nDrawers;

            // determine channel       
            channel = m_tileHWID->channel(hwid);
            // cabling for testbeam (convert to pmt#-1)
            if (fragType > 0) channel = digiChannel2PMT(fragType, channel);

            // gain determined for all digits in collection
            (*m_gainVec[index])[channel] = gain;
            ATH_MSG_DEBUG( "Storing TD for channel: " << channel
                           << " with gain " << m_tileHWID->adc(hwid)
                           << " index " << index );

            // get digits
            sampleVec = (*it)->samples();
            int siz = sampleVec.size();

            if (msgLvl(MSG::DEBUG)) {
              msg(MSG::DEBUG) << "Digits." << siz << " {";
              for (int i = 0; i < siz; i++) {
                msg(MSG::DEBUG) << (int) sampleVec[i] << " ";
              }

              if (siz > m_nSamples) {
                msg(MSG::DEBUG) << "} ONLY " << m_nSamples << " digits saved to ntuple" << endmsg;
              } else {
                msg(MSG::DEBUG) << "}" << endmsg;
              }
            }

            if (siz > m_nSamples) siz = m_nSamples;
            for (n = 0; n < siz; n++) {
              (*m_sampleVec[index])[channel][n] = (int) sampleVec[n];
            }
          }
        }
      } else {
        // Digits in normal mode

        // store evtnr, bcid,crc, size
        *m_rodBCIDVec[type] = (*itColl)->getRODBCID();
        *m_sizeVec[type] = (*itColl)->getFragSize();
        *m_evtVec[type] = m_evtNr;

        headerVec = (*itColl)->getFragChipHeaderWords();
        CRCmask = (*itColl)->getFragDMUMask(); //mask of FE+ROD DMU crc check (16bit+16bit) 0xffffffff == All ok
        fe_crc = CRCmask & 0xFFFF;
        rod_crc = CRCmask >> 16;

        int headsize = headerVec.size();

        for (int ih = 0; ih < headsize; ih++) {
          (*m_bcidVec[type])[ih] = (headerVec[ih] & 0xFFF);
          (*m_feCRCVec[type])[ih] = (fe_crc >> ih & 0x1);
          (*m_rodCRCVec[type])[ih] = (rod_crc >> ih & 0x1);
        }
      
        (*m_slinkCRCVec[type])[0] = ((*itColl)->getFragCRC()>>16) & 0xffff;
        (*m_dmuMaskVec[type])[0] = ((*itColl)->getFragDMUMask()>>16) & 0xffff;
        (*m_slinkCRCVec[type])[1] = (*itColl)->getFragCRC() & 0xffff;
        (*m_dmuMaskVec[type])[1] = (*itColl)->getFragDMUMask() & 0xffff;
      
        // go through all TileDigits in collection
        it = (*itColl)->begin();
        itEnd = (*itColl)->end();
        
        for(;it != itEnd;it++) {
          emptyColl = false;
          
          HWIdentifier hwid=(*it)->adc_HWID();
          // determine channel       
          channel = m_tileHWID->channel(hwid);
          // cabling for testbeam
          if (fragType > 0) channel = digiChannel2PMT(fragType,channel);
 
          // gain
          (*m_gainVec[type])[channel] = m_tileHWID->adc(hwid);
          ATH_MSG_DEBUG( "Storing TD for channel: " << channel
                         << " with gain " << m_tileHWID->adc(hwid) );

        
          // get digits
          sampleVec = (*it)->samples();
          int siz = sampleVec.size();

          if (msgLvl(MSG::DEBUG)) {
            msg(MSG::DEBUG) << "Digits." << siz << " {";
            for (int i = 0; i < siz; i++) {
              msg(MSG::DEBUG) << (int) sampleVec[i] << " ";
            }
            msg(MSG::DEBUG) << "}" << endmsg;
          }

          for (n = 0; n < siz; n++) {
            (*m_sampleVec[type])[channel][n] = (int) sampleVec[n];
          }
        }
      }
    }
    // next container
  }
  
  if (emptyColl)
    return StatusCode::FAILURE;
  else
    return StatusCode::SUCCESS;
}

StatusCode TileTBNtuple::finalize() {

  ATH_MSG_INFO( "finalize() successfully" );

  return StatusCode::SUCCESS;
}

StatusCode TileTBNtuple::connectFile(void) {
  MsgStream log(msgSvc(), name());

  m_ntupleCreated = false;
  char file[100];
  sprintf(file, m_ntupleLoc.c_str(), ++m_currentFileNum);
  std::string ntupleLoc = file;

  NTupleDirPtr DirPtr(ntupleSvc(), ntupleLoc);
  if (!DirPtr) DirPtr = ntupleSvc()->createDirectory(ntupleLoc);
  if (!DirPtr) {
    ATH_MSG_ERROR( "Invalid Ntuple Directory: " );
    return StatusCode::FAILURE;
  }

  m_ntuplePtr = ntupleSvc()->book(DirPtr.ptr(), m_ntupleID, CLID_ColumnWiseTuple, "TileBEAM-Ntuple");
  if (!m_ntuplePtr) {

    std::string ntupleCompleteID = ntupleLoc + "/" + m_ntupleID;

    NTuplePtr nt(ntupleSvc(), ntupleCompleteID);
    if (!nt) {
      ATH_MSG_ERROR( "Failed to book or to retreive ntuple " << ntupleCompleteID );
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO( "Reaccessing ntuple " << ntupleCompleteID );

      m_ntuplePtr = nt;
      // we share ntuple with some other algorithm
      // so have to book all columns now, otherwise Athena will crash
      m_ntupleCreated = true;
      StatusCode sc = initNTuple();
      if (sc.isFailure()) {
        ATH_MSG_ERROR(  " Error during ntuple initialization" );
      }
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode TileTBNtuple::initNTuple(void) {

  m_evtVec.clear();
  m_bcidVec.clear();
  m_rodBCIDVec.clear();
  m_sizeVec.clear();
  m_dmuMaskVec.clear();
  m_slinkCRCVec.clear();
  m_gainVec.clear();
  m_errVec.clear();
  m_sampleVec.clear();
  m_feCRCVec.clear();
  m_rodCRCVec.clear();

  m_eneVec.clear();
  m_timeVec.clear();

  m_efitVec.clear();
  m_tfitVec.clear();
  m_pedfitVec.clear();
  m_chi2Vec.clear();

  m_eOptVec.clear();
  m_tOptVec.clear();
  m_pedOptVec.clear();
  m_chi2OptVec.clear();

  m_eDspVec.clear();
  m_tDspVec.clear();
  m_chi2DspVec.clear();

  // *** Create items for TRIGGER ***

  CHECK( m_ntuplePtr->addItem("TRIGGER/EvTime", m_evTime) );
  CHECK( m_ntuplePtr->addItem("TRIGGER/Run", m_run) );
  CHECK( m_ntuplePtr->addItem("TRIGGER/Evt", m_evt) );
  CHECK( m_ntuplePtr->addItem("TRIGGER/Trig", m_trigType, 0, 65535) );
  

  if ( m_completeNtuple ) {
    // Info from ROD headers
    // m_nDrawers drawers separately (i.e. with duplications) + Beam ROD
    CHECK(m_ntuplePtr->addItem("TRIGGER/L1ID", m_nDrawers + 1, m_l1ID, 0, 65535));
    CHECK(m_ntuplePtr->addItem("TRIGGER/L1Type", m_nDrawers + 1, m_l1Type, 0, 65535));
    CHECK(m_ntuplePtr->addItem("TRIGGER/EvType", m_nDrawers + 1, m_evType, 0, 65535));
    CHECK(m_ntuplePtr->addItem("TRIGGER/EvBCID", m_nDrawers + 1, m_evBCID, 0, 65535));
    CHECK(m_ntuplePtr->addItem("TRIGGER/FrBCID", m_nDrawers, m_frBCID, 0, 65535));
  }

  // *** Create items for MUON ***
  if ( m_beamIdList[MUON_ADC_FRAG] ||
      m_beamIdList[ADDR_ADC_FRAG] ) {

    CHECK(m_ntuplePtr->addItem("MUON/MuBackHit", m_muBackHit));
    CHECK(m_ntuplePtr->addItem("MUON/MuBackSum", m_muBackSum));
    CHECK(m_ntuplePtr->addItem("MUON/MuBack", 14, m_muBack));
    CHECK(m_ntuplePtr->addItem("MUON/MuCalib", 2, m_muCalib));
  }
  
  // *** Create items for ECAL ***
  if ( m_beamIdList[ECAL_ADC_FRAG] ) {
    CHECK( m_ntuplePtr->addItem("ECAL/Ecal",8,m_ecal) );
  }
      
  // *** Create items for LASER ***
  if ( m_beamIdList[LASE_PTN_FRAG] ) {
    CHECK( m_ntuplePtr->addItem("LASER/LasFlag",m_lasFlag,0,65535) );
  }
  if ( (  m_unpackAdder && m_beamIdList[BEAM_ADC_FRAG] ) ||
       ( !m_unpackAdder && m_beamIdList[LASE_ADC_FRAG] ) ) {

    CHECK(m_ntuplePtr->addItem("LASER/Las0", m_las0));
    CHECK(m_ntuplePtr->addItem("LASER/Las1", m_las1));
    CHECK(m_ntuplePtr->addItem("LASER/Las2", m_las2));
    CHECK(m_ntuplePtr->addItem("LASER/Las3", m_las3));
    CHECK(m_ntuplePtr->addItem("LASER/LasExtra", 4, m_lasExtra));
  }
  
  // *** Create items for ADDER ***
  if (m_unpackAdder) {
    if ( m_beamIdList[ADD_FADC_FRAG] ) {
      CHECK(m_ntuplePtr->addItem("ADDER/Adder", 16, 16, m_adder, 0, 255));
      CHECK(m_ntuplePtr->addItem("ADDER/EneAdd", 16, m_eneAdd));
      CHECK(m_ntuplePtr->addItem("ADDER/TimeAdd", 16, m_timeAdd));
    }
  }
  
  // *** Create items for STATUS ***
  // CHECK( m_ntuplePtr->addItem("STATUS/Status",2,3,m_status) );

  // *** Create items for CISPAR ***
  if ( m_beamIdList[DIGI_PAR_FRAG&0x1F] ) {
    CHECK( m_ntuplePtr->addItem("CISPAR/Cispar",16,m_cispar) );
  }
  
  // *** Create items for BEAM ***
  if ( (  m_unpackAdder && m_beamIdList[BEAM_ADC_FRAG] ) ||
       ( !m_unpackAdder && m_beamIdList[COMMON_ADC1_FRAG] ) ) {

    CHECK(m_ntuplePtr->addItem("BEAM/S1cou", m_s1cou, 0, 65535));
    CHECK(m_ntuplePtr->addItem("BEAM/S2cou", m_s2cou, 0, 65535));
    CHECK(m_ntuplePtr->addItem("BEAM/S3cou", m_s3cou, 0, 65535));
    CHECK(m_ntuplePtr->addItem("BEAM/MuTag", m_muTag, 0, 65535));
    CHECK(m_ntuplePtr->addItem("BEAM/Cher1", m_cher1, 0, 65535));
    CHECK(m_ntuplePtr->addItem("BEAM/Cher2", m_cher2, 0, 65535));
    CHECK(m_ntuplePtr->addItem("BEAM/MuHalo", m_muHalo, 0, 65535));
    CHECK(m_ntuplePtr->addItem("BEAM/MuVeto", m_muVeto, 0, 65535));
  }
  
  if ( ! m_unpackAdder ) {

    if ( m_beamIdList[COMMON_ADC2_FRAG] ) {
      CHECK( m_ntuplePtr->addItem("BEAM/S2extra",m_s2extra,0,65535) );
      CHECK( m_ntuplePtr->addItem("BEAM/S3extra",m_s3extra,0,65535) );
    }
    
    if (m_beamIdList[BEAM_ADC_FRAG]) {
      CHECK(m_ntuplePtr->addItem("BEAM/SC1", m_sc1, 0, 65535));
      CHECK(m_ntuplePtr->addItem("BEAM/SC2", m_sc2, 0, 65535));
    }

    if (m_beamIdList[COMMON_PTN_FRAG]) {
      CHECK(m_ntuplePtr->addItem("BEAM/pu", m_commonPU, 0, 65535));
    }
    
    if (m_beamIdList[COMMON_TOF_FRAG]) {
      CHECK(m_ntuplePtr->addItem("BEAM/tof", 8, m_tof, 0, 65535));
    }

    if (m_beamIdList[COMMON_TDC1_FRAG]) {
      CHECK(m_ntuplePtr->addItem("BEAM/btdc1", 16, m_btdc1, 0, 65535));
    }

    if (m_beamIdList[COMMON_TDC2_FRAG]) {
      CHECK(m_ntuplePtr->addItem("BEAM/btdc2", 16, m_btdc2, 0, 65535));
    }

    if (m_beamIdList[COMMON_TDC1_FRAG]) {
      CHECK(m_ntuplePtr->addItem("BEAM/XchN2", m_xChN2));
      CHECK(m_ntuplePtr->addItem("BEAM/YchN2", m_yChN2));
      CHECK(m_ntuplePtr->addItem("BEAM/XchN1", m_xChN1));
      CHECK(m_ntuplePtr->addItem("BEAM/YchN1", m_yChN1));
    }
    if ( m_beamIdList[COMMON_TDC2_FRAG] ) {
      CHECK( m_ntuplePtr->addItem("BEAM/Xcha0",m_xCha0) );
      CHECK( m_ntuplePtr->addItem("BEAM/Ycha0",m_yCha0) );
    }
  } else {
    if (m_beamIdList[BEAM_TDC_FRAG]) {
      CHECK(m_ntuplePtr->addItem("BEAM/btdc1", 8, m_btdc1, 0, 65535));
      CHECK(m_ntuplePtr->addItem("BEAM/btdc2", 8, m_btdc2, 0, 65535));
    }
  }

  if ((m_unpackAdder && m_beamIdList[BEAM_TDC_FRAG])
      || (!m_unpackAdder && m_beamIdList[COMMON_TDC2_FRAG])) {

    CHECK(m_ntuplePtr->addItem("BEAM/Xcha1", m_xCha1));
    CHECK(m_ntuplePtr->addItem("BEAM/Ycha1", m_yCha1));
    CHECK(m_ntuplePtr->addItem("BEAM/Xcha2", m_xCha2));
    CHECK(m_ntuplePtr->addItem("BEAM/Ycha2", m_yCha2));
    CHECK(m_ntuplePtr->addItem("BEAM/Ximp", m_xImp));
    CHECK(m_ntuplePtr->addItem("BEAM/Yimp", m_yImp));
  }
  
  // *** Create items for TileDigits and TileRawChannels ***

  unsigned int listSize = std::min(m_nDrawers, static_cast<unsigned int>(m_drawerMap.size()));

  if (listSize > 0) {

    std::string digit[10] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };
    std::vector<std::string> suffixArr;
    unsigned int length;
    bool testbeam = TileCablingService::getInstance()->getTestBeam();

    if (m_calibMode) {

      length = 2 * m_nDrawers;
      suffixArr.resize(length);

      for (unsigned int i = 0; i < listSize; ++i) {
        unsigned int ros = m_drawerType[i];
        unsigned int drawer = strtol(m_drawerList[i].data(), NULL, 0) & 0x3F;
        std::string digits;
        if (testbeam) {
          digits = digit[drawer & 7];
        } else {
          ++drawer; // count modules from 1
          digits = digit[drawer / 10] + digit[drawer % 10];
        }

        if (ros == 0) {
          std::string suff = m_drawerList[i];
          suff.replace(suff.find("0x"), 2, "");
          suffixArr[i] = suff + "lo";
          suffixArr[i + m_nDrawers] = suff + "hi";
        } else {
          suffixArr[i] = m_rosName[ros] + digits + "lo";
          suffixArr[i + m_nDrawers] = m_rosName[ros] + digits + "hi";
        }
      }
    } else {

      length = m_nDrawers;
      suffixArr.resize(length);

      for (unsigned int i = 0; i < listSize; ++i) {
        unsigned int ros = m_drawerType[i];
        unsigned int drawer = strtol(m_drawerList[i].data(), NULL, 0) & 0x3F;
        std::string digits;
        if (testbeam) {
          digits = digit[drawer & 7];
        } else {
          ++drawer; // count modules from 1
          digits = digit[drawer / 10] + digit[drawer % 10];
        }

        if (ros == 0) {
          std::string suff = m_drawerList[i];
          suff.replace(suff.find("0x"), 2, "");
          suffixArr[i] = suff;
        } else {
          suffixArr[i] = m_rosName[ros] + digits;
        }
      }
    }

    // create NTuple objs in vectors
    for (unsigned int i = 0; i < length; i++) {

      if (i % m_nDrawers < listSize)
      ATH_MSG_DEBUG("Adding items for " << suffixArr[i]);

      NTuple::Item<int> *evt = new NTuple::Item<int>;
      NTuple::Array<int> *bcid = new NTuple::Array<int>;
      NTuple::Item<int> *rodBCID = new NTuple::Item<int>;
      NTuple::Item<int> *size = new NTuple::Item<int>;
      NTuple::Array<int> *dmu = new NTuple::Array<int>;
      NTuple::Array<int> *crc = new NTuple::Array<int>;
      NTuple::Array<int> *gain = new NTuple::Array<int>;
      NTuple::Array<int> *err = new NTuple::Array<int>;
      NTuple::Matrix<int> *sample = new NTuple::Matrix<int>;
      NTuple::Array<int> *fe_crc = new NTuple::Array<int>;
      NTuple::Array<int> *rod_crc = new NTuple::Array<int>;

      NTuple::Array<float> *ene = new NTuple::Array<float>;
      NTuple::Array<float> *time = new NTuple::Array<float>;

      NTuple::Array<float> *efit = new NTuple::Array<float>;
      NTuple::Array<float> *tfit = new NTuple::Array<float>;
      NTuple::Array<float> *pedfit = new NTuple::Array<float>;
      NTuple::Array<float> *chi2 = new NTuple::Array<float>;

      NTuple::Array<float> *eOpt = new NTuple::Array<float>;
      NTuple::Array<float> *tOpt = new NTuple::Array<float>;
      NTuple::Array<float> *pedOpt = new NTuple::Array<float>;
      NTuple::Array<float> *chi2Opt = new NTuple::Array<float>;

      NTuple::Array<float> *eDsp = new NTuple::Array<float>;
      NTuple::Array<float> *tDsp = new NTuple::Array<float>;
      NTuple::Array<float> *chi2Dsp = new NTuple::Array<float>;

      m_evtVec.push_back(evt); // I
      m_bcidVec.push_back(bcid); // U
      m_rodBCIDVec.push_back(rodBCID); // U
      m_sizeVec.push_back(size); // U
      m_dmuMaskVec.push_back(dmu); // U(2)
      m_slinkCRCVec.push_back(crc); // U(2)
      m_gainVec.push_back(gain); // U(48/96)
      m_errVec.push_back(err); // U(48/96)
      m_sampleVec.push_back(sample); // U(48/96,9)
      m_feCRCVec.push_back(fe_crc); //U
      m_rodCRCVec.push_back(rod_crc); //U

      m_eneVec.push_back(ene);
      m_timeVec.push_back(time);

      m_efitVec.push_back(efit);
      m_tfitVec.push_back(tfit);
      m_pedfitVec.push_back(pedfit);
      m_chi2Vec.push_back(chi2);

      m_eOptVec.push_back(eOpt);
      m_tOptVec.push_back(tOpt);
      m_pedOptVec.push_back(pedOpt);
      m_chi2OptVec.push_back(chi2Opt);

      m_eDspVec.push_back(eDsp);
      m_tDspVec.push_back(tDsp);
      m_chi2DspVec.push_back(chi2Dsp);


     if (i % m_nDrawers < listSize) {
        // create ntuple layout
        ATH_MSG_DEBUG( "Adding Leaf to Event '" << ("RAW" + suffixArr[i] + "/Evt" + suffixArr[i]) << "' @" << evt);

        CHECK( m_ntuplePtr->addItem("RAW" + suffixArr[i] + "/Evt" + suffixArr[i], *evt) ); // uint
        CHECK( m_ntuplePtr->addItem("RAW" + suffixArr[i] + "/BCID" + suffixArr[i], 16, *bcid, 0, 65535) ); // uint
        CHECK( m_ntuplePtr->addItem("RAW" + suffixArr[i] + "/rodBCID" + suffixArr[i], *rodBCID, 0, 65535) ); //uint
        CHECK( m_ntuplePtr->addItem("RAW" + suffixArr[i] + "/Size" + suffixArr[i], *size, 0, 65535) ); // uint
        CHECK( m_ntuplePtr->addItem("RAW" + suffixArr[i] + "/DMUMask" + suffixArr[i], 2, *dmu, 0, 65535) ); // uint
        CHECK( m_ntuplePtr->addItem("RAW" + suffixArr[i] + "/SlinkCRC" + suffixArr[i], 2, *crc, 0, 65535) ); // uint
        // XXX: Number of samples taken from JobOptions and number of channels is hard-coded as 48
        CHECK( m_ntuplePtr->addItem("RAW" + suffixArr[i] + "/Gain" + suffixArr[i], 48, *gain, 0, 1) ); // uint
        CHECK( m_ntuplePtr->addItem("RAW" + suffixArr[i] + "/Err" + suffixArr[i], 48, *err, 0, 1023) ); // uint
        if (m_nSamples > 0) {
          CHECK( m_ntuplePtr->addItem("RAW" + suffixArr[i] + "/Sample" + suffixArr[i], 48, m_nSamples, *sample, 0, 1023) ); // uint
        }
        CHECK( m_ntuplePtr->addItem("RAW" + suffixArr[i] + "/feCRC" + suffixArr[i], 16, *fe_crc, 0, 1) ); // uint
        CHECK( m_ntuplePtr->addItem("RAW" + suffixArr[i] + "/rodCRC" + suffixArr[i], 16, *rod_crc, 0, 1) ); // uint

        if (m_flatRawChannelContainer.size() > 0) {

          CHECK( m_ntuplePtr->addItem("ENE" + suffixArr[i] + "/Ene" + suffixArr[i], 48, *ene) ); // float
          CHECK( m_ntuplePtr->addItem("ENE" + suffixArr[i] + "/Time" + suffixArr[i], 48, *time) ); // float
        }

        if (m_fitRawChannelContainer.size() > 0) {

          CHECK( m_ntuplePtr->addItem("FIT" + suffixArr[i] + "/Efit" + suffixArr[i], 48, *efit) ); // float
          CHECK( m_ntuplePtr->addItem("FIT" + suffixArr[i] + "/Tfit" + suffixArr[i], 48, *tfit) ); // float
          CHECK( m_ntuplePtr->addItem("FIT" + suffixArr[i] + "/Pedfit" + suffixArr[i], 48, *pedfit) ); // float
          CHECK( m_ntuplePtr->addItem("FIT" + suffixArr[i] + "/Chi2" + suffixArr[i], 48, *chi2) ); // float
        }

        if (m_optRawChannelContainer.size() > 0) {

          CHECK( m_ntuplePtr->addItem("OPT" + suffixArr[i] + "/Eopt" + suffixArr[i], 48, *eOpt) ); // float
          CHECK( m_ntuplePtr->addItem("OPT" + suffixArr[i] + "/Topt" + suffixArr[i], 48, *tOpt) ); // float
          CHECK( m_ntuplePtr->addItem("OPT" + suffixArr[i] + "/Pedopt" + suffixArr[i], 48, *pedOpt) ); // float
          CHECK( m_ntuplePtr->addItem("OPT" + suffixArr[i] + "/Chi2opt" + suffixArr[i], 48, *chi2Opt) ); // float
        }

        if (m_dspRawChannelContainer.size() > 0) {

          CHECK( m_ntuplePtr->addItem("DSP" + suffixArr[i] + "/Edsp" + suffixArr[i], 48, *eDsp) ); // float
          CHECK( m_ntuplePtr->addItem("DSP" + suffixArr[i] + "/Tdsp" + suffixArr[i], 48, *tDsp) ); // float
          CHECK( m_ntuplePtr->addItem("DSP" + suffixArr[i] + "/Chi2dsp" + suffixArr[i], 48, *chi2Dsp) ); // float
        }

      }
    }
  }

  if (m_completeNtuple) {
    // *** Create items for total energy in samplings ***
    CHECK( m_ntuplePtr->addItem("EneTotal/LarSmp", 4, m_LarEne) );
    CHECK( m_ntuplePtr->addItem("EneTotal/BarSmp", 3, m_BarEne) );
    CHECK( m_ntuplePtr->addItem("EneTotal/ExtSmp", 3, m_ExtEne) );
    CHECK( m_ntuplePtr->addItem("EneTotal/GapSmp", 3, m_GapEne) );
  }

  if (m_beamIdList[COIN_TRIG1_FRAG]) {
    CHECK( m_ntuplePtr->addItem("CoincBoard/CoincTrig1", 96, m_coincTrig1) );
    CHECK( m_ntuplePtr->addItem("CoincBoard/CoincFlag1", m_coincFlag1) );
  }

  if (m_beamIdList[COIN_TRIG2_FRAG]) {
    CHECK( m_ntuplePtr->addItem("CoincBoard/CoincTrig2", 96, m_coincTrig2) );
    CHECK( m_ntuplePtr->addItem("CoincBoard/CoincFlag2", m_coincFlag2) );
  }

  if (m_beamIdList[COIN_TRIG3_FRAG]) {
    CHECK( m_ntuplePtr->addItem("CoincBoard/CoincTrig3", 96, m_coincTrig3) );
    CHECK( m_ntuplePtr->addItem("CoincBoard/CoincFlag3", m_coincFlag3) );
  }

  if (m_beamIdList[COIN_TRIG4_FRAG]) {
    CHECK( m_ntuplePtr->addItem("CoincBoard/CoincTrig4", 96, m_coincTrig4) );
    CHECK( m_ntuplePtr->addItem("CoincBoard/CoincFlag4", m_coincFlag4) );
  }

  if (m_beamIdList[COIN_TRIG5_FRAG]) {
    CHECK( m_ntuplePtr->addItem("CoincBoard/CoincTrig5", 96, m_coincTrig5) );
    CHECK( m_ntuplePtr->addItem("CoincBoard/CoincFlag5", m_coincFlag5) );
  }

  if (m_beamIdList[COIN_TRIG6_FRAG]) {
    CHECK( m_ntuplePtr->addItem("CoincBoard/CoincTrig6", 96, m_coincTrig6) );
    CHECK( m_ntuplePtr->addItem("CoincBoard/CoincFlag6", m_coincFlag6) );
  }
  if (m_beamIdList[COIN_TRIG7_FRAG]) {
    CHECK( m_ntuplePtr->addItem("CoincBoard/CoincTrig7", 96, m_coincTrig7) );
    CHECK( m_ntuplePtr->addItem("CoincBoard/CoincFlag7", m_coincFlag7) );
  }

  if (m_beamIdList[COIN_TRIG8_FRAG]) {
    CHECK( m_ntuplePtr->addItem("CoincBoard/CoincTrig8", 96, m_coincTrig8) );
    CHECK( m_ntuplePtr->addItem("CoincBoard/CoincFlag8", m_coincFlag8) );
  }

  return StatusCode::SUCCESS;
}

StatusCode TileTBNtuple::storeCells() {

  const CaloCellContainer* cellContainer;
  //Retrieve Cell collection from SG

  ATH_MSG_DEBUG( "retrieve cellContainer from SG" );

  CHECK( evtStore()->retrieve(cellContainer, "AllCalo") );

  //Loop over all cells in container. Sum up the Energy and fill 2DHistograms

  ATH_MSG_DEBUG( "succeeded retrieving cellContainer from SG" );

  CaloCellContainer::const_iterator ifirst = cellContainer->begin();
  CaloCellContainer::const_iterator ilast = cellContainer->end();
  ATH_MSG_DEBUG( "TileTBNtuple : about to iterate over CaloCells" );

  m_LarEne[0] = m_LarEne[1] = m_LarEne[2] = m_LarEne[3] = 0.0;
  m_BarEne[0] = m_BarEne[1] = m_BarEne[2] = 0.0;
  m_ExtEne[0] = m_ExtEne[1] = m_ExtEne[2] = 0.0;
  m_GapEne[0] = m_GapEne[1] = m_GapEne[2] = 0.0;

  for (; ifirst != ilast; ++ifirst) {
    //Decode cell information
    const CaloCell* cell_ptr = *ifirst;    // pointer to cell object
    const double energy = cell_ptr->energy();
    const CaloDetDescrElement* caloDDE_ptr = cell_ptr->caloDDE(); //pointer to the DetectorDescriptionElement
    const CaloCell_ID::CaloSample sampl = caloDDE_ptr->getSampling(); //To which sampling belongs this cell?
//    const CaloCell_ID::SUBCALO subcalo= caloDDE_ptr->getSubCalo(); 

    if (sampl == CaloCell_ID::PreSamplerB) {
      m_LarEne[0] += energy;
    } else if (sampl == CaloCell_ID::EMB1) {
      m_LarEne[1] += energy;
    } else if (sampl == CaloCell_ID::EMB2) {
      m_LarEne[2] += energy;
    } else if (sampl == CaloCell_ID::EMB3) {
      m_LarEne[3] += energy;
    } else if (sampl == CaloCell_ID::TileBar0) {
      m_BarEne[0] += energy;
    } else if (sampl == CaloCell_ID::TileBar1) {
      m_BarEne[1] += energy;
    } else if (sampl == CaloCell_ID::TileBar2) {
      m_BarEne[2] += energy;
    } else if (sampl == CaloCell_ID::TileExt0) {
      m_ExtEne[0] += energy;
    } else if (sampl == CaloCell_ID::TileExt1) {
      m_ExtEne[1] += energy;
    } else if (sampl == CaloCell_ID::TileExt2) {
      m_ExtEne[2] += energy;
    } else if (sampl == CaloCell_ID::TileGap1) {
      m_GapEne[1] += energy;
    } else if (sampl == CaloCell_ID::TileGap2) {
      m_GapEne[2] += energy;
    } else if (sampl == CaloCell_ID::TileGap3) {
      m_GapEne[0] += energy;
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode TileTBNtuple::initList() {

  unsigned int size = m_drawerList.size();

  if (size > 0) {

    int frag = strtol(m_drawerList[0].data(), NULL, 0);

    if (frag < 0) { // setup frags IDs from the data

      // Read Digits from TDS
      const TileDigitsContainer *digitsCnt;

      if (evtStore()->retrieve(digitsCnt, m_digitsContainer).isFailure()) {
        ATH_MSG_ERROR( "can't retrieve Digits from TDS" );
        ATH_MSG_ERROR( "can't set up fragment list for ntuple" );

        if (m_nSamples != 0) {
          ATH_MSG_WARNING( "Disable digit samples in ntuple" );
          m_nSamples = 0;
        }

        return StatusCode::SUCCESS;
      }

      // Get iterator for all TDColl in TDCont
      TileDigitsContainer::const_iterator itColl = (*digitsCnt).begin();
      TileDigitsContainer::const_iterator itCollEnd = (*digitsCnt).end();

      std::vector<unsigned int> frags;
      // Go through all TileDigitsCollections
      for (; itColl != itCollEnd; itColl++) {
        // determine type of frag
        if ((*itColl)->begin() != (*itColl)->end()) frags.push_back((*itColl)->identify());
      }
      size = frags.size();

      if (size > 0) {

        unsigned int rosOrder[5] = { 2, 1, 3, 4, 0 };
        unsigned int dr = 0;
        char frg[6] = "0x000";

        m_drawerList.clear();
        // m_drawerType.clear();
        m_drawerMap.clear();

        msg(MSG::INFO) << "setting drawerList from data " << MSG::hex;
        for (unsigned int ir = 0; ir < 5; ++ir) {
          for (unsigned int i = 0; i < size; ++i) {
            unsigned int frag = frags[i];
            if (frag >> 8 == rosOrder[ir]) {
              sprintf(frg, "0x%3.3x", frag);
              m_drawerList.push_back((std::string) frg);
              if (dr == m_drawerType.size()) m_drawerType.push_back(frag >> 8);
              m_drawerMap[frag] = dr;
              msg(MSG::INFO) << " 0x" << frag;
              ++dr;
            }
          }
        }
        msg(MSG::INFO) << MSG::dec << endmsg;

        size = m_drawerType.size();
        if (size < m_nDrawers) {
          m_drawerType.resize(m_nDrawers);
          for (; size < m_nDrawers; ++size)
            m_drawerType[size] = 0;
        }

        msg(MSG::INFO) << "drawerType ";
        for (unsigned int dr = 0; dr < size; ++dr)
          msg(MSG::INFO) << " " << m_drawerType[dr];
        msg(MSG::INFO) << endmsg;

        if (size > m_nDrawers) {
          ATH_MSG_INFO( "increasing m_nDrawers from " << m_nDrawers << " to " << size );
           m_nDrawers = size;
        }

        if (size < 1) size = 1;
        if (m_eventsPerFile == 0) {
          m_eventsPerFile = (int) (200 / size) * 1000;
          ATH_MSG_INFO( "Number of events per file was 0, set it to 200k/" << size << " = " << m_eventsPerFile );
        }

      } else {

        ATH_MSG_ERROR( "can't find any TileDigits collections" );
        ATH_MSG_ERROR( "can't set up fragment list for ntuple" );
      }
    }

    // once again - check number of samples in the data
    // but do not print any ERRORs now

    // Read Digits from TDS
    const TileDigitsContainer *digitsCnt;
    if (evtStore()->retrieve(digitsCnt, m_digitsContainer).isSuccess()) {

      // Get iterator for all TDColl in TDCont
      TileDigitsContainer::const_iterator itColl = (*digitsCnt).begin();
      TileDigitsContainer::const_iterator itCollEnd = (*digitsCnt).end();

      // Go through all TileDigitsCollections
      for (; itColl != itCollEnd; itColl++) {
        if ((*itColl)->begin() != (*itColl)->end()) {
          int siz = (*(*itColl)->begin())->samples().size();
          if (siz > m_nSamples && m_nSamples != 0) {
            ATH_MSG_WARNING(  "Increasing number of digit samples in ntuple from "
                             << m_nSamples << " to " << siz );
            m_nSamples = siz;
          }
        }
      }
    } else {
      if (m_nSamples != 0) {
        ATH_MSG_WARNING( "Disable digit samples in ntuple" );
        m_nSamples = 0;
      }
    }
  }

  return StatusCode::SUCCESS;
}



//////////////////////////////////////////////////////////////////////////////
void TileTBNtuple::getEta() {

  // Get eta from an ASCII file with the following structure : 
  // runnumber eta

  ATH_MSG_INFO( "Get eta for run " << m_runNumber );

  // Find the full path to filename:
  std::string fileName = PathResolver::find_file(m_etaFileName, "DATAPATH");
  ATH_MSG_INFO( "Reading file  " << fileName );

  if (fileName.size() == 0) {
    ATH_MSG_WARNING( "Could not find input file " << m_etaFileName );
    ATH_MSG_WARNING( "Skip reading of eta value " );

  } else {

    std::ifstream etafile;
    etafile.open(fileName.c_str());

    if (etafile.good()) {

      int runNumber = 0;
      float eta = 0;
      while ((runNumber != m_runNumber) && (!etafile.eof())) {
        etafile >> runNumber >> eta;
      }

      if (runNumber != m_runNumber) {
        ATH_MSG_INFO( "Run " << m_runNumber << " has not been found, keep eta and theta at zero" );
        m_eta = m_theta = 0.0;
      } else {
        m_eta = eta;
        m_theta = (M_PI_2 - 2 * atan(exp(m_eta)));
        ATH_MSG_INFO( "Run " << m_runNumber
                             << " has been found with eta=" << m_eta
                             << ", theta =" << m_theta );
      }

    } else {

      ATH_MSG_WARNING( "Problem with file " << fileName );
      ATH_MSG_WARNING( "Skip reading of eta value " );
    }

    etafile.close();
  }
}
