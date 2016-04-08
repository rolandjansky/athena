/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCCALIBEVENT_CSCCALIBDATA_H
#define CSCCALIBEVENT_CSCCALIBDATA_H

/**************************************************************************
Package: MuonSpectrometer/MuonCalib/CscCalib/CscCalibEvent
Name: CscCalibdata.h
Author: Ketevi A. Assamagan
Date & Place: October 03, 2005, BNL

Class to represent CSC pulser calibration data for one strip
****************************************************************************/

#include <vector>

class CscCalibData {

private:

  /* hash it of the the strip */
  int m_idHash;

  /* strip calibration constants */
  std::vector<float> m_calibConst;

public:

  /* default constructor */
  CscCalibData() : m_idHash(0) {}

  /* full constructor */
  CscCalibData(int hashId, std::vector<float> calibConst) :
	m_idHash(hashId), m_calibConst(calibConst) {}
 
  /* destructor */
  virtual ~CscCalibData() {}

  /* set the identifier hash */
  void set_idHash(const int idHash) { m_idHash = idHash; }

  /* set the calibration constants */
  void set_calibConst (const std::vector<float> calibConst) { m_calibConst = calibConst; }

  /* get the identifier hash */
  int idHash() { return m_idHash; }

  /* get the calibration constants */
  const std::vector<float> & calibConst() { return m_calibConst; }

  /** Copy constructor */
  CscCalibData( const CscCalibData& rhs );

  /** Assignement operator */
  CscCalibData& operator =( const CscCalibData& rhs );

};

#endif
