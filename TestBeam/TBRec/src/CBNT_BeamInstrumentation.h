/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBREC_CBNT_BEAM_H
#define TBREC_CBNT_BEAM_H

#include "CBNT_TBRecBase.h"
#include <fstream>
#include <string>

class CBNT_BeamInstrumentation : public CBNT_TBRecBase
{
 public:
  CBNT_BeamInstrumentation(const std::string & name, ISvcLocator * pSvcLocator);

  virtual ~CBNT_BeamInstrumentation();

  //standart algorithm methods
  virtual StatusCode CBNT_initialize() override;
  virtual StatusCode CBNT_execute() override;
  virtual StatusCode CBNT_finalize() override;

  virtual StatusCode CBNT_clear() override;
 private:

  bool m_neverReturnFailure;   // controls when StatusCode::FAILURE can be issued in execute()

  std::string m_containerKey1;
  std::string m_containerKey2;
  std::string m_containerKey3;
  std::string m_containerKey4;
  std::string m_containerKey5;
  std::vector<std::string> m_scint_names;
  std::vector<std::string> m_bpc_names;

  float *m_scint_adc, *m_scint_tdc, *m_bpc_x, *m_bpc_y;
  int m_trig;
  float m_clock;

  // BPC raw data, set to False by default
  bool m_bpc_raw;
  float *m_bpc_up, *m_bpc_down, *m_bpc_left, *m_bpc_right, *m_bpc_adc_hor, *m_bpc_adc_ver;

  // Trigger Array, set to False by default 
  bool m_trig_array;
  std::vector<unsigned int> m_triggflag;

  int m_trig_id;
  std::vector<long> *m_trig_sel;

  //Private function to build names for ntuple entries. Concatinates strings
  //but omitts characters that are problematic for root like -+:*/
  std::string add_name(const char* base, const std::string& extension);
};

#endif
