/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelTB04RawData.h
//   Header file for class PixelTB04RawData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to implement RawData for Pixel, using method 1 of decoding the word
///////////////////////////////////////////////////////////////////
// Version 1.0 13/08/2002 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#ifndef INDETRAWDATA_PIXELTB04RAWDATA_H
#define INDETRAWDATA_PIXELTB04RAWDATA_H

// Base class
#include "InDetRawData/PixelRDORawData.h"

// Data members classes



class PixelTB04RawData : public PixelRDORawData{

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  // Constructor with parameters:
  // offline hashId of the readout element, 
  // the word
  PixelTB04RawData(const Identifier rdoId, const unsigned int word);
  // Constructor with BCID as well (commented out, since not implemented)
  // PixelTB04RawData(const Identifier rdoId, const unsigned int word, const unsigned int BCID);
  // Constructor with BCID and LVL1ID as well
  PixelTB04RawData(const Identifier rdoId, const unsigned int word, const unsigned int BCID, const unsigned int LVL1ID);
  // Constructor using ToT and BCID, LVL1ID and LVL1A as well
  PixelTB04RawData(const Identifier rdoId, const unsigned int ToT, const unsigned int BCID, const unsigned int LVL1ID, const unsigned int LVL1A);


  // Destructor:
  virtual ~PixelTB04RawData();

  ///////////////////////////////////////////////////////////////////
  // Virtual methods 
  ///////////////////////////////////////////////////////////////////




  virtual int getToT() const;    // Time over Threshold value 0-255
  virtual int getBCID() const;   // Beam Crossing ID
  virtual int getLVL1ID() const; // Level 1 accept, 0-15, used if reading 
                           // consecutive BCOs
  virtual int getLVL1A() const;  // ATLAS LVL1 0-255

  // Create a new PixelTB04RawData and return a pointer to it:
  //  static PixelTB04RawData *newObject(const Identifier rdoId, const unsigned int word);

public:
  // public default constructor needed for I/O, but should not be
  // called from an alg
  PixelTB04RawData();

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
private:
  unsigned int m_BCID;
  unsigned int m_LVL1ID;

};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////
// decode TOT information (taken from Calvet RawData class) 
inline int PixelTB04RawData::getToT() const
{
  return (m_word & 0xFF);
}

// decode BCID
inline int PixelTB04RawData::getBCID() const
{
  return (m_BCID);
}

// decode LVL1ID
inline int PixelTB04RawData::getLVL1ID() const
{
  return (m_LVL1ID);
}

// decode LVL1 accept 
inline int PixelTB04RawData::getLVL1A() const
{
  return ( (m_word>>8) & 0xF );
}

#endif // INDETRAWDATA_PIXELTB04RAWDATA_H
