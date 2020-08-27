/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/posXYZHandler.h"
#include "AGDDKernel/AGDDPositioner.h"
#include "AGDDHandlers/globals.h"
#include "GeoModelKernel/Units.h"

#include <iostream>

#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"

posXYZHandler::posXYZHandler(std::string s):XMLHandler(s)
{
}

void posXYZHandler::ElementHandle()
{
	bool res,posRet,rotRet;
	std::string volume=getAttributeAsString("volume",res);
	std::string  sym=getAttributeAsString("sym",res);
	if (res) std::cout<<" symmetry implemented for "<<volume<<std::endl;
	std::string sRot="";
	CLHEP::Hep3Vector cvec;
	GeoTrf::Transform3D crot = GeoTrf::Transform3D::Identity();
	std::vector<double> X_Y_Z=getAttributeAsVector("X_Y_Z",posRet);
	if (posRet) 
	{
		cvec =CLHEP::Hep3Vector(X_Y_Z[0],X_Y_Z[1],X_Y_Z[2]);
	}
	std::vector<double> rot=getAttributeAsVector("rot",rotRet);
	if (rotRet) 
	{
		crot = crot*GeoTrf::RotateZ3D(rot[2]*GeoModelKernelUnits::degree)*GeoTrf::RotateY3D(rot[1]*GeoModelKernelUnits::degree)*GeoTrf::RotateX3D(rot[0]*GeoModelKernelUnits::degree);
	}
	if (s_printFlag) {
		std::cout<<"   posXYV "<<volume;
		if (posRet) std::cout<<" pos= ("<<X_Y_Z[0]<<";"<<X_Y_Z[1]<<";"<<X_Y_Z[2]<<")";
		if (rotRet) std::cout<<" rot= ("<<rot[0]<<";"<<rot[1]<<";"<<rot[2]<<")";
		std::cout<<std::endl;
	}
	AGDDPositioner *p=new AGDDPositioner(volume,Amg::CLHEPTranslationToEigen(cvec)*crot);
	globals::currentPositioner=p;
}
