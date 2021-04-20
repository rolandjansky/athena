/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPOTOOLCONFIG_PASSTHROUGHFILTER_H
#define TRIGJETHYPOTOOLCONFIG_PASSTHROUGHFILTER_H
/********************************************************************
 *
 * NAME:     TrigJetHypoToolConfig_passthroughfilter.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * produces a PassThroughFilter (no filtering).
 *********************************************************************/


#include "TrigCompositeUtils/HLTIdentifier.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

#include "ITrigHypoJetVectorFilterConfig.h"

class TrigJetHypoToolConfig_passthroughfilter:
  public extends<AthAlgTool, ITrigHypoJetVectorFilterConfig> {
  
public:
  
  TrigJetHypoToolConfig_passthroughfilter(const std::string& type,
					  const std::string& name,
					  const IInterface* parent);
  
  virtual ~TrigJetHypoToolConfig_passthroughfilter();

  virtual StatusCode initialize() override;
  virtual FilterPtr getHypoJetVectorFilter() const override;


};
#endif
