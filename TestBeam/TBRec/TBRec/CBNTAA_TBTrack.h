/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CBNTAA_TBTRACK_H
#define CBNTAA_TBTRACK_H

#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "CBNT_TBRecBase.h"
#include <fstream>
#include <string>

class StoreGateSvc;

class CBNTAA_TBTrack : public CBNT_TBRecBase
{
 public:
  CBNTAA_TBTrack(const std::string & name, ISvcLocator * pSvcLocator);

  ~CBNTAA_TBTrack() {};

  virtual StatusCode CBNT_initialize();
  virtual StatusCode CBNT_execute();
  virtual StatusCode CBNT_finalize();

 private:
  // by A. Mineanko
  /** Get Xcryo and Ytable from a text file */
  bool m_readFileforXcryo;
  StatusCode getXcryoYtable(float &x, float &y, float &eBeam);
  int m_nRun;

  // Beam track parameters
  float m_beam_coor_x;
  float m_beam_coor_y;
  float m_beam_chi2_x;
  float m_beam_chi2_y;
  float m_beam_intercept_x;
  float m_beam_intercept_y;
  float m_beam_slope_x;
  float m_beam_slope_y;

  // taken from CBNTAA_BeamInstrumentation
  //Private function to build names for ntuple entries. Concatinates strings
  //but omitts characters that are problematic for root like -+:*/
  std::string add_name(const char* base, const std::string extension);
};

#endif
