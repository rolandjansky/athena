/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Geo2G4SolidFactory.h"
#include "LArWheelSolid.h"
#include "LArWheelSolidDDProxy.h"

#include "GeoSpecialShapes/LArCustomShape.h"

#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoPcon.h"
#include "GeoModelKernel/GeoPgon.h"
#include "GeoModelKernel/GeoPara.h"
#include "GeoModelKernel/GeoTrap.h"
#include "GeoModelKernel/GeoCons.h"
#include "GeoModelKernel/GeoSimplePolygonBrep.h"
#include "GeoModelKernel/GeoTessellatedSolid.h"
#include "GeoModelKernel/GeoEllipticalTube.h"
#include "GeoModelKernel/GeoTorus.h"
#include "GeoModelKernel/GeoGenericTrap.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeIntersection.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"

#include "G4VSolid.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Trd.hh"
#include "G4Polycone.hh"
#include "G4Cons.hh"
#include "G4Polyhedra.hh"
#include "G4Trap.hh"
#include "G4Para.hh"
#include "G4UnionSolid.hh"
#include "G4DisplacedSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4ExtrudedSolid.hh"
#include "G4TessellatedSolid.hh"
#include "G4EllipticalTube.hh"
#include "G4Torus.hh"
#include "G4TriangularFacet.hh"
#include "G4QuadrangularFacet.hh"
#include "G4GenericTrap.hh"

#include <iostream>
#include <map>
#include <cmath>

typedef std::map<const GeoShape*, G4VSolid*, std::less<const GeoShape*> > shapesMap;
typedef std::map<std::string, G4VSolid*,std::less<std::string> > customSolidMap;

const Geo2G4SolidFactory::LArWheelSolid_typemap Geo2G4SolidFactory::s_lwsTypes = {
	{ "LAr::EMEC::InnerWheel::Absorber",		{InnerAbsorberWheel, 1} },
	{ "LAr::EMEC::Pos::InnerWheel::Absorber",	{InnerAbsorberWheel, 1} },

	{ "LAr::EMEC::OuterWheel::Absorber",		{OuterAbsorberWheel, 1} },
	{ "LAr::EMEC::Pos::OuterWheel::Absorber",	{OuterAbsorberWheel, 1} },

	{ "LAr::EMEC::InnerWheel::Electrode",		{InnerElectrodWheel, 1} },
	{ "LAr::EMEC::Pos::InnerWheel::Electrode",	{InnerElectrodWheel, 1} },

	{ "LAr::EMEC::OuterWheel::Electrode",		{OuterElectrodWheel, 1} },
	{ "LAr::EMEC::Pos::OuterWheel::Electrode",	{OuterElectrodWheel, 1} },

	{ "LAr::EMEC::Neg::InnerWheel::Absorber",	{InnerAbsorberWheel, -1} },

	{ "LAr::EMEC::Neg::OuterWheel::Absorber",	{OuterAbsorberWheel, -1} },

	{ "LAr::EMEC::Neg::InnerWheel::Electrode",	{InnerElectrodWheel, -1} },

	{ "LAr::EMEC::Neg::OuterWheel::Electrode",	{OuterElectrodWheel, -1} },

	{ "LAr::EMEC::InnerModule::Absorber",		{InnerAbsorberModule, 1} },

	{ "LAr::EMEC::OuterModule::Absorber",		{OuterAbsorberModule, 1} },

	{ "LAr::EMEC::InnerModule::Electrode",		{InnerElectrodModule, 1} },

	{ "LAr::EMEC::OuterModule::Electrode",	{OuterElectrodModule, 1} },

	{ "LAr::EMEC::InnerWheel::Glue",			{InnerGlueWheel, 1} },
	{ "LAr::EMEC::Pos::InnerWheel::Glue",		{InnerGlueWheel, 1} },

	{ "LAr::EMEC::InnerWheel::Lead",			{InnerLeadWheel, 1} },
	{ "LAr::EMEC::Pos::InnerWheel::Lead",		{InnerLeadWheel, 1} },

	{ "LAr::EMEC::OuterWheel::Glue",			{OuterGlueWheel, 1} },
	{ "LAr::EMEC::Pos::OuterWheel::Glue",		{OuterGlueWheel, 1} },

	{ "LAr::EMEC::OuterWheel::Lead",			{OuterLeadWheel, 1} },
	{ "LAr::EMEC::Pos::OuterWheel::Lead",		{OuterLeadWheel, 1} },

	{ "LAr::EMEC::Neg::InnerWheel::Glue",		{InnerGlueWheel, -1} },

	{ "LAr::EMEC::Neg::OuterWheel::Glue",		{OuterGlueWheel, -1} },

	{ "LAr::EMEC::Neg::InnerWheel::Lead",		{InnerLeadWheel, -1} },

	{ "LAr::EMEC::Neg::OuterWheel::Lead",		{OuterLeadWheel, -1} }
};

Geo2G4SolidFactory::Geo2G4SolidFactory() :
	m_msg("Geo2G4SolidFactory"),
	m_detStore( "StoreGateSvc/DetectorStore", "Geo2G4SolidFactory" )
{
}

G4VSolid *Geo2G4SolidFactory::Build(const GeoShape* geoShape, std::string name) const
{
  G4VSolid* theSolid;

  static customSolidMap customSolids;
  static shapesMap sharedShapes;
  if(sharedShapes.find(geoShape)!=sharedShapes.end())
    return sharedShapes[geoShape];

  // ------- Variables for boolean operations
  G4VSolid* solidA = 0;
  G4VSolid* solidB = 0;
  // ------- Variables for Pcon and Pgon
  int nPlanes;
  double* zPlane = 0;
  double* rInner = 0;
  double* rOuter = 0;

  std::string n = name;

  //
  // The Box
  //
  if(geoShape->typeID() == GeoBox::getClassTypeID() )
    {
      const GeoBox* theBox = dynamic_cast<const GeoBox*> (geoShape);
      if (0==theBox) throw std::runtime_error("TypeID did not match cast for box");
      if (n.empty()) n="G4Box";
      if (theBox->getXHalfLength()<=0.){ msg(MSG::WARNING) << "Box " << n << " has an x side of " << theBox->getXHalfLength() <<" - using std::abs." << endreq;}
      if (theBox->getYHalfLength()<=0.){ msg(MSG::WARNING) << "Box " << n << " has an y side of " << theBox->getYHalfLength() <<" - using std::abs." << endreq;}
      if (theBox->getZHalfLength()<=0.){ msg(MSG::WARNING) << "Box " << n << " has an z side of " << theBox->getZHalfLength() <<" - using std::abs." << endreq;}
      theSolid = new G4Box(n,
                           std::abs(theBox->getXHalfLength()),
                           std::abs(theBox->getYHalfLength()),
                           std::abs(theBox->getZHalfLength()));
    }
  //
  // The Tube
  //
  else if(geoShape->typeID() == GeoTube::getClassTypeID() )
    {
      const GeoTube* theTube = dynamic_cast<const GeoTube*> (geoShape);
      if (0==theTube) throw std::runtime_error("TypeID did not match cast for tube");
      if (n.empty()) n="G4Tube";
      if (theTube->getRMax()<=0.){ msg(MSG::WARNING) << "Tube " << n << " has a max radius of " << theTube->getRMax() <<" - using std::abs." << endreq;}
      if (theTube->getZHalfLength()<=0.){ msg(MSG::WARNING) << "Tube " << n << " has a z half length of " << theTube->getZHalfLength() << " - using std::abs." << endreq;}
      if (theTube->getRMax()<theTube->getRMin()){ msg(MSG::WARNING) << "Tube " << n << " has a max radius of " << theTube->getRMax() << " and a min radius of " << theTube->getRMin() << endreq;}
      theSolid = new G4Tubs(n,
                            theTube->getRMin(),
                            std::abs(theTube->getRMax()),
                            std::abs(theTube->getZHalfLength()),
                            0.,360*CLHEP::deg);
    }
  //
  // The Tubs
  //
  else if(geoShape->typeID() == GeoTubs::getClassTypeID() )
    {
      const GeoTubs* theTubs = dynamic_cast<const GeoTubs*> (geoShape);
      if (0==theTubs) throw std::runtime_error("TypeID did not match cast for tubs");
      if (n.empty()) n="G4Tubs";
      if (theTubs->getRMin()<0.){ msg(MSG::WARNING) << "Tubs " << n << " has a min radius of " << theTubs->getRMax() << endreq;}
      if (theTubs->getRMax()<=0.){ msg(MSG::WARNING) << "Tubs " << n << " has a max radius of " << theTubs->getRMax() <<" - using std::abs." << endreq;}
      if (theTubs->getZHalfLength()<=0.){ msg(MSG::WARNING) << "Tubs " << n << " has a half length of " << theTubs->getZHalfLength() <<" - using std::abs." << endreq;}
      if (theTubs->getRMax()<theTubs->getRMin()){ msg(MSG::WARNING) << "Tubs " << n << " has a max radius of " << theTubs->getRMax() << " and a min radius of " << theTubs->getRMin() << endreq;}
      if (theTubs->getDPhi()<=0.){ msg(MSG::WARNING) << "Tubs " << n << " has a dPhi of " << theTubs->getDPhi() << endreq;}
      theSolid = new G4Tubs(n,
                            theTubs->getRMin(),
                            std::abs(theTubs->getRMax()),
                            std::abs(theTubs->getZHalfLength()),
                            theTubs->getSPhi(),
                            theTubs->getDPhi());
    }
  //
  // The Trd
  //
  else if(geoShape->typeID() == GeoTrd::getClassTypeID() )
    {
      const GeoTrd* theTrd = dynamic_cast<const GeoTrd*> (geoShape);
      if (0==theTrd) throw std::runtime_error("TypeID did not match cast for trd");
      if (n.empty()) n="G4Trd";
      if (theTrd->getXHalfLength1()<0.){ msg(MSG::WARNING) << "Trd " << n << " has a x half length 1 of " << theTrd->getXHalfLength1() << " - using std::abs." << endreq;}
      if (theTrd->getXHalfLength2()<0.){ msg(MSG::WARNING) << "Trd " << n << " has a x half length 2 of " << theTrd->getXHalfLength2() << " - using std::abs." << endreq;}
      if (theTrd->getYHalfLength1()<0.){ msg(MSG::WARNING) << "Trd " << n << " has a y half length 1 of " << theTrd->getYHalfLength1() << " - using std::abs." << endreq;}
      if (theTrd->getYHalfLength2()<0.){ msg(MSG::WARNING) << "Trd " << n << " has a y half length 2 of " << theTrd->getYHalfLength2() << " - using std::abs." << endreq;}
      if (theTrd->getZHalfLength()<=0.){ msg(MSG::WARNING) << "Trd " << n << " has a z half length of " << theTrd->getZHalfLength() << " - using std::abs." << endreq;}
      if (theTrd->getXHalfLength1()<=0. && theTrd->getXHalfLength2()<=0.){ msg(MSG::WARNING) << "Trd " << n << " has an x half length 1 of " << theTrd->getXHalfLength1()
                                                                                             << " and an x half length 2 of " << theTrd->getXHalfLength2() << " - using std::abs." << endreq;}
      if (theTrd->getYHalfLength1()<=0. && theTrd->getYHalfLength2()<=0.){ msg(MSG::WARNING) << "Trd " << n << " has a y half length 1 of " << theTrd->getYHalfLength1()
                                                                                             << " and a y half length 2 of " << theTrd->getYHalfLength2() << " - using std::abs." << endreq;}
      theSolid = new G4Trd(n,
                           std::abs(theTrd->getXHalfLength1()),
                           std::abs(theTrd->getXHalfLength2()),
                           std::abs(theTrd->getYHalfLength1()),
                           std::abs(theTrd->getYHalfLength2()),
                           std::abs(theTrd->getZHalfLength()));
    }
  //
  // GeoPcon
  //
  else if(geoShape->typeID() == GeoPcon::getClassTypeID())
    {
      const GeoPcon* thePcon = dynamic_cast<const GeoPcon*>(geoShape);
      if (0==thePcon) throw std::runtime_error("TypeID did not match cast for pcon");
      if (n.empty()) n="G4Polycone";
      nPlanes = static_cast<int>(thePcon->getNPlanes());
      zPlane = new double[nPlanes];
      rInner = new double[nPlanes];
      rOuter = new double[nPlanes];
      for (unsigned int index=0; index<static_cast<unsigned int>(nPlanes); index++)
        {
          zPlane[index] = thePcon->getZPlane(index);
          rInner[index] = thePcon->getRMinPlane(index);
          rOuter[index] = thePcon->getRMaxPlane(index);
          if (rInner[index]<0.){ msg(MSG::WARNING) << "PCon " << n << " has an inner radius of " << rInner[index] << " for slice " << index << " of " << nPlanes << endreq;}
          if (rOuter[index]<=0.){
            msg(MSG::WARNING) << "PCon " << n << " has an outer radius of " << rOuter[index] << " for slice " << index << " of " << nPlanes << " - using std::abs." << endreq;
            rOuter[index] = std::abs(rOuter[index]);
          }
        }

      theSolid = new G4Polycone(n,
                                thePcon->getSPhi(),
                                thePcon->getDPhi(),
                                nPlanes,
                                zPlane,
                                rInner,
                                rOuter);
    }
  //
  // GeoCons
  //
  else if(geoShape->typeID() == GeoCons::getClassTypeID())
    {
      const GeoCons* theCons = dynamic_cast<const GeoCons*>(geoShape);
      if (0==theCons) throw std::runtime_error("TypeID did not match cast for cons");
      if (n.empty()) n="G4Cons";
      if (theCons->getRMax1()<0.){ msg(MSG::WARNING) << "Cons " << n << " has a max radius 1 of " << theCons->getRMax1() << " - will use std::abs." << endreq;}
      if (theCons->getRMax2()<0.){ msg(MSG::WARNING) << "Cons " << n << " has a max radius 2 of " << theCons->getRMax2() << " - will use std::abs." << endreq;}
      if (theCons->getRMin1()<0.){ msg(MSG::WARNING) << "Cons " << n << " has a min radius 1 of " << theCons->getRMin1() << endreq;}
      if (theCons->getRMin2()<0.){ msg(MSG::WARNING) << "Cons " << n << " has a min radius 2 of " << theCons->getRMin2() << endreq;}
      if (theCons->getDZ()<=0){    msg(MSG::WARNING) << "Cons " << n << " has a DZ of " << theCons->getDZ() << " - will use std::abs." << endreq;}
      if (theCons->getRMax1()<=0. && theCons->getRMax2()<=0.){ msg(MSG::WARNING) << "Cons " << n << " has a max radius 1 of " << theCons->getRMax1()
                                                                                 << " and a max radius 2 of " << theCons->getRMax2() << " - will use std::abs." << endreq;}
      theSolid = new G4Cons(n,
                            theCons->getRMin1(),
                            std::abs(theCons->getRMax1()),
                            theCons->getRMin2(),
                            std::abs(theCons->getRMax2()),
                            std::abs(theCons->getDZ()),
                            theCons->getSPhi(),
                            theCons->getDPhi());
    }
  //
  // GeoPara
  //
  else if(geoShape->typeID() == GeoPara::getClassTypeID())
    {
      const GeoPara* thePara = dynamic_cast<const GeoPara*>(geoShape);
      if (0==thePara) throw std::runtime_error("TypeID did not match cast for para");
      if (n.empty()) n="G4Para";
      if (thePara->getXHalfLength()<=0.){ msg(MSG::WARNING) << "Para " << n << " has an x side of " << thePara->getXHalfLength() <<" - using std::abs." << endreq;}
      if (thePara->getYHalfLength()<=0.){ msg(MSG::WARNING) << "Para " << n << " has an y side of " << thePara->getYHalfLength() <<" - using std::abs." << endreq;}
      if (thePara->getZHalfLength()<=0.){ msg(MSG::WARNING) << "Para " << n << " has an z side of " << thePara->getZHalfLength() <<" - using std::abs." << endreq;}
      theSolid = new G4Para(n,
                            std::abs(thePara->getXHalfLength()),
                            std::abs(thePara->getYHalfLength()),
                            std::abs(thePara->getZHalfLength()),
                            thePara->getAlpha(),
                            thePara->getTheta(),
                            thePara->getPhi());
    }
  //
  // GeoPgon
  //
  else if(geoShape->typeID() == GeoPgon::getClassTypeID())
    {
      const GeoPgon* thePgon = dynamic_cast<const GeoPgon*>(geoShape);
      if (0==thePgon) throw std::runtime_error("TypeID did not match cast for pgon");
      if (n.empty()) n="G4Polyhedra";
      nPlanes = static_cast<int>(thePgon->getNPlanes());
      zPlane = new double[nPlanes];
      rInner = new double[nPlanes];
      rOuter = new double[nPlanes];
      double alpha = thePgon->getDPhi()/(2*thePgon->getNSides());  // 1/2 openning angle
      for (unsigned int index=0; index<static_cast<unsigned int>(nPlanes); index++)
        {
          zPlane[index] = thePgon->getZPlane(index);
          rInner[index] = thePgon->getRMinPlane(index)*cos(alpha);
          rOuter[index] = thePgon->getRMaxPlane(index)*cos(alpha);
          if (rInner[index]<0.){ msg(MSG::WARNING) << "Pgon " << n << " has an inner radius of " << rInner[index] << " for slice " << index << " of " << nPlanes << endreq;}
          if (rOuter[index]<=0.){
            msg(MSG::WARNING) << "Pgon " << n << " has an outer radius of " << rOuter[index] << " for slice " << index << " of " << nPlanes << " - using std::abs." << endreq;
            rOuter[index] = std::abs(rOuter[index]);
          }
        }

      theSolid = new G4Polyhedra(n,
                                 thePgon->getSPhi(),
                                 thePgon->getDPhi(),
                                 thePgon->getNSides(),
                                 nPlanes,
                                 zPlane,
                                 rInner,
                                 rOuter);
    }
  //
  // GeoTrap
  //
  else if(geoShape->typeID() == GeoTrap::getClassTypeID())
    {
      const GeoTrap* theTrap = dynamic_cast<const GeoTrap*>(geoShape);
      if (0==theTrap) throw std::runtime_error("TypeID did not match cast for trap");
      if (n.empty()) n="G4Trap";
      if (theTrap->getZHalfLength()<=0.){ msg(MSG::WARNING) << "Trap " << n << " has an z side of " << theTrap->getZHalfLength() <<" - using std::abs." << endreq;}
      theSolid = new G4Trap(n,
                            std::abs(theTrap->getZHalfLength()),
                            theTrap->getTheta(),
                            theTrap->getPhi(),
                            theTrap->getDydzn(),
                            theTrap->getDxdyndzn(),
                            theTrap->getDxdypdzn(),
                            theTrap->getAngleydzn(),
                            theTrap->getDydzp(),
                            theTrap->getDxdyndzp(),
                            theTrap->getDxdypdzp(),
                            theTrap->getAngleydzp());
    }
  //
  // Simple Polygon Brep
  //
  else if(geoShape->typeID() == GeoSimplePolygonBrep::getClassTypeID())
    {
      const GeoSimplePolygonBrep* theBrep = dynamic_cast<const GeoSimplePolygonBrep*>(geoShape);
      if (0==theBrep) throw std::runtime_error("TypeID did not match cast for brep");
      if (n.empty()) n="G4ExtrudedSolid";
      double dz = theBrep->getDZ();
      int nVertices = theBrep->getNVertices();

      G4TwoVector off(0,0);
      std::vector<G4TwoVector> polygon;

      for(int i=0; i<nVertices; i++)
        polygon.push_back(G4TwoVector(theBrep->getXVertex(nVertices-1-i),theBrep->getYVertex(nVertices-1-i)));

      theSolid = new G4ExtrudedSolid(n,polygon,dz,off,1,off,1);
    }
  //
  // Tessellated Solid
  //
  else if(geoShape->typeID() == GeoTessellatedSolid::getClassTypeID())
    {
      const GeoTessellatedSolid* theTessellated = dynamic_cast<const GeoTessellatedSolid*>(geoShape);
      if (0==theTessellated) throw std::runtime_error("TypeID did not match cast for tessellated solid");
      if(n.empty()) n="G4TessellatedSolid";

      G4TessellatedSolid* g4Tessellated = new G4TessellatedSolid(n);
      for(size_t i=0; i<theTessellated->getNumberOfFacets(); ++i) {
        GeoFacet* geoFacet = theTessellated->getFacet(i);
        G4FacetVertexType vertexType = (geoFacet->getVertexType()==GeoFacet::ABSOLUTE? ABSOLUTE : RELATIVE);
        G4VFacet* g4Facet(0);
        if(geoFacet->getNumberOfVertices()==3)
          g4Facet = new G4TriangularFacet(geoFacet->getVertex(0),
                                          geoFacet->getVertex(1),
                                          geoFacet->getVertex(2),
                                          vertexType);
        else
          g4Facet = new G4QuadrangularFacet(geoFacet->getVertex(0),
                                            geoFacet->getVertex(1),
                                            geoFacet->getVertex(2),
                                            geoFacet->getVertex(3),
                                            vertexType);

        g4Tessellated->AddFacet(g4Facet);
      }
      g4Tessellated->SetSolidClosed(true);
      theSolid = g4Tessellated;
    }
  //
  // Elliptical Tube
  //
  else if(geoShape->typeID() == GeoEllipticalTube::getClassTypeID())
    {
      const GeoEllipticalTube* theEltube = dynamic_cast<const GeoEllipticalTube*>(geoShape);
      if (0==theEltube) throw std::runtime_error("TypeID did not match cast for elliptical tube");
      if (n.empty()) n="G4EllipticalTube";

      if (theEltube->getXHalfLength()<=0.){ msg(MSG::WARNING) << "Eltube " << n << " has an x side of " << theEltube->getXHalfLength() <<" - using std::abs." << endreq;}
      if (theEltube->getYHalfLength()<=0.){ msg(MSG::WARNING) << "Eltube " << n << " has an y side of " << theEltube->getYHalfLength() <<" - using std::abs." << endreq;}
      if (theEltube->getZHalfLength()<=0.){ msg(MSG::WARNING) << "Eltube " << n << " has an z side of " << theEltube->getZHalfLength() <<" - using std::abs." << endreq;}
      G4EllipticalTube* g4Eltube = new G4EllipticalTube(n
                                                        ,std::abs(theEltube->getXHalfLength())
                                                        ,std::abs(theEltube->getYHalfLength())
                                                        ,std::abs(theEltube->getZHalfLength()));
      theSolid = g4Eltube;
    }
  //
  // Torus
  //
  else if(geoShape->typeID() == GeoTorus::getClassTypeID() ) {
    const GeoTorus* theTorus = dynamic_cast<const GeoTorus*> (geoShape);
    if (0==theTorus) throw std::runtime_error("TypeID did not match cast for torus");
    if (n.empty()) n="G4Torus";
    
    theSolid = new G4Torus(n,
			   theTorus->getRMin(),
			   theTorus->getRMax(),
			   theTorus->getRTor(),
			   theTorus->getSPhi(),
			   theTorus->getDPhi());
  }
  //
  // Generic Trap
  //
  else if(geoShape->typeID() == GeoGenericTrap::getClassTypeID()) {
    const GeoGenericTrap* theGenTrap = dynamic_cast<const GeoGenericTrap*>(geoShape);
    if (0==theGenTrap) throw std::runtime_error("TypeID did not match cast for generic trap");
    if (n.empty()) n="G4GenericTrap";
    if (theGenTrap->getZHalfLength()<=0.){ msg(MSG::WARNING) << "GenTrap " << n << " has an z side of " << theGenTrap->getZHalfLength() <<" - using std::abs." << endreq;}
    G4GenericTrap* g4GenTrap = new G4GenericTrap(n
                                                 ,std::abs(theGenTrap->getZHalfLength())
                                                 ,theGenTrap->getVertices());
    theSolid = g4GenTrap;
  }
  //
  // Boolean volumes
  // We make use of G4DisplacedSolid which is not documented in the manual but makes the implementation
  // a lot simpler.
  //
  // GeoShapeShift
  //
  else if (geoShape->typeID() == GeoShapeShift::getClassTypeID() )
    {
      const GeoShapeShift* theShapeShift = dynamic_cast<const GeoShapeShift*> (geoShape);
      if (0==theShapeShift) throw std::runtime_error("TypeID did not match cast for shape shift");
      if (n.empty()) n="DisplacedSolid";
      G4VSolid * undisplacedSolid = Build(theShapeShift->getOp());
      theSolid = new G4DisplacedSolid(n, undisplacedSolid, theShapeShift->getX());
    }
  //
  // GeoShapeUnion
  //
  else if (geoShape->typeID() == GeoShapeUnion::getClassTypeID() )
    {
      const GeoShapeUnion* theUnion = dynamic_cast<const GeoShapeUnion*> (geoShape);
      if (0==theUnion) throw std::runtime_error("TypeID did not match cast for union");
      if (n.empty()) n="Union";
      solidA = Build(theUnion->getOpA());
      solidB = Build(theUnion->getOpB());
      theSolid = new G4UnionSolid(n, solidA, solidB);
    }
  //
  // GeoShapeIntersection
  //
  else if (geoShape->typeID() == GeoShapeIntersection::getClassTypeID() )
    {
      const GeoShapeIntersection* theIntersection = dynamic_cast<const GeoShapeIntersection*>(geoShape);
      if (0==theIntersection) throw std::runtime_error("TypeID did not match cast for intersection");
      if (n.empty()) n="Intersection";
      solidA = Build(theIntersection->getOpA());
      solidB = Build(theIntersection->getOpB());
      theSolid = new G4IntersectionSolid(n, solidA, solidB);
    }
  //
  // GeoShapeSubtraction
  //
  else if (geoShape->typeID() == GeoShapeSubtraction::getClassTypeID() )
    {
      const GeoShapeSubtraction* theSubtraction = dynamic_cast<const GeoShapeSubtraction*>(geoShape);
      if (0==theSubtraction) throw std::runtime_error("TypeID did not match cast for subtraction");
      if (n.empty()) n="Subtraction";
      solidA = Build(theSubtraction->getOpA());
      solidB = Build(theSubtraction->getOpB());
      theSolid = new G4SubtractionSolid(n, solidA, solidB);
    }
  //
  // Custom Shapes (presently LAr shapes only)
  //
  else if(geoShape->typeID() == LArCustomShape::getClassTypeID())
    {
      const LArCustomShape* customShape = dynamic_cast<const LArCustomShape*> (geoShape);
      if (0==customShape) throw std::runtime_error("TypeID did not match cast for custom shape");
      std::string customName = customShape->name();
      customSolidMap::const_iterator it = customSolids.find(customName);
      if(it!=customSolids.end())
        theSolid = it->second;
      else
        {
          theSolid = 0;
//          if(customName == "LAr::EMEC::InnerWheel::Absorber" || customName == "LAr::EMEC::Pos::InnerWheel::Absorber"){
//           theSolid = new LArWheelSolid(customName, InnerAbsorberWheel, 1);
//         } else if(customName == "LAr::EMEC::OuterWheel::Absorber" ||  customName == "LAr::EMEC::Pos::OuterWheel::Absorber"){
//            theSolid = new LArWheelSolid(customName, OuterAbsorberWheel, 1);
//          } else if(customName == "LAr::EMEC::InnerWheel::Electrode" || customName == "LAr::EMEC::Pos::InnerWheel::Electrode"){
//            theSolid = new LArWheelSolid(customName, InnerElectrodWheel, 1);
//          } else if(customName == "LAr::EMEC::OuterWheel::Electrode" || customName == "LAr::EMEC::Pos::OuterWheel::Electrode"){
//            theSolid = new LArWheelSolid(customName, OuterElectrodWheel, 1);
//          } else if(customName == "LAr::EMEC::Neg::InnerWheel::Absorber"){
//            theSolid = new LArWheelSolid(customName, InnerAbsorberWheel, -1);
//          } else if(customName == "LAr::EMEC::Neg::OuterWheel::Absorber"){
//            theSolid = new LArWheelSolid(customName, OuterAbsorberWheel, -1);
//          } else if(customName == "LAr::EMEC::Neg::InnerWheel::Electrode"){
//            theSolid = new LArWheelSolid(customName, InnerElectrodWheel, -1);
//          } else if(customName == "LAr::EMEC::Neg::OuterWheel::Electrode"){
//            theSolid = new LArWheelSolid(customName, OuterElectrodWheel, -1);
//          } else if(customName == "LAr::EMEC::InnerModule::Absorber"){
//            theSolid = new LArWheelSolid(customName, InnerAbsorberModule, 1);
//          } else if(customName == "LAr::EMEC::OuterModule::Absorber"){
//            theSolid = new LArWheelSolid(customName, OuterAbsorberModule, 1);
//          } else if(customName == "LAr::EMEC::InnerModule::Electrode"){
//            theSolid = new LArWheelSolid(customName, InnerElectrodModule, 1);
//          } else if(customName == "LAr::EMEC::OuterModule::Electrode"){
//            theSolid = new LArWheelSolid(customName, OuterElectrodModule, 1);
//          } else if(customName == "LAr::EMEC::InnerWheel::Glue" ||  customName == "LAr::EMEC::Pos::InnerWheel::Glue"){
//            theSolid = new LArWheelSolid(customName, InnerGlueWheel, 1);
//          } else if(customName == "LAr::EMEC::InnerWheel::Lead" ||  customName == "LAr::EMEC::Pos::InnerWheel::Lead"){
//            theSolid = new LArWheelSolid(customName, InnerLeadWheel, 1);
//          } else if(customName == "LAr::EMEC::OuterWheel::Glue" ||  customName == "LAr::EMEC::Pos::OuterWheel::Glue"){
//            theSolid = new LArWheelSolid(customName, OuterGlueWheel, 1);
//          } else if(customName == "LAr::EMEC::OuterWheel::Lead" ||  customName == "LAr::EMEC::Pos::OuterWheel::Lead"){
//            theSolid = new LArWheelSolid(customName, OuterLeadWheel, 1);
//          } else if(customName == "LAr::EMEC::Neg::InnerWheel::Glue"){
//            theSolid = new LArWheelSolid(customName, InnerGlueWheel, -1);
//          } else if(customName == "LAr::EMEC::Neg::InnerWheel::Lead"){
//            theSolid = new LArWheelSolid(customName, InnerLeadWheel, -1);
//          } else if(customName == "LAr::EMEC::Neg::OuterWheel::Glue"){
//            theSolid = new LArWheelSolid(customName, OuterGlueWheel, -1);
//          } else if(customName == "LAr::EMEC::Neg::OuterWheel::Lead"){
//            theSolid = new LArWheelSolid(customName, OuterLeadWheel, -1);
//          }

	theSolid = createLArWheelSolid(customName, s_lwsTypes.at(customName) ); // map.at throws std::out_of_range exception on unknown shape name
	if ( 0 == theSolid ) { 
		std::string error = std::string("Can't create LArWheelSolid for name ") + customName + " in Geo2G4SolidFactory::Build";
		throw std::runtime_error(error);
	}

          if(theSolid != 0) customSolids[customName] = theSolid;
        }
    }
  //
  // Catch All
  //
  else
    {
      msg(MSG::FATAL) << "Sorry this solid is not yet implemented... " <<endreq;
      msg(MSG::FATAL) << geoShape->type() << endreq;
      msg(MSG::FATAL) << "You will have a core dump..." << endreq;
      return 0;
    }

  sharedShapes[geoShape] = theSolid;
  return theSolid;
}

G4VSolid* Geo2G4SolidFactory::createLArWheelSolid(const std::string& name, const LArWheelSolidDef_t & lwsdef) const { // LArWheelSolid_t wheelType, int zside
	LArWheelSolid_t wheelType = lwsdef.first;
	int zside = lwsdef.second;
	
        LArWheelSolid * theLWS = new LArWheelSolid(name, wheelType, zside);

	LArWheelSolidDDProxy * theLWS_p = new LArWheelSolidDDProxy(theLWS);
	// ownership is passed to detStore
	if ( detStore()->record(theLWS_p,  name).isFailure() ) {
		msg(MSG::WARNING) << "Can't store proxy for LArWheelSolid to the DetectorStore" <<endreq;
		delete theLWS_p;
	}
	return theLWS;
}

