/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @class  : TrigRNNOutputCnv_p2
 *
 * @brief transient persistent converter for TrigRNNOutput
 *
 * @author Danilo Enoque Ferreira de Lima  <dferreir@mail.cern.ch> - UFRJ
 *
 * File and Version Information:
 * $Id: TrigRNNOutputCnv_p2.h 361188 2011-04-23 10:04:31Z salvator $
 **********************************************************************************/
#ifndef TRIGEVENTTPCNV_TRIGRNNOUTPUTCNV_P2_H
#define TRIGEVENTTPCNV_TRIGRNNOUTPUTCNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigCaloEvent/TrigRNNOutput.h"
#include "TrigCaloEventTPCnv/TrigRNNOutput_p2.h"

#include "AthLinks/ElementLink.h"
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"
#include "TrigCaloEvent/TrigEMCluster.h"
#include "TrigCaloEvent/TrigEMClusterContainer.h"

class MsgStream;

class TrigRNNOutputCnv_p2  : public T_AthenaPoolTPCnvBase<TrigRNNOutput, TrigRNNOutput_p2>  
{
public:
  TrigRNNOutputCnv_p2() { }

  virtual void persToTrans(const TrigRNNOutput_p2 *persObj, TrigRNNOutput *transObj, MsgStream &log);
  virtual void transToPers(const TrigRNNOutput *transObj, TrigRNNOutput_p2 *persObj, MsgStream &log);

protected:
  ElementLinkCnv_p3< ElementLink<TrigEMClusterContainer> > ELinkTrigEMClusterCnv;
};
#endif

