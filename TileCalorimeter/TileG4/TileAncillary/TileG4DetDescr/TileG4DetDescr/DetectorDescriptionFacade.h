/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DetectorDescriptionFacade_H
#define DetectorDescriptionFacade_H

#include "TileG4DetDescr/DetectorDescription.h"

namespace FADS {

class DetectorDescriptionFacade: public DetectorDescription {
public:
	DetectorDescriptionFacade(const DetectorDescription& d):
				DetectorDescription(d) {}
};

}	// end namespace

#endif
