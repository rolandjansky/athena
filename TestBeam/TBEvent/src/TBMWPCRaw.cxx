/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TBEvent/TBBeamDetector.h"
#include "TBEvent/TBMWPCRaw.h"


#include <vector>


//////////////////////////////////
// Constructors and Destructors //
//////////////////////////////////

TBMWPCRaw::TBMWPCRaw()
  : TBBeamDetector(),
    m_isX(false),
    m_overflowSetFlag(false)
{
  // prepare stores
  //  this->setUpStores();
}

TBMWPCRaw::TBMWPCRaw(const std::string& thisBPCName)
  : TBBeamDetector(thisBPCName),
    m_isX(false)
{
  // prepare stores
  this->reset();
}

TBMWPCRaw::~TBMWPCRaw()
{ }

///////////////////////
// Data Manipulation //
///////////////////////

void TBMWPCRaw::reset()
{
  // reset signals and overflows
  //  m_signals.resize(m_signals.size(),0);
  m_overflowType.resize(m_overflowType.size(),false);
  // global flags
  m_overflowSetFlag = false;
  
  m_cwireno.clear();
  m_nwires.clear();
}

// void TBMWPCRaw::setSignal(TBMWPCRaw::source_type theSignalType,
// 			 TBMWPCRaw::signal_type  theSignal)
// {
//   if ( theSignalType < m_signals.size() )
//     {
//       m_signals[theSignalType] = theSignal;
//     }
// }

// overflow handling for each signal type
// void TBMWPCRaw::setOverflow(TBMWPCRaw::source_type theSignalType)
// {
//   this->setOverflow(theSignalType,true);
// }

// void TBMWPCRaw::resetOverflow(TBMWPCRaw::source_type theSignalType)
// {
//   this->setOverflow(theSignalType,false);
// }

// void TBMWPCRaw::setOverflow(TBMWPCRaw::source_type theSignalType,
// 			   bool overflow)
// {
//   if ( theSignalType < m_signals.size() )
//     {
//       m_overflowType[theSignalType] = overflow;
//     }
//   if ( ! m_overflowSetFlag ) m_overflow = ( m_overflow | overflow );
// }

// global overflow handling
void TBMWPCRaw::setOverflow()
{
  this->setOverflow(true);
}

void TBMWPCRaw::resetOverflow()
{
  this->setOverflow(false);
}

void TBMWPCRaw::setOverflow(bool overflow)
{
  m_overflowSetFlag = true;
  m_overflow        = overflow;
}

/////////////////
// Data Access //
/////////////////

// TBMWPCRaw::signal_type 
// TBMWPCRaw::getSignal(TBMWPCRaw::source_type theSignalType) const
// {
//   return ( theSignalType <= TBMWPCRaw::tdcUp )
//     ? m_signals[theSignalType]
//     : 0;
// }

// bool TBMWPCRaw::isOverflow(TBMWPCRaw::source_type theSignalType) const
// {
//   return ( theSignalType <= TBMWPCRaw::tdcUp )
//     ? m_overflowType[theSignalType]
//     : false;
// }

//////////////
// Internal //
//////////////

// void TBMWPCRaw::setUpStores()
// {
//   // find size
//   unsigned int theSize = (unsigned int) TBMWPCRaw::tdcUp;
//   theSize++;
//   // preset
//   m_signals.resize(theSize);
//   m_overflowType.resize(theSize);
//   this->reset();
// }
