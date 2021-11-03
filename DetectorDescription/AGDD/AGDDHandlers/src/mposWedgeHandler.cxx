/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/mposWedgeHandler.h"
#include "AGDDKernel/AGDDPositioner.h"
#include "AGDDControl/AGDDController.h"
#include "GeoModelKernel/Units.h"

#include <iostream>
#include <vector>

mposWedgeHandler::mposWedgeHandler(const std::string& s,
                                   AGDDController& c)
  : XMLHandler(s, c)
{
}

void mposWedgeHandler::ElementHandle(AGDDController& c,
                                     xercesc::DOMNode *t)
{
	bool res;
	std::string volume=getAttributeAsString(c, t, "volume",res);
	int iWedge=getAttributeAsInt(c, t, "wedge_number",8);
	std::vector<double> iSectors=getAttributeAsVector(c, t, "sectors",res);

	double dWedge=360./iWedge;
	
	double radius=0;
	for (int i=0;i<iWedge;i++)
	{
		double Wedge=dWedge*i;
		if ((int)iSectors[i]==0) continue;
                GeoTrf::Transform3D crot = GeoTrf::RotateZ3D(Wedge*GeoModelKernelUnits::degree);
		double x=radius*std::cos(Wedge*GeoModelKernelUnits::degree);
		double y=radius*std::sin(Wedge*GeoModelKernelUnits::degree);
		double zpos=0;
		GeoTrf::Vector3D cvec=GeoTrf::Vector3D(x,y,zpos);

		new AGDDPositioner(c.GetPositionerStore(),
                                   c.GetVolumeStore(),
                                   volume,GeoTrf::Translation3D(cvec)*crot);
	}
}
