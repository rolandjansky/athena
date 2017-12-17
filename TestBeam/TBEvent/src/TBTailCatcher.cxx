/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "AthContainers/DataVector.h"

#include "TBEvent/TBBeamDetector.h"
#include "TBEvent/TBTailCatcher.h"
#include "TBEvent/TBScintillator.h"

#include <vector>

TBTailCatcher::TBTailCatcher() : TBBeamDetector(), 
				 DataVector< TBScintillator >() 
{ }

TBTailCatcher::TBTailCatcher(const std::string& thisTailCatcherName,
			     bool overflow,
			     const 
			     std::vector< TBScintillator* >&
			     theScints)
  : TBBeamDetector(thisTailCatcherName,overflow)
{
  // store scintillator signals internally
  m_signals.resize(theScints.size(),0.);
  for ( unsigned int i=0; i<theScints.size(); i++ )
    {
      m_signals[i] = theScints[i]->getSignal();
      m_overflow   = m_overflow || theScints[i]->isOverflow();
      push_back(theScints[i]);
    }
}

TBTailCatcher::~TBTailCatcher() 
{ }

void
TBTailCatcher::addScintillator(TBScintillator* thisScint)
{
  m_overflow = m_overflow || thisScint->isOverflow();
  this->push_back(thisScint);
}

const TBScintillator* 
TBTailCatcher::getScintillator(std::string thisScintName) const
{
  DataVector< TBScintillator >::const_iterator first = this->begin();
  while ( first != this->end() && (*first)->getDetectorName() != thisScintName )
    {
      first++;
    }
  return ( first != this->end() )
    ? *first
    : 0;
}

const std::vector<double>& TBTailCatcher::getSignals() const
{
  return m_signals;
}

double
TBTailCatcher::getSignal(std::string thisScintName) const
{
  return ( this->findIndex(thisScintName) < m_signals.size() )
    ? m_signals[this->findIndex(thisScintName)]
    : 0.;
}

double
TBTailCatcher::getSignal(const TBScintillator* thisScint) const
{
  return ( this->findIndex(thisScint->getDetectorName()) < m_signals.size() )
    ? m_signals[this->findIndex(thisScint->getDetectorName())]
    : 0.;
}

unsigned int
TBTailCatcher::findIndex(std::string thisScintName) const
{
  DataVector< TBScintillator >::const_iterator first = this->begin();
  unsigned int theIndex = 0;
  while( first != this->end() && thisScintName != (*first)->getDetectorName() )
    {
      first++;
      theIndex++;
    }
  return theIndex;
}
