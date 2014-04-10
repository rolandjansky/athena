/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Pixel1RawData.cxx
//   Implementation file for class Pixel1RawData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 13/08/2002 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#include <new>
#include "InDetRawData/Pixel1RawData.h"
#include "InDetRawData/PixelRDORawData.h"

// default constructor
Pixel1RawData::Pixel1RawData() :
  PixelRDORawData( Identifier(), 0) //call base-class constructor
{}

// Constructor with parameters:
// offline hashId of the readout element, 
// the word
Pixel1RawData::Pixel1RawData(const Identifier rdoId, const unsigned int word) :
  PixelRDORawData( rdoId, word) //call base-class constructor
{}

// Constructor with full parameter list: 
// offline compact id of the readout channel
// ToT, 
// BCO ID, 
// LVL1 accept, 
// ATLAS wide LVL1
// It calls base class constructor, after packing the data.
Pixel1RawData::Pixel1RawData(const Identifier rdoId, 
		const unsigned int ToT,
		const unsigned int BCID,
		const unsigned int LVL1ID,
		const unsigned int LVL1A
		  ) :
    PixelRDORawData(rdoId, 
		 ((ToT&0xFF)<<0)
		 +((BCID&0xFF)<<8)
		 +((LVL1ID&0xFF)<<16)
		 +((LVL1A&0xF)<<24) ) 
{}


// Destructor:
//should I be destructing something here?
Pixel1RawData::~Pixel1RawData()
{}




//Pixel1RawData *Pixel1RawData::newObject(const Identifier rdoId, const unsigned int word)
//{
//  Pixel1RawData *p_rdo=0;

  // create a new object and check
//  try {
//    p_rdo=new Pixel1RawData(rdoId, word);
//  } catch (...) {
//    p_rdo=0;
//  }
//  return p_rdo;
//}


