/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



///////////////////////////////////////////////////////////////////
// TRT_LoLumRawData.cxx
//   Implementation file for class TRT_LoLumRawData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 13/08/2002 Veronique Boisvert
// Implementation provided by A. Zalite, February 2003
///////////////////////////////////////////////////////////////////

#include <new>
#include "InDetRawData/TRT_LoLumRawData.h"
#include "InDetRawData/TRT_RDORawData.h"

// default constructor
TRT_LoLumRawData::TRT_LoLumRawData() :
  TRT_RDORawData(Identifier(), 0) //call base-class constructor
{}

// Constructor with parameters:
TRT_LoLumRawData::TRT_LoLumRawData(const Identifier rdoId,
                           const unsigned int word) :
  TRT_RDORawData( rdoId, word) //call base-class constructor
{}

// Destructor:
//should I be destructing something here?
TRT_LoLumRawData::~TRT_LoLumRawData()
{}


  // Time over threshold in ns:
double TRT_LoLumRawData::timeOverThreshold() const
{
  double binWidth = 3.125;

  int LE = driftTimeBin( );
  int TE = trailingEdge( );

  if ( 24 == TE )
    TE--;
  else if ( 25 == TE )
    return 0;


  double time = (double) (TE - LE + 1) * binWidth;
  return time;
}


/*
 * Position of first high bit in bins.
 * Returns 0 - 23 if a high bit is found, 24 for all ones.
 */
int TRT_LoLumRawData::driftTimeBin() const 
{ 
  unsigned mask = 0x02000000;

  int i=0;
  while ( !(m_word & mask) && (i < 24) )
  {
     mask >>= 1;
    if (i == 7 || i == 15) 
      mask >>= 1;

    i++;
  } 

  if ( ! m_word )
    return 0;

  return i; 
} 

int TRT_LoLumRawData::trailingEdge() const
{
  if ( m_word & 0x00000001 )   // If last bit is high, return 24
    return 24;

  unsigned mask = 0x00000002;

  int i=1;
  while ( !(m_word & mask) && (i < 24) )
  {
    mask <<= 1;
    if (i == 7 || i == 15)
      mask <<= 1;
    
    i++;
  }
 
  if ( 24 == i )   // If we do not find a bit, return 25
    return 25;
  else
    return (23 - i);
}
