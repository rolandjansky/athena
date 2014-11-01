/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __LARFANSECTION_H__
#define __LARFANSECTION_H__

// helper class to replace G4Polycone
// in certain LArWheelSolid operations

#include<vector>

#include "G4VSolid.hh"

#define LArFanSectionsMult 1

struct LArFanSections
{
	G4double Amin, Amax;
	G4double Bmin, Bmax;
	G4double Amin2, Amax2;
	G4double Bmin2, Bmax2;
	G4double xmin, xmax;
	G4double Cflat2, ABmax, ABmin;
	G4int last_fs, first_flat;
	std::vector<G4double> z;
	std::vector<G4double> rmin2;
	std::vector<G4double> rmax2;

	LArFanSections(
		G4double Ain, G4double Aout,
		G4double Bin, G4double Bout,
		G4double Xmax
	);

	~LArFanSections()
	{
		z.clear();
		rmin2.clear();
		rmax2.clear();
	}

	void prepare(void);
	void print(void) const;
	size_t select_section(const G4double &);
};

#endif // __LARFANSECTION_H__
