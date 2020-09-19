/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GeoShapeConverter.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkDetDescrGeoModelCnv/GeoShapeConverter.h"

#include "TrkVolumes/CylinderVolumeBounds.h"
#include "TrkVolumes/CuboidVolumeBounds.h"
#include "TrkVolumes/TrapezoidVolumeBounds.h"
#include "TrkVolumes/BoundarySurface.h"
#include "TrkVolumes/SubtractedVolumeBounds.h"
#include "TrkVolumes/CombinedVolumeBounds.h"
#include "TrkVolumes/SimplePolygonBrepVolumeBounds.h"
#include "TrkVolumes/VolumeExcluder.h"
#include "TrkVolumes/Volume.h"
// GeoModelKernel
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeIntersection.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoPcon.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoCons.h"
#include "GeoModelKernel/GeoSimplePolygonBrep.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoTrap.h"
#include "GeoModelKernel/GeoPgon.h"
#include "GeoModelKernel/GeoPara.h"
#include "GeoModelKernel/GeoVolumeCursor.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GaudiKernel/SystemOfUnits.h"

// #define DEBUG
#ifdef DEBUG
#  define DEBUG_TRACE(a) do { a } while (0)
#else
#  define DEBUG_TRACE(a) do {  } while (0)
#endif


namespace {
	//commonly used axes
	const Amg::Vector3D gXAxis(1.0, 0.0, 0.0), gYAxis(0.0, 1.0, 0.0), gZAxis(0.0, 0.0, 1.0);
	//commonly used angles, ±90°, 180°
	const double p90deg(90.0 * Gaudi::Units::deg), m90deg(-90.0 * Gaudi::Units::deg), p180deg(180.0 * Gaudi::Units::deg);
}


Trk::CylinderVolumeBounds* Trk::GeoShapeConverter::convert(const GeoTubs* gtubs) const {
    // get the dimensions
    double rMin       = gtubs->getRMin();
    double rMax       = gtubs->getRMax();
    double halflength = gtubs->getZHalfLength();
    // create the volumeBounds
    return new Trk::CylinderVolumeBounds(rMin,rMax,halflength);
}

Trk::CylinderVolumeBounds* Trk::GeoShapeConverter::convert(const GeoTube* gtube) const {
    // get the dimensions
    double rMin       = gtube->getRMin();
    double rMax       = gtube->getRMax();
    double halflength = gtube->getZHalfLength();
    // create the volumeBounds
    return new Trk::CylinderVolumeBounds(rMin,rMax,halflength);
}

Trk::CylinderVolumeBounds* Trk::GeoShapeConverter::convert(const GeoPcon* gpcon, std::vector<double>& zbounds) const {
  // get the pcon igredients ...
  unsigned int numberOfPlanes = gpcon->getNPlanes();
//  std::cout << numberOfPlanes << " planes build this Pcon (pointer: " << gpcon <<") =================" << std::endl;
  // get teh dimensions
  double rMin          = 10e10;
  double rMax          = 0.;
  double zMin          = 10e10;
  double zMax          = 0.;

  for (unsigned int iplane = 0; iplane < numberOfPlanes; ++iplane){

    zMin = gpcon->getZPlane(iplane) < zMin ? gpcon->getZPlane(iplane) : zMin;
    zMax = gpcon->getZPlane(iplane) > zMin ? gpcon->getZPlane(iplane) : zMax;

    rMin = gpcon->getRMinPlane(iplane) < rMin ? gpcon->getRMinPlane(iplane) : rMin;
    rMax = gpcon->getRMaxPlane(iplane) > rMin ? gpcon->getRMaxPlane(iplane) : rMax;

//    std::cout << " Pcon iplane "  << iplane << " gpcon->getZPlane(iplane) " << gpcon->getZPlane(iplane) << " zmin " << zMin << " zmax " << zMax << std::endl;
//    std::cout << " Pcon iplane "  << iplane << " gpcon->getRminPlane(iplane) " << gpcon->getRMinPlane(iplane) << " rmin " << rMin << " gpcon->getRmaxPlane(iplane) " << gpcon->getRMaxPlane(iplane) << " rmax " << rMax << std::endl;

  }
  zbounds.push_back(zMin);
  zbounds.push_back(zMax);

  return new Trk::CylinderVolumeBounds(rMin,rMax,0.5*(zMax-zMin));
}

Trk::CuboidVolumeBounds* Trk::GeoShapeConverter::convert(const GeoBox* gbox) const {

  double halfX = gbox->getXHalfLength();
  double halfY = gbox->getYHalfLength();
  double halfZ = gbox->getZHalfLength();

  return new Trk::CuboidVolumeBounds(halfX,halfY,halfZ);
}

Trk::Volume* Trk::GeoShapeConverter::translateGeoShape(const GeoShape* sh, Amg::Transform3D* transf) const
{
  Trk::Volume* vol=0;
  double tol = 0.1;

  DEBUG_TRACE( std::cout << " translateGeoShape " << sh->type() << std::endl; );

  if ( sh->type()=="Trap") {
    const GeoTrap* trap = dynamic_cast<const GeoTrap*> (sh);
    Trk::TrapezoidVolumeBounds* volBounds = 0;
    if (trap->getDxdyndzp()<trap->getDxdyndzn())
      volBounds=new Trk::TrapezoidVolumeBounds(trap->getDxdyndzp(),trap->getDxdyndzn(),trap->getDydzn(),trap->getZHalfLength() );
    else
      volBounds=new Trk::TrapezoidVolumeBounds(trap->getDxdyndzn(),trap->getDxdyndzp(),trap->getDydzn(),trap->getZHalfLength() );

    vol = new Trk::Volume(new Amg::Transform3D(*transf), volBounds );

    return vol;
  }

  if ( sh->type()=="Pgon") {
    const GeoPgon* pgon = dynamic_cast<const GeoPgon*>(sh);
    if (!pgon) return 0;
    double hlz = 0.5*fabs(pgon->getZPlane(1)-pgon->getZPlane(0));
    double phiH = pgon->getDPhi()/(2.*pgon->getNSides());
    double hly = 0.5*cos(phiH)*(pgon->getRMaxPlane(0)-pgon->getRMinPlane(0));
    double dly = 0.5*cos(phiH)*(pgon->getRMaxPlane(0)+pgon->getRMinPlane(0));
    double hlxmin =pgon->getRMinPlane(0)*sin(phiH);
    double hlxmax =pgon->getRMaxPlane(0)*sin(phiH);

    if (pgon->getDPhi()==2*M_PI) {

      Trk::CylinderVolumeBounds* volBounds = new Trk::CylinderVolumeBounds(pgon->getRMaxPlane(0),hlz);
      Trk::CuboidVolumeBounds* subBounds = new Trk::CuboidVolumeBounds(hlxmax+tol,hlxmax+tol,hlz+tol);
      Trk::Volume* volume = new Trk::Volume(new Amg::Transform3D(*transf),volBounds);
      Trk::Volume* bVol = new Trk::Volume(0,subBounds);
      const unsigned int nsides(pgon->getNSides());
      const double twicePhiH(2.0*phiH);
      const double xTranslationDistance=hlxmax + cos(phiH) * (pgon->getRMaxPlane(0));
      const Amg::Vector3D translationVector(xTranslationDistance, 0., 0.);
      const Amg::Translation3D xTranslation(translationVector);
      for (unsigned int i=0; i<nsides; i++) {
        const double angle = i * twicePhiH;
        Amg::AngleAxis3D zRotation(angle, gZAxis);
        const Amg::Transform3D totalTransform = (*transf) * zRotation * xTranslation;
        
        //should be equivalent to:
        //HepGeom::Transform3D trP(*transf*HepGeom::RotateZ3D(2*i*phiH)*HepGeom::TranslateX3D(hlxmax+cos(phiH)*pgon->getRMaxPlane(0)));
        Trk::Volume* volS = new Trk::Volume(*bVol,totalTransform);
        Trk::SubtractedVolumeBounds* combBounds = new Trk::SubtractedVolumeBounds(volume,volS);
        volume = new Trk::Volume(0,combBounds);
      }
      delete bVol;
      return volume;
    }

    if (pgon->getNSides() == 1 ) {
		Trk::TrapezoidVolumeBounds* volBounds = new Trk::TrapezoidVolumeBounds(hlxmin,hlxmax,hly,hlz);
		const Amg::Vector3D translationVector(0.0, dly, 0.0);
		const Amg::Translation3D yTranslation(translationVector);
		const Amg::AngleAxis3D zRotation(m90deg, gZAxis);
		Amg::Transform3D totalTransform = *transf * zRotation * yTranslation;
		//totalTransform *= zRotation;
		//totalTransform *= yTranslation;
		vol = new Trk::Volume(new Amg::Transform3D(totalTransform), volBounds);
        return vol;
    }

    if (pgon->getNSides() == 2) {
      Trk::CylinderVolumeBounds* cylBounds = new Trk::CylinderVolumeBounds(0,dly+hly,hlz);
      vol = new Trk::Volume(new Amg::Transform3D(*transf),cylBounds );
      return vol;
    }

    return vol;

  }

  if ( sh->type()=="Trd") {
    const GeoTrd* trd = dynamic_cast<const GeoTrd*> (sh);
    //
    double x1= trd->getXHalfLength1();
    double x2= trd->getXHalfLength2();
    double y1= trd->getYHalfLength1();
    double y2= trd->getYHalfLength2();
    double z = trd->getZHalfLength();
    //
    DEBUG_TRACE( std::cout <<  " Trd x1 " << x1 << " x2 " << x2 << " y1 " << y1 << " y2 " << y2 << " z " << z << std::endl; );

// Note this flip comes from the y axis in Tracking -> z axis in Geomodel

    const Amg::AngleAxis3D yzFlip(p90deg,gXAxis);
    if (y1==y2) {
      if ( x1 <= x2 ) {
//
// my P.K. guess
//
	    Trk::TrapezoidVolumeBounds* volBounds=new Trk::TrapezoidVolumeBounds(x1,x2,z,y1);
       	    Amg::Transform3D totalTransform = (*transf) * yzFlip; 
	        vol = new Trk::Volume(new Amg::Transform3D(totalTransform),volBounds);
                DEBUG_TRACE( std::cout <<  " Trd new volume case 1 Trapezoid minHalflengthX " << volBounds->minHalflengthX() << " maxHalflengthX() " << volBounds->maxHalflengthX() << std::endl; );
      } else {

        Trk::TrapezoidVolumeBounds* volBounds=new Trk::TrapezoidVolumeBounds(x2,x1,z,y1);
       	const Amg::AngleAxis3D yFlip(p180deg,gXAxis);
       	const Amg::AngleAxis3D xySign(p180deg,gZAxis);

       	Amg::Transform3D totalTransform = (*transf) * yFlip * yzFlip; 
	    vol = new Trk::Volume(new Amg::Transform3D(totalTransform),volBounds);

        DEBUG_TRACE( {
          std::cout <<  " Trd new volume case 2 Trapezoid minHalflengthX " << volBounds->minHalflengthX() << " maxHalflengthX() " << volBounds->maxHalflengthX() << std::endl;
// original local coordinates in Geomodel (check that routine is NOT called with the  1,1,1 Matrix)
          Amg::Vector3D top(-x1,y1,z);
          Amg::Vector3D bottom(-x2,y1,-z);
          Amg::Vector3D top2(x1,y1,z);
          Amg::Vector3D bottom2(x2,y1,-z);
          
          std::cout << " Original topLocal x " << top.x() << " y " << top.y() << " z " << top.z() << " Radius " << bottom.perp() << std::endl;
          std::cout << " Original bottomLocal x " << bottom.x() << " y " << bottom.y() << " z " << bottom.z() << " Radius " << bottom.perp() << std::endl;
          Amg::Vector3D topG = (*transf)*top;
          Amg::Vector3D bottomG = (*transf)*bottom; 
          std::cout << " top Global x " << topG.x() << " y " << topG.y() << " z " << topG.z() << " Radius " << topG.perp() << std::endl;
          std::cout << " bottom Global x " << bottomG.x() << " y " << bottomG.y() << " z " << bottomG.z() << " Radius " << bottomG.perp() << std::endl;
          topG = (*transf)*top2;
          bottomG = (*transf)*bottom2; 
          std::cout << " top2 Global x " << topG.x() << " y " << topG.y() << " z " << topG.z() << " Radius " << topG.perp() << std::endl;
          std::cout << " bottom2 Global x " << bottomG.x() << " y " << bottomG.y() << " z " << bottomG.z() << " Radius " << bottomG.perp() << std::endl;

// new local coordinates 
          Amg::Vector3D topR(-x2,z,y1);
          Amg::Vector3D bottomR(-x1,-z,y1);
          Amg::Vector3D top2R(x2,z,y1);
          Amg::Vector3D bottom2R(x1,-z,y1);
          topG = totalTransform*topR;
          bottomG = totalTransform*bottomR; 
          std::cout << " topR Global x " << topG.x() << " y " << topG.y() << " z " << topG.z() << " Radius " << topG.perp() << std::endl;
          std::cout << " bottomR Global x " << bottomG.x() << " y " << bottomG.y() << " z " << bottomG.z() << " Radius " << bottomG.perp() << std::endl;
          topG = totalTransform*top2R;
          bottomG = totalTransform*bottom2R; 
          std::cout << " top2R Global x " << topG.x() << " y " << topG.y() << " z " << topG.z() << " Radius " << topG.perp() << std::endl;
          std::cout << " bottom2R Global x " << bottomG.x() << " y " << bottomG.y() << " z " << bottomG.z() << " Radius " << bottomG.perp() << std::endl;

          std::cout << " Original bottomLocal x " << bottom.x() << " y " << bottom.y() << " z " << bottom.z() << " Radius " << bottom.perp() << std::endl;
          std::cout << " topLocal x " << topR.x() << " y " << topR.y() << " z " << topR.z() << " Radius " << topR.perp() << std::endl;
          topG = yFlip*topR;
          std::cout << " topLocal Y Flip  x " << topG.x() << " y " << topG.y() << " z " << topG.z() << " Radius " << topG.perp() << std::endl;
       	  const Amg::AngleAxis3D xFlip(p180deg,gYAxis);
          topG = xFlip*topR;
          std::cout << " topLocal X Flip  x " << topG.x() << " y " << topG.y() << " z " << topG.z() << " Radius " << topG.perp() << std::endl;
          const Amg::AngleAxis3D xyFlip(p90deg,gZAxis);
          topG = xyFlip*topR; 
          std::cout << " topLocal XY Flip  x " << topG.x() << " y " << topG.y() << " z " << topG.z() << " Radius " << topG.perp() << std::endl;
          topG = yzFlip*topR; 
          std::cout << " topLocal YZ Flip  x " << topG.x() << " y " << topG.y() << " z " << topG.z() << " Radius " << topG.perp() << std::endl;
       	  const Amg::AngleAxis3D xzFlip(p90deg,gYAxis);
          topG = xzFlip*topR; 
          std::cout << " topLocal XZ Flip  x " << topG.x() << " y " << topG.y() << " z " << topG.z() << " Radius " << topG.perp() << std::endl;
          topG = xySign*topR; 
          std::cout << " topLocal XY sign Flip  x " << topG.x() << " y " << topG.y() << " z " << topG.z() << " Radius " << topG.perp() << std::endl;
          } ); 
      }
      
      return vol;
    } else if (x1==x2) {
      if ( y1 < y2 ) {
	    Trk::TrapezoidVolumeBounds* volBounds=new Trk::TrapezoidVolumeBounds(y1,y2,z,x1);
//	    const Amg::AngleAxis3D yRotation(p90deg, gYAxis);
//	    const Amg::AngleAxis3D zRotation(p90deg, gZAxis);
//	    Amg::Transform3D totalTransform = yRotation * (*transf) * zRotation;
//
// my P.K. guess
//
            const Amg::AngleAxis3D xyFlip(p90deg,gZAxis);
            Amg::Transform3D totalTransform = (*transf) * xyFlip * yzFlip;
	    vol = new Trk::Volume(new Amg::Transform3D(totalTransform), volBounds);
	    //original line:
	    //vol = new Trk::Volume(new HepGeom::Transform3D(HepGeom::RotateY3D(90*CLHEP::deg)*(*transf)*HepGeom::RotateZ3D(90*CLHEP::deg)), volBounds );
            DEBUG_TRACE( std::cout <<  " Trd new volume case 3 Trapezoid minHalflengthX " << volBounds->minHalflengthX() << " maxHalflengthX() " << volBounds->maxHalflengthX() << std::endl; );
      } else {
	    Trk::TrapezoidVolumeBounds* volBounds=new Trk::TrapezoidVolumeBounds(y2,y1,z,x1);
//	    const Amg::AngleAxis3D yRotation(p90deg, gYAxis);
//	    const Amg::Transform3D totalTransform = yRotation * (*transf);
// my P.K. guess
//
            const Amg::AngleAxis3D xyFlip(p90deg,gZAxis);
            const Amg::AngleAxis3D yFlip(p180deg,gXAxis);
            Amg::Transform3D totalTransform = (*transf) * yFlip * xyFlip * yzFlip;
	    //totalTransform *= (*transf);
	    vol = new Trk::Volume(new Amg::Transform3D(totalTransform), volBounds );
            DEBUG_TRACE( std::cout <<  " Trd new volume case 4 Trapezoid minHalflengthX " << volBounds->minHalflengthX() << " maxHalflengthX() " << volBounds->maxHalflengthX() << std::endl; );
      }
      return vol;
    } else {
      std::cout << "PROBLEM: translating trapezoid: not recognized:"
		<< x1 << "," << x2 << "," << y1 << "," << y2 <<"," << z << std::endl;
    }
  }

  if ( sh->type()=="Box") {
    const GeoBox* box = dynamic_cast<const GeoBox*> (sh);
    //
    double x = box->getXHalfLength();
    double y = box->getYHalfLength();
    double z = box->getZHalfLength();
    Trk::CuboidVolumeBounds* volBounds=new Trk::CuboidVolumeBounds(x,y,z);
    vol = new Trk::Volume(new Amg::Transform3D(*transf), volBounds );
    return vol;
    //
  }
  if ( sh->type()=="Para") {
    const GeoPara* para = dynamic_cast<const GeoPara*> (sh);
    //
    double x = para->getXHalfLength();
    double y = para->getYHalfLength();
    double z = para->getZHalfLength();
    Trk::CuboidVolumeBounds* volBounds=new Trk::CuboidVolumeBounds(x,y,z);
    vol = new Trk::Volume(new Amg::Transform3D(*transf), volBounds );
    return vol;
    //
  }
  if ( sh->type() == "Tube" ) {
    const GeoTube* tube=dynamic_cast<const GeoTube*> (sh);
    double rMin= tube->getRMin();
    double rMax= tube->getRMax();
    double z =   tube->getZHalfLength();
    Trk::CylinderVolumeBounds* volBounds=new Trk::CylinderVolumeBounds(rMin,rMax,z);
    vol = new Trk::Volume(new Amg::Transform3D(*transf), volBounds );
    return vol;
   }

  if ( sh->type() == "Tubs" ) {            // non-trivial case - transform!
    const GeoTubs* tubs=dynamic_cast<const GeoTubs*> (sh);
    double rMin= tubs->getRMin();
    double rMax= tubs->getRMax();
    double z =   tubs->getZHalfLength();
    double aPhi =   tubs->getSPhi();
    double dPhi =   tubs->getDPhi();
    Trk::CylinderVolumeBounds* volBounds=new Trk::CylinderVolumeBounds(rMin,rMax,0.5*dPhi,z);
    Amg::Transform3D totalTransform(*transf);
    const Amg::AngleAxis3D zRotation(aPhi + 0.5*dPhi, gZAxis);
    totalTransform *=zRotation;
    vol = new Trk::Volume(new Amg::Transform3D(totalTransform), volBounds );
    return vol;
  }

  if ( sh->type() == "Cons" ) {
    const GeoCons* cons=dynamic_cast<const GeoCons*> (sh);
    double rMin1= cons->getRMin1();
    double rMin2= cons->getRMin2();
    double rMax1= cons->getRMax1();
    double rMax2= cons->getRMax2();
    double z    = cons->getDZ();
    double aPhi = cons->getSPhi();
    double dPhi = cons->getDPhi();
    // translate into tube with average radius
    if ( dPhi == 2*M_PI ) {
      Trk::CylinderVolumeBounds* volBounds=new Trk::CylinderVolumeBounds(0.5*(rMin1+rMin2),0.5*(rMax1+rMax2),z);
      vol = new Trk::Volume(new Amg::Transform3D(*transf), volBounds );
      return vol;
    } else {
      Trk::CylinderVolumeBounds* volBounds=new Trk::CylinderVolumeBounds(0.5*(rMin1+rMin2),0.5*(rMax1+rMax2),0.5*dPhi,z);
      Amg::Transform3D totalTransform(*transf);
      const Amg::AngleAxis3D zRotation(aPhi + 0.5*dPhi, gZAxis);
      totalTransform *= zRotation;
      vol = new Trk::Volume(new Amg::Transform3D(totalTransform), volBounds );
      return vol;
    }
  }

  if ( sh->type() == "Pcon" ) {
    const GeoPcon* con = dynamic_cast<const GeoPcon*> (sh);
    if (!con) return 0;
    Trk::CylinderVolumeBounds* volBounds = 0;
    double aPhi = con->getSPhi();
    double dPhi = con->getDPhi();
    double z1 = con->getZPlane(0);
    double r1 = con->getRMinPlane(0);
    double R1 = con->getRMaxPlane(0);
    std::vector<Trk::Volume*> cyls;
    const unsigned int nPlanes=con->getNPlanes();
    DEBUG_TRACE( std::cout << " convert pcon aPhi " << aPhi << " dPhi " << dPhi << " z1 " <<z1 << " r1 " << r1 <<" R1 " << R1 << " nPlanes " << nPlanes << std::endl; );
    for (unsigned int iv=1;iv<nPlanes;iv++) {
      double z2 = con->getZPlane(iv);
      double r2 = con->getRMinPlane(iv);
      double R2 = con->getRMaxPlane(iv);
      double zshift = 0.5*(z1+z2);
      double hz = 0.5*fabs(z1-z2);
      double rmin = fmax(r1,r2);
      double rmax = sqrt( ( R1*R1+R1*R2+R2*R2-r1*r1-r1*r2-r2*r2 )/3 + rmin*rmin );
      DEBUG_TRACE( std::cout << " iPlane " << iv <<  " z2 " <<z2 << " r2 " << r2 <<" R2 " << R2 << " zshift " << zshift << " hz " << hz <<  " rmin " << rmin << " rmax " << rmax << std::endl; );
      double dz =  con->getZPlane(iv) -  con->getZPlane(iv-1);
      double drMin =  con->getRMinPlane(iv) -  con->getRMinPlane(iv-1);
      double drMax =  con->getRMaxPlane(iv) -  con->getRMaxPlane(iv-1);
      int nSteps = 1;
      if(fabs(dz)>1&&(fabs(drMin)>0.1*fabs(dz)||fabs(drMax)>0.1*fabs(dz))) {
         double dMax = fabs(dz);
         if(fabs(drMin)> dMax) dMax = fabs(drMin);
         if(fabs(drMax)> dMax) dMax = fabs(drMax);
         nSteps = dMax/50.;
//         nSteps = dMax/100.;
         if(nSteps<2) nSteps = 2;
         if(nSteps>20) nSteps = 20;
         DEBUG_TRACE( std::cout << " Now " << nSteps << " cylinders should be created " << dz << " drMin " << drMin << " drMax " << drMax << " splopeMin " << drMin/dz << " slopeMax " << drMax/dz << std::endl; );
      }
//      nSteps = 1;
      
      for (int j=0;j<nSteps;j++) {

// divide volume into nStep cylinders

        double zStep = (0.5 + j)*dz/nSteps;
        if(nSteps>1) {
          hz     = 0.5*fabs(z1-z2)/nSteps;
          zshift = z1 + zStep;
          rmin   = r1 + drMin*zStep/dz;
          rmax   = R1 + drMax*zStep/dz;
        }
        DEBUG_TRACE( std::cout << " cylinder " << j << " zshift " << zshift << " rmin " << rmin << " rmax " << rmax << " hz " << hz << std::endl; );
      // translate into tube sector
        if ( dPhi == 2*M_PI ) {
	   volBounds=new Trk::CylinderVolumeBounds(rmin,rmax,hz);
	   Amg::Transform3D totalTransform(*transf);
	   Amg::Vector3D translationVector(0.0, 0.0, zshift);
	   Amg::Translation3D zTranslation (translationVector);
	   totalTransform *= zTranslation;
	   cyls.push_back(new Trk::Volume(new Amg::Transform3D(totalTransform), volBounds ));
        } else {
	   volBounds=new Trk::CylinderVolumeBounds(rmin,rmax,0.5*dPhi,hz);
	   Amg::Transform3D totalTransform(*transf);
	   Amg::Vector3D translationVector(0.0, 0.0, zshift);
	   Amg::Translation3D zTranslation (translationVector);
	   totalTransform *= zTranslation;
	   const Amg::AngleAxis3D zRotation(aPhi + 2.0*dPhi, gZAxis);
	   totalTransform *=zRotation;
	   cyls.push_back(new Trk::Volume(new Amg::Transform3D(totalTransform), volBounds ));
        }
      } // end loop over steps
      z1 = z2; r1 = r2; R1 = R2;
    }
    if (cyls.size()<2) return cyls[0];
    else {
      Trk::CombinedVolumeBounds* comb = new Trk::CombinedVolumeBounds(cyls[0],cyls[1],false);
      Trk::Volume* combVol = new Trk::Volume(0,comb);
      unsigned int ic = 2;
      while ( ic<cyls.size() ){
        comb = new Trk::CombinedVolumeBounds(combVol,cyls[ic],false);
        combVol = new Trk::Volume(0,comb);
        ic++;
      }
      return combVol;
    }
  }

  if ( sh->type()=="SimplePolygonBrep") {
    const GeoSimplePolygonBrep* spb = dynamic_cast<const GeoSimplePolygonBrep*> (sh);
    if (!spb) return 0;
    unsigned int nv = spb->getNVertices();
    std::vector<std::pair<double,double> > ivtx(nv);
    for (unsigned int iv = 0; iv < nv; iv++) {
     ivtx[iv]=std::pair<double,double>(spb->getXVertex(iv),spb->getYVertex(iv));
     DEBUG_TRACE( std::cout << " SimplePolygonBrep  x " << spb->getXVertex(iv) << " y " << spb->getYVertex(iv) << " z " << spb->getDZ() << std::endl; );
    }   
    return new Trk::Volume(new Amg::Transform3D(*transf),new Trk::SimplePolygonBrepVolumeBounds(ivtx,spb->getDZ()));
  }

  if ( sh->type()=="Subtraction") {
    const GeoShapeSubtraction* sub = dynamic_cast<const GeoShapeSubtraction*> (sh);
    if (!sub) return 0;
    const GeoShape* shA = sub->getOpA();
    const GeoShape* shB = sub->getOpB();
    Trk::Volume* volA = translateGeoShape(shA, transf);
    if (!volA) return vol;
    Trk::Volume* volB = translateGeoShape(shB, transf);
    if (!volB ) { delete volA; return vol; }
    Trk::SubtractedVolumeBounds* volBounds = new Trk::SubtractedVolumeBounds(volA, volB);
    vol = new Trk::Volume(0, volBounds );
    return vol;
  }

  if ( sh->type()=="Union") {
    const GeoShapeUnion* uni = dynamic_cast<const GeoShapeUnion*> (sh);
    if (!uni) return 0;
    const GeoShape* shA = uni->getOpA();
    const GeoShape* shB = uni->getOpB();
    Trk::Volume* volA = translateGeoShape(shA, transf);
    if (!volA) return vol;
    Trk::Volume* volB = translateGeoShape(shB, transf);
    if (!volB ) { delete volA; return vol; }
    Trk::CombinedVolumeBounds* volBounds = new Trk::CombinedVolumeBounds(volA,volB,false);
    vol = new Trk::Volume(0, volBounds );
    return vol;
  }

  if ( sh->type()=="Intersection") {
    const GeoShapeIntersection* intersect = dynamic_cast<const GeoShapeIntersection*> (sh);
    if (!intersect) return 0;
    const GeoShape* shA = intersect->getOpA();
    const GeoShape* shB = intersect->getOpB();
    Trk::Volume* volA = translateGeoShape(shA, transf);
    if (!volA) return vol;
    Trk::Volume* volB = translateGeoShape(shB, transf);
    if (!volB ) { delete volA; return vol; }
    Trk::CombinedVolumeBounds* volBounds = new Trk::CombinedVolumeBounds(volA,volB,true);
    vol = new Trk::Volume(0, volBounds );
    return vol;
  }

  if ( sh->type()=="Shift") {
    const GeoShapeShift* shift = dynamic_cast<const GeoShapeShift*> (sh);
    if (!shift) return 0;
    const GeoShape* shA = shift->getOp();
    //check this!
    const Amg::Transform3D tr = shift->getX();
    Amg::Transform3D newtransf = *transf * tr;
    Trk::Volume* vol = translateGeoShape(shA, &newtransf);
    return vol;
  }
  std::cout << "shape "<< sh->type() <<" not recognized, return 0" << std::endl;
  return vol;
}


void Trk::GeoShapeConverter::decodeShape(const GeoShape* sh) const
{
  std::cout << "  " ;
  std::cout << "decoding shape:"<< sh->type() << std::endl;

  if ( sh->type()=="Pgon") {
    const GeoPgon* pgon = dynamic_cast<const GeoPgon*>(sh);
    if (pgon)
        std::cout << "polygon: " << pgon->getNPlanes() << " planes " << pgon->getSPhi() << " " << pgon->getDPhi() << " " << pgon->getNSides() << std::endl;
    else
        std::cout << "polygon: WARNING: dynamic_cast failed!" << std::endl;
  }

  if ( sh->type()=="Trd") {
    const GeoTrd* trd = dynamic_cast<const GeoTrd*> (sh);
    std::cout << "dimensions:" << trd->getXHalfLength1() << ","
	      << trd->getXHalfLength2() << ","
	      << trd->getYHalfLength1() << ","
	      << trd->getYHalfLength2() << ","
	      << trd->getZHalfLength() << std::endl;
  }
  if ( sh->type()=="Box") {
    const GeoBox* box = dynamic_cast<const GeoBox*> (sh);
    std::cout << "dimensions:" << box->getXHalfLength() << ","
	      << box->getYHalfLength() << ","
	      << box->getZHalfLength() << std::endl;
  }

  if ( sh->type() == "Tube" ) {
    const GeoTube* tube=dynamic_cast<const GeoTube*> (sh);
    std::cout<<"dimensions:"<< tube->getRMin() << ","
                            << tube->getRMax() << ","
                            << tube->getZHalfLength() << std::endl;
  }

  if ( sh->type() == "Tubs" ) {
    const GeoTubs* tubs=dynamic_cast<const GeoTubs*> (sh);
    std::cout<<"dimensions:"<< tubs->getRMin() << ","
                            << tubs->getRMax() << ","
	     << tubs->getZHalfLength() <<"," << tubs->getSPhi() <<"," << tubs->getDPhi()  << std::endl;
  }

  if ( sh->type() == "Cons" ) {
    const GeoCons* cons=dynamic_cast<const GeoCons*> (sh);
    std::cout<<"dimensions:"<< cons->getRMin1() << "," << cons->getRMin2() << ","
                            << cons->getRMax1() << "," << cons->getRMax2() << ","
	     << cons->getDZ() <<"," << cons->getSPhi() <<"," << cons->getDPhi()  << std::endl;
  }

  if ( sh->type()=="Subtraction") {
    const GeoShapeSubtraction* sub = dynamic_cast<const GeoShapeSubtraction*> (sh);
    const GeoShape* sha = sub->getOpA();
    const GeoShape* shs = sub->getOpB();
    std::cout << "decoding subtracted shape:" << std::endl;
    decodeShape(sha);
    decodeShape(shs);
  }

  if ( sh->type()=="Union") {
    const GeoShapeUnion* sub = dynamic_cast<const GeoShapeUnion*> (sh);
    const GeoShape* shA = sub->getOpA();
    const GeoShape* shB = sub->getOpB();
    std::cout << "decoding shape A:" << std::endl;
    decodeShape(shA);
    std::cout << "decoding shape B:" << std::endl;
    decodeShape(shB);
  }
  if ( sh->type()=="Shift") {
    const GeoShapeShift* shift = dynamic_cast<const GeoShapeShift*> (sh);
    const GeoShape* shA = shift->getOp();
    const GeoTrf::Transform3D transf = shift->getX();
    std::cout << "shifted by:transl:" <<transf.translation() <<", rot:"
              << transf(0,0)<<"," << transf(1,1) <<"," << transf(2,2) << std::endl;
    decodeShape(shA);
  }
}
