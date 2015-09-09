/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @Project: AnalysisUtils
 *
 * @class CombinatoricsOdometer
 *
 * @author Steven Sekula <Stephen.Jacob.Sekula@cernSPAMNOT.ch>
 * @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>
 *
 * @date January 2010
 *
 * @brief Helper class for particle combinatorics of several lists.
 *
 * Based completely on:
 * NAME:     EVComboHelper.h
 * PACKAGE:  offline/PhysicsAnalysis/EventViewBuilder/EventViewCombiners/
 * AUTHORS:  Akira Shibata (ashibata@cern.ch)
 * CREATED:  Feb 2006
********************************************************************/

#include "PATCore/CombinatoricsOdometer.h"

#if !defined(ROOTCORE) || defined(ROOTCORE_PACKAGE_AthContainers)

//////////////////////FlexDigit
FlexDigit::FlexDigit( int digitMax, FlexDigit* nextDigit )
{
  m_digit=0; 
  m_digitMax=digitMax-1; 
  m_nextDigit=nextDigit;
}


FlexDigit FlexDigit::operator++( int )
{
  if(this->isAtMax())
    {  
      this->setDigit( 0 );
      if(!this->isFirstDigit()) (*(this->next()))++;
    }
  else
    {
      this->setDigit( this->digit()+1 );
    }
  return *this;
}


//////////////////////FlexDigits
FlexDigits::FlexDigits( std::vector<std::string>& labels, std::map<std::string, int>& digitMax ) :
  DataVector<FlexDigit>(SG::OWN_ELEMENTS)
{
  m_started=false; 
  this->reserve(labels.size());
  for( std::vector<std::string>::const_iterator label_itr=labels.begin();
       label_itr !=  labels.end();
       label_itr++ )
    {
      FlexDigit* temp = new FlexDigit(digitMax[*label_itr]);
      this->push_back(temp);
    }
}


FlexDigits::FlexDigits( int numDigits, int digitMax ) :
  DataVector<FlexDigit>(SG::OWN_ELEMENTS)
{
  m_started=false; 
  this->reserve(numDigits);
  for( int i=0; i<numDigits; i++ )
    {
      FlexDigit* temp = new FlexDigit(digitMax);
      this->push_back(temp);
    }
}


FlexDigits::~FlexDigits()
{
}


//overloaded vector method, need to tell each digit which one
//is next to it
void FlexDigits::push_back( FlexDigit* aDigit )
{
  if(this->size())
    {
      aDigit->setNext(this->back());
    }
  this->DataVector<FlexDigit>::push_back(aDigit);
}

//just increment by one, no uniqueness check etc (overloaded in OdoMeter)
bool FlexDigits::increment()
{
  if(m_started && this->isZero())
    {
      return false;
    }
  else
    {
      m_started = true;
    }
  (*this)++;
  return true;
}


//for debugging
void FlexDigits::print()
{
  std::cout<<"current State of digits: ";
  for( FlexDigits::iterator digit_itr=this->begin();
       digit_itr!= this->end();
       digit_itr++ )
    {
      std::cout<<"|"<<(*digit_itr)->digit()<<"|";
    }
  std::cout<<std::endl;
}


//if the whole digits are 0
bool FlexDigits::isZero()
{
  bool isZero=true;
  for( FlexDigits::iterator digit_itr=this->begin();
       digit_itr != this->end();
       digit_itr++ )
    {
      isZero = isZero && !(*digit_itr)->digit();
    } 
  return isZero;
}


//only ++ operaor defined (only thing needed for now)
FlexDigits& FlexDigits::operator++( int )
{
  if( this->size() )
    {
      (*(this->back()))++;
    }
  return *this; 
}


//////////////////////OdoMeter
//somewhat specific constructor for label and number of paritlces 
//in each container with the label
OdoMeter::OdoMeter( std::vector<std::string>& labels, std::map<std::string, int>& digitMax )
  : FlexDigits( labels, digitMax ),
    m_pairMeter(0)
{ 
  m_onlyUnique = true;      
  m_onlySingleEntry = true; 
  m_numLabels = labels.size();
  int counter = 0;
  for( ;counter < m_numLabels; counter++ )
    { 
      m_digitAssoc[labels[counter]].insert(counter);
      m_labels.insert(labels[counter]);
    } 
}


//this constructor is for the case when you want to combine
//M objects (numDigits) from one single list of n (digitMax)
OdoMeter::OdoMeter( int numDigits, int digitMax )
  : FlexDigits( numDigits, digitMax ),
    m_pairMeter(0)
{ 
  m_onlyUnique = true;      
  m_onlySingleEntry = true; 
  m_numLabels = numDigits;
  int counter = 0;
  for( ;counter < m_numLabels; counter++ )
    { 
      m_digitAssoc["dummy"].insert(counter);
      m_labels.insert("dummy");
    }
}


OdoMeter::~OdoMeter()
{
  delete m_pairMeter;
  m_pairMeter = NULL;
}



//this function make sure that there's no double counting, should be used in combination
//with hasOnlySingleEntry as isUnique will pass the ones with same entries from one container
bool OdoMeter::isUnique( bool doCheck )
{
  if(!doCheck)
    {
      return true;
    }
  for( std::set<std::string>::const_iterator label_itr = m_labels.begin();
       label_itr != m_labels.end();
       label_itr++ )
    {
      int leftDigit = -1;
      for( std::set<int>::const_iterator digit_itr = m_digitAssoc[*label_itr].begin();
           digit_itr!=m_digitAssoc[*label_itr].end();
           digit_itr++ ) 
        {
          if(leftDigit <= (*this)[*digit_itr]->digit())
            {
              leftDigit=(*this)[*digit_itr]->digit();  
            }
          else
            {
              return false;
            }
        }
    } 
  return true;
}


//this function make sure that two same entries from single container do not exist
//the method uses the notion of std::set which does not allow two same entries.
bool OdoMeter::hasOnlySingleEntry( bool doCheck )
{
  if(!doCheck)
    {
      return true;
    }
  for( std::set<std::string>::const_iterator label_itr = m_labels.begin();
       label_itr != m_labels.end();
       label_itr++ )
    {
      std::set<int> digitsForLabel;
      for( std::set<int>::const_iterator digit_itr = m_digitAssoc[*label_itr].begin();
           digit_itr!=m_digitAssoc[*label_itr].end();
           digit_itr++ )
        {
          digitsForLabel.insert((*this)[*digit_itr]->digit()); 
        }
      if (digitsForLabel.size()!=m_digitAssoc[*label_itr].size())
        {
          return false;
        }
    }
  return true;
}


//increment the meter by one and check uniqueness. if not unique,
//call itself again (recursive)
bool OdoMeter::increment()
{
  if(this->hasStarted() && this->isZero())
    {
      return false;
    }
  else if(!this->hasStarted())
    {
      this->setStarted() ;
    }
  (*this)++;
  bool found = true;
  if(!isUnique(m_onlyUnique))
    {
      found=false;
    }
  if(! hasOnlySingleEntry(m_onlySingleEntry))
    {
      found=false;
    }
  if(!found)
    {
      return this->increment();
    }
  else
    { 
      delete m_pairMeter;
      m_pairMeter = new PairMeter(m_numLabels);
      return true;
    }
}


//output the digit in the form of vector<int>
std::vector<int> OdoMeter::getVector()
{
  std::vector<int> digitsVector;
  for( OdoMeter::const_iterator fdigit_itr = (*this).begin();
       fdigit_itr != (*this).end();
       fdigit_itr++)
    {
      digitsVector.push_back((*fdigit_itr)->digit());
    }
  return digitsVector;
}


//increment() for pairMeter
bool OdoMeter::nextPair()
{
  if(m_pairMeter)
    {
      if( m_pairMeter->increment())
        {
          return true; 
        }
      else
        {
          delete m_pairMeter;
          m_pairMeter = NULL;
        }
    }
  return false;
}


//get unique pairs out of the combined particles (useful for overlap check
std::pair<int,int> OdoMeter::getPair()
{
  if(m_pairMeter)
    {
      return std::make_pair( (*m_pairMeter)[0]->digit(), (*m_pairMeter)[1]->digit() );
    }
  else
    {
      return std::make_pair( 0,0 );
    }
}


//////////////////////PairMeter
PairMeter::PairMeter( int digitMax )
  :OdoMeter( 2, digitMax )
{ 
}


PairMeter::~PairMeter()
{
}


bool PairMeter::increment()
{
  if(this->hasStarted() && this->isZero())
    { 
      return false;
    }
  else if(!this->hasStarted())
    {
      this->setStarted() ;
    }
  (*this)++;
  bool found = true;
  if(!isUnique(true))
    {
      found=false;
    }
  if(!hasOnlySingleEntry(true))
    {
      found=false;
    }
  if(!found)
    {
      return this->increment();
    }
  else
    {
      return true;
    }
}

#endif
