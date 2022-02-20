/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/*
 * Updates: 
 * - 2022 Jan, Riccardo Maria BIANCHI <riccardo.maria.bianchi@cern.ch>
 *   Added MsgStream for logging
 */

#include "TileDetDescr/TileCellDim.h"

#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h" // needed for 'Athena::getMessageSvc()'

#include <stdexcept>
#include <iostream>
#include <cmath>

#define MLOG(x)   if (m_log->level()<=MSG::x) *m_log << MSG::x

TileCellDim::TileCellDim(unsigned int nRows) : 
  m_nRows(nRows),
  m_volume(0)
{
  m_log = new MsgStream(Athena::getMessageSvc(), "TileDetDescrManager");
  // convert tile radius to tile half-length
  m_Radius2HalfLength = tan ( M_PI / 64);

}

TileCellDim::~TileCellDim() 
{
}

double TileCellDim::getRMin(unsigned int index) const
{
  if(index < m_rMin.size())
    return m_rMin[index];
  else
    throw std::runtime_error( "Index out of range when retrieving RMin!" );
}

double TileCellDim::getRMax(unsigned int index) const
{
  if(index < m_rMax.size())
    return m_rMax[index];
  else
    throw std::runtime_error( "Index out of range when retrieving RMax!" );
}
 
double TileCellDim::getZMin(unsigned int index) const
{
  if(index < m_zMin.size())
    return m_zMin[index];
  else
    throw std::runtime_error( "Index out of range when retrieving ZMin!" );
}
 
double TileCellDim::getZMax(unsigned int index) const
{
  MLOG(DEBUG) << "TileCellDim::getZMax - index: " << index << ", size: " << m_zMax.size() << endmsg;
  if(index < m_zMax.size())
    return m_zMax[index];
  else
    throw std::runtime_error( "Index out of range when retrieving ZMax!" );
}

void TileCellDim::addRMin(double rMin)
{
  if(m_rMin.size() < m_nRows)
    m_rMin.push_back(rMin);
  else
    throw std::runtime_error( "Attempt to insert new RMin when vector size reached maximum!" );
}

void TileCellDim::addRMax(double rMax)
{
  if(m_rMax.size() < m_nRows)
    m_rMax.push_back(rMax);
  else
    throw std::runtime_error( "Attempt to insert new RMax when vector size reached maximum!" );
}
 
void TileCellDim::addZMin(double zMin)
{
  if(m_zMin.size() < m_nRows)
    m_zMin.push_back(zMin);
  else
    throw std::runtime_error( "Attempt to insert new ZMin when vector size reached maximum!" );
}

void TileCellDim::addZMax(double zMax)
{
  if(m_zMax.size() < m_nRows)
    m_zMax.push_back(zMax);
  else
    throw std::runtime_error( "Attempt to insert new ZMax when vector size reached maximum!" );
}

void TileCellDim::computeVolume()
{
  m_volume = 0.0;
  
  for (unsigned int i=0; i<m_nRows; ++i)
    m_volume += computeRowVolume( i );
}

double TileCellDim::computeRowVolume(int iRow)
{
  
  return  fabs ( m_Radius2HalfLength * (m_rMin[iRow] + m_rMax[iRow])
         * (m_rMax[iRow] - m_rMin[iRow])
         * (m_zMax[iRow] - m_zMin[iRow]) );
}


void TileCellDim::print() const
{
  std::cout << " >> Cell contains " << m_nRows << " rows" << std::endl;
  for(unsigned int i=0; i<m_nRows; ++i) {
    std::cout << m_rMin[i] << " "
              << m_rMax[i] << " "
              << m_zMin[i] << " "
              << m_zMax[i] << std::endl;
  }
  std::cout << " >> Cell Volume is " << m_volume << std::endl;
}
