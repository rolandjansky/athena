/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigRNNOutputContainerCnv_tlp1
 *
 * @brief "top level" transient-persistent converter for TrigRNNOutputContainer
 *
 * @author Danilo Enoque Ferreira de Lima <dferreir@mail.cern.ch> - UFRJ
 *
 * $Id$
 **********************************************************************************/
#ifndef  TRIGEVENTTPCNV_TRIGRNNOUTPUTCONTAINER_CNV_TLP1_H
#define  TRIGEVENTTPCNV_TRIGRNNOUTPUTCONTAINER_CNV_TLP1_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigCaloEventTPCnv/TrigRNNOutputContainer_tlp1.h"

#include "TrigCaloEventTPCnv/TrigRNNOutputContainerCnv_p1.h"
#include "TrigCaloEventTPCnv/TrigRNNOutputCnv_p1.h" 
 
class TrigRNNOutputContainerCnv_tlp1 : public AthenaPoolTopLevelTPConverter<TrigRNNOutputContainerCnv_p1, TrigRNNOutputContainer_tlp1> {

 public:

  TrigRNNOutputContainerCnv_tlp1();
  virtual ~TrigRNNOutputContainerCnv_tlp1() { }
 
  // default methods  
  virtual void setPStorage(TrigRNNOutputContainer_tlp1 *storage);
  
 protected:
  TrigRNNOutputContainerCnv_p1 m_trigRNNOutputContCnv;
  TrigRNNOutputCnv_p1          m_trigRNNOutputCnv;
 
};

#endif

