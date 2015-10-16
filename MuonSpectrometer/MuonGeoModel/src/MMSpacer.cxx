/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MuonGeoModel/MMSpacer.h"
#include "MuonGeoModel/MMSpacer_Technology.h"
#include "MuonGeoModel/Station.h"
#include "MuonGeoModel/MYSQL.h"
#include "MuonGeoModel/MMSpacerComponent.h"
#include "MuonGeoModel/Cutout.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoSerialDenominator.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoSerialIdentifier.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
// for cutouts:
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoShapeIntersection.h"
#include "GeoModelKernel/GeoShapeShift.h"

namespace MuonGM {

MMSpacer::MMSpacer(Component* ss): DetectorElement(ss->name)
{
  MMSpacerComponent* s = (MMSpacerComponent*)ss;
  m_component = s;
  width = s->dx1;
  longWidth = s->dx2;
  length = s->dy;
  name=s->name;
  thickness = s->GetThickness();
  index = s->index;
}


GeoPhysVol* MMSpacer::build(int minimalgeo)
{
  // std::cout<<"this is MMSpacer::build(int minimalgeo) "<<std::endl;
  std::vector<Cutout*> vcutdef;
  int cutoutson = 0;
  return build(minimalgeo, cutoutson, vcutdef);
}


GeoPhysVol* 
MMSpacer::build(int minimalgeo, int , std::vector<Cutout*> )
{
  // std::cout<<"this is MMSpacer::build "<<std::endl;
  MYSQL* mysql = MYSQL::GetPointer();	
  // std::cout<<"fetching technology "<<std::endl;
  MMSpacer_Technology* t = (MMSpacer_Technology*) mysql->GetTechnology(name);
  thickness = t->Thickness();

  // Build Micromegas mother volume out of G10
  // std::cout<<"creating spacer "<<thickness<<" "<<width<<" "<<longWidth<<" "<<length<<std::endl;
  const GeoShape* strd = new GeoTrd(thickness/2, thickness/2, width/2, 
                                    longWidth/2, length/2);
									
  if (t->lowZCutOuts)
  {
  	double dy=t->lowZCutOutWidth;
	double dz=t->lowZCutOutDZ;
	const GeoShape* sbox = new GeoTrd(thickness,thickness,dy,dy,dz);
	HepGeom::Transform3D cut1,cut2;
	cut1=HepGeom::Translate3D(0.0, -width/2., -length/2.);
	cut2=HepGeom::Translate3D(0.0, width/2., -length/2.);
	strd= &(strd->subtract( (*sbox) << cut1 ));
	strd= &(strd->subtract( (*sbox) << cut2 ));
  }
  if (t->highZCutOuts)
  {
  	double dy=t->highZCutOutWidth;
	double dz=t->highZCutOutDZ;
	const GeoShape* sbox = new GeoTrd(thickness,thickness,dy,dy,dz);
	HepGeom::Transform3D cut1,cut2;
	cut1=HepGeom::Translate3D(0.0, -longWidth/2., length/2.);
	cut2=HepGeom::Translate3D(0.0, longWidth/2., length/2.);
	strd= &(strd->subtract( (*sbox) << cut1 ));
	strd= &(strd->subtract( (*sbox) << cut2 ));
  }


  const GeoMaterial* mtrd = matManager->getMaterial("muo::Honeycomb");
  GeoLogVol* ltrd = new GeoLogVol(logVolName, strd, mtrd);
  GeoPhysVol* ptrd = new GeoPhysVol(ltrd);

  if (minimalgeo == 1) return ptrd;
        
  return ptrd;	
}


void MMSpacer::print()
{
  std::cout << " MMSpacer " << name << " :" << std::endl;
}

} // namespace MuonGM
