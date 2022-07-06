/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DECISIONHANDLING_IROIUPDATERTOOL_H
#define DECISIONHANDLING_IROIUPDATERTOOL_H

#include <memory>

#include "GaudiKernel/IAlgTool.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/StatusCode.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"



/**
 * @class IRoiUpdaterTool
 **/
class IRoiUpdaterTool : virtual public IAlgTool {

public:

  DeclareInterfaceID(IRoiUpdaterTool, 1, 0);

  virtual ~IRoiUpdaterTool() = default;

  virtual std::unique_ptr<TrigRoiDescriptor> execute( const IRoiDescriptor* roi, const EventContext& ctx) const = 0;

  virtual std::unique_ptr<TrigRoiDescriptor> execute( const EventContext& ctx ) const = 0;

};

#endif //> !DECISIONHANDLING_IROIUPDATERTOOL_H

