/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @Class  : TrigRNNOutputCnv
 *
 * @brief transient persistent converter for TrigRNNOutput
 *
 * @author Danilo E. F. de Lima <dferreir@mail.cern.ch>  - UFRJ
 *
 * File and Version Information:
 * $Id$
 **********************************************************************************/
#ifndef TRIGEVENTATHENAPOOL_TRIGRNNOUTPUTCNV_H
#define TRIGEVENTATHENAPOOL_TRIGRNNOUTPUTCNV_H

#include "TrigCaloEvent/TrigRNNOutput.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// typedef to the latest persistent version
class TrigRNNOutput_tlp1;
typedef TrigRNNOutput_tlp1  TrigRNNOutput_PERS;

class MsgStream;

struct TrigRNNOutputCnv_impl;
class TrigRNNOutputCnv  : public T_AthenaPoolCustomCnv<TrigRNNOutput, TrigRNNOutput_PERS > 
{

  friend class CnvFactory<TrigRNNOutputCnv>;

 protected:
  
  TrigRNNOutputCnv(ISvcLocator* svcloc);
  ~TrigRNNOutputCnv();

  TrigRNNOutput_PERS*  createPersistent(TrigRNNOutput* transCont);

  TrigRNNOutput*       createTransient ();

 private:
  TrigRNNOutputCnv_impl* m_impl;
};


#endif
