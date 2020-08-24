/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/mposWedgeHandler.h"
#include "AGDDKernel/AGDDPositioner.h"
#include "GeoModelKernel/Units.h"

#include <iostream>
#include <vector>

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

		p=new AGDDPositioner(volume,GeoTrf::Translation3D(cvec)*crot);
	}
}
