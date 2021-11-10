// -*- C++ -*-
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HGTD_CONDITIONSALGORITHMS_HGTD_DETECTORELEMENTCONDALG_H
#define HGTD_CONDITIONSALGORITHMS_HGTD_DETECTORELEMENTCONDALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "HGTD_ReadoutGeometry/HGTD_DetectorElementCollection.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "StoreGate/CondHandleKeyArray.h"

#include "GaudiKernel/ICondSvc.h"

class HGTD_DetectorManager;

class HGTD_DetectorElementCondAlg : public AthReentrantAlgorithm
{
 public:
  HGTD_DetectorElementCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~HGTD_DetectorElementCondAlg() override = default;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  /** Make this algorithm clonable. */
  virtual bool isClonable() const override { return true; };

 private:
  SG::WriteCondHandleKey<InDetDD::HGTD_DetectorElementCollection> m_writeKey
  {this, "WriteKey", "HGTD_DetectorElementCollection", "Key of output HGTD_DetectorElementCollection for HGTD"};

  ServiceHandle<ICondSvc> m_condSvc{this, "CondSvc", "CondSvc"};

  StringProperty m_detManagerName{this, "DetManagerName", "HGTD", "Name of the DeterctorManager to retrieve"};
  const HGTD_DetectorManager* m_detManager{nullptr};
};

#endif // HGTD_CONDITIONSALGORITHMS_HGTD_DETECTORELEMENTCONDALG_H
