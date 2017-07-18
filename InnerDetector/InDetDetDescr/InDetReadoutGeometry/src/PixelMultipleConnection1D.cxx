/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelMultipleConnection1D.cxx 
//   Implementation file for class PixelMultipleConnection1D
///////////////////////////////////////////////////////////////////
// (c) ATLAS Pixel Detector software
///////////////////////////////////////////////////////////////////
// Version 1.1 05/04/2001 David Calvet
// 06/02/2008 Grant Gorfine
///////////////////////////////////////////////////////////////////

#include "InDetReadoutGeometry/PixelMultipleConnection1D.h"
#include <iostream>

namespace InDetDD {

// Constructor with parameters:
PixelMultipleConnection1D::PixelMultipleConnection1D(const int lowerIndex,
						     const std::vector<int> &connections) 
  : m_lowerDiode(0),
    m_upperDiode(-1),
    m_lowerReadout(0),
    m_upperReadout(-1)
{
  set(lowerIndex, connections);
}

PixelMultipleConnection1D::PixelMultipleConnection1D() 
  : m_lowerDiode(0),
    m_upperDiode(-1),
    m_lowerReadout(0),
    m_upperReadout(-1)
{}
 
void
PixelMultipleConnection1D::set(const int lowerIndex,
			       const std::vector<int> &connections) 
{

  // extract lower and upper diode and readout cell numbers from connections vector
  bool firstDiode = true;
  bool firstReadout = true;
  for(unsigned int index = 0; index < connections.size(); ++index) {
    int readout = connections[index];
    int diode = lowerIndex + index;
    if (firstDiode && readout != diode) {
      m_lowerDiode = diode; 
      m_upperDiode = diode; 
      firstDiode = false;
    }
    if (firstReadout) {
      m_lowerReadout=readout;
      m_upperReadout=readout;
      firstReadout = false;
    }
    if (readout<m_lowerReadout) m_lowerReadout=readout;
    if (readout>m_upperReadout) m_upperReadout=readout;
    if (readout != diode && diode < m_lowerDiode) m_lowerDiode = diode; 
    if (readout != diode && diode > m_upperDiode) m_upperDiode = diode; 
  }

  // Fource readout range to be include diode range (this is generally already the case)
  if (m_lowerReadout > m_lowerDiode) m_lowerReadout = m_lowerDiode;
  if (m_upperReadout < m_upperDiode) m_upperReadout = m_upperDiode;

  m_diodeToReadout.resize(m_upperDiode - m_lowerDiode + 1);
  m_readoutToDiode.resize(m_upperReadout - m_lowerReadout + 1);

  // First fill primary cell. ie diode and readout the same.
  for(unsigned int index = 0; index < connections.size(); ++index) {
    int readout = connections[index];
    int diode = lowerIndex + index;
    if (readout == diode)  m_readoutToDiode[readout-m_lowerReadout].push_back(diode);
  }

  
  // Now the rest of readoutToDiode ie diode and readout not the same.
  // Fill also diodeToReadout
  for(unsigned int index = 0; index < connections.size(); ++index) {
    int readout = connections[index];
    int diode = lowerIndex + index;
    if (readout != diode)  m_readoutToDiode[readout - m_lowerReadout].push_back(diode);
    if (diode >= m_lowerDiode && diode <= m_upperDiode) m_diodeToReadout[diode - m_lowerDiode] = readout;
  }

  // Created ganged map. If the diode is ganged it points to the other diode.
  m_diodeGanged.resize(m_upperReadout - m_lowerReadout + 1);
  for(int iDiode = m_lowerReadout; iDiode <=  m_upperReadout; iDiode++) {
    // Fill with the same index which indicates its not ganged.
    m_diodeGanged[iDiode - m_lowerReadout] = iDiode;
    int readout = iDiode;
    if (!outsideDiode(iDiode)) readout = readoutOfDiode(iDiode);
    if (numberOfConnectedCells(readout) >= 2) {
      int cell0 = connectedCell(readout, 0);
      int cell1 = connectedCell(readout, 1);
      if (iDiode == cell0) {
	m_diodeGanged[iDiode - m_lowerReadout] = cell1;
      } else {
	m_diodeGanged[iDiode - m_lowerReadout] = cell0;
      }
    }
  }

  // For debugging only
  //debugPrintout();
}

void PixelMultipleConnection1D::debugPrintout() 
{
  // Debug printout
  std::cout << "m_lowerDiode = " << m_lowerDiode << std::endl;
  std::cout << "m_upperDiode = " << m_upperDiode << std::endl;
  std::cout << "m_lowerReadout = " << m_lowerReadout << std::endl;
  std::cout << "m_upperReadout = " << m_upperReadout << std::endl;
  std::cout << "m_readoutToDiode: ";
  for (unsigned int i = 0; i < m_readoutToDiode.size(); i++) {
    std::cout << "(" << m_lowerReadout + i << ": ";
    for (unsigned int j = 0; j < m_readoutToDiode[i].size(); j++) {
      std::cout << m_readoutToDiode[i][j] << " ";
    }
    std::cout << ") ";
  }
  std::cout << std::endl;
  
  for (unsigned int i = 0; i < m_diodeToReadout.size(); i++) {
    std::cout << "(" << m_lowerDiode + i << ": " << m_diodeToReadout[i] << ") ";
  }
  std::cout << std::endl;
  
  for (unsigned int i = 0; i < m_diodeGanged.size(); i++) {
    std::cout << "(" << m_lowerReadout + i << ": " << m_diodeGanged[i] << ") ";
  }
  std::cout << std::endl;
}

  

} // namespace InDetDD
