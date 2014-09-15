/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/******************************
Author: Caleb Lampen <lampen@physics.arizona.edu>
University of Arizona, May 2008
 *****/

#include "CscCalibData/CscCalibResult.h"

//**Copy constructor
/*CscCalibResult::CscCalibResult( const CscCalibResult &result) :
  {
  operator=(result);
  }

  CscCalibResult& CscCalibResult::operator=( const CscCalibResult& result) 
  {
  if( this != &result)
  {
  m_hashId = result.m_hashId;
  m_value = result.m_value;
  m_error = result.m_error;
  m_chi2 = result.m_chi2;
  m_ndf = result.m_ndf;
  }
  return *this;
  }*/

/* set the calibration constants */
void CscCalibResult::setValue (const float value) { m_value = value; }
void CscCalibResult::setError (const float error) { m_error = error; }
void CscCalibResult::setChi2 (const float chi2) { m_chi2 = chi2; }
void CscCalibResult::setNdf (const int ndf) { m_ndf= ndf; }
void CscCalibResult::setHashId(const int hashId) { m_hashId = hashId; }

/* get the identifier hash */
int CscCalibResult::hashId() const  { return m_hashId; }

/* get the calibration constants */
float CscCalibResult::value() const   { return m_value; }
float CscCalibResult::error() const   { return m_error; }
float CscCalibResult::chi2()  const  { return m_chi2; }
int CscCalibResult::ndf()     const  { return m_ndf; }


