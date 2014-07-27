/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Implementation of TileROD_Encoder class 

#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "TileByteStream/TileROD_Encoder.h"
#include "TileIdentifier/TileHWID.h"
#include "TileEvent/TileFastRawChannel.h"
#include "TileEvent/TileDigits.h"
#include "TileEvent/TileL2.h"

#include <algorithm> 
#include <cassert>
#include <cmath>

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
    m_rc2bytes4.setUnit(rChUnit);
  } else if (m_type == 5) {
    // 8 upper bits:
    // UULLLTTT
    // 31,30 - units
    // 29,28,27 - length of Level2 part ( = 3 - sumEt, sumEz, sumE )
    // 24,25,26 - OF type
    m_unitType = (rChUnit << 30) | (3 << 27) | (OFType << 24) | (m_type << 16);
    m_rc2bytes5.setUnit(rChUnit);
  }
}

/** convert all TileRawChannels in the current list to 
 a vector of 32bit words
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
  std::vector<const TileL2*>::const_iterator it = m_vTileL2.begin();
  std::vector<const TileL2*>::const_iterator it_end = m_vTileL2.end();

  int currentFrag(-1);
  std::vector<uint32_t>::size_type start = 0;

  int NMuons1 = 0, NMuons2 = 0, frag = 0;

  for (; it != it_end; ++it) {

    const TileL2* l2 = (*it);

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
  std::vector<const TileL2*>::const_iterator it = m_vTileL2.begin();
  std::vector<const TileL2*>::const_iterator it_end = m_vTileL2.end();

  int currentFrag(-1);
  std::vector<uint32_t>::size_type start = 0;

  int frag = 0;

  for (; it != it_end; ++it) {

    const TileL2* l2 = (*it);

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

  std::vector<const TileFastRawChannel*>::const_iterator it = m_vTileRC.begin();
  std::vector<const TileFastRawChannel*>::const_iterator it_end = m_vTileRC.end();

  int currentFrag(-1);
  std::vector<uint32_t>::size_type start = 0;

  for (; it != it_end; ++it) {
    const TileFastRawChannel* rc = (*it);
    int frag = rc->frag() | m_unitType; // FRAG TYPE in upper half of the word

    if (frag != currentFrag) {
      currentFrag = frag;

      // very first word is size of the fragment, which is 50 words
      v.push_back(50);
      // next word is frag ID
      v.push_back(frag);

      // remember where is the first channel
      start = v.size();

      // reserve 48 words for all channels in the drawer
      for (int i = 0; i < 48; ++i)
        v.push_back(0);
    }

    // FIXME:: protection against both low and high gain amplitude
    // for the same raw channel,
    // if this is the case all channels should have both
    // low and high gain and we should use different fragment type

    int chan = rc->channel();
    int gain = rc->adc();
    if (chan < 48) {
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

  std::vector<const TileFastRawChannel*>::const_iterator it = m_vTileRC.begin();
  std::vector<const TileFastRawChannel*>::const_iterator it_end = m_vTileRC.end();

  int currentFrag(-1);
  bool first = true;
  short wc16 = 0;
  std::vector<uint32_t>::size_type head = 0;
  std::vector<uint32_t>::size_type count = 0;

  for (; it != it_end; ++it) {
    const TileFastRawChannel* rc = (*it);
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

  std::vector<const TileFastRawChannel*>::const_iterator it = m_vTileRC.begin();
  std::vector<const TileFastRawChannel*>::const_iterator it_end = m_vTileRC.end();

  int currentFrag(-1);
  std::vector<uint32_t>::size_type start = 0;

  for (; it != it_end; ++it) {
    const TileFastRawChannel* rc = (*it);
    int frag = rc->frag() | m_unitType; // FRAG TYPE in upper half of the word

    if (frag != currentFrag) {
      currentFrag = frag;

      // very first word is start fragment identifier
      v.push_back(0xff1234ff);
      // next word is frag size 54 = 48 + frag ID + frag size + start frag + 3 sumE words
      v.push_back(54);
      // next word is frag ID
      v.push_back(frag);

      // remember where is the first channel
      start = v.size();

      // reserve 48 words for all channels in the drawer and 3 sumE words
      for (int i = 0; i < 51; ++i)
        v.push_back(0);
    }

    // FIXME:: protection against both low and high gain amplitude
    // for the same raw channel,
    // if this is the case all channels should have both
    // low and high gain and we should use different fragment type

    int chan = rc->channel();
    int gain = rc->adc();
    if (chan < 48) {
      v[start + chan] = m_rc2bytes4.getWord(rc, gain);
    }

  } // end of all TileRawChannel

  // dumpROD (v) ;

  return;
}

void TileROD_Encoder::fillRODL2(std::vector<uint32_t>& v) {
  std::vector<const TileL2*>::const_iterator it = m_vTileL2.begin();
  std::vector<const TileL2*>::const_iterator it_end = m_vTileL2.end();

  std::map<int, const TileL2*> l2_map;

  for (; it != it_end; ++it) {
    const TileL2* l2 = (*it);
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

  std::vector<const TileDigits*>::const_iterator it = m_vTileDigi.begin();
  std::vector<const TileDigits*>::const_iterator it_end = m_vTileDigi.end();

  int currentFrag = -1, pos = 0, size = 0;
  for (; it != it_end; it++) {
    const TileDigits* digi = (*it);
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
  std::cout << " Dump of Tile ROD block, size =  " << v.size() << std::endl;
  std::vector<uint32_t>::const_iterator it = v.begin();
  std::vector<uint32_t>::const_iterator it_end = v.end();

  int count = 0, newCount = 3;
  for (; it != it_end; ++it) {
    if (count == 0) {
      msg(MSG::VERBOSE) << "Frag delim = 0x" << std::hex << (*it) << std::dec << endmsg;
    } else if (count == -1) {
      newCount = (*it);
      msg(MSG::VERBOSE) << "Word count = " << newCount << endmsg;
    } else if (count == -2) {
      count += newCount;
      msg(MSG::VERBOSE) << "Frag ID = 0x" << std::hex << (*it) << std::dec << endmsg;
    } else {
      msg(MSG::VERBOSE) << " WORD[" << newCount - count << "] = "
                        << (*it) << " = 0x" << std::hex << (*it)
                        << std::dec << endmsg;
    }
    --count;
  }

}
