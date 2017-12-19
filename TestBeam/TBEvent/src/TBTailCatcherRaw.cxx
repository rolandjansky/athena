/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "AthContainers/DataVector.h"

#include "TBEvent/TBTailCatcherRaw.h"
#include "TBEvent/TBScintillator.h"

#include <vector>

TBTailCatcherRaw::TBTailCatcherRaw() : TBBeamDetector(), 
				 DataVector< TBScintillatorRaw >() 
{ }

TBTailCatcherRaw::TBTailCatcherRaw(const std::string& thisTailCatcherName,
			     bool overflow,
			     const 
			     std::vector< TBScintillatorRaw* >&
			     theScints)
  : TBBeamDetector(thisTailCatcherName,overflow)
{
  // store scintillator signals internally
  m_signals.resize(theScints.size(),0);
  for ( unsigned int i=0; i<theScints.size(); i++ )
    {
      m_signals[i] = theScints[i]->getADCSignal();
      m_overflow   = m_overflow || theScints[i]->isOverflow();
      push_back(theScints[i]);
    }
}

TBTailCatcherRaw::~TBTailCatcherRaw() 
{ }

void
TBTailCatcherRaw::addScintillator(TBScintillatorRaw* thisScint)
{
  m_overflow = m_overflow || thisScint->isOverflow();
  this->push_back(thisScint);
}

const TBScintillatorRaw* 
TBTailCatcherRaw::getScintillator(std::string thisScintName) const
{
  DataVector< TBScintillatorRaw >::const_iterator first = this->begin();
  while ( first != this->end() && (*first)->getDetectorName() != thisScintName )
    {
      first++;
    }
  return ( first != this->end() )
    ? *first
    : 0;
}

const std::vector<int>& TBTailCatcherRaw::getSignals() const
{
  return m_signals;
}

int
TBTailCatcherRaw::getSignal(std::string thisScintName) const
{
  return ( this->findIndex(thisScintName) < m_signals.size() )
    ? m_signals[this->findIndex(thisScintName)]
    : 0;
}

int
TBTailCatcherRaw::getSignal(const TBScintillatorRaw* thisScint) const
{
  return ( this->findIndex(thisScint->getDetectorName()) < m_signals.size() )
    ? m_signals[this->findIndex(thisScint->getDetectorName())]
    : 0;
}

unsigned int
TBTailCatcherRaw::findIndex(std::string thisScintName) const
{
  DataVector< TBScintillatorRaw >::const_iterator first = this->begin();
  unsigned int theIndex = 0;
  while( first != this->end() && thisScintName != (*first)->getDetectorName() )
    {
      first++;
      theIndex++;
    }
  return theIndex;
}
