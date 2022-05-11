/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * L1TopoDataTypes.cpp
 * author : Ignacio Aracena <ignacio.aracena@cern.ch>
 * Created June 2015
 *
 * @brief Tools for handling fixed point Qm.n numbers
 *
**********************************/

#include "L1TopoSimulationUtils/L1TopoDataTypes.h"
#include <string>

using namespace TSU;

TSU::T TSU::complement(const T& v, const unsigned int& p) {
   T res=0;
   for(unsigned int j=0;j<p;++j){
       res += v & (1ul << j) ? 0 : (1ul << j);
   }
   res += 1;
   return res;
}

TSU::T TSU::convert(const T& v, const unsigned& in_p, const unsigned int& in_f, const unsigned int& new_p, const unsigned int& new_f){
   T origval = v;
   // Check if sign bit is set and invert if so
   if((v >> (in_p-1ul)) & 1ul){ origval = complement(v,in_p); }
   T valint = ((origval >> in_f) & ~(1ul<<(in_p-in_f-1ul))) & ((1ul<<(in_p-in_f-1))-1ul);
   T mantissa = (new_f > in_f) ? ((origval & ((1ul<<in_f)-1ul)) << (new_f-in_f)) : ((origval & ((1ul<<in_f)-1ul))>>(in_f-new_f));
   if(!new_f) {mantissa = 0;}
   T out_val = ((valint << new_f) | mantissa);
   if(new_f+1==new_p) out_val = mantissa;
   if((v >> (in_p-1ul)) & 1ul){ // Check if original m_tvalue had sign bit set
       out_val = complement(out_val,new_f+(new_p-new_f-1)+1);
   }
   return out_val;
}

std::string TSU::to_binary(T b, const unsigned int& p){
   std::string pat(p,'0');
   unsigned int idx = p-1;
   while(b){
       pat[idx] = (b & 1ul) ? '1' : '0';
       b = b >> 1ul;
       --idx;
   }
   return pat;
}

float TSU::to_float(const T& value, const unsigned int& m, const unsigned int& n){
   // Find sign
   float res = ((value>>(m-1ul))&1ul) ? -(1ul<<(m-n)) : 0.;
   // Get integer part
   res += (value>>n)&((1ul<<(m-n))-1ul) ? float((value>>n)&((1ul<<(m-n))-1ul)) : 0;
   // Do the fractional part
   for(unsigned int j=0;j<n;++j){
       res += (value & (1ul << (n-1ul-j))) ? 1./(2ul<<(j)) : 0;
   }
   return res;
}
    
