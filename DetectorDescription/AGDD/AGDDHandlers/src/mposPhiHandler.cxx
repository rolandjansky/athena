/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/mposPhiHandler.h"
#include "AGDDKernel/AGDDPositioner.h"
#include <iostream>

#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/TwoVector.h"
#include "CLHEP/Geometry/Transform3D.h"

mposPhiHandler::mposPhiHandler(std::string s):XMLHandler(s),p(0)
{
}

void mposPhiHandler::ElementHandle()
{

	bool res;
	std::string volume=getAttributeAsString("volume");
	int icopy=getAttributeAsInt("ncopy",0);
	double phi0=getAttributeAsDouble("Phi0",res);
	if (!res) phi0=0;
	double dphi=getAttributeAsDouble("dPhi",res);
	
	if (!res) // dphi must be calculated 
	{
		if (icopy>0) 
			dphi=360./icopy;
		else
		{
			std::cout<<"mposPhiHandler: both icopy and dphi are null!! volume "<<volume<<" cannot continue!"<<std::endl;
			return;
		}
	}
	bool impliedRot;
	std::string s;
	std::string iR=getAttributeAsString("impliedRot",impliedRot);
	if (impliedRot) s=iR;
	double lateral_displacement=getAttributeAsDouble("S",res);
	if (!res) lateral_displacement=0;

	std::vector<double> vvv	= getAttributeAsVector("R_Z",res);
	double rad=0,zpos=0;
	if (res)
	{
		rad=vvv[0];
		zpos=vvv[1];
	}
	
	CLHEP::Hep3Vector cvec;
	CLHEP::HepRotation crot;
	
	vvv=getAttributeAsVector("rot",res);
	const double deg=M_PI/180.;
	if (res) 
	{
		crot.rotateX(vvv[0]*deg);
		crot.rotateY(vvv[1]*deg);
		crot.rotateZ(vvv[2]*deg);
	}

	const double degrad=M_PI/180.;
	
	for (int i=0;i<icopy;i++)
	{
		CLHEP::Hep2Vector position(rad,lateral_displacement);
	    CLHEP::Hep3Vector cvec;
//	    CLHEP::HepRotation crot;
		double phi=phi0+dphi*i;
		position.rotate(phi*degrad);

		double x=position.x();
		double y=position.y();
		
		cvec=CLHEP::Hep3Vector(x,y,zpos);
		if (s!="false"&&i>0) crot.rotateZ(dphi*degrad);
		if (s!="false"&&i==0) crot.rotateZ(phi0*degrad);

		p=new AGDDPositioner(volume,crot,cvec);
	}
}
