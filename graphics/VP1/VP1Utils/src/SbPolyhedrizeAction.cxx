/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeIntersection.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoCons.h"
#include "GeoModelKernel/GeoPcon.h"
#include "GeoModelKernel/GeoPgon.h"
#include "GeoModelKernel/GeoTrap.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoPara.h"
#include "GeoModelKernel/GeoSimplePolygonBrep.h"
#include "GeoModelKernel/GeoTessellatedSolid.h"
#include "GeoModelKernel/GeoFacet.h"
#include "GeoModelKernel/GeoGenericTrap.h"
#include "VP1Utils/SbPolyhedrizeAction.h"

#include "VP1HEPVis/SbPolyhedron.h"

#include <map>

SbPolyhedrizeAction::SbPolyhedrizeAction()
  : m_polyhedron(NULL)
{
  setDepthLimit(0);
}


SbPolyhedrizeAction::~SbPolyhedrizeAction()
{
  delete m_polyhedron;
}

void SbPolyhedrizeAction::handleShift(const GeoShapeShift *shift)
{
  shift->getOp()->exec(this);
  CLHEP::HepRotation rotation=shift->getX().getRotation();
  CLHEP::Hep3Vector  translation = shift->getX().getTranslation();

  // rbianchi - 14.12.2012
//  SbVec3f    trx(translation.x(),translation.y(),translation.z());
//  SbRotation rot(SbMatrix(rotation.xx(),rotation.yx(),rotation.zx(),0,
//			  rotation.xy(),rotation.yy(),rotation.zy(),0,
//			  rotation.xz(),rotation.yz(),rotation.zz(),0,
//			  0,0,0,1));
  SbVec3d    trx(translation.x(),translation.y(),translation.z());
  #include <VP1HEPVis/SbRotation.h> //using doubles instead of floats.
  HEPVis::SbRotation rot(rotation.xx(),rotation.yx(),rotation.zx(),0,
			  rotation.xy(),rotation.yy(),rotation.zy(),0,
			  rotation.xz(),rotation.yz(),rotation.zz(),0,
			  0,0,0,1);
  //---

  m_polyhedron->Transform (rot, trx);
}

void SbPolyhedrizeAction::handleUnion(const GeoShapeUnion *unio)
{
  SbPolyhedrizeAction auxA,auxB;
  unio->getOpA()->exec(&auxA);
  unio->getOpB()->exec(&auxB);
  m_polyhedron = new SbPolyhedron(auxA.getPolyhedron()->add(*auxB.getPolyhedron()));
}

void SbPolyhedrizeAction::handleIntersection(const GeoShapeIntersection *isect)
{
  SbPolyhedrizeAction auxA,auxB;
  isect->getOpA()->exec(&auxA);
  isect->getOpB()->exec(&auxB);
  m_polyhedron=new SbPolyhedron(auxA.getPolyhedron()->intersect(*auxB.getPolyhedron()));
}

void SbPolyhedrizeAction::handleSubtraction(const GeoShapeSubtraction *subtract)
{
  SbPolyhedrizeAction auxA,auxB;
  subtract->getOpA()->exec(&auxA);
  subtract->getOpB()->exec(&auxB);
  m_polyhedron=new SbPolyhedron(auxA.getPolyhedron()->subtract(*auxB.getPolyhedron()));
}

void SbPolyhedrizeAction::handleBox(const GeoBox *box)
{
  m_polyhedron=new SbPolyhedronBox (box->getXHalfLength(),
				   box->getYHalfLength(),
				   box->getZHalfLength());
}

void SbPolyhedrizeAction::handleCons(const GeoCons *cons)
{
  m_polyhedron = new SbPolyhedronCons (cons->getRMin1(),
				      cons->getRMax1(),
				      cons->getRMin2(),
				      cons->getRMax2(),
				      cons->getDZ(),
				      cons->getSPhi(),
				      cons->getDPhi());
}

void SbPolyhedrizeAction::handlePara(const GeoPara *para)
{
  m_polyhedron=new SbPolyhedronPara(para->getXHalfLength(),
				   para->getYHalfLength(),
				   para->getZHalfLength(),
				   para->getAlpha(),
				   para->getTheta(),
				   para->getPhi());
}

void SbPolyhedrizeAction::handlePcon(const GeoPcon *pcon)
{
  double *z = new double[pcon->getNPlanes ()];
  double *rmn = new double[pcon->getNPlanes ()];
  double *rmx = new double[pcon->getNPlanes ()];

  for (unsigned s = 0; s < pcon->getNPlanes (); s++) {
    z[s] = pcon->getZPlane (s);
    rmn[s] = pcon->getRMinPlane (s);
    rmx[s] = pcon->getRMaxPlane (s);
  }
  m_polyhedron = new SbPolyhedronPcon (pcon->getSPhi(), pcon->getDPhi(), pcon->getNPlanes (), z, rmn, rmx);
  
  delete[]z;
  delete[]rmn;
  delete[]rmx;
}

void SbPolyhedrizeAction::handlePgon(const GeoPgon *pgon)
{
  double *z   = new double[pgon->getNPlanes ()];
  double *rmn = new double[pgon->getNPlanes ()];
  double *rmx = new double[pgon->getNPlanes ()];

  for (unsigned s = 0; s < pgon->getNPlanes (); s++) {
    z[s]   = pgon->getZPlane (s);
    rmn[s] = pgon->getRMinPlane (s);
    rmx[s] = pgon->getRMaxPlane (s);
  }
  m_polyhedron = new SbPolyhedronPgon (pgon->getSPhi(), pgon->getDPhi(), pgon->getNSides(), pgon->getNPlanes (), z, rmn, rmx);
  
  delete[]z;
  delete[]rmn;
  delete[]rmx;
}

void SbPolyhedrizeAction::handleTrap(const GeoTrap *trap)
{
  m_polyhedron = new SbPolyhedronTrap (trap->getZHalfLength(),
				      trap->getTheta(),
				      trap->getPhi(),
				      trap->getDydzn(),
				      trap->getDxdyndzn(),
				      trap->getDxdypdzn(), 0,
				      trap->getDydzp(),
				      trap->getDxdyndzp(),
				      trap->getDxdypdzp(),0);
}

void SbPolyhedrizeAction::handleTrd(const GeoTrd *trd)
{
  m_polyhedron = new SbPolyhedronTrd2 (trd->getXHalfLength1(),
				      trd->getXHalfLength2(),
				      trd->getYHalfLength1(),
				      trd->getYHalfLength2(),
				      trd->getZHalfLength());
}

void SbPolyhedrizeAction::handleTube(const GeoTube *tube)
{
  m_polyhedron = new SbPolyhedronTube (tube->getRMin(),tube->getRMax(),tube->getZHalfLength());
}

void SbPolyhedrizeAction::handleTubs(const GeoTubs *tubs)
{
  m_polyhedron= new SbPolyhedronTubs (tubs->getRMin(),
				     tubs->getRMax(),
				     tubs->getZHalfLength(),
				     tubs->getSPhi(),
				     tubs->getDPhi());
}

void SbPolyhedrizeAction::handleSimplePolygonBrep(const GeoSimplePolygonBrep *brep)
{
  double dz = brep->getDZ();
  std::vector<double> x, y;
  for(unsigned int i=0; i<brep->getNVertices(); i++)
  {
    x.push_back(brep->getXVertex(i));
    y.push_back(brep->getYVertex(i));
  }

  m_polyhedron = new SbPolyhedronPolygonXSect(x,y,dz);
}

void SbPolyhedrizeAction::handleTessellatedSolid(const GeoTessellatedSolid *tessellated)
{
  // ______ Perform actions a la SetSolidClosed ________
  double vertTolerance = 1E-15;  // kCarTolerance ??
  std::vector<GeoFacetVertex> vertexList;
  std::map<GeoFacet*,std::vector<size_t> > facetVertIndInSolid;  // Vacet vertex indices in the vertexList

  // Loop over facets in the solid
  for(size_t facetIndInSolid = 0; facetIndInSolid<tessellated->getNumberOfFacets(); ++facetIndInSolid) {
    GeoFacet* facet = tessellated->getFacet(facetIndInSolid);

    facetVertIndInSolid[facet] = std::vector<size_t>();
    std::vector<size_t>& current = facetVertIndInSolid[facet];
    if(facet->getNumberOfVertices()==3)
      current.resize(3,0);
    else
      current.resize(4,0);

    size_t vertexListSize = vertexList.size();
    GeoFacetVertex vertex(0.,0.,0.);

    // Loop over vertices in the current facet
    for(size_t vertexIndInFacet=0; vertexIndInFacet<facet->getNumberOfVertices(); ++vertexIndInFacet) {
      vertex = facet->getVertex(vertexIndInFacet);

      // Check if we already have this vertex in our vertexList
      bool found = false;
      size_t j=0;
      while(j<vertexListSize && !found) {
	GeoFacetVertex vertexToCheck = vertexList[j];
	found = (vertexToCheck-vertex).mag() < vertTolerance;
	if(!found) j++;
      }

      if(found) {
	current[vertexIndInFacet] = j;
      }
      else {
	vertexList.push_back(vertex);
	current[vertexIndInFacet] = vertexList.size()-1;
      }
    }
  }

  // ______ Create Polyhedron itself ______
  SbPolyhedronArbitrary* polyhedron=  new SbPolyhedronArbitrary(vertexList.size(),tessellated->getNumberOfFacets());

  // add vertices from the list
  for(size_t vertexInd=0;  vertexInd<vertexList.size(); ++vertexInd) {
    GeoFacetVertex vertex = vertexList[vertexInd];
    polyhedron->AddVertex(vertex.x(),vertex.y(),vertex.z());
  }

  // add facets
  for(size_t facetIndInSolid = 0; facetIndInSolid<tessellated->getNumberOfFacets(); ++facetIndInSolid) {
    GeoFacet* facet = tessellated->getFacet(facetIndInSolid);
    std::vector<size_t>& current = facetVertIndInSolid[facet];

    size_t v[4];
    for (size_t j=0; j<4; j++) 
      v[j] = (j<current.size() ? current[j]+1 : 0);

    polyhedron->AddFacet(v[0],v[1],v[2],v[3]);
  }

  polyhedron->Finalize();
  m_polyhedron = polyhedron;
}

void SbPolyhedrizeAction::handleGenericTrap(const GeoGenericTrap *gentrap)
{
  std::vector<std::pair<double,double> > vertices;
  vertices.reserve(8);
  for(size_t i=0; i<8; ++i)
    vertices.push_back(std::pair<double,double>(gentrap->getVertices()[i].x(),gentrap->getVertices()[i].y()));
  m_polyhedron = new SbPolyhedronGenericTrap(gentrap->getZHalfLength(),vertices);
}

const SbPolyhedron * SbPolyhedrizeAction::getPolyhedron() const
{
  return m_polyhedron;
}






