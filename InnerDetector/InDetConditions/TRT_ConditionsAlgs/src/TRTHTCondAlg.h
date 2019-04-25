/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTHTCONDALG_H
#define TRTHTCONDALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "TRT_ConditionsData/HTcalculator.h"
#include "AthenaPoolUtilities/CondAttrListVec.h"
#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/Property.h"


class TRTHTCondAlg : public AthReentrantAlgorithm
{
 public:
  TRTHTCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TRTHTCondAlg() override;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;

 private:
  ServiceHandle<ICondSvc> m_condSvc;
  SG::ReadCondHandleKey<CondAttrListVec> m_ReadKey{this,"HTReadKey","/TRT/Calib/PID_vector","Pid in-key"};
  SG::WriteCondHandleKey<HTcalculator> m_WriteKey{this,"HTWriteKey","HTcalculator","HTcalcutor out-key"};

};
#endif
