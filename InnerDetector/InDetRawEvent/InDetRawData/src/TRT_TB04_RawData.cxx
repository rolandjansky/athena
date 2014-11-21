/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_TB04_RawData.cxx
//   Implementation file for class TRT_TB04_RawData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 13/08/2002 Veronique Boisvert
// Implementation provided by A. Zalite, February 2003
///////////////////////////////////////////////////////////////////

#include <new>
#include "InDetRawData/TRT_TB04_RawData.h"
#include "InDetRawData/TRT_RDORawData.h"

// default constructor
TRT_TB04_RawData::TRT_TB04_RawData() :
  TRT_RDORawData(Identifier(), 0) //call base-class constructor
{
	m_timeword = 0;
}

// Constructor with parameters:
TRT_TB04_RawData::TRT_TB04_RawData(const Identifier rdoId,
                           const unsigned int word) :
  TRT_RDORawData( rdoId, word) //call base-class constructor
{
	m_timeword = 0;
}

// Constructor with parameters:
// 
TRT_TB04_RawData::TRT_TB04_RawData(const Identifier rdoId,
				   const unsigned int word, 
				   const unsigned int timeword) :
  TRT_RDORawData( rdoId, word) //call base-class constructor
{
   if ( timeword > 25 )          // Fix for bad trigger phase info
      m_timeword = 0;
   else
      m_timeword = timeword;
}


// Destructor:
//should I be destructing something here?
TRT_TB04_RawData::~TRT_TB04_RawData()
{}

  // High level threshold:
bool TRT_TB04_RawData::highLevel() const
{
  if (m_word & 0x04020100)
    return true;
  return false;
}

  // Time over threshold in ns for valid digits; zero otherwise:
double TRT_TB04_RawData::timeOverThreshold() const
{
  double binWidth = 3.125;

  unsigned mask = 0x02000000;

  int i;
  for (i = 0; i < 24; ++i)
  {
    if (m_word & mask)
      break;

    mask >>= 1;
    if (i == 7 || i == 15)
      mask >>= 1;
  }

  //if (i == 0 || i > 16)
  //  return 0.;

  mask = 0x00000001;

  int j;
  for (j = 0; j < 24; ++j)
  {
    if (m_word & mask)
    break;

    mask <<= 1;
    if (j == 7 || j == 15)
      mask <<= 1;
  }

  double time = (double) (24 - i - j) * binWidth;
  return time;
}

/*
 * drift time in bin
 * This is the Wrong Way to do this.  We lose half our precision in ns
 * because we are returning the bin of the leading edge rather than its
 * time in ns.  The precision of the trigger phase is twice that of the
 * time digitization, but we are forced to add time in 3.125ns steps.
 */
int TRT_TB04_RawData::driftTimeBin() const 
{ 
  unsigned mask = 0x02000000;
  int    TimeAdjBin;

  int i;
  for (i = 0; i < 24; ++i) 
  { 
    if (m_word & mask) 
      break; 
 
    mask >>= 1;
    if (i == 7 || i == 15) 
      mask >>= 1;
  } 

  TimeAdjBin = m_timeword / 2;     // 0.5 bin for each PLL step.

  return i + TimeAdjBin; 
} 


//TRT_TB04_RawData *TRT_TB04_RawData::newObject(const Identifier rdoId, const unsigned int word)
//{
//  TRT_TB04_RawData *p_rdo=0;

  // create a new object and check
//  try {
//    p_rdo=new TRT_TB04_RawData(rdoId, word);
//  } catch (...) {
//    p_rdo=0;
//  }
//  return p_rdo;
//}

