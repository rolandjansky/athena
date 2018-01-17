/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBEVENT_TBBPCRAW_H
#define TBEVENT_TBBPCRAW_H
///////////////////////////////////////////////////////////////////////////////
/// \brief raw data class for BPC measurement
///
/// The TBBPCRaw class provides the object characteristics for the raw data
/// from the BPCs, i.e. four time and two pulse height measurements. 
///
/// \author Peter Loch <loch@physics.arizona.edu>
/// \author Pierre-Antoine Delsart <delsart@lps.umontreal.ca>
///
/// \date  May 20, 2004 - first implementation of new design
///
//////////////////////////////////////////////////////////////////////

#include "TBEvent/TBBeamDetector.h"

#include "TBEvent/TBTDCRawCont.h"
#include "TBEvent/TBADCRawCont.h"

#include "AthLinks/ElementLink.h"

#include <string>
#include <vector>
#include <list>



class TBBPCRaw : public TBBeamDetector
{
 public:

  typedef TBTDCRaw::signal_type                         tdc_signal_type;
  typedef std::vector< ElementLink<TBTDCRawCont> > tdc_store_type; 
  typedef tdc_store_type::const_iterator             tdc_store_iterator;

  typedef TBADCRaw::signal_type                         adc_signal_type;
  typedef std::vector< ElementLink<TBADCRawCont> > adc_store_type;
  typedef adc_store_type::const_iterator             adc_store_iterator;

  enum SignalSource { tdcLeft       = 0,
		     tdcRight      = 1,
		     tdcUp         = 2,
		     tdcDown       = 3,
		     adcHorizontal = 4,
		     adcVertical   = 5,
		     unknown       = 6 };

  /////////////////////////////////
  // Constructors and Destructor //
  /////////////////////////////////

  TBBPCRaw();  

  /// \brief standard constructor 

  TBBPCRaw(const std::string& thisBPCName) : TBBeamDetector(thisBPCName) 
  {}

  TBBPCRaw(const std::string& thisBPCName,
	   const TBTDCRawCont* theTDCCont,
	   const std::list<const TBTDCRaw*>& theTDCs,
	   const TBADCRawCont* theADCCont,
	   const std::list<const TBADCRaw*>& theADCs);
	
  ~TBBPCRaw();

  /////////////////
  // Data Access //
  /////////////////
  void setSignals(const TBTDCRawCont* theTDCCont,
		  const std::list<const TBTDCRaw*>& theTDCs,
		  const TBADCRawCont* theADCCont,
		  const std::list<const TBADCRaw*>& theADCs);

  tdc_signal_type getTDCLeft()       const;
  tdc_signal_type getTDCRight()      const;
  tdc_signal_type getTDCUp()         const;
  tdc_signal_type getTDCDown()       const;
  tdc_signal_type getTDCSignal(SignalSource theSource) const;

  adc_signal_type getADCHorizontal() const;
  adc_signal_type getADCVertical()   const;
  adc_signal_type getADCSignal(SignalSource theSource) const;

  // access overflow
  virtual bool isOverflow() const;
  virtual bool isOverflow(SignalSource theSource) const;

  // iterator on time measurements
  tdc_store_iterator tdc_begin() const;
  tdc_store_iterator tdc_end()   const;
  SignalSource getSignalSource(tdc_store_iterator& theTDCIterator) const;

  // iterator on amplitude mesaurement
  adc_store_iterator adc_begin() const;
  adc_store_iterator adc_end()   const;
  SignalSource getSignalSource(adc_store_iterator& theADCIterator) const;

 private:

  //////////////////
  // Private Data //
  //////////////////

  tdc_store_type m_tdcSignals;
  adc_store_type m_adcSignals;

  ///////////////////////
  // Private Functions //
  ///////////////////////
    
/*   template<typename T> */
/*     size_t findIndex(const T& theStore,T::const_iterator& theIterator) */
/*   { */
/*     T::const_iterator firstInStore = theStore.begin(); */
/*     T::const_iterator lastInStore  = theStore.end(); */
/*     size_t theIndex = 0; */
/*     while ( firstInStore != theIterator && firstInStore != lastInStore ) */
/*       { */
/* 	theIndex++; */
/* 	firstInStore++; */
/*       } */
/*     if ( firstInStore == lastInStore ) theIndex = size_t(-1); */
/*     return theIndex; */
/*   } */


  size_t findIndex( const tdc_store_type& theStore,tdc_store_iterator& theIterator) const
  {
    tdc_store_type::const_iterator firstInStore = theStore.begin();
    tdc_store_type::const_iterator lastInStore  = theStore.end();
    size_t theIndex = 0;
    while ( firstInStore != theIterator && firstInStore != lastInStore )
      {
	theIndex++;
	firstInStore++;
      }
    if ( firstInStore == lastInStore ) theIndex = size_t(-1);
    return theIndex;
  }
  size_t findIndex(const adc_store_type& theStore,adc_store_type::const_iterator& theIterator) const
  {
    adc_store_type::const_iterator firstInStore = theStore.begin();
    adc_store_type::const_iterator lastInStore  = theStore.end();
    size_t theIndex = 0;
    while ( firstInStore != theIterator && firstInStore != lastInStore )
      {
	theIndex++;
	firstInStore++;
      }
    if ( firstInStore == lastInStore ) theIndex = size_t(-1);
    return theIndex;
  }

};

//////////////////////
// Inline Functions //
//////////////////////

// global overflow
inline bool TBBPCRaw::isOverflow() const
{
  return m_overflow;
}

// specific signals
inline TBBPCRaw::tdc_signal_type TBBPCRaw::getTDCLeft() const
{
  return this->getTDCSignal(TBBPCRaw::tdcLeft);
}

inline TBBPCRaw::tdc_signal_type TBBPCRaw::getTDCRight() const
{
  return this->getTDCSignal(TBBPCRaw::tdcRight);
}

inline TBBPCRaw::tdc_signal_type TBBPCRaw::getTDCUp() const
{
  return this->getTDCSignal(TBBPCRaw::tdcUp);
}

inline TBBPCRaw::tdc_signal_type TBBPCRaw::getTDCDown() const
{
  return this->getTDCSignal(TBBPCRaw::tdcDown);
}

inline TBBPCRaw::tdc_signal_type TBBPCRaw::getADCHorizontal() const
{
  return this->getADCSignal(TBBPCRaw::adcHorizontal);
}

inline TBBPCRaw::tdc_signal_type TBBPCRaw::getADCVertical() const
{
  return this->getADCSignal(TBBPCRaw::adcVertical);
}

// iterators
inline TBBPCRaw::tdc_store_iterator TBBPCRaw::tdc_begin() const
{
  return m_tdcSignals.begin();
}
inline TBBPCRaw::tdc_store_iterator TBBPCRaw::tdc_end() const
{
  return m_tdcSignals.end();
}
inline TBBPCRaw::adc_store_iterator TBBPCRaw::adc_begin() const
{
  return m_adcSignals.begin();
}
inline TBBPCRaw::adc_store_iterator TBBPCRaw::adc_end() const
{
  return m_adcSignals.end();
}
#endif
