/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDMolecule_H
#define AGDDMolecule_H

#include "AGDDModel/AGDDSimpleMaterial.h"

#include <string>
#include <vector>

class AGDDElement;
class AGDDMaterialStore;

class AGDDMolecule: public AGDDSimpleMaterial {
	friend std::ostream& operator <<(std::ostream&, const AGDDMolecule &);
public:
	AGDDMolecule(AGDDMaterialStore& ms, const std::string&,double);
        void AddElement(AGDDMaterialStore& ms, const std::string&);
	void NAtoms(int);
	int NComponents() const {return m_theElements.size();}
	AGDDElement* Element(int i) {return m_theElements[i];}
	int Composition(int i) const {return m_theComposition[i];}
private:
	std::vector<AGDDElement*> m_theElements;
	std::vector<int> m_theComposition;
};

#endif
