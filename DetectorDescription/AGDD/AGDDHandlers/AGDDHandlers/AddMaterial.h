/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AddMaterial_H
#define AddMaterial_H

#include <string>
#include <vector>

class AddMaterial {
public:
	void Clear() 
	{
		names.clear();
		natoms.clear();
		fractions.clear();
	}
	std::vector<std::string> names;
	std::vector<int> natoms;
	std::vector<double> fractions;
};

#endif
