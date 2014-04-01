/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigRNNOutputContainer_p1
 *
 * @brief persistent partner for TrigRNNOutputContainer
 *
 * @author Danilo Enoque Ferreira de Lima <dferreir@mail.cern.ch> - UFRJ
 *
 * File and Version Information:
 * $Id$
 **********************************************************************************/
#ifndef TRIGEVENTTPCNV_TRIGRNNOUTPUTCONTAINER_P1_H
#define TRIGEVENTTPCNV_TRIGRNNOUTPUTCONTAINER_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>
  
class TrigRNNOutputContainer_p1 : public std::vector<TPObjRef> {
 public:
  TrigRNNOutputContainer_p1() { }
};

#endif

