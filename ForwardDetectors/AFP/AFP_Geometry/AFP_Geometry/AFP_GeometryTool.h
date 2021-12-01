/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @class AFP_GeometryTool
 * @brief
 **/

#ifndef AFP_GEOMETRY_AFP_GeometryTool_H
#define AFP_GEOMETRY_AFP_GeometryTool_H

// Package includes
#include "AFP_Geometry/IAFP_GeometryTool.h"

// Framework includes
#include "AthenaBaseComps/AthAlgTool.h"

#include "AFP_Geometry/AFP_Geometry.h"
#include "AFP_Geometry/AFP_constants.h"
#include "AFP_Geometry/AFP_ConfigParams.h"


class AFP_GeometryTool : public extends<AthAlgTool, IAFP_GeometryTool>, public AFP_Geometry {
public:
	AFP_GeometryTool(const std::string& type, const std::string& name, const IInterface* parent);
	virtual ~AFP_GeometryTool() override;

	virtual StatusCode initialize() override;
	virtual StatusCode finalize() override;

	virtual void SetCfgParams(AFP_CONFIGURATION * pCfgParams) override;

};

#endif // AFP_GEOMETRY_AFP_GeometryTool_H
