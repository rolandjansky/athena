/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTPIDNNCONDALG_H
#define TRTPIDNNCONDALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "TRT_ConditionsNN/TRTPIDNN.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "GaudiKernel/ICondSvc.h"
#include "Gaudi/Property.h"

/****************************************************************************************\

  Conditions algorithm to handle TRT PID NN.

  Author: Christian Grefe (christian.grefe@cern.ch)

\****************************************************************************************/

class TRTPIDNNCondAlg : public AthAlgorithm
{
 public:
  TRTPIDNNCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TRTPIDNNCondAlg() override;

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;

 private:
  ServiceHandle<ICondSvc> m_condSvc;
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKey{this,"TRTPIDNNReadKey","/TRT/Calib/PID_NN","TRTPIDNN in-key"};
  SG::WriteCondHandleKey<InDet::TRTPIDNN> m_writeKey{this,"TRTPIDNNWriteKey","TRTPIDNN","TRTPIDNN out-key"};

};
#endif
