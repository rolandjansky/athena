/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2Geo/chamberPositionerHandler.h"
#include "AGDD2Geo/XercesParser.h"
#include "AGDD2Geo/AGDDPositioner.h"
#include <iostream>

#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Geometry/Transform3D.h"


chamberPositionerHandler::chamberPositionerHandler(std::string s):XMLHandler(s)
{
}

void chamberPositionerHandler::ElementHandle()
{
	bool res;
	std::string volume=getAttributeAsString("volume",res);
	double radius=getAttributeAsDouble("radius",res);
	double zPos=getAttributeAsDouble("zPos",res);
	
	double phi0=getAttributeAsDouble("phi0",0.);
	int iWedge=getAttributeAsInt("wedge_number",8);
	std::string zLayout=getAttributeAsString("zLayout","Z_SYMMETRIC");
	std::string type=getAttributeAsString("type","BARREL");
	std::string iSectors=getAttributeAsString("iSectors","11111111");
	
	double dPhi=360./iWedge;
	

	const double pi=M_PI;
	const double degrad=pi/180.;

	assert (iSectors.size()==iWedge);
	
//     vvv=getAttributeAsVector("rot",res);
//     if (res) 
//     {
//        crot=CLHEP::HepRotation();
//        crot.rotateX(vvv[0]*degrad);
//        crot.rotateY(vvv[1]*degrad);
//        crot.rotateZ(vvv[2]*degrad);
//     }
	
 	for (int i=0;i<iWedge;i++)
 	{
 		double Wedge=dPhi*i+phi0;
 		if (iSectors[i]=='0') continue;
		if (zLayout!="Z_NEGATIVE")
		{
			CLHEP::Hep3Vector cvec;
			CLHEP::HepRotation crot;
			if (type=="ENDCAP") 
			{
				crot.rotateY(90*degrad);
				crot.rotateZ(Wedge*degrad);
			}
			else 
				crot.rotateZ(Wedge*degrad);
 			double x=radius*cos(Wedge*degrad);
 			double y=radius*sin(Wedge*degrad);
 			double zpos=zPos;
 			cvec=CLHEP::Hep3Vector(x,y,zpos);
 			AGDDPositioner *p __attribute__((__unused__));
 			p=new AGDDPositioner(volume,crot,cvec);
		}
		if (zLayout!="Z_POSITIVE")
                {
                        CLHEP::Hep3Vector cvec;
                        CLHEP::HepRotation crot;
                        if (type=="ENDCAP")
                        {
                                crot.rotateY(90*degrad);
                                crot.rotateZ(Wedge*degrad);
				crot.rotateX(180.*degrad);
                        }
                        else
                                crot.rotateZ(Wedge*degrad);
                        double x=radius*cos(Wedge*degrad);
                        double y=radius*sin(Wedge*degrad);
                        double zpos=zPos;
                        cvec=CLHEP::Hep3Vector(x,y,-zpos);
                        AGDDPositioner *p __attribute__((__unused__));
                        p=new AGDDPositioner(volume,crot,cvec);
                }
 	}

}
