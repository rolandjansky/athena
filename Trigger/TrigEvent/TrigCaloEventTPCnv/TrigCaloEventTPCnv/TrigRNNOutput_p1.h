/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigRNNOutput_p1
 *
 * @brief persistent partner for TrigRNNOutput
 *
 * @author Danilo Enoque Ferreira de Lima <dferreir@mail.cern.ch> - UFRJ
 *
 * File and Version Information:
 * $Id$
 **********************************************************************************/
#ifndef TRIGEVENTTPCNV_TRIGRNNOUTPUT_P1_H
#define TRIGEVENTTPCNV_TRIGRNNOUTPUT_P1_H

#include "DataModelAthenaPool/ElementLinkVector_p1.h"

class TrigRNNOutput_p1 {

  friend class TrigRNNOutputCnv;

  template <class T>
    friend class TrigRNNOutputConverterBase;

 public:
    
  // default constructor
  TrigRNNOutput_p1() { }

  // default destructor
  ~TrigRNNOutput_p1() { }

 private:

  std::vector<float> m_output;
  ElementLinkInt_p1 m_cluster;
};

#endif
