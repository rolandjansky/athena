/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1GeometrySystems/DumpShape.h"

#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoCons.h"
#include "GeoModelKernel/GeoTrap.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoPcon.h"
#include "GeoModelKernel/GeoPgon.h"
#include "GeoModelKernel/GeoPara.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeIntersection.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoGenericTrap.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "VP1Utils/VP1LinAlgUtils.h"
#include "Inventor/SbMatrix.h"
#include <cassert>

//Fixme: Add support for the SimpleBREP GeoShape.

//____________________________________________________________________
QStringList DumpShape::shapeToStringList(const GeoShape* shape)
{
  QStringList out;

  if(shape->typeID() == GeoBox::getClassTypeID() ) {
    const GeoBox* theBox = dynamic_cast<const GeoBox*> (shape);
    assert(theBox);
    out << " =========> Box:";
    out << "  X = "+QString::number(theBox->getXHalfLength()/CLHEP::mm)+" mm";
    out << "  Y = "+QString::number(theBox->getYHalfLength()/CLHEP::mm)+" mm";
    out << "  Z = "+QString::number(theBox->getZHalfLength()/CLHEP::mm)+" mm";
  } else if(shape->typeID() == GeoTube::getClassTypeID() ) {
    const GeoTube* theTube = dynamic_cast<const GeoTube*> (shape);
    assert(theTube);
    out << " =========> Tube:";
    out << "  RMin = "+QString::number(theTube->getRMin()/CLHEP::mm)+" mm";
    out << "  RMax = "+QString::number(theTube->getRMax()/CLHEP::mm)+" mm";
    out << "  DZ = "+QString::number(theTube->getZHalfLength()/CLHEP::mm)+" mm";
  } else if(shape->typeID() == GeoTubs::getClassTypeID() ) {
    const GeoTubs* theTubs = dynamic_cast<const GeoTubs*> (shape);
    assert(theTubs);
    out << " =========> Tubs:";
    out << "  RMin = "+QString::number(theTubs->getRMin()/CLHEP::mm)+" mm";
    out << "  RMax = "+QString::number(theTubs->getRMax()/CLHEP::mm)+" mm";
    out << "  DZ = "+QString::number(theTubs->getZHalfLength()/CLHEP::mm)+" mm";
    out << "  SPhi = "+QString::number(theTubs->getSPhi());
    out << "  DPhi = "+QString::number(theTubs->getDPhi());
  } else if(shape->typeID() == GeoTrd::getClassTypeID() ) {
    const GeoTrd* theTrd = dynamic_cast<const GeoTrd*> (shape);
    assert(theTrd);
    out << " =========> Trd:";
    out << "  X1 = "+QString::number(theTrd->getXHalfLength1()/CLHEP::mm)+" mm";
    out << "  X2 = "+QString::number(theTrd->getXHalfLength2()/CLHEP::mm)+" mm";
    out << "  Y1 = "+QString::number(theTrd->getYHalfLength1()/CLHEP::mm)+" mm";
    out << "  Y2 = "+QString::number(theTrd->getYHalfLength2()/CLHEP::mm)+" mm";
    out << "  Z  = "+QString::number(theTrd->getZHalfLength()/CLHEP::mm)+" mm";
  } else if(shape->typeID() == GeoPcon::getClassTypeID()) {
    const GeoPcon* thePcon = dynamic_cast<const GeoPcon*>(shape);
    assert(thePcon);
    out << " =========> Pcon:";
    out<<"  Start phi = "+QString::number(thePcon->getSPhi())+", Delta phi = "+QString::number(thePcon->getDPhi());
    for (unsigned index=0; index < thePcon->getNPlanes(); ++index) {
      out << "  Plane "+QString::number(index);
      out << "    -> RMin = "+QString::number(thePcon->getRMinPlane(index)/CLHEP::mm)+" mm";
      out << "    -> RMax = "+QString::number(thePcon->getRMaxPlane(index)/CLHEP::mm)+" mm";
      out << "    -> Z    = "+QString::number(thePcon->getZPlane(index)/CLHEP::mm)+" mm";
    }
  } else if(shape->typeID() == GeoCons::getClassTypeID()) {
    const GeoCons* theCons = dynamic_cast<const GeoCons*>(shape);
    assert(theCons);
    out << " =========> Cons:";
    out << "  RMin1 = "<<QString::number(theCons->getRMin1()/CLHEP::mm)+" mm";
    out << "  RMin2 = "<<QString::number(theCons->getRMin2()/CLHEP::mm)+" mm";
    out << "  RMax1 = "<<QString::number(theCons->getRMax1()/CLHEP::mm)+" mm";
    out << "  RMax2 = "<<QString::number(theCons->getRMax2()/CLHEP::mm)+" mm";
    out << "  DZ = "<<QString::number(theCons->getDZ()/CLHEP::mm)+" mm";
    out << "  SPhi = "+QString::number(theCons->getSPhi());
    out << "  DPhi = "+QString::number(theCons->getDPhi());
  } else if(shape->typeID() == GeoPara::getClassTypeID()) {
    const GeoPara* thePara = dynamic_cast<const GeoPara*>(shape);
    assert(thePara);
    out << " =========> Para:";
    out << "  X = "+QString::number(thePara->getXHalfLength()/CLHEP::mm)+" mm";
    out << "  Y = "+QString::number(thePara->getYHalfLength()/CLHEP::mm)+" mm";
    out << "  Z = "+QString::number(thePara->getZHalfLength()/CLHEP::mm)+" mm";
    out << "  Alpha = "+QString::number(thePara->getAlpha());
    out << "  Theta = "+QString::number(thePara->getTheta());
    out << "  Phi = "+QString::number(thePara->getPhi());
  } else if(shape->typeID() == GeoPgon::getClassTypeID()) {
    const GeoPgon* thePgon = dynamic_cast<const GeoPgon*>(shape);
    assert(thePgon);
    out << " =========> Pgon:";
    for (unsigned int index=0; index<thePgon->getNPlanes(); index++)
    {
      out << "  Plane "+QString::number(index);
      out << ":   -> RMin = "+QString::number(thePgon->getRMinPlane(index)/CLHEP::mm)+" mm";
      out << ":   -> RMax = "+QString::number(thePgon->getRMaxPlane(index)/CLHEP::mm)+" mm";
      out << "    ->    Z = "+QString::number(thePgon->getZPlane(index)/CLHEP::mm)+" mm";
    }
  } else if(shape->typeID() == GeoTrap::getClassTypeID()) {
    const GeoTrap* theTrap = dynamic_cast<const GeoTrap*>(shape);
    assert(theTrap);
    out << " =========> Trap:";
    out << "  DZ = "+QString::number(theTrap->getZHalfLength()/CLHEP::mm)+" mm";
    out << "  Theta = "+QString::number(theTrap->getTheta());
    out << "  Phi = "+QString::number(theTrap->getPhi());
    out << "  Dydzn = "+QString::number(theTrap->getDydzn()/CLHEP::mm)+" mm";
    out << "  Dxdyndzn = "+QString::number(theTrap->getDxdyndzn()/CLHEP::mm)+" mm";
    out << "  Dxdypdzn = "+QString::number(theTrap->getDxdypdzn()/CLHEP::mm)+" mm";
    out << "  Angleydzn = "+QString::number(theTrap->getAngleydzn());
    out << "  Dydzp = "+QString::number(theTrap->getDydzp()/CLHEP::mm)+" mm";
    out << "  Dxdyndzp = "+QString::number(theTrap->getDxdyndzp()/CLHEP::mm)+" mm";
    out << "  Dxdypdzp = "+QString::number(theTrap->getDxdypdzp()/CLHEP::mm)+" mm";
    out << "  Angleydzp = "+QString::number(theTrap->getAngleydzp());
  }
  // Boolean volumes:
  else if (shape->typeID() == GeoShapeShift::getClassTypeID() ) {
    const GeoShapeShift* theShift = dynamic_cast<const GeoShapeShift*>(shape);
    assert(theShift);
    out << " =========> GeoShapeShift:";
    SbMatrix t;
    VP1LinAlgUtils::transformToMatrix(theShift->getX(), t);
    float translation_x, translation_y, translation_z, rotaxis_x, rotaxis_y, rotaxis_z, rotangle_radians;
    VP1LinAlgUtils::decodeTransformation( t,
					  translation_x, translation_y, translation_z,
					  rotaxis_x, rotaxis_y, rotaxis_z, rotangle_radians );
    out << "   Information about contained transformation:";
    out << "===&gt; Translation:";
    out << "        x = "+QString::number(translation_x/CLHEP::mm)+" mm";
    out << "        y = "+QString::number(translation_y/CLHEP::mm)+" mm";
    out << "        z = "+QString::number(translation_z/CLHEP::mm)+" mm";
    out << "===&gt; Rotation:";
    out << "        axis x = "+QString::number(rotaxis_x);
    out << "        axis y = "+QString::number(rotaxis_y);
    out << "        axis z = "+QString::number(rotaxis_z);
    out << "        angle = "+QString::number(rotangle_radians*180.0/M_PI)+" deg";

    out << "   Information about contained GeoShape:";
    foreach(QString str,shapeToStringList(theShift->getOp())){
      out <<"  ==>  "+str;
    }
  } else if (shape->typeID() == GeoShapeUnion::getClassTypeID() ) {
    const GeoShapeUnion* theUnion = dynamic_cast<const GeoShapeUnion*>(shape);
    assert(theUnion);
    out << " =========> GeoShapeUnion:";
    out << "   First operand:";
    foreach (QString s,shapeToStringList(theUnion->getOpA()))
      out << "  ==> "+s;
    out << "   Second operand:";
    foreach (QString s,shapeToStringList(theUnion->getOpB()))
      out << "  ==> "+s;
  } else if (shape->typeID() == GeoShapeIntersection::getClassTypeID() ) {
    const GeoShapeIntersection* theIntersection = dynamic_cast<const GeoShapeIntersection*>(shape);
    assert(theIntersection);
    out << " =========> GeoShapeIntersection:";
    out << "   First operand:";
    foreach (QString s,shapeToStringList(theIntersection->getOpA()))
      out << "  ==> "+s;
    out << "   Second operand:";
    foreach (QString s,shapeToStringList(theIntersection->getOpB()))
      out << "  ==> "+s;
  } else if (shape->typeID() == GeoShapeSubtraction::getClassTypeID() ) {
    const GeoShapeSubtraction* theSubtraction = dynamic_cast<const GeoShapeSubtraction*>(shape);
    assert(theSubtraction);
    out << " =========> GeoShapeSubtraction:";
    out << "   First operand:";
    foreach (QString s,shapeToStringList(theSubtraction->getOpA()))
      out << "  ==> "+s;
    out << "   Second operand:";
    foreach (QString s,shapeToStringList(theSubtraction->getOpB()))
      out << "  ==> "+s;
  } else if (shape->typeID() == GeoGenericTrap::getClassTypeID() ) {
    const GeoGenericTrap* theGenericTrap = dynamic_cast<const GeoGenericTrap*>(shape);
    assert(theGenericTrap);
    out << " =========> GenericTrap:";
    out << " halfZlength = "+QString::number(theGenericTrap->getZHalfLength()/CLHEP::mm)+" mm";
    std::vector<CLHEP::Hep2Vector> vertices = theGenericTrap->getVertices();
    //in total 8 vertices by definition!
    out << "==> First 4 vertices at - hz/2";
    out << "(Ax,Ay) = ( "+QString::number(vertices[0].x()/CLHEP::mm)+" , "+QString::number(vertices[0].y()/CLHEP::mm)+" ) mm";
    out << "(Bx,By) = ( "+QString::number(vertices[1].x()/CLHEP::mm)+" , "+QString::number(vertices[1].y()/CLHEP::mm)+" ) mm";
    out << "(Cx,Cy) = ( "+QString::number(vertices[2].x()/CLHEP::mm)+" , "+QString::number(vertices[2].y()/CLHEP::mm)+" ) mm";
    out << "(Dx,Dy) = ( "+QString::number(vertices[3].x()/CLHEP::mm)+" , "+QString::number(vertices[3].y()/CLHEP::mm)+" ) mm";
    out <<"==> Second 4 vertices at +hz/2";
    out << "(Ax,Ay) = ( "+QString::number(vertices[4].x()/CLHEP::mm)+" , "+QString::number(vertices[4].y()/CLHEP::mm)+" ) mm";
    out << "(Bx,By) = ( "+QString::number(vertices[5].x()/CLHEP::mm)+" , "+QString::number(vertices[5].y()/CLHEP::mm)+" ) mm";
    out << "(Cx,Cy) = ( "+QString::number(vertices[6].x()/CLHEP::mm)+" , "+QString::number(vertices[6].y()/CLHEP::mm)+" ) mm";
    out << "(Dx,Dy) = ( "+QString::number(vertices[7].x()/CLHEP::mm)+" , "+QString::number(vertices[7].y()/CLHEP::mm)+" ) mm";
  } else {
    out << " =========> Unknown shape...";
  }

  return out;

}
