/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDMixture_H
#define AGDDMixture_H

#include "AGDDModel/AGDDSimpleMaterial.h"

#include <string>
#include <vector>

class AGDDElement;
class AGDDMaterialStore;

class AGDDMixture: public AGDDSimpleMaterial {
	friend std::ostream& operator <<(std::ostream&, const AGDDMixture &);
public:
	AGDDMixture(AGDDMaterialStore& ms, const std::string&,double);
        void AddMaterial(AGDDMaterialStore& ms, const std::string&);
	void Fraction(double);
	int NComponents() const {return m_theMaterials.size();}
	AGDDSimpleMaterial* Material(int i) {return m_theMaterials[i];}
	double Composition(int i) const {return m_theComposition[i];}
private:
	std::vector<AGDDSimpleMaterial*> m_theMaterials;
	std::vector<double> m_theComposition;
};

#endif
