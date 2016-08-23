/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEG4DETDESCR_DETECTORDESCRIPTIONFACADE_H
#define TILEG4DETDESCR_DETECTORDESCRIPTIONFACADE_H

#include "TileG4DetDescr/DetectorDescription.h"

namespace FADS {

class DetectorDescriptionFacade: public DetectorDescription {
public:
	DetectorDescriptionFacade(const DetectorDescription& d):
				DetectorDescription(d) {}
};

}	// end namespace

#endif // TILEG4DETDESCR_DETECTORDESCRIPTIONFACADE_H
