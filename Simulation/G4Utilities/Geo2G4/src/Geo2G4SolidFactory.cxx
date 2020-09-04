/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Geo2G4SolidFactory.h"
#include "LArWheelSolid.h"
#include "LArWheelSliceSolid.h"
#include "LArWheelSolidDDProxy.h"


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
#include "GeoModelKernel/GeoUnidentifiedShape.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

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
#include <utility>

#include "GeoPrimitives/CLHEPtoEigenConverter.h"

typedef std::map<const GeoShape*, G4VSolid*, std::less<const GeoShape*> > shapesMap;
typedef std::map<std::string, G4VSolid*,std::less<std::string> > customSolidMap;

const Geo2G4SolidFactory::LArWheelSolid_typemap Geo2G4SolidFactory::s_lwsTypes = {
/* entries w/o explicit Pos/Neg kept for backward compatibility */
    { "LAr::EMEC::InnerWheel::Absorber",           {InnerAbsorberWheel, 1} },
    { "LAr::EMEC::InnerWheel::Electrode",          {InnerElectrodWheel, 1} },
    { "LAr::EMEC::InnerWheel::Glue",               {InnerGlueWheel, 1} },
    { "LAr::EMEC::InnerWheel::Lead",               {InnerLeadWheel, 1} },

    { "LAr::EMEC::OuterWheel::Absorber",           {OuterAbsorberWheel, 1} },
    { "LAr::EMEC::OuterWheel::Electrode",          {OuterElectrodWheel, 1} },
    { "LAr::EMEC::OuterWheel::Glue",               {OuterGlueWheel, 1} },
    { "LAr::EMEC::OuterWheel::Lead",               {OuterLeadWheel, 1} },

    { "LAr::EMEC::Pos::InnerWheel::Absorber",      {InnerAbsorberWheel, 1} },
    { "LAr::EMEC::Pos::InnerWheel::Electrode",     {InnerElectrodWheel, 1} },
    { "LAr::EMEC::Pos::InnerWheel::Glue",          {InnerGlueWheel, 1} },
    { "LAr::EMEC::Pos::InnerWheel::Lead",          {InnerLeadWheel, 1} },

    { "LAr::EMEC::Pos::OuterWheel::Absorber",      {OuterAbsorberWheel, 1} },
    { "LAr::EMEC::Pos::OuterWheel::Electrode",     {OuterElectrodWheel, 1} },
    { "LAr::EMEC::Pos::OuterWheel::Glue",          {OuterGlueWheel, 1} },
    { "LAr::EMEC::Pos::OuterWheel::Lead",          {OuterLeadWheel, 1} },

    { "LAr::EMEC::Neg::InnerWheel::Absorber",      {InnerAbsorberWheel, -1} },
    { "LAr::EMEC::Neg::InnerWheel::Electrode",     {InnerElectrodWheel, -1} },
    { "LAr::EMEC::Neg::InnerWheel::Glue",          {InnerGlueWheel, -1} },
    { "LAr::EMEC::Neg::InnerWheel::Lead",          {InnerLeadWheel, -1} },

    { "LAr::EMEC::Neg::OuterWheel::Absorber",      {OuterAbsorberWheel, -1} },
    { "LAr::EMEC::Neg::OuterWheel::Electrode",     {OuterElectrodWheel, -1} },
    { "LAr::EMEC::Neg::OuterWheel::Glue",          {OuterGlueWheel, -1} },
    { "LAr::EMEC::Neg::OuterWheel::Lead",          {OuterLeadWheel, -1} },

    { "LAr::EMEC::InnerModule::Absorber",          {InnerAbsorberModule, 1} },
    { "LAr::EMEC::InnerModule::Electrode",         {InnerElectrodModule, 1} },
    { "LAr::EMEC::OuterModule::Absorber",          {OuterAbsorberModule, 1} },
    { "LAr::EMEC::OuterModule::Electrode",         {OuterElectrodModule, 1} },

    { "LAr::EMEC::Pos::InnerCone::Absorber",       {InnerAbsorberCone, 1} },
    { "LAr::EMEC::Pos::InnerCone::Electrode",      {InnerElectrodCone, 1} },
    { "LAr::EMEC::Pos::InnerCone::Glue",           {InnerGlueCone, 1} },
    { "LAr::EMEC::Pos::InnerCone::Lead",           {InnerLeadCone, 1} },

    { "LAr::EMEC::Neg::InnerCone::Absorber",       {InnerAbsorberCone, -1} },
    { "LAr::EMEC::Neg::InnerCone::Electrode",      {InnerElectrodCone, -1} },
    { "LAr::EMEC::Neg::InnerCone::Glue",           {InnerGlueCone, -1} },
    { "LAr::EMEC::Neg::InnerCone::Lead",           {InnerLeadCone, -1} },

    { "LAr::EMEC::Pos::OuterFrontCone::Absorber",  {OuterAbsorberFrontCone, 1} },
    { "LAr::EMEC::Pos::OuterFrontCone::Electrode", {OuterElectrodFrontCone, 1} },
    { "LAr::EMEC::Pos::OuterFrontCone::Glue",      {OuterGlueFrontCone, 1} },
    { "LAr::EMEC::Pos::OuterFrontCone::Lead",      {OuterLeadFrontCone, 1} },

    { "LAr::EMEC::Neg::OuterFrontCone::Absorber",  {OuterAbsorberFrontCone, -1} },
    { "LAr::EMEC::Neg::OuterFrontCone::Electrode", {OuterElectrodFrontCone, -1} },
    { "LAr::EMEC::Neg::OuterFrontCone::Glue",      {OuterGlueFrontCone, -1} },
    { "LAr::EMEC::Neg::OuterFrontCone::Lead",      {OuterLeadFrontCone, -1} },

    { "LAr::EMEC::Pos::OuterBackCone::Absorber",   {OuterAbsorberBackCone, 1} },
    { "LAr::EMEC::Pos::OuterBackCone::Electrode",  {OuterElectrodBackCone, 1} },
    { "LAr::EMEC::Pos::OuterBackCone::Glue",       {OuterGlueBackCone, 1} },
    { "LAr::EMEC::Pos::OuterBackCone::Lead",       {OuterLeadBackCone, 1} },

    { "LAr::EMEC::Neg::OuterBackCone::Absorber",   {OuterAbsorberBackCone, -1} },
    { "LAr::EMEC::Neg::OuterBackCone::Electrode",  {OuterElectrodBackCone, -1} },
    { "LAr::EMEC::Neg::OuterBackCone::Glue",       {OuterGlueBackCone, -1} },
    { "LAr::EMEC::Neg::OuterBackCone::Lead",       {OuterLeadBackCone, -1} },
};

Geo2G4SolidFactory::Geo2G4SolidFactory() :
    m_msg("Geo2G4SolidFactory"),
    m_detStore( "StoreGateSvc/DetectorStore", "Geo2G4SolidFactory" )
{
}

G4VSolid *Geo2G4SolidFactory::Build(const GeoShape* geoShape, std::string name) const
{
  G4VSolid* theSolid(nullptr);

  static customSolidMap customSolids;
  static shapesMap sharedShapes;
  if(sharedShapes.find(geoShape)!=sharedShapes.end())
    return sharedShapes[geoShape];

  // ------- Variables for boolean operations
  G4VSolid* solidA(nullptr);
  G4VSolid* solidB(nullptr);
  // ------- Variables for Pcon and Pgon
  int nPlanes;
  double* zPlane(nullptr);
  double* rInner(nullptr);
  double* rOuter(nullptr);

  std::string n = std::move(name);

  //
  // The Box
  //
  if(geoShape->typeID() == GeoBox::getClassTypeID() )
    {
      const GeoBox* theBox = dynamic_cast<const GeoBox*> (geoShape);
      if (nullptr==theBox) throw std::runtime_error("TypeID did not match cast for box");
      if (n.empty()) n="G4Box";
      if (theBox->getXHalfLength()<=0.){ ATH_MSG_WARNING("Box " << n << " has an x side of " << theBox->getXHalfLength() <<" - using std::abs.");}
      if (theBox->getYHalfLength()<=0.){ ATH_MSG_WARNING("Box " << n << " has an y side of " << theBox->getYHalfLength() <<" - using std::abs.");}
      if (theBox->getZHalfLength()<=0.){ ATH_MSG_WARNING("Box " << n << " has an z side of " << theBox->getZHalfLength() <<" - using std::abs.");}
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
      if (nullptr==theTube) throw std::runtime_error("TypeID did not match cast for tube");
      if (n.empty()) n="G4Tube";
      if (theTube->getRMax()<=0.){ ATH_MSG_WARNING("Tube " << n << " has a max radius of " << theTube->getRMax() <<" - using std::abs.");}
      if (theTube->getZHalfLength()<=0.){ ATH_MSG_WARNING("Tube " << n << " has a z half length of " << theTube->getZHalfLength() << " - using std::abs.");}
      if (theTube->getRMax()<theTube->getRMin()){ ATH_MSG_WARNING("Tube " << n << " has a max radius of " << theTube->getRMax() << " and a min radius of " << theTube->getRMin());}
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
      if (nullptr==theTubs) throw std::runtime_error("TypeID did not match cast for tubs");
      if (n.empty()) n="G4Tubs";
      if (theTubs->getRMin()<0.){ ATH_MSG_WARNING("Tubs " << n << " has a min radius of " << theTubs->getRMax());}
      if (theTubs->getRMax()<=0.){ ATH_MSG_WARNING("Tubs " << n << " has a max radius of " << theTubs->getRMax() <<" - using std::abs.");}
      if (theTubs->getZHalfLength()<=0.){ ATH_MSG_WARNING("Tubs " << n << " has a half length of " << theTubs->getZHalfLength() <<" - using std::abs.");}
      if (theTubs->getRMax()<theTubs->getRMin()){ ATH_MSG_WARNING("Tubs " << n << " has a max radius of " << theTubs->getRMax() << " and a min radius of " << theTubs->getRMin());}
      if (theTubs->getDPhi()<=0.){ ATH_MSG_WARNING("Tubs " << n << " has a dPhi of " << theTubs->getDPhi());}
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
      if (nullptr==theTrd) throw std::runtime_error("TypeID did not match cast for trd");
      if (n.empty()) n="G4Trd";
      if (theTrd->getXHalfLength1()<0.){ ATH_MSG_WARNING("Trd " << n << " has a x half length 1 of " << theTrd->getXHalfLength1() << " - using std::abs.");}
      if (theTrd->getXHalfLength2()<0.){ ATH_MSG_WARNING("Trd " << n << " has a x half length 2 of " << theTrd->getXHalfLength2() << " - using std::abs.");}
      if (theTrd->getYHalfLength1()<0.){ ATH_MSG_WARNING("Trd " << n << " has a y half length 1 of " << theTrd->getYHalfLength1() << " - using std::abs.");}
      if (theTrd->getYHalfLength2()<0.){ ATH_MSG_WARNING("Trd " << n << " has a y half length 2 of " << theTrd->getYHalfLength2() << " - using std::abs.");}
      if (theTrd->getZHalfLength()<=0.){ ATH_MSG_WARNING("Trd " << n << " has a z half length of " << theTrd->getZHalfLength() << " - using std::abs.");}
      if (theTrd->getXHalfLength1()<=0. && theTrd->getXHalfLength2()<=0.){ ATH_MSG_WARNING("Trd " << n << " has an x half length 1 of " << theTrd->getXHalfLength1()
                                                                                             << " and an x half length 2 of " << theTrd->getXHalfLength2() << " - using std::abs.");}
      if (theTrd->getYHalfLength1()<=0. && theTrd->getYHalfLength2()<=0.){ ATH_MSG_WARNING("Trd " << n << " has a y half length 1 of " << theTrd->getYHalfLength1()
                                                                                             << " and a y half length 2 of " << theTrd->getYHalfLength2() << " - using std::abs.");}
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
      if (nullptr==thePcon) throw std::runtime_error("TypeID did not match cast for pcon");
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
          if (rInner[index]<0.){ ATH_MSG_WARNING("PCon " << n << " has an inner radius of " << rInner[index] << " for slice " << index << " of " << nPlanes);}
          if (rOuter[index]<=0.){
            ATH_MSG_WARNING("PCon " << n << " has an outer radius of " << rOuter[index] << " for slice " << index << " of " << nPlanes << " - using std::abs.");
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
      if (nullptr==theCons) throw std::runtime_error("TypeID did not match cast for cons");
      if (n.empty()) n="G4Cons";
      if (theCons->getRMax1()<0.){ ATH_MSG_WARNING("Cons " << n << " has a max radius 1 of " << theCons->getRMax1() << " - will use std::abs.");}
      if (theCons->getRMax2()<0.){ ATH_MSG_WARNING("Cons " << n << " has a max radius 2 of " << theCons->getRMax2() << " - will use std::abs.");}
      if (theCons->getRMin1()<0.){ ATH_MSG_WARNING("Cons " << n << " has a min radius 1 of " << theCons->getRMin1());}
      if (theCons->getRMin2()<0.){ ATH_MSG_WARNING("Cons " << n << " has a min radius 2 of " << theCons->getRMin2());}
      if (theCons->getDZ()<=0){    ATH_MSG_WARNING("Cons " << n << " has a DZ of " << theCons->getDZ() << " - will use std::abs.");}
      if (theCons->getRMax1()<=0. && theCons->getRMax2()<=0.){ ATH_MSG_WARNING("Cons " << n << " has a max radius 1 of " << theCons->getRMax1()
                                                                                 << " and a max radius 2 of " << theCons->getRMax2() << " - will use std::abs.");}
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
      if (nullptr==thePara) throw std::runtime_error("TypeID did not match cast for para");
      if (n.empty()) n="G4Para";
      if (thePara->getXHalfLength()<=0.){ ATH_MSG_WARNING("Para " << n << " has an x side of " << thePara->getXHalfLength() <<" - using std::abs.");}
      if (thePara->getYHalfLength()<=0.){ ATH_MSG_WARNING("Para " << n << " has an y side of " << thePara->getYHalfLength() <<" - using std::abs.");}
      if (thePara->getZHalfLength()<=0.){ ATH_MSG_WARNING("Para " << n << " has an z side of " << thePara->getZHalfLength() <<" - using std::abs.");}
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
      if (nullptr==thePgon) throw std::runtime_error("TypeID did not match cast for pgon");
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
          if (rInner[index]<0.){ ATH_MSG_WARNING("Pgon " << n << " has an inner radius of " << rInner[index] << " for slice " << index << " of " << nPlanes);}
          if (rOuter[index]<=0.){
            ATH_MSG_WARNING("Pgon " << n << " has an outer radius of " << rOuter[index] << " for slice " << index << " of " << nPlanes << " - using std::abs.");
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
      if (nullptr==theTrap) throw std::runtime_error("TypeID did not match cast for trap");
      if (n.empty()) n="G4Trap";
      if (theTrap->getZHalfLength()<=0.){ ATH_MSG_WARNING("Trap " << n << " has an z side of " << theTrap->getZHalfLength() <<" - using std::abs.");}
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
      if (nullptr==theBrep) throw std::runtime_error("TypeID did not match cast for brep");
      if (n.empty()) n="G4ExtrudedSolid";
      double dz = theBrep->getDZ();
      int nVertices = theBrep->getNVertices();

      G4TwoVector off(0,0);
      std::vector<G4TwoVector> polygon;

      polygon.reserve(nVertices);
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
      if (nullptr==theTessellated) throw std::runtime_error("TypeID did not match cast for tessellated solid");
      if(n.empty()) n="G4TessellatedSolid";

      G4TessellatedSolid* g4Tessellated = new G4TessellatedSolid(n);
      for(size_t i=0; i<theTessellated->getNumberOfFacets(); ++i) {
        GeoFacet* geoFacet = theTessellated->getFacet(i);
        G4FacetVertexType vertexType = (geoFacet->getVertexType()==GeoFacet::ABSOLUTE? ABSOLUTE : RELATIVE);
        G4VFacet* g4Facet(nullptr);
        if(geoFacet->getNumberOfVertices()==3)
          g4Facet = new G4TriangularFacet(Amg::EigenToHep3Vector(geoFacet->getVertex(0)),
                                          Amg::EigenToHep3Vector(geoFacet->getVertex(1)),
					  Amg::EigenToHep3Vector(geoFacet->getVertex(2)),
                                          vertexType);
        else
          g4Facet = new G4QuadrangularFacet(Amg::EigenToHep3Vector(geoFacet->getVertex(0)),
					    Amg::EigenToHep3Vector(geoFacet->getVertex(1)),
                                            Amg::EigenToHep3Vector(geoFacet->getVertex(2)),
                                            Amg::EigenToHep3Vector(geoFacet->getVertex(3)),
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
      if (nullptr==theEltube) throw std::runtime_error("TypeID did not match cast for elliptical tube");
      if (n.empty()) n="G4EllipticalTube";

      if (theEltube->getXHalfLength()<=0.){ ATH_MSG_WARNING("Eltube " << n << " has an x side of " << theEltube->getXHalfLength() <<" - using std::abs.");}
      if (theEltube->getYHalfLength()<=0.){ ATH_MSG_WARNING("Eltube " << n << " has an y side of " << theEltube->getYHalfLength() <<" - using std::abs.");}
      if (theEltube->getZHalfLength()<=0.){ ATH_MSG_WARNING("Eltube " << n << " has an z side of " << theEltube->getZHalfLength() <<" - using std::abs.");}
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
    if (nullptr==theTorus) throw std::runtime_error("TypeID did not match cast for torus");
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
    if (nullptr==theGenTrap) throw std::runtime_error("TypeID did not match cast for generic trap");
    if (n.empty()) n="G4GenericTrap";
    if (theGenTrap->getZHalfLength()<=0.){ ATH_MSG_WARNING("GenTrap " << n << " has an z side of " << theGenTrap->getZHalfLength() <<" - using std::abs.");}

    // Translate vector of vertices from Eigen to CLHEP
    std::vector<CLHEP::Hep2Vector> clhepVertices;
    clhepVertices.reserve(theGenTrap->getVertices().size());
    for(const GeoTrf::Vector2D& geoVertex : theGenTrap->getVertices()) {
      clhepVertices.push_back(CLHEP::Hep2Vector(geoVertex.x(),geoVertex.y()));
    }

    G4GenericTrap* g4GenTrap = new G4GenericTrap(n
                                                 ,std::abs(theGenTrap->getZHalfLength())
                                                 ,clhepVertices);
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
      if (nullptr==theShapeShift) throw std::runtime_error("TypeID did not match cast for shape shift");
      if (n.empty()) n="DisplacedSolid";
      G4VSolid * undisplacedSolid = Build(theShapeShift->getOp());
      theSolid = new G4DisplacedSolid(n, undisplacedSolid, Amg::EigenTransformToCLHEP(theShapeShift->getX()));
    }
  //
  // GeoShapeUnion
  //
  else if (geoShape->typeID() == GeoShapeUnion::getClassTypeID() )
    {
      const GeoShapeUnion* theUnion = dynamic_cast<const GeoShapeUnion*> (geoShape);
      if (nullptr==theUnion) throw std::runtime_error("TypeID did not match cast for union");
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
      if (nullptr==theIntersection) throw std::runtime_error("TypeID did not match cast for intersection");
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
      if (nullptr==theSubtraction) throw std::runtime_error("TypeID did not match cast for subtraction");
      if (n.empty()) n="Subtraction";
      solidA = Build(theSubtraction->getOpA());
      solidB = Build(theSubtraction->getOpB());
      theSolid = new G4SubtractionSolid(n, solidA, solidB);
    }
  //
  // Custom Shapes (presently LAr shapes only)
  //
  else if(geoShape->typeID() == GeoUnidentifiedShape::getClassTypeID())
    {
      const GeoUnidentifiedShape* customShape = dynamic_cast<const GeoUnidentifiedShape*> (geoShape);
      if (nullptr==customShape) throw std::runtime_error("TypeID did not match cast for custom shape");
      if (customShape->name()=="LArCustomShape") {

	std::string customName = customShape->asciiData();
   
	customSolidMap::const_iterator it = customSolids.find(customName);
	if(it!=customSolids.end())
	  theSolid = it->second;
	else
	  {
	    theSolid = nullptr;
	    if(customName.find("Slice") != std::string::npos){
	      theSolid = createLArWheelSliceSolid(customShape);
	    } else {
	      theSolid = createLArWheelSolid(customName, s_lwsTypes.at(customName) ); // map.at throws std::out_of_range exception on unknown shape name
	    }
	    if ( nullptr == theSolid ) {
	      std::string error = std::string("Can't create LArWheelSolid for name ") + customName + " in Geo2G4SolidFactory::Build";
	      throw std::runtime_error(error);
	    }
	    
	    if(theSolid != nullptr) customSolids[customName] = theSolid;
	  }
      }
    }
  //
  // Catch All
  //
  else
    {
      ATH_MSG_FATAL("Sorry this solid is not yet implemented... ");
      ATH_MSG_FATAL(geoShape->type());
      ATH_MSG_FATAL("You will have a core dump...");
      return nullptr;
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
          ATH_MSG_WARNING("Can't store proxy for LArWheelSolid to the DetectorStore");
                delete theLWS_p;
        }
        return theLWS;
}

G4VSolid* Geo2G4SolidFactory::createLArWheelSliceSolid(const GeoUnidentifiedShape* customShape) const
{
  LArWheelSliceSolid *theLWS = new LArWheelSliceSolid(customShape->asciiData());

    LArWheelSolidDDProxy *theLWS_p = new LArWheelSolidDDProxy(theLWS);
    // ownership is passed to detStore
    if(detStore()->record(theLWS_p, theLWS->GetName()).isFailure()){
        ATH_MSG_WARNING("Can't store proxy for LArWheelSolid to the DetectorStore");
        delete theLWS_p;
    }
    return theLWS;
}
