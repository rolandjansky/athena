/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
	virtual StatusCode construct ATLAS_NOT_THREAD_SAFE () override;
	
	virtual StatusCode initialize ATLAS_NOT_THREAD_SAFE () override;
	
private:
	

};

#endif
