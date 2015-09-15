/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MaterialFacade_H
#define MaterialFacade_H

#include <string>
#include "FadsMaterial/FadsSimpleMaterial.h"


class Material {
public:
	Material(const std::string name,double a, double z, double density);
	static void List();
};

class CompositeMaterial {
public:
	CompositeMaterial(const std::string name,double density);
	void AddMaterial(const std::string name,double fractionMass);
	void AddElement(const std::string name,int nAtoms);
private:
	FADS::FadsSimpleMaterial *sm;
	std::string name;
	double density;
};

#endif
