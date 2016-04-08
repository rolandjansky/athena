/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDSimpleMaterial_H
#define AGDDSimpleMaterial_H

#include <string>
#include <iostream>

#include "AGDDModel/MaterialTypes.h"

class AGDDSimpleMaterial {
	friend std::ostream& operator <<(std::ostream&, const AGDDSimpleMaterial &);
public:
	AGDDSimpleMaterial(std::string n, double d):
		m_name(n),m_density(d),m_created(false),m_theMaterial(0)
	{
		m_mType=Undefined;
		RegisterToStore(this);
	}
	virtual ~AGDDSimpleMaterial() {}
	void RegisterToStore(AGDDSimpleMaterial*);
	std::string GetName();
	double GetDensity() {return m_density;}
	material_type GetMaterialType() {return m_mType;}
	void Created(bool b) {m_created=b;}
	bool Extant() {return m_created;}
	void* GetMaterial() {return m_theMaterial;}
	void SetMaterial(void* mat) {m_theMaterial=mat;}
protected:
	std::string m_name;
	double m_density;
	bool m_created;
	
	material_type m_mType;
	void* m_theMaterial;
};

#endif
