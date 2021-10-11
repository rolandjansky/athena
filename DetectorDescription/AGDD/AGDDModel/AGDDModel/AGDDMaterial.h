/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDMaterial_H
#define AGDDMaterial_H

#include "AGDDModel/AGDDSimpleMaterial.h"

#include <string>

class AGDDElement;
class AGDDMaterialStore;

class AGDDMaterial: public AGDDSimpleMaterial {
	friend std::ostream& operator <<(std::ostream&, const AGDDMaterial &);
public:
	AGDDMaterial(AGDDMaterialStore& ms,
                     const std::string&,AGDDElement*, double);
	AGDDMaterial(AGDDMaterialStore& ms,
                     const std::string&,int, double,double);
	int GetZ() const {return m_z;}
	double GetA() const {return m_a;}
	AGDDElement* GetElement() {return m_element;}
	void Created(bool b) {m_matCreated=b;}
	bool Extant() const {return m_matCreated;}
private:
	int m_z;
	double m_a;
	AGDDElement* m_element;	
	bool m_matCreated;
};

#endif
