/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBEVENT_TBSCINTILLATORRaw_H
#define TBEVENT_TBSCINTILLATORRaw_H
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Templated class for represenetation of scintillator signals (pulse        //
// heights). Typical implementation:                                         //
//                                                                           //
// TBScintillator<unsigned short> theRawSignal;  // raw signal               //
// TBScintillator<double> theRecoSignal;         // reconstructed signal     //
//  Modified: Jan 29th 2004 by Malachi Schram                                //
///////////////////////////////////////////////////////////////////////////////


#include "TBEvent/TBBeamDetector.h"
#include "TBEvent/TBTDCRawCont.h"
#include "TBEvent/TBADCRawCont.h"
#include "AthLinks/ElementLink.h"
#include "AthenaKernel/CLASS_DEF.h"


class TBScintillatorRaw : public TBBeamDetector
{ 
 public:

  typedef TBADCRaw::signal_type    adc_signal_type;
  typedef TBTDCRaw::signal_type    tdc_signal_type;
  /////////////////////////////////
  // Constructors and Destructor //
  /////////////////////////////////

  TBScintillatorRaw() : TBBeamDetector()
  { }

  TBScintillatorRaw(const std::string& scintallatorName) : TBBeamDetector(scintallatorName)
  { }

  TBScintillatorRaw(const std::string& scintallatorName,
		 const TBTDCRawCont* tdcCont,TBTDCRaw* tbtdc,
		 const TBADCRawCont* adcCont,TBADCRaw* tbadc);

  virtual ~TBScintillatorRaw()
    { }

  ///////////////////////
  // Data Manipulation //
  ///////////////////////

    void setSignals(const TBTDCRawCont* tdcCont,TBTDCRaw* tbtdc,
		 const TBADCRawCont* adcCont,TBADCRaw* tbadc);

    tdc_signal_type getTDCSignal() const;
    bool isTDCOverflow() const;
    bool isTDCUnderThreshold() const;
    
    adc_signal_type getADCSignal() const;
    bool isADCOverflow() const;


  //////////
  // Data //
  //////////

 protected:

  ElementLink<TBTDCRawCont> m_tdclink;
  ElementLink<TBADCRawCont> m_adclink;


};

inline TBScintillatorRaw::tdc_signal_type TBScintillatorRaw::getTDCSignal() const
{
  return (*m_tdclink)->getTDC();

}


inline bool TBScintillatorRaw::isTDCOverflow() const
{
  return (*m_tdclink)->isOverflow();

}

inline bool TBScintillatorRaw::isTDCUnderThreshold() const
{
  return (*m_tdclink)->isUnderThreshold();

}

inline TBScintillatorRaw::adc_signal_type TBScintillatorRaw::getADCSignal() const
{
  return (*m_adclink)->getADC();
}

inline bool TBScintillatorRaw::isADCOverflow() const
{
  return (*m_adclink)->isOverflow();

}

 




CLASS_DEF( TBScintillatorRaw , 35513831 , 1 )


#endif
