/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class PRDHandle_SpacePoint              //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: September 2008                           //
//                                                            //
//           update: March 2014 Riccardo.Maria.Bianchi@cern.ch//
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1PRDSystems/PRDHandle_SpacePoint.h"
#include "VP1PRDSystems/PRDCollHandle_SpacePoints.h"
#include "VP1Utils/HitsSoNodeManager.h"

#include <Inventor/C/errors/debugerror.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoVertexProperty.h>

//____________________________________________________________________
class PRDHandle_SpacePoint::Imp {
public:
};


//____________________________________________________________________
PRDHandle_SpacePoint::PRDHandle_SpacePoint(PRDCollHandle_SpacePoints* ch,const Trk::SpacePoint* sp)
  : PRDHandleBase(ch), m_sp(sp), m_d(new Imp)
{
}

//____________________________________________________________________
PRDHandle_SpacePoint::~PRDHandle_SpacePoint()
{
  delete m_d;
}

//____________________________________________________________________
int PRDHandle_SpacePoint::regionIndex() {
  static const double l=isSCT() ? 100.0 : 200.0;//CLHEP::mm
  Amg::Vector3D c(center());
  return       static_cast<int>(c.z()/l)
         +1000*static_cast<int>(c.y()/l)
      +1000000*static_cast<int>(c.x()/l);
  //Fixme: Use identifiers instead for more intuitive regions.
}

//____________________________________________________________________
void PRDHandle_SpacePoint::buildShapes(SoNode*&shape_simple, SoNode*&shape_detailed)
{
  if (getSecondPRD()) {
    Amg::Transform3D prdtransform(getTransform_CLHEP());
    Amg::Vector3D p((prdtransform.inverse())*(m_sp->globalPosition()));
    SoPointSet       * points    = new SoPointSet;
    SoVertexProperty * vertices = new SoVertexProperty;
    vertices->vertex.set1Value(0,p.x(),p.y(),p.z());
    points->numPoints=1;
    points->vertexProperty.setValue(vertices);
    shape_simple = points;
  } else {
    shape_simple = common()->nodeManager()->getShapeNode_Point();
  }
  shape_detailed = shape_simple;
}
