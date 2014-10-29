/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 *  ErrorMatrixCompressor.cxx
 *
 *  Created by Dmitry Emeliyanov on 12/11/08.
 *  <Dmitry.Emeliyanov@cern.ch>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "EventCommonTPCnv/ErrorMatrixCompressor.h"

#include <fstream>
#include <cstring>

using std::memset;

void DecoderFloat_IEEE754::print()
{
  //  printf("0x%X\n",m_data.l);
  unsigned int mask = 0x00000001;
  for(int i=31;i>=0;i--)
    {
      unsigned int m = mask << i;
      //printf("0x%X\n",m);
      if(m_data.l & m) std::cout<<"1";
      else std::cout<<"0";
    }
  std::cout<<std::endl;
  /*
    mask = 0x80000000;
    int sign = (m_data.l & mask) >> 31;
    mask = 0x7f800000;
    short int exponent = (m_data.l & mask) >> 23;
    mask = 0x007fffff;
    unsigned int mant = (m_data.l & mask);
    printf("Sign=0x%X Exponent = 0x%X Mantissa = 0x%X\n",sign,exponent,mant); 
    printf("Sign=%d Exponent = %d Mantissa = %ld\n",sign,exponent,mant);  
  */
}

short int DecoderFloat_IEEE754::getExponent()
{
  unsigned int mask = 0x7f800000;
  return ((m_data.l & mask) >> 23);
}

unsigned int DecoderFloat_IEEE754::getMantissa()
{
  unsigned int mask = 0x007fffff;
  return (m_data.l & mask);
}


void DecoderFloat_IEEE754::setExponent(short int ex)
{
  unsigned int buf=ex;
  buf=buf<<23;
  unsigned int mask=0x7F800000;
  m_data.l=m_data.l | (buf & mask);
}

void DecoderFloat_IEEE754::setSign(int s)
{
  if(s>0)
    {
      m_data.l=m_data.l & 0x7FFFFFFF;
    }
  else
    {
      m_data.l=m_data.l | 0x80000000;
    }
}


ErrorMatrixCompressor::ErrorMatrixCompressor(int b)
{
  short int biasArray[6] = {116,120,118,116,116,102};


  double T[5]={0.2,0.5,100.0,100.0,20000.0};

  m_bitStrip=b;
  for(int i=0;i<6;i++)
    {
      m_biases[i]=biasArray[i];
    }
  for(int i=0;i<5;i++)
    {
      m_scales[i]=T[i];
    }
  m_limits[0]=15;
  m_limits[1]=31;
  m_srcMasks[0]= 0x80000000;
  m_srcMasks[1]= 0xC0000000; 
  m_srcMasks[2]= 0xE0000000; 
  m_srcMasks[3]= 0xF0000000;
  m_srcMasks[4]= 0xF8000000; 
  m_srcMasks[5]= 0xFC000000;
  m_srcMasks[6]= 0xFE000000;
  m_srcMasks[7]= 0xFF000000;
  m_srcMasks[8]= 0xFF800000;
  m_srcMasks[9]= 0xFFC00000;
  m_srcMasks[10]=0xFFE00000;
  m_srcMasks[11]=0xFFF00000;
  m_srcMasks[12]=0xFFF80000;
  m_srcMasks[13]=0xFFFC0000;
  m_srcMasks[14]=0xFFFE0000;
  m_srcMasks[15]=0xFFFF0000;
  m_tripVec.clear();
  m_tripVec.push_back(Triplet(0,1,3));
  m_tripVec.push_back(Triplet(2,4,6));
  m_tripVec.push_back(Triplet(5,7,8));
  m_tripVec.push_back(Triplet(9,10,11));
  m_tripVec.push_back(Triplet(14,12,13));
}

void ErrorMatrixCompressor::setBiases(const int b[6])
{
  for(int i=0;i<6;i++)
    {
      m_biases[i]=b[i];
    }
}

void ErrorMatrixCompressor::setUpperLimits(const int l[2])
{
  for(int i=0;i<2;i++)
    {
      m_limits[i]=l[i];
    }
}


bool ErrorMatrixCompressor::CholeskyDecomposition(double a[5][5], double L[5][5])
{

  int i,j,k;
  double sum;
  double p[5];

  for(i=0;i<5;i++)
    {
      for(j=i;j<5;j++)
	{
	  sum=a[i][j];
	  for(k=i-1;k>=0;k--)
	    sum-=a[i][k]*a[j][k];
	  if(i==j)
	    {
	      if(sum<=0.0)
		{
		  return false;
		}
	      p[i]=sqrt(sum);L[i][i]=p[i];
	    }
	  else
	    { 
	      a[j][i]=sum/p[i];
	      L[j][i]=a[j][i];
	    }
	}
    }
  return true;
}

bool ErrorMatrixCompressor::compress(std::vector<double> src, std::vector<unsigned int>& dest)
{
  int i,j;
  double L[5][5],C0[5][5],C[5][5];
  float S[5][5];

  dest.clear();

  int idx=0;
  for (i=0;i<5;i++)
    for(j=0;j<=i;j++)
      {
	C[i][j]=src[idx];
	C[j][i]=C0[i][j]=C0[j][i]=C[i][j];
	idx++;
      }

  memset(&L[0][0],0,sizeof(L));

  for(i=0;i<5;i++)
    for(j=0;j<=i;j++) 
      {
	C[i][j]*=(m_scales[i]*m_scales[j]);
	C[j][i]=C[i][j];
      }
  if(!CholeskyDecomposition(C,L)) return false;
  memset(&S[0][0],0,sizeof(S));
  for(i=0;i<5;i++)
    for(j=0;j<=i;j++) S[i][j]=L[i][j];

  std::vector<FloatRep> vecFR;
  vecFR.clear();

  for(i=0;i<5;i++)
    {
      for(j=0;j<=i;j++) 
	{
	  m_decoder.setF(S[i][j]);
	  char sign=(S[i][j]<0)?1:0;
	  unsigned int mant=m_decoder.getMantissa();
	  unsigned short int ex=m_decoder.getExponent();
	  vecFR.push_back(FloatRep(sign,ex,mant));
	}
    }
  std::vector<unsigned short> vShorts;
  vShorts.clear();
  if(!compressFR(vecFR,vShorts)) return false;

  //re-packing as ints

  //  std::cout<<"Total "<<vShorts.size()<<" unsigned short"<<std::endl;

  int nBits=0;
  unsigned int buffer = 0x00000000;

  for(std::vector<unsigned short>::iterator it = vShorts.begin(); it != vShorts.end();++it) {
    // printf("short = 0x%X\n",(*it));
    if(nBits==2) {
      dest.push_back(buffer);
      nBits=0;
      buffer = 0x00000000;
    }
    if(nBits==0) {
      buffer = (*it);
      buffer = buffer << 16;
      nBits++;
    }
    else {
      buffer = buffer | (*it);
      nBits++;
    }
  }
  dest.push_back(buffer);
  return true;
}
  
bool ErrorMatrixCompressor::restore(std::vector<unsigned int> src, std::vector<double>& dest)
{
  int i,j;
  float S[5][5];
  double L[5][5],C[5][5];

  dest.clear();

  std::vector<FloatRep> vecFR;
  vecFR.clear();

  std::vector<unsigned short> vShorts;

  vShorts.clear();

  for(std::vector<unsigned int>::iterator it = src.begin(); it!=src.end(); ++it) {
    unsigned short s1,s2;

    s1 = (unsigned short)((0xFFFF0000 & (*it)) >> 16);
    s2 = (unsigned short)(0x0000FFFF & (*it));
    vShorts.push_back(s1);
    //if(!((s2==0) && ((it+1)==src.end()))) //do not store last zero
    vShorts.push_back(s2);
  }

  if(!restoreFR(vShorts,vecFR)) return false;

  std::vector<FloatRep>::iterator fIt(vecFR.begin());
  memset(&S[0][0],0,sizeof(S));
  for(i=0;i<5;i++)
    for(j=0;j<=i;j++)
      {
	if(fIt==vecFR.end()) break;
	S[i][j]=(*fIt).restore();
        ++fIt;
      }

  memset(&L[0][0],0,sizeof(L));
  for(i=0;i<5;i++)
    for(j=0;j<=i;j++) L[i][j]=S[i][j];
  for(i=0;i<5;i++)  
    for(j=i;j<5;j++)
      {
	C[i][j]=0.0;
	for(int k=0;k<5;k++)
	  C[i][j]+=L[i][k]*L[j][k];
	C[j][i]=C[i][j];
      }
  for(i=0;i<5;i++)  
    for(j=0;j<=i;j++)
      {
	C[i][j]/=(m_scales[i]*m_scales[j]);
	C[j][i]=C[i][j];
      }
  for(i=0;i<5;i++) for(j=0;j<=i;j++) dest.push_back(C[i][j]);
  return true;
}


bool ErrorMatrixCompressor::compressFR(std::vector<FloatRep> src, std::vector<unsigned short>& dest)
{
  unsigned short buf=0x0000;
  dest.clear();
  int nMantLength=23-m_bitStrip;
  if(nMantLength<8)
    {
      std::cout<<"Requested mantissa reduction is too large: 23->"<<nMantLength<<std::endl;
      return false;
    }
  std::vector<FloatRep>::iterator fIt;
  /*
  for(fIt=src.begin();fIt!=src.end();++fIt)
    {
      printf("%d %d %d -> %2.8f\n",(*fIt).sign(),
	     (*fIt).exponent(),(*fIt).mantissa(),(*fIt).restore());
    }
  */
  // 1. Check limits
  fIt=src.begin();
  int i,j;
  for (i=0;i<5;i++)
    for (j=0;j<=i;j++)
      {
	unsigned short int ex=(*fIt).exponent();
	unsigned int mant=(*fIt).mantissa();
	int bias,limit;
	if(i==j)
	  {
	    bias=m_biases[i];
	    limit=m_limits[0];
	  }
	else
	  {
	    bias=m_biases[5];
	    limit=m_limits[1];
	  }
	if((ex!=0)&&(mant!=0))
	  {
	    if((ex<bias)||(ex-bias>limit))
	      {
		//		std::cout<<"i="<<i<<" j="<<j<<" ex="<<ex<<" bias="<<bias<<" lim="<<limit<<std::endl;
		return false;
	      }
	  }
	++fIt;
      }
  // 2. Pack exponents: diag e + non-diag s1 + non-diag e1 + non-diag s2 + non-diag e2
	
  i=0;
  for(std::vector<Triplet>::iterator trIt=m_tripVec.begin();trIt!=m_tripVec.end();++trIt)
    {
      int i1,i2[2];
      i1=(*trIt).m_d;
      i2[0]=(*trIt).m_nd1;i2[1]=(*trIt).m_nd2;
      //printf("%d %d %d\n",i1,i2[0],i2[1]);
      buf=0x0000;
      unsigned short e;
      
      e=src[i1].exponent()-m_biases[i];
      buf = buf | ((e<<12) & 0xF000);      
      if((src[i2[0]].exponent()==0)&&(src[i2[0]].mantissa()==0))
	{
	  e=src[i2[0]].exponent();
	  buf = buf | 0x0800;//-0
	}
      else
	{
	  if(src[i2[0]].sign())
	    buf = buf | 0x0800;
	  e=src[i2[0]].exponent()-m_biases[5];
	}
      buf = buf | ((e<<6) & 0x07C0);      
      if((src[i2[1]].exponent()==0)&&(src[i2[1]].mantissa()==0))
	{
	  buf = buf | 0x0020;//-0
	  e=src[i2[1]].exponent();
	}
      else
	{
	  e=src[i2[1]].exponent()-m_biases[5];
	  if(src[i2[1]].sign())
	    buf = buf | 0x0020;
	}
      buf = buf | (e & 0x001F);
      //      printf("Exponent 0x%X\n",buf);
      dest.push_back(buf);
      i++;
    }
  // 3. Pack reduced mantissas 
  fIt=src.begin();
  unsigned int nPacked=0;
  int nFreeBits=0,nBitsToStore=0,nBufferLength=0;
  unsigned int srcBuffer=0x00000000;
  //  printf("L=%d\n",nMantLength);
  while (nPacked<=src.size()+1)
    {
      if(nFreeBits==0)
	{
	  if(nBufferLength!=0)
	    {
	      dest.push_back(buf);
	      //      printf("Storing 0x%X\n",buf);
	    }
	  buf=0x0000;
	  nFreeBits=16;
	  nBufferLength++;
	}
      if(nBitsToStore==0)
	{
	  if(nPacked!=0)
	    ++fIt;
	  if(fIt==src.end()) break;
	  nPacked++;
	  srcBuffer=((*fIt).mantissa()<<9);
	  // printf("Packing 0x%X = %ud\n",(*fIt).mantissa(),(*fIt).mantissa());
	  nBitsToStore=nMantLength;
	}
      int Np=(nBitsToStore>nFreeBits)?nFreeBits:nBitsToStore;
      unsigned int mask=m_srcMasks[Np-1];
      unsigned int slice = srcBuffer & mask;
      slice = (slice >> (32-nFreeBits)) & 0x0000FFFF;
      unsigned int tmp=(unsigned int)(slice);
      //	printf("Np=%d Tmp=0x%X\n",Np,tmp);
      buf =  buf | tmp;
      srcBuffer = srcBuffer << Np;
      //	printf("dest=0x%X src=0x%X\n",buf,srcBuffer);
      nFreeBits-=Np;
      nBitsToStore-=Np;
      //cout<<"F="<<nFreeBits<<" TS="<<nBitsToStore<<endl;
    }
  //  printf("Storing 0x%X\n",buf);
  dest.push_back(buf);
  src.clear();
  return true;
}
  
bool ErrorMatrixCompressor::restoreFR(std::vector<unsigned short> src, std::vector<FloatRep>& dest)
{
  int i,nRestored,nFreeBits,nBitsToStore;
  unsigned short buf=0x0000;
  unsigned int destBuffer=0x00000000;
  std::vector<unsigned short>::iterator uIt(src.begin());
  dest.clear();

  for(i=0;i<15;i++)
    {
      dest.push_back(FloatRep(0,0,0));
    }

  i=0;
  for(;uIt!=src.end();++uIt)
    {
      int i0=m_tripVec[i].m_d;
      int i1=m_tripVec[i].m_nd1;
      int i2=m_tripVec[i].m_nd2;
      buf=(*uIt);
      char s=((buf & 0x0800)==0)?0:1;
      dest[i1].sign(s);
      s=((buf & 0x0020)==0)?0:1;
      dest[i2].sign(s);
      unsigned short e = ((buf & 0xF000) >> 12);
      dest[i0].exponent(e+m_biases[i]);
      e = ((buf & 0x07C0) >> 6);
      dest[i1].exponent(e+m_biases[5]);
      e = (buf & 0x001F);
      dest[i2].exponent(e+m_biases[5]);
      i++;
      if(i==5) break;
    }
  if(i<5) return false;
  nRestored=0;
  nBitsToStore=0;
  nFreeBits=0;
  int nMantLength=23-m_bitStrip;
  while(nRestored<=15)
    {	
      if(nFreeBits==0)
	{
	  if(nRestored!=0)
	    {
	      //printf("Dest 0x%X ",destBuffer);
	      destBuffer = destBuffer << m_bitStrip;
	      //printf("<< 0x%X = %lu\n",destBuffer,destBuffer);
	      dest[nRestored-1].mantissa(destBuffer);
	    }
	  ++nRestored;
	  destBuffer=0x00000000;
	  nFreeBits=nMantLength;
	}
      if(nBitsToStore==0)
	{
          if(uIt==src.end()) break;
	  ++uIt;
          if(uIt==src.end()) 
	    {
	      //printf("Breaking ... nR=%d\n",nRestored);
	      break;
	    }
	  nBitsToStore=16;buf=(*uIt);//printf("Source 0x%X\n",buf);
	}
      int Np=(nFreeBits>nBitsToStore) ? nBitsToStore : nFreeBits;
      unsigned int tmp = buf;
      tmp = tmp >> (16-Np);
      buf = (unsigned int)((buf << Np) & 0x0000FFFF);
      //printf("F=%d TS=%d Np=%d 0x%X 0x%X\n",nFreeBits,nBitsToStore,Np,tmp,buf);
      nBitsToStore-=Np;
      //printf("Copy 0x%X ",destBuffer);
      destBuffer = destBuffer << Np;
      //printf("<< 0x%X ",destBuffer);
      destBuffer = destBuffer | tmp;
      //printf(" | 0x%X\n",destBuffer);
      nFreeBits-=Np;
      //printf("nR=%d\n",nRestored);
    }
  return true;
}

