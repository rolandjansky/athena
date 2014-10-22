/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKRECORD_P0_H
#define TRACKRECORD_P0_H

#include "CLHEP/Vector/ThreeVector.h"

class TrackRecord_p0 {
public:
	TrackRecord_p0() : Energy(0), Momentum(0,0,0), PDG_code(0), Position(0,0,0) {}
	virtual ~TrackRecord_p0() {}

	double GetEnergy() {return Energy;}
	CLHEP::Hep3Vector GetPosition() {return Position;}
	CLHEP::Hep3Vector GetMomentum() {return Momentum;}
	int GetPDGCode() {return PDG_code;}

private:
	double Energy;
	CLHEP::Hep3Vector Momentum;
	int PDG_code;
	CLHEP::Hep3Vector Position;
};

#endif

