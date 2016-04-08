/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMissingEtEventTPCnv
 * @Class  : TrigMissingET_p3
 *
 * @brief persistent partner for TrigMissingET
 *
 * @author Diego Casadei  <Diego.Casadei@cern.ch>  - NYU
 *
 * File and Version Information:
 * $Id: TrigMissingET_p3.h 722331 2016-02-07 21:58:59Z ssnyder $
 **********************************************************************************/
#ifndef TRIGMISSINGETEVENTTPCNV_TRIGMISSINGET_p3_H
#define TRIGMISSINGETEVENTTPCNV_TRIGMISSINGET_p3_H

#include <stdint.h>
#include <string>
#include <vector>
#include "TrigMissingEtEvent/TrigMissingET.h"

class TrigMissingET_p3
{
  friend class TrigMissingETCnv_p3;

 public:
  
  TrigMissingET_p3() : m_flag(0) {}
  virtual ~TrigMissingET_p3(){}
  
  //private:
  /** Basic information **/

  float m_allTheFloats[5];
  //float m_ex, m_ey, m_ez;  //<! components of the missing energy vector
  //float m_sum_et, m_sum_e; //<! sums of |Et| and |E|
  int   m_flag;            //<! event flag: 0 (default) means OK
  long  m_roiWord;         //<! RoI word

  /** Auxiliary information **/
  /// transient class contains std::vector< TrigMissingEtComponent > m_compVec;
  /// that is "flattened" here into separate arrays
  unsigned int m_comp_number; //<! size of all following vectors
  std::vector<std::string>    m_c_name;
  std::vector<short>          m_c_status;
  std::vector<float>          m_c_ex;
  std::vector<float>          m_c_ey;
  std::vector<float>          m_c_ez;
  std::vector<float>          m_c_sumEt;
  std::vector<float>          m_c_sumE;
  std::vector<float>          m_c_calib0;
  std::vector<float>          m_c_calib1;
  std::vector<short>          m_c_sumOfSigns;
  std::vector<unsigned short> m_c_usedChannels;



};

#endif
