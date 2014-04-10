/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 *  ErrorMatrixCompressor.h
 *
 *  Created by Dmitry Emeliyanov on 12/11/2008.
 *  <Dmitry.Emeliyanov@cern.ch>
 *
 */

#ifndef __ERROR_MATRIX_COMPRESSOR_H__
#define __ERROR_MATRIX_COMPRESSOR_H__
  
#include <vector>
#include <iostream>


class DecoderFloat_IEEE754
{

union FloatLongIntUnion
{
  float f;
  unsigned int l;
};

 public:
  DecoderFloat_IEEE754()
    {
      m_data.f=0.0;
    }
  ~DecoderFloat_IEEE754() {};
  void setF(float f) 
    {
      m_data.f=f;
    }
  float getF()
    {
      if(m_data.l==(unsigned int)0x80000000)
	return 0.0;
      else
	return m_data.f;
    }
  void setL( int l)
    {
      m_data.l=l;
    }
  unsigned int getL()
    {
      return m_data.l;
    }
  void print();
  short int getExponent();
  unsigned int getMantissa();
  void setExponent(short int);
  void setSign(int);

  FloatLongIntUnion m_data;
};

class FloatRep
{
public:
  FloatRep(char s, unsigned short e, unsigned int m) :
    m_sign(s), m_exp(e), m_mant(m)
    {
      
    }
  ~FloatRep(){};
  float restore()
    {
      union FL {
	float m_f;
	unsigned int m_l;
      } decoder;

      if((m_mant==0)&&(m_sign!=0))
	return 0.0;

      unsigned int buf=0x00000000;

      buf = m_mant;
      buf = buf | (m_exp << 23);
      if (m_sign!=0)
	buf = buf | 0x80000000;
      decoder.m_l=buf;
      return decoder.m_f;
    }
  char sign()
    {
      return m_sign;
    }
  unsigned short exponent()
    {
      return m_exp;
    }
  unsigned int mantissa()
    {
      return m_mant;
    }
  void sign(char s)
    {
      m_sign=(s==0)?0:1;
    }
  void exponent(unsigned short int e)
    {
      m_exp=e;
    }
  void mantissa(unsigned int m)
    {
      m_mant=m;
    }

private:
  char m_sign;
  unsigned int m_exp;
  unsigned int m_mant;
};



class ErrorMatrixCompressor{

class Triplet {

  public :
    Triplet(int d, int nd1, int nd2) : m_d(d), m_nd1(nd1), m_nd2(nd2)
    {
      
    }
  ~Triplet(){};
  int m_d,m_nd1,m_nd2;
};

 public:

  ErrorMatrixCompressor(int);
  ~ErrorMatrixCompressor(){m_tripVec.clear();}

  bool compress(std::vector<double>, std::vector<unsigned int>&);
  bool restore(std::vector<unsigned int>, std::vector<double>&);
  void setBiases(const int b[6]);
  void setUpperLimits(const int l[2]);

 protected:

  bool CholeskyDecomposition(double a[5][5], double L[5][5]);
  bool compressFR(std::vector<FloatRep>,std::vector<unsigned short>&);
  bool restoreFR(std::vector<unsigned short>,std::vector<FloatRep>&);

  short int m_biases[6];
  short int m_limits[2];
  double m_scales[5];

  int m_bitStrip;

 private:
  DecoderFloat_IEEE754 m_decoder;
  unsigned int m_srcMasks[16];
  std::vector<Triplet> m_tripVec;
};

#endif
