/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/chamberPositionerHandler.h"
#include "AGDDKernel/AGDDDetectorPositioner.h"
#include "AGDDKernel/AGDDVolumeStore.h"
#include "AGDDKernel/AGDDVolume.h"
#include "AGDDKernel/AGDDDetector.h"
#include <iostream>

#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Geometry/Transform3D.h"


chamberPositionerHandler::chamberPositionerHandler(std::string s):XMLHandler(s)
{
}

void chamberPositionerHandler::ElementHandle()
{
	std::string volume=getAttributeAsString("volume");
	
	AGDDVolume* theVol=AGDDVolumeStore::GetVolumeStore()->GetVolume(volume);
	AGDDDetector* mCham=dynamic_cast<AGDDDetector*>(theVol);
	std::string subType;
	if (!mCham) 
	{
		std::cout<<"chamberPositionerHandler: something wrong! returning..."<<std::endl;
		return;
	}
	else
		subType=mCham->subType();
	
	double radius=getAttributeAsDouble("radius");
	double zPos=getAttributeAsDouble("zPos");
	
	double phi0=getAttributeAsDouble("phi0",0.);
	int iWedge=getAttributeAsInt("wedge_number",8);
	
	mCham->position.Zposition=zPos;
	mCham->position.Radius=radius;
	mCham->position.PhiStart=phi0;
	
	std::string zLayout=getAttributeAsString("zLayout","Z_SYMMETRIC");
	std::string type=getAttributeAsString("type","BARREL");
	std::string chType=getAttributeAsString("chamberType");
	std::string iSectors=getAttributeAsString("iSectors","11111111");
	std::string detectorType=getAttributeAsString("detectorType","MDT");
	int etaIndex=getAttributeAsInt("etaIndex",0);
	
	double dPhi=360./iWedge;
	

	const double pi=M_PI;
	const double degrad=pi/180.;

	if (iSectors.size()!= (unsigned int) iWedge) throw;
	
	// std::cout<<" =============>> this is chamberPositionerHandler::ElementHandle() "<<theVol->GetName()<<" "<<subType<<std::endl;
	
 	for (int i=0;i<iWedge;i++)
 	{
 		if (iSectors[i]=='0') continue;
		if (zLayout!="Z_NEGATIVE")
		{
			double Wedge=dPhi*i+phi0;
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
 			AGDDDetectorPositioner *p __attribute__((__unused__));
 			p=new AGDDDetectorPositioner(volume,crot,cvec);
			p->SensitiveDetector(true);
			p->ID.phiIndex=i;
			p->ID.sideIndex=1;
			p->ID.etaIndex=etaIndex;
			p->ID.detectorType=detectorType;
			mCham->SetAddressAndPosition(p);
		}
		if (zLayout!="Z_POSITIVE")
        {
			double Wedge=dPhi*i+phi0;
            CLHEP::Hep3Vector cvec;
            CLHEP::HepRotation crot;
            if (type=="ENDCAP")
            {
                crot.rotateY(90*degrad);
                crot.rotateZ(-Wedge*degrad);
				crot.rotateX(180.*degrad);
            }
            else
                crot.rotateZ(Wedge*degrad);
            double x=radius*cos(Wedge*degrad);
            double y=radius*sin(Wedge*degrad);
            double zpos=zPos;
            cvec=CLHEP::Hep3Vector(x,y,-zpos);
            AGDDDetectorPositioner *p __attribute__((__unused__));
            p=new AGDDDetectorPositioner(volume,crot,cvec);
			p->SensitiveDetector(true);
			p->ID.phiIndex=i;
			p->ID.sideIndex=-1;
			p->ID.etaIndex=-etaIndex;
			p->ID.detectorType=detectorType;
			mCham->SetAddressAndPosition(p);
        }
 	}

}
