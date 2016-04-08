/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDMaterial_H
#define AGDDMaterial_H

#include "AGDDModel/AGDDSimpleMaterial.h"

#include <string>

class AGDDElement;

class AGDDMaterial: public AGDDSimpleMaterial {
	friend std::ostream& operator <<(std::ostream&, const AGDDMaterial &);
public:
	AGDDMaterial(std::string,AGDDElement*, double);
	AGDDMaterial(std::string,int, double,double);
	int GetZ() {return m_z;}
	double GetA() {return m_a;}
	AGDDElement* GetElement() {return m_element;}
	void Created(bool b) {m_created=b;}
	bool Extant() {return m_created;}
private:
	int m_z;
	double m_a;
	AGDDElement* m_element;	
	bool m_created;
};

#endif
