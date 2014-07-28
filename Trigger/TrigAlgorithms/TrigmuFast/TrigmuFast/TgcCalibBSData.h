/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGC_CALIB_BS_DATA
#define TGC_CALIB_BS_DATA

#include <cstring>


namespace LVL2CALIBRATION {


struct TGC_BYTESTREAM_FRAGMENTCOUNT
{
    unsigned count:24;
    unsigned id:8;
};

struct TGC_BYTESTREAM_READOUTHIT
{
unsigned channel: 8;
unsigned sbId: 5;
unsigned sbType: 3;
unsigned adj: 1;
unsigned ldbId: 4;
unsigned bcBitmap: 3;
unsigned tracklet: 6;
unsigned fill1: 1;
unsigned ok: 1;

    TGC_BYTESTREAM_READOUTHIT()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct TGC_BYTESTREAM_READOUTTRACKLET
{
unsigned rphi: 5;
unsigned subm: 2;
unsigned seg: 1;
signed   delta: 4;
unsigned sbId: 5;
unsigned ldbId: 4;
unsigned bcBitmap: 3;
unsigned trkType: 3;
unsigned fill1: 1;
unsigned slbType: 3;
unsigned ok: 1;

    TGC_BYTESTREAM_READOUTTRACKLET()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct TGC_BYTESTREAM_READOUTTRIPLETSTRIP
{
unsigned phi: 4;
unsigned subc: 2;
unsigned fill1: 1;
unsigned seg: 1;
unsigned fill2: 4;
unsigned sbId: 5;
unsigned ldbId: 4;
unsigned bcBitmap: 3;
unsigned trkType: 3;
unsigned fill3: 1;
unsigned slbType: 3;
unsigned ok: 1;

    TGC_BYTESTREAM_READOUTTRIPLETSTRIP()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct TGC_BYTESTREAM_HIPT
{
signed   delta: 5;
unsigned sub: 1;
unsigned hitId: 3;
unsigned hipt: 1;
unsigned cand: 1;
unsigned chip: 2;
unsigned sector: 3;
unsigned fwd: 1;
unsigned strip: 1;
unsigned bcBitmap: 3;
unsigned fill1: 11;

    TGC_BYTESTREAM_HIPT()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct TGC_BYTESTREAM_SL
{
unsigned roi: 8;
unsigned overlap: 1;
unsigned threshold: 3;
unsigned fill1: 1;
unsigned bcId: 3;
unsigned sign: 1;
unsigned cand: 1;
unsigned sector: 3;
unsigned fwd: 1;
unsigned bcBitmap: 3;
unsigned cand2plus: 1;
unsigned fill2: 6;

    TGC_BYTESTREAM_SL()
    {
        memset(this, 0, sizeof(*this));
    }
};

inline static unsigned bcBitmap(uint16_t bcTag)
{
    return (bcTag == 1 ? 4 :
            ((bcTag == 0 || bcTag == 2) ? 2 :
             (bcTag == 3 ? 1 :
              0)));
}

template <class T>
inline uint32_t toBS32(T& data)
{
    return *(uint32_t*)(&data);
}

template <class T>
inline void fromBS32(uint32_t data, T& s)
{
    union {uint32_t* d; T* v;} t;
    t.d = &data;
    s = *t.v;
    //s = *(T*)(&data);
}


} // end of namespeace LVL2CALIBRATION

#endif
