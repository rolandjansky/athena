/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Tile includes
#include "TileRecUtils/TileRawChannelBuilder.h"
#include "TileRecUtils/TileBeamInfoProvider.h"
#include "TileRecUtils/ITileRawChannelTool.h"
#include "TileEvent/TileDigits.h"
#include "TileEvent/TileRawChannel.h"
#include "TileIdentifier/TileHWID.h"
#include "TileConditions/TileInfo.h"
#include "CaloIdentifier/TileID.h"

// Atlas includes
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/WriteHandle.h"

// Gaudi includes


static const InterfaceID IID_ITileRawChannelBuilder("TileRawChannelBuilder", 1, 0);

const InterfaceID& TileRawChannelBuilder::interfaceID() {
  return IID_ITileRawChannelBuilder;
}

int TileRawChannelBuilder::s_error[MAX_CHANNELS] = {0};
int TileRawChannelBuilder::s_dmuerr[MAX_DMUS] = {0};
int TileRawChannelBuilder::s_lastDrawer = -1;
bool TileRawChannelBuilder::s_badDrawer = false;


void TileRawChannelBuilder::resetDrawer() {
  s_lastDrawer = -1;
  s_badDrawer = false;
}

void TileRawChannelBuilder::resetOverflows() {
  m_overflows.clear();
}

Overflows_t& TileRawChannelBuilder::getOverflowedChannels() {
  return m_overflows;
}

std::string TileRawChannelBuilder::getTileRawChannelContainerID() {
  return m_rawChannelContainerKey.key();
}

/**
 * Constructor
 */
TileRawChannelBuilder::TileRawChannelBuilder(const std::string& type
    , const std::string& name, const IInterface* parent)
  : AthAlgTool(type, name, parent)
  , m_rChType(TileFragHash::Default)
  , m_rChUnit(TileRawChannelUnit::ADCcounts)
  , m_bsflags(0)
  , m_tileID(0)
  , m_tileHWID(0) 
  , m_tileInfo(0)
  , m_beamInfo("TileBeamInfoProvider/TileBeamInfoProvider")
  , m_noiseFilterTools() // "TileRawChannelNoiseFilter/TileRawChannelNoiseFilter")
  , m_trigType(0)
  , m_idophys(false)
  , m_idolas(false)
  , m_idoped(false)
  , m_idocis(false)
  , m_cischan(-1)
  , m_capdaq(100)
  , m_evtCounter(0)
  , m_chCounter(0)
  , m_nChL(0)
  , m_nChH(0)
  , m_RChSumL(0.0)
  , m_RChSumH(0.0)
  , m_notUpgradeCabling(true)
{
  resetDrawer();
  memset(s_error, 0, sizeof(s_error));
  memset(s_dmuerr, 0, sizeof(s_dmuerr));

  declareProperty("calibrateEnergy", m_calibrateEnergy = false);
  declareProperty("correctTime", m_correctTime = false);
  declareProperty("AmpMinForAmpCorrection", m_ampMinThresh = 15.0);
  declareProperty("TimeMinForAmpCorrection", m_timeMinThresh = -25.0);
  declareProperty("TimeMaxForAmpCorrection", m_timeMaxThresh =  25.0);
  declareProperty("RunType", m_runType = 0);
  declareProperty("BeamInfo", m_beamInfo);
  declareProperty("NoiseFilterTools", m_noiseFilterTools);
  declareProperty("DataPoolSize", m_dataPoollSize = -1);
  declareProperty("UseDSPCorrection", m_useDSP = true);

}

/**
 * Destructor
 */
TileRawChannelBuilder::~TileRawChannelBuilder() {
}

/**
 * Initializer
 */
StatusCode TileRawChannelBuilder::initialize() {

  ATH_MSG_INFO( "TileRawChannelBuilder::initialize()" );

  m_trigType = m_runType;
  m_idophys = ((m_trigType == 0) || (m_trigType == 1));
  m_idolas = ((m_trigType == 2) || (m_trigType == 3));
  m_idoped = ((m_trigType == 4) || (m_trigType == 5));
  m_idocis = ((m_trigType == 8) || (m_trigType == 9));
  m_cischan = -1;
  m_capdaq = 0;
  m_chCounter = 0;
  m_evtCounter = 0;
  m_rawChannelCnt = NULL;
  m_nChL = m_nChH = 0;
  m_RChSumL = m_RChSumH = 0.0;

  // retrieve TileID helpers and TileIfno from det store
  CHECK( detStore()->retrieve(m_tileID, "TileID") );
  CHECK( detStore()->retrieve(m_tileHWID, "TileHWID") );

  CHECK( detStore()->retrieve(m_tileInfo, "TileInfo") );

  CHECK( m_beamInfo.retrieve() );

  // access tools and store them
  CHECK( m_noiseFilterTools.retrieve() );
  ATH_MSG_DEBUG( "Successfully retrieve  NoiseFilterTools: " << m_noiseFilterTools );

  if(m_calibrateEnergy){
    ATH_MSG_DEBUG( "Obsolete calibrateEnergy flag is set to True in jobOptions - disabling it" );
    m_calibrateEnergy = false;
  }
  
  // check if we want to keep ADC counts or convert them to pCb
  m_rChUnit = (m_calibrateEnergy) ? TileRawChannelUnit::PicoCoulombs
                                  : TileRawChannelUnit::ADCcounts;

  // if unit is not pCb, but already MeV one can use method TileRawChannelContainer::set_unit()
  // later to declare that

  // 8 upper bits of bsflags:
  // UUPPSTTT
  // 31,30 - units
  // 29,28 - pulse type = 3 for offline reco
  // 27    - 7(=0) or 9(=1) samples
  // 24,25,26 - TileFragHash::TYPE - OF algorithm type
  int nsamp = (m_tileInfo->NdigitSamples() > 7) ? 1 : 0;
  m_bsflags = (m_rChUnit << 30) | (3 << 28) | (nsamp << 27) | (m_rChType << 24);

  // bits 12-15 - various options
  if (m_correctTime)  m_bsflags |= 0x1000;

  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << "TileRawChannelBuilder created, storing rc in '"
                    << m_rawChannelContainerKey.key() << "'" << endmsg;
    msg(MSG::DEBUG) << " calibrate energy = " << m_calibrateEnergy << endmsg;
    msg(MSG::DEBUG) << " correct time = " << m_correctTime << endmsg;
    msg(MSG::DEBUG) << " run type = " << m_runType << endmsg;
  }

  if (m_dataPoollSize < 0) m_dataPoollSize = m_tileHWID->channel_hash_max();

  ServiceHandle<TileCablingSvc> cablingSvc("TileCablingSvc", name());
  CHECK( cablingSvc.retrieve());
    
  const TileCablingService* cabling = cablingSvc->cablingService();
  if (!cabling) {
    ATH_MSG_ERROR( "Unable to retrieve TileCablingService" );
    return StatusCode::FAILURE;
  }
  
  m_notUpgradeCabling = (cabling->getCablingType() != TileCablingService::UpgradeABC);

  ATH_CHECK( m_rawChannelContainerKey.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode TileRawChannelBuilder::finalize() {
  ATH_MSG_INFO( "Finalizing" );
  return StatusCode::SUCCESS;
}

StatusCode TileRawChannelBuilder::createContainer() {
  initLog();

  // create TRC container
  m_rawChannelCnt = std::make_unique<TileRawChannelContainer>(true, m_rChType, m_rChUnit, SG::VIEW_ELEMENTS);
  m_rawChannelCnt->set_bsflags(m_bsflags);

  ATH_MSG_DEBUG( "Created TileRawChannelContainer '" << m_rawChannelContainerKey.key() << "'" );

  return StatusCode::SUCCESS;
}

void TileRawChannelBuilder::initLog() {

  // update only if there is new event
  if (m_evtCounter != m_beamInfo->eventCounter()) {

    m_evtCounter = m_beamInfo->eventCounter();
    if (m_runType != 0) m_trigType = m_runType;
    else m_trigType = m_beamInfo->trigType();

    if (0 == m_trigType) {
      m_idophys = (m_beamInfo->calibMode() == 0);
      m_idolas = false;
      m_idoped = false;
      m_idocis = (m_beamInfo->calibMode() == 1);
    } else {
      m_idophys = (m_trigType <= 1);
      m_idolas = ((m_trigType == 2) || (m_trigType == 3));
      m_idoped = ((m_trigType == 4) || (m_trigType == 5));
      m_idocis = ((m_trigType == 8) || (m_trigType == 9));
    }

    const unsigned int *cispar = m_beamInfo->cispar();
    if (0 == cispar[7]) { // if capdaq not set, it can't be CIS event
      if (m_idocis) { // cis flag was set incorrectly, change to ped
        m_idoped = true;
        m_idocis = false;
      }
      m_capdaq = 0.0;
    } else {
      m_capdaq = (cispar[7] < 10) ? 5.2 : 100.0;
    }
    m_cischan = cispar[8] - 1; // channel where CIS is fired (-1 = all channels)

    ATH_MSG_DEBUG( "Trig type is " << m_trigType
                  << "; dophys is " << ((m_idophys) ? "true" : "false")
                  << "; dolas is " << ((m_idolas) ? "true" : "false")
                  << "; doped is " << ((m_idoped) ? "true" : "false")
                  << "; docis is " << ((m_idocis) ? "true" : "false")
                  << "; capacitor is " << m_capdaq
                  << "; cis chan is " << m_cischan );
  }
}

TileRawChannel* TileRawChannelBuilder::rawChannel(const TileDigits* digits) {
  ++m_chCounter;
  ATH_MSG_WARNING( "Default constructor for rawChannel!" );
  TileRawChannel *rawCh = new TileRawChannel(digits->adc_HWID(), 0.0, 0.0, 0.0);
  return rawCh;
}

void TileRawChannelBuilder::fill_drawer_errors(const TileDigitsCollection* coll) {
  const TileDQstatus* DQstatus = m_beamInfo->getDQstatus();

  int frag = coll->identify();
  int ros = (frag >> 8);
  int drawer = (frag & 0xff);

  s_lastDrawer = frag;

  memset(s_error, 0, sizeof(s_error));
  memset(s_dmuerr, 0, sizeof(s_dmuerr));
  int nch = 0;
  bool bigain = DQstatus->isBiGain();
  if (!bigain) { // in bigain runs we don't have DQ status fragment
    for (int ch = 0; ch < MAX_CHANNELS; ch += 3) {
      if (!DQstatus->isAdcDQgood(ros, drawer, ch, 0)) {
        s_error[ch + 2] = s_error[ch + 1] = s_error[ch] = -3;
        s_dmuerr[ch / 3] = 3;
        nch += 3;
      }
    }
  }
  if (nch == MAX_CHANNELS) { // all bad - nothing to do
    s_badDrawer = true;
    ATH_MSG_VERBOSE( "Drawer 0x" << MSG::hex << frag << MSG::dec
                    << " is bad - skipping bad patterns check " );
    return;
  } else {
    s_badDrawer = false;
    ATH_MSG_VERBOSE(  "Drawer 0x" << MSG::hex << frag << MSG::dec
                    << " looking for bad patterns in digits" );
  }

  float mindig, maxdig;
  int nchbad[2] = { 0, 0 };

  // Iterate over all digits in this collection
  TileDigitsCollection::const_iterator digitItr = coll->begin();
  TileDigitsCollection::const_iterator lastDigit = coll->end();

  for (; digitItr != lastDigit; ++digitItr) {
    const TileDigits * pDigits = (*digitItr);
    HWIdentifier adcId = pDigits->adc_HWID();
    int channel = m_tileHWID->channel(adcId);
    int gain = m_tileHWID->adc(adcId);

    if (s_error[channel]) {
      ATH_MSG_VERBOSE( "BadCh " << ros
                        << "/" << drawer
                        << "/" << channel
                        << "/" << gain << " BAD DQ STATUS ");

    } else {

      int err = CorruptedData(ros, drawer, channel, gain, pDigits->samples(), mindig, maxdig);

      if (err) {

        s_error[channel] = err;
        if (err > -5) {
          ++s_dmuerr[channel / 3];
          ++nchbad[channel / 24];
        }

        if (msgLvl(MSG::VERBOSE)) {

          msg(MSG::VERBOSE) << "BadCh " << ros
                            << "/" << drawer
                            << "/" << channel
                            << "/" << gain;
          if (err) {
            if (err < -5) msg(MSG::VERBOSE) << " Warning " << err;
            else msg(MSG::VERBOSE) << " Error " << err;
          }
          if (mindig > 2046.99) msg(MSG::VERBOSE) << " BADDQ";
          if (maxdig > 1022.9) msg(MSG::VERBOSE) << " Overflow";
          if (mindig < 0.1) msg(MSG::VERBOSE) << " Underflow";
          if (err < 0) msg(MSG::VERBOSE) << " Const";

          msg(MSG::VERBOSE) << " samp=";
          std::vector<float> digits = pDigits->samples();
          for (unsigned int i = 0; i < digits.size(); ++i) {
            msg(MSG::VERBOSE) << " " << digits[i];
          }
          msg(MSG::VERBOSE) << endmsg;
        }

      } else {
        if (mindig < 0.01) err += 1;
        if (maxdig > 1022.99) err += 2;
        if (err) s_error[channel] = err - 10;
      }
    }
  }

  // check if we want to mask half a drawer 
  // in this case set error = -4 for channels which were good before

  int ndmubad[2] = { 0, 0 };
  int dmu = 0;
  for (; dmu < MAX_DMUS / 2; ++dmu) { // first half
    if (s_dmuerr[dmu] > 1)
      ++ndmubad[0]; // count DMUs with at least two bad channels
  }
  for (; dmu < MAX_DMUS; ++dmu) { // second half
    if (s_dmuerr[dmu] > 1)
      ++ndmubad[1]; // count DMUs with at least two bad channels
  }

  int ndmulimit[2] = { 3, 3 }; // max number of bad DMUs when half-drawer is not yet masked
                               // if 4 DMUs will be bad - mask whole half-drawer
  if (frag > 0x2ff) { // if extended barrel
    if (frag == 0x30e || frag == 0x411)
      ndmulimit[0] = 4; // in EB special one DMU is always bad (missing)
    ndmulimit[1] = 5; // in second half of EB 4 DMUs ara always bad (missing)
                      // only if 7 DMUs are bad, mask whole half-drawer
  }

  bool printall = true;
  for (int p = 0; p < 2; ++p) {
    if (ndmubad[p] > ndmulimit[p] && nchbad[p] > 0) {
      if (msgLvl(MSG::VERBOSE)) {
        msg(MSG::VERBOSE) << "Drawer 0x" << MSG::hex << frag << MSG::dec
                          << " masking whole " << ((p) ? "second" : "first")
                          << " half" << endmsg;
        if (printall) {
          msg(MSG::VERBOSE) << "nDMuErr ";
          for (int d = 0; d < MAX_DMUS; ++d) {
            msg(MSG::VERBOSE) << " " << s_dmuerr[d];
          }
          msg(MSG::VERBOSE) << " total " << ndmubad[p] << " errors" << endmsg;

          msg(MSG::VERBOSE) << "ChErr ";
          int ch = 0;
          while (ch < MAX_CHANNELS) {
            msg(MSG::VERBOSE) << " " << s_error[ch++];
            msg(MSG::VERBOSE) << " " << s_error[ch++];
            msg(MSG::VERBOSE) << " " << s_error[ch++];
            msg(MSG::VERBOSE) << "  ";
          }

          msg(MSG::VERBOSE) << " total " << nchbad[p]
                            << " bad patterns" << endmsg;
          printall = false;
        }
      }
      int ch = (p) ? MAX_CHANNELS / 2 : 0;
      int chmax = (p) ? MAX_CHANNELS : MAX_CHANNELS / 2;
      for (; ch < chmax; ++ch) {
        if (s_error[ch] == 0 || s_error[ch] < -5) { // channel was good before
          s_error[ch] = -4;
        }
      }
    }
  }

}

const char * TileRawChannelBuilder::BadPatternName(float ped) {
  static const char * errname[25] = {
      "-10 - good signal",
      "-9 - underflow",
      "-8 - overflow",
      "-7 - underflow and overflow",
      "-6 - constant signal",
      "-5 - disconnected channel",
      "-4 - half a drawer masked",
      "-3 - bad DQ status",
      "-2 - underflow in all samples",  
      "-1 - overflow in all samples",
      "0 - unknown error",
      "1 - jump from zero to saturation",
      "2 - samples with zeros",
      "3 - at least two saturated. others - close to pedestal",
      "4 - two distinct levels with at least 2 samples each",
      "5 - pedestal with jump up in one sample",
      "6 - pedestal with jump down in one sample",
      "7 - signal with jump up in one sample",
      "8 - signal with jump down in one sample",
      "9 - base line above threshold in low gain",
      "10 - jump down in first sample in low gain",
      "11 - jump down in last sample in low gain",
      "12 - jump up in one sample above const",
      "13 - jump down in one sample below const",
      "14 - unknown error"
  };
  
  return errname[std::min(24, std::max(0, int((ped + 500) * 1e-4)))];
}

    
void TileRawChannelBuilder::build(const TileDigitsCollection* coll) {
  int frag = coll->identify();

  // make sure that errror array is up-to-date
  if (frag != s_lastDrawer && m_notUpgradeCabling) {
    fill_drawer_errors(coll);
  }

  // Iterate over all digits in this collection
  TileDigitsCollection::const_iterator digitItr = coll->begin();
  TileDigitsCollection::const_iterator lastDigit = coll->end();

  for (; digitItr != lastDigit; ++digitItr) {

    TileRawChannel* rch = rawChannel((*digitItr));

    if (m_notUpgradeCabling) {

      int err = s_error[m_tileHWID->channel(rch->adc_HWID())];
      
      if (err) {
        if (err == -8 || err == -7) m_overflows.push_back(std::make_pair(rch, (*digitItr)));
        float ped = rch->pedestal() + 100000 + 10000 * err;
        rch->setPedestal(ped);
        if (msgLvl(MSG::VERBOSE) && !s_badDrawer) {
          if (err < -5) {
            msg(MSG::VERBOSE) << "BadCh " << m_tileHWID->to_string(rch->adc_HWID())
                              << " warning = " << BadPatternName(ped) << endmsg;
          } else {
            msg(MSG::VERBOSE) << "BadCh " << m_tileHWID->to_string(rch->adc_HWID())
                              << " error = " << BadPatternName(ped) << endmsg;
          }
        }
      }

    }

    m_rawChannelCnt->push_back(rch);
  }
}

StatusCode TileRawChannelBuilder::commitContainer() {

  ToolHandleArray<ITileRawChannelTool>::iterator itrTool = m_noiseFilterTools.begin();
  ToolHandleArray<ITileRawChannelTool>::iterator endTool = m_noiseFilterTools.end();

  const TileRawChannelContainer * dspCnt = m_beamInfo->dspContainer();

  if ( m_useDSP && (m_beamInfo->incompleteDigits() || m_chCounter<12288) && dspCnt && itrTool!=endTool) {
    ATH_MSG_DEBUG( "Incomplete container - use noise filter corrections from DSP container" );
    if (m_beamInfo->noiseFilterApplied()) {
      ATH_MSG_DEBUG( "Noise filter was already applied to DSP container before, use it as it is" );
    } else {
      for (;itrTool!=endTool;++itrTool){
        if ((*itrTool)->process(dspCnt).isFailure()) {
          ATH_MSG_ERROR( " Error status returned from noise filter " );
        } else {
          ATH_MSG_DEBUG( "Noise filter applied to DSP container" );
        }
      }
      m_beamInfo->setNoiseFilterApplied(true);
    }

    TileRawChannelContainer::const_iterator itColl = m_rawChannelCnt->begin();
    TileRawChannelContainer::const_iterator itCollEnd = m_rawChannelCnt->end();

    TileRawChannelContainer::const_iterator dtColl = dspCnt->begin();
    TileRawChannelContainer::const_iterator dtCollEnd = dspCnt->end();

    // Go through all TileRawChannelCollections
    for (; itColl != itCollEnd && dtColl !=dtCollEnd; ++itColl, ++dtColl) {
      const TileRawChannelCollection *coll = (*itColl);
      const TileRawChannelCollection *dcoll = (*dtColl);

      if (coll->identify() != dcoll->identify()) {

        ATH_MSG_ERROR( " Error in applying noise corrections " << MSG::hex 
                       << " collection IDs 0x" << coll->identify() <<  " and 0x" << dcoll->identify() 
                       << " do not match " << MSG::dec );
        break;
      }
      
      // iterate over all channels in a collection
      TileRawChannelCollection::const_iterator rchItr=coll->begin();
      TileRawChannelCollection::const_iterator lastRch=coll->end();

      TileRawChannelCollection::const_iterator dspItr=dcoll->begin();
      TileRawChannelCollection::const_iterator dspLast=dcoll->end();

      for(; rchItr!=lastRch; ++rchItr) {
        TileRawChannel* rch = (*rchItr);
        HWIdentifier adc_id = rch->adc_HWID();
        while (dspItr != dspLast && adc_id != (*dspItr)->adc_HWID()) {
          ++dspItr;
        }
        if (dspItr != dspLast) {
          float corr = (*dspItr)->pedestal();
          ATH_MSG_VERBOSE( "Ch "<<m_tileHWID->to_string(adc_id)
                           <<" amp " << rch->amplitude() << " ped " << rch->pedestal() 
                           << " corr " << corr );
          rch->setAmplitude (rch->amplitude() - corr);
          rch->setPedestal (rch->pedestal() + corr);
        } else {
          ATH_MSG_WARNING(" Problem in applying noise corrections " 
                        << " can not find channel in DSP container with HWID "
                        << m_tileHWID->to_string(adc_id) );
          dspItr = dcoll->begin();
        }
      }
    }
    
  } else {

    for (ToolHandle<ITileRawChannelTool>& noiseFilterTool : m_noiseFilterTools) {
      if (noiseFilterTool->process(m_rawChannelCnt.get()).isFailure()) {
        ATH_MSG_ERROR( " Error status returned from noise filter " );
      } else {
        ATH_MSG_DEBUG( "Noise filter applied to the container" );
      }
    }

  }
  
  ATH_MSG_DEBUG( " nCh=" << m_chCounter
                << " nChH/L=" << m_nChH << "/" << m_nChL
                << " RChSumH/L=" << m_RChSumH << "/" << m_RChSumL );

  SG::WriteHandle<TileRawChannelContainer> rawChannelsContainer(m_rawChannelContainerKey);
  ATH_CHECK( rawChannelsContainer.record(std::move(m_rawChannelCnt)) );

  endLog();

  return StatusCode::SUCCESS;
}

void TileRawChannelBuilder::endLog() {
  m_chCounter = 0;
  m_nChL = m_nChH = 0;
  m_RChSumL = m_RChSumH = 0.0;

}

double TileRawChannelBuilder::correctAmp(double phase, bool of2) {

 double corr = 1.0;
 if (of2) {
   // estimation from Belen for rel 14.0.0
   /*double a,b,c;
   if(fabs(phase)<5.){
   a=0.137; b=0.0877; c=0.0865;
   }else{
   a=0.565; b=0.116; c=0.0751;
   }
   corr=(1+(a+b*phase+c*phase*phase)/100.);
   */

  // estimation from Vakhtang for rel 14.4.0
  /*double k = (phase < 0.0 ? 0.0009400 : 0.0010160);
  corr = (1.0 + k * phase * phase);
  */

   // Parabolic correction from Tigran
   double a1,a2,b,c;
   a1 = phase < 0.0 ? 0.000940774 : 0.00102111;
   a2 = phase < 0.0 ? 0.000759051 : 0.000689625;
   b = phase < 0.0 ? -2.0 * 7.0 * (a1 - a2) : 2.0 * 12.5 * (a1 - a2);
   c = phase < 0.0 ? 1.0 - 7.0 * 7.0 * (a1-a2) :  1.0 - 12.5 * 12.5 * (a1-a2);
   if (phase < 12.5 && phase > -7.0) corr = a1 * phase * phase + 1.0;
   else corr = phase * ( a2  * phase + b) + c;


 } else {
  /*double a,b,c;
  if(phase<0){
     a=1.0002942; b=0.0003528; c=0.0005241;
  }else{
     a=1.0001841; b=-0.0004182; c=0.0006167;
  }
  corr = a + phase * ( b + c * phase);
  */

  /*double k = (phase < 0.0 ? 0.0005241 : 0.0006167);
  corr = (1.0 + k * phase * phase);
  */

  // 4th degree polynomial correction from Tigran
  double k1 = (phase < 0.0 ? -0.0000326707:0.000380336);
  double k2 = (phase < 0.0 ? -0.000560962:-0.000670487);
  double k3 = (phase < 0.0 ? -0.00000807869:0.00000501773);
  double k4 = (phase < 0.0 ? -0.000000145008:0.0000000584647);

  corr = 1.0 / (1.0 + (k1 + (k2  + (k3 + k4 *phase)*phase)*phase)*phase);


 }

  return corr;
}


// Time correction for shifted pulses by Tigran
double TileRawChannelBuilder::correctTime(double phase, bool of2) {

  double correction = 0.0;
  
  if (of2) {
    if(phase < 0)  {
      correction = (-0.00695743 + (0.0020673 - (0.0002976 + 0.00000361305 * phase) * phase) * phase) * phase;
    } else {
      correction = (0.0130013 + (0.00128769 + (-0.000550218 + 0.00000755344 * phase) * phase) * phase) * phase;
    }
  }
  // OF1 does not need correction

  return correction;
}



int TileRawChannelBuilder::CorruptedData(int ros, int drawer, int channel, int gain,
    const std::vector<float> & digits, float &dmin, float &dmax) {
  bool eb = (ros > 2);
  bool ebsp = ((ros == 3 && drawer == 14) || (ros == 4 && drawer == 17));
  bool empty = ((eb && ((channel > 23 && channel < 30) || channel > 41)) || (ebsp && channel < 3));
  bool not_gap = !(empty || (eb && (channel == 0 || channel == 1 || channel == 12 || channel == 13))
      || (ebsp && (channel == 18 || channel == 19)));

  const float epsilon = 4.1; // allow +/- 2 counts fluctuations around const value
  const float delta[4] = { 29.9, 29.9, 49.9, 99.9 }; // jump levels between constLG, constHG, non-constLG, non-constHG
  const float level1 = 99.9; // jump from this level to 1023 is bad 
  const float level2 = 149.9; // base line at this level in low gain is bad
  const float narrowLevel[2] = { 29.9, 49.9 }; // minimal amplitude for narrow pulses
  const float delt = std::min(std::min(std::min(delta[0], delta[1]), std::min(delta[2], delta[3])),
      std::min(narrowLevel[0], narrowLevel[1]));
  const float secondMaxLevel = 0.3;

  int error = 0;

  unsigned int nSamp = digits.size();
  if (nSamp) {
    dmin = dmax = digits[0];
    unsigned int pmin = 0;
    unsigned int pmax = 0;
    unsigned int nzero = (dmin < 0.01) ? 1 : 0;
    unsigned int nover = (dmax > 1022.99) ? 1 : 0;

    for (unsigned int i = 1; i < nSamp; ++i) {
      float dig = digits[i];
      if (dig > dmax) {
        dmax = dig;
        pmax = i;
      } else if (dig < dmin) {
        dmin = dig;
        pmin = i;
      }
      if (dig < 0.01) ++nzero;
      else if (dig > 1022.99) ++nover;
    }

    float dmaxmin = dmax - dmin;
    //std::cout << " ros " << ros << " drawer " << drawer << " channel " << channel << " not_gap " << not_gap << " nzero " << nzero << " nover " << nover << std::endl;

    if (dmin > 1022.99) { // overflow in all samples
      error = (dmin > 2046.99) ? -3 : -1; // dmin=2047 - masking in overlay job (set in TileDigitsMaker)

    } else if (dmax < 0.01) { // underflow in all samples
      error = (empty) ? -5 : -2; // set different type of errors for exsiting and non-existing channels

    } else if (dmaxmin < 0.01) { // constant value in all samples
      error = -6;

    } else if (nzero && nover) { // jump from zero to saturation
      error = 1;

    } else if ((nzero && (not_gap || empty)) || nzero > 1) { // one sample at zero in normal channel
      error = 2;                                           // or 2 samples at zero in gap/crack/MBTS

    } else if (gain == 0 && dmin > level2) { // baseline above threshold in low gain is bad
      error = 9;

    } else if (dmaxmin > delt) { // check that max-min is above minimal allowed jump

      float abovemin = dmax;
      float belowmax = dmin;
      unsigned int nmin = 0;
      unsigned int nmax = 0;
      for (unsigned int i = 0; i < nSamp; ++i) {
        float smp = digits[i];
        if (smp - dmin < epsilon) {
          ++nmin;
        }
        if (dmax - smp < epsilon) {
          ++nmax;
        }
        if (smp < abovemin && smp > dmin) {
          abovemin = smp;
        }
        if (smp > belowmax && smp < dmax) {
          belowmax = smp;
        }
      }
      // more than two different values - shift index by 2, i.e. use thresholds for non-const levels
      int gainInd = (abovemin != dmax || belowmax != dmin) ? gain + 2 : gain;
      bool big_jump = (dmaxmin > delta[gainInd]);
      bool max_in_middle = (pmax > 0 && pmax < nSamp - 1);
      bool min_in_middle = (pmin > 0 && pmin < nSamp - 1);

      if (nover > 1 && belowmax < level1) {  // at least two saturated. others - close to pedestal
        error = 3;
      } else if (nmax + nmin == nSamp && big_jump) {
        if (nmax > 1 && nmin > 1) { // at least 2 samples at two distinct levels
          error = 4;
        } else if (nmax == 1) {
          if (max_in_middle) { // jump up in one sample, but not at the edge
            error = 5;
          }
        } else if (nmin == 1) { // jump down in one sample
          error = 6;
        }
      }
      if (error == 0 && dmaxmin > narrowLevel[gain]) {
        float secondMax = dmaxmin * secondMaxLevel;
        float dminPlus = dmin + secondMax;
        float dmaxMinus = dmax - secondMax;
        if (not_gap) { // jumps above two (or one) neighbour samples
          if (max_in_middle && std::max(digits[pmax - 1], digits[pmax + 1]) < dminPlus) {
            error = 7; // jump up in one sample in the middle, which is much higher than two neighbours
          } else if (min_in_middle && std::min(digits[pmin - 1], digits[pmin + 1]) > dmaxMinus) {
            error = 8; // jump down in one sample, which is much lower than two neighbours
          } else if (big_jump && gain == 0) { // check first and last sample only in low gain
            if (pmin == 0 && digits[1] > dmax - secondMax) {
              error = 10; // jump down in first sample. which is much lower than next one
            } else if (pmin == nSamp - 1 && digits[pmin - 1] > dmax - secondMax) {
              error = 11; // jump down in last sample. which is much lower than previous one
            }
          }
        }
        if (!error && big_jump) { // jumps above all samples 
          if ((max_in_middle || gain == 0) && nmax == 1 && belowmax < dminPlus) {
            error = 12; // jump up in one sample in the middle, which is much higher than all others
          } else if ((min_in_middle || gain == 0) && nmin == 1 && abovemin > dmaxMinus) {
            error = 13; // jump down in one sample, which is much lower than all others (
          }
        }
      }
    }

  } else {
    dmin = dmax = 0.0;
  }

  return error;
}
