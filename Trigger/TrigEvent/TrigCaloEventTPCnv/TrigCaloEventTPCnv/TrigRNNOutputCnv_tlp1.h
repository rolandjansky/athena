/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigRNNOutputCnv_tlp1
 *
 * @brief "top level" transient-persistent converter for TrigRNNOutput
 *
 * @author Danilo Enoque Ferreira de Lima <dferreir@mail.cern.ch> - UFRJ
 *
 * File and Version Information:
 * $Id$
 **********************************************************************************/
#ifndef TRIGEVENTTPCNV_TRIGRNNOUTPUTCNV_TLP1_H
#define TRIGEVENTTPCNV_TRIGRNNOUTPUTCNV_TLP1_H

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigCaloEventTPCnv/TrigRNNOutput_tlp1.h"
#include "TrigCaloEventTPCnv/TrigRNNOutputCnv_p1.h"

class TrigRNNOutputCnv_tlp1 : public AthenaPoolTopLevelTPConverter<TrigRNNOutputCnv_p1, TrigRNNOutput_tlp1> {

 public:

  TrigRNNOutputCnv_tlp1();
  virtual ~TrigRNNOutputCnv_tlp1() { }

  virtual void setPStorage(TrigRNNOutput_tlp1 *storage);

 protected:

  // T/P converters for ALL types used in TrigRNNOutput
  TrigRNNOutputCnv_p1            m_trigRNNOutputCnv;

};


#endif

