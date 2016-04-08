/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMissingEtEventTPCnv
 * @Class  : TrigMissingET_p2
 *
 * @brief persistent partner for TrigMissingET
 *
 * @author Diego Casadei  <Diego.Casadei@cern.ch>  - NYU
 *
 * File and Version Information:
 * $Id: TrigMissingET_p2.h,v 1.2 2009-04-01 22:10:11 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMISSINGETEVENTTPCNV_TRIGMISSINGET_p2_H
#define TRIGMISSINGETEVENTTPCNV_TRIGMISSINGET_p2_H

#include <stdint.h>
#include <string>
#include <vector>
#include "TrigMissingEtEvent/TrigMissingET.h"

class TrigMissingET_p2 
{
  friend class TrigMissingETCnv;

 public:
  
  TrigMissingET_p2() {}
  virtual ~TrigMissingET_p2(){}
  
  //private:
  /** Basic information **/
  float m_ex, m_ey, m_ez;  //<! components of the missing energy vector
  float m_sum_et, m_sum_e; //<! sums of |Et| and |E|
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
