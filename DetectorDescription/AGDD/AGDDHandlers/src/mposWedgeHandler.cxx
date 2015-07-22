/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/mposWedgeHandler.h"
#include "AGDDKernel/AGDDPositioner.h"
#include <iostream>

#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Geometry/Transform3D.h"

mposWedgeHandler::mposWedgeHandler(std::string s):XMLHandler(s),p(0)
{
}

void mposWedgeHandler::ElementHandle()
{
	bool res;
	std::string volume=getAttributeAsString("volume",res);
	int iWedge=getAttributeAsInt("wedge_number",8);
	std::vector<double> iSectors=getAttributeAsVector("sectors",res);
		
	double dWedge=360./iWedge;
	
	CLHEP::Hep3Vector cvec;
	CLHEP::HepRotation crot;

	const double degrad=M_PI/180.;
	
	double radius=0;
	for (int i=0;i<iWedge;i++)
	{
		double Wedge=dWedge*i;
		if ((int)iSectors[i]==0) continue;
	    CLHEP::Hep3Vector cvec;
	    CLHEP::HepRotation crot;
		crot.rotateZ(Wedge*degrad);
		double x=radius*cos(Wedge*degrad);
		double y=radius*sin(Wedge*degrad);
		double zpos=0;
		cvec=CLHEP::Hep3Vector(x,y,zpos);

		p=new AGDDPositioner(volume,crot,cvec);
	}
}
