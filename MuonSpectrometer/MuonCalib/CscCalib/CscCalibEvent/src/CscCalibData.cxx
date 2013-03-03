/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************
Author: Ketevi A. Assamagan
BNL, October 3, 2005
**********************************************************************/

#include "CscCalibEvent/CscCalibData.h"

/** copy constructor */
CscCalibData::CscCalibData( const CscCalibData& rhs ) :
  m_idHash                      ( rhs.m_idHash ),
  m_calibConst                  ( rhs.m_calibConst )

{}

/** Assignment operator */
CscCalibData& CscCalibData::operator=( const CscCalibData& rhs ) {
  if ( this != &rhs ) {
    m_idHash                      = rhs.m_idHash;
    m_calibConst                  = rhs.m_calibConst;
  }
  return *this;
}

  
