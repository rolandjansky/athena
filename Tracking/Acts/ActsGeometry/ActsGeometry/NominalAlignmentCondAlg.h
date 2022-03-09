/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;

private:
  SG::WriteCondHandleKey<ActsGeometryContext> m_wchk{
      this, "ActsAlignmentKey", "ActsAlignment", "cond handle key"};

  ServiceHandle<IActsTrackingGeometrySvc> m_trackingGeometrySvc;
};
