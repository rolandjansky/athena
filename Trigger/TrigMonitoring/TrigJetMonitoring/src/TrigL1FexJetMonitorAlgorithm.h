
/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETMONITORING_TRIGL1FEXJETMONITORALGORITHM_H
#define TRIGJETMONITORING_TRIGL1FEXJETMONITORALGORITHM_H

#include "TrigJetMonitoring/ITrigJetMonitorTool.h"

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "AsgDataHandles/ReadDecorHandleKey.h"


/*
 * Algorithm uses an AlgTool templated on the Container type
 * to extract data (as floats) to be monitored. Supported containers are:
 * 
 * JetContainerType: xAOD:::jFexSRJetRoIContainer
 * JetContainerType: xAOD:::jFexLRJetRoIContainer
 * JetContainerType: xAOD:::gFexJetRoIContainer
 */

class TrigL1FexJetMonitorAlgorithm : public AthMonitorAlgorithm {
public:
  TrigL1FexJetMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TrigL1FexJetMonitorAlgorithm();
  virtual StatusCode initialize() override;
  
  virtual StatusCode fillHistograms(const EventContext& ctx) const override;
  
private:
  ToolHandle<ITrigJetMonitorTool> m_filler {
  this, "filler", {}, "jet data calculator"};

  Gaudi::Property<std::string> m_groupName {
    this, "group_name",  {},  "name of monitoring group"};
  
  
   
};
#endif
