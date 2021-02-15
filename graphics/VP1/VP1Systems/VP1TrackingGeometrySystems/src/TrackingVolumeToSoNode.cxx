/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////
//                                                                     //
//  Implementation of class TrackingVolumeToSoNode                     //
//                                                                     //
//  Author: Andreas Salzburger <Andreas.Salzburger@cern.ch> (primary)  //
//          Thomas Kittelmann <Thomas.Kittelmann@cern.ch>              //
//                                                                     //
//  Initial VP1 version: June 2007                                     //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#include "GeoPrimitives/GeoPrimitives.h"
#include "VP1TrackingGeometrySystems/TrackingVolumeToSoNode.h"
#include "VP1HEPVis/nodes/SoGenericBox.h"
#include "VP1HEPVis/nodes/SoTubs.h"
#include "VP1Utils/SoVisualizeAction.h"
#include "VP1Utils/VP1LinAlgUtils.h"

#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoSeparator.h>

#include "TrkGeometry/TrackingVolume.h"
#include "TrkVolumes/CuboidVolumeBounds.h"
#include "TrkVolumes/CylinderVolumeBounds.h"
#include "TrkVolumes/TrapezoidVolumeBounds.h"
#include "TrkVolumes/DoubleTrapezoidVolumeBounds.h"
#include "TrkVolumes/BevelledCylinderVolumeBounds.h"
#include "TrkVolumes/SubtractedVolumeBounds.h"
#include "TrkVolumes/CombinedVolumeBounds.h"
#include "TrkVolumes/SimplePolygonBrepVolumeBounds.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoShapeIntersection.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoSimplePolygonBrep.h"
#include "GeoModelKernel/GeoDefinitions.h"

#include "GaudiKernel/SystemOfUnits.h"


TrackingVolumeToSoNode::TrackingVolumeToSoNode()
{
  SoGenericBox::initClass();
  SoTubs::initClass();
}

TrackingVolumeToSoNode::~TrackingVolumeToSoNode()
{
}

SoNode*  TrackingVolumeToSoNode::translateTrackingVolume(const Trk::TrackingVolume& tvol) const{

  //Fixme: We need to add a transformation also.

  SoNode* sovol = 0;

  // get the surface bounds
  const Trk::VolumeBounds& tvolBounds = tvol.volumeBounds();

  const Trk::SimplePolygonBrepVolumeBounds* tvolSpbBounds = dynamic_cast<const Trk::SimplePolygonBrepVolumeBounds*>(&tvolBounds);
  if (tvolSpbBounds) {

    // tvolBounds = &( (tvolSpbBounds->combinedVolume())->volumeBounds() ); 
    GeoSimplePolygonBrep* shape = new GeoSimplePolygonBrep(tvolSpbBounds->halflengthZ());
    for (unsigned int i=0;i<tvolSpbBounds->xyVertices().size();i++)
      shape->addVertex(tvolSpbBounds->xyVertices()[i].first,tvolSpbBounds->xyVertices()[i].second);

    SoVisualizeAction* sa = new SoVisualizeAction();
    sa->handleShape(shape);
    sovol = sa->getShape();
 
  }

  const Trk::SubtractedVolumeBounds* tvolSubtrBounds = dynamic_cast<const Trk::SubtractedVolumeBounds*>(&tvolBounds);
  if (tvolSubtrBounds) {

    const GeoShape* shape = getShapeFromBounds(tvolSubtrBounds);

    SoVisualizeAction* sa = new SoVisualizeAction();
    sa->handleShape(shape);
    sovol = sa->getShape();
  }

  const Trk::CombinedVolumeBounds* tvolCombBounds = dynamic_cast<const Trk::CombinedVolumeBounds*>(&tvolBounds);
  if (tvolCombBounds) {

    const GeoShape* shape = getShapeFromBounds(tvolCombBounds);

    SoVisualizeAction* sa = new SoVisualizeAction();
    sa->handleShape(shape);
    sovol = sa->getShape();
  }

  const Trk::CylinderVolumeBounds* tvolCylBounds = dynamic_cast<const Trk::CylinderVolumeBounds*>(&tvolBounds);
  if (tvolCylBounds){
    // the Tubs Node
    SoTubs* tvolTubs = new SoTubs();
     (*tvolTubs).pRMin = tvolCylBounds->innerRadius();
     (*tvolTubs).pRMax = tvolCylBounds->outerRadius();
     (*tvolTubs).pDz   = tvolCylBounds->halflengthZ();
     (*tvolTubs).pDPhi = 2*tvolCylBounds->halfPhiSector();
     (*tvolTubs).pSPhi = -tvolCylBounds->halfPhiSector();
   
    sovol = tvolTubs;
  }
  const Trk::CuboidVolumeBounds* tvolCubBounds = dynamic_cast<const Trk::CuboidVolumeBounds*>(&tvolBounds);
  if (tvolCubBounds){
     // A Cube
    SoGenericBox * gb = new SoGenericBox;
    gb->setParametersForBox( tvolCubBounds->halflengthX(),tvolCubBounds->halflengthY(),tvolCubBounds->halflengthZ());
    
    sovol = gb;
  }
  const Trk::TrapezoidVolumeBounds* tvolTrdBounds = dynamic_cast<const Trk::TrapezoidVolumeBounds*>(&tvolBounds);
  if (tvolTrdBounds) {
    // A Trapezoid
    const double minHalfX(tvolTrdBounds->minHalflengthX()), maxHalfX(tvolTrdBounds->maxHalflengthX()),
      halfY(tvolTrdBounds->halflengthY()), halfZ(tvolTrdBounds->halflengthZ());
    SoGenericBox * gb = new SoGenericBox;
    gb->setParametersForTrapezoid( halfZ/*dz*/, 0/*theta*/, 0/*phi*/, halfY/*dy1*/,
				   minHalfX/*dx1*/, maxHalfX/*dx2*/, halfY/*dy2*/, minHalfX/*dx3*/,
				   maxHalfX/*dx4*/, 0/*alp1*/, 0/*alp2*/ );
    
    sovol = gb;
  }

  const Trk::DoubleTrapezoidVolumeBounds* tvolDTrdBounds = dynamic_cast<const Trk::DoubleTrapezoidVolumeBounds*>(&tvolBounds);
  if (tvolDTrdBounds) {
    double minX = tvolDTrdBounds->minHalflengthX();
    double medX = tvolDTrdBounds->medHalflengthX();
    double maxX = tvolDTrdBounds->maxHalflengthX();
    double y1   = tvolDTrdBounds->halflengthY1();
    double y2   = tvolDTrdBounds->halflengthY2();
    double z    = tvolDTrdBounds->halflengthZ();
    Trk::TrapezoidVolumeBounds* trBoundA = new Trk::TrapezoidVolumeBounds(minX,medX,z,y1);
    Trk::TrapezoidVolumeBounds* trBoundB = new Trk::TrapezoidVolumeBounds(medX,maxX,z,y2);
    const GeoShape* trdA = getShapeFromBounds( trBoundA );
    const GeoShape* trdB = getShapeFromBounds( trBoundB );
    const GeoShapeShift* shiftA = new GeoShapeShift(trdA,GeoTrf::TranslateY3D(-y1)*GeoTrf::RotateX3D(-90*CLHEP::deg)*GeoTrf::RotateZ3D(-90*CLHEP::deg));
    const GeoShapeShift* shiftB = new GeoShapeShift(trdB,GeoTrf::TranslateY3D(+y2)*GeoTrf::RotateX3D(-90*CLHEP::deg)*GeoTrf::RotateZ3D(-90*CLHEP::deg));
    const GeoShapeUnion* dtrd   = new GeoShapeUnion(shiftA,shiftB); 

    SoVisualizeAction* sa = new SoVisualizeAction();
    sa->handleShape(dtrd);
    sovol = sa->getShape();
  }

  const Trk::BevelledCylinderVolumeBounds* tvolBevCylBounds = dynamic_cast<const Trk::BevelledCylinderVolumeBounds*>(&tvolBounds);
  if (tvolBevCylBounds) {
    // the Tubs Node
    int type = tvolBevCylBounds->type();
    double hPhi =   tvolBevCylBounds->halfPhiSector();
    double tp   =   tan(hPhi);
    double innerR = tvolBevCylBounds->innerRadius();
    double outerR = tvolBevCylBounds->outerRadius();
    double hZ     = tvolBevCylBounds->halflengthZ();
    if (type<1) {
      SoTubs* tvolTubs = new SoTubs();
      (*tvolTubs).pRMin = innerR;
      (*tvolTubs).pRMax = outerR;
      (*tvolTubs).pDz   = hZ;
      (*tvolTubs).pDPhi = 2*hPhi;
      (*tvolTubs).pSPhi = -hPhi;
      sovol = tvolTubs;
    } else if ( type==1 ) {
      const GeoTubs* tubs = new GeoTubs(innerR,outerR,hZ,-hPhi,2*hPhi);
      const GeoBox*  box = new GeoBox( innerR, tp*innerR, hZ);
      const GeoShapeSubtraction* sub = new GeoShapeSubtraction(tubs,new GeoShapeShift(box,GeoTrf::Transform3D::Identity()));
      SoVisualizeAction* sa = new SoVisualizeAction();
      sa->handleShape(sub);
      sovol = sa->getShape();
    } else if ( type==2 ) {
      const GeoTubs* tubs = new GeoTubs(innerR,outerR/cos(hPhi),hZ,-hPhi,2*hPhi);
      const GeoBox*  box = new GeoBox( outerR*(1./cos(hPhi)-1.),tp*outerR,hZ );
      const GeoShapeSubtraction* sub = new GeoShapeSubtraction(tubs,new GeoShapeShift(box,GeoTrf::TranslateX3D(outerR/cos(hPhi))));
      SoVisualizeAction* sa = new SoVisualizeAction();
      sa->handleShape(sub);
      sovol = sa->getShape();
    } else if ( type==3 ) {
      const GeoTubs* tubs = new GeoTubs(innerR,outerR/cos(hPhi),hZ,-hPhi,2*hPhi);
      const GeoBox*  boxO = new GeoBox( outerR*(1./cos(hPhi)-1.),tp*outerR,hZ );
      const GeoBox*  boxI = new GeoBox( innerR, tp*innerR, hZ);
      const GeoShapeUnion* uni = new GeoShapeUnion(boxI,new GeoShapeShift(boxO,GeoTrf::TranslateX3D(outerR/cos(hPhi))));
      const GeoShapeSubtraction* sub = new GeoShapeSubtraction(tubs,uni);
      SoVisualizeAction* sa = new SoVisualizeAction();
      sa->handleShape(sub);
      sovol = sa->getShape();
    }
  }

  if (!sovol) return 0;

  // place and transform them
  SoSeparator* sosep = new SoSeparator();
  SoTransform* sotra = VP1LinAlgUtils::toSoTransform(tvol.transform());
  sosep->addChild(sotra);
  sosep->addChild(sovol);
  
  return sosep;

}

const GeoShape* TrackingVolumeToSoNode::getShapeFromBounds(const Trk::VolumeBounds* tvolBounds ) const
{
  const GeoShape* sh=0;
  // cast bounds
  const Trk::CylinderVolumeBounds* tvolCylBounds = dynamic_cast<const Trk::CylinderVolumeBounds*>(tvolBounds);
  const Trk::CuboidVolumeBounds* tvolCubBounds = dynamic_cast<const Trk::CuboidVolumeBounds*>(tvolBounds);
  const Trk::TrapezoidVolumeBounds* tvolTrdBounds = dynamic_cast<const Trk::TrapezoidVolumeBounds*>(tvolBounds);
  //const Trk::DoubleTrapezoidVolumeBounds* tvolDTrdBounds = dynamic_cast<const Trk::DoubleTrapezoidVolumeBounds*>(tvolBounds);
  const Trk::BevelledCylinderVolumeBounds* tvolBevCylBounds = dynamic_cast<const Trk::BevelledCylinderVolumeBounds*>(tvolBounds);
  const Trk::SubtractedVolumeBounds* tvolSubtrBounds = dynamic_cast<const Trk::SubtractedVolumeBounds*>(tvolBounds);
  const Trk::CombinedVolumeBounds* tvolCombBounds = dynamic_cast<const Trk::CombinedVolumeBounds*>(tvolBounds);
  const Trk::SimplePolygonBrepVolumeBounds* tvolSpbBounds = dynamic_cast<const Trk::SimplePolygonBrepVolumeBounds*>(tvolBounds);

  if (tvolSpbBounds) {

    GeoSimplePolygonBrep* shape = new GeoSimplePolygonBrep(tvolSpbBounds->halflengthZ());
    for (unsigned int i=0;i<tvolSpbBounds->xyVertices().size();i++)
      shape->addVertex(tvolSpbBounds->xyVertices()[i].first,tvolSpbBounds->xyVertices()[i].second);

    return shape; 
  }

  if (tvolCylBounds){
    // the Tube Node
    const GeoTube* tube = new GeoTube(tvolCylBounds->innerRadius(),
				      tvolCylBounds->outerRadius(),
				      tvolCylBounds->halflengthZ() ); 
    return tube;    
  }
  if (tvolCubBounds){
     // the Cube Node
    const GeoBox* box = new GeoBox(tvolCubBounds->halflengthX(),
				   tvolCubBounds->halflengthY(),
                                   tvolCubBounds->halflengthZ());
    return box; 
  }
  if (tvolTrdBounds) {
    // the Trapezoid Node
     
    const GeoTrd* trd = new GeoTrd(tvolTrdBounds->halflengthY(),
				   tvolTrdBounds->halflengthY(),
				   tvolTrdBounds->minHalflengthX(),
				   tvolTrdBounds->maxHalflengthX(),
                                   tvolTrdBounds->halflengthZ());
    

    return trd; 
  }
  
  if (tvolBevCylBounds) {
    // bevelled cylinder: define subtracted shape and let GeoModel to process it
    double innerR = tvolBevCylBounds->innerRadius();
    double outerR = tvolBevCylBounds->outerRadius();
    double halfZ  = tvolBevCylBounds->halflengthZ();
    double thPlus = tvolBevCylBounds->thetaPlus();
    double thMinus= tvolBevCylBounds->thetaMinus();
    const GeoTube* tub = new GeoTube(innerR, outerR, halfZ); 
    GeoTrf::Transform3D trBoxP = GeoTrf::TranslateZ3D(+halfZ)*GeoTrf::RotateY3D(thPlus);
    const GeoShapeShift* boxP = new GeoShapeShift(new GeoBox(2*outerR/cos(thPlus),outerR ,outerR*sin(thPlus)),trBoxP); 
    GeoTrf::Transform3D trBoxM = GeoTrf::TranslateZ3D(-halfZ)*GeoTrf::RotateY3D(-thMinus);
    const GeoShapeShift* boxM = new GeoShapeShift(new GeoBox(2*outerR/cos(thPlus),outerR ,outerR*sin(thPlus)),trBoxM); 
    const GeoShapeSubtraction* shape= new GeoShapeSubtraction( new GeoShapeSubtraction(tub,boxM), boxP);

    return shape;
  }  
  
  if (tvolSubtrBounds) {
   
    const Trk::VolumeBounds* vb1 = &(tvolSubtrBounds->outer()->volumeBounds());
    const Trk::VolumeBounds* vb2 = &(tvolSubtrBounds->inner()->volumeBounds());
    const Trk::TrapezoidVolumeBounds* tr1 = dynamic_cast<const Trk::TrapezoidVolumeBounds*> (vb1);  
    const Trk::TrapezoidVolumeBounds* tr2 = dynamic_cast<const Trk::TrapezoidVolumeBounds*> (vb2);  
    Amg::Transform3D transf1 = tvolSubtrBounds->outer()->transform();
    Amg::Transform3D transf2 = tvolSubtrBounds->inner()->transform();
    if (tr1) {
      Trk::TrapezoidVolumeBounds* ntr = new Trk::TrapezoidVolumeBounds(tr1->minHalflengthX(),tr1->maxHalflengthX(),
								       tr1->halflengthZ(),tr1->halflengthY());
      vb1 = ntr;
      transf1 = transf1 * Amg::getRotateX3D(-90*Gaudi::Units::deg) * Amg::getRotateZ3D(-90*Gaudi::Units::deg);
    }
    if (tr2) {
      Trk::TrapezoidVolumeBounds* ntr = new Trk::TrapezoidVolumeBounds(tr2->minHalflengthX(),tr2->maxHalflengthX(),
								       tr2->halflengthZ(),tr2->halflengthY());
      vb2 = ntr;
      transf2 = transf2 * Amg::getRotateX3D(-90*Gaudi::Units::deg) * Amg::getRotateZ3D(-90*Gaudi::Units::deg);
    }
    const GeoShape* shape1 = getShapeFromBounds(vb1);
    const GeoShape* shape2 = getShapeFromBounds(vb2);
    const GeoShapeShift* shift1 = new GeoShapeShift(shape1, transf1);
    const GeoShapeShift* shift2 = new GeoShapeShift(shape2, transf2);
    const GeoShapeSubtraction* subtr= new GeoShapeSubtraction(shift1, shift2);
    return subtr;
  }

  if (tvolCombBounds) {
    const Trk::VolumeBounds* vb1 = &(tvolCombBounds->first()->volumeBounds());
    const Trk::VolumeBounds* vb2 = &(tvolCombBounds->second()->volumeBounds());
    const Trk::TrapezoidVolumeBounds* tr1 = dynamic_cast<const Trk::TrapezoidVolumeBounds*> (vb1);  
    const Trk::TrapezoidVolumeBounds* tr2 = dynamic_cast<const Trk::TrapezoidVolumeBounds*> (vb2);  
    Amg::Transform3D transf1 = tvolCombBounds->first()->transform();
    Amg::Transform3D transf2 = tvolCombBounds->second()->transform();
    if (tr1) {
      Trk::TrapezoidVolumeBounds* ntr = new Trk::TrapezoidVolumeBounds(tr1->minHalflengthX(),tr1->maxHalflengthX(),
								       tr1->halflengthZ(),tr1->halflengthY());
      vb1 = ntr;
      transf1 = transf1*Amg::getRotateX3D(-90*Gaudi::Units::deg)*Amg::getRotateZ3D(-90*Gaudi::Units::deg);
    }
    if (tr2) {
      Trk::TrapezoidVolumeBounds* ntr = new Trk::TrapezoidVolumeBounds(tr2->minHalflengthX(),tr2->maxHalflengthX(),
								       tr2->halflengthZ(),tr2->halflengthY());
      vb2 = ntr;
      transf2 = transf2*Amg::getRotateX3D(-90*Gaudi::Units::deg)*Amg::getRotateZ3D(-90*Gaudi::Units::deg);
    }
    const GeoShape* shape1 = getShapeFromBounds(vb1);
    const GeoShape* shape2 = getShapeFromBounds(vb2);
    const GeoShapeShift* shift1 = new GeoShapeShift(shape1,transf1);
    const GeoShapeShift* shift2 = new GeoShapeShift(shape2,transf2);
    if (tvolCombBounds->intersection()) {
      const GeoShapeIntersection* intersection= new GeoShapeIntersection( shift1, shift2);
      return intersection;
    }
    const GeoShapeUnion* uni= new GeoShapeUnion( shift1, shift2);
    return uni;
  }

  std::cout << "TrackingVolumeToSoNode::getShapeFromBounds: bounds not recognized" << std::endl;

  return sh;
}
  


