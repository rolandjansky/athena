/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCTSiLorentzAngleTestAlg.h
 * @author Susumu.Oda@cern.ch
 **/
#ifndef SCTSiLorentzAngleTestAlg_h
#define SCTSiLorentzAngleTestAlg_h

// Athena includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "InDetCondTools/ISiLorentzAngleTool.h"

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"

// forward declarations
class SCT_ID;

/**
 * @class SCTSiLorentzAngleTestAlg
 * This class tests SCTLorentzAngleTool
 **/

class SCTSiLorentzAngleTestAlg: public AthReentrantAlgorithm
{
 public:
  SCTSiLorentzAngleTestAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SCTSiLorentzAngleTestAlg() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;

 private:
  ToolHandle<ISiLorentzAngleTool> m_tool{this, "SCTLorentzAngleTool", "SCTLorentzAngleTool", "Tool to retrieve SCT Lorentz Angle Tool"};
  const SCT_ID* m_id;
};

#endif // SCTSiLorentzAngleTestAlg_h
