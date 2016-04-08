/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigRNNOutput_p2
 *
 * @brief persistent partner for TrigRNNOutput
 *
 * @author Danilo Enoque Ferreira de Lima <dferreir@mail.cern.ch> - UFRJ
 *
 * File and Version Information:
 * $Id: TrigRNNOutput_p2.h 724033 2016-02-15 16:24:28Z gemmeren $
 **********************************************************************************/
#ifndef TRIGEVENTTPCNV_TRIGRNNOUTPUT_P2_H
#define TRIGEVENTTPCNV_TRIGRNNOUTPUT_P2_H

#include "DataModelAthenaPool/ElementLink_p3.h"

class TrigRNNOutput_p2 {

  friend class TrigRNNOutputCnv_p2;

 public:
    
  // default constructor
  TrigRNNOutput_p2() { }

  // default destructor
  ~TrigRNNOutput_p2() { }

 private:

  std::vector<float> m_output;
  ElementLinkInt_p3 m_cluster;
};

#endif
