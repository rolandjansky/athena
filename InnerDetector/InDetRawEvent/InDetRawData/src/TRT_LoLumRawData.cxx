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

  if ( (0 == LE) || (24 == LE) || (24 == TE) || (0 == TE) || (23 == LE) )
    {
      return 0;
    }

  double time = (double) (TE - LE + 1) * binWidth;
  
  return time;

}

// Position of first low-to-high bit transition
int TRT_LoLumRawData::driftTimeBin() const
{
  unsigned  mask = 0x02000000;
  unsigned  m_word_LE = m_word>>6;
  m_word_LE = m_word_LE<<6;
 
  mask >>=1;
  bool SawZero = false;
  int i;
  for(i=1;i<18;++i)
  { 
    if      (  (m_word_LE & mask) && SawZero) break;
    else if ( !(m_word_LE & mask) ) SawZero = true; 
    mask>>=1;
    if(i==7 || i==15) mask>>=1;
  }
  if(i==18) i=0;
  return i;
}

// Position of first low-to-high bit transition moving from the right
// or 24 if no transition is found
int TRT_LoLumRawData::trailingEdge() const
{
  unsigned mask = 0x00000001;
  unsigned mask_word = 0x0001fff0; // 11111111 1 11110000   
  unsigned mask_last_bit =0x10; //10000
  
  unsigned m_word_TE = m_word & mask_word;
  
  bool SawZero=true;
  bool SawZero1=false;
  bool SawZero2=false;
  bool SawUnit1=false;
  int i=0;
  int j=0;
  int k=0;
  
  if(m_word_TE & mask_last_bit) 
  {
  
	for (j = 0; j < 11; ++j)
	{
		mask_last_bit=mask_last_bit<<1;
		
		if(j==3) mask_last_bit=mask_last_bit<<1;
		
		if ( !(m_word_TE & mask_last_bit) )
		{
			SawZero2 = true;
			break;			
		}
	}
	
	if(SawZero2 == false) return 19;

	if(SawZero2 == true){
		for (k = j+1; k < 11; ++k)
		{
			mask_last_bit=mask_last_bit<<1;

			if(k==3) mask_last_bit=mask_last_bit<<1;

			if ( m_word_TE & mask_last_bit )
			{
				SawUnit1 = true;
				break;					
			}
		} 
	}
	
	if(SawUnit1 == false && SawZero2 == true) return 19;
	
  }
  
  //+++++++++++++++++++++++++++++++++++++
  
  for (i = 0; i < 24; ++i)
  {
  
	if(!(m_word_TE & mask) && i>3)
	{
	  SawZero1 = true;
	}
    if(SawZero1){  
		if ( (m_word_TE & mask) && SawZero )
			break;
		else if ( !(m_word_TE & mask) )
			SawZero = true;
    }
    mask <<= 1;
    if (i == 7 || i == 15)
      mask <<= 1;
  }
 
  if ( 24 == i )
    return i;

  return (23 - i);

}

