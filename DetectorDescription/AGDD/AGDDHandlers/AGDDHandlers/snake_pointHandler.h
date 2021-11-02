/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef snake_pointHandler_H
#define snake_pointHandler_H

#include "AGDDControl/XMLHandler.h"

#include "GeoModelKernel/GeoDefinitions.h"

#include <string>

class snake_pointHandler:public XMLHandler {
public:
	snake_pointHandler(const std::string&,
                           AGDDController& c);
	virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
	GeoTrf::Vector3D CurrentPoint() {return m_point;}
private:
	GeoTrf::Vector3D m_point;
};

#endif
