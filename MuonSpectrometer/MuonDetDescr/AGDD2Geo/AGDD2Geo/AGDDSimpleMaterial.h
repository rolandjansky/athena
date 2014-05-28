/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDSimpleMaterial_H
#define AGDDSimpleMaterial_H

#include <string>
#include <iostream>

#include "AGDD2Geo/MaterialTypes.h"

class AGDDSimpleMaterial {
	friend std::ostream& operator <<(std::ostream&, const AGDDSimpleMaterial &);
public:
	AGDDSimpleMaterial(std::string n, double d):
		_name(n),_density(d),_created(false)
	{
		mType=Undefined;
		RegisterToStore(this);
	}
	virtual ~AGDDSimpleMaterial() {}
	void RegisterToStore(AGDDSimpleMaterial*);
	std::string GetName();
	double GetDensity() {return _density;}
	material_type GetMaterialType() {return mType;}
	void Created(bool b) {_created=b;}
	bool Extant() {return _created;}
	void* GetMaterial() {return theMaterial;}
	void SetMaterial(void* mat) {theMaterial=mat;}
protected:
	std::string _name;
	double _density;
	bool _created;
	
	material_type mType;
	void* theMaterial;
};

#endif
