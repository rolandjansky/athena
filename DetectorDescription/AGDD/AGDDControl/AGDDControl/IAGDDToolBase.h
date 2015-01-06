/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IAGDDToolBase_H
#define IAGDDToolBase_H

#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IAGDDToolBase("IAGDDToolBase",1,0);

//  Interface to define an AGDD-constructing tool to be used from AGDD2GeoSvc

class IAGDDToolBase: virtual public IAlgTool
{
public:
	static const InterfaceID& interfaceID() {return IID_IAGDDToolBase;}
	
	virtual ~IAGDDToolBase() {}
	
	virtual StatusCode construct() =0;
};

#endif
