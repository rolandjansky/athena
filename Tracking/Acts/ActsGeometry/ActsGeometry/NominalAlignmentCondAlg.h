/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#pragma once

// ATHENA
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ICondSvc.h"
#include "StoreGate/WriteCondHandleKey.h"

// PACKAGE

// STL
#include <string>

class IActsTrackingGeometrySvc;
class ActsAlignmentStore;

class ActsGeometryContext;

/// @class NominalAlignmentCondAlg
/// Conditions algorithm which produces an (effectively)
/// infinitely valid ActsAlignmentStore which has
/// nominal alignments (= identity deltas)
///
class NominalAlignmentCondAlg : public AthAlgorithm {

public:
  NominalAlignmentCondAlg(const std::string &name, ISvcLocator *pSvcLocator);
  virtual ~NominalAlignmentCondAlg();

  virtual bool isClonable() const override { return true; }

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;

private:
  SG::WriteCondHandleKey<ActsGeometryContext> m_wchk{
      this, "ActsAlignmentKey", "ActsAlignment", "cond handle key"};

  ServiceHandle<ICondSvc> m_cs;
  ServiceHandle<IActsTrackingGeometrySvc> m_trackingGeometrySvc;
};
