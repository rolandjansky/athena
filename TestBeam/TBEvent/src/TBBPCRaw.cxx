/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "AthLinks/ElementLink.h"

#include "TBEvent/TBBeamDetector.h"
#include "TBEvent/TBADCRaw.h"
#include "TBEvent/TBADCRawCont.h"
#include "TBEvent/TBTDCRaw.h"
#include "TBEvent/TBTDCRawCont.h"
#include "TBEvent/TBBPCRaw.h"

#include <string>
#include <list>

//////////////////////////////////
// Constructors and Destructors //
//////////////////////////////////

TBBPCRaw::TBBPCRaw() : TBBeamDetector()
{ }

TBBPCRaw::TBBPCRaw(const std::string&             thisBPCName,
		   const TBTDCRawCont*          theTDCCont,
		   const std::list<const TBTDCRaw*>& theTDCs,
		   const TBADCRawCont*          theADCCont,
		   const std::list<const TBADCRaw*>& theADCs)
  : TBBeamDetector(thisBPCName)
{
  setSignals(theTDCCont,theTDCs,theADCCont,theADCs);
}

void TBBPCRaw::setSignals(const TBTDCRawCont* theTDCCont,
			  const std::list<const TBTDCRaw*>& theTDCs,
			  const TBADCRawCont* theADCCont,
			  const std::list<const TBADCRaw*>& theADCs)
{
  // store TDC measurement
  std::list<const TBTDCRaw*>::const_iterator firstTDC = theTDCs.begin();
  std::list<const TBTDCRaw*>::const_iterator lastTDC  = theTDCs.end();
  for ( ; firstTDC != lastTDC; firstTDC++ )
    {
      ElementLink<TBTDCRawCont> theLink;
      //      const TBTDCRaw* tbtdc= (*firstTDC);
      theLink.toContainedElement(*theTDCCont,(*firstTDC));
      m_tdcSignals.push_back(theLink);
      m_overflow = m_overflow || (*firstTDC)->isOverflow();
    }

  // store ADC measurement
  std::list<const TBADCRaw*>::const_iterator firstADC = theADCs.begin();
  std::list<const TBADCRaw*>::const_iterator lastADC  = theADCs.end();
  for ( ; firstADC != lastADC; firstADC++ )
    {
      ElementLink<TBADCRawCont> theLink;
      theLink.toContainedElement(*theADCCont,(*firstADC));
      m_adcSignals.push_back(theLink);
      m_overflow = m_overflow || (*firstADC)->isOverflow();
    }
}

TBBPCRaw::~TBBPCRaw()
{ }

/////////////////
// Data Access //
/////////////////

// specific overflow condition
bool TBBPCRaw::isOverflow(SignalSource theSource) const
{
  bool test;
  switch ( theSource )
    {
    case TBBPCRaw::tdcLeft:
    case TBBPCRaw::tdcRight:
    case TBBPCRaw::tdcUp:
    case TBBPCRaw::tdcDown:
      {
	size_t theIndex = (size_t)theSource;
	const TBTDCRaw* theTDC = (*m_tdcSignals[theIndex]);
	test = theTDC->isOverflow();
      }
      break;
    case TBBPCRaw::adcHorizontal:
    case TBBPCRaw::adcVertical:
      {
	size_t theIndex = (size_t)theSource - (size_t)TBBPCRaw::adcHorizontal;
	const TBADCRaw* theADC = (*m_adcSignals[theIndex]);
	test = theADC->isOverflow();
      }
      break;
    default:
      test = false;
      break;
    }
  return test;
}

// TDC data 
TBBPCRaw::tdc_signal_type
TBBPCRaw::getTDCSignal(TBBPCRaw::SignalSource theSource) const
{
  tdc_signal_type theSignal;
  switch ( theSource )
    {
    case TBBPCRaw::tdcLeft:
    case TBBPCRaw::tdcRight:
    case TBBPCRaw::tdcUp:
    case TBBPCRaw::tdcDown:
      {
	size_t theIndex = (size_t)theSource;
	theSignal = (*(*m_tdcSignals[theIndex])).getTDC();
      }
      break;
    default:
      theSignal = 0;
      break;
    }
  return theSignal;
}

// ADC data
TBBPCRaw::adc_signal_type
TBBPCRaw::getADCSignal(TBBPCRaw::SignalSource theSource) const
{
  adc_signal_type theSignal;
  switch ( theSource )
    {
    case TBBPCRaw::adcHorizontal:
    case TBBPCRaw::adcVertical:
      {
	size_t theIndex = (size_t)theSource - (size_t)TBBPCRaw::adcHorizontal;
	theSignal = (*(*m_adcSignals[theIndex])).getADC();
      }
      break;
    default:
      theSignal = 0;
      break;
    }
  return theSignal;
}

// signal source from iterators
TBBPCRaw::SignalSource 
TBBPCRaw::getSignalSource(TBBPCRaw::tdc_store_iterator& theIterator) const
{
  size_t theIndex = this->findIndex(m_tdcSignals,theIterator);
  SignalSource theSource;
  switch ( theIndex )
    {
    case 0:
      theSource = TBBPCRaw::tdcLeft;
      break;
    case 1:
      theSource = TBBPCRaw::tdcRight;
      break;
    case 2:
      theSource = TBBPCRaw::tdcUp;
      break;
    case 3:
      theSource = TBBPCRaw::tdcDown;
      break;
    default:
      theSource = TBBPCRaw::unknown;
      break;
    }
  return theSource;
}

TBBPCRaw::SignalSource
TBBPCRaw::getSignalSource(adc_store_iterator& theIterator) const
{
  size_t theIndex = this->findIndex(m_adcSignals,theIterator);
  SignalSource theSource;
  switch ( theIndex )
    {
    case 0:
      theSource = TBBPCRaw::adcHorizontal;
      break;
    case 1 :
      theSource = TBBPCRaw::adcVertical;
      break;
    default:
      theSource = TBBPCRaw::unknown;
      break;
    }
  return theSource;
}
      
