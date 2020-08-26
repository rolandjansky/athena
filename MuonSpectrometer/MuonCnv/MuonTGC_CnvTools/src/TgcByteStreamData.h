/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CNVTOOLS_TGCBYTESTREAMDATA_H
#define MUONTGC_CNVTOOLS_TGCBYTESTREAMDATA_H

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

#include <cstring> // This file is needed to use memset. 

const uint32_t TGC_BYTESTREAM_HEADER = 0xee1234ee;
const uint32_t TGC_BYTESTREAM_HEADERSIZE = 0x9;
const uint32_t TGC_BYTESTREAM_HEADERVERSION = 0x03000300;
const uint32_t TGC_BYTESTREAM_STATUSSIZE = 0x5;

/** The struct for source ID in ByteStream. */
struct TGC_BYTESTREAM_SORUCEID
{
  unsigned rodid: 8;
  unsigned fill1: 8;
  unsigned side:  8;
  unsigned fill2: 8;

  TGC_BYTESTREAM_SORUCEID() { memset(this, 0, sizeof(*this)); }
};

/** The struct for errors in ByteStream. */
struct TGC_BYTESTREAM_ERRORS
{
  unsigned badBcID:  1;
  unsigned badL1Id:  1;
  unsigned timedout: 1;
  unsigned badData:  1;
  unsigned overflow: 1;
  unsigned fill1:   11;

  TGC_BYTESTREAM_ERRORS() { memset(this, 0, sizeof(*this)); }
};

/** The struct for local status in ByteStream. */
struct TGC_BYTESTREAM_LOCALSTATUS
{
  unsigned mergedHitBCs:      1;
  unsigned mergedTrackletBCs: 1;
  unsigned sortedHits:        1;
  unsigned sortedTracklets:   1;
  unsigned fill1:            10;
  unsigned hasRoI:            1;
  unsigned fakeSsw:           1;

  unsigned fill2:             1;
  unsigned raw:               1;
  unsigned hitsRO:            1;
  unsigned trkRO:             1;
  unsigned hitsCH:            1;
  unsigned trkCH:             1;
  unsigned fill3:             2;
  unsigned hipt:              1;
  unsigned sl:                1;
  unsigned fill4:             6;

  TGC_BYTESTREAM_LOCALSTATUS() { memset(this, 0, sizeof(*this)); }
};

/** The struct for fragment count in ByteStream. */
struct TGC_BYTESTREAM_FRAGMENTCOUNT
{
  unsigned count: 24;
  unsigned id:     8;
};

/** The struct for hit in ByteStream. */
struct TGC_BYTESTREAM_READOUTHIT
{
  unsigned channel:  8;
  unsigned sbId:     5;
  unsigned sbType:   3;
  unsigned adj:      1;
  unsigned ldbId:    4;
  unsigned bcBitmap: 3;
  unsigned tracklet: 6;
  unsigned fill1:    1;
  unsigned ok:       1;

  TGC_BYTESTREAM_READOUTHIT() { memset(this, 0, sizeof(*this)); }
};

/** The struct for tracklet in ByteStream. */
struct TGC_BYTESTREAM_READOUTTRACKLET
{
  unsigned rphi:     5;
  unsigned subm:     2;
  unsigned seg:      1;
  signed   delta:    4;
  unsigned sbId:     5;
  unsigned ldbId:    4;
  unsigned bcBitmap: 3;
  unsigned trkType:  3;
  unsigned fill1:    1;
  unsigned slbType:  3;
  unsigned ok:       1;

  TGC_BYTESTREAM_READOUTTRACKLET() { memset(this, 0, sizeof(*this)); }
};

/** The struct for triplet strip in ByteStream. */
struct TGC_BYTESTREAM_READOUTTRIPLETSTRIP
{
  unsigned phi:      4;
  unsigned subc:     2;
  unsigned fill1:    1;
  unsigned seg:      1;
  unsigned fill2:    4;
  unsigned sbId:     5;
  unsigned ldbId:    4;
  unsigned bcBitmap: 3;
  unsigned trkType:  3;
  unsigned fill3:    1;
  unsigned slbType:  3;
  unsigned ok:       1;

  TGC_BYTESTREAM_READOUTTRIPLETSTRIP() { memset(this, 0, sizeof(*this)); }
};

/** The struct for HiPt in ByteStream. */
struct TGC_BYTESTREAM_HIPT
{
  signed   delta:    5;
  unsigned sub:      1;
  unsigned hitId:    3;
  unsigned hipt:     1;
  unsigned cand:     1;
  unsigned chip:     2;
  unsigned sector:   3;
  unsigned fwd:      1;
  unsigned strip:    1;
  unsigned bcBitmap: 3;
  unsigned fill1:   11;

  TGC_BYTESTREAM_HIPT() { memset(this, 0, sizeof(*this)); }
};

/** The struct for Inner trigger bits in ByteStream. */
struct TGC_BYTESTREAM_HIPT_INNER
{
  unsigned inner:    8;
  unsigned fill1:    1;
  unsigned hipt:     1;
  unsigned cand:     1;
  unsigned chip:     2;
  unsigned sector:   3;
  unsigned fwd:      1;
  unsigned strip:    1;
  unsigned bcBitmap: 3;
  unsigned fill2:   11;

  TGC_BYTESTREAM_HIPT_INNER() { memset(this, 0, sizeof(*this)); }
};

/** The struct for SL in ByteStream. */
struct TGC_BYTESTREAM_SL
{
  unsigned roi:       8;
  unsigned overlap:   1;
  unsigned threshold: 3;
  unsigned fill1:     1;
  unsigned bcId:      3;
  unsigned sign:      1;
  unsigned cand:      1;
  unsigned sector:    3;
  unsigned fwd:       1;
  unsigned bcBitmap:  3;
  unsigned cand2plus: 1;
  unsigned veto:      1;
  unsigned fill2:     5;

  TGC_BYTESTREAM_SL() { memset(this, 0, sizeof(*this)); }
};

/**********************/
/** structs for NewSL */
/**********************/

struct TGC_BYTESTREAM_NSL_ROI
{
  unsigned roi:       8;
  unsigned pt:        4;
  unsigned charge:    1;
  unsigned coinflag:  3;
  unsigned fill1:     6;
  unsigned bcBitmap:  2;
  unsigned sector:    4;
  unsigned fwd:       1;
  unsigned type:      3;

  TGC_BYTESTREAM_NSL_ROI() { memset(this, 0, sizeof(*this)); }
};

struct TGC_BYTESTREAM_NSW_POS
{
  unsigned eta:       8;
  unsigned phi:       6;
  unsigned fill1:    10;
  unsigned sector:    4;
  unsigned fwd:       1;
  unsigned type:      3;

  TGC_BYTESTREAM_NSW_POS() { memset(this, 0, sizeof(*this)); }
};

struct TGC_BYTESTREAM_NSW_ANG
{
  unsigned angle:     5;
  unsigned phires:    1;
  unsigned lowres:    1;
  unsigned fill1:     2;
  unsigned nswid:     4;
  unsigned bcid:      4;
  unsigned input:     3;
  unsigned cand:      2;
  unsigned bcBitmap:  2;
  unsigned sector:    4;
  unsigned fwd:       1;
  unsigned type:      3;

  TGC_BYTESTREAM_NSW_ANG() { memset(this, 0, sizeof(*this)); }
};

struct TGC_BYTESTREAM_RPCBIS78_POS
{
  unsigned eta:       6;
  unsigned phi:       6;
  unsigned fill1:    12;
  unsigned sector:    4;
  unsigned fwd:       1;
  unsigned type:      3;

  TGC_BYTESTREAM_RPCBIS78_POS() { memset(this, 0, sizeof(*this)); }
};
struct TGC_BYTESTREAM_RPCBIS78_COIN
{
  unsigned fill1:     4;
  unsigned flag:      2;
  unsigned dphi:      3;
  unsigned deta:      3;
  unsigned fill2:     1;
  unsigned bcid:      4;
  unsigned fill3:     3;
  unsigned cand:      2;
  unsigned bcBitmap:  2;
  unsigned sector:    4;
  unsigned fwd:       1;
  unsigned type:      3;

  TGC_BYTESTREAM_RPCBIS78_COIN() { memset(this, 0, sizeof(*this)); }
};

struct TGC_BYTESTREAM_NSL_EIFI
{
  unsigned ei:        8;
  unsigned fi:        8;
  unsigned fill1:     6;
  unsigned bcBitmap:  2;
  unsigned sector:    4;
  unsigned fwd:       1;
  unsigned type:      3;

  TGC_BYTESTREAM_NSL_EIFI() { memset(this, 0, sizeof(*this)); }
};

struct TGC_BYTESTREAM_NSL_HIPT
{
  signed   delta:     5;
  unsigned sub:       1;
  unsigned hitId:     3;
  unsigned hipt:      1;
  unsigned cand:      1;
  unsigned chip:      2;
  unsigned strip:     1;
  unsigned fill1:     8;
  unsigned bcBitmap:  2;
  unsigned sector:    4;
  unsigned fwd:       1;
  unsigned type:      3;

  TGC_BYTESTREAM_NSL_HIPT() { memset(this, 0, sizeof(*this)); }
};

struct TGC_BYTESTREAM_NSL_TMDB
{
  unsigned module:   12;
  unsigned bcid:      4;
  unsigned fill1:     6;
  unsigned bcBitmap:  2;
  unsigned sector:    4;
  unsigned fwd:       1;
  unsigned type:      3;

  TGC_BYTESTREAM_NSL_TMDB() { memset(this, 0, sizeof(*this)); }
};

/*****************************/
/** end of structs for NewSL */
/*****************************/


template <typename DEST, typename SRC> DEST* my_pointer_cast(SRC* src) {
  void* ptr = src;  
  return reinterpret_cast<DEST*>(ptr); 
}
template <class T> inline uint32_t toBS32(T& data) { return *(my_pointer_cast<uint32_t>(&data)); }
template <class T> inline uint16_t toBS16(T& data) { return *(my_pointer_cast<uint16_t>(&data)); }
template <class T> inline void fromBS32(uint32_t data, T& s) { s = *(my_pointer_cast<T>(&data)); }
template <class T> inline void fromBS16(uint16_t data, T& s) { s = *(my_pointer_cast<T>(&data)); }
inline uint8_t  low8(uint32_t data)   { return (uint8_t)(data & 0x000000ff); }
inline uint16_t low16(uint32_t data)  { return (uint16_t)(data & 0x0000ffff); }
inline uint16_t high16(uint32_t data) { return (uint16_t)(data >> 16); }
inline uint16_t bcId(uint32_t data) { return data & 0xfff; }

inline int fragmentCount(uint32_t data, int id)
{
  TGC_BYTESTREAM_FRAGMENTCOUNT counter;
  fromBS32(data, counter);
  if(counter.id != (unsigned int)id)
    {
      IMessageSvc* msgSvc = 0;
      ISvcLocator* svcLocator = Gaudi::svcLocator();
      StatusCode sc = svcLocator->service("MessageSvc", msgSvc);
      if(!sc.isFailure()) {
	MsgStream log(msgSvc, "Muon TgcByteStreamData fragmentCount");
	log << MSG::ERROR << "Requested count of frament " << id << ", data is for fragment " << counter.id << endmsg;
      }
      return 0;
    }
  return counter.count;
}

inline uint32_t from16(uint16_t high16, uint16_t low16)
{
  return (uint32_t)((high16 << 16) | low16);
}

inline uint32_t from8(uint8_t high8, uint8_t highmid8, uint8_t lowmid8, uint8_t low8)
{
  return (uint32_t)((high8 << 24) | (highmid8 << 16) | (lowmid8 << 8) | low8);
}

inline unsigned bcBitmap(uint16_t bcTag)
{
  return (bcTag == 1 ? 4 :
	  ((bcTag == 0 || bcTag == 2) ? 2 :
	   (bcTag == 3 ? 1 :
	    0)));
}

inline unsigned bcTag(unsigned bcBitMap)
{
  return (bcBitMap == 4 ? 1 :
	  (bcBitMap == 2 ? 2 :
	   (bcBitMap == 1 ? 3 :
	    0)));
}

#endif // MUONTGC_CNVTOOLS_TGCBYTESTREAMDATA_H
