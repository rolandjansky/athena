/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/posXYZHandler.h"
#include "AGDDKernel/AGDDPositioner.h"
#include "AGDDHandlers/globals.h"
#include "GeoModelKernel/Units.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include <iostream>

posXYZHandler::posXYZHandler(std::string s):XMLHandler(s)
{
}

void posXYZHandler::ElementHandle()
{
	bool res=false;
	bool posRet=false;
	bool rotRet=false;
	std::string volume=getAttributeAsString("volume",res);
	std::string  sym=getAttributeAsString("sym",res);
    if (res) {
        MsgStream log(Athena::getMessageSvc(),"posXYZHandler");
        log<<MSG::INFO<<"ElementHandle() - symmetry implemented for "<<volume<<endmsg;
    }
	std::string sRot="";
	GeoTrf::Vector3D cvec(0,0,0);
	GeoTrf::Transform3D crot = GeoTrf::Transform3D::Identity();
	std::vector<double> X_Y_Z=getAttributeAsVector("X_Y_Z",posRet);
	if (posRet) cvec =GeoTrf::Vector3D(X_Y_Z[0],X_Y_Z[1],X_Y_Z[2]);
	std::vector<double> rot=getAttributeAsVector("rot",rotRet);
	if (rotRet) crot = crot*GeoTrf::RotateZ3D(rot[2]*GeoModelKernelUnits::degree)*GeoTrf::RotateY3D(rot[1]*GeoModelKernelUnits::degree)*GeoTrf::RotateX3D(rot[0]*GeoModelKernelUnits::degree);
	if (s_printFlag) {
		MsgStream log(Athena::getMessageSvc(),"posXYZHandler");
        log<<MSG::INFO<<"ElementHandle() - posXYV "<<volume<<endmsg;
		if (posRet) log<<MSG::INFO<<" pos= ("<<X_Y_Z[0]<<";"<<X_Y_Z[1]<<";"<<X_Y_Z[2]<<")"<<endmsg;
		if (rotRet) log<<MSG::INFO<<" rot= ("<<rot[0]<<";"<<rot[1]<<";"<<rot[2]<<")"<<endmsg;
	}
	AGDDPositioner *p=new AGDDPositioner(volume,GeoTrf::Translation3D(cvec)*crot);
	globals::currentPositioner=p;
}
