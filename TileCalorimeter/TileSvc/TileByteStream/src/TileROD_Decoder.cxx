/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileEvent/TileCell.h"

#include <algorithm>
#include <iomanip>
#include <sstream>

// Implementation of TileROD_Decoder class

// Gaudi includes
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/ServiceHandle.h"

// Atlas includes
#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"
#include "AthenaKernel/errorcheck.h"

#include "TileRecUtils/TileRawChannelBuilder.h"
#include "TileL2Algs/TileL2Builder.h"
#include "TileByteStream/TileHid2RESrcID.h"
#include "TileIdentifier/TileHWID.h"
#include "TileIdentifier/TileTBFrag.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileConditions/TileCondToolOfcCool.h"
#include "TileConditions/TileCondToolTiming.h"
#include "TileConditions/TileCondToolEmscale.h"
#include "TileConditions/TileCablingService.h"
#include "TileRecUtils/TileRawChannelBuilder.h"
#include "TileByteStream/TileROD_Decoder.h"


#define DO_NOT_USE_MUON_TAG true

static const InterfaceID IID_ITileROD_Decoder("TileROD_Decoder", 1, 0);

/** constructor
 */

TileROD_Decoder::TileROD_Decoder(const std::string& type, const std::string& name,
                                 const IInterface* parent)
  : AthAlgTool(type, name, parent)
  , m_tileToolTiming("TileCondToolTiming")
  , m_tileCondToolOfcCool("TileCondToolOfcCool")
  , m_tileToolEmscale("TileCondToolEmscale")
  , m_hid2re(0)
  , m_hid2reHLT(0)
  , m_maxChannels(TileCalibUtils::MAX_CHAN)
  , m_fullTileRODs(320000) // default 2017 full mode
{
  declareInterface<TileROD_Decoder>(this);
  
  declareProperty("useFrag0", m_useFrag0 = true);
  declareProperty("useFrag1", m_useFrag1 = true);
  declareProperty("useFrag4", m_useFrag4 = true);
  declareProperty("useFrag5Raw", m_useFrag5Raw = false);
  declareProperty("useFrag5Reco", m_useFrag5Reco = false);
  declareProperty("ignoreFrag4HLT", m_ignoreFrag4HLT = false);
  
  declareProperty("TileCondToolTiming", m_tileToolTiming);
  declareProperty("TileCondToolOfcCool", m_tileCondToolOfcCool, "TileCondToolOfcCool");
  declareProperty("TileCondToolEmscale", m_tileToolEmscale);
  
  declareProperty("TileCellEthresholdMeV", m_TileCellEthreshold = -100000.);
  declareProperty("TileDefaultChannelBuilder", m_TileDefaultChannelBuilder = "TileRawChannelBuilderFlatFilter/TileROD_RCBuilder");
  declareProperty("TileDefaultL2Builder", m_TileDefaultL2Builder = "TileL2Builder/TileROD_L2Builder");
  declareProperty("VerboseOutput", m_verbose = false);
  declareProperty("calibrateEnergy", m_calibrateEnergy = true); // convert ADC counts to pCb for RawChannels
  declareProperty("suppressDummyFragments", m_suppressDummyFragments = false);
  declareProperty("maskBadDigits", m_maskBadDigits = false); // put -1 in digits vector for channels with bad BCID or CRC in unpack_frag0
  declareProperty("MaxWarningPrint", m_maxWarningPrint = 1000);
  declareProperty("MaxErrorPrint", m_maxErrorPrint = 100);

  declareProperty("AllowedTimeMin", m_allowedTimeMin = -50.); // set amp to zero if time is below allowed time min
  declareProperty("AllowedTimeMax", m_allowedTimeMax =  50.); // set amp to zero if time is above allowed time max
  declareProperty("fullTileMode", m_fullTileRODs); // run from which to take the cabling (for the moment, either 320000 - full 2017 mode - or 0 - 2016 mode)

  m_correctAmplitude = false;
  updateAmpThreshold(15.);
  m_timeMinThresh = -25;
  m_timeMaxThresh = 25;
  
  for (unsigned int i = 0; i < 7; ++i) {
    m_digitsMetaData.push_back(new std::vector<uint32_t>);
    m_digitsMetaData[i]->reserve(16);
    m_rawchannelMetaData.push_back(new std::vector<uint32_t>);
    m_rawchannelMetaData[i]->reserve(2);
    (*(m_rawchannelMetaData[i]))[0] = 0;
    (*(m_rawchannelMetaData[i]))[1] = 0;
  }
  m_sizeOverhead = 2; // 2 extra words in every frag by default (frag id + frag size)
  // but for all data after 2005 it is set to 3 later in the code
  
  m_of2 = true;
  m_rChType = TileFragHash::Digitizer;
  m_rChUnit = TileRawChannelUnit::ADCcounts;
  m_bsflags = 0;
  m_container = 0;
  m_MBTS = NULL;
  m_cell2Double.reserve(23); // Maximum number of cells in a drawer
  m_WarningCounter = 0;
  m_ErrorCounter = 0;
  
  m_OFWeights.resize(4 * TileCalibUtils::MAX_DRAWERIDX, NULL);
}

void TileROD_Decoder::updateAmpThreshold(float ampMinThresh) {
  m_ampMinThresh = ampMinThresh;
  m_ampMinThresh_pC = m_ampMinThresh * (12.5 / 1023.);
  m_ampMinThresh_MeV = m_ampMinThresh * (12.5 / 1023. / 1.05 * 1000.);
}

/** destructor
 */
TileROD_Decoder::~TileROD_Decoder() {
  for (unsigned int i = 0; i < 7; ++i) {
    delete m_digitsMetaData[i];
    delete m_rawchannelMetaData[i];
  }
  if (m_hid2re) delete m_hid2re;
  if (m_hid2reHLT) delete m_hid2reHLT;
  
  for (unsigned int id = 0; id < 4 * TileCalibUtils::MAX_DRAWERIDX; ++id)
    if (m_OFWeights[id]) delete m_OFWeights[id];
}

const InterfaceID& TileROD_Decoder::interfaceID() {
  return IID_ITileROD_Decoder;
}

void TileROD_Decoder::printWarningCounter(bool printIfNoWarning) {
  if (printIfNoWarning || m_WarningCounter > 0) {
    ATH_MSG_WARNING( "Found " << m_WarningCounter << " warnings in decoding words");
  }
}

void TileROD_Decoder::printErrorCounter(bool printIfNoError) {
  if (printIfNoError || m_ErrorCounter > 0) {
    ATH_MSG_ERROR( "Found " << m_ErrorCounter << " errors in decoding words");
  }
}

int TileROD_Decoder::getWarningCounter() {
  return m_WarningCounter;
}

int TileROD_Decoder::getErrorCounter() {
  return m_ErrorCounter;
}

StatusCode TileROD_Decoder::initialize() {
  
  m_rc2bytes5.setVerbose(m_verbose);
  m_rc2bytes4.setVerbose(m_verbose);
  m_rc2bytes2.setVerbose(m_verbose);
  m_rc2bytes.setVerbose(m_verbose);
  m_d2Bytes.setVerbose(m_verbose);
  
  ServiceHandle<IToolSvc> toolSvc("ToolSvc", this->name());
  CHECK( toolSvc.retrieve() );
  
  // retrieve TileHWID helper from det store
  CHECK( detStore()->retrieve(m_tileHWID, "TileHWID") );
  
  if (m_useFrag5Raw || m_useFrag5Reco) {
    //=== get TileCondToolOfcCool
    CHECK( m_tileCondToolOfcCool.retrieve() );
    
    //=== get TileToolTiming
    CHECK( m_tileToolTiming.retrieve() );
  }
  
  //=== get TileCondToolEmscale
  CHECK( m_tileToolEmscale.retrieve() );
 
  // Get Tool to TileChannelBuilder, to be used to convert automatically digits->channels.
  ATH_MSG_DEBUG( "creating algtool " << m_TileDefaultChannelBuilder );
  ListItem algRC(m_TileDefaultChannelBuilder);
  CHECK( toolSvc->retrieveTool(algRC.type(), algRC.name(), m_RCBuilder, this) );
  
  ATH_MSG_DEBUG( "algtool " << m_TileDefaultChannelBuilder << " created " );
  CHECK( m_RCBuilder->setProperty(BooleanProperty("calibrateEnergy", m_calibrateEnergy)) );
  
  m_maxChannels = TileCablingService::getInstance()->getMaxChannels();

  m_pRwChVec.reserve(m_maxChannels);
  // Accumulate TileRawChannels for storing temp data
  for (unsigned int i = 0; i < m_maxChannels; ++i) {
    m_pRwChVec.push_back(new TileFastRawChannel());
  }
  m_Rw2Cell[0].reserve(m_maxChannels);
  m_Rw2Cell[1].reserve(m_maxChannels);
  m_Rw2Cell[2].reserve(m_maxChannels * TileCalibUtils::MAX_DRAWER);
  m_Rw2Cell[3].reserve(m_maxChannels * TileCalibUtils::MAX_DRAWER);
  m_Rw2Pmt[0].reserve(m_maxChannels);
  m_Rw2Pmt[1].reserve(m_maxChannels);
  m_Rw2Pmt[2].reserve(m_maxChannels * TileCalibUtils::MAX_DRAWER);
  m_Rw2Pmt[3].reserve(m_maxChannels * TileCalibUtils::MAX_DRAWER);
  
  if (m_TileDefaultL2Builder.size() > 0) {
    ATH_MSG_DEBUG( "creating algtool " << m_TileDefaultL2Builder );
    ListItem algL2(m_TileDefaultL2Builder);
    CHECK( toolSvc->retrieveTool(algL2.type(), algL2.name(), m_L2Builder, this) );
    ATH_MSG_DEBUG( "algtool " << m_TileDefaultL2Builder << " created " );
  } else {
    m_L2Builder = 0;
  }
  
  // Resets error flag
  m_error = 0x0;
  
  // Initialize
  this->m_hashFunc.initialize(m_tileHWID);
  
  return StatusCode::SUCCESS;
}

StatusCode TileROD_Decoder::finalize() {
  // Delete reserved TileFastRawChannels
  for (unsigned int i = 0; i < m_pRwChVec.size(); ++i) {
    delete m_pRwChVec[i];
  }
  m_pRwChVec.clear();
  for (unsigned int i = 0; i < 4; ++i) {
    m_Rw2Cell[i].clear();
    m_Rw2Pmt[i].clear();
  }
  m_list_of_masked_drawers.clear();
  if (m_WarningCounter != 0) printWarningCounter(1);
  if (m_ErrorCounter != 0) printErrorCounter(1);
  return StatusCode::SUCCESS;
}

// check if bit is set
bool TileROD_Decoder::checkBit(const uint32_t* p, int chan) const {
  // chan = (0,47)
  int a = chan / 32;
  int r = chan % 32;
  // a = (0,1), r = ( 0, 31 )
  return *(p + a) & (1 << r);
  
}

/**
 * Unpack TileDigits stored in ROD.
 * Assume that mode (calib/normal) is the same
 * for every invocation.
 */
void TileROD_Decoder::unpack_frag0(uint32_t version, const uint32_t* p, pDigiVec & pDigits) const {
  int gain = 0;
  int n;
  
  // pointer to current data word
  const uint32_t *data;
  // current chip #
  int chip;
  // channel #
  int channel;
  
  // first word is full frag size, first two words are not data
  int size = *(p) - m_sizeOverhead;
  // second word is frag ID (0x100-0x4ff) and frag type
  int frag = *(p + 1) & 0xFFFF;
  
  // Position of first data word, ignore 2 frag header words
  data = p + 2;
  
  uint32_t all00 = TileFragStatus::ALL_00;
  uint32_t allFF = TileFragStatus::ALL_FF;

  for (int i = 0; i < size; ++i) {
    uint32_t w = data[i];
    if (allFF && w!=0xFFFFFFFF) {
      allFF = TileFragStatus::ALL_OK;
      if (all00 == TileFragStatus::ALL_OK) break;
    }
    if (all00 && w != 0) {
      all00 = TileFragStatus::ALL_OK;
      if (allFF == TileFragStatus::ALL_OK) break;
    }
  }

  uint32_t status = (all00 | allFF);
  m_digitsMetaData[6]->push_back( status );

  // check that fragment is not dummy
  if (m_suppressDummyFragments && status != TileFragStatus::ALL_OK) {
    return; // nothing reasonable found
  }
  
  // if (msgLvl(MSG::VERBOSE))
  //   msg(MSG::VERBOSE) << "version=" << version << " size: " << (*p) << " " << size;
  if (m_sizeOverhead == 2 && (version == 0x2 || version == 0x1)) { /* can not guess number of samples from size */
    if (size > 178 && size < 205)
      size = 179; /* we expect this number (9+2)*16+2 */
    else if (size > 274 && size < 405) size = 275; /* we expect this number (7*2+3)*16+2 */
    ++data; // ignore first word coming from drawer
  }
  
  // if (msgLvl(MSG::VERBOSE))
  //   msg(MSG::VERBOSE) << size << endmsg;
  
  // number of chips in the data, only 16 is expected
  int chipCount = 16;
  // one chip contains 3 channels
  int channelCount = chipCount * 3;
  // size of 'block'. In normal mode one block is header and chipCount words with samples and CRC word
  // In calib mode one block is header word, <chipCount> low gain samples, header, <chipCount> high gain and CRC word
  int blockSize = size / chipCount;
  // deplacement of high gain samples in calib mode
  int gainOffset = 0;
  // calculate number of words per chip including header and excluding CRC
  int wordsPerChip = 0;
  // number of data words per chip, excluding header and CRC
  int dataWordsPerChip = 0;
  // real number of channels and chips
  // int channelCountReal,chipCountReal;
  
  // Digitizer mode doesn't change between events
  static int digiMode = -1;
  
  if (digiMode < 0) { // try to find digi mode until good mode is found
    digiMode = m_d2Bytes.getDigiMode(data, chipCount, blockSize);
  }
  
  if (digiMode > 0 || (digiMode < 0 && blockSize == 17)) {
    // calibration mode
    // 2X number of chips
    // chipCount <<= 1;
    // channelCount = chipCount*3;
    
    // lengt with header and without CRC
    wordsPerChip = (blockSize - 1) / 2;
    // lengt without header and CRC
    dataWordsPerChip = wordsPerChip - 1;
    
    //channelCountReal = channelCount/2;
    //chipCountReal = chipCount/2;
    
    gainOffset = wordsPerChip;
  } else {
    // assume normal mode
    // block contains one CRC word
    wordsPerChip = blockSize - 1;
    // one header word
    dataWordsPerChip = wordsPerChip - 1;
    
    // channelCountReal = channelCount;
    // chipCountReal = chipCount;
  }
  
  if (msgLvl(MSG::VERBOSE)) {
    msg(MSG::VERBOSE) << "Unpacking TileDigits, ID=0x" << MSG::hex << frag
    << ", size=" << MSG::dec << size << endmsg;
    
    msg(MSG::VERBOSE) << " chips=" << chipCount << endmsg;
    msg(MSG::VERBOSE) << " channels=" << channelCount << endmsg;
    msg(MSG::VERBOSE) << " block size=" << blockSize << endmsg;
    msg(MSG::VERBOSE) << " words/chip=" << wordsPerChip << endmsg;
    msg(MSG::VERBOSE) << " data/chip=" << dataWordsPerChip << endmsg;
    msg(MSG::VERBOSE) << " gain offs=" << gainOffset << endmsg;
    msg(MSG::VERBOSE) << " mode=" << digiMode << endmsg;
  }
  
  if (size == 0) {
    ATH_MSG_VERBOSE( "Size is 0, no digits extracted" );
    return;
  }
  
  TileDigits *td;
  HWIdentifier drawerID = m_tileHWID->drawer_id(frag);
  HWIdentifier adcID;
  std::vector<std::vector<float>*> *digiVec;
  
  // take BCID from chip header with good parity
  uint32_t bcid = m_d2Bytes.getBCID(data, chipCount, blockSize);
  
  if (gainOffset > 0) {
    // calibration mode
    pDigits.reserve(96);
    std::vector<uint32_t> *chipHeaderLow = m_digitsMetaData[2];
    std::vector<uint32_t> *chipCRCLow = m_digitsMetaData[3];
    std::vector<uint32_t> *chipHeaderHigh = m_digitsMetaData[4];
    std::vector<uint32_t> *chipCRCHigh = m_digitsMetaData[5];
    
    // loop through all chips
    for (chip = 0; chip < chipCount; ++chip) {
      ATH_MSG_VERBOSE( "Unpacking calib data for chip " << chip );
      
      // Extract low gain digits for fragment
      digiVec = m_d2Bytes.getDigits(data + 1, dataWordsPerChip);
      
      for (n = 0; n < 3; ++n) {
        // calc channel #
        channel = chip * 3 + n;
        // create ID
        adcID = m_tileHWID->adc_id(drawerID, channel, TileHWID::LOWGAIN);
        
        td = new TileDigits(adcID, *(*digiVec)[n]);
        pDigits.push_back(td);
        
        delete ((*digiVec)[n]);
      }
      delete digiVec;
      
      // Extract high gain digits for fragment
      digiVec = m_d2Bytes.getDigits(data + 1 + gainOffset, dataWordsPerChip);
      
      for (n = 0; n < 3; ++n) {
        // calc channel #
        channel = chip * 3 + n;
        // create ID
        adcID = m_tileHWID->adc_id(drawerID, channel, TileHWID::HIGHGAIN);
        
        td = new TileDigits(adcID, *(*digiVec)[n]);
        pDigits.push_back(td);
        
        delete ((*digiVec)[n]);
      }
      delete digiVec;
      
      // store metadata
      chipHeaderLow->push_back(*data);
      chipCRCLow->push_back(*(data + gainOffset - 1));
      chipHeaderHigh->push_back(*(data + gainOffset));
      chipCRCHigh->push_back(*(data + blockSize - 1));
      
      // next block
      data += blockSize;
    }
  } else {
    // Normal mode
    pDigits.reserve(pDigits.size() + 48);
    std::vector<uint32_t> *chipHeader = m_digitsMetaData[2];
    std::vector<uint32_t> *chipCRC = m_digitsMetaData[3];
    
    // loop through all chips
    for (chip = 0; chip < chipCount; ++chip) {
      // Extract digits for fragment
      digiVec = m_d2Bytes.getDigits(data + 1, dataWordsPerChip);
      
      ATH_MSG_VERBOSE( "Header $"  << MSG::hex << (*data)
                      << " CRC $" << *(data + blockSize - 1) );
      
      for (n = 0; n < 3; ++n) {
        // calc channel #
        channel = chip * 3 + n;
        // get gain from chip header
        gain = m_d2Bytes.getGain(data, n);
        // create ID
        adcID = m_tileHWID->adc_id(drawerID, channel, gain);
        td = new TileDigits(adcID, *(*digiVec)[n]);
        
        ATH_MSG_VERBOSE( "Frag: $" << MSG::hex << frag << MSG::dec
                        << " G:" << gain
                        << " C:" << channel
                        << " BCID: " << MSG::hex << m_d2Bytes.getBCID(data, chipCount, blockSize) << MSG::dec
                        << " Data={" << (int) (*(*digiVec)[n])[0]
                        << "," << (int) (*(*digiVec)[n])[1]
                        << "," << (int) (*(*digiVec)[n])[2]
                        << "," << (int) (*(*digiVec)[n])[3]
                        << "," << (int) (*(*digiVec)[n])[4]
                        << "," << (int) (*(*digiVec)[n])[5]
                        << "," << (int) (*(*digiVec)[n])[6] << "}" );
        
        
        pDigits.push_back(td);
        
        delete ((*digiVec)[n]);
      }
      delete digiVec;
      
      // store some metadata, first word in chip is header, last is CRC
      chipHeader->push_back(*data);
      chipCRC->push_back(*(data + blockSize - 1));
      
      // go to next block
      data += blockSize;
    }
  }
  
  int extra = size - (data - p) + 2;
  
  // check size of fragment - we expect 2 extra words (DMU mask and CRC) or more!
  if (extra < 2) {
    ATH_MSG_WARNING( "Too short fragment ! Expected at least " << MSG::dec << (data - p)
                    << " data words, while size from header is " << size
                    << " data words (plus " << m_sizeOverhead << " words overhead)" );
  }
  
  if (msgLvl(MSG::VERBOSE)) {
    msg(MSG::VERBOSE) << "Trailer 0x" << MSG::hex
    << data[0] << " 0x" << data[1];
    for (int i = 2; i < extra; ++i) {
      msg(MSG::VERBOSE) << " 0x" << data[i];
    }
    msg(MSG::VERBOSE) << MSG::dec << endmsg;
    
    // check frag trailer
    if (extra > 0) {
      if ((data[0] & 0xffff) != (data[0] >> 16)) {
        msg(MSG::VERBOSE) << MSG::hex << "Trailer numbers do not match: "
        << data[0] << MSG::dec << endmsg;
      } else {
        if ((data[0] & 0xffff) == 0xffff)
          msg(MSG::VERBOSE) << "Found barrel trailer" << endmsg;
        else if ((data[0] & 0xffff) == 0xfff)
          msg(MSG::VERBOSE) << "Found extended barrel trailer" << endmsg;
        else
          msg(MSG::VERBOSE) << MSG::hex << "Unexpected trailer number: "
          << (data[0] & 0xffff) << MSG::dec << endmsg;
      }
    }
    
    // check CRC trailer
    if (extra > 1) {
      if ((data[1] & 0xffff) != (data[1] >> 16)) {
        msg(MSG::VERBOSE) << "Trailer CRC numbers do not match" << endmsg;
      } else {
        msg(MSG::VERBOSE) << MSG::hex << "Found CRC 0x" << (data[1] & 0xffff)
        << MSG::dec << endmsg;
      }
    }
  }
  
  // store metadata for this collection
  // size, fragID, BCID,  DMU Mask, CRC and other extra words
  m_digitsMetaData[0]->push_back(size);
  m_digitsMetaData[0]->push_back(frag);
  m_digitsMetaData[0]->push_back(bcid);
  for (int i = 0; i < extra; ++i) {
    m_digitsMetaData[1]->push_back(data[i]);
  }
  if (m_sizeOverhead == 2 && (version == 0x2 || version == 0x1)) { // keep also first skipped word
    m_digitsMetaData[1]->push_back(p[2]);
  }
  
  if (dataWordsPerChip > 14) { // can not be true !!!
    digiMode = -1; // check again digiMode in next fragment
  }
}

void TileROD_Decoder::unpack_frag1(uint32_t /* version */, const uint32_t* p,
                                   pDigiVec & pDigits) const {
  // first word is full frag size, first two words are not data
  int size = *(p) - m_sizeOverhead;
  // second word is frag ID (0x100-0x4ff) and frag1 type (old and new version).
  int frag = *(p + 1) & 0xFFFF;
  int frag1version = (*(p + 1) >> 31) & 0x1;
  int nbchanformat1 = ((*(p + 1)) >> 24) & 0x3F;
  
  // store metadata for this collection
  // size, fragID, BCID
  m_digitsMetaData[0]->push_back(size);
  m_digitsMetaData[0]->push_back(frag);
  m_digitsMetaData[0]->push_back(0);
  
  if (frag1version == 0) { //Old version
    p += 2; // 2 words so far
    
    int nsamp = (*(p) >> 8) & 0x0F; // find number of samples in first data word
    int words_per_chan = nsamp + 1; // one extra 16bit word with channel number
    int nchan = 2 * size / words_per_chan;
    
    if (2 * size != nchan * words_per_chan || nchan <= 0 || nchan > 48) {
      ATH_MSG_ERROR( "Digi frag type=1 fragId=0x" << MSG::hex << frag << MSG::dec
                    << " Nsamp=" << nsamp
                    << " Nchan=" << nchan
                    << " Wrong Size=" << size );
      return;
    }
    
    HWIdentifier drawerID = m_tileHWID->drawer_id(frag);
    std::vector<float> digiVec(nsamp);
    pDigits.reserve(nchan);
    
    const uint16_t* p16 = (const uint16_t *) p;
    
    for (int ch = 0; ch < nchan; ++ch) {
      
      int channel = (*p16) & 0xFF;
      int nsamp1 = (*(p16) >> 8) & 0x0F;
      int gain = (*p16) >> 15;
      ++p16;
      
      for (int samp = 0; samp < nsamp; ++samp) {
        digiVec[samp] = *p16++;
      }
      
      if (channel < 48 && nsamp1 == nsamp) {
        HWIdentifier adcID = m_tileHWID->adc_id(drawerID, channel, gain);
        TileDigits * td = new TileDigits(adcID, digiVec);
        pDigits.push_back(td);
      } else {
        ATH_MSG_ERROR( "Digi frag type=1 fragId=0x" << MSG::hex << frag << MSG::dec
                      << " CHind=" << ch
                      << " channel=" << channel
                      << " nsamp=" << nsamp1 << "/" << nsamp );
      }
      
      if (msgLvl(MSG::VERBOSE)) {
        msg(MSG::VERBOSE) << MSG::hex << "Frag: $" << frag << MSG::dec
        << " G:" << gain
        << " C:" << channel
        << " Data={";
        for (int samp = 0; samp < nsamp; ++samp) {
          msg(MSG::VERBOSE) << " " << (int) digiVec[samp];
        }
        msg(MSG::VERBOSE) << "}  " << MSG::hex;
        for (int samp = 0; samp < nsamp / 2 + 1; ++samp) {
          msg(MSG::VERBOSE) << (int) *p++ << " ";
        }
        msg(MSG::VERBOSE) << MSG::dec << endmsg;
      }
    }
  } //End of old version
  
  else if (frag1version == 1) { //New version
    p += 2; // 2 words so far
    
    int nsamp = 7; // New frag1 only for 7 samples
    int SizeOfFrag1 = size * 2; // Number of 16 bit words
    int nbchanformat2 = (SizeOfFrag1 - (3 * nbchanformat1)) / 5;
    
    int nchan = nbchanformat1 + nbchanformat2;
    
    if ((nchan) > 48 || ((nbchanformat1 * 3) + (nbchanformat2 * 5) > SizeOfFrag1)) {
      ATH_MSG_ERROR( "Digi frag type=1 fragId=0x" << MSG::hex << frag << MSG::dec
                    << " frag1Version=" << frag1version
                    << " Nsamp=" << nsamp
                    << " NchanFormat1=" << nbchanformat1
                    << " NchanFormat2=" << nbchanformat2
                    << " Wrong Size=" << size);
      return;
    }
    
    HWIdentifier drawerID = m_tileHWID->drawer_id(frag);
    std::vector<float> digiVec(nsamp);
    pDigits.reserve(nchan);
    pDigiVec sDigits(48);
    
    int ptr16index = 1;
    int channel = 0;
    uint16_t word1 = 0;
    uint16_t word2 = 0;
    uint16_t word3 = 0;
    uint16_t word4 = 0;
    uint16_t word5 = 0;
    
    for (int chf1 = 0; chf1 < nbchanformat1; ++chf1) {
      if (ptr16index == 1)
        channel = ((*p >> 26) & 0x3F);
      else
        channel = ((*p >> 10) & 0x3F);
      
      int gain = 1;
      
      if (ptr16index == 1) {
        word1 = (uint16_t) ((*p >> 16) & 0xFFFF);
        word2 = (uint16_t) (*p & 0xFFFF);
        word3 = (uint16_t) ((*(p + 1) >> 16) & 0xFFFF);
        ptr16index = 0;
      }
      
      else if (ptr16index == 0) {
        word1 = (uint16_t) (*p & 0xFFFF);
        word2 = (uint16_t) ((*(p + 1) >> 16) & 0xFFFF);
        word3 = (uint16_t) (*(p + 1) & 0xFFFF);
        ptr16index = 1;
      }
      
      uint16_t Smin = (word1 & 0x3FF);
      
      digiVec[0] = ((word3 >> 4) & 0xF) + Smin;
      digiVec[1] = ((word3 >> 0) & 0xF) + Smin;
      digiVec[2] = ((word3 >> 8) & 0xF) + Smin;
      digiVec[3] = ((word3 >> 12) & 0xF) + Smin;
      digiVec[4] = ((word2 >> 4) & 0xF) + Smin;
      digiVec[5] = ((word2 >> 0) & 0xF) + Smin;
      digiVec[6] = ((word2 >> 8) & 0xF) + Smin;
      
      HWIdentifier adcID = m_tileHWID->adc_id(drawerID, channel, gain);
      TileDigits * td = new TileDigits(adcID, digiVec);
      sDigits.at(channel) = td;
      p += 1 + ptr16index;
      
      if (msgLvl(MSG::VERBOSE)) {
        msg(MSG::VERBOSE) << MSG::hex << "Frag: $" << frag << MSG::dec
        << " G:" << gain
        << " C:" << channel
        << " Data={";
        for (int samp = 0; samp < 7; ++samp) {
          msg(MSG::VERBOSE) << " " << (int) digiVec[samp];
        }
        msg(MSG::VERBOSE) << "}  " << MSG::hex
        << " WORD1: " << word1
        << " WORD2: " << word2
        << " WORD3: " << word3 << MSG::dec << endmsg;
      }
    }
    
    for (int chf2 = 0; chf2 < nbchanformat2; ++chf2) {
      if (ptr16index == 1)
        channel = ((*p) & 0x3F);
      else
        channel = ((*(p + 1) >> 16) & 0x3F);
      
      if (ptr16index == 1) {
        word1 = (uint16_t) ((*p >> 16) & 0xFFFF);
        word2 = (uint16_t) ((*p) & 0xFFFF);
        word3 = (uint16_t) ((*(p + 1) >> 16) & 0xFFFF);
        word4 = (uint16_t) (*(p + 1) & 0xFFFF);
        word5 = (uint16_t) ((*(p + 2) >> 16) & 0xFFFF);
        ptr16index = 0;
      } else if (ptr16index == 0) {
        word1 = (uint16_t) ((*p) & 0xFFFF);
        word2 = (uint16_t) ((*(p + 1) >> 16) & 0xFFFF);
        word3 = (uint16_t) (*(p + 1) & 0xFFFF);
        word4 = (uint16_t) ((*(p + 2) >> 16) & 0xFFFF);
        word5 = (uint16_t) (*(p + 2) & 0xFFFF);
        ptr16index = 1;
      }
      
      int gain = ((word2 >> 6) & 0x1);
      
      digiVec[0] = ((word1 << 9) & 0x200) + ((word2 >> 7) & 0x1FF);
      digiVec[1] = (word1 >> 1) & 0x3FF;
      digiVec[2] = (word4 << 5 & 0x3E0) + ((word1 >> 11) & 0x1F);
      digiVec[3] = (word4 >> 5) & 0x3FF;
      digiVec[4] = ((word3 << 1) & 0x3FE) + ((word4 >> 15) & 0x1);
      digiVec[5] = ((word5 << 7) & 0x380) + ((word3 >> 9) & 0x7F);
      digiVec[6] = (word5 >> 3) & 0x3FF;
      
      HWIdentifier adcID = m_tileHWID->adc_id(drawerID, channel, gain);
      TileDigits * td = new TileDigits(adcID, digiVec);
      sDigits.at(channel) = td;
      p += (2 + ptr16index);
      
      if (msgLvl(MSG::VERBOSE)) {
        msg(MSG::VERBOSE) << MSG::hex << "Frag: $" << frag << MSG::dec
        << " G:" << gain
        << " C:" << channel
        << " Data={";
        for (int samp = 0; samp < nsamp; ++samp) {
          msg(MSG::VERBOSE) << " " << (int) digiVec[samp];
        }
        msg(MSG::VERBOSE) << "}  " << MSG::hex
        << " WORD1: " << word1
        << " WORD2: " << word2
        << " WORD3: " << word3
        << " WORD4: " << word4
        << " WORD5: " << word5 << MSG::dec << endmsg;
      }
      
    }
    for (uint i = 0; i < sDigits.size(); i++) {
      TileDigits * td = sDigits.at(i);
      if (td) pDigits.push_back(td);
    }
  } //End of new version
  
}

void TileROD_Decoder::unpack_frag2(uint32_t /* version */, const uint32_t* p,
                                   pRwChVec & pChannel) const {
  // first word is frag size
  int count = *(p);
  // second word is frag ID and frag type
  int frag = *(p + 1) & 0xFFFF;
  
  //  ATH_MSG_VERBOSE( "Unpacking TileRawChannels, ID=" << frag << " size=" << count );
  
  //  ATH_MSG_VERBOSE( "first 2 words " << MSG::hex
  //                  << "  0x" << *p
  //                  << "  0x" << *(p+1)
  //                  << MSG::dec );
  
  p += 2; // 2 words so far
  int wc = m_sizeOverhead; // can be 2 or 3 words
  
  HWIdentifier drawerID = m_tileHWID->drawer_id(frag);
  TileRawChannel* rc;
  
  for (unsigned int ch = 0; ch < m_maxChannels; ++ch) {
    unsigned int w = (*p);
    
    int gain = m_rc2bytes2.gain(w);
    HWIdentifier adcID = m_tileHWID->adc_id(drawerID, ch, gain);
    
    if (w != 0) { // skip invalid channels
      rc = new TileRawChannel(adcID, m_rc2bytes2.amplitude(w), m_rc2bytes2.time(w),
                              m_rc2bytes2.quality(w));
    } else {
      rc = new TileRawChannel(adcID, 0.0, -100.0, 31);
    }
    
    pChannel.push_back(rc);
    
    //      ATH_MSG_VERBOSE(" frag 0x" << MSG::hex << frag
    //                      << MSG::dec << " ch " << ch
    //                      << " " << MSG::hex << w << MSG::dec
    //                      << " " << gain
    //                      << " " << m_rc2bytes2.amplitude(w)
    //                      << " " << m_rc2bytes2.time(w)
    //                      << " " << m_rc2bytes2.quality(w) );
    
    ++wc;
    ++p;
  }
  
  if (wc != count) {
    // check word count
    ATH_MSG_ERROR( "unpack_frag2 => Incorrect word count: "
                  << wc << " != " << count );
    assert(0);
    // return;
  }
  
  return;
}

void TileROD_Decoder::unpack_frag3(uint32_t /* version */, const uint32_t* p,
                                   pRwChVec & pChannel) const {
  // first word is frag size
  int count = *(p);
  // second word is frag ID and frag type
  int frag = *(p + 1) & 0xFFFF;
  
  //  ATH_MSG_VERBOSE( "first 4 words " << MSG::hex
  //                  << "  0x" << *p
  //                  << "  0x" << *(p+1)
  //                  << "  0x" << *(p+2)
  //                  << "  0x" << *(p+3) << MSG::dec );
  
  // followed by 2 map words
  const uint32_t* pMap = p + 2;
  
  const short* p16 = (const short *)p;
  
  p16 = p16 + 8; // 8 16bit words so far
  short wc16 = 4 + m_sizeOverhead * 2; // can be 8 or 10 (if overhead is 3 words)
  
  HWIdentifier drawerID = m_tileHWID->drawer_id(frag);
  TileRawChannel* rc;
  
  for (unsigned int ch = 0; ch < m_maxChannels; ++ch) {
    if (checkBit(pMap, ch)) {
      int gain = m_rc2bytes.gain(*p16);
      HWIdentifier adcID = m_tileHWID->adc_id(drawerID, ch, gain);
      
      rc = new TileRawChannel(adcID
                              , m_rc2bytes.amplitude(*p16)
                              , m_rc2bytes.time(*(p16 + 1))
                              , m_rc2bytes.quality(*(p16 + 2)));
      pChannel.push_back(rc);
      
      ATH_MSG_VERBOSE( " frag 0x" << MSG::hex << frag << MSG::dec
                      << " ch " << ch << " " << MSG::hex
                      << "0x" << *p16 << *(p16+1) << *(p16+2) << MSG::dec
                      << " " << gain
                      << " " << m_rc2bytes.amplitude(*p16)
                      << " " << m_rc2bytes.time(*(p16+1))
                      << " " << m_rc2bytes.quality(*(p16+2)) );
      
      wc16 = wc16 + 3;
      p16 = p16 + 3;
    } // channel present
  } // all bits, done with this frag
  
  if (wc16 % 2 == 1) {
    ++wc16; // align
    ++p16;
  }
  if (wc16 != 2 * count) {
    // check word count
    ATH_MSG_ERROR( "unpack_frag3 => Incorrect word count: "
                  << wc16 << " != 2*" << count );
    assert(0);
    // return;
  }
  
  return;
}

void TileROD_Decoder::unpack_frag4(uint32_t /* version */, const uint32_t* p,
                                   pRwChVec & pChannel) const {
  // first word is frag size
  int count = *(p);
  // second word is frag ID and frag type
  int frag = *(p + 1) & 0xFFFF;
  
  //  ATH_MSG_VERBOSE( "Unpacking TileRawChannels, ID=" << frag << " size=" << count <<s);
  
  //  ATH_MSG_VERBOSE( "first 2 words " << MSG::hex
  //                  << "  0x" << *p
  //                  << "  0x" << *(p+1) << MSG::dec );
  
  p += 2; // 2 words so far
  int wc = m_sizeOverhead; // can be 2 or 3 words
  
  HWIdentifier drawerID = m_tileHWID->drawer_id(frag);
  TileRawChannel* rc;
  uint32_t all00 = TileFragStatus::ALL_00;
  uint32_t allFF = TileFragStatus::ALL_FF;

  for (unsigned int ch = 0; ch < m_maxChannels; ++ch) {
    unsigned int w = (*p);
    
    int gain = m_rc2bytes4.gain(w);
    HWIdentifier adcID = m_tileHWID->adc_id(drawerID, ch, gain);
    
    if (allFF && w!=0xFFFFFFFF) allFF = TileFragStatus::ALL_OK;
    if (w != 0) { // skip invalid channels
      if (all00) all00 = TileFragStatus::ALL_OK;
      rc = new TileRawChannel(adcID
                              , m_rc2bytes4.amplitude(w)
                              , m_rc2bytes4.time(w)
                              , m_rc2bytes4.quality(w));
    } else {
      rc = new TileRawChannel(adcID, 0.0, -100., 31);
    }
    
    pChannel.push_back(rc);
    
    //      ATH_MSG_VERBOSE(  MSG::hex << " frag 0x" << frag << MSG::dec
    //                      << " ch " << ch
    //                      << " 0x" << MSG::hex << w << MSG::dec
    //                      << " " << gain
    //                      << " " << m_rc2bytes4.amplitude(w)
    //                      << " " << m_rc2bytes4.time(w)
    //                      << " " << m_rc2bytes4.quality(w) );
    
    ++wc;
    ++p;
  }
  
  m_rawchannelMetaData[6]->push_back( all00 | allFF );

  if (wc > count) {
    // check word count
    ATH_MSG_ERROR( "unpack_frag4 => Incorrect word count: "
                  << wc << " != " << count );
    assert(0);
    // return;
    //  } else if (wc < count) {
    //    ATH_MSG_VERBOSE("unpack_frag4 => extra " << count-wc << " words in frag" );
  }
  
  return;
}

void TileROD_Decoder::unpack_frag5(uint32_t /* version */, const uint32_t* p, pDigiVec & pDigits,
                                   pRwChVec & pChannel) {
  // first word is frag size
  int count = *(p);
  // second word is frag ID and frag type
  int frag = *(p + 1) & 0xFFFF;
  int size_L2 = (*(p + 1) >> (32 - 2 - 3)) & 0x7;
  
  // store metadata for this collection
  // size, fragID, BCID
  m_digitsMetaData[0]->push_back(count);
  m_digitsMetaData[0]->push_back(frag);
  m_digitsMetaData[0]->push_back(0);
  
  TileRawChannel2Bytes5::TileChanData ChanData[48];
  uint32_t* ptrFrag = (uint32_t*) (p - 1); // begin of fragment
  uint32_t* ptrOFW = getOFW(frag, m_rc2bytes5.getUnit()); // get OF Weights
  m_rc2bytes5.unpack(ptrOFW, ptrFrag, ChanData);
  
  int wc = m_sizeOverhead; // can be 2 or 3 words
  int bc = (size_L2 * 32 + 48) / 8; // size_L2 + BadBits
  
  HWIdentifier drawerID = m_tileHWID->drawer_id(frag);
  
  std::vector<float> digiVec(7);
  if (m_useFrag5Raw) pDigits.reserve(pDigits.size() + 48);
  if (m_useFrag5Reco) pChannel.reserve(pChannel.size() + 48);
  
  for (int ch = 0; ch < 48; ++ch) {
    int size = m_rc2bytes5.get_size_code(ChanData[ch].code);
    int gain = ChanData[ch].gain;
    HWIdentifier adcID = m_tileHWID->adc_id(drawerID, ch, gain);
    
    if (m_useFrag5Raw) {
      for (int i = 0; i < 7; ++i) digiVec[i] = ChanData[ch].s[i];
      TileDigits* td = new TileDigits(adcID, digiVec);
      pDigits.push_back(td);
    }
    
    if (m_useFrag5Reco) {
      int format = m_rc2bytes5.get_format(ChanData[ch].code);
      int quality = m_rc2bytes5.get_quality(ChanData[ch].bad, format);
      float ene = ChanData[ch].ene;
      float time = ChanData[ch].time;
      
      TileRawChannel* rc = new TileRawChannel(adcID, ene, time, quality);
      pChannel.push_back(rc);
    }
    bc += size;
  }
  
  wc += (bc + 3) / 4;
  if (wc > count) {
    // check word count
    ATH_MSG_ERROR( " unpack_frag5 => Incorrect word count: "
                  << wc << " != " << count );
    assert(0);
    // return;
  } else if (wc < count) {
    ATH_MSG_WARNING( "unpack_frag5 => extra " << count - wc
                    << " words in frag" );
    ATH_MSG_WARNING( " count = " << count
                    << " wc = " << wc
                    << " bc = " << bc
                    << " words in frag" );
  }
  return;
}

void TileROD_Decoder::unpack_frag6(uint32_t /*version*/, const uint32_t* p, pDigiVec & pDigits) const {


  int size = *(p) - m_sizeOverhead;

  // second word is frag ID (0x100-0x4ff) and frag type
  int frag = *(p + 1) & 0xFFFF;
  
  HWIdentifier adcID;
  HWIdentifier drawerID = m_tileHWID->drawer_id(frag);

  // pointer to current data word
  const uint32_t* data = p + 2;     // Position of first data word, 

  uint32_t all00 = TileFragStatus::ALL_00;
  uint32_t allFF = TileFragStatus::ALL_FF;

  for (int i = 0; i < size; ++i) {
    uint32_t w = data[i];
    if (allFF && w!=0xFFFFFFFF) {
      allFF = TileFragStatus::ALL_OK;
      if (all00 == TileFragStatus::ALL_OK) break;
    }
    if (all00 && w != 0) {
      all00 = TileFragStatus::ALL_OK;
      if (allFF == TileFragStatus::ALL_OK) break;
    }
  }

  uint32_t status = (all00 | allFF);
  m_digitsMetaData[6]->push_back( status );

  // check that fragment is not dummy
  if (m_suppressDummyFragments && status != TileFragStatus::ALL_OK) {
    ATH_MSG_WARNING("FRAG6: Suppress dummy fragment!!!");
    return; // nothing reasonable found
  }

  unsigned int moduleID[4] = {0};
  unsigned int runType[4] = {0};

  unsigned int runNumber[4] = {0};
  
  unsigned int pedestalHi[4] = {0};
  unsigned int pedestalLo[4] = {0};
  
  unsigned int chargeInjected[4] = {0};
  unsigned int timeInjected[4] = {0};
  unsigned int capacitor[4] = {0};
  
  
  uint32_t bcid[4] = {0};
  uint32_t l1id[4] = {0};


  float samples[2][48][16] = {{{0}}};

  const uint32_t* const end_data = data + size;
  while (data < end_data) {
    if ((*data & 0x00FFFF00) == 0x00BABE00) {
      unsigned int miniDrawer = *data & 0xFF;

      if ((++data < end_data) && (*data == 0x12345678) && (++data < end_data)) {

        unsigned int fragSize   = *data & 0xFF;
        unsigned int paramsSize = (*data >> 8 ) & 0xFF;
        moduleID[miniDrawer]    = (*data >> 16) & 0xFF;
        runType[miniDrawer]     = (*data >> 24) & 0xFF;

        if (fragSize != 192) {
          ATH_MSG_ERROR("FRAG6: Unpacking minidrawer [" << miniDrawer 
                        << "] with fragment size: " << fragSize 
                        << " not implemented => skip it!!!");
          continue;
        }
        
        // check trailer
        const uint32_t* trailer = data + paramsSize + 3 + fragSize; // 2 = (BCID + L1ID)

        if ((trailer + 3) <= end_data // 3 = (trailer size)
            && *trailer == 0x87654321
            && *(++trailer) == 0xBCBCBCDC
            && *(++trailer) == (0x00DEAD00 | (miniDrawer) | (miniDrawer << 24))) {

          if (paramsSize == 3){
            runNumber[miniDrawer]  = *(++data);
            
            pedestalHi[miniDrawer] = *(++data)      & 0xFFF;
            pedestalLo[miniDrawer] = (*data >> 12 ) & 0xFFF;
            
            chargeInjected[miniDrawer] = *(++data)     & 0xFFF;
            timeInjected[miniDrawer]   = (*data >> 12) & 0xFF;
            capacitor[miniDrawer]      = (*data >> 20) & 0x1;
          } else {

            ATH_MSG_WARNING("FRAG6: Unpacking minidrawer [" << miniDrawer 
                            << "] parameters with size: " << paramsSize
                            << " not implemented!!!");
            
            data += paramsSize;
          }

          bcid[miniDrawer] = (*(++data) >> 16) & 0xFFFF;
          l1id[miniDrawer] = *(++data);


          if (msgLvl(MSG::VERBOSE)) {
            msg(MSG::VERBOSE) << "FRAG6: Found MD[" << miniDrawer << "] fragment"
                              << ", Run type: " << runType[miniDrawer]
                              << ", Module ID: " << moduleID[miniDrawer]
                              << ", BCID: " << bcid[miniDrawer]
                              << ", L1ID: " << l1id[miniDrawer]  << endmsg;

            if (paramsSize == 3) {
              msg(MSG::VERBOSE) << "FRAG6: MD[" << miniDrawer << "] Parameters:"
                                << " Run number: " << runNumber[miniDrawer]
                                << ", Ppedestal Lo: " << pedestalLo[miniDrawer]
                                << ", Ppedestal Hi: " << pedestalHi[miniDrawer]
                                << ", Charge injected: " << chargeInjected[miniDrawer]
                                << ", Time injected: " << timeInjected[miniDrawer]
                                << ", Capacitor: " << capacitor[miniDrawer] << endmsg;
            }
          }


          const uint16_t* sample = (const uint16_t *) (++data);

          std::vector<int> gains = {1, 0}; // HG seems to be first
          for (int gain : gains) {
            int start_channel(miniDrawer * 12);
            int end_channel(start_channel + 12);
            for (int channel = start_channel; channel < end_channel; ++channel) {

              for (int samplesIdx = 15; samplesIdx >= 0; --samplesIdx) {
                samples[gain][channel][samplesIdx] = (*sample & 0x0FFF);
                ++sample;
              }

            }
          }

          data = ++trailer;

        } else {
          ATH_MSG_WARNING("FRAG6: Not found correct MD[" << miniDrawer << "] fragment trailer:  => skip it!!!");
        }
      }
    } else {
      ++data;
    }
  }


  pDigits.reserve(96);
  
  // always two gains
  for (int gain = 0; gain < 2; ++gain) {
    // always 48 channels
    for (int channel = 0; channel < 48; ++channel) {
      adcID = m_tileHWID->adc_id(drawerID, channel, gain);
      // always 16 samples
      std::vector<float> digiVec(&samples[gain][channel][0], &samples[gain][channel][16]);
      pDigits.push_back(new TileDigits(adcID, digiVec));
      ATH_MSG_VERBOSE("FRAG6: " << (std::string) *(pDigits.back()));
    }
  }
  
}

void TileROD_Decoder::unpack_fragA(uint32_t /* version */, const uint32_t* p,
                                   pRwChVec & /* pChannel */) const {
  // second word is frag ID and frag type
  int size = *(p);
  
  p += 2; // 2 words so far
  
  unsigned int w;
  
  // Global CRC
  w = (*p);
  m_rawchannelMetaData[0]->push_back(w & 0xFFFF);
  if (w >> 31 & 0x1) //most significant bit is 1. It means that it is a fragment containing the DSP BCID
    m_rawchannelMetaData[0]->push_back((w >> 16) & 0x7FFF); //DSP BCID
  else
    m_rawchannelMetaData[0]->push_back(0xDEAD); //0xDEAD if it is not filled
  ++p;
  
  for (int i = 0; i < (size - 4); ++i) {
    w = (*p);
    m_rawchannelMetaData[i + 1]->push_back(w & 0xFFFF);
    m_rawchannelMetaData[i + 1]->push_back(w >> 16);
    ++p;
  }
  
  return;
}

void TileROD_Decoder::unpack_fragAHLT(uint32_t /* version */, const uint32_t* p, uint16_t rob_bcid,
                                      uint16_t& mask) const {
  // first word is full frag size, including header
  int size = *(p);
  if (size < 9) return;
  
  // second word is frag ID (0x100-0x4ff) and frag type
  uint16_t frag = (*(++p)) & 0xfff;
  
  uint16_t* w = (uint16_t*) (++p); // Jump to first DQ word
  /* order of 16bit DQ words in DQ fragment
   ---------------------
   | Global CRC
   | DSP BCID
   | BCID checks
   | Mem parity err
   | Single strobe err
   | Double strobe err
   | Head format err
   | Head parity err
   | Sample format err
   | Sample parity err
   | FE chip mask err
   | ROD chip mask err
   ---------------------
   */
  if ((*w++) & 0x1) {
    mask = 0xffff;
    return;
  } // bad CRC
  mask = *w++; // BCID in DSP, can be zero (i.e. not set)
  if (mask != rob_bcid && mask > 0) {
    mask = 0xffff;
    return;
  } // BCIDs do not match - wrong event
  
  mask = *w++; // bcid checks, one bit per DMU
  if (mask & 0x0002) {
    mask = 0xffff;
    return;
  } // second DMU is bad - everything is bad
  
  if (mask & 0x00F0) { // at least one error in second motherboard, where we don't expect errors in ext.barrel
    uint16_t BCIDerr = mask;
    int n_badMB = 0;
    if (frag > 0x2FF) { // do not count non-existing DMUs in EB
      if ((frag == 0x30E) || frag == 0x411) {
        BCIDerr &= 0x3cff;
      } else {
        BCIDerr &= 0x3cfe;
      }
    }
    while (BCIDerr) {
      if (BCIDerr & 0xF) ++n_badMB;
      BCIDerr >>= 4;
    }
    if (n_badMB == 4) { // BCID errors in all 4 motherboards - assume that all DMUs are bad
      mask = 0xffff;
      return;
    }
  }
  
  mask |= *w++; // memory
  
  w += 2; // Ignore 2 guys - strobe
  uint16_t fe_mask = *w++; // head format
  fe_mask |= *w++; // head parity
  fe_mask |= *w++; // sample format
  fe_mask |= *w++; // sample parity
  
  if (fe_mask) { // something is wrong in data format - FE mask can not be used
    mask |= fe_mask;
    w++;
  } else { // real FE mask is valid only if there are no format or parity errors
    fe_mask = *w++;
    if ((frag > 0x2FF)) { // I am in extended barrel
      if ((frag == 0x30E) || frag == 0x411) fe_mask <<= 1; // shift by one DMU in EBA15 EBC18
      fe_mask = (fe_mask & 0xFF) | ((fe_mask & 0xF00) << 2); // shift upper half by two DMUs
    }
    mask |= ~fe_mask; // fe chip mask (inverted logic!)
  }
  
  mask |= ~(*w); // rod chip mask (inverted logic!)
  
  return;
}

void TileROD_Decoder::unpack_frag10(uint32_t /* version */, const uint32_t* p,
                                    TileL2Container & pL2) const {
  // MTag-MET fragment 0x10 (staging mode)
  
  // second word is frag ID and frag type
  int size = *(p);
  int frag = *(p + 1) & 0xFFFF;
  
  int nDrawer[2];
  nDrawer[0] = frag & 0x3F;
  nDrawer[1] = (frag & 0xFC0) >> 6;
  
  p += 2; // 2 words so far
  
  uint32_t w;
  uint32_t Mu_energy0;
  uint32_t Mu_energy1;
  uint32_t Mu_energy2;
  uint32_t Mu_pattern;
  uint32_t Mu_drawer;
  uint32_t Mu_quality;
  
  std::vector<std::vector<float>*> sumE;
  std::vector<std::vector<unsigned int>*> word;
  std::vector<std::vector<float>*> eta;
  std::vector<std::vector<float>*> energy0;
  std::vector<std::vector<float>*> energy1;
  std::vector<std::vector<float>*> energy2;
  std::vector<std::vector<unsigned int>*> quality;
  
  for (unsigned int i = 0; i < 2; ++i) {
    sumE.push_back(new std::vector<float>);
    word.push_back(new std::vector<unsigned int>);
    eta.push_back(new std::vector<float>);
    energy0.push_back(new std::vector<float>);
    energy1.push_back(new std::vector<float>);
    energy2.push_back(new std::vector<float>);
    quality.push_back(new std::vector<unsigned int>);
  }
  
  float eta_LB[9] = {
    ((0.00 + 2 * 0.05) / 3),  // D0, BC1, A1
    ((0.20 + 2 * 0.15) / 3),  // D1, BC2, A2
    ((0.20 + 2 * 0.25) / 3),  // D1, BC3, A3
    ((0.40 + 2 * 0.35) / 3),  // D2, BC4, A4
    ((0.40 + 2 * 0.45) / 3),  // D2, BC5, A5
    ((0.40 + 2 * 0.55) / 3),  // D2, BC6, A6
    ((0.60 + 2 * 0.55) / 3),  // D3, BC6, A6
    ((0.60 + 2 * 0.65) / 3),  // D3, BC7, A7
    ((0.60 + 2 * 0.75) / 3)   // D3, BC8, A8
  };
  
  float eta_EB[17] = {
    ((1.00 + 1.05 + 1.15) / 3), // D5, B11, A12
    ((1.00 + 1.15 + 1.15) / 3), // D5, B12, A12
    ((1.00 + 1.15 + 1.25) / 3), // D5, B12, A13
    ((1.00 + 1.25 + 1.15) / 3), // D5, B13, A12
    ((1.00 + 1.25 + 1.25) / 3), // D5, B13, A13
    ((1.00 + 1.25 + 1.35) / 3), // D5, B13, A14
    ((1.20 + 1.05 + 1.15) / 3), // D6, B11, A12
    ((1.20 + 1.15 + 1.15) / 3), // D6, B12, A12
    ((1.20 + 1.15 + 1.25) / 3), // D6, B12, A13
    ((1.20 + 1.25 + 1.15) / 3), // D6, B13, A12
    ((1.20 + 1.25 + 1.25) / 3), // D6, B13, A13
    ((1.20 + 1.25 + 1.35) / 3), // D6, B13, A14
    ((1.20 + 1.35 + 1.25) / 3), // D6, B14, A13
    ((1.20 + 1.35 + 1.35) / 3), // D6, B14, A14
    ((1.20 + 1.35 + 1.45) / 3), // D6, B14, A15
    ((1.20 + 1.45 + 1.35) / 3), // D6, B15, A14
    ((1.20 + 1.45 + 1.45) / 3)  // D6, B15, A15
  };
  
  // Transverse energy - 2 words
  sumE[0]->push_back((float) ((int32_t) (*p++) - 9000));
  sumE[1]->push_back((float) ((int32_t) (*p++) - 9000));
  
  // Muon tagging
  
  int NMuons = (int) (size - 5) / 2;
  
  for (int mu = 0; mu < NMuons; ++mu) {
    
    w = (*p);
    
    Mu_energy2 = w & 0x1FFFFFF; // 25 bits
    Mu_pattern = (w >> 25) & 0x1F; // 5 bits
    Mu_drawer = (w >> 30) & 1; // 1 bit
    Mu_quality = w >> 31; // 1 bit
    
    word[Mu_drawer]->push_back(w);
    
    w = *(p + 1);
    
    Mu_energy0 = w & 0xFFFF;    // 16 bits
    Mu_energy1 = w >> 16;       // 16 bits
    
    word[Mu_drawer]->push_back(w);
    
    // Muon eta coordinate
    switch (frag >> 12) {
      case 1:
        eta[Mu_drawer]->push_back(eta_LB[Mu_pattern]);
        break;
      case 2:
        eta[Mu_drawer]->push_back(-eta_LB[Mu_pattern]);
        break;
      case 3:
        eta[Mu_drawer]->push_back(eta_EB[Mu_pattern]);
        break;
      case 4:
        eta[Mu_drawer]->push_back(-eta_EB[Mu_pattern]);
        break;
      default:
        ATH_MSG_WARNING("Unknown fragment: " << (frag >> 8));
        break;
    }
    
    // Energy deposited in TileCal by the muon (MeV)
    energy0[Mu_drawer]->push_back(Mu_energy0 / 2.);
    energy1[Mu_drawer]->push_back(Mu_energy1 / 2.);
    energy2[Mu_drawer]->push_back(Mu_energy2 / 2.);
    
    // Muon quality factor
    quality[Mu_drawer]->push_back(Mu_quality);
    
    p += 2;
  }
  
  for (unsigned int i = 0; i < 2; ++i) {
    
    // frag ID
    int fragId = (((frag & 0xF000) >> 4) | nDrawer[i]);
    
    // store sumEt
    (*pL2[m_hashFunc(fragId)]).setEt(*sumE[i]);
    
    // store Muon data
    (*pL2[m_hashFunc(fragId)]).setMu((*(eta[i])), (*(energy0[i])), (*(energy1[i])), (*(energy2[i])),
                                     (*(quality[i])), (*(word[i])));
    
    delete sumE[i];
    delete word[i];
    delete eta[i];
    delete energy0[i];
    delete energy1[i];
    delete energy2[i];
    delete quality[i];
  }
  
  return;
}

void TileROD_Decoder::unpack_frag11(uint32_t /* version */, const uint32_t* p,
                                    TileL2Container & pL2) const {
  // MTag-MET fragment 0x11 (full mode)
  
  // second word is frag ID and frag type
  int size = *(p);
  int frag = *(p + 1) & 0xFFFF;
  
  p += 2; // 2 words so far
  
  uint32_t w;
  uint32_t Mu_energy0;
  uint32_t Mu_energy1;
  uint32_t Mu_energy2;
  uint32_t Mu_pattern;
  //uint32_t Mu_drawer;
  uint32_t Mu_quality;
  
  std::vector<unsigned int> word;
  std::vector<float> eta;
  std::vector<float> energy0;
  std::vector<float> energy1;
  std::vector<float> energy2;
  std::vector<unsigned int> quality;
  
  float eta_LB[9] = {
    ((0.00 + 2 * 0.05) / 3),  // D0, BC1, A1
    ((0.20 + 2 * 0.15) / 3),  // D1, BC2, A2
    ((0.20 + 2 * 0.25) / 3),  // D1, BC3, A3
    ((0.40 + 2 * 0.35) / 3),  // D2, BC4, A4
    ((0.40 + 2 * 0.45) / 3),  // D2, BC5, A5
    ((0.40 + 2 * 0.55) / 3),  // D2, BC6, A6
    ((0.60 + 2 * 0.55) / 3),  // D3, BC6, A6
    ((0.60 + 2 * 0.65) / 3),  // D3, BC7, A7
    ((0.60 + 2 * 0.75) / 3)   // D3, BC8, A8
  };
  
  float eta_EB[17] = {
    ((1.00 + 1.05 + 1.15) / 3), // D5, B11, A12
    ((1.00 + 1.15 + 1.15) / 3), // D5, B12, A12
    ((1.00 + 1.15 + 1.25) / 3), // D5, B12, A13
    ((1.00 + 1.25 + 1.15) / 3), // D5, B13, A12
    ((1.00 + 1.25 + 1.25) / 3), // D5, B13, A13
    ((1.00 + 1.25 + 1.35) / 3), // D5, B13, A14
    ((1.20 + 1.05 + 1.15) / 3), // D6, B11, A12
    ((1.20 + 1.15 + 1.15) / 3), // D6, B12, A12
    ((1.20 + 1.15 + 1.25) / 3), // D6, B12, A13
    ((1.20 + 1.25 + 1.15) / 3), // D6, B13, A12
    ((1.20 + 1.25 + 1.25) / 3), // D6, B13, A13
    ((1.20 + 1.25 + 1.35) / 3), // D6, B13, A14
    ((1.20 + 1.35 + 1.25) / 3), // D6, B14, A13
    ((1.20 + 1.35 + 1.35) / 3), // D6, B14, A14
    ((1.20 + 1.35 + 1.45) / 3), // D6, B14, A15
    ((1.20 + 1.45 + 1.35) / 3), // D6, B15, A14
    ((1.20 + 1.45 + 1.45) / 3)  // D6, B15, A15
  };
  
  // Transverse energy
  std::vector<float> sumE(1, (float) ((int32_t) (*p++) - 9000));
  (*pL2[m_hashFunc(frag)]).setEt(sumE);
  
  // Muon tagging
  
  int NMuons = (int) (size - 4) / 2;
  
  for (int mu = 0; mu < NMuons; ++mu) {
    
    w = (*p);
    
    Mu_energy2 = w & 0x1FFFFFF; // 25 bits
    Mu_pattern = (w >> 25) & 0x1F; // 5 bits
    //Mu_drawer = (w >> 30) & 1; // 1 bit
    Mu_quality = w >> 31; // 1 bit
    
    word.push_back(w);
    
    w = *(p + 1);
    
    Mu_energy0 = w & 0xFFFF;    // 16 bits
    Mu_energy1 = w >> 16;       // 16 bits
    
    word.push_back(w);
    
    // Muon eta coordinate
    switch (frag >> 8) {
      case 1:
        eta.push_back(eta_LB[Mu_pattern]);
        break;
      case 2:
        eta.push_back(-eta_LB[Mu_pattern]);
        break;
      case 3:
        eta.push_back(eta_EB[Mu_pattern]);
        break;
      case 4:
        eta.push_back(-eta_EB[Mu_pattern]);
        break;
      default:
        ATH_MSG_WARNING("Unknown fragment: " << (frag >> 8));
        break;
    }
    
    // Energy deposited in TileCal by the muon (MeV)
    energy0.push_back(Mu_energy0 / 2.);
    energy1.push_back(Mu_energy1 / 2.);
    energy2.push_back(Mu_energy2 / 2.);
    
    // Muon quality factor
    quality.push_back(Mu_quality);
    
    p += 2;
  }
  
  (*pL2[m_hashFunc(frag)]).setMu(eta, energy0, energy1, energy2, quality, word);
  
  return;
}

void TileROD_Decoder::unpack_frag12(uint32_t /* version */, const uint32_t* p,
                                    TileL2Container & pL2) const {
  // MTag fragment 0x12 (staging mode)
  
  // second word is frag ID and frag type
  int size = *(p);
  int frag = *(p + 1) & 0xFFFF;
  
  int nDrawer[2];
  nDrawer[0] = frag & 0x3F;
  nDrawer[1] = (frag & 0xFC0) >> 6;
  
  p += 2; // 2 words so far
  
  uint32_t w;
  uint32_t Mu_energy0;
  uint32_t Mu_energy1;
  uint32_t Mu_energy2;
  uint32_t Mu_pattern;
  uint32_t Mu_drawer;
  uint32_t Mu_quality;
  
  std::vector<std::vector<unsigned int>*> word;
  std::vector<std::vector<float>*> eta;
  std::vector<std::vector<float>*> energy0;
  std::vector<std::vector<float>*> energy1;
  std::vector<std::vector<float>*> energy2;
  std::vector<std::vector<unsigned int>*> quality;
  
  for (unsigned int i = 0; i < 2; ++i) {
    word.push_back(new std::vector<unsigned int>);
    eta.push_back(new std::vector<float>);
    energy0.push_back(new std::vector<float>);
    energy1.push_back(new std::vector<float>);
    energy2.push_back(new std::vector<float>);
    quality.push_back(new std::vector<unsigned int>);
  }
  
  float eta_LB[9] = {
    ((0.00 + 2 * 0.05) / 3),  // D0, BC1, A1
    ((0.20 + 2 * 0.15) / 3),  // D1, BC2, A2
    ((0.20 + 2 * 0.25) / 3),  // D1, BC3, A3
    ((0.40 + 2 * 0.35) / 3),  // D2, BC4, A4
    ((0.40 + 2 * 0.45) / 3),  // D2, BC5, A5
    ((0.40 + 2 * 0.55) / 3),  // D2, BC6, A6
    ((0.60 + 2 * 0.55) / 3),  // D3, BC6, A6
    ((0.60 + 2 * 0.65) / 3),  // D3, BC7, A7
    ((0.60 + 2 * 0.75) / 3)   // D3, BC8, A8
  };
  
  float eta_EB[17] = {
    ((1.00 + 1.05 + 1.15) / 3), // D5, B11, A12
    ((1.00 + 1.15 + 1.15) / 3), // D5, B12, A12
    ((1.00 + 1.15 + 1.25) / 3), // D5, B12, A13
    ((1.00 + 1.25 + 1.15) / 3), // D5, B13, A12
    ((1.00 + 1.25 + 1.25) / 3), // D5, B13, A13
    ((1.00 + 1.25 + 1.35) / 3), // D5, B13, A14
    ((1.20 + 1.05 + 1.15) / 3), // D6, B11, A12
    ((1.20 + 1.15 + 1.15) / 3), // D6, B12, A12
    ((1.20 + 1.15 + 1.25) / 3), // D6, B12, A13
    ((1.20 + 1.25 + 1.15) / 3), // D6, B13, A12
    ((1.20 + 1.25 + 1.25) / 3), // D6, B13, A13
    ((1.20 + 1.25 + 1.35) / 3), // D6, B13, A14
    ((1.20 + 1.35 + 1.25) / 3), // D6, B14, A13
    ((1.20 + 1.35 + 1.35) / 3), // D6, B14, A14
    ((1.20 + 1.35 + 1.45) / 3), // D6, B14, A15
    ((1.20 + 1.45 + 1.35) / 3), // D6, B15, A14
    ((1.20 + 1.45 + 1.45) / 3)  // D6, B15, A15
  };
  
  int NMuons = (int) (size - 3) / 2;
  
  for (int mu = 0; mu < NMuons; ++mu) {
    
    w = (*p);
    
    Mu_energy2 = w & 0x1FFFFFF; // 25 bits
    Mu_pattern = (w >> 25) & 0x1F; // 5 bits
    Mu_drawer = (w >> 30) & 1; // 1 bit
    Mu_quality = w >> 31; // 1 bit
    
    word[Mu_drawer]->push_back(w);
    
    w = *(p + 1);
    
    Mu_energy0 = w & 0xFFFF;    // 16 bits
    Mu_energy1 = w >> 16;       // 16 bits
    
    word[Mu_drawer]->push_back(w);
    
    // Muon eta coordinate
    switch (frag >> 12) {
      case 1:
        eta[Mu_drawer]->push_back(eta_LB[Mu_pattern]);
        break;
      case 2:
        eta[Mu_drawer]->push_back(-eta_LB[Mu_pattern]);
        break;
      case 3:
        eta[Mu_drawer]->push_back(eta_EB[Mu_pattern]);
        break;
      case 4:
        eta[Mu_drawer]->push_back(-eta_EB[Mu_pattern]);
        break;
      default:
        ATH_MSG_WARNING("Unknown fragment: " << (frag >> 8));
        break;
    }
    
    // Energy deposited in TileCal by the muon (MeV)
    energy0[Mu_drawer]->push_back(Mu_energy0 / 2.);
    energy1[Mu_drawer]->push_back(Mu_energy1 / 2.);
    energy2[Mu_drawer]->push_back(Mu_energy2 / 2.);
    
    // Muon quality factor
    quality[Mu_drawer]->push_back(Mu_quality);
    
    p += 2;
  }
  
  for (unsigned int i = 0; i < 2; ++i) {
    
    // frag ID
    int fragId = (((frag & 0xF000) >> 4) | nDrawer[i]);
    
    (*pL2[m_hashFunc(fragId)]).setMu((*(eta[i])), (*(energy0[i])), (*(energy1[i])), (*(energy2[i])),
                                     (*(quality[i])), (*(word[i])));
    
    delete word[i];
    delete eta[i];
    delete energy0[i];
    delete energy1[i];
    delete energy2[i];
    delete quality[i];
  }
  
  return;
}

void TileROD_Decoder::unpack_frag13(uint32_t /* version */, const uint32_t* p,
                                    TileL2Container & pL2) const {
  // MTag fragment 0x13 (full mode)
  
  // second word is frag ID and frag type
  int size = *(p);
  int frag = *(p + 1) & 0xFFFF;
  
  p += 2; // 2 words so far
  
  uint32_t w;
  uint32_t Mu_energy0;
  uint32_t Mu_energy1;
  uint32_t Mu_energy2;
  uint32_t Mu_pattern;
  //uint32_t Mu_drawer;
  uint32_t Mu_quality;
  
  std::vector<unsigned int> word;
  std::vector<float> eta;
  std::vector<float> energy0;
  std::vector<float> energy1;
  std::vector<float> energy2;
  std::vector<unsigned int> quality;
  
  float eta_LB[9] = {
    ((0.00 + 2 * 0.05) / 3),  // D0, BC1, A1
    ((0.20 + 2 * 0.15) / 3),  // D1, BC2, A2
    ((0.20 + 2 * 0.25) / 3),  // D1, BC3, A3
    ((0.40 + 2 * 0.35) / 3),  // D2, BC4, A4
    ((0.40 + 2 * 0.45) / 3),  // D2, BC5, A5
    ((0.40 + 2 * 0.55) / 3),  // D2, BC6, A6
    ((0.60 + 2 * 0.55) / 3),  // D3, BC6, A6
    ((0.60 + 2 * 0.65) / 3),  // D3, BC7, A7
    ((0.60 + 2 * 0.75) / 3)   // D3, BC8, A8
  };
  
  float eta_EB[17] = {
    ((1.00 + 1.05 + 1.15) / 3), // D5, B11, A12
    ((1.00 + 1.15 + 1.15) / 3), // D5, B12, A12
    ((1.00 + 1.15 + 1.25) / 3), // D5, B12, A13
    ((1.00 + 1.25 + 1.15) / 3), // D5, B13, A12
    ((1.00 + 1.25 + 1.25) / 3), // D5, B13, A13
    ((1.00 + 1.25 + 1.35) / 3), // D5, B13, A14
    ((1.20 + 1.05 + 1.15) / 3), // D6, B11, A12
    ((1.20 + 1.15 + 1.15) / 3), // D6, B12, A12
    ((1.20 + 1.15 + 1.25) / 3), // D6, B12, A13
    ((1.20 + 1.25 + 1.15) / 3), // D6, B13, A12
    ((1.20 + 1.25 + 1.25) / 3), // D6, B13, A13
    ((1.20 + 1.25 + 1.35) / 3), // D6, B13, A14
    ((1.20 + 1.35 + 1.25) / 3), // D6, B14, A13
    ((1.20 + 1.35 + 1.35) / 3), // D6, B14, A14
    ((1.20 + 1.35 + 1.45) / 3), // D6, B14, A15
    ((1.20 + 1.45 + 1.35) / 3), // D6, B15, A14
    ((1.20 + 1.45 + 1.45) / 3)  // D6, B15, A15
  };
  
  int NMuons = (int) (size - 3) / 2;
  
  for (int mu = 0; mu < NMuons; ++mu) {
    
    w = (*p);
    
    Mu_energy2 = w & 0x1FFFFFF; // 25 bits
    Mu_pattern = (w >> 25) & 0x1F; // 5 bits
    //Mu_drawer = (w >> 30) & 1; // 1 bit
    Mu_quality = w >> 31; // 1 bit
    
    word.push_back(w);
    
    w = *(p + 1);
    
    Mu_energy0 = w & 0xFFFF;    // 16 bits
    Mu_energy1 = w >> 16;       // 16 bits
    
    word.push_back(w);
    
    // Muon eta coordinate
    switch (frag >> 8) {
      case 1:
        eta.push_back(eta_LB[Mu_pattern]);
        break;
      case 2:
        eta.push_back(-eta_LB[Mu_pattern]);
        break;
      case 3:
        eta.push_back(eta_EB[Mu_pattern]);
        break;
      case 4:
        eta.push_back(-eta_EB[Mu_pattern]);
        break;
      default:
        ATH_MSG_WARNING("Unknown fragment: " << (frag >> 8));
        break;
    }
    
    // Energy deposited in TileCal by the muon (MeV)
    energy0.push_back(Mu_energy0 / 2.);
    energy1.push_back(Mu_energy1 / 2.);
    energy2.push_back(Mu_energy2 / 2.);
    
    // Muon quality factor
    quality.push_back(Mu_quality);
    
    p += 2;
  }
  
  (*pL2[m_hashFunc(frag)]).setMu(eta, energy0, energy1, energy2, quality, word);
  
  return;
}

void TileROD_Decoder::unpack_frag14(uint32_t /* version */, const uint32_t* p,
                                    TileL2Container & pL2) const {
  // Met fragment 0x14 (staging mode) - obsolete, now sumEt is part of frag4/frag5
  
  // second word is frag ID and frag type
  int frag = *(p + 1) & 0xFFFF;
  
  int nDrawer[2];
  nDrawer[0] = frag & 0x3F;
  nDrawer[1] = (frag & 0xFC0) >> 6;
  
  p += 2; // 2 words so far
  
  std::vector<float> sumE(1);
  
  for (unsigned int i = 0; i < 2; ++i) {
    
    int fragId = (((frag & 0xF000) >> 4) | nDrawer[i]);
    
    sumE[0] = (float) ((int32_t) (*p) - 9000);
    (*pL2[m_hashFunc(fragId)]).setEt(sumE);
    
    ++p;
  }
  
  return;
}

void TileROD_Decoder::unpack_frag15(uint32_t /* version */, const uint32_t* p,
                                    TileL2Container & pL2) const {
  // Met fragment 0x15 (full mode) - obsolete, now sumEt is part of frag4/frag5
  
  // second word is frag ID and frag type
  int frag = *(p + 1) & 0xFFFF;
  
  p += 2; // 2 words so far
  
  std::vector<float> sumE(1);
  
  sumE[0] = (float) ((int32_t) (*p) - 9000);
  (*pL2[m_hashFunc(frag)]).setEt(sumE);
  
  return;
}

void TileROD_Decoder::unpack_frag16(uint32_t version, const uint32_t* p,
                                    TileLaserObject & laserObject) {
  
  //frag ID
  
  //int frag = *(p+1) & 0xFFFF;
  //int type = *(p+1) >>16;
  
  // pointer to current data word
  const uint32_t *pData;
  
  //position of first data word, skip the first two words of the header fragment
  
  pData = p + 2;
  
  int counter = 0;
  
  counter = *pData;
  
  ++pData;
  
  int reqAmp = 0;
  int filt = 0;
  //int rawfilt=0;
  //int status=0;
  bool errorFlag = false;
  
  if ((*pData & 0xFF000000) == 0x20000000) {
    
    if (version == 1) {
      
      reqAmp = *pData & 0xFFFF;
      
    } else {
      
      reqAmp = *pData & 0xFFFF;
      
      //     rawfilt = (*pData >>16) & 0xF;
      
      if (version == 2) {
        filt = (((*pData >> 16) & 7) ^ 7) + 2;
      } else {
        filt = 9 - (((*pData >> 17) & 1) * 4 + ((*pData >> 18) & 1) * 2 + ((*pData >> 19) & 1));
      }
      
      if (((*pData >> 16) & 1) == 1) filt = 0; // Filter wheel moving
      
      if (filt > 8) filt -= 8;
    }
    //status = (*pData >>20) & 0xF;
    
  } else {
    errorFlag = true;
    if (m_ErrorCounter <= m_maxErrorPrint)
      ATH_MSG_ERROR( "In decoding word 20" );
  }
  
  ++pData;
  
  //int delay=0;
  int measAmp = 0;
  
  if ((*pData & 0xFF000000) == 0x21000000) {
    
    measAmp = *pData & 0xFFF;
    //delay = (*pData >>12) & 0xFFF;
    
  } else {
    errorFlag = true;
    if (m_ErrorCounter <= m_maxErrorPrint)
      ATH_MSG_ERROR( "In decoding word 21" );
  }
  
  ++pData;
  
  //int status1=0;
  int tdc1 = 0;
  int tdc2 = 0;
  
  if ((*pData & 0xFF000000) == 0x22000000) {
    
    if (version == 1) {
      
      tdc1 = (*pData >> 16) & 0xF;
      tdc2 = (*pData >> 20) & 0xF;
    } else {
      
      tdc1 = *pData & 0xFFFF;
      
    }
    
  } else {
    errorFlag = true;
    if (m_ErrorCounter <= m_maxErrorPrint)
      ATH_MSG_ERROR( "In decoding word 22" );
    
  }
  
  ++pData;
  
  //int status2=0;
  //int tdc2=0;
  
  if ((*pData & 0xFF000000) == 0x23000000) {
    
    if (version == 1) {
      
      tdc2 = (*pData >> 12) & 0xFFF;
      tdc1 = *pData & 0xFFF;
      
    } else {
      
      tdc2 = *pData & 0xFFFF;
      
      //status2 = (*pData >>16) & 0xFF;
    }
    
  } else {
    errorFlag = true;
    if (m_ErrorCounter <= m_maxErrorPrint)
      ATH_MSG_ERROR( "In decoding word 23" );
  }
  
  ++pData;
  
  int chan0 = 0;
  int chan1 = 0;
  
  if ((*pData & 0xFF000000) == 0x44000000) {
    
    chan0 = 4095 - (*pData & 0xFFF);
    chan1 = 4095 - ((*pData >> 12) & 0xFFF);
    
  } else {
    errorFlag = true;
    if (m_ErrorCounter <= m_maxErrorPrint)
      ATH_MSG_ERROR( "In decoding word 24" );
  }
  
  ++pData;
  
  int chan2 = 0;
  int chan3 = 0;
  
  if ((*pData & 0xFF000000) == 0x45000000) {
    
    chan2 = 4095 - (*pData & 0xFFF);
    chan3 = 4095 - ((*pData >> 12) & 0xFFF);
    
  } else {
    errorFlag = true;
    if (m_ErrorCounter <= m_maxErrorPrint)
      ATH_MSG_ERROR( "In decoding word 25" );
  }
  
  ++pData;
  
  int chan4 = 0;
  int chan5 = 0;
  
  if ((*pData & 0xFF000000) == 0x46000000) {
    
    chan4 = 4095 - (*pData & 0xFFF);
    chan5 = 4095 - ((*pData >> 12) & 0xFFF);
    
  } else {
    errorFlag = true;
    if (m_ErrorCounter <= m_maxErrorPrint)
      ATH_MSG_ERROR( "In decoding word 26" );
  }
  
  ++pData;
  
  //int chan6=0;
  //int chan7=0;
  
  if ((*pData & 0xFF000000) == 0x47000000) {
    
    //chan6 = 4095 - (*pData & 0xFFF);
    //chan7 = 4095 - ((*pData >>12) & 0xFFF);
    
  } else {
    errorFlag = true;
    if (m_ErrorCounter <= m_maxErrorPrint)
      ATH_MSG_ERROR( "In decoding word 27" );
  }
  
  ++pData;
  
  int meanPed_diode1 = 0;
  int rmsPed_diode1 = 0;
  
  double MeanPed_diode1 = 0;
  double RmsPed_diode1 = 0;
  
  rmsPed_diode1 = *pData & 0xFFFF;
  meanPed_diode1 = (*pData >> 16) & 0xFFFF;
  
  MeanPed_diode1 = static_cast<double>(meanPed_diode1) * 0.1;
  
  RmsPed_diode1 = static_cast<double>(rmsPed_diode1) * 0.01;
  
  ++pData;
  
  int meanPed_diode2 = 0;
  int rmsPed_diode2 = 0;
  
  double MeanPed_diode2 = 0;
  double RmsPed_diode2 = 0;
  
  rmsPed_diode2 = *pData & 0xFFFF;
  meanPed_diode2 = (*pData >> 16) & 0xFFFF;
  
  MeanPed_diode2 = static_cast<double>(meanPed_diode2) * 0.1;
  
  RmsPed_diode2 = static_cast<double>(rmsPed_diode2) * 0.01;
  
  ++pData;
  
  int meanPed_diode3 = 0;
  int rmsPed_diode3 = 0;
  
  double MeanPed_diode3 = 0;
  double RmsPed_diode3 = 0;
  
  rmsPed_diode3 = *pData & 0xFFFF;
  meanPed_diode3 = (*pData >> 16) & 0xFFFF;
  
  MeanPed_diode3 = static_cast<double>(meanPed_diode3) * 0.1;
  
  RmsPed_diode3 = static_cast<double>(rmsPed_diode3) * 0.01;
  
  ++pData;
  
  int meanPed_diode4 = 0;
  int rmsPed_diode4 = 0;
  
  double MeanPed_diode4 = 0;
  double RmsPed_diode4 = 0;
  
  rmsPed_diode4 = *pData & 0xFFFF;
  meanPed_diode4 = (*pData >> 16) & 0xFFFF;
  
  MeanPed_diode4 = static_cast<double>(meanPed_diode4) * 0.1;
  
  RmsPed_diode4 = static_cast<double>(rmsPed_diode4) * 0.01;
  
  ++pData;
  
  int meanPed_pmt1 = 0;
  int rmsPed_pmt1 = 0;
  
  double MeanPed_pmt1 = 0;
  double RmsPed_pmt1 = 0;
  
  rmsPed_pmt1 = *pData & 0xFFFF;
  meanPed_pmt1 = (*pData >> 16) & 0xFFFF;
  
  MeanPed_pmt1 = static_cast<double>(meanPed_pmt1) * 0.1;
  
  RmsPed_pmt1 = static_cast<double>(rmsPed_pmt1) * 0.01;
  
  ++pData;
  
  int meanPed_pmt2 = 0;
  int rmsPed_pmt2 = 0;
  
  double MeanPed_pmt2 = 0;
  double RmsPed_pmt2 = 0;
  
  rmsPed_pmt2 = *pData & 0xFFFF;
  meanPed_pmt2 = (*pData >> 16) & 0xFFFF;
  
  MeanPed_pmt2 = static_cast<double>(meanPed_pmt2) * 0.1;
  
  RmsPed_pmt2 = static_cast<double>(rmsPed_pmt2) * 0.01;
  
  ++pData;
  
  time_t lastPedMeas = *pData;
  //struct tm *time = localtime(&lastPedMeas);
  //printf("Date is %d/%02d/%02d\n", time->tm_year+1900, time->tm_mon+1, time->tm_mday);
  //printf("Time is %02d:%02d\n", time->tm_hour, time->tm_min);
  
  ++pData;
  
  int meanAlpha_diode1 = 0;
  int rmsAlpha_diode1 = 0;
  
  double MeanAlpha_diode1 = 0;
  double RmsAlpha_diode1 = 0;
  
  rmsAlpha_diode1 = *pData & 0xFFFF;
  meanAlpha_diode1 = (*pData >> 16) & 0xFFFF;
  
  MeanAlpha_diode1 = static_cast<double>(meanAlpha_diode1) * 0.1;
  
  RmsAlpha_diode1 = static_cast<double>(rmsAlpha_diode1) * 0.01;
  
  ++pData;
  
  int meanAlpha_diode2 = 0;
  int rmsAlpha_diode2 = 0;
  
  double MeanAlpha_diode2 = 0;
  double RmsAlpha_diode2 = 0;
  
  rmsAlpha_diode2 = *pData & 0xFFFF;
  meanAlpha_diode2 = (*pData >> 16) & 0xFFFF;
  
  MeanAlpha_diode2 = static_cast<float>(meanAlpha_diode2) * 0.1;
  
  RmsAlpha_diode2 = static_cast<float>(rmsAlpha_diode2) * 0.01;
  
  ++pData;
  
  //  ATH_MSG_VERBOSE( "Diode 2 value is " << MeanAlpha_diode2 );
  
  int meanAlpha_diode3 = 0;
  int rmsAlpha_diode3 = 0;
  
  double MeanAlpha_diode3 = 0;
  double RmsAlpha_diode3 = 0;
  
  rmsAlpha_diode3 = *pData & 0xFFFF;
  meanAlpha_diode3 = (*pData >> 16) & 0xFFFF;
  
  MeanAlpha_diode3 = static_cast<double>(meanAlpha_diode3) * 0.1;
  
  RmsAlpha_diode3 = static_cast<double>(rmsAlpha_diode3) * 0.01;
  
  ++pData;
  
  int meanAlpha_diode4 = 0;
  int rmsAlpha_diode4 = 0;
  
  double MeanAlpha_diode4 = 0;
  double RmsAlpha_diode4 = 0;
  
  rmsAlpha_diode4 = *pData & 0xFFFF;
  meanAlpha_diode4 = (*pData >> 16) & 0xFFFF;
  
  MeanAlpha_diode4 = static_cast<double>(meanAlpha_diode4) * 0.1;
  
  RmsAlpha_diode4 = static_cast<double>(rmsAlpha_diode4) * 0.01;
  
  ++pData;
  
  time_t lastAlphaMeas = *pData;
  //tm *Time = localtime(&lastAlphaMeas);
  //printf("Date is %d/%02d/%02d\n", Time->tm_year+1900, Time->tm_mon+1, Time->tm_mday);
  //printf("Time is %02d:%02d\n", Time->tm_hour, Time->tm_min);
  
  ++pData;
  
  if (version > 1) {
    
    //int pedAlpha_diode1=0;
    //int pedRMSAlpha_diode1=0;
    
    //double PedAlpha_diode1=0;
    //double PedRMSAlpha_diode1=0;
    
    //pedRMSAlpha_diode1 = *pData & 0xFFFF ;
    //pedAlpha_diode1 = (*pData >> 16) & 0xFFFF;
    
    //PedAlpha_diode1 = static_cast<double>(pedAlpha_diode1)/10;
    
    //PedRMSAlpha_diode1 = static_cast<double>(pedRMSAlpha_diode1)/100;
    
    ++pData;
    
    //int pedAlpha_diode2=0;
    //int pedRMSAlpha_diode2=0;
    
    //double PedAlpha_diode2=0;
    //double PedRMSAlpha_diode2=0;
    
    //pedRMSAlpha_diode2 = *pData & 0xFFFF ;
    //pedAlpha_diode2 = (*pData >> 16) & 0xFFFF;
    
    //PedAlpha_diode2 = static_cast<double>(pedAlpha_diode2)/10;
    
    //PedRMSAlpha_diode2 = static_cast<double>(pedRMSAlpha_diode2)/100;
    
    ++pData;
    
    //int pedAlpha_diode3=0;
    //int pedRMSAlpha_diode3=0;
    
    //double PedAlpha_diode3=0;
    //double PedRMSAlpha_diode3=0;
    
    //pedRMSAlpha_diode3 = *pData & 0xFFFF ;
    //pedAlpha_diode3 = (*pData >> 16) & 0xFFFF;
    
    //PedAlpha_diode3 = static_cast<double>(pedAlpha_diode3)/10;
    
    //PedRMSAlpha_diode3 = static_cast<double>(pedRMSAlpha_diode3)/100;
    
    ++pData;
    
    //int pedAlpha_diode4=0;
    //int pedRMSAlpha_diode4=0;
    
    //double PedAlpha_diode4=0;
    //double PedRMSAlpha_diode4=0;
    
    //pedRMSAlpha_diode4 = *pData & 0xFFFF ;
    //pedAlpha_diode4 = (*pData >> 16) & 0xFFFF;
    
    //PedAlpha_diode4 = static_cast<double>(pedAlpha_diode4)/10;
    
    //PedRMSAlpha_diode4 = static_cast<double>(pedRMSAlpha_diode4)/100;
    
    ++pData;
    
    //time_t lastAlphaPedMeas = *pData;
    
    ++pData;
  }
  
  int diodeTemp = 0;
  int seconds1 = 0;
  
  double DiodeTemp = 0;
  
  diodeTemp = *pData & 0xFFF;
  seconds1 = (*pData >> 12) & 0xFFFFF;
  
  DiodeTemp = static_cast<double>(diodeTemp) * 0.1;
  
  ++pData;
  
  int boxTemp = 0;
  int seconds2 = 0;
  
  double BoxTemp = 0;
  
  boxTemp = *pData & 0xFFF;
  seconds2 = (*pData >> 12) & 0xFFFFF;
  
  BoxTemp = static_cast<double>(boxTemp) * 0.1;
  
  ++pData;
  
  int hum = 0;
  int seconds3 = 0;
  
  double Hum = 0;
  
  hum = *pData & 0xFFF;
  seconds3 = (*pData >> 12) & 0xFFFFF;
  
  Hum = static_cast<double>(hum) * 0.1;
  
  ++pData;
  
  int gasFlow = 0;
  int seconds4 = 0;
  
  double GasFlow = 0;
  
  gasFlow = *pData & 0xFFF;
  seconds4 = (*pData >> 12) & 0xFFFFF;
  
  GasFlow = static_cast<double>(gasFlow) * 0.1;
  
  ++pData;
  
  int PLCstatus = 0;
  //int seconds5 =0;
  
  PLCstatus = *pData & 0xFFF;
  //seconds5 = (*pData >> 12) & 0xFFFFF;
  int alphaPos = PLCstatus & 0x7;
  int LVdiodes = (PLCstatus >> 0x3) & 0x1;
  int HVpmts = (PLCstatus >> 0x4) & 0x3;
  int shutter = (PLCstatus >> 0x6) & 0x3;
  int interLock = (PLCstatus >> 0x8) & 0x1;
  int alarm = (PLCstatus >> 0x9) & 0x7;
  
  laserObject.setPmt(0, chan4, tdc1, MeanPed_pmt1, RmsPed_pmt1, 0);
  
  laserObject.setPmt(1, chan5, tdc2, MeanPed_pmt2, RmsPed_pmt2, 0);
  
  laserObject.setDiode(0, chan0, MeanPed_diode1, RmsPed_diode1, MeanAlpha_diode1, RmsAlpha_diode1, 0, 0, 0);
  
  laserObject.setDiode(1, chan1, MeanPed_diode2, RmsPed_diode2, MeanAlpha_diode2, RmsAlpha_diode2, 0, 0, 0);
  
  laserObject.setDiode(2, chan2, MeanPed_diode3, RmsPed_diode3, MeanAlpha_diode3, RmsAlpha_diode3, 0, 0, 0);
  
  laserObject.setDiode(3, chan3, MeanPed_diode4, RmsPed_diode4, MeanAlpha_diode4, RmsAlpha_diode4, 0, 0, 0);
  
  laserObject.setControl(DiodeTemp, seconds1, BoxTemp, seconds2, GasFlow, seconds4, Hum, seconds3, lastPedMeas, lastAlphaMeas);
  
  laserObject.setLaser(counter, reqAmp, measAmp, filt, 0, 1);
  
  laserObject.setPLC(alphaPos, LVdiodes, HVpmts, shutter, interLock, alarm);
  
  if (errorFlag) m_ErrorCounter++;
} // unpack_frag16

void TileROD_Decoder::unpack_frag17(uint32_t /* version */, const uint32_t* p,
                                    TileLaserObject & laserObject) {
  
  // first word is full frag size, first two words are not data
  int size = *(p);
  // second word is frag ID and frag type
  //int frag = *(p + 1) & 0xFFFF;
  //position of first data word, skip the first two words of the header fragment
  //const uint32_t *pData = p+2;
  
  bool debug = msgLvl(MSG::DEBUG);
  
  if (true) {
    int maxn=size+2-m_sizeOverhead;
    for(int n=0; n<maxn; ++n){
      ATH_MSG_DEBUG("WORD " << n << " (" << n+11 << ") : (DEC) " << p[n] << "  (HEX) 0x" << MSG::hex << p[n] << MSG::dec );
    }
  }
  
  // p[2]    00 00 00 tt    Daq Type
  // p[3]    nn nn nn nn    Laser Count
  // p[4]    rr rr mm mm    rrrr = Requested Intensity    mmmm = measured intensity
  // p[5]    00 0f dd dd    f = filter    dddd = Delay Slama
  // [[6]    00 00 ll ll    Linearity DAC Value
  laserObject.setLaser(p[3], (p[4]>>16), (p[4] & 0xFFFF), (p[5]>>16) & 0x000F, (p[5] & 0xFFFF), 2);
  laserObject.setControl(-99,-99,-99,-99,-99,-99,-99,-99,-99,-99);
  laserObject.setPLC(-99,-99,-99,-99,-99,-99);
  laserObject.setDaqType(int(p[2]));
  
  ATH_MSG_DEBUG("SETTING DAQ TYPE IN DECODER = " << MSG::hex << "0x" << int(p[2]) << "  " << MSG::dec << int(p[2]));
  
  int countr = p[3];
  int idiode = (p[4]>>16);
  int filter = (p[5]>>16) & 0x000F;
  int timing = (p[5] & 0xFFFF);
  int daqtyp = p[2];
  
  if(laserObject.isLASERII()) ATH_MSG_DEBUG("LASERII VERSION IS " << laserObject.getVersion());
  else                        ATH_MSG_DEBUG("LASERI VERSION IS "  << laserObject.getVersion());
  
  ATH_MSG_DEBUG("laserObject.setLaser: " << MSG::dec << "COUNTER=" << countr << " | "
                << "I_DIODE=" << idiode << " | "
                << "FILTERN=" << filter << " | "
                << "TIMINGD=" << timing << " | "
                << "DAQTYPE=" << daqtyp );
  
  int las[32];
  double ped[32];
  double std[32];
  
  // ATTENTION: note that HG has even index (0,2,4,...) in las[], ped[], std[] arrays
  static const int HG=0;
  static const int LG=1;
  
  const uint32_t *word = p+7; // move to 7th word
  int adc=0;
  // decode 32 ADC half-words (16 low & high channels)
  while (adc<31) {
    // ll ll hh hh    ADC Channel 1 & 0 (Low & High Gain)
    las[adc++] = 8500 - ((*word) & 0xFFFF);
    las[adc++] = 8500 - ((*word) >> 16);
    ++word;
  }
  // TDC word immediately after ADC words
  // aa aa bb bb    aaaa = TDC N1    bbbb = TDC N0
  int TDC0 = (*word) & 0xFFFF;
  int TDC1 = (*word) >> 16;
  
  if (size>130) {
    
    uint32_t sum[32];
    uint64_t ssq[32];
    
    // p[128] Nb event
    double nevt = double(p[128]);
    if(p[128]==0 || (p[128]==3072 && p[32]<21504000) ) {
      nevt=1024.; // HAS TO BE REMOVED!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      ATH_MSG_DEBUG("Nevents= " << p[128] << " => assuming 1024");
    }
    
    if (nevt>0) {
      
      word = p+32; // jump to 32th word
      
      // decode sums for 16 channels (6 words per channel)
      for(int channel=0;channel<16;++channel) {
        
        // Sum Xi  channel 0
        // Sum Xi  channel 1
        // Sum Xi2  LSB channel 0
        // Sum Xi2  MSB channel 0
        // Sum Xi2  LSB channel 1
        // Sum Xi2  MSB channel 1
        
        sum[channel*2+HG] = *(word++);
        sum[channel*2+LG] = *(word++);
        
        uint32_t lsb0 = *(word++);
        uint32_t msb0 = *(word++);
        uint32_t lsb1 = *(word++);
        uint32_t msb1 = *(word++);
        uint64_t MSB0 = (uint64_t) msb0 << 32;
        uint64_t MSB1 = (uint64_t) msb1 << 32;
        uint64_t SSQ0 = MSB0 | lsb0;
        uint64_t SSQ1 = MSB1 | lsb1;
        
        ssq[channel*2+HG] = SSQ0;
        ssq[channel*2+LG] = SSQ1;
        
        // COMPUTE MEAN AND STANDARD DEVIATION
        ped[channel*2+HG] = double(sum[channel*2+HG])/nevt;
        ped[channel*2+LG] = double(sum[channel*2+LG])/nevt;
        
        std[channel*2+HG] = double(ssq[channel*2+HG])/nevt - ped[channel*2+HG]*ped[channel*2+HG];
        std[channel*2+LG] = double(ssq[channel*2+LG])/nevt - ped[channel*2+LG]*ped[channel*2+LG];
        if (std[channel*2+HG]>0.0) std[channel*2+HG] = sqrt(std[channel*2+HG]);
        if (std[channel*2+LG]>0.0) std[channel*2+LG] = sqrt(std[channel*2+LG]);
        
        // PLEASE NOTE THAT   LG EQUALS 1  WHEN  TILEID::LOWGAIN  EQUALS 0
        //                    HG EQUALS 0  WHEN  TILEID::HIGHGAIN EQUALS 1
        // THIS CONFUSING INDEXING IS TAKEN CARE OF IN THE FOLLOWING TWO LINES AND WILL *NOT*
        // BE PROPAGATED FURTHER DOWN THE CHAIN. THIS MEANS THAT AFTER THIS LG=0 AND HG=1
	laserObject.setCalibType(int(p[31]));
        laserObject.setCalib(channel,int(p[31]), double(sum[channel*2+LG]), double(ssq[channel*2+LG]), nevt, TileID::LOWGAIN);
        laserObject.setCalib(channel,int(p[31]), double(sum[channel*2+HG]), double(ssq[channel*2+HG]), nevt, TileID::HIGHGAIN);
        
        // DEBUGGING OUTPUT
        if(debug){
          ATH_MSG_DEBUG("PED CHAN=" << channel << " GAIN=LG" << " TYPE=" << laserObject.getType(channel,TileID::LOWGAIN,0) << " MEAN=" << laserObject.getMean(channel,TileID::LOWGAIN,0) << " SIGMA=" << laserObject.getSigma(channel,TileID::LOWGAIN,0) << " N=" << laserObject.getN(channel,TileID::LOWGAIN,0) << " isSet?=" << laserObject.isSet(channel,TileID::LOWGAIN,0) );
          ATH_MSG_DEBUG("PED CHAN=" << channel << " GAIN=HG" << " TYPE=" << laserObject.getType(channel,TileID::HIGHGAIN,0) << " MEAN=" << laserObject.getMean(channel,TileID::HIGHGAIN,0) << " SIGMA=" << laserObject.getSigma(channel,TileID::HIGHGAIN,0) << " N=" << laserObject.getN(channel,TileID::HIGHGAIN,0) << " isSet?=" << laserObject.isSet(channel,TileID::HIGHGAIN,0) );
          
          ATH_MSG_DEBUG("PED CHAN=" << channel << " GAIN=LG" << " TYPE=" << laserObject.getType(channel,TileID::LOWGAIN,1) << " MEAN=" << laserObject.getMean(channel,TileID::LOWGAIN,1) << " SIGMA=" << laserObject.getSigma(channel,TileID::LOWGAIN,1) << " N=" << laserObject.getN(channel,TileID::LOWGAIN,1) << " isSet?=" << laserObject.isSet(channel,TileID::LOWGAIN,1) );
          ATH_MSG_DEBUG("PED CHAN=" << channel << " GAIN=HG" << " TYPE=" << laserObject.getType(channel,TileID::HIGHGAIN,1) << " MEAN=" << laserObject.getMean(channel,TileID::HIGHGAIN,1) << " SIGMA=" << laserObject.getSigma(channel,TileID::HIGHGAIN,1) << " N=" << laserObject.getN(channel,TileID::HIGHGAIN,1) << " isSet?=" << laserObject.isSet(channel,TileID::HIGHGAIN,1) );
          
          ATH_MSG_DEBUG("LED CHAN=" << channel << " GAIN=LG" << " TYPE=" << laserObject.getType(channel,TileID::LOWGAIN,2) << " MEAN=" << laserObject.getMean(channel,TileID::LOWGAIN,2) << " SIGMA=" << laserObject.getSigma(channel,TileID::LOWGAIN,2) << " N=" << laserObject.getN(channel,TileID::LOWGAIN,2) << " isSet?=" << laserObject.isSet(channel,TileID::LOWGAIN,2) );
          ATH_MSG_DEBUG("LED CHAN=" << channel << " GAIN=HG" << " TYPE=" << laserObject.getType(channel,TileID::HIGHGAIN,2) << " MEAN=" << laserObject.getMean(channel,TileID::HIGHGAIN,2) << " SIGMA=" << laserObject.getSigma(channel,TileID::HIGHGAIN,2) << " N=" << laserObject.getN(channel,TileID::HIGHGAIN,2) << " isSet?=" << laserObject.isSet(channel,TileID::HIGHGAIN,2) );
          
          ATH_MSG_DEBUG("ALP CHAN=" << channel << " GAIN=LG" << " TYPE=" << laserObject.getType(channel,TileID::LOWGAIN,3) << " MEAN=" << laserObject.getMean(channel,TileID::LOWGAIN,3) << " SIGMA=" << laserObject.getSigma(channel,TileID::LOWGAIN,3) << " N=" << laserObject.getN(channel,TileID::LOWGAIN,3) << " isSet?=" << laserObject.isSet(channel,TileID::LOWGAIN,3) );
          ATH_MSG_DEBUG("ALP CHAN=" << channel << " GAIN=HG" << " TYPE=" << laserObject.getType(channel,TileID::HIGHGAIN,3) << " MEAN=" << laserObject.getMean(channel,TileID::HIGHGAIN,3) << " SIGMA=" << laserObject.getSigma(channel,TileID::HIGHGAIN,3) << " N=" << laserObject.getN(channel,TileID::HIGHGAIN,3) << " isSet?=" << laserObject.isSet(channel,TileID::HIGHGAIN,3) );
          
          ATH_MSG_DEBUG(MSG::hex << msb0 << " + " << lsb0 << " => " << SSQ0 <<
                        MSG::dec << "  >>>  D" << channel << "(HG)"
                        << " SUMX/N="
                        << sum[channel*2+HG]
                        << " / " << nevt
                        << " = "
                        << ped[channel*2+HG]
                        << " SUMX2/N="
                        << ssq[channel*2+HG]
                        << " / " << nevt
                        << " => STD="
                        << std[channel*2+0]);
          ATH_MSG_DEBUG(MSG::hex << msb1 << " + " << lsb1 << " => " << SSQ1 <<
                        MSG::dec << "  >>>  D" << channel << "(LG)"
                        << " SUMX/N="
                        << sum[channel*2+LG]
                        << " / " << nevt
                        << " = "
                        << ped[channel*2+LG]
                        << " SUMX2/N="
                        << ssq[channel*2+LG]
                        << " / " << nevt
                        << " => STD="
                        << std[channel*2+LG]);
        } // IF
        
        ped[channel*2+0] = 8500. - ped[channel*2+0];
        ped[channel*2+1] = 8500. - ped[channel*2+1];
      }
      
    } else { // nevt == 0
      ATH_MSG_DEBUG("WARNING NEVT=0");
      memset(ped,0,sizeof(ped));
      memset(std,0,sizeof(std));
    }
    
    if (debug) {
      for(int channel=0;channel<16;++channel){
        ATH_MSG_DEBUG("HG  CHANNEL " << channel << ":  sig=" << las[HG+channel*2] << " ped=" << ped[HG+channel*2] << "+/-" << std[HG+channel*2]);
        ATH_MSG_DEBUG("LG  CHANNEL " << channel << ":  sig=" << las[LG+channel*2] << " ped=" << ped[LG+channel*2] << "+/-" << std[LG+channel*2]);
      }
    }
    
  } else { // short fragment
    ATH_MSG_DEBUG("SHORT FRAGMENT, size=" << p[0] << " type=0x" << MSG::hex << p[1] << MSG::dec);
    memset(ped,0,sizeof(ped));
    memset(std,0,sizeof(std));
  }
  
  // ATTENTION: note that HG has even index (0,2,4,...) in las[], ped[], std[] arrays
  
  // STORE PMT INFORMATION
  //PMT1
  laserObject.setPmt(0,las[HG+10*2],TDC0,laserObject.getMean(10,TileID::HIGHGAIN,0),laserObject.getSigma(10,TileID::HIGHGAIN,0),TileID::HIGHGAIN);
  laserObject.setPmt(0,las[LG+10*2],TDC0,laserObject.getMean(10,TileID::LOWGAIN,0), laserObject.getSigma(10,TileID::LOWGAIN,0), TileID::LOWGAIN);
  //PMT2
  laserObject.setPmt(1,las[HG+14*2],TDC1,laserObject.getMean(14,TileID::HIGHGAIN,0),laserObject.getSigma(14,TileID::HIGHGAIN,0),TileID::HIGHGAIN);
  laserObject.setPmt(1,las[LG+14*2],TDC1,laserObject.getMean(14,TileID::LOWGAIN,0), laserObject.getSigma(14,TileID::LOWGAIN,0), TileID::LOWGAIN);
  
  // STORE DIODE INFORMATION
  for(int diode=0;diode<10;++diode){
    laserObject.setDiode(diode,las[HG+diode*2],laserObject.getMean( diode,TileID::HIGHGAIN,0),
                                               laserObject.getSigma(diode,TileID::HIGHGAIN,0),
                                               laserObject.getMean( diode,TileID::HIGHGAIN,3),
                                               laserObject.getSigma(diode,TileID::HIGHGAIN,3),0.,0.,TileID::HIGHGAIN);
    laserObject.setDiode(diode,las[LG+diode*2],laserObject.getMean( diode,TileID::LOWGAIN,0),
                                               laserObject.getSigma(diode,TileID::LOWGAIN,0),
                                               laserObject.getMean( diode,TileID::LOWGAIN,3),
                                               laserObject.getSigma(diode,TileID::LOWGAIN,3),0.,0.,TileID::LOWGAIN);
  } // FOR
  laserObject.setDiode(10,las[HG+22],laserObject.getMean( 11,TileID::HIGHGAIN,0),
                                     laserObject.getSigma(11,TileID::HIGHGAIN,0),
                                     laserObject.getMean( 11,TileID::HIGHGAIN,3),
                                     laserObject.getSigma(11,TileID::HIGHGAIN,3),0.,0.,TileID::HIGHGAIN); // EXTERNAL CIS 0 HG
  laserObject.setDiode(10,las[LG+22],laserObject.getMean( 11,TileID::LOWGAIN,0),
                                     laserObject.getSigma(11,TileID::LOWGAIN,0),
                                     laserObject.getMean( 11,TileID::LOWGAIN,3),
                                     laserObject.getSigma(11,TileID::LOWGAIN,3),0.,0.,TileID::LOWGAIN);  // EXTERNAL CIS 0 LG
  laserObject.setDiode(11,las[HG+24],laserObject.getMean( 12,TileID::HIGHGAIN,0),
                                     laserObject.getSigma(12,TileID::HIGHGAIN,0),
                                     laserObject.getMean( 12,TileID::HIGHGAIN,3),
                                     laserObject.getSigma(12,TileID::HIGHGAIN,3),0.,0.,TileID::HIGHGAIN); // INTERNAL CIS HG
  laserObject.setDiode(11,las[LG+24],laserObject.getMean( 12,TileID::LOWGAIN,0),
                                     laserObject.getSigma(12,TileID::LOWGAIN,0),
                                     laserObject.getMean( 12,TileID::LOWGAIN,3),
                                     laserObject.getSigma(12,TileID::LOWGAIN,3),0.,0.,TileID::LOWGAIN);  // INTERNAL CIS LG
  laserObject.setDiode(12,las[HG+26],laserObject.getMean( 13,TileID::HIGHGAIN,0),
                                     laserObject.getSigma(13,TileID::HIGHGAIN,0),
                                     laserObject.getMean( 13,TileID::HIGHGAIN,3),
                                     laserObject.getSigma(13,TileID::HIGHGAIN,3),0.,0.,TileID::HIGHGAIN); // PHOTODIODE PHOCAL HG
  laserObject.setDiode(12,las[LG+26],laserObject.getMean( 13,TileID::LOWGAIN,0),
                                     laserObject.getSigma(13,TileID::LOWGAIN,0),
                                     laserObject.getMean( 13,TileID::LOWGAIN,3),
                                     laserObject.getSigma(13,TileID::LOWGAIN,3),0.,0.,TileID::LOWGAIN);  // PHOTODIODE PHOCAL LG
  laserObject.setDiode(13,las[HG+30],laserObject.getMean( 15,TileID::HIGHGAIN,0),
                                     laserObject.getSigma(15,TileID::HIGHGAIN,0),
                                     laserObject.getMean( 15,TileID::HIGHGAIN,3),
                                     laserObject.getSigma(15,TileID::HIGHGAIN,3),0.,0.,TileID::HIGHGAIN); // EXTERNAL CIS HG
  laserObject.setDiode(13,las[LG+30],laserObject.getMean( 15,TileID::LOWGAIN,0),
                                     laserObject.getSigma(15,TileID::LOWGAIN,0),
                                     laserObject.getMean( 15,TileID::LOWGAIN,3),
                                     laserObject.getSigma(15,TileID::LOWGAIN,3),0.,0.,TileID::LOWGAIN);  // EXTERNAL CIS LG
  
} // unpack_frag17


void TileROD_Decoder::unpack_brod(uint32_t /* version */, const uint32_t* p,
                                  pBeamVec & pBeam) const {
  // first word is frag size
  int count = *(p);
  // second word is frag ID and frag type
  int frag = *(p + 1) & 0xFFFF;
  
  p += 2; // 2 words so far
  int datasize = count - m_sizeOverhead; // can be 2 or 3 words less
  
  if (msgLvl(MSG::VERBOSE)) {
    msg(MSG::VERBOSE) << "Unpacking Beam Elements, ID=0x" << MSG::hex << frag
    << ", size=" << MSG::dec << datasize;
    for (int ch = 0; ch < datasize; ++ch) {
      if (0 == ch % 8) msg(MSG::VERBOSE) << endmsg;
      msg(MSG::VERBOSE) << p[ch] << " ";
    }
    msg(MSG::VERBOSE) << endmsg;
  }
  
  if (datasize <= 0) return;
  
  frag &= 0xff; // reset upper byte, because it's different in 2003 and 2004
  HWIdentifier drawerID = m_tileHWID->drawer_id(frag);
  TileBeamElem* rc;
  
  switch (frag) {
      
      /* ************************************************************************************* */
      /*    LeCroy 1176 16bit 1ns TDC FRAG in Tile Beam crate or in Common Beam crate          */
    case BEAM_TDC_FRAG:
    case COMMON_TDC1_FRAG:
    case COMMON_TDC2_FRAG: {
      
      uint32_t prev = 0xFF;
      std::vector<uint32_t> adc;
      for (int c = 0; c < datasize; ++c) {
        uint32_t time = *p & 0xFFFF;
        uint32_t flag = *p >> 16;
        //uint32_t edge =  flag & 0x01;
        uint32_t chan = (flag >> 1) & 0x0F;
        uint32_t bad = (flag >> 5) & 0x01;
        if (prev != chan && !bad) {
          if (prev != 0xFF) {
            HWIdentifier adcID = m_tileHWID->adc_id(drawerID, prev, 0);
            rc = new TileBeamElem(adcID, adc);
            pBeam.push_back(rc);
            adc.clear();
          }
          prev = chan;
        }
        adc.push_back(time);
        ++p;
      }
      if (prev != 0xFF) {
        HWIdentifier adcID = m_tileHWID->adc_id(drawerID, prev, 0);
        rc = new TileBeamElem(adcID, adc);
        pBeam.push_back(rc);
      }
    }
      break;
      
      /* ************************************************************************************* */
      /*           CAEN V775 12 bit TDC  (1count=35psec) in common beam crate			 */
    case COMMON_TOF_FRAG: {
      
      uint32_t prev = 0xFF;
      std::vector<uint32_t> adc;
      for (int c = 0; c < datasize; ++c) {
        uint32_t time = *p & 0x1FFF; // uppermost bit is overflow flag
        // uint32_t underthreshold = ( *p >> 13) & 1; // should be always 0
        uint32_t chan = (*p >> 16) & 0x7FF; // take 11 bits, but 6 upper bits should be 0
        if (prev != chan && chan < 16) { // ignore 16 upper channels and corrupted data
          if (prev != 0xFF) {	        // channel >= 32 can be only in corrupted data
            HWIdentifier adcID = m_tileHWID->adc_id(drawerID, prev, 0);
            rc = new TileBeamElem(adcID, adc);
            pBeam.push_back(rc);
            adc.clear();
          }
          prev = chan;
        }
        adc.push_back(time);
        ++p;
      }
      if (prev < 16) { // ignore 16 upper channels and corrupted data
        HWIdentifier adcID = m_tileHWID->adc_id(drawerID, prev, 0);
        rc = new TileBeamElem(adcID, adc);
        pBeam.push_back(rc);
      }
    }
      break;
      
      /* ************************************************************************************* */
      /*                                      ADDERS FRAG					 */
    case ADD_FADC_FRAG: {
      
      uint32_t val, channel[16][16];
      int nmodule = 4, nchan, nsamp, ch;
      
      nchan = nmodule * 4;
      nsamp = datasize / nmodule;
      
      ATH_MSG_VERBOSE( " Adders: nmod=" << nmodule
                      << ", nchan=" << nchan
                      << ", nsamp=" << nsamp );
      
      if (nmodule * nsamp == datasize) {
        /* Unpack DATA */
        for (int m = 0; m < nmodule; ++m) {
          /* extract all samples for 4 channels in the module */
          for (int s = 0; s < nsamp; ++s) {
            val = *p;
            for (int c = 0; c < 4; ++c) {
              ch = m * 4 + c;
              if (ch < nchan) channel[ch][s] = val & 0xFF;
              val >>= 8;
            }
            ++p;
          }
        }
        /* ***** */
        for (ch = 0; ch < nchan; ++ch) {
          HWIdentifier adcID = m_tileHWID->adc_id(drawerID, ch, 0);
          std::vector<uint32_t> adc;
          for (int s = 0; s < nsamp; ++s) {
            adc.push_back(channel[ch][s]);
          }
          rc = new TileBeamElem(adcID, adc);
          pBeam.push_back(rc);
        }
      } else {
        ATH_MSG_ERROR("unpack_brod => Unexpected Adders size: " << MSG::dec << datasize );
        return;
      }
    }
      break;
      
#ifndef LASER_OBJ_FRAG
#define LASER_OBJ_FRAG   0x016
#endif
#ifndef LASER2_OBJ_FRAG
#define LASER2_OBJ_FRAG   0x017
#endif
      
    case LASER_OBJ_FRAG:
    case LASER2_OBJ_FRAG: {
      
      std::vector<uint32_t> digits;
      for (int ch = 0; ch < datasize; ++ch) {
        digits.push_back((*p));
        ++p;
      }
      HWIdentifier adcID = m_tileHWID->adc_id(drawerID, 0, 0);
      rc = new TileBeamElem(adcID, digits);
      pBeam.push_back(rc);
    }
      break;
      
      /* ************************************************************************************* */
      /*                                   OTHER FRAGS: ONE WORD = ONE CHANNEL       		 */
    default: {
      
      int chmax = (datasize > 16) ? 15 : datasize;
      for (int ch = 0; ch < chmax; ++ch) {
        HWIdentifier adcID = m_tileHWID->adc_id(drawerID, ch, 0);
        rc = new TileBeamElem(adcID, (*p));
        pBeam.push_back(rc);
        ++p;
      }
      if (chmax != datasize) {
        std::vector<uint32_t> digits;
        for (int ch = chmax; ch < datasize; ++ch) {
          digits.push_back((*p));
          ++p;
        }
        HWIdentifier adcID = m_tileHWID->adc_id(drawerID, chmax, 0);
        rc = new TileBeamElem(adcID, digits);
        pBeam.push_back(rc);
      }
    }
      break;
  }
  
  return;
}

StatusCode TileROD_Decoder::convert(const RawEvent* re, TileL2Container* L2Cnt) {
  
  ATH_MSG_DEBUG( "Reading L2 data from ByteStream" );
  
  if (!re) {
    ATH_MSG_FATAL( "RawEvent passed to 'convert'-function is a null pointer!" );
    return StatusCode::FAILURE;
  }
  
  //bool eValid = re->check_tree();
  //if (! eValid) return StatusCode::FAILURE;
  
  //  const VDETF& vdetf = re->compounds(); //Get subdetector fragements from the raw event.
  uint32_t total_sub = re->nchildren();
  
  if (msgLvl(MSG::VERBOSE)) {
    msg(MSG::VERBOSE) << "Full Event: " << endmsg;
    msg(MSG::VERBOSE) << MSG::hex << "Full source ID: " << re->source_id()
    << MSG::dec << endmsg;
    msg(MSG::VERBOSE) << "Fragment size in words: " << re->fragment_size_word() << endmsg;
    msg(MSG::VERBOSE)<< "# of rob frags: " << total_sub << endmsg;
  }
  
  for (uint32_t i_rob = 0; i_rob < total_sub; ++i_rob) {
    const uint32_t* p_rob;
    re->child(p_rob, i_rob);
    const eformat::ROBFragment<const uint32_t*> robFrag(p_rob);
    
    eformat::helper::SourceIdentifier id = eformat::helper::SourceIdentifier(robFrag.source_id());
    unsigned int subDetId = id.subdetector_id();
    
    if (subDetId >= 0x50 && subDetId < 0x60) { //Select Tile-Subdetectors
      fillCollectionL2(&robFrag, *L2Cnt);
    }
  }
  
  // Debug
  
  if (msgLvl(MSG::VERBOSE)) {
    msg(MSG::VERBOSE) << "convertL2: " << L2Cnt->size()
    << " TileL2 objects created" << endmsg;
    TileL2Container::const_iterator ind = L2Cnt->begin();
    TileL2Container::const_iterator last = L2Cnt->end();
    
    int num = 0;
    for (; ind != last; ++ind) {
      msg(MSG::VERBOSE) << "Ind " << num++ << " " << (std::string) (*(*ind)) << endmsg;
    }
  }
  
  return StatusCode::SUCCESS;
}

void TileROD_Decoder::fillCollectionL2(const ROBData * rob, TileL2Container & v) {
  uint32_t version = rob->rod_version() & 0xFFFF;
  
  uint32_t wc = 0;
  uint32_t size = data_size(rob);
  const uint32_t * p = get_data(rob);
  int fragmin = 0xFFF, fragmax = -1;
  
  int counter = 0;
  
  if (size) {
    bool V3format = (*(p) == 0xff1234ff); // additional frag marker since Sep 2005
    if (!V3format && version>0xff) {
      V3format = true;
      if ((m_WarningCounter++) < m_maxWarningPrint)
        ATH_MSG_WARNING("fillCollectionL2( corrupted frag separator 0x" << MSG::hex << (*p) << " instead of 0xff1234ff in ROB 0x" << rob->rod_source_id() << MSG::dec );
    }
    if (V3format) ++p; // skip frag marker
  }
  
  while (wc < size) { // iterator over all words in a ROD
    
    // first word is frag size
    uint32_t count = *(p);
    // second word is frag ID and frag type
    int frag = *(p + 1) & 0xFFFF;
    if (frag < fragmin) fragmin = frag;
    if (frag > fragmax) fragmax = frag;
    
    int type = (*(p + 1) >> 16) & 0xFF;
    
    if (count < m_sizeOverhead || count > size - wc) {
      int cnt = 0;
      for (; wc < size; ++wc, ++cnt, ++p) {
        if ((*p) == 0xff1234ff) {
          ++cnt;
          ++wc;
          ++p;
          break;
        }
      }
      if ((m_ErrorCounter++) < m_maxErrorPrint) {
        msg(MSG::ERROR) << "Frag " << MSG::hex << "0x" << frag
        << MSG::dec << " has unexpected size: " << count;
        if (wc < size) {
          msg(MSG::ERROR) << "  skipping " << cnt << " words to the next frag" << endmsg;
        } else {
          msg(MSG::ERROR) << " ignoring " << cnt << " words till the end of ROD frag" << endmsg;
        }
      }
      continue;
    }
    
    if (type == 4) { // frag4 which contains sum Et at the end
      
      if (unpack_frag4L2(version, p, v)) {
        counter++;
      }
      
    } else if (type == 5 && m_useFrag5Reco) { // new fragment which contains sum Et
      
      if (unpack_frag5L2(version, p, v)) {
        counter++;
      }
      
    } else if ((type >> 4) == 1) { // all frags 0x10-0x15 are MET or MTag frags
      
      counter++;
      
      switch (type) {
          
        case 0x10:
          unpack_frag10(version, p, v);
          break;
        case 0x11:
          unpack_frag11(version, p, v);
          break;
        case 0x12:
          unpack_frag12(version, p, v);
          break;
        case 0x13:
          unpack_frag13(version, p, v);
          break;
        case 0x14:
          unpack_frag14(version, p, v);
          break;
        case 0x15:
          unpack_frag15(version, p, v);
          break;
          
        default:
          int frag = *(p + 1) & 0xFFFF;
          ATH_MSG_ERROR( "Unknown frag type=" << type << " for frag=" << frag );
          assert(0);
          break;
      }
    }
    
    p += count;
    wc += count;
  }
  
  if (wc != size) {
    // check word count
    if ((m_ErrorCounter++) < m_maxErrorPrint) {
      ATH_MSG_ERROR( "Incorrect ROD size: " << wc << " words instead of " << size );
    }
    assert(0);
    // return;
  }
  
  if (m_rChUnit < TileRawChannelUnit::OnlineOffset && counter == 0 && m_L2Builder) {
    if (m_L2Builder->process(fragmin, fragmax, &v).isFailure()) {
      ATH_MSG_ERROR( "Failure in " << m_L2Builder );
      return;
    }
  }
  
  return;
}

void TileROD_Decoder::fillCollectionL2ROS(const ROBData * rob, TileL2Container & v) {
  uint32_t size = data_size(rob);
  const uint32_t * p = get_data(rob);
  const uint32_t ROB_to_decode = ((*p) & 0xFFFF); // Multiply by two
  const uint32_t virtualROBJump = ((*p) >> 16) >> 2; // Divide by four (four drawer-1ROB)
  if (size < ROB_to_decode * virtualROBJump + 1) {
    ATH_MSG_ERROR( "Declared size =" << size
                  << "; virtualROBJump=" << virtualROBJump
                  << "; ROB_to_decode=" << ROB_to_decode );
    return;
  }
  p++; // Jump first word
  
  std::vector<float> sumE(3, 0.0);
  uint32_t idAndType;
  int frag, hash, unit;
  
  for (size_t irob = 0; irob < ROB_to_decode; ++irob) {
    for (size_t drawInRob = 0; drawInRob < virtualROBJump; ++drawInRob) {
      
      idAndType = *(p++);
      frag = idAndType & 0xFFF;
      hash = m_hashFunc(frag);
      if (hash > -1) {
        unit = (idAndType >> (32 - 2)) & 0x3;
        sumE[0] = Frag5_unpack_bin2sum(unit, (int)(*(p++)));
        sumE[1] = Frag5_unpack_bin2sum(unit, (int)(*(p++)));
        sumE[2] = Frag5_unpack_bin2sum(unit, (int)(*(p++)));
        
        (v[hash])->setEt(sumE);
      } else {
        p += 3;
      }
    } // End of drawInRob loop
  } // End of rob loop
  
}

StatusCode TileROD_Decoder::convertLaser(const RawEvent* re, TileLaserObject* laserObject) {
  
  ATH_MSG_DEBUG( "Reading TileLaser data from ByteStream" );
  
  if (!re) {
    ATH_MSG_FATAL( "RawEvent passed to 'convert'-function is a null pointer!" );
    return StatusCode::FAILURE;
  }
  
  //bool eValid = re->check_tree();
  //if (! eValid) return StatusCode::FAILURE;
  
  //  const VDETF& vdetf = re->compounds(); //Get subdetector fragments from the raw event.
  uint32_t total_sub = re->nchildren();
  
  if (msgLvl(MSG::VERBOSE)) {
    msg(MSG::VERBOSE) << "Full Event: " << endmsg;
    msg(MSG::VERBOSE) << MSG::hex << "Full source ID: " << re->source_id() << MSG::dec << endmsg;
    msg(MSG::VERBOSE) << "Fragment size in words: " << re->fragment_size_word() << endmsg;
    msg(MSG::VERBOSE) << "# of rob frags: " << total_sub << endmsg;
  }
  
  for (uint32_t i_rob = 0; i_rob < total_sub; ++i_rob) {
    const uint32_t* p_rob;
    re->child(p_rob, i_rob);
    const eformat::ROBFragment<const uint32_t*> robFrag(p_rob);
    
    eformat::helper::SourceIdentifier id = eformat::helper::SourceIdentifier(
                                                                             robFrag.rod_source_id());
    unsigned int subDetId = id.subdetector_id();
    
    if (subDetId >= 0x50 && subDetId < 0x60) { //Select Tile-Subdetectors
      fillTileLaserObj(&robFrag, *laserObject);
    }
  }
  
  return StatusCode::SUCCESS;
} // end of convertLaser()

void TileROD_Decoder::fillTileLaserObj(const ROBData * rob, TileLaserObject & v) {
  // v.setBCID(-999);  // TileLaserObject default tag -> Laser only if BCID != -999
  
  uint32_t version = rob->rod_version() & 0xFFFF;
  
  uint32_t wc = 0;
  uint32_t size = data_size(rob);
  const uint32_t * p = get_data(rob);
  
  ATH_MSG_DEBUG(" Version = " << version <<
                " wc = "      << wc      <<
                " size = "    << size    <<
                " pointer = " << p );

  if (!size) {
    ATH_MSG_DEBUG(" Nothing to unpack");
    return;
  }
  
  bool V3format = (*(p) == 0xff1234ff); // additional frag marker since Sep 2005
  if (!V3format && version>0) {
    V3format = true;
    if ((m_WarningCounter++) < m_maxWarningPrint)
      ATH_MSG_WARNING("fillTileLaserObj: corrupted frag separator 0x" << MSG::hex << (*p) << " instead of 0xff1234ff in ROB 0x" << rob->rod_source_id() << MSG::dec );
  }
  if (V3format) {
    ++p; // skip frag marker
    m_sizeOverhead = 3;
  } else {
    m_sizeOverhead = 2;
  }
  
  if(V3format) ATH_MSG_DEBUG("tile flag found! size overhead = " << m_sizeOverhead);
  else         ATH_MSG_DEBUG("tile flag not found! size overhead = " << m_sizeOverhead);
  
  while (wc < size) { // iterator over all words in a ROD
    // first word is frag size
    uint32_t count = *(p);
    // second word is frag ID and frag type
    int frag = *(p + 1) & 0xFFFF;
    int type = *(p + 1) >> 16;
    
    msg(MSG::DEBUG) << wc << " / " << size << " HEX = 0x" << MSG::hex << count << " DEC = " << MSG::dec << count << " ( FRAG , TYPE ) = " << frag << " , " << type << endmsg;
    
    if (count < m_sizeOverhead || count > size - wc) {
      int cnt = 0;
      for (; wc < size; ++wc, ++cnt, ++p) {
        if ((*p) == 0xff1234ff) {
          msg(MSG::DEBUG) << "DATA POINTER: HEX = " << MSG::hex << (*p) << " DEC = " << MSG::dec << (*p) << endmsg;
          ++cnt;
          ++wc;
          ++p;
          break;
        }
      }
      if ((m_ErrorCounter++) < m_maxErrorPrint) {
        msg(MSG::ERROR) << "Frag: " << MSG::hex << "0x" << frag << MSG::dec
        << " has unexpected size: " << count;
        if (wc < size) {
          msg(MSG::ERROR) << "  skiping " << cnt << " words to the next frag" << endmsg;
        } else {
          msg(MSG::ERROR) << " ignoring " << cnt << " words till the end of ROD frag" << endmsg;
        }
      }
      continue;
    }
    
    ATH_MSG_DEBUG( "DECODER" << " " << "FRAG=" << frag << " TYPE=" << type << " SIZE=" << size << " FOUND!" );
    
    if (frag == 0x16 && type == 0x20){
      
      const int LASTROD_BCID = rob->rod_bc_id();
      v.setBCID(LASTROD_BCID);
      
      if (size>29){
        ATH_MSG_DEBUG("UNPACKING FRAG16");
        unpack_frag16(version, p, v);
      } else if (size>24) {
        ATH_MSG_DEBUG("UNPACKING NEW FRAG16");
        unpack_frag17(version, p, v);
      }
    }
    
    if (frag == 0x17 && type == 0x20){
      
      const int LASTROD_BCID = rob->rod_bc_id();
      v.setBCID(LASTROD_BCID);
      
      ATH_MSG_DEBUG("UNPACKING FRAG17");
      unpack_frag17(version, p, v);
    } // IF
    
    p += count;
    wc += count;
  }
  
  if (wc != size) {
    // check word count
    if ((m_ErrorCounter++) < m_maxErrorPrint) {
      ATH_MSG_ERROR("Incorrect ROD size: " << wc << " words instead of " << size );
    }
    assert(0);
    // return;
  }
} //end of FillLaserObj

void TileROD_Decoder::fillCollectionHLT(const ROBData * rob, TileCellCollection & v) {
  uint32_t version = rob->rod_version() & 0xFFFF;
  // Resets error flag
  m_error = 0x0;
  
  // figure out which fragment we want to unpack
  TileRawChannelCollection::ID frag_id = v.identify();
  
  /*
   if (frag_id < 0x100) { // BEAM ROD frag - nothing to do
   m_error|=0x10000;
   return;
   }
   */
  
  uint32_t wc = 0;
  uint32_t size = data_size(rob);
  const uint32_t * p = get_data(rob);
  // prepare bcid with one extra bit set for comparison with bcid in DQ fragment
  uint16_t rob_bcid = ((rob->rod_bc_id() & 0x7FFF) | 0x8000);
  
  if (size) {
    bool V3format = (*(p) == 0xff1234ff); // additional frag marker since Sep 2005
    if (!V3format && version>0xff) {
      V3format = true;
      if ((m_WarningCounter++) < m_maxWarningPrint)
        ATH_MSG_WARNING("fillCollectionHLT: corrupted frag separator 0x" << MSG::hex << (*p) << " instead of 0xff1234ff in ROB 0x" << rob->rod_source_id() << MSG::dec );
    }
    if (V3format) {
      ++p; // skip frag marker
      m_sizeOverhead = 3;
    } else {
      m_sizeOverhead = 2;
    }
  }
  uint16_t DQuality = 0x0;
  bool fragFound = false;
  bool DQfragMissing = true;
  bool masked_drawer = false;
  for (size_t m = 0; m < m_list_of_masked_drawers.size(); ++m)
    if (m_list_of_masked_drawers[m] == frag_id) {
      masked_drawer = true;
      break;
    }
  
  while (wc < size) { // iterator over all words in a ROD
    
    // first word is frag size
    uint32_t count = *(p);
    // second word is frag ID (16 bits) frag type (8 bits) and additional flags
    uint32_t idAndType = *(p + 1);
    int frag = (idAndType & 0xFFFF);
    int type = (idAndType & 0xFF0000) >> 16;
    
    if (count < m_sizeOverhead || count > size - wc) {
      int cnt = 0;
      for (; wc < size; ++wc, ++cnt, ++p) {
        if ((*p) == 0xff1234ff) {
          ++cnt;
          ++wc;
          ++p;
          break;
        }
      }
      if ((m_ErrorCounter++) < m_maxErrorPrint) {
        msg(MSG::ERROR) << "Frag: " << MSG::hex << "0x" << frag << MSG::dec
        << " has unexpected size: " << count;
        if (wc < size) {
          msg(MSG::ERROR) << "  skipping " << cnt << " words to the next frag" << endmsg;
        } else {
          msg(MSG::ERROR) << " ignoring " << cnt << " words till the end of ROD frag" << endmsg;
        }
      }
      m_error |= 0x10000;
      continue;
    }
    
    //if (frag == frag_id && ((type > 1 && type < 6) || type==0xa)) { // proper fragment found - unpack it
    if (frag == frag_id) { // proper fragment found - unpack it
      
      switch (type) {
        case 2:
          fragFound = true;
          DQfragMissing = false;
          m_correctAmplitude = false;
          unpack_frag2HLT(version, p, m_pRwChVec);
          break;
        case 3:
          fragFound = true;
          DQfragMissing = false;
          m_correctAmplitude = false;
          unpack_frag3HLT(version, p, m_pRwChVec);
          break;
        case 4:
          if (!m_ignoreFrag4HLT && !fragFound) {
            fragFound = true;
            int unit = (idAndType & 0xC0000000) >> 30;
            m_rc2bytes4.setUnit(unit);
            
            int DataType = (idAndType & 0x30000000) >> 28;
            
            if (DataType < 3) { // real data
              
              m_of2 = ((idAndType & 0x4000000) != 0);
              int nIter = (idAndType & 0x3000000) >> 24;
              m_correctAmplitude = (!nIter); // automatic detection of nIter
              m_rChUnit = (TileRawChannelUnit::UNIT) (unit + TileRawChannelUnit::OnlineOffset); // Online units in real data
              
            } else { // simulated data
              
              DQfragMissing = false;
              m_correctAmplitude = false;
              m_rChUnit = (TileRawChannelUnit::UNIT) (unit); // Offline units in simulated data
            }
            
            unpack_frag4HLT(version, p, m_pRwChVec);
          }
          break;
          
        case 5:
          if (!fragFound) {
            fragFound = true;
            int unit = (idAndType & 0xC0000000) >> 30;
            m_rc2bytes5.setUnit(unit);
            
            m_of2 = ((idAndType & 0x4000000) != 0);
            m_correctAmplitude = true; // fragment 5 will appear only if there is no iterations, so correction required
            m_rChUnit = (TileRawChannelUnit::UNIT) (unit + TileRawChannelUnit::OnlineOffset);
            
            unpack_frag5HLT(version, p, m_pRwChVec);
          }
          break;
          
        case 0xa:
          DQfragMissing = false;
          unpack_fragAHLT(version, p, rob_bcid, DQuality);
          break;
          
        default:
          // Just ignore it
          break;
      }
    }
    p += count;
    wc += count;
  }
  
  if (DQfragMissing && !masked_drawer) m_error |= 0x40000;
  
  if (fragFound) {
    if (masked_drawer) DQuality = 0x0;
    make_copyHLT(m_pRwChVec, v, DQuality);
  } else if (!masked_drawer) m_error |= 0x20000;
  
  return;
}

void TileROD_Decoder::make_copyHLT(pFRwChVec & pChannel, TileCellCollection & v,
                                   const uint16_t DQuality) {
  typedef pFRwChVec::iterator ITERATOR;
  // int gain = 0;
  float ener, time, qual = 0;
  TileCell* pCell;
  m_cell2Double.clear();
  
  if (DQuality == 0xffff) m_error |= 0x80000;
  
  TileRawChannelCollection::ID frag_id = (v.identify() & 0x0FFF);
  int ros = (frag_id >> 8);
  int drawer = (frag_id & 0x3F);
  int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
  unsigned int channelIdx;
  unsigned int adcIdx;
  bool recalibrate = (m_rChUnit != TileRawChannelUnit::OnlineMegaElectronVolts && m_rChUnit != TileRawChannelUnit::MegaElectronVolts);
  
  int sec = ros - 1; // 0-3 for barrel +/- and EB +/-
  
  if (pChannel.size() > 0) { // convert raw channels to cells
    
    // Zero all cell energies
    /*
    for (unsigned int cell = 0; cell < v.size(); ++cell) {
      ((TileCell*) v[cell])->setEnergy_nonvirt(0.0F, 0.0F, 0, CaloGain::INVALIDGAIN);
      ((TileCell*) v[cell])->setTime_nonvirt(-100.0F); // Just to mark a non-initialized cell
      ((TileCell*) v[cell])->setQuality_nonvirt(static_cast<unsigned char>(255), 0, 0);
      ((TileCell*) v[cell])->setQuality_nonvirt(static_cast<unsigned char>(255), 0, 1);
    }
    */

    for (TileCell* cell : v) {
      cell->setEnergy_nonvirt(0.0F, 0.0F, 0, CaloGain::INVALIDGAIN);
      cell->setTime_nonvirt(-100.0F); // Just to mark a non-initialized cell
      cell->setQuality_nonvirt(static_cast<unsigned char>(255), 0, 0);
      cell->setQuality_nonvirt(static_cast<unsigned char>(255), 0, 1);
    }


    int idxraw = drawer * m_maxChannels;
    
    ITERATOR rawItr = pChannel.begin();
    ITERATOR end = pChannel.end();
    
    if (sec < 2) { // special treatment of first PMT in pos/neg barrel which belongs to D0 cell
      idxraw = 0;
      channelIdx = (*rawItr)->channel();
      bool no_dmu_mask = true;
      if (DQuality) {
        if (DQuality >> (channelIdx / 3) & 0x1) no_dmu_mask = false;
      }
      
      adcIdx = (*rawItr)->adc();
      qual = (*rawItr)->quality();
      bool do_mask = false;
      if ((qual < QUALITY_THRESHOLD) && no_dmu_mask) {
        ener = (*rawItr)->amplitude();
        time = (*rawItr)->time();
        // FIXME:: To speed up HLT processing we keep OnlineMegaElectronVolts
        // but this means that we can end up with different units (online or offline)
        if (recalibrate && ener!=0.0F) {
          ener = m_tileToolEmscale->channelCalib(drawerIdx, channelIdx, adcIdx, ener, m_rChUnit,
                                                 TileRawChannelUnit::MegaElectronVolts);
        }
        // parabolic correction for good but slightly out-of-time signals
        if (m_correctAmplitude) {
          if (time<m_allowedTimeMin || time>m_allowedTimeMax) {
            ener = 0.0F;
            time = 0.0F;
          } else if (ener > m_ampMinThresh_MeV
                     && time > m_timeMinThresh
                     && time < m_timeMaxThresh) {
            ener *= TileRawChannelBuilder::correctAmp(time,m_of2);
          }
        }
      } else {
        // ignore energy from bad channel
        ener = 0.0F;
        time = 0.0F;
        do_mask = true;
      }
      
      if (sec > 0) { // sec=1 - negative
        m_D0channeg[drawer].set(channelIdx, adcIdx, ener, time, qual);
        m_D0Existneg[drawer] = true;
        m_D0Maskneg[drawer] = do_mask;
      } else { // sec=0 - positive
        m_D0chanpos[drawer].set(channelIdx, adcIdx, ener, time, qual);
        m_D0Existpos[drawer] = true;
        m_D0Maskpos[drawer] = do_mask;
      }
      ++rawItr; // done with first channel in barrel, go to second
      ++idxraw;
    }
    
    int MBTS_chan=((ros>2)?-1:43);
    
    for (; rawItr != end; ++rawItr) {
      TileFastRawChannel* rawPtr = *rawItr;
      const int rw2cell = m_Rw2Cell[sec][idxraw];
      if (rw2cell != -1) {
        // Get Variables
        pCell = v[rw2cell];
        channelIdx = rawPtr->channel();
        bool no_dmu_mask = true;
        if (DQuality) {
          if (DQuality >> (channelIdx / 3) & 0x1) no_dmu_mask = false;
        }
        adcIdx = rawPtr->adc();
        qual = rawPtr->quality();
        time = 0.0F;
        if (no_dmu_mask && (qual < QUALITY_THRESHOLD)) {
          // gain = adcIdx;
          ener = rawPtr->amplitude();
          time = rawPtr->time();
          // FIXME:: To speed up HLT processing we keep OnlineMegaElectronVolts
          // but this means that we can end up with different units (online or offline)
          if (recalibrate && ener!=0.0F) {
            ener = m_tileToolEmscale->channelCalib(drawerIdx, channelIdx, adcIdx, ener, m_rChUnit,
                                                   TileRawChannelUnit::MegaElectronVolts);
          }
          // parabolic correction for good but slightly out-of-time signals
          if (m_correctAmplitude) {
            if (time<m_allowedTimeMin || time>m_allowedTimeMax) {
              ener = 0.0F;
              time = 0.0F;
            } else if (ener > m_ampMinThresh_MeV
                       && time > m_timeMinThresh
                       && time < m_timeMaxThresh) {
              ener *= TileRawChannelBuilder::correctAmp(time,m_of2);
            }
          }
          if (pCell->time() != -100.0F) pCell->setTime(time, m_Rw2Pmt[sec][idxraw]);
          else pCell->setTime_nonvirt(time);
          
          pCell->setQuality_nonvirt(std::min(255, abs((int) qual)), 0, m_Rw2Pmt[sec][idxraw]);
          pCell->addEnergy(ener, m_Rw2Pmt[sec][idxraw], adcIdx);
          
          if (pCell->caloDDE()->onl2() == TileHWID::NOT_VALID_HASH)
            pCell->setQuality_nonvirt(0, 0, 1);
          
        } else if (pCell->caloDDE()->onl2() != TileHWID::NOT_VALID_HASH) {
          pCell->addEnergy(0.0F, m_Rw2Pmt[sec][idxraw], 0U);
          pCell->setQuality_nonvirt((unsigned char) 255, 0, m_Rw2Pmt[sec][idxraw]);
          m_cell2Double.push_back(pCell); // have to double later
        } else { // Mask it here for Gap
          // (pCell)->setEnergy_nonvirt(0.0F, 0.0F, 0, CaloGain::INVALIDGAIN);
          pCell->setTime_nonvirt(0.0F);
          pCell->setQuality_nonvirt(static_cast<unsigned char>(qual), 0, 1);
        }
      } else if (MBTS_chan<0) MBTS_chan=rawPtr->channel();// end of if
      ++idxraw;
      //if ( DQuality ) idxraw1++;
    } // End of for TileRawChannel
    
    if (MBTS_chan==4) { // assign zero energy and high gain  to second PMT in special C10
      pCell = v[m_Rw2Cell[sec][5]]; // find cell with channel 5 connected
      pCell->addEnergy(0., 1-m_Rw2Pmt[sec][5], 1);
    }
    
    if (m_MBTS != NULL && MBTS_chan >= 0) {
      unsigned int idx = m_mapMBTS[frag_id];
      if (idx < (*m_MBTS).size()) { // MBTS present (always last channel)
        TileCell* pCell = (*m_MBTS)[idx];
        TileFastRawChannel* rawPtr = pChannel[MBTS_chan];
        channelIdx = rawPtr->channel();
        adcIdx = rawPtr->adc();
        float ener = rawPtr->amplitude();
        float time = rawPtr->time();
        float qual = rawPtr->quality();
        if (qual < QUALITY_THRESHOLD) {
          if (m_rChUnit==TileRawChannelUnit::MegaElectronVolts) { // go back to pC - not standard configuration
            ener /= m_tileToolEmscale->channelCalib(drawerIdx, channelIdx, adcIdx, 1.0, // calibrate to PicoCoulombs
                                                    TileRawChannelUnit::PicoCoulombs, m_rChUnit);
          } else {
            ener = m_tileToolEmscale->channelCalib(drawerIdx, channelIdx, adcIdx, ener, // calibrate to PicoCoulombs
                                                   m_rChUnit, TileRawChannelUnit::PicoCoulombs);
          }
          // parabolic correction for good but slightly out-of-time signals
          if (m_correctAmplitude) {
            if (time<m_allowedTimeMin || time>m_allowedTimeMax) {
              ener = 0.0F;
              time = 0.0F;
            } else if (ener > m_ampMinThresh_pC
                       && time > m_timeMinThresh
                       && time < m_timeMaxThresh) {
              ener *= TileRawChannelBuilder::correctAmp(time,m_of2);
            }
          }
        } else {
          // ignore energy from bad channel
          ener = 0.0F;
          time = -100.0F;
        }
        pCell->setEnergy(ener, 0.0F);
        pCell->setTime_nonvirt(time);
        pCell->setQuality_nonvirt(std::min(255, abs((int) qual)), 0, 0);
      } // End of if idx
      
    } // End of if m_MBTS
    
  } // end of if vec<TileRawChannel>::size > 0)
  
  for (std::vector<TileCell*>::const_iterator d_it = m_cell2Double.begin();
       d_it != m_cell2Double.end(); ++d_it) {
    
    //    int gain = 0;
    //    if ((*d_it)->gain1() == 1 || (*d_it)->gain2() == 1) gain = 1;
    int gain = (*d_it)->gain1() | (*d_it)->gain2();
    int qual = (*d_it)->qual1() & (*d_it)->qual2();
    
    (*d_it)->setEnergy_nonvirt((*d_it)->e(), (*d_it)->e(), gain, gain);
    (*d_it)->setQuality_nonvirt(static_cast<unsigned char>(qual), 0, 0);
    (*d_it)->setQuality_nonvirt(static_cast<unsigned char>(qual), 0, 1);
    if ((*d_it)->time() == -100.0F) (*d_it)->setTime_nonvirt(0.0F);
  }
  
}

void TileROD_Decoder::unpack_frag2HLT(uint32_t /* version */, const uint32_t* p,
                                      pFRwChVec & pChannel) const {
  // first word is frag size
  int count = *(p);
  
  p += 2; // 2 words so far
  int wc = m_sizeOverhead; // can be 2 or 3 words
  
  for (unsigned int ch = 0; ch < m_maxChannels; ++ch) {
    unsigned int w = (*p);
    
    if (w != 0) { // skip invalid channels
      
      pChannel[ch]->set(ch
                        , m_rc2bytes2.gain(w)
                        , m_rc2bytes2.amplitude(w)
                        , m_rc2bytes2.time(w)
                        , m_rc2bytes2.quality(w));
      
    } else {
      pChannel[ch]->set(ch, 1U, 0.0F, 0.0F, 0.0F);
    }
    ++wc;
    ++p;
  }
  
  if (wc != count) {
    // check word count
    ATH_MSG_ERROR("Incorrect word count: " << wc << " != " << count );
    assert(0);
    // return;
  }
  
  return;
}

void TileROD_Decoder::unpack_frag3HLT(uint32_t /* version */, const uint32_t* p,
                                      pFRwChVec & pChannel) const {
  // first word is frag size
  int count = *(p);
  // second word is frag ID and frag type
  
  //  ATH_MSG_VERBOSE( "first 4 words " << MSG::hex
  //                  << "  0x" << *p
  //                  << "  0x" << *(p+1)
  //                  << "  0x" << *(p+2)
  //                  << "  0x" << *(p+3) << MSG::dec );
  
  // followed by 2 map words
  const uint32_t* pMap = p + 2;
  
  const short* p16 = (const short *)p;
  
  p16 = p16 + 8; // 8 16bit words so far
  short wc16 = 4 + m_sizeOverhead * 2; // can be 8 or 10 (if overhead is 3 words)
  
  for (unsigned int ch = 0U; ch < 48U; ++ch) {
    if (checkBit(pMap, ch)) {
      
      pChannel[ch]->set(ch
                        , m_rc2bytes.gain(*p16)
                        , m_rc2bytes.amplitude(*p16)
                        , m_rc2bytes.time(*(p16 + 1))
                        , m_rc2bytes.quality(*(p16 + 2)));
      
      //      ATH_MSG_VERBOSE(" frag 0x" << MSG::hex  << frag << MSG::dec
      //                      << " ch " << ch
      //                      << " " << MSG::hex << "0x" << *p16 << *(p16+1) << *(p16+2) << MSG::dec
      //                      << " " << gain
      //                      << " " << m_rc2bytes.amplitude(*p16)
      //                      << " " << m_rc2bytes.time(*(p16+1))
      //                      << " " << m_rc2bytes.quality(*(p16+2)) );
      
      wc16 = wc16 + 3;
      p16 = p16 + 3;
    } // channel present
    else {
      pChannel[ch]->set(ch, 1U, 0.0F, 0.0F, 0.0F);
    }
  } // all bits, done with this frag
  
  if (wc16 % 2 == 1) {
    ++wc16; // align
    ++p16;
  }
  if (wc16 != 2 * count) {
    // check word count
    ATH_MSG_ERROR( "unpack_frag3HLT => Incorrect wordcount: "
                  << wc16 << " != 2*" << count );
    assert(0);
    // return;
  }
  
  return;
}

void TileROD_Decoder::unpack_frag4HLT(uint32_t /* version */, const uint32_t* p,
                                      pFRwChVec & pChannel) const {
  // first word is frag size
  int count = *(p);
  // second word is frag ID and frag type
  
  p += 2; // 2 words so far
  int wc = m_sizeOverhead; // can be 2 or 3 words
  for (unsigned int ch = 0U; ch < m_maxChannels; ++ch) {
    unsigned int w = (*p);
    if (w != 0) { // skip invalid channels
      pChannel[ch]->set(ch
                        , m_rc2bytes4.gain(w)
                        , m_rc2bytes4.amplitude(w)
                        , m_rc2bytes4.time(w)
                        , m_rc2bytes4.quality(w));
      
    } else {
      pChannel[ch]->set(ch, 1U, 0.0F, 0.0F, 0.0F);
    }
    ++wc;
    ++p;
  }
  
  if (wc > count) {
    // check word count
    ATH_MSG_ERROR( "unpack_frag4HLT => Incorrect word count: "
                  << wc << " != " << count );
    assert(0);
    // return;
  }
  
  return;
}

void TileROD_Decoder::unpack_frag5HLT(uint32_t /* version */, const uint32_t* p,
                                      pFRwChVec & pChannel) const {
  // first word is frag size
  int count = *(p);
  // second word is frag ID and frag type
  //  int frag = *(p + 1) & 0xFFFF;
  int size_L2 = (*(p + 1) >> (32 - 2 - 3)) & 0x7;
  
  p += 2; // 2 words so far
  int wc = m_sizeOverhead; // can be 2 or 3 words
  uint16_t* ptr_bad = (uint16_t*) (p + 48 + size_L2); // Reco + Size_L2
  uint16_t bad_bits[3] = { ptr_bad[1], ptr_bad[0], ptr_bad[3] };
  
  uint32_t code;
  int fmt, bad, gain(0), ene_bin(0), time_bin(0), quality;
  unsigned int w, unit = m_rc2bytes5.getUnit();
  float ene(0.0), time(0.0);
  
  unsigned int ch = 0U;
  for (unsigned int i = 0U; i < 3U; ++i) {
    uint16_t bad16 = ~bad_bits[i]; // note inversion here
    for (unsigned int j = 0U; j < 16U; ++j) {
      w = (*p);
      
      code = w >> 24;
      fmt = m_rc2bytes5.FormatLookup[code];
      Frag5_unpack_reco_bin(w, code, gain, ene_bin, time_bin);
      Frag5_unpack_bin2reco(unit, gain, ene_bin, ene, time_bin, time);
      //m_rc2bytes5.unpack_reco(w, fmt, gain, ene, time);
      
      bad = (bad16 & 0x1);
      bad16 >>= 1;
      quality = m_rc2bytes5.get_quality(bad, fmt);
      pChannel[ch]->set(ch, gain, ene, time, quality);
      
      ++ch;
      ++wc;
      ++p;
    }
  }
  
  if (wc > count) {
    // check word count
    ATH_MSG_ERROR( "unpack_frag5HLT => Incorrect word count: "
                  << wc << " != " << count );
    assert(0);
    // return;
  }
  
  return;
}

bool TileROD_Decoder::unpack_frag4L2(uint32_t /* version */, const uint32_t* p,
                                     TileL2Container & pL2) const {
  // first word is frag size
  int size = *(p);
  // second word is frag ID and frag type
  uint32_t idAndType = *(p + 1);
  int frag = idAndType & 0xFFFF;
  int unit = (idAndType >> (32 - 2)) & 0x3;
  
#ifdef DO_NOT_USE_MUON_TAG
  
  p += 50; // offset 50: 2 words in header and 48 channels after
  
#else
  
  // this part should be very similar to unpack_frag4HLT - it reads amplitude from DSP
  // and then uses vector of 48 amplitudes to calculate Muon tags
  // but since nobody is using Tile Muon tags in L2 trigger
  // this part is not implemented
  
  p+=50;// offset 50: 2 words in header and 48 channels after
  
#endif
  
  int size_L2 = size - m_sizeOverhead - 48;
  if (size_L2 > 0) {
    
    std::vector<float> sumE;
    while (size_L2--) {
      sumE.push_back(Frag5_unpack_bin2sum(unit, (int)(*(p++))));
    }
    (*pL2[m_hashFunc(frag)]).setEt(sumE);
    
    return true;
    
  } else {
    
    return false; // indicate that there is no sumEt info in the fragment
  }
}

bool TileROD_Decoder::unpack_frag5L2(uint32_t /* version */, const uint32_t* p,
                                     TileL2Container & pL2) const {
  // second word is frag ID and frag type
  uint32_t idAndType = *(p + 1);
  int frag = idAndType & 0xFFFF;
  int unit = (idAndType >> (32 - 2)) & 0x3;
  int size_L2 = (idAndType >> (32 - 2 - 3)) & 0x7;
  
#ifdef DO_NOT_USE_MUON_TAG
  
  p += 50; // offset 50: 2 words in header and 48 channels after
  
#else
  
  // this part is very similar to unpack_frag5HLT - it reads amplitude from DSP
  // and then uses vector of 48 amplitudes to calculate Muon tags
  // all this is written only for tests and will not be used actually,
  // since nobody is using Tile Muon tags in L2 trigger
  
  p+=2;// 2 words so far
  
  int drawer = (frag & 0x3F);
  int ros = (frag >> 8);
  int drawerIdx = TileCalibUtils::getDrawerIdx(ros,drawer);
  TileRawChannelUnit::UNIT onlUnit = (TileRawChannelUnit::UNIT) ( unit + TileRawChannelUnit::OnlineOffset );
  bool recalibrate = (onlUnit != TileRawChannelUnit::OnlineMegaElectronVolts);
  float E_MeV[48];
  int Gain[48];
  bool bad[48];
  
  uint16_t* ptr_bad = (uint16_t*)(p + 48 + size_L2);// Reco + Size_L2
  uint16_t bad_bits[3] = {ptr_bad[1], ptr_bad[0], ptr_bad[3]};
  
  uint32_t code;
  int fmt, gain(0), ene_bin(0), time_bin(0);
  unsigned int w;
  float ene(0.0), time(0.0);
  
  int ch = 0;
  for (int i = 0; i < 3; ++i) {
    uint16_t bad16 = ~bad_bits[i]; // note inversion here
    for (int j = 0; j < 16; ++j) {
      w = (*p);
      
      code = w >> 24;
      fmt = m_rc2bytes5.FormatLookup[code];
      Frag5_unpack_reco_bin(w, code, gain, ene_bin, time_bin);
      Frag5_unpack_bin2reco(unit, gain, ene_bin, ene, time_bin, time);
      //m_rc2bytes5.unpack_reco(w, fmt, gain, ene, time);
      
      bad[ch] = (bad16 & 0x1); bad16 >>= 1;
      Gain[ch] = gain;
      if (recalibrate)
        E_MeV[ch] = m_tileToolEmscale->channelCalib(drawerIdx, ch, gain, ene, onlUnit, TileRawChannelUnit::MegaElectronVolts);
      else
        E_MeV[ch] = ene;
      
      ++ch;
      ++p;
    }
  }
  
  std::vector<double> EtaMuons;
  std::vector<double> EMuons0;
  std::vector<double> EMuons1;
  std::vector<double> EMuons2;
  std::vector<unsigned int> qf;
  std::vector<unsigned int> word;
  
  switch (ros) {
    case TileHWID::BARREL_POS: m_L2Builder->MTagLB(ros,drawer,E_MeV,Gain,bad,EtaMuons,EMuons0,EMuons1,EMuons2,qf,word); break;
    case TileHWID::BARREL_NEG: m_L2Builder->MTagLB(ros,drawer,E_MeV,Gain,bad,EtaMuons,EMuons0,EMuons1,EMuons2,qf,word); break;
    case TileHWID::EXTBAR_POS: m_L2Builder->MTagEB(ros,drawer,E_MeV,Gain,bad,EtaMuons,EMuons0,EMuons1,EMuons2,qf,word); break;
    case TileHWID::EXTBAR_NEG: m_L2Builder->MTagEB(ros,drawer,E_MeV,Gain,bad,EtaMuons,EMuons0,EMuons1,EMuons2,qf,word); break;
    default: ATH_MSG_ERROR( "unpack_frag5L2: incorrect ros value: " << ros );
  }
  
  (*pL2[m_hashFunc(frag)]).setMu(EtaMuons, EMuons0, EMuons1, EMuons2, qf, word);
  
#endif
  
  if (size_L2 > 0) {
    
    std::vector<float> sumE;
    while (size_L2--) {
      sumE.push_back(Frag5_unpack_bin2sum(unit, (int)(*(p++))));
    }
    (*pL2[m_hashFunc(frag)]).setEt(sumE);
    
    return true;
    
  } else {
    
    return false; // indicate that there is no sumEt info in the fragment
  }
}

void TileROD_Decoder::initD0cellsHLT() {
  for (int i = 0; i < 64; ++i) {
    m_D0Existneg[i] = false;
    m_D0Existpos[i] = false;
    m_D0Maskneg[i] = false;
    m_D0Maskpos[i] = false;
  }
}

void TileROD_Decoder::mergeD0cellsHLT(TileCellCollection & v) {
  
  TileRawChannelCollection::ID frag_id = (v.identify() & 0x0FFF);
  int ros = (frag_id >> 8);
  if (ros == 1) {
    int drawer = (frag_id & 0xFF);
    TileCellCollection::iterator pCell = v.begin();
    pCell += 2;
    if (m_D0Existneg[drawer] && m_D0Existpos[drawer]) {
      double amp1 = m_D0chanpos[drawer].amplitude();
      double amp2 = m_D0channeg[drawer].amplitude();
      int gain1 = m_D0chanpos[drawer].adc();
      int gain2 = m_D0channeg[drawer].adc();
      if ((!m_D0Maskneg[drawer]) && (!m_D0Maskpos[drawer])) {
        (*pCell)->setEnergy(amp1, amp2, gain1, gain2);
        (*pCell)->setTime(m_D0chanpos[drawer].time());
        (*pCell)->setTime(m_D0channeg[drawer].time(), 1);
        (*pCell)->setQuality(static_cast<unsigned char>(m_D0chanpos[drawer].quality()), 0, 0);
        (*pCell)->setQuality(static_cast<unsigned char>(m_D0channeg[drawer].quality()), 0, 1);
      } else if (m_D0Maskpos[drawer]) {
        (*pCell)->setEnergy(amp2, amp2, gain1, gain2);
        (*pCell)->setTime(m_D0channeg[drawer].time());
        (*pCell)->setQuality(static_cast<unsigned char>(m_D0channeg[drawer].quality()), 0, 0);
      } else {
        (*pCell)->setEnergy(amp1, amp1, gain1, gain1);
        (*pCell)->setTime(m_D0chanpos[drawer].time());
        (*pCell)->setQuality(static_cast<unsigned char>(m_D0chanpos[drawer].quality()), 0, 0);
      }
    } else if (m_D0Existpos[drawer]) {
      double amp1 = m_D0chanpos[drawer].amplitude();
      int gain1 = m_D0chanpos[drawer].adc();
      (*pCell)->setEnergy(amp1, m_D0chanpos[drawer].time(), gain1, 1);
      (*pCell)->setQuality(static_cast<unsigned char>(m_D0chanpos[drawer].quality()), 0, 1);
    }
  }
}

void TileROD_Decoder::loadMBTS_Ptr(TileCellCollection* col,
                                   std::map<unsigned int, unsigned int>& mapMBTS, int MBTS_channel) {
  m_MBTS = col;
  m_mapMBTS = mapMBTS;
  m_MBTS_channel = MBTS_channel;
  return;
}

void TileROD_Decoder::initHid2re() {
  if (m_hid2re) return;
  
  ATH_MSG_DEBUG( "initHid2re() for run " << m_fullTileRODs );

  m_hid2re = new TileHid2RESrcID(m_tileHWID,m_fullTileRODs); // setting normal frag2RODmap and map dedicated to TMDB
  
  // Check whether we want to overwrite default ROB IDs
  
  IProperty* propertyServer(0);
  if (serviceLocator()->service("ByteStreamCnvSvc", propertyServer).isSuccess()) {
    
    std::vector<std::string> vecProp;
    StringArrayProperty vecProperty("ROD2ROBmap", vecProp);
    
    if (propertyServer->getProperty(&vecProperty).isSuccess()) {
      
      if (vecProperty.value().size() % 2 == 1) {
        ATH_MSG_DEBUG( "Length of ROD2ROBmap is and odd value, "
                       << " means that we'll scan event for all fragments to create proper map" );
        
        IROBDataProviderSvc* robSvc;
        if (service("ROBDataProviderSvc", robSvc).isSuccess()) {
          const eformat::FullEventFragment<const uint32_t*> * event = robSvc->getEvent();
          try {
            event->check_tree();
            m_hid2re->setROD2ROBmap(event, msg());
          } catch (...) {
            ATH_MSG_DEBUG( "Bad event, mapping might be incomplete! " );
            // bad event, but process anyhow (well, till next bug report )
            m_hid2re->setROD2ROBmap(event, msg());
          }
        }
      } else if (vecProperty.value().size() == 0) {
        ATH_MSG_DEBUG( "Length of ROD2ROBmap vector is zero, "
                       << " means that predefined mapping for run " << m_fullTileRODs << " will be used " );
      } else {
        ATH_MSG_DEBUG( "Apply additional remapping for " << vecProperty.value().size()/2 << " fragments from jobOptions ");
        m_hid2re->setROD2ROBmap(vecProperty.value(), msg());
      }
    }
  }
}

void TileROD_Decoder::initHid2reHLT() {
  if (m_hid2reHLT) return;

  ATH_MSG_DEBUG( "initHid2reHLT() for run " << m_fullTileRODs );

  m_hid2reHLT = new TileHid2RESrcID(m_tileHWID,m_fullTileRODs); // setting a frag2RODmap and map dedicated to TMDB
}

void TileROD_Decoder::initTileMuRcvHid2re() {
  ATH_MSG_DEBUG( "initTileMuRcvHid2re() for run " << m_fullTileRODs );
  initHid2re();
}

uint32_t* TileROD_Decoder::getOFW(int fragId, int unit) {
  if (unit >> 2) {
    ATH_MSG_WARNING( "Wrong online reconstruction units for getOFW: unit=" << unit
                    << " => assume unit=" << (unit & 3) );
    unit &= 3;
  }
  
  uint32_t drawerIdx = TileCalibUtils::getDrawerIdxFromFragId(fragId);
  size_t id = (drawerIdx << 2 | unit);
  std::vector<uint32_t>* ofw = m_OFWeights[id];
  
  if (ofw == NULL) {
    ATH_MSG_DEBUG("getOFC fragId: 0x" << MSG::hex << fragId << MSG::dec << " Unit: " << unit);
    TileRawChannelUnit::UNIT chan_unit = (TileRawChannelUnit::UNIT) (unit
                                                                     + TileRawChannelUnit::OnlineOffset);
    
    ofw = m_OFWeights[id] = new std::vector<uint32_t>;
    
    bool of2 = true;
    const TileOfcWeightsStruct* weights;
    std::vector<double> a(7), b(7), c(7), g(7), dg(7);
    
    for (int ch = 0; ch < 48; ++ch) {
      for (int gain = 0; gain < 2; ++gain) {
        float phase = -m_tileToolTiming->getSignalPhase(drawerIdx, ch, gain);
        weights = m_tileCondToolOfcCool->getOfcWeights(drawerIdx, ch, gain, phase, of2);
        
        double calib = m_tileToolEmscale->channelCalibOnl(drawerIdx, ch, gain, 1.0, chan_unit);
        
        if (unit != 0 && gain) calib *= 64.0;
        
        for (int i = 0; i < 7; ++i) {
          a[i] = weights->w_a[i];
          b[i] = weights->w_b[i];
          c[i] = weights->w_c[i];
          g[i] = weights->g[i];
          dg[i] = weights->dg[i];
        }
        Format6(a, b, c, g, dg
                , ch // channel
                , 0 // phase = 0 poskol'ku ne ponyal kak okruglyat'
                , calib // calibration
                , *ofw
                , false // verbose
                );
      } // gain
    } // ch
  } // if ofw == NULL
  
  return &((*ofw)[0]);
}

/* TMDB dedicated functions will have a group of functions for each dedicated container 


Each container has 2/3 methods associated
     - fill            : 
     - unpacking : decode words
*/

void TileROD_Decoder::fillCollection_TileMuRcv_Digi(const ROBData* rob , TileDigitsCollection &coll) {

  ATH_MSG_DEBUG( " TileROD_Decoder::fillCollection_TileMuRcv_Digi " );

  int  wc         = 0;
  bool b_sof      = false;
  bool b_digiTmdb = false;

  uint32_t version  = rob->rod_version() & 0xFFFF;
  uint32_t sourceid = rob->rod_source_id();
  int size          = rob->rod_ndata();

  uint32_t sfrag_version = 0x0;
  uint32_t sfrag_type    = 0x0;
  int sfrag_size         = 0;

  const uint32_t *p = get_data(rob);

  ATH_MSG_DEBUG( " Trying DECODER over source ID: "<<MSG::hex<<" 0x"<<sourceid<<MSG::dec<<" size of full fragment: "<<size );

  while ( wc < size ) { // iterator over all words for a robid   
    // first word is the start of the tile subfragment in v3format it is 0xff1234ff
    if ((*p) == 0xff1234ff) b_sof=true;

    ATH_MSG_DEBUG( " Start of sub-fragment word: "<<MSG::hex<<(*p)<<MSG::dec<<" false/true: "<<b_sof );

    if (b_sof) {
      // second word is frag size
      sfrag_size   = *(p+1);
      // third word is frag version (16-bits) + type (16-bits)
      sfrag_version = *(p+2) & 0xFFFF;
      sfrag_type    = *(p+2) >> 16;
      b_sof = false;
    }

    if (sfrag_type==0x40) {
      b_digiTmdb   = true;
      ATH_MSG_DEBUG( MSG::hex<<" DECODER sub-fragment VERSION= 0x"<<sfrag_version<<" TYPE= 0x"<<sfrag_type<< MSG::dec<<" SIZE="<<sfrag_size<<" FOUND! " );
      // type and version are identified and we move to the end of the loop
      wc += size-wc;
      p  += 3;
    } else {
      // carry on looping to search for 0x40
      wc++;
      p++;
    }
  }

  if ( b_digiTmdb ) {
    // the TMDB raw channel 0x40
    uint32_t collID = coll.identify();
    unpack_frag40( collID, version,  p, sfrag_size-3, coll);
  }

  //delete_vec(digi);

  return;

}

void TileROD_Decoder::fillCollection_TileMuRcv_RawChannel(const ROBData* rob , TileRawChannelCollection &coll) {

  ATH_MSG_DEBUG( " TileROD_Decoder::fillCollection_TileMuRcv_RawChannel " );

  int wc        = 0;
  bool b_sof    = false;
  bool b_rcTmdb = false;

  uint32_t version  = rob->rod_version() & 0xFFFF;
  uint32_t sourceid = rob->rod_source_id();
  int size          = rob->rod_ndata();
  int offset = 0;

  uint32_t sfrag_version = 0x0;
  uint32_t sfrag_type    = 0x0;
  int sfrag_size = 0;

  const uint32_t *p = get_data(rob);

  ATH_MSG_DEBUG( " Trying DECODER over source ID: "<<MSG::hex<<" 0x"<<sourceid<<MSG::dec<<" size of full fragment: "<<size );

  while ( wc < size ) { // iterator over all words in a robid
    // first word is the start of the tile subfragment in v3format it is 0xff1234ff
    if ((*p) == 0xff1234ff) b_sof=true;
    ATH_MSG_DEBUG( " Start of sub-fragment word : "<<MSG::hex<< (*p)<<MSG::dec<<" false/true : "<<b_sof );
    // if at the start of the fragment than we should find more information about this (sub-)fragment in the following two words 
    if (b_sof) {
      // second word is frag size
      sfrag_size    = *(p+1);
      // third word is frag version (16-bits) + type (16-bits)
      sfrag_version = *(p+2) & 0xFFFF;
      sfrag_type    = *(p+2) >> 16;
      b_sof=false;
    }
    // for tmdb we will start to have a fragment with three sub-fragments ordered as 0x40 0x41 0x42
    // we investigate if there are any of type 0x40 before going on 0x41
    // like this the loop is faster since we can access the size of this sub-fragment
    //         
    if (sfrag_type==0x40) {
      ATH_MSG_DEBUG(MSG::hex<<" DECODER sub-fragment VERSION= 0x"<<sfrag_version<<" TYPE= 0x"<<sfrag_type<<MSG::dec<<" SIZE="<<sfrag_size<<" FOUND! Keep on looking for 0x41.");
      offset = sfrag_size;
      wc += offset;
      p  += offset;
    } else if (sfrag_type==0x41) {
      b_rcTmdb   = true;
      ATH_MSG_DEBUG(MSG::hex<<" DECODER sub-fragment VERSION= 0x"<<sfrag_version<<" TYPE= 0x"<<sfrag_type<<MSG::dec<<" SIZE="<<sfrag_size<<" FOUND!");
      // type and version are identified and we move to the end of the loop
      wc += size-wc;
      p+=3;
    } else {
      ATH_MSG_DEBUG(MSG::hex<<" DECODER sub-fragment VERSION= 0x"<<sfrag_version<<" TYPE= 0x"<<sfrag_type<<MSG::dec<<" SIZE="<<sfrag_size<<" FOUND!");
      // carry on looping to search for 0x41
      wc++;
      p++;
    }
  }

  if ( b_rcTmdb ) {
    // the TMDB raw channel 0x41
    //
    uint32_t collID = coll.identify();
    unpack_frag41( collID, version,  p, sfrag_size-3, coll);
  }

  //delete_vec(rc);

return;
}

StatusCode TileROD_Decoder::convertTMDBDecision(const RawEvent* re, TileMuonReceiverContainer* tileMuRcv)
{

  ATH_MSG_DEBUG( " TileROD_Decoder::convertTMDBDecision " );

  if (!re) {
    ATH_MSG_FATAL( "RawEvent passed to 'convert'-function is a null pointer!" );
    return StatusCode::FAILURE;
  }

  uint32_t total_sub = re->nchildren();

  for (uint32_t i_rob = 0; i_rob < total_sub; ++i_rob) {
    const uint32_t* p_rob;
    re->child(p_rob, i_rob);
    const eformat::ROBFragment<const uint32_t*> robFrag(p_rob);

    //eformat::helper::SourceIdentifier id = eformat::helper::SourceIdentifier(robFrag.source_id());
    uint32_t sourceid = robFrag.source_id();
    uint32_t modid = sourceid >> 16;
    //msg(MSG::VERBOSE) <<MSG::hex<<" sourceId: 0x"<< robFrag.source_id() <<MSG::dec<< endmsg;

    if ((modid == 0x51 || modid == 0x52 || modid == 0x53 || modid == 0x54) && (sourceid & 0xf00)) {
      fillContainer_TileMuRcv_Decision(&robFrag, *tileMuRcv);
    }
  }

  return StatusCode::SUCCESS;
}

void TileROD_Decoder::fillContainer_TileMuRcv_Decision(const ROBData* rob ,TileMuonReceiverContainer &cont) {

  ATH_MSG_DEBUG( "TileROD_Decoder::fillContainer_TileMuRcv_Decision" );

  int wc          = 0;
  bool b_sof      = false;
  bool b_deciTmdb = false;

  uint32_t version  = rob->rod_version() & 0xFFFF;
  uint32_t sourceid = rob->rod_source_id();
  int size          = rob->rod_ndata();
  int offset        = 0;

  uint32_t sfrag_version = 0x0;
  uint32_t sfrag_type    = 0x0;
  int sfrag_size         = 0;

  const uint32_t *p = get_data(rob);
  std::vector<const uint32_t *> pFragTmdb;
  m_sizeOverhead = 3;

  ATH_MSG_DEBUG( " Trying DECODER over source ID: " << MSG::hex
                << " 0x" << sourceid << MSG::dec
                << " full fragment size " << size );

  while ( wc < size ) { // iterator over all words in a ROD
    // first word is the start of the tile subfragment in v3format it is 0xff1234ff
    if ((*p) == 0xff1234ff)  b_sof=true;

    ATH_MSG_DEBUG( " start of sub-fragment word : " << MSG::hex << (*p) << MSG::dec << " false/true : " << b_sof );

    if (b_sof) {
      // second word is frag size
      sfrag_size = *(p + 1);
      // third word is frag version [31-16] and type [15-0]
      sfrag_version = *(p + 2) & 0xFFFF;
      sfrag_type = *(p + 2) >> 16;
      b_sof = false;
    }
    // for tmdb we will start to have three sub-fragments ordered as 0x40 0x41 0x42
    // we investigate if there are any of type 0x40,0x41 before going on 0x42
    // like this the loop is faster since we can access the size of this two sub-fragments
    if (sfrag_type == 0x40) {
      ATH_MSG_DEBUG( MSG::hex << " DECODER sub-fragment VERSION=" << sfrag_version
                    << " TYPE=" << sfrag_type << MSG::dec
                    << " SIZE=" << sfrag_size
                    << " FOUND! Keep on looking for 0x42.");
      offset = sfrag_size;
      wc += offset;
      p += offset;
    } else if (sfrag_type == 0x41) {
      ATH_MSG_DEBUG( MSG::hex << " DECODER sub-fragment VERSION=" << sfrag_version
                    << " TYPE=" << sfrag_type << MSG::dec
                    <<" SIZE=" << sfrag_size
                    << " FOUND! Keep on looking for 0x42.");
      offset = sfrag_size;
      wc += offset;
      p += offset;
    } else if (sfrag_type == 0x42) {
      b_deciTmdb = true;
      ATH_MSG_DEBUG( MSG::hex << " DECODER sub-fragment VERSION=" << sfrag_version
                    << " TYPE=" << sfrag_type << MSG::dec
                    << " SIZE=" <<sfrag_size << " FOUND!");
      wc += size - wc;
      p += 3;
    } else {
      ++p;
      ++wc;
    }
  }
  // the TMDB result    0x42
  //
  if (b_deciTmdb) unpack_frag42( sourceid, version, p, sfrag_size-3, cont);

return;
}

void TileROD_Decoder::unpack_frag40(uint32_t collid, uint32_t version, const uint32_t* p, int datasize,
    TileDigitsCollection &coll) {

  int ros = (collid >> 8);
  int drawer = collid & 0xff;
  HWIdentifier drawerID = m_tileHWID->drawer_id(ros, drawer);
  int nsamp = 7; // assume fixed number of samples for the moment
  int nmod = (ros > 2) ? 8 : 4; // we have 8 modules per fragment in ext.barrel, 4 modules in barrel
  int nchan = 4 * datasize / nmod / nsamp; // factor 4 because in one 32-bit word we have 4 samples

  int nsamp1 = nsamp - 1;

  std::vector<float> digits(nsamp);

  // Each 32 bit word is made of four 8-bit words each a ADC of a sample like
  //
  //     32nd bit -> |   37/D6R  |   38/D6L  |  16/D5R  |  17/D5L  | <- 1st bit
  // 
  // First come 8 words (M0->M7) with sample 0 and so forth i.e. the digits for the first module are placed by words: {0,8,16,24,32,40,48}.
  // Note that order of samples is inversed with respect to time line, 
  // i.e. first sample from data should go to the last sample in TileDigits vector

  int wpos = (collid % nmod) * nchan; // location of first sample for given channel in given module
  int jump = nchan * nmod; // distance between samples for one channel in number of bytes
  const unsigned char * adc = reinterpret_cast<const unsigned char *>(p);

  for (int i = 0; i < nchan; ++i) {
    for (int j = 0; j < nsamp; ++j) {
      digits[nsamp1 - j] = adc[wpos + jump * j];
    }
    ++wpos;
    HWIdentifier adcID = m_tileHWID->adc_id(drawerID, i, TileID::LOWGAIN);
    TileDigits *digitmp = new TileDigits(adcID, digits);
    coll.push_back(digitmp);
  }

  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << "TileROD_Decoder::unpack_frag40  frag: 0x" << MSG::hex << collid << MSG::dec
                    << " version: " << version << endmsg;

    int pos = coll.size() - nchan;
    const char * strchannel[5] = { " d5L ", " d5R ", " d6L ", " d6R ", " xxx " };
    for (int i = 0; i < nchan; ++i) {
      const std::vector<float> & sample = coll.at(pos + i)->samples();
      std::stringstream ss;
      for (const auto & val : sample) {
        ss << std::setw(5) << int(val);
      }
      msg(MSG::DEBUG) << " ros:" << ros
                      << " drawer:" << drawer
                      << " ch:" << i << strchannel[std::min(i, 4)] << ss.str() << endmsg;
    }
  }

  return;
}

void TileROD_Decoder::unpack_frag41( uint32_t collid, uint32_t version, const uint32_t* p, int datasize, TileRawChannelCollection &coll ){

  int ros=(collid>>8);
  int drawer=collid&0xff;
  HWIdentifier drawerID = m_tileHWID->drawer_id(ros,drawer);
  float calFactorADC2MeV = 1.0;

  int nmod = (ros>2)?8:4; // we have 8 modules per fragment in ext.barrel, 4 modules in barrel
  int nchan = datasize / nmod;

  int wpos = (collid % nmod) * nchan;

  if (version == 0) {

    // Each 32 bit word is an energy of a channel pmt
    //
    //    32nd bit -> |                     17/D5L                  | <- 1st bit
    //    32nd bit -> |                     16/D5R                  | <- 1st bit
    //    32nd bit -> |                     37/D6L                  | <- 1st bit
    //    32nd bit -> |                     38/D6R                  | <- 1st bit
    //
    // Each 4 words respect a module. 

    const int32_t * pp = reinterpret_cast<const int32_t *>(p);
    pp += wpos;
    for (int i = 0; i < nchan; ++i) {
      HWIdentifier adcID = m_tileHWID->adc_id(drawerID, i, TileID::LOWGAIN);
      TileRawChannel *rctmp = new TileRawChannel(adcID, (*pp) * calFactorADC2MeV, 0., 0.);
      coll.push_back(rctmp);
      ++pp;
    }

  } else {

    // Each 32 bit word is made of two 16-bit words each a energy of a channel pmt
    //
    //    32nd bit -> |         16/D5R        |        17/D5L       | <- 1st bit
    //    32nd bit -> |         38/D6R        |        37/D6L       | <- 1st bit
    //
    // Each 2 words respect a module. 
      
    nchan *= 2; // change number of channels calculated before assuming 16 bits for channel
    const int16_t * pp = reinterpret_cast<const int16_t *>(p);
    pp += wpos;
    for (int i = 0; i < nchan; ++i) {
      HWIdentifier adcID = m_tileHWID->adc_id(drawerID, i, TileID::LOWGAIN);
      TileRawChannel *rctmp = new TileRawChannel(adcID, (*pp) * calFactorADC2MeV, 0., 0.);
      coll.push_back(rctmp);
      ++pp;
    }
  }
  
  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << " TileROD_Decoder::unpack_frag41  frag: 0x" << MSG::hex << collid << MSG::dec
                    << " version: " << version << endmsg;

    int nbits = (version == 0) ? 32 : 16;
    msg(MSG::DEBUG) << " position of " << nbits << "-bit word inside sub-fragment 0x41: " << wpos << endmsg;
    int pos = coll.size() - nchan;
    const char * strchannel[5] = { " d5L ", " d5R ", " d6L ", " d6R ", " xxx " };
    for (int i = 0; i < nchan; ++i) {
      msg(MSG::DEBUG) << " ros:" << ros
                      << " drawer:" << drawer
                      << " ch:" << i << strchannel[std::min(i, 4)] << coll.at(pos + i)->amplitude() << endmsg;
    }
  }

  return;
}

void TileROD_Decoder::unpack_frag42( uint32_t sourceid, uint32_t version, const uint32_t* p, int datasize, TileMuonReceiverContainer &v ){

  // results are hold in 3 16-bit words.
  //
  //    32nd bit -> |        results2       ||        results1       | <- 1st bit
  //                |          0x0          ||        results3       |
  //
  //    32nd bit -> | m-5 | m-4 | m-3 | m-2 || m-2 | m-1 | m-0 | 0x0 | <- 1st bit
  //                |          0x0          || 0x0 | m-7 | m-6 | m-5 |
  //
  // each 4 bit word is
  //                   0      1      2     3    <-- in Obj
  //                | d56h | d56l | d6h | d6l |
  //                  bit3   bit2  bit1  bit0 
  //
  // bit-3 -> result.at(0)
  // bit-2        .
  // bit-1        .
  // bit-0 -> result.at(3)

  int nbit,nmod;
  uint32_t word;
  int drawer = (sourceid & 0xf0000) >> 8; // get ROS number from source ID
  if (drawer<0x300) { // barrel
    nbit = 4;
    nmod = 4;
    drawer |= ((sourceid & 0x0000f) << 2);
    word = (datasize > 0) ? p[0] : 0; // just use first word, although expect all zeros for the moment
  } else {
    nbit = 4;
    nmod = 8;
    drawer |= ((sourceid & 0x0000f) << 3);
    word = (datasize > 1) ? (p[1] << 20) | ((p[0] >> 8) & 0xff000) | ((p[0] >> 4) & 0xfff) : 0;
  }

  std::vector<bool> result(nbit);
  for (int j = 0; j < nmod; ++j) { // loop over modules
    for (int k = nbit-1; k >= 0; --k) { // loop over bits of one module in inversed order
      result[k] = ((word & 1) != 0);
      word >>= 1;
    }

    TileMuonReceiverObj * obj = new TileMuonReceiverObj(drawer, result);
    v.push_back(obj);

    ++drawer;
  }
  
  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << " TileROD_Decoder::unpack_frag42  source ID: 0x" << MSG::hex << sourceid << MSG::dec
                    << " version: " << version << endmsg;

    for (size_t j = v.size() - nmod; j < v.size(); ++j) {
      const std::vector<bool> & r = v[j]->GetDecision();
      std::stringstream ss;
      for (const auto & val : r) {
        ss << std::setw(2) << val;
      }
      msg(MSG::DEBUG) << MSG::hex << "0x" << v[j]->GetID() << MSG::dec << ss.str() << endmsg;
    }
  }

  return;
}

