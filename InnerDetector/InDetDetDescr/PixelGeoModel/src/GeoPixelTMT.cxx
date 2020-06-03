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
#include "GeoModelKernel/GeoNameTag.h"

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

  // we want to use a assemby, tehrefor we need to dummy volume to trigger the mechanism
  const GeoMaterial* ether = mat_mgr->getMaterial("special::Ether");
  GeoBox* dummybox= new GeoBox(4711., 4711., 4711.);
  GeoLogVol* dummyTMT = new GeoLogVol("TMT",dummybox,ether);
  GeoPhysVol* theTMT = new GeoPhysVol(dummyTMT);

  // get the material by dividing the total material by the volume per layer,
  // the weight is only stored in DB for the whole TMT layer
  // we will commonly use TMT as name for the stuff
  std::string matName = gmt_mgr->getMaterialName("TMT", gmt_mgr->GetLD());

  double volume = 12647.7; // this number seems to be wrong
  const GeoMaterial* material = mat_mgr->getMaterialForVolume(matName,volume); 
  GeoNameTag* tag = new GeoNameTag("TMT");
  
  // this part is unchanged: reading the DB, creating the shapes of the volumes and defining their position 
  HepGeom::RotateX3D traprot(180.*CLHEP::deg);

  int halfNModule = gmt_mgr->PixelNModule()/2;

  for (int ii = 0; ii < gmt_mgr->PixelTMTNumParts(); ii++) {
    double z1 =  gmt_mgr->PixelTMTPosZ1(ii);
    double z2 =  gmt_mgr->PixelTMTPosZ2(ii);
    double xbase1 =  gmt_mgr->PixelTMTBaseX1(ii);
    double xbase2 =  gmt_mgr->PixelTMTBaseX2(ii);
    double w1 =  gmt_mgr->PixelTMTWidthX1(ii);
    double w2 =  gmt_mgr->PixelTMTWidthX2(ii);
    double widthy   = gmt_mgr->PixelTMTWidthY(ii);
    double ypos = gmt_mgr->PixelTMTPosY(ii);
    bool   perModule = gmt_mgr->PixelTMTPerModule(ii);


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

    // end of the old part
    // now we put everything into the assembly
    if (!perModule) { // For middle section and others
      
      // create the colume, move it to the correct relative position and add it to the assembly

      GeoLogVol* tmpLogVol= new GeoLogVol("TMT",shape,material);
      GeoPhysVol* tmpPhysVol= new GeoPhysVol(tmpLogVol);
      GeoTransform* trans = new GeoTransform(HepGeom::Translate3D(xpos,ypos,zpos));
      
      theTMT->add(tag);
      theTMT->add(trans);
      theTMT->add(tmpPhysVol);      
     
    } else { // Once per module, copied in +z and -z side.
      // we will add the same volume many times, need to create it only once
    
      GeoLogVol* tmpLogVol= new GeoLogVol("TMT",shape,material);
      GeoPhysVol* tmpPhysVol= new GeoPhysVol(tmpLogVol);

      for (int ii = 0; ii < halfNModule; ii++) {
	
	// move the dublicates to the correct relative position and add it to the assembly
       	double zshift = gmt_mgr->PixelModuleZPosition(1) * ii;

	GeoTransform* transPos = new GeoTransform(HepGeom::Translate3D(xpos,ypos,zpos+zshift));
	theTMT->add(tag);
	theTMT->add(transPos);
	theTMT->add(tmpPhysVol);

	GeoTransform* transNeg = new GeoTransform(HepGeom::Translate3D(xpos,ypos,-(zpos+zshift))*HepGeom::RotateX3D(180*CLHEP::deg));
	theTMT->add(tag);
	theTMT->add(transNeg);
	theTMT->add(tmpPhysVol);

      }      
    } 
  }
  // Return the assembly
  return theTMT;

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
