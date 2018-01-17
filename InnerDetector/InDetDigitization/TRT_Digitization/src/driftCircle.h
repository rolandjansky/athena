/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_DRIFTCIRCLE_H
#define TRT_DRIFTCIRCLE_H

// driftCircle.h
// This local copy is only for development/debugging purposes
// Adapted from TRT_DriftCircle.h
// For basic tuning and development tests.
// 13/10/2012 Andrew.Beddall@cern.ch

void bits32(unsigned int n) {
  for (unsigned int k=0; k<32; k++) {
    unsigned int i = n & 0x80000000u;
    bool b = (i==0x80000000u);
    std::cout << b;
    n=n<<1;
  }
  std::cout << std::endl;
}

void bits27(unsigned int n) {
  for (unsigned int k=0; k<32; k++) {
    if (k>4) {
      unsigned int i = n & 0x80000000u;
      bool b = (i==0x80000000u);
      if (!((k-5)%9)) std::cout << " ";
      std::cout << b;
      if (!((k-5)%9)) std::cout << " ";
    }
    n=n<<1;
  }
  std::cout << std::endl;
}

void bits24(unsigned int n) {
  for (unsigned int k=0; k<32; k++) {
    if (k>4) {
      unsigned int i = n & 0x80000000u;
      bool b = (i==0x80000000u);
      if (((k-5)%9)) std::cout << b;
    }
    n=n<<1;
  }
  std::cout << std::endl;
}

int driftTimeBin(unsigned int m_word) {
  unsigned mask = 0x02000000;
  bool SawZero = false;
  int i;
  for(i=0;i<24;++i)
    { if ( (m_word & mask) && SawZero) break;
      else if ( !(m_word & mask) ) SawZero = true;
      mask>>=1;
      if(i==7 || i==15) mask>>=1;
    }
  if(i==24) i=0;
  return i;
}

int trailingEdge(unsigned int m_word) {
  unsigned mask = 0x00000001;
  bool SawZero=false;
  int i;
  for (i = 0; i < 24; ++i)
  {
    if ( (m_word & mask) && SawZero )
      break;
    else if ( !(m_word & mask) )
      SawZero = true;
    mask <<= 1;
    if (i == 7 || i == 15)
      mask <<= 1;
  }

  if ( 24 == i )
    return i;
  return (23 - i);
}

bool lowLevelMiddle(unsigned int m_word) {
  return (m_word & 0x0001FE00);
}

bool highLevel(unsigned int m_word) {
  return (m_word & 0x04020100);
}

bool highLevel1(unsigned int m_word) {
  return (m_word & 0x04000000);
}

bool highLevel2(unsigned int m_word) {
  return (m_word & 0x00020000);
}

bool highLevel3(unsigned int m_word) {
  return (m_word & 0x00000100);
}

bool firstBinHigh(unsigned int m_word) {
  return (m_word & 0x02000000);
}

bool lastBinHigh(unsigned int m_word) {
  return (m_word & 0x1);
}

double rawDriftTime(unsigned int m_word) {
  return (driftTimeBin(m_word)+0.5)*3.125;
}

double timeOverThreshold(unsigned int m_word) {
  double binWidth = 3.125;
  int LE = driftTimeBin(m_word);
  int TE = trailingEdge(m_word);
  if ( (24 == LE) || (24 == TE) || (0 == TE) || (23 == LE) ) return 0.0;
  return (double) (TE - LE + 1) * binWidth;
}

int bitTimeOverThreshold(unsigned int m_word) {
  return floor(timeOverThreshold(m_word)/3.125+0.5);
}

int nLTbits(unsigned int m_word) {
  unsigned mask = 0x02000000;
  int m=0;
  for (int i=0;i<24;++i) {
    if ( m_word & mask ) m++;
    mask>>=1;
    if (i==7 || i==15) mask>>=1;
  }
  return m;
}

void print_mword_properties(unsigned int mword) {
  int LE  = driftTimeBin(mword);
  int TE  = trailingEdge(mword);
  int ToT = bitTimeOverThreshold(mword);
  int nLT = nLTbits(mword);
  std::cout << "AJB " << LE << " " << TE << " " << ToT << " " << nLT << std::endl;
}

/*
bool driftTimeValid(unsigned int m_word) {
  return m_word & 0x08000000;
}

void setDriftTimeValid(bool valid) {
  unsigned maskfalse = 0xF7FFFFFF;
  unsigned masktrue = 0x08000000;
  if( valid ) {
    m_word |= masktrue;
  } else {
    m_word &= maskfalse; }
}

bool isNoise(unsigned int m_word) {
  if( timeOverThreshold(unsigned int m_word) < 5. || timeOverThreshold(unsigned int m_word) > 60. ) return true;
  return false;
}
*/

#endif
