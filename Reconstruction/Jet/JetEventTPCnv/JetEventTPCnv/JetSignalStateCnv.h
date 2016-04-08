/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEVENTTPCNV_SIGNALSTATESTORE_H 
#define JETEVENTTPCNV_SIGNALSTATESTORE_H 

/** *************************************
 @class SignalStateCnv  Helper class to convert a double into a char 

 New implementation: Sept. 12th 2008 Rolf Seuster
 
 Still keep the old implementation around - we need this to read old jets.
 Now, we store the differences as
  Delta_0 = M_cal / M_raw - 1
  Delta_1 = phi_cal - phi_raw
  Delta_2 = eta_cal - eta_raw
  Delta_3 = Pt_cal / Pt_raw - 1
  
 These differences will in general be small, so storing them as a semi-float 
 will help. The new functions return a vector<unsigned short> where all deltas are
 compressed together, a bit simular to EventCommonTPCnv/Compressor.
 An additional parameter determines the compression, store everything in
 2, 3, 4, 6 or 8 short.
  2 short ==  4 char =  8 bit / delta  : 3 exponent, 1 sign,  4 mantissa
  3 short ==  48 bit = 12 bit / delta  : 4 exponent, 1 sign,  7 mantissa
  4 short ==  64 bit = 16 bit / delta  : 5 exponent, 1 sign, 10 mantissa
  8 short == 128 bit == 4 floats
 We can also take into account that:
   Delta_0/3 are always positive
   Delta_1   is in [-pi;pi]
 to improve the precision.
 
 
 Convert ratio of calibrated and uncalibrated jet quantities
 into a char. It is assumed that the ratio is in a given range
 so that he precision loss due to converting a float in a double is 
 small (~0.5 %)
 
 first implementation (before finding better/cleaner way?)

 @author P.A. Delsart
 
************************************* */

#include "JetEventTPCnv/JetConverterBase.h"

#include <cmath>

class SignalStateCnv {
 public:
  
  SignalStateCnv(){
    m_delta_p =1.0;
    m_step_p  = 1.0 / 127;
    m_delta_m = 0.4;
    m_step_m  = 0.4 / 127;
    m_center  = 0.8;
  }

  // converting double ratio to char to store uncalibrated quantities :
  // We will assume that ratio uncal/cal is in [1-delta_m, 1+delta_p]
  double m_delta_p ;
  double m_step_p  ;
  double m_delta_m ;
  double m_step_m  ;
  double m_center  ;


  // round to closest integer :
  char round(double d){return (char)floor(d+0.5);}

  char char_from_ratio(double r){
    double d= r -m_center ;
    if( d>0){
      if(d>m_delta_p) d = m_delta_p;
      return round(d/m_step_p);    
    }else{
      if(d<-m_delta_m) d = m_delta_m;
      return (round(fabs(d)/m_step_m) | 128);    
    }

  }

  double ratio_from_char(char c){
    bool isneg = c & 128; // are storing a negativ diff ?
    int val   = c & 127;  // the actual diff value
    if(isneg){
      return  (m_center - val*m_step_m);
    }else{
      return  (m_center + val*m_step_p);
    }

  }
  
  enum CompressionLevel { HIGH, MEDIUM, LOW, NO_COMPRESSION };
  
  /* the new configurable converters */
  JetConverterTypes::signalState_pers_t
    compress( const JetConverterTypes::momentum & momCal,
	      const JetConverterTypes::momentum & momRaw,
	      CompressionLevel level, MsgStream& msg )
    {
      int exponent[4];
      double mantissa[4] = {0};
      JetConverterTypes::signalState_pers_t ps;
      
      double delta0(0);
      double delta1(0);
      double delta2(0);
      double delta3(0);
      int factor;
      
      msg << MSG::VERBOSE << "called JetSignalStateCnv::compress() for :" << endreq;
      msg << MSG::VERBOSE << " raw momentum ( px | py | pz | m ) : ( "
	  << momRaw.m_px << " | " 
	  << momRaw.m_py << " | " 
	  << momRaw.m_pz << " | " 
	  << momRaw.m_m << " )" << endreq;
      msg << MSG::VERBOSE << " cal momentum ( px | py | pz | m ) : ( "
	  << momCal.m_px << " | " 
	  << momCal.m_py << " | " 
	  << momCal.m_pz << " | " 
	  << momCal.m_m << " )" << endreq;

      bool forceNoCompression=false;
      
      // switch off compression, if M or pT of raw or calibrated jet are zero...
      if ( momRaw.m_m == 0 || momCal.m_m == 0 )
	forceNoCompression=true;
      if ( momRaw.m_px == 0 && momRaw.m_py == 0 )
	forceNoCompression=true;
      if ( momCal.m_px == 0 && momCal.m_py == 0 )
	forceNoCompression=true;
      
      if ( forceNoCompression )
	{
	  msg << MSG::DEBUG << "M or PT of calibrated or raw signal state of jet zero !"
	      << " Switching off compression for this jet !!!" << endreq;
	  level = NO_COMPRESSION;
	}
      
      // prepare for compression the ratios
      if ( level != NO_COMPRESSION )
	{
	  double angleC = atan2( momCal.m_py, momCal.m_px );
	  double angleR = atan2( momRaw.m_py, momRaw.m_px );
	  double p_traC = sqrt( momCal.m_px * momCal.m_px + momCal.m_py * momCal.m_py );
	  double p_traR = sqrt( momRaw.m_px * momRaw.m_px + momRaw.m_py * momRaw.m_py );
	  mantissa[0] = frexp( momCal.m_m / momRaw.m_m - 1, &exponent[0] );
	  mantissa[1] = frexp( angleC - angleR,             &exponent[1] );
	  mantissa[2] = frexp( momCal.eta() - momRaw.eta(), &exponent[2] );
	  mantissa[3] = frexp( p_traC / p_traR - 1,         &exponent[3] );
	  
	  delta0 = momCal.m_m / momRaw.m_m - 1;
	  delta1 = angleC - angleR;
	  delta2 = momCal.eta() - momRaw.eta();
	  delta3 = p_traC / p_traR - 1;
	}
      
      unsigned long tmp0(0);
      unsigned long tmp1(0);
      unsigned long tmp2(0);
      unsigned long tmp3(0);
      
      // here, we'll store the 
      unsigned short vec0(0);
      unsigned short vec1(0);
      unsigned short vec2(0);
      
      switch(level)
	{
	case HIGH:               // HIGH compression means, store differnces in 2 shorts
	  factor = 0;            // compress the mass ratio
	  exponent[0] += 2;
	  if ( exponent[0] > 0x7 ) exponent[0] = 0x7;
	  if ( exponent[0] < 0x0 )
	    {
	      factor = -exponent[0];
	      exponent[0] = 0x0;
	    }
	  tmp0 = int( fabs(mantissa[0]) * 0x10 ) >> factor;
	  tmp0 |= ( exponent[0] & 0x7 ) << 5;
	  if ( mantissa[0] < 0 ) tmp0 |= 0x10;
	  
	  factor = 0;            // compress delta phi
	  exponent[1] += 5;
	  if ( exponent[1] > 0x7 ) exponent[1] = 0x7;
	  if ( exponent[1] < 0x0 )
	    {
 	      factor = -exponent[1];
	      exponent[1] = 0x0;
	    }
	  tmp1 = int( fabs(mantissa[1]) * 0x10 ) >> factor;
	  tmp1 |= ( exponent[1] & 0x7 ) << 5;
	  if ( mantissa[1] < 0 ) tmp1 |= 0x10;
	  
	  factor = 0;            // compresss delta eta
	  exponent[2] += 5;
	  if ( exponent[2] > 0x7 ) exponent[2] = 0x7;
	  if ( exponent[2] < 0x0 )
	    {
 	      factor = -exponent[2];
	      exponent[2] = 0x0;
	    }
	  tmp2 = int( fabs(mantissa[2]) * 0x10 ) >> factor;
	  tmp2 |= ( exponent[2] & 0x7 ) << 5;
	  if ( mantissa[2] < 0 ) tmp2 |= 0x10;
	  
	  factor = 0;            // compresss the pT ratio
	  exponent[3] += 5;
	  if ( exponent[3] > 0x7 ) exponent[3] = 0x7;
	  if ( exponent[3] < 0x0 )
	    {
	      factor = -exponent[3];
	      exponent[3] = 0x0;
	    }
	  tmp3 = int( fabs(mantissa[3]) * 0x10 ) >> factor;
	  tmp3 |= ( exponent[3] & 0x7 ) << 5;
	  if ( mantissa[3] < 0 ) tmp3 |= 0x10;
	  
	  vec0  = tmp0;
	  vec0 += tmp1 << 8;
	  vec1  = tmp2;
	  vec1 += tmp3 << 8;
	  ps.push_back(vec0);
	  ps.push_back(vec1);
	  break;
	  
	case MEDIUM:             // MEDIUM compression means, store differences in 3 shorts
	  factor = 0;            // compress the mass ratio
	  exponent[0] += 3;
	  if ( exponent[0] > 0xF ) exponent[0] = 0xF;
	  if ( exponent[0] < 0x0 )
	    {
	      factor = -exponent[0];
	      exponent[0] = 0;
	    }
	  tmp0 = int( fabs(mantissa[0]) * 0x80 ) >> factor;
	  if ( mantissa[0] < 0 ) tmp0 |= 0x80;
	  
	  factor = 0;            // compress delta phi
	  exponent[1] += 6;
	  if ( exponent[1] > 0xF ) exponent[1] = 0xF;
	  if ( exponent[1] < 0x0 )
	    {
	      factor = -exponent[1];
	      exponent[1] = 0;
	    }
	  tmp1 = int( fabs(mantissa[1]) * 0x80 ) >> factor;
	  if ( mantissa[1] < 0 ) tmp1 |= 0x80;
	  
	  factor = 0;            // compresss delta eta
	  exponent[2] += 6;
	  if ( exponent[2] > 0xF ) exponent[2] = 0xF;
	  if ( exponent[2] < 0x0 )
	    {
	      factor = -exponent[2];
	      exponent[2] = 0;
	    }
	  tmp2 = int( fabs(mantissa[2]) * 0x80 ) >> factor;
	  if ( mantissa[2] < 0 ) tmp2 |= 0x80;
	  
	  factor = 0;            // compresss the pT ratio
	  exponent[3] += 6;
	  if ( exponent[3] > 0xF ) exponent[3] = 0xF;
	  if ( exponent[3] < 0x0 )
	    {
	      factor = -exponent[3];
	      exponent[3] = 0;
	    }
	  tmp3 = int( fabs(mantissa[3]) * 0x80 ) >> factor;
	  if ( mantissa[3] < 0 ) tmp3 |= 0x80;
	  
	  vec0  =   ( tmp0 & 0xFF );
	  vec0 +=   ( tmp1 & 0xFF ) << 8;
	  vec1  =   ( tmp2 & 0xFF );
	  vec1 +=   ( tmp3 & 0xFF ) << 8;
	  vec2  =     exponent[0];
	  vec2 |=     exponent[1] <<  4;
	  vec2 |=     exponent[2] <<  8;
	  vec2 |=     exponent[3] << 12;
	  ps.push_back(vec0);
	  ps.push_back(vec1);
	  ps.push_back(vec2);
	  break;
	  
	case LOW:                // LOW compression means, store differences in 4 shorts
	  factor = 0;            // compress the mass ratio
	  exponent[0] += 5;
	  if ( exponent[0] > 31 ) exponent[0] = 31;
	  if ( exponent[0] <  0 )
	    {
	      factor = -exponent[0];
	      exponent[0] = 0;
	    }
	  tmp0 = int( fabs(mantissa[0]) * 0x400 ) >> factor;
	  tmp0 |= exponent[0] << 11;
	  if ( mantissa[0] < 0 ) tmp0 |= 0x400;
	  
	  factor = 0;            // compress delta phi
	  exponent[1] += 7;
	  if ( exponent[1] > 31 ) exponent[1] = 31;
	  if ( exponent[1] <  0 )
	    {
	      factor = -exponent[1];
	      exponent[1] = 0;
	    }
	  tmp1 = int( fabs(mantissa[1]) * 0x400 ) >> factor;
	  tmp1 |= exponent[1] << 11;
	  if ( mantissa[1] < 0 ) tmp1 |= 0x400;
	  
	  factor = 0;            // compresss delta eta
	  exponent[2] += 7;
	  if ( exponent[2] > 31 ) exponent[2] = 31;
	  if ( exponent[2] <  0 )
	    {
	      factor = -exponent[2];
	      exponent[2] = 0;
	    }
	  tmp2 = int( fabs(mantissa[2]) * 0x400 ) >> factor;
	  tmp2 |= exponent[2] << 11;
	  if ( mantissa[2] < 0 ) tmp2 |= 0x400;
	  
	  factor = 0;            // compresss the pT ratio
	  exponent[3] += 7;
	  if ( exponent[3] > 31 ) exponent[3] = 31;
	  if ( exponent[3] <  0 )
	    {
	      factor = -exponent[3];
	      exponent[3] = 0;
	    }
	  tmp3 = int( fabs(mantissa[3]) * 0x400 ) >> factor;
	  tmp3 |= exponent[3] << 11;
	  if ( mantissa[3] < 0 ) tmp3 |= 0x400;
	  
	  ps.push_back(tmp0);
	  ps.push_back(tmp1);
	  ps.push_back(tmp2);
	  ps.push_back(tmp3);
  	  break;
	case NO_COMPRESSION:
	default:
	  union {
	    unsigned short s[2];
	    float f;
	  } m;
	  
	  m.f = momRaw.m_px;  ps.push_back(m.s[0]);  ps.push_back(m.s[1]);
	  m.f = momRaw.m_py;  ps.push_back(m.s[0]);  ps.push_back(m.s[1]);
	  m.f = momRaw.m_pz;  ps.push_back(m.s[0]);  ps.push_back(m.s[1]);
	  m.f = momRaw.m_m;   ps.push_back(m.s[0]);  ps.push_back(m.s[1]);
	  msg << MSG::VERBOSE << " compress x : " << m.f << " = " << m.s[0] << " = " << m.s[1] << endreq;
	  msg << MSG::VERBOSE << " compress y : " << m.f << " = " << m.s[0] << " = " << m.s[1] << endreq;
	  msg << MSG::VERBOSE << " compress z : " << m.f << " = " << m.s[0] << " = " << m.s[1] << endreq;
	  msg << MSG::VERBOSE << " compress m : " << m.f << " = " << m.s[0] << " = " << m.s[1] << endreq;
	  break;
	}
      if ( msg.level() <= MSG::VERBOSE )
	{
	  msg << MSG::VERBOSE << " compress # ps : " << ps.size() << " : ";
	  for ( JetConverterTypes::signalState_pers_t::const_iterator it=ps.begin(); it != ps.end(); ++it )
	    msg << std::hex << *it << " ";
	  msg << std::dec << endreq;
	  msg << MSG::VERBOSE << " compress x : " << momRaw.m_px << endreq;
	  msg << MSG::VERBOSE << " compress y : " << momRaw.m_py << endreq;
	  msg << MSG::VERBOSE << " compress z : " << momRaw.m_pz << endreq;
	  msg << MSG::VERBOSE << " compress m : " << momRaw.m_m  << endreq;
	  
	  int d0(0);
	  int d1(0);
	  
	  switch(level)
	    {
	    case HIGH:    d0=2; d1=5; break;
	    case MEDIUM:  d0=3; d1=6; break;
	    case LOW:     d0=5; d1=7; break;
	    case NO_COMPRESSION:
	    default:                  break;
	    };
	  
	  msg << MSG::DEBUG << " compress M   : " << delta0 << " = "
	      << mantissa[0] << " *2^ " << exponent[0]-d0 << endreq;
	  msg << MSG::DEBUG << " compress phi : " << delta1 << " = "
	      << mantissa[1] << " *2^ " << exponent[1]-d1 << endreq;
	  msg << MSG::DEBUG << " compress eta : " << delta2 << " = "
	      << mantissa[2] << " *2^ " << exponent[2]-d1 << endreq;
	  msg << MSG::DEBUG << " compress pT  : " << delta3 << " = "
	      << mantissa[3] << " *2^ " << exponent[3]-d1 << endreq;
	}
      return ps;
    };
  
  JetConverterTypes::momentum decompress( JetConverterTypes::signalState_pers_t ps,
					  JetConverterTypes::momentum momCal,
					  MsgStream& msg )
    {
      JetConverterTypes::momentum momRaw;
      int exponent[4] = {0};
      double mantissa[4] = {0};
      //std::cout<< "   decompressing momCal px="<< momCal.m_px << "  "<< momCal.m_py << "  "<< momCal.m_pz << "  "<< momCal.m_m << std::endl;
      
      switch( ps.size() )
	{
	case 2:
	  exponent[0] = ( ( ps[0] >>  5 ) & 0x7 ) - 2;
	  exponent[1] = ( ( ps[0] >> 13 ) & 0x7 ) - 5;
	  exponent[2] = ( ( ps[1] >>  5 ) & 0x7 ) - 5;
	  exponent[3] = ( ( ps[1] >> 13 ) & 0x7 ) - 5;
	  
	  mantissa[0] = double(   ps[0]        & 0x1F ) / 0x10;
	  mantissa[1] = double( ( ps[0] >> 8 ) & 0x1F ) / 0x10;
	  mantissa[2] = double(   ps[1]        & 0x1F ) / 0x10;
	  mantissa[3] = double( ( ps[1] >> 8 ) & 0x1F ) / 0x10;
	  if ( ps[0] & 0x10   ) mantissa[0] = -mantissa[0];
	  if ( ps[0] & 0x1000 ) mantissa[1] = -mantissa[1];
	  if ( ps[1] & 0x10   ) mantissa[2] = -mantissa[2];
	  if ( ps[1] & 0x1000 ) mantissa[3] = -mantissa[3];
	  break;
	  
	case 3:
	  exponent[0] = (   ps[2]         & 0xF ) - 3;
	  exponent[1] = ( ( ps[2] >>  4 ) & 0xF ) - 6;
	  exponent[2] = ( ( ps[2] >>  8 ) & 0xF ) - 6;
	  exponent[3] = ( ( ps[2] >> 12 ) & 0xF ) - 6;
	  
	  mantissa[0] = double(   ps[0]        & 0x7F ) / 0x80;
	  mantissa[1] = double( ( ps[0] >> 8 ) & 0x7F ) / 0x80;
	  mantissa[2] = double(   ps[1]        & 0x7F ) / 0x80;
	  mantissa[3] = double( ( ps[1] >> 8 ) & 0x7F ) / 0x80;
	  if ( ps[0] & 0x80   ) mantissa[0] = -mantissa[0];
	  if ( ps[0] & 0x8000 ) mantissa[1] = -mantissa[1];
	  if ( ps[1] & 0x80   ) mantissa[2] = -mantissa[2];
	  if ( ps[1] & 0x8000 ) mantissa[3] = -mantissa[3];
	  break;
	  
	case 4:
	  exponent[0] = ( ( ps[0] >> 11 ) & 0x1F ) - 5;
	  exponent[1] = ( ( ps[1] >> 11 ) & 0x1F ) - 7;
	  exponent[2] = ( ( ps[2] >> 11 ) & 0x1F ) - 7;
	  exponent[3] = ( ( ps[3] >> 11 ) & 0x1F ) - 7;
	  
	  mantissa[0] = double( ps[0] & 0x3FF ) / 0x400;
	  mantissa[1] = double( ps[1] & 0x3FF ) / 0x400;
	  mantissa[2] = double( ps[2] & 0x3FF ) / 0x400;
	  mantissa[3] = double( ps[3] & 0x3FF ) / 0x400;
	  if ( ps[0] & 0x400 ) mantissa[0] = -mantissa[0];
	  if ( ps[1] & 0x400 ) mantissa[1] = -mantissa[1];
	  if ( ps[2] & 0x400 ) mantissa[2] = -mantissa[2];
	  if ( ps[3] & 0x400 ) mantissa[3] = -mantissa[3];
	  break;
	  
	case 8:
	default:
	  union {
	    unsigned short s[2];
	    float f;
	  } m;
	  
	  m.s[0] = ps[0]; m.s[1] = ps[1]; momRaw.m_px = m.f;
	  m.s[0] = ps[2]; m.s[1] = ps[3]; momRaw.m_py = m.f;
	  m.s[0] = ps[4]; m.s[1] = ps[5]; momRaw.m_pz = m.f;
	  m.s[0] = ps[6]; m.s[1] = ps[7]; momRaw.m_m = m.f;
	  msg << MSG::VERBOSE << " RS x : " << m.f << " = " << m.s[0] << " = " << m.s[1] << endreq;
	  msg << MSG::VERBOSE << " RS y : " << m.f << " = " << m.s[0] << " = " << m.s[1] << endreq;
	  msg << MSG::VERBOSE << " RS z : " << m.f << " = " << m.s[0] << " = " << m.s[1] << endreq;
	  msg << MSG::VERBOSE << " RS m : " << m.f << " = " << m.s[0] << " = " << m.s[1] << endreq;
	  break;
	};
      
      if ( ps.size() < 8 )
	{
	  double delta0 = ldexp( mantissa[0], exponent[0] );
	  double delta1 = ldexp( mantissa[1], exponent[1] );
	  double delta2 = ldexp( mantissa[2], exponent[2] );
	  double delta3 = ldexp( mantissa[3], exponent[3] );

	  //std::cout<< " decompressing "<< delta0 <<"  "<< delta1 << "  "<< delta2 <<"  "<< delta3 << "  "<< std::endl;

	  if( ( delta0==-1) || (delta3==-1)){
	    msg << MSG::WARNING << "A jet was badly decompressed. Returning null to avoid fpe in SignalStateCnv::decompress "<<endreq;
	    return momRaw;
	  }
	  
	  msg << MSG::VERBOSE << " RS 0 : " << delta0 << " = " << mantissa[0] << " = " << exponent[0] << endreq;
	  msg << MSG::VERBOSE << " RS 1 : " << delta1 << " = " << mantissa[1] << " = " << exponent[1] << endreq;
	  msg << MSG::VERBOSE << " RS 2 : " << delta2 << " = " << mantissa[2] << " = " << exponent[2] << endreq;
	  msg << MSG::VERBOSE << " RS 3 : " << delta3 << " = " << mantissa[3] << " = " << exponent[3] << endreq;
	  
	  double angleC = atan2( momCal.m_py, momCal.m_px );
	  double p_traC = sqrt( momCal.m_px * momCal.m_px + momCal.m_py * momCal.m_py );
	  
	  double angleR = angleC - delta1;
	  double eta_R  = momCal.eta() - delta2;
	  double p_traR = p_traC / ( delta3 + 1 );
	  momRaw.m_m  = momCal.m_m / ( delta0 + 1 );
	  momRaw.m_px = p_traR * cos(angleR);
	  momRaw.m_py = p_traR * sin(angleR);
	  momRaw.m_pz = p_traR * sinh(eta_R);
	}
      msg << MSG::VERBOSE << " RS # ps : " << ps.size() << " : ";
      for ( JetConverterTypes::signalState_pers_t::const_iterator it=ps.begin(); it != ps.end(); ++it )
	msg << std::hex << *it << " ";
      msg << std::dec << endreq;
      msg << MSG::VERBOSE << " RS x : " << momRaw.m_px << endreq;
      msg << MSG::VERBOSE << " RS y : " << momRaw.m_py << endreq;
      msg << MSG::VERBOSE << " RS z : " << momRaw.m_pz << endreq;
      msg << MSG::VERBOSE << " RS m : " << momRaw.m_m  << endreq;
      
      return momRaw;
    };
  
};
#endif
