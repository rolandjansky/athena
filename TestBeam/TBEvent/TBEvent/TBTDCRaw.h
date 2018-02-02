/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBEVENT_TBTDCRAW_H
#define TBEVENT_TBTDCRAW_H


#include "TBEvent/TBBeamDetector.h"
#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"

#include <string>

/** @class TBTDCRaw TBTDCRaw.h "TBEvent/TBTDCRaw.h"
Data object holding tdc measurement.
@author Malachi Schram
*/

class TBTDCRaw : public TBBeamDetector
{ 
 public:

  typedef  unsigned int signal_type;

  TBTDCRaw()
    : m_tdc(0),
      m_underThreshold(false)
    {;}
  TBTDCRaw(const std::string& tdcID, bool overflow,
	signal_type theTDC, bool theUnderThreshold);
  ~TBTDCRaw();

  //Setter//
  void setTDC(signal_type theTDC)
    { m_tdc = theTDC; }
  
  void setUnderThreshold(bool theUnderThreshold)
    {  m_underThreshold = theUnderThreshold; }
  
  // Getter //    
  signal_type getTDC() const {return m_tdc;}
  signal_type isUnderThreshold() const {return m_underThreshold;}


 private:
  
  signal_type m_tdc; 
  bool m_underThreshold;
  
};
CLASS_DEF( TBTDCRaw , 170957041 , 1 )
#endif
