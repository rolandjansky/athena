/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : RingerRings_tlp1
 *
 * @brief "top level" persistent partner for RingerRings
 *
 * @author Danilo Enoque Ferreira de Lima <dferreir@mail.cern.ch> - UFRJ
 *
 * File and Version Information:
 * $Id: RingerRings_tlp1.h,v 1.1 2009-01-22 13:17:09 salvator Exp $
 **********************************************************************************/
#ifndef TRIGEVENTTPCNV_TRIGRNNOUTPUT_TLP1_H
#define TRIGEVENTTPCNV_TRIGRNNOUTPUT_TLP1_H

#include "TrigCaloEventTPCnv/TrigRNNOutput_p1.h"

class TrigRNNOutput_tlp1
{
 public:
  
  TrigRNNOutput_tlp1() {}
  friend class TrigRNNOutputCnv_tlp1; 
  
 private:
  
  std::vector<TrigRNNOutput_p1> m_trigRNNOutput;

};

#endif
