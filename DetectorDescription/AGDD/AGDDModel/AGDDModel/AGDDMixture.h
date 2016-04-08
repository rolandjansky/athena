/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDMixture_H
#define AGDDMixture_H

#include "AGDDModel/AGDDSimpleMaterial.h"

#include <string>
#include <vector>

class AGDDElement;

class AGDDMixture: public AGDDSimpleMaterial {
	friend std::ostream& operator <<(std::ostream&, const AGDDMixture &);
public:
	AGDDMixture(std::string,double);
	void AddMaterial(std::string);
	void Fraction(double);
	int NComponents() {return m_theMaterials.size();}
	AGDDSimpleMaterial* Material(int i) {return m_theMaterials[i];}
	double Composition(int i) {return m_theComposition[i];}
private:
	std::vector<AGDDSimpleMaterial*> m_theMaterials;
	std::vector<double> m_theComposition;
};

#endif
