/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDMolecule_H
#define AGDDMolecule_H

#include "AGDDModel/AGDDSimpleMaterial.h"

#include <string>
#include <vector>

class AGDDElement;

class AGDDMolecule: public AGDDSimpleMaterial {
	friend std::ostream& operator <<(std::ostream&, const AGDDMolecule &);
public:
	AGDDMolecule(std::string,double);
	void AddElement(std::string);
	void NAtoms(int);
	int NComponents() {return m_theElements.size();}
	AGDDElement* Element(int i) {return m_theElements[i];}
	int Composition(int i) {return m_theComposition[i];}
private:
	std::vector<AGDDElement*> m_theElements;
	std::vector<int> m_theComposition;
};

#endif
