/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _MATERIAL_MANAGER_H_
#define _MATERIAL_MANAGER_H_
//---------------------------------------------------------//
//                                                         //
//  Not a very brilliant implementation.  We are planning  //
//  to provide the materials in some other way, once we    //
//  get going...                                           //
//                                                         //
//---------------------------------------------------------//
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoElement.h"
#include <string>
#include <vector>
class MaterialManager {
public:

	static const MaterialManager *GetMaterialManager()  {
		if (!instance) instance = new MaterialManager();
		return instance;
	}

	// Warning, until somebody does some work here, we will 
	// construct the detector out of nothing but thin air!
	const GeoMaterial *getMaterial (const std::string & string) const;

private:

    

	static MaterialManager *instance;
	static std::vector<GeoMaterial *> materials;

};
#endif

