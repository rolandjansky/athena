/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCCONDPARTYPE_H
#define CSCCONDPARTYPE_H

namespace MuonCalib { 
  class CscCondParType  {
    public:
      //Note that some software expects all flags STAT to be status flag related
      enum {
        GAIN,           //Gas gain
        PULSE_SLOPE,    //relative strip fc/ADC gain determined from pulser
        RUN_SLOPE,      //relative strip fc/AD gain determined from normal data run
        PED,            //Pedestal mean
        NOISE,          //Pedestal sigma
        THOLD,          //Threshold
        PEAKT,          //Peaking time
        WIDTH,          //Bipolar shape width
        SAT1,           //Saturation parameter 1   
        SAT2,           //Saturation parameter 2
        PEAKC,          //Peak charge
        SAMPLE_TIME_RATIO,       //Boolean info
        OCCUPANCY,      //integer occupancy
        STAT,           //Status Flag. Partial status flag bits must be higher than this
        HOT_STAT,       //Hot channel 
        DEAD_STAT,      //Dead channel
        PULSE_STAT,     //pulser problem
        CALIB_STAT,     //calibration problem
        SAT_STAT        //sturation problem
      };
      CscCondParType();
      CscCondParType(const unsigned int & t);
      CscCondParType(const CscCondParType & t);

      CscCondParType& operator=(unsigned int t);
      CscCondParType& operator=(const CscCondParType& t);

      operator unsigned int() const;

    private:
      int m_type;

  };

  inline  CscCondParType::CscCondParType() : m_type(0) {}

  inline CscCondParType::CscCondParType(const unsigned int & t) {
    m_type = t;
  }

  inline CscCondParType::CscCondParType(const CscCondParType & t) {
    m_type = t;
  }

  inline CscCondParType & CscCondParType::operator=(unsigned int t) {
    m_type = t;
    return *this;
  }

  inline CscCondParType & CscCondParType::operator=(const CscCondParType& t) {
    m_type = t;
    return *this;
  }

  inline CscCondParType::operator unsigned int() const {
    return m_type;
  }

  inline std::ostream & operator<< (std::ostream& s , const CscCondParType& type) {
    switch(type) {

      case CscCondParType::GAIN : 
        return  s << "GAIN";
      case CscCondParType::PULSE_SLOPE :
        return  s << "PULSE_SLOPE";
      case CscCondParType::RUN_SLOPE :
        return  s << "RUN_SLOPE";
      case CscCondParType::PED :
        return  s << "PED";
      case CscCondParType::NOISE : 
        return  s << "NOISE";
      case CscCondParType::THOLD : 
        return  s << "THOLD";
      case CscCondParType::PEAKT :
        return  s << "PEAKT";
      case CscCondParType::WIDTH :
        return  s << "WIDTH";
      case CscCondParType::SAT1 :
        return  s << "SAT1";
      case CscCondParType::SAT2 :
        return  s << "SAT2";
      case CscCondParType::PEAKC : 
        return s << "PEAKC";
      case CscCondParType::SAMPLE_TIME_RATIO :
        return s << "SAMPLE_TIME_RATIO";
      case CscCondParType::OCCUPANCY : 
        return s << "OCCUPANCY";
      case CscCondParType::STAT :
        return s << "STAT";
      case CscCondParType::HOT_STAT :
        return s << "HOT_STAT";
      case CscCondParType::DEAD_STAT :
        return s << "DEAD_STAT";
      case CscCondParType::PULSE_STAT :
        return s << "PULSE_STAT";
      case CscCondParType::SAT_STAT :
        return s << "SAT_STAT";
      default:
        return s << "UNKNOWN";
    };
  }
}
#endif
