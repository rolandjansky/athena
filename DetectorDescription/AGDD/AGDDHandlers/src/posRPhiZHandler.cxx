/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/posRPhiZHandler.h"
#include "AGDDKernel/AGDDPositioner.h"
#include "GeoModelKernel/Units.h"

#include <iostream>
#include <vector>

posRPhiZHandler::posRPhiZHandler(std::string s):XMLHandler(s),p(0)
{
}

void posRPhiZHandler::ElementHandle()
{
	bool res=false;
	std::string volume=getAttributeAsString("volume",res);
	std::vector<double> vvv=getAttributeAsVector("R_Phi_Z",res);
	double radius=vvv[0];
	double phi=vvv[1];
	double zpos=vvv[2];
	GeoTrf::Transform3D crot = GeoTrf::Transform3D::Identity();

        vvv=getAttributeAsVector("rot",res);
        if (res) 
        {
                crot = crot*GeoTrf::RotateZ3D(vvv[2]*GeoModelKernelUnits::degree)*GeoTrf::RotateY3D(vvv[1]*GeoModelKernelUnits::degree)*GeoTrf::RotateX3D(vvv[0]*GeoModelKernelUnits::degree);
        }

	crot = GeoTrf::RotateZ3D(phi*GeoModelKernelUnits::degree)*crot;
	double x=radius*std::cos(phi*GeoModelKernelUnits::degree);
	double y=radius*std::sin(phi*GeoModelKernelUnits::degree);
	GeoTrf::Vector3D cvec=GeoTrf::Vector3D(x,y,zpos);

	p=new AGDDPositioner(volume,GeoTrf::Translation3D(cvec)*crot);
}
