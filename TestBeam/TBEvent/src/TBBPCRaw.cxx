/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
		   const std::vector<const TBTDCRaw*>& theTDCs,
		   const TBADCRawCont*          theADCCont,
		   const std::vector<const TBADCRaw*>& theADCs)
  : TBBeamDetector(thisBPCName)
{
  setSignals(theTDCCont,theTDCs,theADCCont,theADCs);
}

void TBBPCRaw::setSignals(const TBTDCRawCont* theTDCCont,
			  const std::vector<const TBTDCRaw*>& theTDCs,
			  const TBADCRawCont* theADCCont,
			  const std::vector<const TBADCRaw*>& theADCs)
{
  // store TDC measurement
  for (const TBTDCRaw* tdc : theTDCs)
    {
      ElementLink<TBTDCRawCont> theLink;
      //      const TBTDCRaw* tbtdc= tdc;
      theLink.toContainedElement(*theTDCCont,tdc);
      m_tdcSignals.push_back(theLink);
      m_overflow = m_overflow || tdc->isOverflow();
    }

  // store ADC measurement
  for (const TBADCRaw* adc : theADCs)
    {
      ElementLink<TBADCRawCont> theLink;
      theLink.toContainedElement(*theADCCont,adc);
      m_adcSignals.push_back(theLink);
      m_overflow = m_overflow || adc->isOverflow();
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
      
