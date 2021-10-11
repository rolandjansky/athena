/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "ParticleEvent/SelectedParticles.h"

#include <sstream>

using namespace std;

SelectedParticles::SelectedParticles(const vector<unsigned>& indices):m_bits(){
  int nind=indices.size();
  this->SetMaxBits(nind);
  for(int i=0;i<nind;++i){
    this->SetBit(indices[i],true);
  }
}

void SelectedParticles::SetBit(unsigned i, bool good){
  unsigned nwords=m_bits.size();
  unsigned iword=i/32;

  if(iword+1>nwords){
    for(unsigned iw=nwords; iw<iword+1; ++iw){
      unsigned temp=0;
      m_bits.push_back(temp);
    }
  }

  if(good==false) return;

  unsigned bitnum = i % 32 ;
  unsigned temp = 1 << bitnum;
  m_bits[iword] |= temp;

  return;
}

void SelectedParticles::SetMaxBits(unsigned maxbits)
{
  unsigned nwords=maxbits/32;
  for(unsigned iw=0; iw<nwords+1; ++iw){
    unsigned temp=0;
    m_bits.push_back(temp);
  }
}


string SelectedParticles::displayBits() const
{
  stringstream os;
  unsigned c;
  // Invariant: displayed j 32-bit ints so far
  unsigned nwords=m_bits.size();
  vector<unsigned>::const_iterator itb=m_bits.begin();
  int j=0;
  for (; itb != m_bits.end(); ++itb){
    // display mask will be a single 1 at the most significant bit
    unsigned displayMask = 1u << 31;
    // need to display the words in reverse order
    int temp = nwords - j - 1;
    ++j;
    // Invariant: displayed c bits in the unsigned int
    // displayMask has shifted its bits c places
    for (c = 0; c != 32; c++ ) {
      //if the current most significant bit in value temp is a 1
      // display it if not display zero
      os << (m_bits[temp] & displayMask ? '1' : '0');
      // shift the displayMask one bit, so there is a new
      // bit to check with value
      displayMask >>= 1;
      
      // group display by bytes
      if (c % 8 == 7 )
	os << ' ';
    }
    // a new line for each 32-bit line
    os << std::endl;
  }
  os << std::endl;
  return os.str();
}

bool SelectedParticles::isGood(unsigned ipart) const
{
  
  unsigned c;
  unsigned nwords=m_bits.size();
  vector<unsigned>::const_iterator itb=m_bits.begin();
  unsigned j=ipart/32;
  if(j>nwords-1) return false;  // this should probably be an abort
  for (; itb != m_bits.end(); ++itb){
    // display mask will be a single 1 at the most significant bit
    unsigned displayMask = 1;
    
    // Invariant: displayed c bits in the unsigned int
    // displayMask has shifted its bits c places
    for (c = 0; c != 32; c++ ) {
      //if the current most significant bit in value temp is a 1
      // display it if not display zero
      if (m_bits[j] & displayMask) {
	if(c==ipart) return true;
      }
      // shift the displayMask one bit, so there is a new
      // bit to check with value
      displayMask <<= 1;
    }
  }
  return false;
}

unsigned SelectedParticles::numGood() const
{
  unsigned nwords=m_bits.size();
  if(nwords==0) return 0;
  vector<unsigned> goodparts;
  goodParticles(goodparts);
  return goodparts.size();
}

void SelectedParticles::goodParticles(std::vector<unsigned>& goodparts ) const
{
  unsigned nbits=m_bits.size()*32;
  for(unsigned ibit=0; ibit<nbits; ++ibit) {
    if(isGood(ibit)) goodparts.push_back(ibit);
  }
  return;
}

