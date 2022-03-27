/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1Utils/SoVisualizeAction.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoCons.h"
#include "GeoModelKernel/GeoPcon.h"
#include "GeoModelKernel/GeoTrap.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoSimplePolygonBrep.h"
#include "GeoModelKernel/GeoTessellatedSolid.h"
#include "GeoModelKernel/GeoFacet.h"
#include "GeoModelKernel/GeoGenericTrap.h"
#include "GeoModelKernel/GeoUnidentifiedShape.h"
#include "VP1HEPVis/nodes/SoTubs.h"
#include "VP1HEPVis/nodes/SoCons.h"
#include "VP1HEPVis/nodes/SoGenericBox.h"
#include "VP1HEPVis/nodes/SoPcons.h"
#include "VP1HEPVis/nodes/SoTessellated.h"
#include "VP1HEPVis/SbPolyhedron.h"
#include "VP1HEPVis/nodes/SoLAr.h"
#include "VP1HEPVis/nodes/SoPolyhedron.h"
#include "VP1HEPVis/VP1HEPVisUtils.h"
#include "VP1Utils/SbPolyhedrizeAction.h"

// System of units
#ifdef BUILDVP1LIGHT
	#include "GeoModelKernel/Units.h"
	#define SYSTEM_OF_UNITS GeoModelKernelUnits // --> 'GeoModelKernelUnits::cm'
#else
  #include "GaudiKernel/SystemOfUnits.h"
  #define SYSTEM_OF_UNITS Gaudi::Units // --> 'Gaudi::Units::cm'
#endif

#include <iostream>

SoVisualizeAction::SoVisualizeAction()
  : m_shape(0)
{
  VP1HEPVisUtils::initAllCustomClasses();
  setDepthLimit(0);
}

SoVisualizeAction::~SoVisualizeAction()
{
  // Don't delete.  Let ref count take care of the memory.
}

void SoVisualizeAction::handleShape(const GeoShape *shape)
{
#ifndef BUILDVP1LIGHT
  const GeoUnidentifiedShape* custom = dynamic_cast< const GeoUnidentifiedShape* >( shape );
  if( custom) {
    std::string customName = custom->name();
    static const double eta_hi  = 3.2;
    static const double eta_mid = 2.5;
    static const double eta_low = 1.375;

    static const double dMechFocaltoWRP      = 3691. *SYSTEM_OF_UNITS::mm;  //=endg_z1
    static const double dWRPtoFrontFace      =   11. *SYSTEM_OF_UNITS::mm;
    static const double rOuterCutoff         = 2034. *SYSTEM_OF_UNITS::mm;  //=endg_rlimit
    static const double activeLength         = 510*SYSTEM_OF_UNITS::mm;
    static const double straightStartSection = 2*SYSTEM_OF_UNITS::mm;
    static const double wheelThickness       = activeLength+2*straightStartSection;

    SoLAr::initClass();
    SoLAr *solar = new SoLAr();
    if (customName.find("Inner")!=std::string::npos) {
      float zPlane[2],rInner[2],rOuter[2];
      zPlane[0]=0;
      zPlane[1]=wheelThickness;

      double tanThetaInner = 2. * exp(-eta_hi ) / (1. - exp(2.*-eta_hi ));
      double tanThetaMid   = 2. * exp(-eta_mid) / (1. - exp(2.*-eta_mid));
      double zWheelFrontFace = dMechFocaltoWRP + dWRPtoFrontFace;
      double zWheelBackFace = zWheelFrontFace + wheelThickness;
      rInner[0] = zWheelFrontFace * tanThetaInner;
      rInner[1] = zWheelBackFace  * tanThetaInner;
      // Note that there is a 3mm gap between the outer surface of the
      // inner wheel and the inner surface of the outer wheel.
      double HalfGapBetweenWheels = 0.15*SYSTEM_OF_UNITS::cm;  // In DB EMECGEOMETRY.DCRACK
      rOuter[0] = zWheelFrontFace * tanThetaMid - HalfGapBetweenWheels;
      rOuter[1] = zWheelBackFace  * tanThetaMid - HalfGapBetweenWheels;
      solar->fRmin.setValues(0,2,rInner);
      solar->fRmax.setValues(0,2,rOuter);
      solar->fDz.setValues  (0,2,zPlane);
    }
    else if (customName.find("Outer")!=std::string::npos) {
      float zPlane[3], rInner[3], rOuter[3];
      zPlane[0] = 0;
      zPlane[2] = wheelThickness;
      double tanThetaMid   = 2. * exp(-eta_mid) / (1. - exp(2.*-eta_mid));
      double tanThetaOuter = 2. * exp(-eta_low) / (1. - exp(2.*-eta_low));
      double zWheelFrontFace = dMechFocaltoWRP + dWRPtoFrontFace;
      double zWheelBackFace = zWheelFrontFace + wheelThickness;
      // Note that there is a 3mm gap between the outer surface of the
      // inner wheel and the inner surface of the outer wheel.
      double HalfGapBetweenWheels = 0.15*SYSTEM_OF_UNITS::cm;  // In DB! (EMECGEOMETRY.DCRACK);
      rInner[0] = zWheelFrontFace * tanThetaMid + HalfGapBetweenWheels;
      rInner[2] = zWheelBackFace  * tanThetaMid + HalfGapBetweenWheels;
      rOuter[0] = zWheelFrontFace * tanThetaOuter;
      rOuter[2] = zWheelBackFace  * tanThetaOuter;
      // If we simply left it at that, the outer wheel would protrude
      // beyond the edge of the cryostat.  We have to "slice off" the lip
      // of the outer wheel to create a shape that's roughtly like a
      // bathtub plug.
      // Use the endpoints of the outer arrays to define lines in the
      // (z,r) plane.
      double slopeMin = (rInner[2] - rInner[0]) / (zPlane[2] - zPlane[0]);
      double slopeMax = (rOuter[2] - rOuter[0]) / (zPlane[2] - zPlane[0]);
      double interceptMin = rInner[0] - slopeMin * zPlane[0];
      double interceptMax = rOuter[0] - slopeMax * zPlane[0];
      // At what value of z does the outer line hit the cutoff?
      zPlane[1] = (rOuterCutoff - interceptMax) / slopeMax;
      // Calculate the radii at this z:
      rInner[1] = slopeMin * zPlane[1] + interceptMin;
      rOuter[1] = rOuterCutoff;
      // Now override the radius to keep the wheel inside the cryostat:
      rOuter[2] = rOuterCutoff;
      solar->fRmin.setValues(0,3,rInner);
      solar->fRmax.setValues(0,3,rOuter);
      solar->fDz.setValues  (0,3,zPlane);
    }

    solar->fSPhi= 0;
    solar->fDPhi= 2*M_PI;
    m_shape=solar;
  } else {
#endif // not BUILDVP1LIGHT
    // We don't recognize it.  Try to polyhedrize it!
    SbPolyhedrizeAction a;
    shape->exec(&a);
    const SbPolyhedron *poly =a.getPolyhedron();
    if (poly) {
      SoPolyhedron *myPoly = new SoPolyhedron(poly);
      m_shape=myPoly;
    }
#ifndef BUILDVP1LIGHT
  }
#endif // not BUILDVP1LIGHT
}

void SoVisualizeAction::handleBox(const GeoBox *box)
{
  //qDebug() << "SoVisualizeAction::handleBox";
  SoGenericBox * gb = new SoGenericBox;
  gb->setParametersForBox( box->getXHalfLength(),box->getYHalfLength(),box->getZHalfLength() );
  m_shape=gb;
}

void SoVisualizeAction::handleCons(const GeoCons *cons)
{
  //qDebug() << "SoVisualizeAction::handleCons";
  SoCons::initClass();
  SoCons *socons= new SoCons;
  socons->fRmin1 =cons->getRMin1();
  socons->fRmin2 =cons->getRMin2();
  socons->fRmax1 =cons->getRMax1();
  socons->fRmax2 =cons->getRMax2();
  socons->fDz    =cons->getDZ();
  socons->fSPhi  =cons->getSPhi();
  socons->fDPhi  =cons->getDPhi();

  m_shape=socons;
}

void SoVisualizeAction::handlePcon(const GeoPcon *pcon)
{

  //qDebug() << "SoVisualizeAction::handlePcon";

  //Set up temporary data arrays for profile:
  float *z  = new float[pcon->getNPlanes()];
  float *rmn= new float[pcon->getNPlanes()];
  float *rmx= new float[pcon->getNPlanes()];

  for (unsigned s=0;s<pcon->getNPlanes();++s) {
    z[s]=pcon->getZPlane(s);
    rmn[s]=pcon->getRMinPlane(s);
    rmx[s]=pcon->getRMaxPlane(s);
  }

  SoPcons::initClass();
  SoPcons *sopcons = new SoPcons();
  sopcons->fSPhi = pcon->getSPhi();
  sopcons->fDPhi = pcon->getDPhi();
  sopcons->fRmin.setValues(0,pcon->getNPlanes(),rmn);
  sopcons->fRmax.setValues(0,pcon->getNPlanes(),rmx);
  sopcons->fDz.setValues  (0,pcon->getNPlanes(),z);

  //Delete temporary arrays:
  delete  [] z;
  delete  [] rmn;
  delete  [] rmx;

  m_shape=sopcons;

}

void SoVisualizeAction::handleTrap(const GeoTrap *trap)
{
  //qDebug() << "SoVisualizeAction::handleTrap";
  SoGenericBox * gb = new SoGenericBox;
  gb->setParametersForTrapezoid(trap->getZHalfLength(), trap->getTheta(), trap->getPhi(),
				trap->getDydzn(), trap->getDxdyndzn(), trap->getDxdypdzn(),
				trap->getDydzp(), trap->getDxdyndzp(), trap->getDxdypdzp(),
				trap->getAngleydzn(), trap->getAngleydzp());
  m_shape=gb;
}

void SoVisualizeAction::handleTrd(const GeoTrd *trd)
{
  //qDebug() << "SoVisualizeAction::handleTrd";
  SoGenericBox * gb = new SoGenericBox;
  gb->setParametersForTrd( trd->getXHalfLength1(), trd->getXHalfLength2(),
			   trd->getYHalfLength1(), trd->getYHalfLength2(),
			   trd->getZHalfLength() );
  m_shape=gb;
}

void SoVisualizeAction::handleTube(const GeoTube *tube)
{
  //qDebug() << "SoVisualizeAction::handleTube";
  SoTubs *sotubs= new SoTubs;
  sotubs->pRMin= tube->getRMin();
  sotubs->pRMax= tube->getRMax();
  sotubs->pDz  = tube->getZHalfLength();
  sotubs->pSPhi= 0;
  sotubs->pDPhi= 2*M_PI;
  m_shape=sotubs;
}

void SoVisualizeAction::handleTubs(const GeoTubs *tubs)
{
  //qDebug() << "SoVisualizeAction::handleTubs";
  SoTubs *sotubs= new SoTubs;
  sotubs->pRMin= tubs->getRMin();
  sotubs->pRMax= tubs->getRMax();
  sotubs->pDz  = tubs->getZHalfLength();
  sotubs->pSPhi= tubs->getSPhi();
  sotubs->pDPhi= tubs->getDPhi();
  m_shape=sotubs;
}

void SoVisualizeAction::handleSimplePolygonBrep(const GeoSimplePolygonBrep *brep)
{
  //qDebug() << "SoVisualizeAction::handleSimplePolygonBrep";
  //Fixme: Detect if order of vertices is the wrong way around... and reorder if necessary.

  double dz = brep->getDZ();
  std::vector<double> x, y;
  for(unsigned int i=0; i<brep->getNVertices(); ++i)
  {
    x.push_back(brep->getXVertex(i));
    y.push_back(brep->getYVertex(i));
  }

  SbPolyhedronPolygonXSect sbPoly(x,y,dz);
  SoPolyhedron* soPoly = new SoPolyhedron(sbPoly);
  m_shape = soPoly;
}

void SoVisualizeAction::handleTessellatedSolid (const GeoTessellatedSolid* geoTessellated)
{
  //qDebug() << "SoVisualizeAction::handleTessellatedSolid";

  SoTessellated * soTessellated = new SoTessellated;
  for(size_t i=0; i<geoTessellated->getNumberOfFacets();++i) {
    GeoFacet* facet = geoTessellated->getFacet(i);
    if(facet->getNumberOfVertices()==3) {
      if(facet->getVertexType()==GeoFacet::ABSOLUTE)
	soTessellated->addTriangularFacet(facet->getVertex(0).x(),facet->getVertex(0).y(),facet->getVertex(0).z(),
					  facet->getVertex(1).x(),facet->getVertex(1).y(),facet->getVertex(1).z(),
					  facet->getVertex(2).x(),facet->getVertex(2).y(),facet->getVertex(2).z());
      else
	soTessellated->addTriangularFacet(facet->getVertex(0).x(),facet->getVertex(0).y(),facet->getVertex(0).z(),
					  facet->getVertex(0).x()+facet->getVertex(1).x(),
					  facet->getVertex(0).y()+facet->getVertex(1).y(),
					  facet->getVertex(0).z()+facet->getVertex(1).z(),
					  facet->getVertex(0).x()+facet->getVertex(2).x(),
					  facet->getVertex(0).y()+facet->getVertex(2).y(),
					  facet->getVertex(0).z()+facet->getVertex(2).z());
    }
    else {
      if(facet->getVertexType()==GeoFacet::ABSOLUTE)
	soTessellated->addQuadrangularFacet(facet->getVertex(0).x(),facet->getVertex(0).y(),facet->getVertex(0).z(),
					    facet->getVertex(1).x(),facet->getVertex(1).y(),facet->getVertex(1).z(),
					    facet->getVertex(2).x(),facet->getVertex(2).y(),facet->getVertex(2).z(),
					    facet->getVertex(3).x(),facet->getVertex(3).y(),facet->getVertex(3).z());
      else
	soTessellated->addQuadrangularFacet(facet->getVertex(0).x(),facet->getVertex(0).y(),facet->getVertex(0).z(),
					    facet->getVertex(0).x()+facet->getVertex(1).x(),
					    facet->getVertex(0).y()+facet->getVertex(1).y(),
					    facet->getVertex(0).z()+facet->getVertex(1).z(),
					    facet->getVertex(0).x()+facet->getVertex(2).x(),
					    facet->getVertex(0).y()+facet->getVertex(2).y(),
					    facet->getVertex(0).z()+facet->getVertex(2).z(),
					    facet->getVertex(0).x()+facet->getVertex(3).x(),
					    facet->getVertex(0).y()+facet->getVertex(3).y(),
					    facet->getVertex(0).z()+facet->getVertex(3).z());

    }
  }
  soTessellated->finalize();
  m_shape=soTessellated;
}

void SoVisualizeAction::handleGenericTrap(const GeoGenericTrap *gentrap)
{
  //qDebug() << "SoVisualizeAction::handleGenericTrap";
  SoGenericBox * gb = new SoGenericBox;
  const GeoGenericTrapVertices& trapVertices = gentrap->getVertices();
  double dZ = gentrap->getZHalfLength();
  gb->setGenericParameters(trapVertices[0].x(),trapVertices[0].y(),-dZ,
			   trapVertices[1].x(),trapVertices[1].y(),-dZ,
			   trapVertices[2].x(),trapVertices[2].y(),-dZ,
			   trapVertices[3].x(),trapVertices[3].y(),-dZ,
			   trapVertices[4].x(),trapVertices[4].y(),dZ,
			   trapVertices[5].x(),trapVertices[5].y(),dZ,
			   trapVertices[6].x(),trapVertices[6].y(),dZ,
			   trapVertices[7].x(),trapVertices[7].y(),dZ);
  m_shape=gb;
}
