/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/posRPhiZHandler.h"
#include "AGDDKernel/AGDDPositioner.h"
#include "GeoModelKernel/Units.h"

#include <iostream>

#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Geometry/Transform3D.h"

posRPhiZHandler::posRPhiZHandler(std::string s):XMLHandler(s),p(0)
{
}

void posRPhiZHandler::ElementHandle()
{
	bool res;
	std::string volume=getAttributeAsString("volume",res);
	std::vector<double> vvv=getAttributeAsVector("R_Phi_Z",res);
	
	double radius,phi,zpos;

	radius=vvv[0];
	phi=vvv[1];
	zpos=vvv[2];
	
	CLHEP::Hep3Vector cvec;
	CLHEP::HepRotation crot;

        vvv=getAttributeAsVector("rot",res);
        if (res) 
        {
                crot=CLHEP::HepRotation();
                crot.rotateX(vvv[0]*GeoModelKernelUnits::degree);
                crot.rotateY(vvv[1]*GeoModelKernelUnits::degree);
                crot.rotateZ(vvv[2]*GeoModelKernelUnits::degree);
        }

	crot.rotateZ(phi*GeoModelKernelUnits::degree);
	double x=radius*cos(phi*GeoModelKernelUnits::degree);
	double y=radius*sin(phi*GeoModelKernelUnits::degree);
	cvec=CLHEP::Hep3Vector(x,y,zpos);

	p=new AGDDPositioner(volume,crot,cvec);
}
