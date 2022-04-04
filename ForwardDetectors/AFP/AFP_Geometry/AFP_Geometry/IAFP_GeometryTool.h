/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#ifndef AFP_GEOMETRY_IAFP_GeometryTool_H
#define AFP_GEOMETRY_IAFP_GeometryTool_H 1
 
#include "GaudiKernel/IAlgTool.h"
#include "AFP_Geometry/AFP_ConfigParams.h"

class IAFP_GeometryTool : virtual public IAlgTool {
public:
   virtual void SetCfgParams(AFP_CONFIGURATION * pCfgParams) = 0;
 
   DeclareInterfaceID( IAFP_GeometryTool, 1, 0);
 
};
#endif // !AFP_GEOMETRY_IAFP_GeometryTool_H
