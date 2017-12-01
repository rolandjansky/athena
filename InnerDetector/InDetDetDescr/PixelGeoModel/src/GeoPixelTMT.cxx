/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Build The TMT.
// This is built one time per layer. 
// Each layer is given slightly different mass due to differences
// in the measured mass.

#include "PixelGeoModel/GeoPixelTMT.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoTrap.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"

#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeUnion.h"

#include <algorithm>
using std::max;

GeoPixelTMT::GeoPixelTMT()
{
  m_physVol = Build();
}

GeoVPhysVol* GeoPixelTMT::Build() {
  const GeoShape * lastShape = 0;
  double totVolume = 0;
  HepGeom::RotateX3D traprot(180.*CLHEP::deg);

  int halfNModule = m_gmt_mgr->PixelNModule()/2;

  for (int ii = 0; ii < m_gmt_mgr->PixelTMTNumParts(); ii++) {
    double z1 =  m_gmt_mgr->PixelTMTPosZ1(ii);
    double z2 =  m_gmt_mgr->PixelTMTPosZ2(ii);
    double xbase1 =  m_gmt_mgr->PixelTMTBaseX1(ii);
    double xbase2 =  m_gmt_mgr->PixelTMTBaseX2(ii);
    double w1 =  m_gmt_mgr->PixelTMTWidthX1(ii);
    double w2 =  m_gmt_mgr->PixelTMTWidthX2(ii);
    double widthy   = m_gmt_mgr->PixelTMTWidthY(ii);
    double ypos = m_gmt_mgr->PixelTMTPosY(ii);
    bool   perModule = m_gmt_mgr->PixelTMTPerModule(ii);


    // Code below assume z2>z1. Swap if this is not the case
    if (z1>z2) {
      std::swap(z1,z2);
      std::swap(xbase1,xbase2);
      std::swap(w1,w2);
    }

    double length = z2-z1;
    double zpos = 0.5*(z1+z2);
    double xpos = 0.5*(xbase1 + xbase2 - 0.5*(w2+w1));

    double theta = 0;  
    if (w1 != w2 || xbase1 != xbase2) {
      theta = atan((xbase2 - xbase1 - 0.5*(w2-w1))/length);
    }

    //std::cout << "ii = " << ii << std::endl;
    //std::cout << " xbase1 = " << xbase1 << std::endl;
    //std::cout << " xbase2 = " << xbase2 << std::endl;
    //std::cout << " w1 = " << w1 << std::endl;
    //std::cout << " w2 = " << w2 << std::endl;
    //std::cout << " widthy = " << widthy << std::endl;
    //std::cout << " z1 = " << z1 << std::endl;
    //std::cout << " z2 = " << z2 << std::endl;
    //std::cout << " ypos = " << ypos << std::endl;
    //std::cout << " theta(CLHEP::deg) = " << theta/CLHEP::deg << std::endl;

    double phi = 0;
    double angleydzn = 0;
    double angleydzp = 0;
  
    const GeoShape * shape = 0;
    if (w1 == w2 && theta == 0) {
      // Its a box
      shape = new GeoBox(0.5*w1, 0.5*widthy, 0.5*length);
    } else {
      shape = new GeoTrap(0.5*length, theta, phi, 0.5*widthy, 0.5*w1, 0.5*w1, angleydzn, 
			  0.5*widthy, 0.5*w2, 0.5*w2, angleydzp);
      // Test GeoModel volume calculation. OK.
      //double testVolume = 0.5*(w1+w2)*length*widthy;
      //std::cout << "Volume Trap: GeoModel: " << shape->volume() << "   My calc: " << testVolume << std::endl;
    }

    if (!perModule) { // For middle section and others
      totVolume += shape->volume();
      HepGeom::Translate3D trans(xpos,ypos,zpos);
      lastShape = addShape(lastShape, shape, trans);

    } else { // Once per module, copied in +z and -z side.
      for (int ii = 0; ii < halfNModule; ii++) {
	totVolume += shape->volume() * 2;// added twice below
	double zshift = m_gmt_mgr->PixelModuleZPosition(1) * ii;

	HepGeom::Translate3D transPos(xpos,ypos,zpos+zshift);
	lastShape = addShape(lastShape, shape, transPos); 

	HepGeom::Transform3D transNeg = HepGeom::Translate3D(xpos,ypos,-(zpos+zshift))*HepGeom::RotateX3D(180*CLHEP::deg);
	lastShape = addShape(lastShape, shape, transNeg); 
      }      
    } 
  }

  if(lastShape==0) {
    msg(MSG::ERROR) << "There is no shape for GeoPixelTMT " <<endmsg;
    return 0; 
  } else {
  const GeoShape * tmtShape = lastShape;
    // don't trust boolean volume
    // double totVolume = tmtShape->volume();
    //std::cout << "TMT volume " << totVolume/CLHEP::cm3 << std::endl;
    std::string matName = m_gmt_mgr->getMaterialName("TMT", m_gmt_mgr->GetLD());
    const GeoMaterial* trapMat = m_mat_mgr->getMaterialForVolume(matName,totVolume);
    GeoLogVol* theTMT = new GeoLogVol("TMT",tmtShape,trapMat);

    // No need to set m_transform as default transform is OK.

     return new GeoPhysVol(theTMT);
  }
}

const GeoShape * 
GeoPixelTMT::addShape(const GeoShape * lastShape, const GeoShape * nextShape, const HepGeom::Transform3D & trans)
{
  const GeoShape * shiftedShape = &(*nextShape << trans);
  if (lastShape) {
    lastShape = &(lastShape->add(*shiftedShape));
  } else {
    lastShape = shiftedShape;
  }
  return lastShape;
}
