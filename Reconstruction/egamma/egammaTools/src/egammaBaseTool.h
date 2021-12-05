/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMATOOLS_EGAMMABASETOOL_H
#define EGAMMATOOLS_EGAMMABASETOOL_H

/**
  @class  egammaBaseTool
          Base Tool for all egamma tool that require
          an egamma object as its input. Hence the specific versions
          will have access to all the framework services in addition
          of being passed an 'egamma' object.  The derived
          tools must implement
          execute(const EventContext& ctx, xAOD::Egamma* eg).
          Used typically for constucting ToolHandleArrays of
          egammaTools
  @author F. Derue, A. Kaczmarska
*/

#include "egammaInterfaces/IegammaBaseTool.h"

#include "AthenaBaseComps/AthAlgTool.h"

class egammaBaseTool
  : public AthAlgTool
  , virtual public IegammaBaseTool
{

public:
  /** @brief Default constructor*/
  egammaBaseTool(const std::string& type,
                 const std::string& name,
                 const IInterface* parent);
  /** @brief destructor*/
  virtual ~egammaBaseTool() = default;

  /** @brief initialize method*/
  virtual StatusCode initialize() override;
  /** @brief finalize method*/
  virtual StatusCode finalize() override;
};

#endif // EGAMMATOOLS_EGAMMABASETOOL_H
