/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDMaterial_H
#define AGDDMaterial_H

#include "AGDD2Geo/AGDDSimpleMaterial.h"

#include <string>

class AGDDElement;

class AGDDMaterial: public AGDDSimpleMaterial {
	friend std::ostream& operator <<(std::ostream&, const AGDDMaterial &);
public:
	AGDDMaterial(std::string,AGDDElement*, double);
	AGDDMaterial(std::string,int, double,double);
	int GetZ() {return _z;}
	double GetA() {return _a;}
	AGDDElement* GetElement() {return _element;}
	void Created(bool b) {_created=b;}
	bool Extant() {return _created;}
private:
	int _z;
	double _a;
	AGDDElement* _element;	
	bool _created;
};

#endif
