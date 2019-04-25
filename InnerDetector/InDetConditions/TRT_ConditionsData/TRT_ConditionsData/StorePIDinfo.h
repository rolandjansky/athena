/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef INDETTRT_STOREPIDINFO
#define INDETTRT_STOREPIDINFO

///////////////////////////////////////////////////////////////////
// StorePIDinfo.h , (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#include <vector>
#include "GaudiKernel/StatusCode.h"
class StorePIDinfo{
  public:
   StorePIDinfo();
   StorePIDinfo(int nbins, float min, float max, std::vector<float> values);
   ~StorePIDinfo();
   void update (int nbins, float min, float max, std::vector<float> values );
   void push_back ( float value );
   StatusCode check ( int gas, int detpart) const; 
   float GetValue ( float input	) const; 
   float GetBinValue ( int bin ) const; 
   int   GetBin   ( float input	) const; 
  private:
   unsigned int m_nbins		;
   float m_min		;
   float m_max		;
   std::vector<float>  	m_values;
};
#endif

