/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TileEvent/src/TileDQstatus.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2018
 * @brief Information produced by TileDQstatusAlg
 *        (used to be done by TileBeamInfoProvider).
 */


#include "TileEvent/TileDQstatus.h"
#include "TileEvent/TileRawChannelCollection.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>


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
TileDQstatus::TileDQstatus()
  : m_isBiGain(false),
    m_checkDigi(false),
    m_BCID(0),
    m_incompleteDigits(true),
    m_calibMode(0xFFFFFFFF),
    m_cispar{0},
    m_trigType(0)
{
  setAllGood();
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

// Function to fill error arrays from DQ fragment stores in DSP RawChannelContainer
// If monogain run, both gains contain the same results
void TileDQstatus::fillArrays(const TileRawChannelCollection *coll,
                              const TileDigitsContainer* digitsCnt,
                              int gain, unsigned short fragBCID)
{
  if (digitsCnt) {
    m_checkDigi = true;
  }

  int frag = coll->identify();
  int partition = (frag >> 8); // 0=AUX,1=LBA,2=LBC,3=EBA,4=EBC
  int drawer = (frag & 0x3F);  // 0-63
  bool eb = (frag > 0x2ff);    // true if ext.barrel
  bool ebsp = (frag == 0x30e || frag == 0x411);// EBA15 or EBC18

  m_isBiGain |= (coll->size() > 48);
  // attention! it's assignment below, i.e. only single "=", not "=="
  // LF: ... which is something very dangerous. Does it provide any speed advantage?
  if ((m_GlobalCRCErrArray[partition][drawer][gain] = coll->getFragGlobalCRC()))    ++m_counter;
  if ((m_BCIDErrArray[partition][drawer][gain] = fragBCID))                         ++m_counter;
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

  unsigned short BCIDerr = fragBCID;

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

  if ((m_BCIDErrArray[partition][drawer][gain] & 0x2) && digitsCnt) {
    fillBCIDErrDetail(digitsCnt, frag, gain);
  } else {
    m_BCIDErrArrayDetail[partition][drawer][gain] = fragBCID;
  }
  
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

void TileDQstatus::fillBCIDErrDetail(const TileDigitsContainer* digitsCnt,
                                     int frag, int gain)
{
  int partition = (frag >> 8);
  int drawer = (frag & 0x3F);
  m_BCIDErrArrayDetail[partition][drawer][gain] =
      m_BCIDErrArray[partition][drawer][gain]; //initialize
  if (digitsCnt == NULL)
    // by default a conservative return value of 1 (channel Mask)
    m_BCIDErrArrayDetail[partition][drawer][gain] = short(-1);  
  else {
    TileDigitsContainer::const_iterator collItr = digitsCnt->begin();
    TileDigitsContainer::const_iterator lastColl = digitsCnt->end();

    for (; collItr != lastColl; ++collItr) { // Loop over TileModules
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


void TileDQstatus::fillTrips(unsigned int partition,
                             const std::vector<float>& trips,
                             double* rndmVec,
                             MsgStream& msg)
{
  for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer) {
    if (trips[drawer] > rndmVec[drawer]) {
      msg << MSG::DEBUG
          << "The drawer has been tripped (simulation): "
          << TileCalibUtils::getDrawerString(partition, drawer) << endmsg;
      m_GlobalCRCErrArray[partition][drawer][0] = -1;
      m_GlobalCRCErrArray[partition][drawer][1] = -1;
      m_counter += 2;
    }
  }
}


void TileDQstatus::setIncompleteDigits (bool incomplete)
{
  m_incompleteDigits = incomplete;
}


void TileDQstatus::setCalibMode (uint32_t calibMode)
{
  m_calibMode = calibMode;
}


uint32_t* TileDQstatus::cispar()
{
  return m_cispar;
}


void TileDQstatus::setTrigType (int trigType)
{
  m_trigType = trigType;
}
