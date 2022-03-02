/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCHitPattern.h"
#include <iostream>
#include <cstdlib>
#include <cstring>

namespace LVL1TGCTrigger {

TGCHitPattern::TGCHitPattern() 
  :m_pattern(0),m_length(-1)
{
}

TGCHitPattern::~TGCHitPattern()
{
  if(m_pattern!=0)  delete [] m_pattern; 
  m_pattern  = 0;
  m_length   =-1;
}

TGCHitPattern::TGCHitPattern(int len)
  :m_pattern(0), m_length(len)
{
  if ( m_length > 0 ) {
    m_pattern = new bool [len];
    for(int i=0; i<m_length; i+=1) m_pattern[i] = false;    
  } else {
    m_length = -1;
  }
}

TGCHitPattern::TGCHitPattern(const TGCHitPattern& right)
  :m_pattern(0),m_length(-1)
{
  m_length = right.m_length;
  if ( m_length > 0 ) {
    m_pattern = new bool [m_length];
    for(int i=0; i<m_length; i+=1 ) {
      m_pattern[i] = right.m_pattern[i];
    }
  }
}

TGCHitPattern& TGCHitPattern::operator=(const TGCHitPattern& right)
{
  if (this != &right) {
    if ( m_pattern != 0 ) delete [] m_pattern;
    m_pattern = 0;

    m_length = right.m_length;
    if ( m_length > 0 ) {
      m_pattern = new bool [m_length];
      for(int i=0; i<m_length; i+=1 ) {
        m_pattern[i] = right.m_pattern[i];
      }
    }
  }
  return *this;
}

bool TGCHitPattern::getChannel(int iChannel) const
{
  if(iChannel < m_length){
    return m_pattern[iChannel];
  }else{
    std::cerr << "internal error TGCHitPattern::getChannel()" << std::endl;
    return 0;
  }
}

void TGCHitPattern::setLength(int lengthIn)
{
  m_length = lengthIn;

  if ( m_length > 0 ) {
    if ( m_pattern != 0) delete [] m_pattern;
    m_pattern = new bool [m_length];
  }
}


void TGCHitPattern::setChannel(int iChannel, bool hit)
{
  m_pattern[iChannel] = hit;
}

void TGCHitPattern::setChannel(char* pat){
  int size = strlen(pat);
  if(size == m_length){
    for(int i = 0; i < size; i++){
      if(pat[i]-'0'){
        setChannel(i, true);
      }
    }
  } else {
#ifdef TGCCOUT
    std::cout << "TGCHitPattern::setChannel() error : size is different" << std::endl;
#endif
  }
}    

void TGCHitPattern::clear()
{
  int i;
  for( i=0; i<m_length; i+=1) m_pattern[i] = false;    
}

void TGCHitPattern::print(int  unit) const 
{
  int i;
  for( i=0; i<m_length; i+=1){
    if(i%unit==0) std::cout<<"\n_";
    if(m_pattern[i])
      std::cout << 1;
    else
      std::cout << 0;
  }
  std::cout << std::endl;
}

void TGCHitPattern::print() const 
{

#ifdef BITPATTERN
  std::cout << " [" << m_length << "bit] ";
#endif
#ifdef BITPATTERN
  for( int i=0; i<m_length; i+=1)
    std::cout << m_pattern[i] <<std::endl;
#endif
#ifdef TGCCOUT
  for( int i=0; i<m_length; i+=1)
    if(m_pattern[i]) std::cout<<' '<<i;
  std::cout << std::endl;
#endif
}

void TGCHitPattern::printb() const
{
#ifdef TGCCOUT
  for( int i=0; i<m_length; i+=1){
    std::cout << m_pattern[i];
  }
#endif
}

void TGCHitPattern::printb(std::ofstream* ofs) const
{
  int i;
  for( i=0; i<m_length; i+=1){
    *ofs << m_pattern[i];
  }
}

void TGCHitPattern::push_back(TGCHitPattern* hp)
{
    bool* ptmp;
    ptmp = 0;
    ptmp = new bool [hp->getLength() + m_length];
    int i;
    for(i=0; i<m_length ; i++){
        ptmp[i] = m_pattern[i];
    }
    for(i=m_length; i<(hp->getLength() + m_length); i++){
        ptmp[i] = hp->getChannel(i-m_length);
    }
    if(m_pattern)delete [] m_pattern;
    m_pattern = ptmp;
    m_length = hp->getLength() + m_length;
}

void TGCHitPattern::dec2bin(int dec)
{
    for(int i= m_length -1; i >= 0; i--){ 
        if( (dec >> i) & 1 )
             m_pattern[m_length-1-i] = 1;
        else 
             m_pattern[m_length-1-i] = 0;
    } 
} 


// new method for hit visualization (KH 19/01/01) 
#ifdef TGCCOUT
void TGCHitPattern::visual(int  Cycle, int  Width , int Tag ) const
{
  int i,j;
  if (Tag == 1) {
    std::cout << "| ";
    for(i=0; i<Cycle; i++) {
      for (j=0; j<Width; j++) std::cout << "=";
    }
    std::cout << " |\n| ";
    for(i=0; i<Cycle; i++) {
      for (j=0; j<Width; j++) {
      if ((j == 0) && (i%10 == 0)) std::cout << i/10;
      else                         std::cout << " "; 
      }
    }

    std::cout << " |\n| ";
    for(i=0; i<Cycle; i++) {
      for (j=0; j<Width; j++) {
      if (j == 0) std::cout << i%10;
      else        std::cout << " "; 
      }
    }

    std::cout << " |\n| ";
    for(i=0; i<Cycle; i++) {
      for (j=0; j<Width; j++) std::cout << "-";
    }

    std::cout << " |\n";
  }
  else {
    std::cout << "| ";
    for(i=0; i<m_length; i++) {
      if ((i>0) && (i%Cycle==0)) std::cout<<" |\n| ";
      for (j=0; j<Width; j++) {
        if(m_pattern[i]) std::cout << "*";
        else           std::cout << ".";
      }
    }
    std::cout << " |\n";
  }
}
#else
void TGCHitPattern::visual(int  , int   , int ) const
{
  // do nothing
}
#endif

// new method to set hit patterns (KH 08/05/01) 
void TGCHitPattern::setPattern(bool* newpattern)
{
  if(m_pattern)delete [] m_pattern;
  m_pattern = newpattern;
}



} //end of namespace bracket
