/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/Spacer.h"
#include "MuonGeoModel/Station.h"
#include "MuonGeoModel/MYSQL.h"
#include "MuonGeoModel/Technology.h"
#include "MuonGeoModel/SpaComponent.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoVPhysVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoSerialDenominator.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoSerialIdentifier.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GaudiKernel/SystemOfUnits.h"
// for cutouts:
#include "GeoModelKernel/GeoShapeSubtraction.h"

#define skip_spacer false

namespace MuonGM {

Spacer::Spacer(Component *ss): DetectorElement(ss->name)
{
  SpaComponent* s = dynamic_cast<SpaComponent*>(ss);
  width = s->dx1;
  longWidth = s->dx2;
  MYSQL* mysql = MYSQL::GetPointer();
  thickness = mysql->GetTechnology(s->name)->thickness;
  length = s->dy;
  m_component = *s;	
}


GeoVPhysVol* Spacer::build()
{
  int cutoutson = 0;
  return build(cutoutson);
}

GeoVPhysVol * Spacer::build(int /*cutoutson*/)
{
  if (name == "SPA06" || name == "SPA01") {
    double excent = m_component.excent;
    double maxwLength = m_component.maxwdy;
         
    const GeoShape* strd = new GeoTrd(thickness/2.,thickness/2.,
                                      width/2., longWidth/2., maxwLength/2.);
    // std::cout<< "build SPA06 or SPA01: trapezoid "<<thickness<<" "<<thickness<<" "
    //          << width << " " << longWidth << " " << maxwLength << std::endl;
    double upWidth = 0.;
    if ( excent > length ) upWidth = longWidth*(excent-length)/(excent-maxwLength);
//          std::cout<<" SPA6: width, lwidth, length, excent, maxwlength, upWidth "
//                   <<width<<" "<<longWidth<<" "<<length<<" "
//                   <<excent<<" "<<maxwLength<<" "<<upWidth<<std::endl;
    if (excent > length && (length-maxwLength) > 0 ){
             //std::cout<<" Here is an exagonal SPA ***** named "<<name<<" excent = "
             //         <<excent<<" length = "<<length<<std::endl;
      GeoTrd* upTrd   = new GeoTrd(thickness/2.,thickness/2., longWidth/2., upWidth/2.,
                                   (length-maxwLength)/2.);		
      strd = & ( (strd->add(  (*upTrd) << GeoTrf::TranslateZ3D( length/2. )) )
                 << GeoTrf::TranslateZ3D( (maxwLength - length)/2.) );
    }

    const GeoMaterial* mtrd = matManager->getMaterial("std::Aluminium");
    const GeoLogVol* lspa = new GeoLogVol("CSCspacer", strd, mtrd);
    GeoPhysVol* pspa = new GeoPhysVol(lspa);
    return pspa;

  } else {
    const GeoShape* strd = new GeoTrd(thickness/2, thickness/2,
                                      width/2, longWidth/2, length/2);
    const GeoMaterial* mtrd = matManager->getMaterial("std::Air");
    GeoLogVol* ltrd = new GeoLogVol("Spacer", strd, mtrd);
    GeoPhysVol* ptrd = new GeoPhysVol(ltrd);
	
    double tckibeam = thickness/5.;
    double dx = tckibeam;
    double dy = 3.*tckibeam;
	  
    GeoVPhysVol *ptrdtemp=NULL;

    GeoTrd* strd1 = new GeoTrd(dx/2, dx/2, dy/2, dy/2, length/2);
    const GeoMaterial* mtrd1 = matManager->getMaterial("std::Aluminium");
    GeoLogVol* ltrd1 = new GeoLogVol("ibeam1", strd1, mtrd1);
    GeoPhysVol* ptrd1 = new GeoPhysVol(ltrd1);
  
    GeoTrd* strd2 = new GeoTrd(dy/2, dy/2, dx/2, dx/2, length/2);
    const GeoMaterial* mtrd2 = matManager->getMaterial("std::Aluminium");
    GeoLogVol* ltrd2 = new GeoLogVol("ibeam2", strd2, mtrd2);
    GeoPhysVol* ptrd2 = new GeoPhysVol(ltrd2);

    GeoSerialDenominator* sd = new GeoSerialDenominator(name + " ibeam");
    if (!skip_spacer) {
      ptrd->add(sd);
      ptrd->add(new GeoSerialIdentifier(0));
    }
    double newpos=-thickness/2.;
    double wherepos=newpos;
    double wherewidth=-width/2.;
    for (int j = 0; j < 3; j++) {
      for (int i = 0; i < 3; i++) {
        ptrdtemp = ptrd1;
        if (i == 1){
          ptrdtemp = ptrd2;
          tckibeam = dy;
        }
        GeoTransform* xf = new GeoTransform(GeoTrf::Translate3D(wherepos + tckibeam/2, wherewidth+dy/2, 0));
        if (!skip_spacer) {
          ptrd->add(xf);
          ptrd->add(ptrdtemp);
        }
        wherepos += tckibeam;
        tckibeam = dx;
      }
      wherepos -= thickness;
      wherewidth += (width/2.-dy/2.);
    }
	
    double vtubl=(width-5*tckibeam)/2.;

    GeoSerialDenominator* ntube = new GeoSerialDenominator(name + " vbeam");
    if (!skip_spacer){
      ptrd->add(ntube);
      ptrd->add(new GeoSerialIdentifier(0));
    }

    for (int k1 = 0; k1 < 2; k1++){
      for (int k = 0; k < 2; k++){
        GeoTransform* ttube = new GeoTransform(GeoTrf::RotateX3D(-90*Gaudi::Units::deg)* GeoTrf::Translate3D(
                                               0.,
                                               -(vtubl+tckibeam)/2.-(k-1)*(vtubl+tckibeam),
                                               -length/4.-(k1-1)*length/2));
        if (!skip_spacer) ptrd->add(ttube);
      }
    }
        
    return ptrd;	
  }
    
}

void Spacer::print()
{
	std::cout<<"Spacer "<<name<<" :"<<std::endl;
}
                                                                                
} // namespace MuonGM
