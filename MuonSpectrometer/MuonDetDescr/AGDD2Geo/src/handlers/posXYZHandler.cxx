/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2Geo/posXYZHandler.h"
#include "AGDD2Geo/XercesParser.h"
#include "AGDD2Geo/AGDDPositioner.h"
#include "AGDD2Geo/globals.h"
#include <iostream>

#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Geometry/Transform3D.h"

posXYZHandler::posXYZHandler(std::string s):XMLHandler(s)
{
//	std::cout<<"Creating handler for posXYZ"<<std::endl;
}

void posXYZHandler::ElementHandle()
{
//	std::cout<<"handling for posXYZ";
	bool res,posRet,rotRet;
	std::string volume=getAttributeAsString("volume",res);
	std::string  sym=getAttributeAsString("sym",res);
	if (res) msgLog()<<MSG::ERROR<<" symmetry implemented for "<<volume<<endreq;
	if (msgLog().level()<=MSG::DEBUG)
	msgLog()<<MSG::DEBUG<<"\tposXYZ volume "<<volume;
	std::string sRot="";
	CLHEP::Hep3Vector cvec;
	CLHEP::HepRotation crot;
	std::vector<double> X_Y_Z=getAttributeAsVector("X_Y_Z",posRet);
	if (posRet) 
	{
		if (msgLog().level()<=MSG::DEBUG)
	           msgLog()<<MSG::DEBUG<<" X_Y_Z "<<X_Y_Z;
		cvec =CLHEP::Hep3Vector(X_Y_Z[0],X_Y_Z[1],X_Y_Z[2]);
	}
	std::vector<double> rot=getAttributeAsVector("rot",rotRet);
	const double pi=2*asin(1.);
	const double deg=pi/180.;
	if (rotRet) 
	{
		if (msgLog().level()<=MSG::DEBUG)
		msgLog()<<MSG::DEBUG<<" rot "<<rot;
		crot=CLHEP::HepRotation();
		crot.rotateX(rot[0]*deg);
		crot.rotateY(rot[1]*deg);
		crot.rotateZ(rot[2]*deg);
	}
	if (printFlag) {
		std::cout<<"   posXYV "<<volume;
		if (posRet) std::cout<<" pos= ("<<X_Y_Z[0]<<";"<<X_Y_Z[1]<<";"<<X_Y_Z[2]<<")";
		if (rotRet) std::cout<<" rot= ("<<rot[0]<<";"<<rot[1]<<";"<<rot[2]<<")";
		std::cout<<std::endl;
	}
	if (msgLog().level()<=MSG::DEBUG)
	msgLog()<<MSG::DEBUG<<endreq;
	AGDDPositioner *p=new AGDDPositioner(volume,crot,cvec);
	globals::currentPositioner=p;
}
