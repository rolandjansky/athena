/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

//
// This class to hold general services
//
// The services support several different shapes. The meaning of the parameters
// depends on the particular shape:
//
//
// TUBE or empty
//   Ignored: RIN2,ROUT2,PHI,WIDTH,REPEAT
// TUBS
//   Ignored: RIN2,ROUT2
//   PHI: phi start location of tube sector
//   WIDTH (CLHEP::deg): phi width of sector
//   REPEAT: Repeat the tube sector this many times in phi with equal distance between them.
// CONS, CONE
//   WIDTH,REPEAT ignored if CONE
//   RIN2,ROUT2: rmin, rmx at zmax. Same as RIN, ROUT if <=0.
//   PHI, WIDTH, REPEAT same as TUBS
// PGON
//   Ignored: WIDTH
//   RIN,ROUT,RIN2,ROUT2 defined at corner of polygon.
//   PHI: phi start location of a corner
//   REPEAT: Sides of polygon
// PGON2
//   Ignored: WIDTH
//   RIN,ROUT,RIN2,ROUT2 defined at side of polygon.
//   PHI: phi start location of center of side.
//   REPEAT: Sides of polygon
// PGON3 (pgon at outer edge, circular inner edge)
//   Ignored: WIDTH
//   RIN,RIN2 radius of inner edge (tube or cone)
//   ROUT,ROUT2 defined at corner of polygon.
//   PHI: phi start location of a corner
//   REPEAT: Sides of polygon
// PGON31 (pgon at inner edge, circular outer edge)
//   Ignored: WIDTH
//   RIN,RIN2 radius of inner edge (tube or cone)
//   ROUT,ROUT2 defined at corner of polygon.
//   PHI: phi start location of a corner
//   REPEAT: Sides of polygon
// PGON4 (pgon at outer edge, circular inner edge)
//   Ignored: WIDTH
//   RIN,RIN2 radius of inner edge (tube or cone)
//   ROUTROUT2 defined at side of polygon.
//   PHI: phi start location of center of side.
//   REPEAT: Sides of polygon
// ROD
//   Ignored: ROUT, RIN2, ROUT2
//   RIN: Radial position of center of tube
//   PHI: phi position of center
//   WIDTH (mm): diameter
//   REPEAT: Repeat this many times in phi with equal distance between them.
// ROD2  (hollow tube not centered around Z axis)
//   Ignored: ROUT, ROUT2
//   RIN: Radial position of center of tube
//   RIN2: inner radius
//   PHI: phi position of center
//   WIDTH (mm): diameter
//   REPEAT: Repeat this many times in phi with equal distance between them.
// BOX
//   Ignored: RIN2, ROUT2
//   ROUT-RIN = thickness of box (radially)
//   (RIN+ROUT)/2 = radial poistion of center of box
//   PHI: phi position of center
//   WIDTH (mm) = width of box
//   REPEAT: Repeat this many times in phi with equal distance between them.
// TRAP
//   Ignored: RIN2, ROUT2
//   ROUT-RIN = thickness of trapezoid (radially)
//   (RIN+ROUT)/2 = radial poistion of center of trapzoid
//   PHI: phi position of center
//   WIDTH (mm) = width of trapezoid at center
//   REPEAT: Repeat this many times in phi with equal distance between them.
//
//   **IMPORTANT NOTE** WIDTH can be in degrees or mm. See OraclePixGeoManager

#include "InDetGeoModelUtils/ServiceVolume.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoCons.h"
#include "GeoModelKernel/GeoPgon.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTrap.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

namespace InDetDD {
  ServiceVolume::ServiceVolume()
    : m_rmin(0),
    m_rmax(0),
    m_rmin2(0),
    m_rmax2(0),
    m_zmin(0),
    m_zmax(0),
    //m_volId(0),
    m_zsymm(false),
    m_geoShape(0),
    m_material(0),
    m_phiLoc(0),
    m_phiWidth(0),
    m_needsRotation(false),
    m_sides(0),
    m_nCopies(1),
    //m_origLength(0),
    m_origVolume(0),
    m_volume(0),
    m_safety(0),
    m_lockGeoShape(0),
    m_splittableR(true),
    m_splittableZ(true),
    m_envNum(0),
    m_envParentNum(0),
    m_zShift(0.)
  {}


  void
  ServiceVolume::reduceSize(double safety) {
    // Don't do anything if its a very thin volume.
    if (length() > 4. * safety) {
      if (m_zmax < m_zmin) std::swap(m_zmin, m_zmax);
      m_safety = safety;
    }
    m_geoShape = 0;
  }

  void
  ServiceVolume::setLabel(const std::string& name, int volId) {
    std::ostringstream o;
    o.fill('0');
    o << name << std::setw(2) << volId;
    m_label = o.str();
  }

  std::string
  ServiceVolume::fullLabel() const {
    if (m_volName.empty()) return m_label;

    return m_label + "_" + m_volName;
  }

  void
  ServiceVolume::print() const {
    std::cout << m_rmin << " "
              << m_rmax << " "
              << m_zmin << " "
              << m_zmax << " "
              << m_region << " "
              << fullLabel()
              << std::endl;
  }

  const GeoShape*
  ServiceVolume::getShape() const {
    // If prebuilt then return
    if (m_geoShape.get()) return m_geoShape.get();

    //
    // Dimensions
    //
    //double rmin = rmin();
    //double rmax = rmax();
    //double rmin2 = rmin2();
    //double rmax2 = rmax2();
    //double phiLoc = phiLoc();
    //double phiWidth = phiWidth();
    //int sides = sides();
    //const std::string & shapeType = shapeType();

    double halflength = 0.5 * length();

    //std::cout << "Building service volume " << logName << ": "
    //	      << rmin << ", "
    //	      << rmax << ", "
    //	      << halflength << ", "
    //	      << materialName << std::endl;

    const GeoShape* serviceShape = 0;
    double volume = 0;

    // Check if service needs to be shifted
    //   if(fabs(m_zShift)>0.001)
    //      std::cout<<"SHIFTED SERVICE : "<<m_volName<<" "<<m_shapeType<<std::endl;

    if (m_shapeType.empty() || m_shapeType == "TUBE") {
      serviceShape = new GeoTube(m_rmin, m_rmax, halflength);
    } else if (m_shapeType == "TUBS") {
      serviceShape = new GeoTubs(m_rmin, m_rmax, halflength, m_phiLoc, m_phiWidth);
    } else if (m_shapeType == "CONS" || m_shapeType == "CONE") {
      double phiWidthTmp = m_phiWidth;
      if (m_shapeType == "CONE" || phiWidthTmp == 0) {
        phiWidthTmp = 2 * M_PI;
      }
      serviceShape = new GeoCons(m_rmin, m_rmin2, m_rmax, m_rmax2, halflength, m_phiLoc, phiWidthTmp);
    } else if (m_shapeType == "PGON") {
      GeoPgon* shapeTmp = new GeoPgon(m_phiLoc, 2 * M_PI, m_sides);
      shapeTmp->addPlane(-halflength, m_rmin, m_rmax);
      shapeTmp->addPlane(halflength, m_rmin2, m_rmax2);
      serviceShape = shapeTmp;
    } else if (m_shapeType == "PGON2") {
      // Radius defined at the side, not the corner
      double alpha = M_PI / m_sides;
      double cosalpha = cos(alpha);
      double rminB = m_rmin / cosalpha;
      double rmaxB = m_rmax / cosalpha;
      double rmin2B = m_rmin2 / cosalpha;
      double rmax2B = m_rmax2 / cosalpha;
      GeoPgon* shapeTmp = new GeoPgon(m_phiLoc - alpha, 2 * M_PI, m_sides);
      shapeTmp->addPlane(-halflength, rminB, rmaxB);
      shapeTmp->addPlane(halflength, rmin2B, rmax2B);
      serviceShape = shapeTmp;
    } else if (m_shapeType == "PGON3" || m_shapeType == "PGON4") {
      // Outer edge
      GeoPgon* shapeTmp1 = 0;
      if (m_shapeType == "PGON3") {
        shapeTmp1 = new GeoPgon(m_phiLoc, 2 * M_PI, m_sides);
        shapeTmp1->addPlane(-halflength, 0, m_rmax);
        shapeTmp1->addPlane(halflength, 0, m_rmax2);
      } else { //PGON4
        double alpha = M_PI / m_sides;
        double cosalpha = cos(alpha);
        double rmaxB = m_rmax / cosalpha;
        double rmax2B = m_rmax2 / cosalpha;
        shapeTmp1 = new GeoPgon(m_phiLoc - alpha, 2 * M_PI, m_sides);
        shapeTmp1->addPlane(-halflength, 0, rmaxB);
        shapeTmp1->addPlane(halflength, 0, rmax2B);
      }
      // Don't trust boolean volume calculation.
      volume = shapeTmp1->volume();
      // Inner edge
      GeoShape* shapeTmp2 = 0;
      if (m_rmin == m_rmin2) {
        shapeTmp2 = new GeoTube(0, m_rmin, halflength + 0.1 * Gaudi::Units::mm);
        volume -= 2 * M_PI * m_rmin * m_rmin * halflength;
      } else {
        shapeTmp2 = new GeoCons(0, 0, m_rmin, m_rmin2, halflength + 0.1 * Gaudi::Units::mm, 0, 2 * M_PI);
        volume -= 2 * M_PI * pow(0.5 * (m_rmin + m_rmin2), 2) * halflength;
      }
      serviceShape = &(shapeTmp1->subtract(*shapeTmp2));
    }
//   else if (m_shapeType == "PGON31"){
//     // Outer edge
//     GeoTube *shapeTmp1 = new GeoTube(0,m_rmax,halflength);
//     halflength+=0.1*CLHEP::mm;
//     double alpha = M_PI/m_sides;
//     double cosalpha = cos(alpha);
//     double rmaxB = m_rmin/cosalpha;
//     double rmax2B = m_rmin2/cosalpha;
//     GeoPgon* shapeTmp2 = new GeoPgon(m_phiLoc-alpha,2*M_PI,m_sides);
//     shapeTmp2->addPlane(-halflength,0.,rmaxB);
//     shapeTmp2->addPlane(halflength,0.,rmax2B);
//     // Don't trust boolean volume calculation.
//     volume = shapeTmp1->volume() - shapeTmp2->volume();
//     serviceShape = &(shapeTmp1->subtract(*shapeTmp2));
//   }
    else if (m_shapeType == "ROD") {
      serviceShape = new GeoTube(0, 0.5 * m_phiWidth, halflength);
    } else if (m_shapeType == "ROD2") {
      //    std::cout<<"ROD2 : "<<m_rmin<<" "<<m_rmin2<<" "<<0.5*m_phiWidth<<" "<<halflength<<std::endl;
      serviceShape = new GeoTube(m_rmin2 - m_rmin, 0.5 * m_phiWidth, halflength);
    } else if (m_shapeType == "BOX") {
      serviceShape = new GeoBox(0.5 * (m_rmax - m_rmin), 0.5 * m_phiWidth, halflength);
    } else if (m_shapeType == "TRAP") {
      double thickness = 0.5 * (m_rmax - m_rmin);
      double averad = 0.5 * (m_rmin + m_rmax);
      double w1 = 0.5 * m_phiWidth * m_rmin / averad;
      double w2 = 0.5 * m_phiWidth * m_rmax / averad;
      serviceShape = new GeoTrap(halflength, 0, 0, thickness, w1, w2, 0, thickness, w1, w2, 0);
    } else {
      // msg(MSG::ERROR) << "Unrecognized shape for services" << m_shapeType << endmsg;
      std::cout << "ServiceVolume: ERROR: Unrecognized shape for services" << m_shapeType << std::endl;
    }

    if (!volume && serviceShape != 0) volume = serviceShape->volume();

    m_volume = volume;
    m_geoShape.set(serviceShape);
    return serviceShape;
  }

  double
  ServiceVolume::volume() const {
    // Make sure shape is already built.
    getShape();
    return m_volume;
  }

  void
  ServiceVolume::setGeoShape(const GeoShape* geoShape, double volume) {
    m_geoShape.reset();
    if (geoShape) {
      m_volume = volume;
      // We allow a volume to specified as the volume calculation for some shapes (ie boolean volumes) are unreliable.
      // If volume is not supplied, get it from the shape itself.
      if (!m_volume) m_volume = geoShape->volume();
      m_geoShape.set(geoShape);
      m_lockGeoShape = true; // This disables resetGeoShape().
      setShapeType("CUSTOM");
    } else {
      // If pass null pointer we unlock the shape.
      m_lockGeoShape = false;
    }
  }

  double
  ServiceVolume::origVolume() const {
    if (m_origVolume) return m_origVolume;

    return volume();
  }

  void
  ServiceVolume::setSplittable() {
    m_splittableR = m_splittableZ = true;
    if (m_shapeType == "CUSTOM") {
      m_splittableR = m_splittableZ = false;
    } else if (!(m_shapeType == "" || m_shapeType == "TUBE" || m_shapeType == "TUBS")) {
      m_splittableR = false;
    }
  }

  GeoShapeHolder::GeoShapeHolder()
    : m_geoShape(0)
  {}

  GeoShapeHolder::GeoShapeHolder(const GeoShape* geoShape)
    : m_geoShape(geoShape) {
    if (m_geoShape) m_geoShape->ref();
  }

  GeoShapeHolder::~GeoShapeHolder() {
    reset();
  }

  GeoShapeHolder::GeoShapeHolder(const GeoShapeHolder& rhs) {
    m_geoShape = rhs.m_geoShape;
    if (m_geoShape) m_geoShape->ref();
  }

  GeoShapeHolder&
  GeoShapeHolder::operator = (const GeoShapeHolder& rhs) {
    if (&rhs != this) {
      reset();
      m_geoShape = rhs.m_geoShape;
      if (m_geoShape) m_geoShape->ref();
    }
    return *this;
  }
  
  GeoShapeHolder&
  GeoShapeHolder::operator = (GeoShapeHolder&& rhs) {
    if (&rhs != this) {
      if (m_geoShape) m_geoShape->unref();//this geoshape will be overwritten, so decrement its reference
      m_geoShape = rhs.m_geoShape; //simply equate the pointers
      rhs.m_geoShape=nullptr;      //render the original unusable for safety
      //if (m_geoShape) m_geoShape->ref(); << no need to increment the reference; the original is moved here
      //                                      with its original refcount intact.
    }
    return *this;
  }

  void
  GeoShapeHolder::set(const GeoShape* geoShape) {
    reset();
    m_geoShape = geoShape;
    if (m_geoShape) m_geoShape->ref();
  }

  void
  GeoShapeHolder::reset() {
    if (m_geoShape) m_geoShape->unref();
    m_geoShape = 0;
  }
} // end namespace
