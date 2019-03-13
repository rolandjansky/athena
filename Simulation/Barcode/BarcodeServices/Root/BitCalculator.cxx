/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "BarcodeServices/BitCalculator.h"
#include <iostream>

ClassImp(Barcode::BitCalculator)


Barcode::BitCalculator::BitCalculator()
  : TObject()
  , m_nbitsparent(12)
  , m_nbitspid(10)
  , m_nbitshs(1)
  , m_nbitssimulator(3)
  , m_nbitsbcid( 8*sizeof(int) - m_nbitsparent - m_nbitspid - m_nbitshs - m_nbitssimulator )
  //  , m_nbitsparent(16)
  //  , m_nbitspid(6)
  //  , m_nbitshs(4)
  //  , m_nbitssimulator(0)
  //  , m_nbitsbcid(6)
  , m_parentOne(0)
  , m_pidOne(0)
  , m_hsOne(0)
  , m_simulatorOne(0)
  , m_bcidOne(0)
{
  for (int i=0; i<m_nbitsparent; ++i) {
    int j = 1<<i ;
    m_parentOne = (m_parentOne | j);
  }

  for (int i=m_nbitsparent; i<m_nbitsparent+m_nbitspid; ++i) {
    int j = 1<<i ;
    m_pidOne = (m_pidOne | j);
  }

  for (int i=m_nbitsparent+m_nbitspid; i<m_nbitsparent+m_nbitspid+m_nbitshs; ++i) {
    int j = 1<<i ;
    m_hsOne = (m_hsOne | j);
  }

  for (int i=m_nbitsparent+m_nbitspid+m_nbitshs; i<m_nbitsparent+m_nbitspid+m_nbitshs+m_nbitssimulator; ++i) {
    int j = 1<<i ;
    m_simulatorOne = (m_simulatorOne | j);
  }

  for (int i=m_nbitsparent+m_nbitspid+m_nbitshs+m_nbitssimulator; i<m_nbitsparent+m_nbitspid+m_nbitshs+m_nbitssimulator+m_nbitsbcid; ++i) {
    int j = 1<<i ;
    m_bcidOne = (m_bcidOne | j);
  }

}


int
Barcode::BitCalculator::GetParentID(const int& barcode) const
{
  return (barcode&m_parentOne)>>(0);
}


int
Barcode::BitCalculator::GetPIDofParent(const int& barcode) const
{
  return (barcode&m_pidOne)>>(m_nbitsparent);
}


int
Barcode::BitCalculator::GetHS(const int& barcode) const
{
  return (barcode&m_hsOne)>>(m_nbitsparent+m_nbitspid);
}


int
Barcode::BitCalculator::GetSimulator(const int& barcode) const
{
  return (barcode&m_simulatorOne)>>(m_nbitsparent+m_nbitspid+m_nbitshs);
}


int
Barcode::BitCalculator::GetBCID(const int& barcode) const
{
  // MB this works for both int and unsigned int
  int negTest = 1<<(8*sizeof(int)-1);
  bool isNeg(false);
  if ((barcode&negTest)!=0) { isNeg = true; }

  int bc(barcode);
  if (isNeg) { bc &= ~negTest; } // turn off last bit, for bit shifting

  bc = (bc&m_bcidOne)>>(m_nbitsparent+m_nbitspid+m_nbitshs+m_nbitssimulator);

  if (isNeg) { // turn on 'last, shifted hs bit' again
    //int lastBit = 1<<(m_nbitsbcid-1);
    //bc |= lastBit;
    bc = -bc;
  }

  return bc;

  //// MB: line for unsigned ints
  //return (barcode&m_bcidOne)>>(m_nbitsparent+m_nbitspid+m_nbitshs);
}


void
Barcode::BitCalculator::SetParentID(int& barcode, const int& parentID) const
{
  if (parentID<0) {
    std::cerr <<  "ERROR: parent-ID is negative: " << parentID << " Only accepting positive number." << std::endl;
    return;
  }

  int shifted = parentID << 0;

  if (parentID>=(1<<m_nbitsparent)) {
    std::cerr <<  "ERROR: parent-ID too big for barcode: " << parentID << " (limit = " << (1<<m_nbitsparent)-1 << "). Reset to zero." << std::endl;
    shifted = 0;
  }

  barcode = barcode & ~m_parentOne ; // reset parent id
  barcode = barcode | shifted ;    // set new parent id
}


void
Barcode::BitCalculator::SetPIDofParent(int& barcode, const int& parentPID) const
{
  if (parentPID<0) {
    std::cerr <<  "ERROR: PID is negative: " << parentPID << " Only accepting positive number." << std::endl;
    return;
  }

  int shifted = parentPID << (m_nbitsparent);

  if (parentPID>=(1<<m_nbitspid)) {
    std::cerr <<  "ERROR: parentPID too big for barcode: " << parentPID << " (limit = " << (1<<m_nbitspid)-1 << "). Reset to zero." << std::endl;
    shifted = 0;
  }

  barcode = barcode & ~m_pidOne ; // reset parentPID
  barcode = barcode | shifted ;    // set new parentPID
}


void
Barcode::BitCalculator::SetHS(int& barcode, const int& hs) const
{
  if (hs<0) {
    std::cerr <<  "ERROR: hard scatter: " << hs << " Only accepting positive number." << std::endl;
    return;
  }

  int shifted = hs << (m_nbitsparent+m_nbitspid);

  if (hs>=(1<<m_nbitshs)) {
    std::cerr <<  "ERROR: HS too big for barcode: " << hs << " (limit = " << (1<<m_nbitshs)-1 << "). Reset to zero." << std::endl;
    shifted = 0;
  }

  barcode = barcode & ~m_hsOne ; // reset hs
  barcode = barcode | shifted ;  // set new hs
}


void
Barcode::BitCalculator::SetSimulator(int& barcode, const int& simulator) const
{
  int shifted = simulator << (m_nbitsparent+m_nbitspid+m_nbitshs);

  if (simulator>=(1<<m_nbitssimulator)) {
    std::cerr <<  "ERROR: Simulator too big for barcode: " << simulator << " (limit = " << (1<<m_nbitssimulator)-1 << "). Reset to zero." << std::endl;
    shifted = 0;
  }

  barcode = barcode & ~m_simulatorOne ;   // reset simulator
  barcode = barcode | shifted ;    // set new simulator
}


void
Barcode::BitCalculator::SetBCID(int& barcode, const int& bcid) const // also to set negative bcid
{
  bool isNeg = (bcid<0) ;
  int bcidpositive = (isNeg ? -bcid : bcid);

  int shifted = bcidpositive << (m_nbitsparent+m_nbitspid+m_nbitshs+m_nbitssimulator);
  int negbit  = ( isNeg ? (1 << (m_nbitsparent+m_nbitspid+m_nbitshs+m_nbitssimulator+m_nbitsbcid-1)) : 0 );
  shifted = shifted | negbit ;

  if (bcidpositive>=(1<<(m_nbitsbcid-1))) { // subtract one for negative bit
    std::cerr <<  "ERROR: bcid too big for barcode: " << bcid << " (absolute limit = " << (1<<(m_nbitsbcid-1))-1 << "). Reset to zero." << std::endl;
    shifted = 0;
  }

  barcode = barcode & ~m_bcidOne ;  // reset bcid
  barcode = barcode | shifted ;   // set new bcid
}


void
Barcode::BitCalculator::PrintBits(const int& barcode) const
{
  for (int i = sizeof(int)*8 -1 ; i>=0; --i) {
    int j = 1<<i;
    int pos = barcode & j ;
    std::cout << (pos!=0 ? "1" : "0") << "";
  }
  std::cout << std::endl;
}


void
Barcode::BitCalculator::Summary(const int& barcode) const
{
  std::cout << "Barcode:      " << barcode << "\n"
            << "Bits:         ";   PrintBits(barcode);
  std::cout << "Parent ID:    " << GetParentID(barcode) << "\n"
            << "Parent PID:   " << GetPIDofParent(barcode) << "\n"
            << "Hard scatter: " << GetHS(barcode) << "\n"
            << "Simulator:    " << GetSimulator(barcode) << "\n"
            << "BCID:         " << GetBCID(barcode) << std::endl;
}
