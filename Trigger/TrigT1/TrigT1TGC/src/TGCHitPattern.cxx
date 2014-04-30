/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCHitPattern.hh"
#include <iostream>
#include <cstdlib>
#include <cstring>

namespace LVL1TGCTrigger {

TGCHitPattern::TGCHitPattern() 
  :pattern(0),length(-1),cPattern(0)
{
}

TGCHitPattern::~TGCHitPattern()
{
  if(pattern!=0)  delete [] pattern; 
  if(cPattern!=0) delete [] cPattern;
  pattern  = 0;
  cPattern = 0;
  length   =-1;
}

TGCHitPattern::TGCHitPattern(int len)
  :pattern(0), length(len), cPattern(0)
{
  if ( length > 0 ) {
    pattern = new bool [len];
    for(int i=0; i<length; i+=1) pattern[i] = false;    
  } else {
    length = -1;
  }
}

TGCHitPattern::TGCHitPattern(const TGCHitPattern& right)
  :pattern(0),length(-1),cPattern(0)
{
  length = right.length;
  if ( length > 0 ) {
    pattern = new bool [length];
    for(int i=0; i<length; i+=1 ) {
      pattern[i] = right.pattern[i];
    }
  }
}

TGCHitPattern& TGCHitPattern::operator=(const TGCHitPattern& right)
{
  if (this != &right) {
    if (cPattern !=0) delete [] cPattern;
    cPattern = 0;
    if ( pattern != 0 ) delete [] pattern;
    pattern = 0;

    length = right.length;
    if ( length > 0 ) {
      pattern = new bool [length];
      for(int i=0; i<length; i+=1 ) {
        pattern[i] = right.pattern[i];
      }
    }
  }
  return *this;
}

bool TGCHitPattern::getChannel(int iChannel) const
{
  if(iChannel < length){
    return pattern[iChannel];
  }else{
    std::cerr << "internal error TGCHitPattern::getChannel()" << std::endl;
    return 0;
  }
}

void TGCHitPattern::setLength(int lengthIn)
{
  length = lengthIn;

  if ( length > 0 ) {
    if ( pattern != 0) delete [] pattern;
    if ( cPattern !=0) delete [] cPattern;
    cPattern = 0;
    pattern = new bool [length];
    if ( !pattern ) {
      std::cerr << "TGCHitPattern::setLength: Memory allocation failure." << std::endl;
      exit(1);
    }
  }
}


void TGCHitPattern::setChannel(int iChannel, bool hit)
{
  pattern[iChannel] = hit;
  if(cPattern){delete [] cPattern;}
  cPattern = 0;
}

void TGCHitPattern::setChannel(char* pat){
  int size = strlen(pat);
  if(size == length){
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
  if(cPattern){delete [] cPattern;}
  cPattern = 0;
}    

void TGCHitPattern::clear()
{
  int i;
  for( i=0; i<length; i+=1) pattern[i] = false;    
  if(cPattern){delete [] cPattern;}
  cPattern = 0;
}

bool TGCHitPattern::isEmpty() const
{
  int i;
  for( i=0; i<length; i+=1)
    if(pattern[i]) return false;
  return true;
}

void TGCHitPattern::print(int  unit) const 
{
  int i;
  for( i=0; i<length; i+=1){
    if(i%unit==0) std::cout<<"\n_";
    if(pattern[i])
      std::cout << 1;
    else
      std::cout << 0;
  }
  std::cout << std::endl;
}

void TGCHitPattern::print() const 
{

#ifdef BITPATTERN
  std::cout << " [" << length << "bit] ";
#endif
#ifdef BITPATTERN
  for( int i=0; i<length; i+=1)
    std::cout << pattern[i] <<std::endl;
#endif
#ifdef TGCCOUT
  for( int i=0; i<length; i+=1)
    if(pattern[i]) std::cout<<' '<<i;
  std::cout << std::endl;
#endif
}

void TGCHitPattern::printb() const
{
#ifdef TGCCOUT
  for( int i=0; i<length; i+=1){
    std::cout << pattern[i];
  }
#endif
}

void TGCHitPattern::printb(std::ofstream* ofs) const
{
  int i;
  for( i=0; i<length; i+=1){
    *ofs << pattern[i];
  }
}

void TGCHitPattern::printb(std::ofstream* ofs, int lengthIn) const
{
  int i;
  for( i=length-lengthIn; i<length; i+=1){
    *ofs << pattern[i];
  }
}

void TGCHitPattern::printb3(std::ofstream* ofs) const 
{
  *ofs << pattern[3];
}

void TGCHitPattern::printb123(std::ofstream* ofs) const
{
  int i;
  for( i=1; i<4; i+=1){
    *ofs << pattern[i];
  }
}

void TGCHitPattern::insert(int pos, bool v)
{
    //insert pos-1,pos
    if(0<=pos && pos<=length){
        bool* ptmp;
        ptmp = new bool [length+1];
	int i;
        for(i=0; i<pos; i++){
            ptmp[i] = pattern[i];
        }
        ptmp[pos] = v;
        for(i=(pos+1); i<(length+1); i++){
            ptmp[i] = pattern[i-1];
        }
        delete [] pattern;
        pattern = ptmp;
        length++;
    }
    if (cPattern !=0) delete [] cPattern;
    cPattern =0;
}

void TGCHitPattern::replace(int pos, TGCHitPattern* hp)
{
    if(pos + hp->getLength() < length){
        for(int i=pos; i<(pos + hp->getLength()); i++){
            pattern[i] = hp->getChannel(i-pos);
        }
    }
    if(cPattern){delete [] cPattern;}
    cPattern = 0;
}

void TGCHitPattern::push_back(TGCHitPattern* hp)
{
    bool* ptmp;
    ptmp = 0;
    ptmp = new bool [hp->getLength() + length];
    if(!ptmp){
        std::cerr << "TGCHitPattern::push_back: Memory allocation failure." << std::endl;
        exit(1);
    }
    int i;
    for(i=0; i<length ; i++){
        ptmp[i] = pattern[i];
    }
    for(i=length; i<(hp->getLength() + length); i++){
        ptmp[i] = hp->getChannel(i-length);
    }
    if(pattern)delete [] pattern;
    pattern = ptmp;
    length = hp->getLength() + length;
    if (cPattern !=0) delete [] cPattern;
    cPattern =0;
}

void TGCHitPattern::resize(int size)
{
  if ( length > 0 ) {
    if(pattern)delete [] pattern;
    pattern = new bool [size];
    length = size;
    if ( !pattern ) {
      std::cerr << "TGCHitPattern::resize: Memory allocation failure." << std::endl;
      exit(1);
    }
  } else {
    length = -1;
  }
  if (cPattern !=0) delete [] cPattern;
  cPattern =0;

  clear();
}

void TGCHitPattern::del(int pos)
{
    bool* ptmp;
    ptmp = 0;
    ptmp = new bool [length-1];
    if(!ptmp){
        std::cerr << "TGCHitPattern::push_back: Memory allocation failure." << std::endl;
        exit(1);
    }
    int i;
    for(i=0; i<pos ; i++){
        ptmp[i] = pattern[i];
    }
    for(i=pos; i<length-1; i++){
        ptmp[i] = pattern[i+1];
    }
    if(pattern)delete [] pattern;
    pattern = ptmp;
    length--;
    if (cPattern !=0) delete [] cPattern;
    cPattern =0;
}

void TGCHitPattern::dec2bin(int dec)
{
    for(int i= length -1; i >= 0; i--){ 
        if( (dec >> i) & 1 )
             pattern[length-1-i] = 1;
        else 
             pattern[length-1-i] = 0;
    } 
    if(cPattern){delete [] cPattern;}
    cPattern = 0;
} 

void TGCHitPattern::dec2binInv(int dec)
{
  int dec8 = dec - 8;

  if(dec>=8){
    if(dec8>=8){
      pattern[3]=0;
      pattern[2]=0;
      pattern[1]=0;
      pattern[1]=0;
    }else{
      pattern[0]=1;
      if(dec8==7){
	pattern[3]=1;
	pattern[2]=1;
	pattern[1]=1;
      }
      if(dec8==6){
	pattern[3]=0;
	pattern[2]=1;
	pattern[1]=1;
      }
      if(dec8==5){
	pattern[3]=1;
	pattern[2]=0;
	pattern[1]=1;
      }
      if(dec8==4){
	pattern[3]=0;
	pattern[2]=0;
	pattern[1]=1;
      }
      if(dec8==3){
	pattern[3]=1;
	pattern[2]=1;
	pattern[1]=0;
      }
      if(dec8==2){
	pattern[3]=0;
	pattern[2]=1;
	pattern[1]=0;
      }
      if(dec8==1){
	pattern[3]=1;
	pattern[2]=0;
	pattern[1]=0;
      }
      if(dec8==0){
	pattern[3]=0;
	pattern[2]=0;
	pattern[1]=0;
      }
    }
  }

  if(dec<8){
    pattern[0]=0;
    if(dec==7){
      pattern[3]=1;
      pattern[2]=1;
      pattern[1]=1;
    }
    if(dec==6){
      pattern[3]=0;
      pattern[2]=1;
      pattern[1]=1;
    }
    if(dec==5){
      pattern[3]=1;
      pattern[2]=0;
      pattern[1]=1;
    }
    if(dec==4){
      pattern[3]=0;
      pattern[2]=0;
      pattern[1]=1;
    }
    if(dec==3){
      pattern[3]=1;
      pattern[2]=1;
      pattern[1]=0;
    }
    if(dec==2){
      pattern[3]=0;
      pattern[2]=1;
      pattern[1]=0;
    }
    if(dec==1){
      pattern[3]=1;
      pattern[2]=0;
      pattern[1]=0;
    }
    if(dec==0){
      pattern[3]=0;
      pattern[2]=0;
      pattern[1]=0;
    }
  }
  if(cPattern){delete [] cPattern;}
  cPattern = 0;
} 

void TGCHitPattern::write(char* buf) const
{
    for(int i= 0; i<length; i++){ 
        buf[i] = pattern[i];
    } 
} 

bool* TGCHitPattern::getPatad(void) const
{
    return pattern; 
} 

void TGCHitPattern::reverse(int pos)
{
    if(pattern[pos]){
        pattern[pos] = 0;
    }else{
        pattern[pos] = 1;
    } 
    if(cPattern){delete [] cPattern;}
    cPattern = 0;
} 

const char* TGCHitPattern::bool2char(void)
{
    if(cPattern) return cPattern;
    if(0<length){
        if(cPattern){delete [] cPattern;}
        cPattern = new char [length+1];
        if(cPattern){
            for(int i = 0; i < length; i++){
              cPattern[i] = '0';
              if(pattern[i])cPattern[i] = '1';
            }
            cPattern[length] = '\0';
            return cPattern;
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
    for(i=0; i<length; i++) {
      if ((i>0) && (i%Cycle==0)) std::cout<<" |\n| ";
      for (j=0; j<Width; j++) {
        if(pattern[i]) std::cout << "*";
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
  if (cPattern !=0) delete [] cPattern;
  cPattern =0;
  if(pattern)delete [] pattern;
  pattern = newpattern;
}



} //end of namespace bracket
