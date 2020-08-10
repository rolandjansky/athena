/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINDETACCELERATIONTOOL_TRIGINDETACCELERATIONTOOL_H
#define TRIGINDETACCELERATIONTOOL_TRIGINDETACCELERATIONTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TrigInDetAccelerationTool/ITrigInDetAccelerationTool.h"

#include "TrigInDetAccelerationService/ITrigInDetAccelerationSvc.h"

class TrigInDetAccelerationTool: public AthAlgTool, virtual public ITrigInDetAccelerationTool {

 public:
  TrigInDetAccelerationTool( const std::string&, const std::string&, const IInterface* );
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  size_t virtual exportSeedMakingJob(const TrigCombinatorialSettings&, const IRoiDescriptor*, const std::vector<TrigSiSpacePointBase>&, TrigAccel::DATA_EXPORT_BUFFER&) const override;
  
 private:

  ServiceHandle<ITrigInDetAccelerationSvc> m_accelSvc;
};

#endif
