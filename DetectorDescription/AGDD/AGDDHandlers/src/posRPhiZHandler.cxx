/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/posRPhiZHandler.h"
#include "AGDDControl/AGDDController.h"
#include "AGDDKernel/AGDDPositioner.h"
#include "GeoModelKernel/Units.h"

#include <iostream>
#include <vector>

posRPhiZHandler::posRPhiZHandler(const std::string& s,
                                 AGDDController& c)
  : XMLHandler(s, c)
{
}

void posRPhiZHandler::ElementHandle(AGDDController& c,
                                    xercesc::DOMNode *t)
{
	bool res=false;
	std::string volume=getAttributeAsString(c, t, "volume",res);
	std::vector<double> vvv=getAttributeAsVector(c, t, "R_Phi_Z",res);
	double radius=vvv[0];
	double phi=vvv[1];
	double zpos=vvv[2];
	GeoTrf::Transform3D crot = GeoTrf::Transform3D::Identity();

        vvv=getAttributeAsVector(c, t, "rot",res);
        if (res) 
        {
                crot = crot*GeoTrf::RotateZ3D(vvv[2]*GeoModelKernelUnits::degree)*GeoTrf::RotateY3D(vvv[1]*GeoModelKernelUnits::degree)*GeoTrf::RotateX3D(vvv[0]*GeoModelKernelUnits::degree);
        }

	crot = GeoTrf::RotateZ3D(phi*GeoModelKernelUnits::degree)*crot;
	double x=radius*std::cos(phi*GeoModelKernelUnits::degree);
	double y=radius*std::sin(phi*GeoModelKernelUnits::degree);
	GeoTrf::Vector3D cvec=GeoTrf::Vector3D(x,y,zpos);

	new AGDDPositioner(c.GetPositionerStore(),
                           c.GetVolumeStore(),
                           volume,GeoTrf::Translation3D(cvec)*crot);
}
