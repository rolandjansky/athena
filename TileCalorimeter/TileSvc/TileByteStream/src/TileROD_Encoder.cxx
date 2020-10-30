/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Implementation of TileROD_Encoder class 

#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "TileByteStream/TileROD_Encoder.h"
#include "TileIdentifier/TileHWID.h"
#include "TileEvent/TileFastRawChannel.h"
#include "TileEvent/TileDigits.h"
#include "TileEvent/TileL2.h"
#include "TileEvent/TileMuonReceiverObj.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"

#include <iomanip>
#include <sstream>
#include <algorithm> 
#include <cassert>
#include <cmath>


TileROD_Encoder::TileROD_Encoder(): 
  AthMessaging (Athena::getMessageSvc(), "TileROD_Encoder"),
  m_tileHWID(0), 
  m_verbose(false), 
  m_type(0), 
  m_unitType(0),
  m_rChUnit(0),
  m_maxChannels(TileCalibUtils::MAX_CHAN) {
}


void TileROD_Encoder::setTileHWID(const TileHWID* tileHWID, bool verbose, unsigned int type) {
  m_tileHWID = tileHWID;
  m_verbose = verbose;
  m_type = type;
  m_unitType = (m_type << 16);

  switch (type) {
    case 4:
      m_rc2bytes4.setVerbose(verbose);
      break;
    case 5:
      m_rc2bytes5.setVerbose(verbose);
      break;
    case 3:
      m_rc2bytes3.setVerbose(verbose);
      break;
    case 2:
      m_rc2bytes2.setVerbose(verbose);
      break;
    default:
      break;
  }
}

void TileROD_Encoder::setTileHWID(const TileHWID* tileHWID) {
   m_tileHWID = tileHWID;
}

void TileROD_Encoder::setTypeAndUnit(TileFragHash::TYPE type, TileRawChannelUnit::UNIT unit) {

  unsigned int OFType = (unsigned int) type;
  if (OFType > 7) {
    OFType = 0;
    ATH_MSG_ERROR( "setTypeAndUnit Incorrect raw data type =" << type
                  << " using type=0 instead " );
  }

  unsigned int rChUnit = (unsigned int) unit % (unsigned int) TileRawChannelUnit::OnlineOffset;
  if (rChUnit > 3) {
    rChUnit = 0;
    ATH_MSG_ERROR( "setTypeAndUnit Incorrect raw data unit =" << unit
                  << " assuming ADC counts (0) " );
  }

  // make sure that we use frag type 4 for units which are not ADC counts
  if (rChUnit != 0 && m_type < 4) m_type = 4;
  if (m_type == 4) { // new fragments have non-zero upper byte
    // 8 upper bits:
    // UUPPSTTT
    // 31,30 - units
    // 29,28 - pulse type ( = 3 for simulated data)
    // 27    - 7(=0) or 9(=1) samples (assume 7 samples for now)
    // 24,25,26 - OF type
    // next 8 bits - frag type ( 2,3,4, ... )
    m_unitType = (rChUnit << 30) | (3 << 28) | (0 << 27) | (OFType << 24) | (m_type << 16);
    m_rChUnit = rChUnit;
  } else if (m_type == 5) {
    // 8 upper bits:
    // UULLLTTT
    // 31,30 - units
    // 29,28,27 - length of Level2 part ( = 3 - sumEt, sumEz, sumE )
    // 24,25,26 - OF type
    m_unitType = (rChUnit << 30) | (3 << 27) | (OFType << 24) | (m_type << 16);
    m_rChUnit = rChUnit;
  }

}

void TileROD_Encoder::setMaxChannels(int maxChannels) {
  m_maxChannels = maxChannels;
}


/** convert all TileRawChannels in the current list to a vector of 32bit words
*/

void TileROD_Encoder::fillROD(std::vector<uint32_t>& v) {
  switch (m_type) {
    case 4:
      fillROD4(v);
      break;
    case 5:
      fillROD5(v);
      break;
    case 3:
      fillROD3(v);
      break;
    case 2:
      fillROD2(v);
      break;
    case 1:
      fillROD1(v);
      break;
    case 0x10:
      fillROD10(v);
      break;
    case 0x12:
      fillROD12(v);
      break;
    default:
      ATH_MSG_ERROR( "fillROD -> Unknown packing type " << m_type );
      assert(0);
      break;
      // return;
  }
}


void TileROD_Encoder::fillROD10(std::vector<uint32_t>& v) {

  int currentFrag(-1);
  std::vector<uint32_t>::size_type start = 0;

  int NMuons1 = 0, NMuons2 = 0, frag = 0;

  for (const TileL2* l2 : m_vTileL2) {

    int ros = ((l2->identify()) | 0xff) >> 8;
    int drawer = (l2->identify()) & 0xff;

    if ((drawer % 2) == 0) frag = 0x00100000 + (ros << 12) + ((drawer + 1) << 6) + drawer;

    if (frag != currentFrag) { // superdrawer is even

      currentFrag = frag;

      // fragment marker
      v.push_back(0xff1234ff);

      // fragment size (variable depending on the number of muons found)
      v.push_back(5);

      // fragment ID
      v.push_back(frag);

      // first word after fragment header
      start = v.size();

      // reserve 2 words for Et
      for (int i = 0; i < 2; ++i)
        v.push_back(0);

      // Et superdrawer #1
      int wet = (int) round(l2->sumEt());
      v[start] = (unsigned int) (wet + 9000); // shift by 9000 to be compatible with frag 0x10 format

      // MTag superdrawer #1
      NMuons1 = l2->NMuons();
      for (int i = 0; i < 2 * NMuons1; ++i)
        v.push_back(l2->val(i));

    } else { // superdrawer is odd

      // Et superdrawer #2
      int wet = (int) round(l2->sumEt());
      v[start + 1] = (unsigned int) (wet + 9000); // shift by 9000 to be compatible with frag 0x10 format

      // MTag superdrawer #2
      NMuons2 = l2->NMuons();
      for (int i = 0; i < 2 * NMuons2; ++i)
        v.push_back(l2->val(i));

      // re-write fragment size
      v[start - 2] = 5 + 2 * NMuons1 + 2 * NMuons2;

    }

  }

  //assert(0);
}

void TileROD_Encoder::fillROD12(std::vector<uint32_t>& v) {

  int currentFrag(-1);
  std::vector<uint32_t>::size_type start = 0;

  int frag = 0;

  for (const TileL2* l2 : m_vTileL2) {

    int ros = (l2->identify()) >> 8;
    int drawer = (l2->identify()) & 0xff;

    if ((drawer % 2) == 0)
      frag = 0x120000 + (ros << 12) + ((drawer + 1) << 6) + drawer;
    else
      frag = 0x120000 + (ros << 12) + (drawer << 6) + (drawer - 1);

    if (frag != currentFrag) { // superdrawer is even

      currentFrag = frag;

      // fragment marker
      v.push_back(0xff1234ff);

      // fragment size (variable depending on the number of muons found)
      v.push_back(3);

      // fragment ID
      v.push_back(frag);

      // first word after fragment header
      start = v.size();
    }

    // MTag data
    int Ndata = l2->Ndata();
    if (Ndata) {
      for (int i = 0; i < Ndata; ++i)
        v.push_back(l2->val(i));

      // re-write fragment size
      v[start - 2] += Ndata;
    }
  }

  //assert(0);
}

void TileROD_Encoder::fillROD2(std::vector<uint32_t>& v) {
  //std::sort(m_vTileRC.begin(), m_vTileRC.end(), m_order);

  int currentFrag(-1);
  std::vector<uint32_t>::size_type start = 0;

  for (const TileFastRawChannel* rc : m_vTileRC) {

    int frag = rc->frag() | m_unitType; // FRAG TYPE in upper half of the word

    if (frag != currentFrag) {
      currentFrag = frag;

      // very first word is size of the fragment, which is 50 words
      v.push_back(50);
      // next word is frag ID
      v.push_back(frag);

      // remember where is the first channel
      start = v.size();
      // reserve maximum number of channels in a drawer words 
      // for all channels in the drawer
      v.resize(start + m_maxChannels, 0);

    }

    // FIXME:: protection against both low and high gain amplitude
    // for the same raw channel,
    // if this is the case all channels should have both
    // low and high gain and we should use different fragment type

    int chan = rc->channel();
    int gain = rc->adc();
    if (chan < m_maxChannels) {
      v[start + chan] = m_rc2bytes2.getWord(rc, gain);
    }

  } // end of all TileRawChannel

  // dumpROD (v) ;

  return;
}

void TileROD_Encoder::fillROD3(std::vector<uint32_t>& v) {
  // get a reference to vector<short>, through static_cast
  // be very careful with v16... make sure it is aligned 
  std::vector<short>* p16;
  p16 = (std::vector<short>*) (&v);
  std::vector<short>& v16 = *p16;

  //std::sort(m_vTileRC.begin(), m_vTileRC.end(), m_order); 

  int currentFrag(-1);
  bool first = true;
  short wc16 = 0;
  std::vector<uint32_t>::size_type head = 0;
  std::vector<uint32_t>::size_type count = 0;

  for (const TileFastRawChannel* rc : m_vTileRC) {


    int frag = rc->frag() | m_unitType; // FRAG TYPE in upper half of the word
    int chan = rc->channel();
    int gain = rc->adc();

    if (frag != currentFrag) {
      currentFrag = frag;
      // a new frag
      if (!first) {
        // close the current frag
        if ((v16.size() % 2) == 1) {
          v16.push_back(0);
          ++wc16;
        }
        // inclusive word (32bit) for this frag
        v[count] = wc16 / 2;
      } else
        first = false;

      // very first word is size of the fragment
      // remember where it is
      count = v.size();
      v.push_back(0);
      //  add frag ID
      v.push_back(frag);

      // remember where map starts
      head = v.size();
      // 2 words (64 bits) for channel map
      v.push_back(0);
      v.push_back(0);
      wc16 = 8; // we stored 4 full words (8 shorts) already
    }

    // FIXME:: protection against both low and high gain amplitude
    // for the same raw channel,
    // if this is the case all channels should have both
    // low and high gain and we should use different fragment type

    if (checkBit(&(v[head]), chan)) {
      // the same channel with another gain alreay exists, ignore second one
    } else {
      // get the shorts, and increment wc16 by number of shorts written. 
      wc16 += m_rc2bytes3.getBytes(rc, gain, v16);
      // set bitmap for this channel
      setBit(&(v[head]), chan);
    }

  } // end of all TileRawChannel

  if (!first) {
    // close the last Frag
    if ((v16.size() % 2) == 1) {
      v16.push_back(0);
      ++wc16;
    }
    // inclusive word (32bit) for this frag
    v[count] = wc16 / 2;
  }

  // dumpROD (v) ;

  return;
}

void TileROD_Encoder::fillROD4(std::vector<uint32_t>& v) {
  //std::sort(m_vTileRC.begin(), m_vTileRC.end(), m_order);

  int currentFrag(-1);
  std::vector<uint32_t>::size_type start = 0;

  for (const TileFastRawChannel* rc : m_vTileRC) {

    int frag = rc->frag() | m_unitType; // FRAG TYPE in upper half of the word

    if (frag != currentFrag) {
      currentFrag = frag;

      // very first word is start fragment identifier
      v.push_back(0xff1234ff);
      // next word is frag size: 
      // (maximum number of channels in a drawer) + ...
      // ... + (6 = start frag + frag size + frag ID + 3 sumE words)
      v.push_back(m_maxChannels + 6);
      // next word is frag ID
      v.push_back(frag);

      // remember where is the first channel
      start = v.size();

      // reserve (maximum number of channels in a drawer) words 
      // for all channels in the drawer and 3 sumE words
      v.resize(start + m_maxChannels + 3, 0);

    }

    // FIXME:: protection against both low and high gain amplitude
    // for the same raw channel,
    // if this is the case all channels should have both
    // low and high gain and we should use different fragment type

    int chan = rc->channel();
    int gain = rc->adc();
    if (chan < m_maxChannels) {
      v[start + chan] = m_rc2bytes4.getWord(*rc, m_rChUnit, gain);
    }

  } // end of all TileRawChannel

  // dumpROD (v) ;

  return;
}

void TileROD_Encoder::fillRODL2(std::vector<uint32_t>& v) {

  std::map<int, const TileL2*> l2_map;

  for (const TileL2* l2 : m_vTileL2) {
    l2_map[l2->identify()] = l2;
    //std::cout << "l2 id=0x"<<std::hex<<l2->identify()<<std::dec<<std::endl;
  }

  std::vector<uint32_t>::size_type start = 3;

  while (start <= v.size()) {
    uint32_t size = v[start - 2];
    uint32_t fragtype = v[start - 1];
    int type = (fragtype >> 16) & 0xFF;
    //std::cout <<std::hex<<"frag 0x"<<fragtype<<std::dec<<" size "<<size<<std::endl;
    if ((type == 4 && size > 53) || (type == 5 && ((fragtype >> 27) & 7) > 2)) {
      int frag = fragtype & 0xFFFF;
      std::map<int, const TileL2*>::const_iterator l2_it = l2_map.find(frag);
      if (l2_it != l2_map.end()) {
        int unit = (fragtype >> 30);
        const TileL2* l2 = l2_it->second;
        int et = round(l2->sumEt() * AMPLITUDE_FACTOR5_HG[unit]);
        v[start + 48] = (uint32_t) et;
        int ez = round(l2->sumEz() * AMPLITUDE_FACTOR5_HG[unit]);
        v[start + 49] = (uint32_t) ez;
        int e = round(l2->sumE() * AMPLITUDE_FACTOR5_HG[unit]);
        v[start + 50] = (uint32_t) e;
        //std::cout << "Found "<<et<<","<<ez<<","<<e<<std::endl;
      }
    }
    start += size;
  }

  if (msgLvl(MSG::VERBOSE)) dumpROD(v);

  return;
}

void TileROD_Encoder::fillROD1(std::vector<uint32_t>& v) {

  std::sort(m_vTileDigi.begin(), m_vTileDigi.end(), m_order);

  int currentFrag = -1, pos = 0, size = 0;
  for (const TileDigits* digi : m_vTileDigi) {
    
    HWIdentifier adcID = digi->adc_HWID();
    int frag = m_tileHWID->frag(adcID) | (0x01 << 16); // FRAG TYPE = 1 in upper half of the word
    if (frag != currentFrag) {
      if (currentFrag != -1) v[pos] = size;
      currentFrag = frag;
      // first word is start of fragment identifier
      v.push_back(0xFF1234FF);
      // next word is frag size
      pos = v.size();
      v.push_back(0);
      // next word is frag ID
      v.push_back(frag);
      size = 3;
    }
    size += m_Digi2bytes.getBytes(digi, m_tileHWID, v);
  }
  if (currentFrag != -1) v[pos] = size;
  return;
}

void TileROD_Encoder::fillROD5(std::vector<uint32_t>& /* v */) {
  ATH_MSG_ERROR( "fillROD5 -> store raw channels in frag5 - not yet implemented " );
}

void TileROD_Encoder::fillROD5D(std::vector<uint32_t>& /* v */) {
  ATH_MSG_ERROR( "fillROD5D -> store digits in frag5 - not yet implemented " );
}

// == START of TMDB Encoders: Digits, Raw Channel, Decision

// TMDB Digits

void TileROD_Encoder::fillRODTileMuRcvDigi(std::vector<uint32_t>&  v) {

  ATH_MSG_DEBUG( "TMDB encoding sub-fragment 0x40: loop over " << m_vTileDigi.size() << " objects" );

  // sub-fragment marker
  // 
  v.push_back(0xff1234ff);

  // sub-fragment size
  // set the size for the sub-fragment (3 [header] + 8 [# 32bit word/digit/pmt/module] x N [# digit/pmt/module])
  uint32_t size = m_vTileDigi.size()  * 7; // assume 7 samples
  size = (size+3)>>2; // convert numner of bytes into number of 32bit words
  v.push_back(3+size);
  uint savepos=v.size()-1;

  // type & version: the version is a 16-bit number and is set by fixing the 3th hexadecimal digit (8-12 in bits) to 5 and leaving all other free
  //
  uint32_t verfrag      = 0x500;
  uint32_t type_version = (0x40 << 16) + verfrag; 	
  v.push_back(type_version);

  v.resize(v.size()+size); // prepare place for extra words

  // counters and temporary words
  //
  int  word8bit_cnt = 0;// number of 8bit words collected 1..4
  int  wc           = 0;// number of blocks of 7 32-bit words saved in ROD fragment 1..8
  int  chc          = 0;// number of digits inside the tile digits collection
  int  nwc = (m_vTileDigi.size() + 3)>>2; // convert number of channels into number of 4-byte blocks
  uint nsamp = 7;
  uint32_t word[7];
  memset(word, 0, sizeof(word));

  for (const TileDigits* digi : m_vTileDigi) {

    if (wc==nwc) {
      ATH_MSG_WARNING( "Too many channels per fragment for TMDB frag 0x40 - ignoring all the rest" );
      break;
    }
  
    // Get identifier(s) and digits for later usage
    //
    std::vector<float> digits = digi->samples();
    nsamp = digits.size();
    if (nsamp>7) {
      ATH_MSG_WARNING( "Too many samples in digits for TMDB frag 0x40, using first 7 instead of "<<nsamp );
      nsamp=7;
      digits.resize(nsamp);
    }

    // Digits from TMDB come to fragment in the reverse order i.e. s1->s7 ... s7->s1
    //
    std::reverse(digits.begin(),digits.end());
    
    // Define two counters: (a) to count for the 8bit sub-fragments (each word has 4 8bit sub-fragments) 
    //                      (b) to count for the 32bit words (each digit has 7 32bit words)
    //
    //                      |  s(i)-m(j)-d6r  |  s(i)-m(j)-d6l  |  s(i)-m(j)-d5r  |  s(i)-m(j)-d5l  |
    //
    int shift = word8bit_cnt * 8;
    for ( uint i=0; i<nsamp; ++i ) {
      word[i]  += ((int) digits[i]) << shift; 
    }

    if (msgLvl(MSG::DEBUG)) {
      HWIdentifier hwid = digi->adc_HWID();
      int ros     = m_tileHWID->ros(hwid);
      int drawer  = m_tileHWID->drawer(hwid);
      int channel = m_tileHWID->channel(hwid);
      const char * strchannel[5] = {" d5L "," d5R "," d6L "," d6R ", " xxx "};
      int j=std::min(channel,4);
      if (ros<3) j=4;
      for ( uint i=0; i<nsamp; ++i ) {
        msg(MSG::DEBUG) << ros << "/" << drawer << "/" << channel << strchannel[j]
                        <<"\tSample "<<7-i<<" bits |" << std::setfill('0') << std::setw(2) 
                        << shift << "-" << std::setw(2) << shift+7 << std::setfill('0') 
                        << "| of 32-bit word "<<3 + nwc*i + wc<<" "<<digits[i]
                        <<" "<<MSG::hex<<word[i]<<MSG::dec << endmsg;
      }
    }

    ++word8bit_cnt;

    // When word8bit_cnt=4 a set of 32bit words word[0..6] are completed and are saved at a position inside the ROD fragment vector:
    //
    //   1st 32 bit word holds : | digit0 pmt3 mod0 | digit0 pmt2 mod0 | digit0 pmt1 mod0 | digit0 pmt0 mod0 |
    //   2nd 32 bit word holds : | digit0 pmt3 mod1 | digit0 pmt2 mod1 | digit0 pmt1 mod1 | digit0 pmt0 mod1 |
    //   ...
    //   8th 32 bit word holds : | digit0 pmt3 mod7 | digit0 pmt2 mod7 | digit0 pmt1 mod7 | digit0 pmt0 mod7 |
    //   ...
    //
    // word8bit_cnt is reset to 0 and a new set of words[0..6] is restarted.
    //
    if ( word8bit_cnt == 4 ) {
      for ( uint i=0; i<nsamp; ++i ) {
        v.at( 3 + nwc*i + wc ) = word[i];
      }
      ++wc;
      word8bit_cnt=0;
      memset(word, 0, sizeof(word));
    }
    ++chc;
  }

  if ( word8bit_cnt != 0  && wc<nwc ) { // some extra channels 
    ATH_MSG_WARNING( "Unexpected number of channels for TMDB frag 0x40" << wc*4 + word8bit_cnt );
    for ( uint i=0; i<nsamp; ++i ) {
      v.at( 3 + nwc*i + wc ) = word[i];
    }
    ++wc;
    word8bit_cnt=0;
    memset(word, 0 ,sizeof(word));
  }

  v.at(savepos)=3+size;  // not actually needed - size was already set correctly

  ATH_MSG_DEBUG( "Check version and counters: "<<MSG::hex<< verfrag <<MSG::dec<<" "<< chc <<" "<< wc << " save in position: " << savepos );

  // dump fragment
  //	
  if (msgLvl(MSG::VERBOSE)) {
    msg(MSG::VERBOSE) << "Check content of ROD fragment after including sub-fragment (0x40)... " << v.size() << endmsg;
    for (size_t i=0; i<v.size(); ++i)
      msg(MSG::VERBOSE) << i << "\t" << v.at(i) << MSG::hex << " 0x" << v.at(i) << MSG::dec << endmsg;
  }

  return;
}

// TMDB Raw Channel

void TileROD_Encoder::fillRODTileMuRcvRawChannel(std::vector<uint32_t>&  v) {

  ATH_MSG_DEBUG( "TMDB encoding sub-fragment 0x41: loop over " << m_vTileRC.size() << " objects" );

  const float TMDB_AMPLITUDE_FACTOR = 1.0;

  // sub-fragment marker
  //
  v.push_back(0xff1234ff);

  // sub-fragment size
  //
  v.push_back(3);
  uint savepos=v.size()-1;

  // type & version: the version is a 16-bit number and is set by fixing the 3th hexadecimal digit (8-12 in bits) to 5 and leaving all other free
  //
  uint32_t verfrag      = 0x500;
  // FIXME: for the moment (July 2015) we use 32-bit packing only, so we hide 16-bit version under ifdef
  // if we decide to use 16-bit version, additional flag for encoder should be passed from top-level algorithm
#ifdef ALLOW16BIT
  if (use_16bit_packing) verfrag += 2;
  int32_t word16 = 0x0;
#endif
  uint32_t type_version = (0x41 << 16) + verfrag;
  v.push_back(type_version);

  // counters and temporary words
  //
  int wc       = 0;
  int chc      = 0;
  uint32_t word  = 0x0;

  for (const TileFastRawChannel* rc : m_vTileRC) {

    // energies of individual pmts 2 words per module  |  d5r(16)  |  d5l(17)  |
    //                                                 |  d6r(37)  |  d6l(38)  | 
    //
    float    f_amp = rc -> amplitude();
    int32_t  i_amp = lround(f_amp*TMDB_AMPLITUDE_FACTOR) ;

  // FIXME: for the moment (July 2015) we use 32-bit packing only, so we hide 16-bit version under ifdef
#ifdef ALLOW16BIT
    switch (verfrag){
    case 0x502:
      // 2's complement (binary conversion for negative numbers) 
      //    x in R and bin_x the binary conversion of x ; (if x<0) then bin_x = 0xffff - abs(x) + 1 (else) bin_x = abs(x)
      //
      // v0 : - Feb'15: each 32-bit words holds a cell of a module 16-bit for left side PMT 16 bit for right side pmt
      //
      //   1st 32 bit word holds : |    d5r m0    |    d5l m0    |
      //   2nd 32 bit word holds : |    d6r m0    |    d6l m0    |
      //   ...
      //   7th 32 bit word holds : |    d5r m3    |    d5l m3    |
      //   8th 32 bit word holds : |    d6r m3    |    d6l m3    | 
      //   ...
      //      - While reading the FPGA it was noticed a longer word coming out larger that 16-bit
      //      - Keep this (for now) just for history but it may be that later we understand better the FPGA output
      //
      //limit to the range of 16-bit integer
      if (i_amp>0x7FFF) word16 = 0x7FFF;
      else if (i_amp<-0x8000) word16 = -0x8000;
      else word16 = i_amp;

      if (chc&1) {
        word |= word16 << 16;
        v.push_back(word);
        ++wc;
      } else {
        word = word16 & 0xFFFF;
      }
      break;

    //case 0x500:
    default:
#else
    {
#endif
      // vMay'15 current version each 32-bit word is a channel
      word = (uint32_t)i_amp;
      v.push_back(word);
      ++wc;
    }

    if (msgLvl(MSG::DEBUG)) {
        int frag_id = rc->frag();
        int drawer  = (frag_id&0xFF);
        int ros     = frag_id>>8;
        int channel = rc->channel();
        const char * strchannel[5] = {" d5L "," d5R "," d6L "," d6R ", " xxx "};
        int j=std::min(channel,4);
        if (ros<3) j=4;
        msg(MSG::DEBUG) << ros << "/" << drawer << "/" << channel << strchannel[j] 
                        <<"\tAmp " << f_amp << " " << i_amp << " " 
                        <<" ch cnt " << chc << " word cnt " << wc 
                        << " word 0x" <<MSG::hex<< word <<MSG::dec<<endmsg;
    }
    
    ++chc;
  }

  // FIXME: for the moment (July 2015) we use 32-bit packing only, so we hide 16-bit version under ifdef
#ifdef ALLOW16BIT
  if (verfrag==0x502 && wc*2 != chc) { // odd number of channels for 16-bit frags
    v.push_back(word); // saving last channel
    ++wc;
  }
#endif

  v.at(savepos)=3+wc; 

  ATH_MSG_DEBUG("Check version and counters: "<<MSG::hex<< verfrag <<MSG::dec<<" "<< chc <<" "<< wc <<" save in position: "<< savepos );

  if (msgLvl(MSG::VERBOSE)) {
    msg(MSG::VERBOSE) << "Check content of ROD fragment after including sub-fragment (0x41)... "<< m_vTileRC.size() <<" "<< v.size() << endmsg;
    for (size_t i=0; i<v.size(); ++i) {
      msg(MSG::VERBOSE) << i <<"\t"<< v.at(i) << MSG::hex << " 0x" << v.at(i) << MSG::dec << endmsg;
    }
  }
  return;
}

// TMDB Decision

void TileROD_Encoder::fillRODTileMuRcvObj(std::vector<uint32_t>& v) {

  // this is the subfragment type 0x42	

  ATH_MSG_DEBUG( "TMDB encoding sub-fragment 0x42: loop over " << m_vTileMuRcvObj.size() << " objects" );

  // sub-fragment marker
  //
  v.push_back(0xff1234ff);

  // sub-fragment size
  //
  v.push_back(5);
  uint savepos = v.size()-1;

  // type & version
  //
  uint32_t verfrag      = 0x500;
  uint32_t type_version = (0x42 << 16) + verfrag;
  v.push_back(type_version);
  
  // counters and temporary words
  //
  int wc = 0;
  int chc= 0;
  uint32_t result1      = 0x0;
  uint32_t result2      = 0x0;
  uint32_t result3      = 0x0;

  for (const TileMuonReceiverObj* tmurcv : m_vTileMuRcvObj) {

    // results are hold in 3 16-bit words.
    // 
    //     32nd bit -> |        results2       || results1       | <- 1st bit
    //                 |          0x0          ||        results3 |
    // 
    //     32nd bit -> | m-5 | m-4 | m-3 | m-2 || m-2 | m-1 | m-0 | 0x0 | <- 1st bit
    //                 |          0x0          || 0x0 | m-7 | m-6 | m-5 |
    //                 
    // each 4 bit word is
    //
    //                 0      1      2     3    <-- in Obj
    //              | d56h | d56l | d6h | d6l |
    //                bit3   bit2  bit1  bit0
    //   
  
    int modid = tmurcv->identify() & 0xff;  

    const std::vector<bool> & slin = tmurcv->GetDecision();
    int imax = std::min((int)slin.size(),4);
    uint32_t word4b = 0x0;
    for (int i=0;i<imax;++i){
      // slin   d56h d56l  d6h  d6l
      // word4b bit3 bit2 bit1 bit0
      if (slin[i]) word4b |= 1 << (3-i);
    }
    
    if (msgLvl(MSG::DEBUG)) {
      std::stringstream ss;
      for (const auto & val : slin) {
          ss<<std::setw(2)<<val;
      }
      msg(MSG::DEBUG) << "Result for module: "<<modid<<" in TMDB board "<<modid%8<<MSG::hex<<": 0x"<<word4b<<MSG::dec<<" from "<<ss.str() << endmsg; 
    }
    
    switch (modid%8) {
    case 0: result1 |= word4b << 4  ; break;
    case 1: result1 |= word4b << 8  ; break;
    case 2: result1 |= word4b << 12 ; result2 |= word4b; break;
    case 3: result2 |= word4b << 4  ; break; 
    case 4: result2 |= word4b << 8  ; break;
    case 5: result2 |= word4b << 12 ; result3 |= word4b; break;
    case 6: result3 |= word4b << 4  ; break;
    case 7: result3 |= word4b << 8  ; break;
    }
    ++chc;
  }

  ATH_MSG_DEBUG( "Summary : "<<MSG::hex<<" Results 1: 0x"<<result1<<" Results 2: 0x"<<result2<<" Results 3: 0x"<<result3<< MSG::dec );

  v.push_back( result1 | (result2 << 16) ); ++wc;// | 5 4 3 2 | 2 1 0 - |
  v.push_back( result3 ); ++wc;                  // | - - - - | - 7 6 5 | '-' means free/not set/0x0
  v.at(savepos)=3+wc;

  ATH_MSG_DEBUG( "Check version and counters: "<<MSG::hex<< verfrag <<MSG::dec<<" "<< chc <<" "<< wc <<" save in position: "<< savepos );

  if (msgLvl(MSG::VERBOSE)) {
    msg(MSG::VERBOSE) << "Check content of ROD fragment after including sub-fragment (0x42)... " << v.size() << endmsg;
    for (size_t i=0; i<v.size(); ++i) {
      msg(MSG::VERBOSE) << i << "\t" << v.at(i) << MSG::hex << " 0x" << v.at(i) << MSG::dec << endmsg;
    }
  }
  
  return;	
}

// == END of TMDB Encoders

// set the bit accordingly.  
// the bits are used when reading  TileROD_Decoder::checkBit
void TileROD_Encoder::setBit(uint32_t* p, int chan) {
// chan = (0,47)   
  int a = chan / 32;
  int r = chan % 32;
// a = (0,1), r = ( 0, 31 )
  *(p + a) |= (1 << r);
  return;
}

// check if bit is set
bool TileROD_Encoder::checkBit(const uint32_t* p, int chan) {
// chan = (0,47)   
  int a = chan / 32;
  int r = chan % 32;
// a = (0,1), r = ( 0, 31 )
  return *(p + a) & (1 << r);

}

void TileROD_Encoder::dumpROD(const std::vector<uint32_t>& v) {
  msg(MSG::VERBOSE) << " Dump of Tile ROD block, size =  " << v.size() << endmsg;

  int count = 0, newCount = 3;
  for (const uint32_t data : v) {
    if (count == 0) {
      msg(MSG::VERBOSE) << "Frag delim = 0x" << std::hex << data << std::dec << endmsg;
    } else if (count == -1) {
      newCount = data;
      msg(MSG::VERBOSE) << "Word count = " << newCount << endmsg;
    } else if (count == -2) {
      count += newCount;
      msg(MSG::VERBOSE) << "Frag ID = 0x" << std::hex << data << std::dec << endmsg;
    } else {
      msg(MSG::VERBOSE) << " WORD[" << newCount - count << "] = "
                        << data << " = 0x" << std::hex << data
                        << std::dec << endmsg;
    }
    --count;
  }
}
