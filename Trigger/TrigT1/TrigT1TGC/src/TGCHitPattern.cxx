/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCHitPattern.hh"
#include <iostream>
#include <cstdlib>
#include <cstring>

namespace LVL1TGCTrigger {

TGCHitPattern::TGCHitPattern() 
  :m_pattern(0),m_length(-1),m_cPattern(0)
{
}

TGCHitPattern::~TGCHitPattern()
{
  if(m_pattern!=0)  delete [] m_pattern; 
  if(m_cPattern!=0) delete [] m_cPattern;
  m_pattern  = 0;
  m_cPattern = 0;
  m_length   =-1;
}

TGCHitPattern::TGCHitPattern(int len)
  :m_pattern(0), m_length(len), m_cPattern(0)
{
  if ( m_length > 0 ) {
    m_pattern = new bool [len];
    for(int i=0; i<m_length; i+=1) m_pattern[i] = false;    
  } else {
    m_length = -1;
  }
}

TGCHitPattern::TGCHitPattern(const TGCHitPattern& right)
  :m_pattern(0),m_length(-1),m_cPattern(0)
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
    if (m_cPattern !=0) delete [] m_cPattern;
    m_cPattern = 0;
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
    if ( m_cPattern !=0) delete [] m_cPattern;
    m_cPattern = 0;
    m_pattern = new bool [m_length];
    if ( !m_pattern ) {
      std::cerr << "TGCHitPattern::setLength: Memory allocation failure." << std::endl;
      exit(1);
    }
  }
}


void TGCHitPattern::setChannel(int iChannel, bool hit)
{
  m_pattern[iChannel] = hit;
  if(m_cPattern){delete [] m_cPattern;}
  m_cPattern = 0;
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
  if(m_cPattern){delete [] m_cPattern;}
  m_cPattern = 0;
}    

void TGCHitPattern::clear()
{
  int i;
  for( i=0; i<m_length; i+=1) m_pattern[i] = false;    
  if(m_cPattern){delete [] m_cPattern;}
  m_cPattern = 0;
}

bool TGCHitPattern::isEmpty() const
{
  int i;
  for( i=0; i<m_length; i+=1)
    if(m_pattern[i]) return false;
  return true;
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

void TGCHitPattern::printb(std::ofstream* ofs, int lengthIn) const
{
  int i;
  for( i=m_length-lengthIn; i<m_length; i+=1){
    *ofs << m_pattern[i];
  }
}

void TGCHitPattern::printb3(std::ofstream* ofs) const 
{
  *ofs << m_pattern[3];
}

void TGCHitPattern::printb123(std::ofstream* ofs) const
{
  int i;
  for( i=1; i<4; i+=1){
    *ofs << m_pattern[i];
  }
}

void TGCHitPattern::insert(int pos, bool v)
{
    //insert pos-1,pos
    if(0<=pos && pos<=m_length){
        bool* ptmp;
        ptmp = new bool [m_length+1];
	int i;
        for(i=0; i<pos; i++){
            ptmp[i] = m_pattern[i];
        }
        ptmp[pos] = v;
        for(i=(pos+1); i<(m_length+1); i++){
            ptmp[i] = m_pattern[i-1];
        }
        delete [] m_pattern;
        m_pattern = ptmp;
        m_length++;
    }
    if (m_cPattern !=0) delete [] m_cPattern;
    m_cPattern =0;
}

void TGCHitPattern::replace(int pos, TGCHitPattern* hp)
{
    if(pos + hp->getLength() < m_length){
        for(int i=pos; i<(pos + hp->getLength()); i++){
            m_pattern[i] = hp->getChannel(i-pos);
        }
    }
    if(m_cPattern){delete [] m_cPattern;}
    m_cPattern = 0;
}

void TGCHitPattern::push_back(TGCHitPattern* hp)
{
    bool* ptmp;
    ptmp = 0;
    ptmp = new bool [hp->getLength() + m_length];
    if(!ptmp){
        std::cerr << "TGCHitPattern::push_back: Memory allocation failure." << std::endl;
        exit(1);
    }
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
    if (m_cPattern !=0) delete [] m_cPattern;
    m_cPattern =0;
}

void TGCHitPattern::resize(int size)
{
  if ( m_length > 0 ) {
    if(m_pattern)delete [] m_pattern;
    m_pattern = new bool [size];
    m_length = size;
    if ( !m_pattern ) {
      std::cerr << "TGCHitPattern::resize: Memory allocation failure." << std::endl;
      exit(1);
    }
  } else {
    m_length = -1;
  }
  if (m_cPattern !=0) delete [] m_cPattern;
  m_cPattern =0;

  clear();
}

void TGCHitPattern::del(int pos)
{
    bool* ptmp;
    ptmp = 0;
    ptmp = new bool [m_length-1];
    if(!ptmp){
        std::cerr << "TGCHitPattern::push_back: Memory allocation failure." << std::endl;
        exit(1);
    }
    int i;
    for(i=0; i<pos ; i++){
        ptmp[i] = m_pattern[i];
    }
    for(i=pos; i<m_length-1; i++){
        ptmp[i] = m_pattern[i+1];
    }
    if(m_pattern)delete [] m_pattern;
    m_pattern = ptmp;
    m_length--;
    if (m_cPattern !=0) delete [] m_cPattern;
    m_cPattern =0;
}

void TGCHitPattern::dec2bin(int dec)
{
    for(int i= m_length -1; i >= 0; i--){ 
        if( (dec >> i) & 1 )
             m_pattern[m_length-1-i] = 1;
        else 
             m_pattern[m_length-1-i] = 0;
    } 
    if(m_cPattern){delete [] m_cPattern;}
    m_cPattern = 0;
} 

void TGCHitPattern::dec2binInv(int dec)
{
  int dec8 = dec - 8;

  if(dec>=8){
    if(dec8>=8){
      m_pattern[3]=0;
      m_pattern[2]=0;
      m_pattern[1]=0;
      m_pattern[1]=0;
    }else{
      m_pattern[0]=1;
      if(dec8==7){
	m_pattern[3]=1;
	m_pattern[2]=1;
	m_pattern[1]=1;
      }
      if(dec8==6){
	m_pattern[3]=0;
	m_pattern[2]=1;
	m_pattern[1]=1;
      }
      if(dec8==5){
	m_pattern[3]=1;
	m_pattern[2]=0;
	m_pattern[1]=1;
      }
      if(dec8==4){
	m_pattern[3]=0;
	m_pattern[2]=0;
	m_pattern[1]=1;
      }
      if(dec8==3){
	m_pattern[3]=1;
	m_pattern[2]=1;
	m_pattern[1]=0;
      }
      if(dec8==2){
	m_pattern[3]=0;
	m_pattern[2]=1;
	m_pattern[1]=0;
      }
      if(dec8==1){
	m_pattern[3]=1;
	m_pattern[2]=0;
	m_pattern[1]=0;
      }
      if(dec8==0){
	m_pattern[3]=0;
	m_pattern[2]=0;
	m_pattern[1]=0;
      }
    }
  }

  if(dec<8){
    m_pattern[0]=0;
    if(dec==7){
      m_pattern[3]=1;
      m_pattern[2]=1;
      m_pattern[1]=1;
    }
    if(dec==6){
      m_pattern[3]=0;
      m_pattern[2]=1;
      m_pattern[1]=1;
    }
    if(dec==5){
      m_pattern[3]=1;
      m_pattern[2]=0;
      m_pattern[1]=1;
    }
    if(dec==4){
      m_pattern[3]=0;
      m_pattern[2]=0;
      m_pattern[1]=1;
    }
    if(dec==3){
      m_pattern[3]=1;
      m_pattern[2]=1;
      m_pattern[1]=0;
    }
    if(dec==2){
      m_pattern[3]=0;
      m_pattern[2]=1;
      m_pattern[1]=0;
    }
    if(dec==1){
      m_pattern[3]=1;
      m_pattern[2]=0;
      m_pattern[1]=0;
    }
    if(dec==0){
      m_pattern[3]=0;
      m_pattern[2]=0;
      m_pattern[1]=0;
    }
  }
  if(m_cPattern){delete [] m_cPattern;}
  m_cPattern = 0;
} 

void TGCHitPattern::write(char* buf) const
{
    for(int i= 0; i<m_length; i++){ 
        buf[i] = m_pattern[i];
    } 
} 

bool* TGCHitPattern::getPatad(void) const
{
    return m_pattern; 
} 

void TGCHitPattern::reverse(int pos)
{
    if(m_pattern[pos]){
        m_pattern[pos] = 0;
    }else{
        m_pattern[pos] = 1;
    } 
    if(m_cPattern){delete [] m_cPattern;}
    m_cPattern = 0;
} 

const char* TGCHitPattern::bool2char(void)
{
    if(m_cPattern) return m_cPattern;
    if(0<m_length){
        if(m_cPattern){delete [] m_cPattern;}
        m_cPattern = new char [m_length+1];
        if(m_cPattern){
            for(int i = 0; i < m_length; i++){
              m_cPattern[i] = '0';
              if(m_pattern[i])m_cPattern[i] = '1';
            }
            m_cPattern[m_length] = '\0';
            return m_cPattern;
        }
    }
    return 0;
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
  if (m_cPattern !=0) delete [] m_cPattern;
  m_cPattern =0;
  if(m_pattern)delete [] m_pattern;
  m_pattern = newpattern;
}



} //end of namespace bracket
