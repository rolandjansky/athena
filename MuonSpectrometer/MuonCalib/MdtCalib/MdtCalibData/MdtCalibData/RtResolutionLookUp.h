/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIB_RTRESOLUTIONLOOKUP_H
#define MUONCALIB_RTRESOLUTIONLOOKUP_H

#include "MdtCalibData/IRtResolution.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include <iostream>

namespace MuonCalib{

  /** Equidistant look up table for resolution tables with the time as key. 
      The first parameter should be the time corresponding to the first_bin.
      The second parameter should be the binsize.
      
      The r value is calculated by linear interpolation.
  */
  
class RtResolutionLookUp : public IRtResolution {
  public:
    explicit RtResolutionLookUp( const ParVec& vec ) : IRtResolution(vec)  {
      if( vec.size() < 4 ){
        MsgStream log(Athena::getMessageSvc(),"RtResolutionLookUp");
        log<<MSG::WARNING<<"<to few parameters>"<<endmsg;
        m_t_min=9e9;
        m_bin_size=1.0; //will be always out of range	
      } else {
        m_t_min = par(0);
        m_bin_size = par(1);
      }
    }
    
    std::string name() const {
      return "RtResolutionLookUp"; 
    }

    /** returns drift radius for a given time */
    double resolution( double t, double ) const;

  private:
    int getBin( double t ) const { return (int)((t-m_t_min)/m_bin_size); }
    
    // take offset due to m_t_min and binsize into account
    int rtBins() const { return nPar()-2; }
    double getRadius( int bin ) const { return par( bin + 2 ); }
    // returns best matching bin within rtRange
    int binInRtRange( double t) const;

    double m_t_min;
    double m_bin_size;
  };

  inline
  double RtResolutionLookUp::resolution( double t, double ) const {

    // get first bin 
    int bin = binInRtRange( t );

    // shift bin so we are using the last two bins for extrapolation
    if( bin >= rtBins() - 1 ) bin = rtBins() - 2;

    double r1 = getRadius( bin );      // get bin value
    double r2 = getRadius( bin + 1 );  // get value of next bin
    double dr = r2-r1;

    // scale factor for interpolation
    double scale = (t-m_t_min)/m_bin_size - (double)bin;

    double reso = r1 + dr*scale;
    return reso >= 0 ? reso : 0;
  }

  inline
  int RtResolutionLookUp::binInRtRange( double t) const {
    // get bin 
    int bin = getBin( t );

    // if t corresponds to a negative bin return first
    if( bin < 0 ) bin = 0;

    // if t corresponds to a bin outside the range of the lookup table return the last bin
    if( bin >= rtBins() ) bin = rtBins()-1;

    return bin;
  }
}  //namespace MuonCalib
#endif
