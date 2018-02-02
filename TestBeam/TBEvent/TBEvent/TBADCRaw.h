/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBEVENT_TBADCRAW_H
#define TBEVENT_TBADCRAW_H


#include "TBEvent/TBBeamDetector.h"
#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"

#include <string>

/** @class TBADCRaw TBADCRaw.h "TBEvent/TBADCRaw.h"
Data object holding adc measurement.
@author Malachi Schram
*/

class TBADCRaw : public TBBeamDetector
{ 
 public:

  typedef  unsigned int signal_type;

  TBADCRaw() : m_adc(0) {;}
  TBADCRaw(const std::string& adcID, bool overflow, signal_type theADC);
  ~TBADCRaw();

  //Setter//
  inline void setADC(signal_type theADC)
    { m_adc = theADC; }
  
  // Getter //    
  signal_type getADC() const {return m_adc;}
  
 private:
  
  signal_type m_adc;    
  
};
CLASS_DEF( TBADCRaw , 14764070 , 1 )

#endif
