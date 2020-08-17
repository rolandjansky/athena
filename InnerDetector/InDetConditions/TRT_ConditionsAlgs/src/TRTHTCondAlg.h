/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTHTCONDALG_H
#define TRTHTCONDALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "TRT_ConditionsData/HTcalculator.h"
#include "AthenaPoolUtilities/CondAttrListVec.h"
#include "GaudiKernel/ICondSvc.h"
#include "Gaudi/Property.h"


class TRTHTCondAlg : public AthAlgorithm
{
 public:
  TRTHTCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TRTHTCondAlg() override;

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;

 private:
  ServiceHandle<ICondSvc> m_condSvc;
  SG::ReadCondHandleKey<CondAttrListVec> m_ReadKey{this,"HTReadKey","/TRT/Calib/PID_vector","Pid in-key"};
  SG::WriteCondHandleKey<HTcalculator> m_WriteKey{this,"HTWriteKey","HTcalculator","HTcalcutor out-key"};

};
#endif
