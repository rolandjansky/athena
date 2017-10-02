/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////
//                                                                     //
//  Implementation of class SurfaceToSoNode                            //
//                                                                     //
//  Author: Andreas Salzburger <Andreas.Salzburger@cern.ch> (primary)  //
//          Thomas Kittelmann <Thomas.Kittelmann@cern.ch>              //
//                                                                     //
//  Initial VP1 version: June 2007                                     //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#include "VP1Utils/SurfaceToSoNode.h"
#include "VP1Utils/VP1LinAlgUtils.h"

#include <Inventor/C/errors/debugerror.h>
//#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoLineSet.h>
#include "VP1HEPVis/nodes/SoGenericBox.h"
#include "VP1HEPVis/nodes/SoTubs.h"
#include "VP1HEPVis/nodes/SoPolyhedron.h"
#include "VP1HEPVis/SbPolyhedron.h"

#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkSurfaces/CylinderBounds.h"
#include "TrkSurfaces/DiscBounds.h"
#include "TrkSurfaces/RectangleBounds.h"
#include "TrkSurfaces/TrapezoidBounds.h"
#include "TrkSurfaces/AnnulusBounds.h"
#include "TrkSurfaces/RotatedTrapezoidBounds.h"
#include "TrkSurfaces/DiamondBounds.h"
#include "TrkSurfaces/NoBounds.h"

#include "GeoModelKernel/GeoSimplePolygonBrep.h"

double SurfaceToSoNode::surfaceThickness = 0.1;

SurfaceToSoNode::SurfaceToSoNode()
{
  SoTubs::initClass();
  SoGenericBox::initClass();
}

SurfaceToSoNode::~SurfaceToSoNode(){}

SoNode*    SurfaceToSoNode::translateSurface(const Trk::Surface& sf, const bool& simple) const
{

  SoNode* sono =0;
    const Trk::CylinderSurface* ccsf = dynamic_cast<const Trk::CylinderSurface*>(&sf);
    if (ccsf) sono = this->translateCylinderSurface(*ccsf);

    const Trk::DiscSurface* cdsf = sono ? 0 : dynamic_cast<const Trk::DiscSurface*>(&sf);
    if (cdsf) sono = this->translateDiscSurface(*cdsf);

    const Trk::PlaneSurface* cpsf = sono ? 0 : dynamic_cast<const Trk::PlaneSurface*>(&sf);
    if (cpsf) sono = this->translatePlaneSurface(*cpsf);

    const Trk::StraightLineSurface* cssf = sono ? 0 : dynamic_cast<const Trk::StraightLineSurface*>(&sf);
    if (cssf) sono = this->translateStraightLineSurface(*cssf, simple);

    const Trk::PerigeeSurface* cpersf    = sono ? 0 : dynamic_cast<const Trk::PerigeeSurface*>(&sf);
    if (cpersf) sono = this->translatePerigeeSurface(*cpersf);

    if (!sono) {
      std::cout<<"ERROR! Surface unknown!"<<std::endl;
      return 0;
    }

    // place and transform them
    SoSeparator* sosep = new SoSeparator();
    
    // Horrible hack for ITK annulus bounds. What should really happen is we draw the surface where the bounds are. But this was never done before.
    Amg::Transform3D transform = sf.transform();
    if (cpsf) {
      const Trk::AnnulusBounds* cannulus = dynamic_cast<const Trk::AnnulusBounds*>(&(cpsf->bounds()));
      if (cannulus){
        Amg::Vector3D vec(0.0,0.5*(cannulus->maxR()+cannulus->minR()),0.0);
        transform.translate(vec);
      }
    }
  
    SoTransform* sotra = VP1LinAlgUtils::toSoTransform(transform);
    sosep->addChild(sotra);
    sosep->addChild(sono);

    return sosep;
}

SoNode*     SurfaceToSoNode::translatePlaneSurface(const Trk::PlaneSurface& psf ) const
{
   const Trk::RectangleBounds* crecbo = dynamic_cast<const Trk::RectangleBounds*>(&(psf.bounds()));
   if (crecbo){
    SoGenericBox * gb = new SoGenericBox;
    gb->setParametersForBox(crecbo->halflengthPhi(),crecbo->halflengthEta(),0.5*surfaceThickness);
    gb->drawEdgeLines.setValue(true);
     return gb;
   }

   const Trk::TrapezoidBounds* ctrabo = dynamic_cast<const Trk::TrapezoidBounds*>(&(psf.bounds()));
   if (ctrabo){
     SoGenericBox * gb = new SoGenericBox;
     const double hminphi = ctrabo->minHalflengthPhi();
     const double hmaxphi = ctrabo->maxHalflengthPhi();
     const double heta    = ctrabo->halflengthEta();
     gb->setParametersForTrapezoid( 0.5*surfaceThickness/*dz*/, 0/*theta*/, 0/*phi*/, heta/*dy1*/,
				    hminphi/*dx1*/, hmaxphi/*dx2*/, heta/*dy2*/, hminphi/*dx3*/,
				    hmaxphi/*dx4*/, 0/*alp1*/, 0/*alp2*/ );
     gb->drawEdgeLines.setValue(true);
     return gb;
   }
   const Trk::RotatedTrapezoidBounds* crottrabo = dynamic_cast<const Trk::RotatedTrapezoidBounds*>(&(psf.bounds()));
   if (crottrabo){
     SoGenericBox * gb = new SoGenericBox;
     const double hminphi = crottrabo->minHalflengthY();
     const double hmaxphi = crottrabo->maxHalflengthY();
     const double heta    = crottrabo->halflengthX();
     gb->setParametersForTrapezoid( 0.5*surfaceThickness/*dz*/, 0/*theta*/, 0/*phi*/, heta/*dy1*/,
				    hminphi/*dx1*/, hmaxphi/*dx2*/, heta/*dy2*/, hminphi/*dx3*/,
				    hmaxphi/*dx4*/, 0/*alp1*/, 0/*alp2*/ );
     gb->drawEdgeLines.setValue(true);
     return gb;
   }
   const Trk::DiamondBounds* cdiabo = dynamic_cast<const Trk::DiamondBounds*>(&(psf.bounds()));
   if (cdiabo){
     double dz=0.25;
     std::vector<double> x,y;
     x.push_back(cdiabo->minHalflengthX());y.push_back( -2*cdiabo->halflengthY1());
     x.push_back(cdiabo->medHalflengthX());y.push_back( 0.);
     if (cdiabo->halflengthY2()>0.) {
       x.push_back(cdiabo->maxHalflengthX()); y.push_back( 2*cdiabo->halflengthY2()); 
       x.push_back(-cdiabo->maxHalflengthX());y.push_back( 2*cdiabo->halflengthY2()); 
     }
     x.push_back(-cdiabo->medHalflengthX());y.push_back( 0.);
     x.push_back(-cdiabo->minHalflengthX());y.push_back( -2*cdiabo->halflengthY1());

     SbPolyhedronPolygonXSect sbPoly(x,y,dz);
     return new SoPolyhedron(sbPoly);;
   }
   
   const Trk::AnnulusBounds* cannulus = dynamic_cast<const Trk::AnnulusBounds*>(&(psf.bounds()));
   if (cannulus){
     SoGenericBox * gb = new SoGenericBox;
     const double hminphi = 0.5*cannulus->minR()*cannulus->phi();
     const double hmaxphi = 0.5*cannulus->maxR()*cannulus->phi();
     const double heta    = 0.5 * (cannulus->maxR() - cannulus->minR());
     gb->setParametersForTrapezoid( 0.5*surfaceThickness/*dz*/, 0/*theta*/, 0/*phi*/, heta/*dy1*/,
				    hminphi/*dx1*/, hmaxphi/*dx2*/, heta/*dy2*/, hminphi/*dx3*/,
				    hmaxphi/*dx4*/, 0/*alp1*/, 0/*alp2*/ );
     gb->drawEdgeLines.setValue(true);
     return gb;
   }   
   const Trk::NoBounds* nobo = dynamic_cast<const Trk::NoBounds*>(&(psf.bounds()));
   if (nobo){
    SoGenericBox * gb = new SoGenericBox;
    gb->setParametersForBox(50.0,50.0,0.5*surfaceThickness); 
    gb->drawEdgeLines.setValue(false);
     return gb;
   }
   
   return 0;
}

SoNode*    SurfaceToSoNode::translateCylinderSurface(const Trk::CylinderSurface& csf ) const
{
   //std::cout<<"translateCylinderSurface"<<std::endl;

  double radius  =  csf.bounds().r();
  double hlength =  csf.bounds().halflengthZ();

  SoTubs* cylinderSurface = new SoTubs();
  (*cylinderSurface).pRMin = radius-0.25;
  (*cylinderSurface).pRMax = radius+0.25;
  (*cylinderSurface).pDz   = hlength;

  return cylinderSurface;

}

SoNode*    SurfaceToSoNode::translateDiscSurface(const Trk::DiscSurface& dsf) const
{
   //std::cout<<"translateDiscSurface"<<std::endl;
  const Trk::DiscBounds* cdbo = dynamic_cast<const Trk::DiscBounds*>(&(dsf.bounds()));
  if (cdbo){
   double iradius    = cdbo->rMin();
   double oradius    = cdbo->rMax();
   double halfphisec = cdbo->halfPhiSector();

   SoTubs* discSurface = new SoTubs();
   (*discSurface).pRMin = iradius;
   (*discSurface).pRMax = oradius;
   (*discSurface).pDz   = 0.25;

   if (fabs(halfphisec-M_PI)>10e-5){
     // sweep back the rotation as the sweepangle starts from x-axis
     (*discSurface).pSPhi = -halfphisec;
     (*discSurface).pDPhi = 2.* halfphisec;
   }

   return discSurface;
  }
  return 0;
}

SoNode*    SurfaceToSoNode::translateStraightLineSurface(const Trk::StraightLineSurface& slsf, const bool& simple ) const
{
  //std::cout<<"translateStraightLineSurface"<<std::endl;
  const Trk::CylinderBounds* ccbo = dynamic_cast<const Trk::CylinderBounds*>(&(slsf.bounds()));
   if (ccbo){
    double hlength =  ccbo->halflengthZ();
    if ( simple )
    {
      SoVertexProperty * scatVtxProperty = new SoVertexProperty();
      scatVtxProperty->vertex.set1Value(0, 0.0,0.0,-hlength);
      scatVtxProperty->vertex.set1Value(1, 0.0,0.0, hlength);
      SoLineSet * lineSurface = new SoLineSet();
      lineSurface->numVertices = 2;
      lineSurface->vertexProperty = scatVtxProperty;
      return lineSurface;
    }

    double radius  =  ccbo->r();
    SoTubs* lineSurface = new SoTubs();
    (*lineSurface).pRMin = 0.;
    (*lineSurface).pRMax = radius;
    (*lineSurface).pDz   = hlength;
    return lineSurface;
  }
  return 0;
}

SoNode*    SurfaceToSoNode::translatePerigeeSurface(const Trk::PerigeeSurface& /*persf*/) const
{
  //TK: Why do we need depend on the input???? FIXME
   //std::cout<<"translatePerigeeSurface"<<std::endl;
   SoTubs* perigeeSurface = new SoTubs();
    (*perigeeSurface).pRMin = 0.;
    (*perigeeSurface).pRMax = 5.;
    (*perigeeSurface).pDz   = 4000.;
    return perigeeSurface;
}
