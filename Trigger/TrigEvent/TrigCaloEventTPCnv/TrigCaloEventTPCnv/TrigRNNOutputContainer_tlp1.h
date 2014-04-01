/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigRNNOutputContainer_tlp1
 *
 * @brief "top level" persistent partner for TrigRNNOutputContainer
 *
 * @author Danilo Enoque Ferreira de Lima <dferreir@mail.cern.ch> - UFRJ
 *
 * $Id$
 **********************************************************************************/
#ifndef TRIGEVENTTPCNV_TRIGRNNOUTPUTCONTAINER_TLP1_H
#define TRIGEVENTTPCNV_TRIGRNNOUTPUTCONTAINER_TLP1_H

#include "TrigCaloEventTPCnv/TrigRNNOutputContainer_p1.h"
#include "TrigCaloEventTPCnv/TrigRNNOutput_p1.h"

class TrigRNNOutputContainer_tlp1 {

 public:
  TrigRNNOutputContainer_tlp1() { }
  friend class TrigRNNOutputContainerCnv_tlp1;

 private:

  std::vector<TrigRNNOutputContainer_p1> m_TrigRNNOutputContainers;
  std::vector<TrigRNNOutput_p1> m_TrigRNNOutput;

};
 
#endif

