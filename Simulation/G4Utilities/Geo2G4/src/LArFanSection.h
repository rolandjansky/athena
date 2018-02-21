/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __LARFANSECTION_H__
#define __LARFANSECTION_H__

// helper class to replace G4Polycone
// in certain LArWheelSolid operations

class LArFanSections
{
public:
	G4double Amin, Amax;
	G4double Bmin, Bmax;
	G4double Amin2, Amax2;
	G4double Bmin2, Bmax2;
	G4double xmin, xmax;
	G4double Cflat2, ABmax, ABmin;

	LArFanSections(
		G4double ri1, G4double ri2,
		G4double ro1, G4double ro2,
		G4double Xmax, G4double z1, G4double z2
	);

	void print(void) const;
};

#endif // __LARFANSECTION_H__
