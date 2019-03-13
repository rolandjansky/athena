/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// SiChargedDiode.h // Header file for class SiChargedDiode
/////////////////////////////////////////////////////////////////// 
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////// 
// Class which contains the sum and the composition of all bare 
//charges (SiTotalCharge) deposited in one SiDiode 
/////////////////////////////////////////////////////////////////// 
// Version 2.1 09/06/2001 David Calvet 
// Revisited 04-03-03 Davide Costanzo
//  added a int flag as a private data member to store the noise, 
//  disconnected, bad_tot information. The relative bunch number is 
//  also stored in this word. 
//  the word will is meant to be copied as it is in the SDO
///////////////////////////////////////////////////////////////////

#ifndef SIDIGITIZATION_SICHARGEDDIODE_H 
#define SIDIGITIZATION_SICHARGEDDIODE_H

// Data member classes
#include "InDetSimEvent/SiTotalCharge.h"
#include "InDetReadoutGeometry/SiCellId.h"
#include "InDetReadoutGeometry/SiReadoutCellId.h"

class SiHelper; // used to set the flag word
//class SiChargedDiode;

class SiChargedDiode {

    friend class SiHelper;

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  // Constructor with parameters:
    SiChargedDiode(const SiTotalCharge::alloc_t& alloc,
                   const InDetDD::SiCellId & diode, const InDetDD::SiReadoutCellId & roCell,  int flagword=0, SiChargedDiode* nextInCluster=NULL);

  ///////////////////////////////////////////////////////////////////
  // Const methods:
  ///////////////////////////////////////////////////////////////////

  // Diode which contains this charge:
  const InDetDD::SiCellId & diode() const;

  // Readout cell associated to diode 
  // (usually the same id as the diode except for ganged pixels):
  const InDetDD::SiReadoutCellId & getReadoutCell();

  // total charge and its composition:
  const SiTotalCharge & totalCharge() const;

  // total deposited charge:
  double charge() const;
  
  // flag, disconnected etc.
  int flag() const;
  
  //neighbouring strip for navigation 
  SiChargedDiode * nextInCluster();
  ///////////////////////////////////////////////////////////////////
  // Non-const methods:
  ///////////////////////////////////////////////////////////////////

  // add another charge:
  void add(const SiCharge &charge);
  // add a total charge
  void add(const SiTotalCharge &totcharge);
  //add a neighbouring strip for navigation 
  void setNextInCluster(SiChargedDiode* nextInCluster);

  ///////////////////////////////////////////////////////////////////
  // Private methods:
  ///////////////////////////////////////////////////////////////////
private:

  SiChargedDiode();

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
private:
  InDetDD::SiCellId m_diode; // SiDiode which contains this charge
                          // the pointed SiDiode is owned by the SiChargedDiode
  SiTotalCharge m_totalCharge; // total charge and its composition
  InDetDD::SiReadoutCellId m_readoutCell; //Readout cell associated to this diode
  int m_word;   // a flag for noise etc etc as in InDetSimData
  SiChargedDiode * m_nextInCluster; //the next strip to navigate to - allows traversing clusters since the SiChargedDiodeCollection is not guaranteed to be contiguous
};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////
inline const InDetDD::SiCellId & SiChargedDiode::diode() const 
{
  return m_diode;
}

inline int SiChargedDiode::flag() const 
{
  return m_word;
}

inline const SiTotalCharge &SiChargedDiode::totalCharge() const
{
  return m_totalCharge;
}
inline const InDetDD::SiReadoutCellId & SiChargedDiode::getReadoutCell() {
  return m_readoutCell;
}

inline double SiChargedDiode::charge() const
{
  return m_totalCharge.charge();
}

inline SiChargedDiode * SiChargedDiode::nextInCluster()
{
  return m_nextInCluster;
}

inline void SiChargedDiode::add(const SiCharge &charge)
{
  m_totalCharge.add(charge);
}

inline void SiChargedDiode::add(const SiTotalCharge &totcharge)
{
  m_totalCharge.add(totcharge);
}

inline void SiChargedDiode::setNextInCluster(SiChargedDiode* nextInCluster)
{
   m_nextInCluster = nextInCluster;
}
///////////////////////////////////////////////////////////////////
// Input/Output stream functions:
///////////////////////////////////////////////////////////////////
std::ostream &operator<<(std::ostream &out,const SiChargedDiode &chargedDiode);

#endif // SIDIGITIZATION_SICHARGEDDIODE_H

