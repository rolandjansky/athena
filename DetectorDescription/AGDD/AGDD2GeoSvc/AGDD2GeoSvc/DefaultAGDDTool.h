/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DefaultAGDDTool_H
#define DefaultAGDDTool_H

#include "AGDDControl/AGDDToolBase.h"
#include "GaudiKernel/StatusCode.h"

class DefaultAGDDTool: public AGDDToolBase
{
public:
	DefaultAGDDTool(const std::string& type, const std::string& name, 
				 const IInterface* parent);
	virtual StatusCode construct();
	
	virtual StatusCode initialize();
	
private:
	

};

#endif
