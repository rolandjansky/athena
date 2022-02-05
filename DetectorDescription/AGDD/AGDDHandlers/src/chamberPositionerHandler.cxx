/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/chamberPositionerHandler.h"
#include "AGDDKernel/AGDDDetectorStore.h"
#include "AGDDKernel/AGDDDetectorPositioner.h"
#include "AGDDKernel/AGDDDetector.h"
#include "AGDDControl/AGDDController.h"
#include "GeoModelKernel/Units.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include <iostream>

chamberPositionerHandler::chamberPositionerHandler(const std::string& s,
                                                   AGDDController& c)
  : XMLHandler(s, c)
{
}

void chamberPositionerHandler::ElementHandle(AGDDController& c,
                                             xercesc::DOMNode *t)
{
	std::string volume=getAttributeAsString(c, t, "volume");

	AGDDDetector* mCham=c.GetDetectorStore().GetDetector(volume);
	std::string subType;
	if (!mCham) 
	{
		MsgStream log(Athena::getMessageSvc(),"chamberPositionerHandler");
		log<<MSG::WARNING<<"ElementHandle() - Could not retrieve volume "<<volume<<" from DetectorStore!"<<endmsg;
		return;
	}
	else
		subType=mCham->subType();
	
	double radius=getAttributeAsDouble(c, t, "radius");
	double zPos=getAttributeAsDouble(c, t, "zPos");
	
	double phi0=getAttributeAsDouble(c, t, "phi0",0.);
	int iWedge=getAttributeAsInt(c, t, "wedge_number",8);
	
	std::string zLayout=getAttributeAsString(c, t, "zLayout","Z_SYMMETRIC");
	std::string type=getAttributeAsString(c, t, "type","BARREL");
	std::string chType=getAttributeAsString(c, t, "chamberType");
	std::string iSectors=getAttributeAsString(c, t, "iSectors","11111111");
	std::string detectorType=getAttributeAsString(c, t, "detectorType","MDT");
	int etaIndex=getAttributeAsInt(c, t, "etaIndex",0);
	
	double dPhi=360./iWedge;
	if (iSectors.size()!= (unsigned int) iWedge) std::abort();

 	for (int i=0;i<iWedge;i++)
 	{
 		if (iSectors[i]=='0') continue;
		if (zLayout!="Z_NEGATIVE")
		{
			double Wedge=dPhi*i+phi0;
			GeoTrf::Transform3D crot = GeoTrf::Transform3D::Identity();
			if (type=="ENDCAP") 
			{
				//	fix to ensure right order of planes			
				crot = crot*GeoTrf::RotateZ3D(Wedge*GeoModelKernelUnits::degree)*GeoTrf::RotateY3D(90*GeoModelKernelUnits::degree)*GeoTrf::RotateZ3D(180.*GeoModelKernelUnits::degree);
			}
			else crot = crot*GeoTrf::RotateZ3D(Wedge*GeoModelKernelUnits::degree);
 			double x=radius*std::cos(Wedge*GeoModelKernelUnits::degree);
 			double y=radius*std::sin(Wedge*GeoModelKernelUnits::degree);
 			double zpos=zPos;
 			GeoTrf::Vector3D cvec=GeoTrf::Vector3D(x,y,zpos);
 			AGDDDetectorPositioner *p __attribute__((__unused__));
 			p=new AGDDDetectorPositioner(c.GetPositionerStore(),c.GetVolumeStore(),volume,GeoTrf::Translation3D(cvec)*crot);
			p->SensitiveDetector(true);
			p->ID.phiIndex=i;
			p->ID.sideIndex=1;
			p->ID.etaIndex=etaIndex;
			p->ID.detectorType=detectorType;
			
			p->position.Zposition=zpos;
			p->position.Radius=radius;
			p->position.PhiStart=phi0;
			p->position.Phi=Wedge;
			
			mCham->SetAddressAndPosition(p);
			
		}
		if (zLayout!="Z_POSITIVE")
                {
                  double Wedge=dPhi*i+phi0;
                  GeoTrf::Transform3D crot = GeoTrf::Transform3D::Identity();
                  if (type=="ENDCAP")
                  {
				//	fix to ensure right order of planes			
                    crot = crot*GeoTrf::RotateX3D(180.*GeoModelKernelUnits::degree)*GeoTrf::RotateZ3D(-Wedge*GeoModelKernelUnits::degree)*GeoTrf::RotateY3D(90*GeoModelKernelUnits::degree)*GeoTrf::RotateZ3D(180.*GeoModelKernelUnits::degree);
                  }
                  else crot = crot*GeoTrf::RotateZ3D(Wedge*GeoModelKernelUnits::degree);
                  double x=radius*std::cos(Wedge*GeoModelKernelUnits::degree);
                  double y=radius*std::sin(Wedge*GeoModelKernelUnits::degree);
                  double zpos=zPos;
                  GeoTrf::Vector3D cvec=GeoTrf::Vector3D(x,y,-zpos);
                  AGDDDetectorPositioner *p __attribute__((__unused__));
                  p=new AGDDDetectorPositioner(c.GetPositionerStore(),c.GetVolumeStore(),volume,GeoTrf::Translation3D(cvec)*crot);
                  p->SensitiveDetector(true);
                  p->ID.phiIndex=i;
                  p->ID.sideIndex=-1;
                  p->ID.etaIndex=-etaIndex;
                  p->ID.detectorType=detectorType;
			
                  p->position.Zposition=-zpos;
                  p->position.Radius=radius;
                  p->position.PhiStart=phi0;
                  p->position.Phi=Wedge;
			
                  mCham->SetAddressAndPosition(p);
                }
 	}

}
