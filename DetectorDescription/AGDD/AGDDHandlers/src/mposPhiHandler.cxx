/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/mposPhiHandler.h"
#include "AGDDKernel/AGDDPositioner.h"
#include "GeoModelKernel/Units.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include <iostream>

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
			MsgStream log(Athena::getMessageSvc(),"mposPhiHandler");
			log<<MSG::WARNING<<"ElementHandle() - both icopy and dphi are nullptr!! volume "<<volume<<" cannot continue!"<<endmsg;
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
	
	GeoTrf::Transform3D crot = GeoTrf::Transform3D::Identity();
	
	vvv=getAttributeAsVector("rot",res);
	if (res) 
	{
		crot = crot*GeoTrf::RotateZ3D(vvv[2]*GeoModelKernelUnits::degree)*GeoTrf::RotateY3D(vvv[1]*GeoModelKernelUnits::degree)*GeoTrf::RotateX3D(vvv[0]*GeoModelKernelUnits::degree);
	}
	
	for (int i=0;i<icopy;i++)
	{
		GeoTrf::Vector2D position(rad,lateral_displacement);
		double phi=phi0+dphi*i;
		position = Eigen::Rotation2Dd(phi*GeoModelKernelUnits::degree)*position;

		double x=position.x();
		double y=position.y();
		
		GeoTrf::Vector3D cvec=GeoTrf::Vector3D(x,y,zpos);
		if (s!="false"&&i>0) crot = GeoTrf::RotateZ3D(dphi*GeoModelKernelUnits::degree)*crot;
		else if (s!="false"&&i==0) crot = GeoTrf::RotateZ3D(phi0*GeoModelKernelUnits::degree)*crot;

		p=new AGDDPositioner(volume,GeoTrf::Translation3D(cvec)*crot);
	}
}
