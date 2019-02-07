/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/Cutout.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoPara.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoTrap.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GaudiKernel/SystemOfUnits.h"

namespace MuonGM {

Cutout::Cutout()
{
    dx=dy=widthXs=widthXl=lengthY=excent=dead1=thickness=0.;
    ijob=subtype=icut=0;
}

Cutout::Cutout(const Cutout& c)
{
  dx=c.dx;
  dy=c.dy;
  widthXs=c.widthXs;
  widthXl=c.widthXl;
  lengthY=c.lengthY;
  excent=c.excent;
  dead1=c.dead1;
  ijob=c.ijob;
  subtype=c.subtype;
  icut=c.icut;
  thickness = c.thickness;
}

void Cutout::setThickness(double compThickness)
{
  thickness = compThickness;
}

const GeoShape* Cutout::build()
{
  // GeoModel x = chamber thickness (amdb z), y=width (amdb x)
  //          z= chamber length (amdb y)
  //  cutout widthXf parallel chamber width, cutout thickness
  //    = component thickness, cutout lengthY parallel chamber L.
  // position it with its intrinsic position info:
  double zpos = dy+lengthY/2.;
//  std::cout << "translate 0, "<<dx<<", "<<zpos<<std::endl;
//  std::cout << "widthXl="<<widthXl<<" widthXs="<<widthXs<<" lengthY="
//	    <<lengthY<<" excent="<<excent<<" dead1="<<dead1<<std::endl;
  GeoTrf::Transform3D xfTemp = GeoTrf::Translate3D(0.,dx,zpos);
  const GeoShape *sCutout;
  /*
  // This is just to make sure we are putting stuff in the right place:
  GeoBox *cutoutbox = new GeoBox(thickness/2.,widthXl/2.,lengthY/2.);
  sCutout = & ( (*cutoutbox) <<xfTemp);
  */
  // This is the proper way to do it, but not sure if complicated working...
  if (widthXl==widthXs && dead1 == 0.)
    {
      GeoBox *cutoutbox = new GeoBox(thickness/2.,widthXs/2.,lengthY/2.);
      sCutout = & ( (*cutoutbox) <<xfTemp);
      cutoutbox->ref();
      cutoutbox->unref();
    }
  else if (dead1 ==0.)
    {
      GeoTrd *cutouttrd = new GeoTrd(thickness/2., thickness/2.,
				     widthXs/2., widthXl/2.,
				     lengthY/2.);
      sCutout = & ( (*cutouttrd) <<xfTemp);
    }
  else if (widthXl==widthXs)
    {
      // angle between length-axis and HV/RO ends of chamber:
      double alpha = atan(2.*excent/lengthY);
      // polar and azimuthal angles of vector describing offset of
      //   cutout planes:
      double theta = -dead1*Gaudi::Units::degree;
      double phi = -90.*Gaudi::Units::degree;
      // GeoPara requires the +/- z faces be parallel to the x-y plane,
      //   so choose x = width, y=length, z=thickness:
      GeoPara *cutoutpara = new GeoPara(widthXs/2.,lengthY/2.,thickness/2.,
					alpha,theta,phi);
      // now rotate it so thickness is x-axis, width is y-axis, length z-axis:
      GeoTrf::Transform3D xRot = GeoTrf::RotateX3D(-90.*Gaudi::Units::degree)*
	GeoTrf::RotateY3D(-90.*Gaudi::Units::degree);
      xfTemp = xfTemp * xRot;
      sCutout = & ( (*cutoutpara) <<xfTemp);
      cutoutpara->ref();
      cutoutpara->unref();
    }
  else  
    {
      GeoTrap *cutouttrap = new GeoTrap(thickness/2.,dead1*Gaudi::Units::degree,90.*Gaudi::Units::degree,
					excent,widthXs/2.,widthXl/2.,
					atan((2.*excent+(widthXl-widthXs)/2.)/
					     lengthY),
					excent,widthXs/2.,widthXl/2.,
					atan((2.*excent+(widthXl-widthXs)/2.)/
					     lengthY)
					);
      // now rotate it so thickness is x-axis, width is y-axis, length z-axis:
      GeoTrf::Transform3D xRot = GeoTrf::RotateX3D(-90.*Gaudi::Units::degree)*
	GeoTrf::RotateY3D(-90.*Gaudi::Units::degree);
      xfTemp = xfTemp * xRot;
      sCutout = & ( (*cutouttrap) <<xfTemp);
      cutouttrap->ref();
      cutouttrap->unref();
    }
  
  return sCutout;
}


std::ostream& operator<<(std::ostream &os, const Cutout& p)
{
	os<<" Cutout: "
          <<" x/y/width_s/width_l/length/excent/dead1: "
          <<p.dx<<" "
          <<p.dy<<" "
          <<p.widthXs<<" "
          <<p.widthXl<<" "
          <<p.lengthY<<" "
          <<p.excent<<" "
          <<p.dead1
	  <<" component index="<<p.ijob;
        
	return os;
}
} // namespace MuonGM
