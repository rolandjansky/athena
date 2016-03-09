/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1CoordinateAxes                 //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: April 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1GuideLineSystems/VP1CoordinateAxes.h"
#include "VP1Base/VP1QtUtils.h"
#include <Inventor/C/errors/debugerror.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoSphere.h>

//____________________________________________________________________
class VP1CoordinateAxes::Imp {
public:
  Imp(VP1CoordinateAxes *,
      SoMaterial * xmat,
      SoMaterial * ymat,
      SoMaterial * zmat,
      SoSeparator * attachsep);
  VP1CoordinateAxes * theclass;
  SoMaterial * materialXAxis;
  SoMaterial * materialYAxis;
  SoMaterial * materialZAxis;
  SoSeparator * attachSep;

  bool shown;
  SbVec3f origo;
  double axislength;//negative means show negative parts
  double relaxisthick;

  SoSeparator * sep;
  SoTranslation * overall_translation;
  SoSphere * alternative_Sphere;
  SoSeparator * singleaxis_sep;
  SoCylinder * singleaxis_cyl;
  SoCone * singleaxis_cone;
  SoTranslation * singleaxis_cyltranslation;
  SoTranslation * singleaxis_conetranslation;

  void updateFields();
  void ensureInit3DObjects();
};

//____________________________________________________________________
VP1CoordinateAxes::VP1CoordinateAxes(SoMaterial * xmat,SoMaterial * ymat,SoMaterial * zmat,
				     SoSeparator * attachsep,
				     IVP1System * sys,QObject * parent)
  : QObject(parent), VP1HelperClassBase(sys,"VP1CoordinateAxes"), d(new Imp(this,xmat,ymat,zmat,attachsep))
{
}

//____________________________________________________________________
VP1CoordinateAxes::~VP1CoordinateAxes()
{
  setShown(false);
  if (d->sep)
    d->sep->unref();
  if (d->singleaxis_sep)
    d->singleaxis_sep->unref();
  d->materialXAxis->unref();
  d->materialYAxis->unref();
  d->materialZAxis->unref();
  d->attachSep->unref();
  delete d;
}

//____________________________________________________________________
VP1CoordinateAxes::Imp::Imp(VP1CoordinateAxes *tc,SoMaterial * xmat,SoMaterial * ymat,SoMaterial * zmat,SoSeparator * as)
  : theclass(tc), materialXAxis(xmat), materialYAxis(ymat), materialZAxis(zmat), attachSep(as), shown(false),
    origo(SbVec3f(0,0,0)), axislength(1), relaxisthick(0.1),
    sep(0), overall_translation(0), alternative_Sphere(0), singleaxis_sep(0), singleaxis_cyl(0),
    singleaxis_cone(0), singleaxis_cyltranslation(0), singleaxis_conetranslation(0)
{
  materialXAxis->ref();
  materialYAxis->ref();
  materialZAxis->ref();
  attachSep->ref();
}

//____________________________________________________________________
void VP1CoordinateAxes::Imp::ensureInit3DObjects()
{
  if (sep)
    return;
  theclass->messageVerbose("Building 3D objects");
  sep = new SoSeparator; sep->ref();

  singleaxis_sep = new SoSeparator; singleaxis_sep->ref();
  singleaxis_cyltranslation = new SoTranslation; singleaxis_sep->addChild(singleaxis_cyltranslation);
  singleaxis_cyl = new SoCylinder; singleaxis_sep->addChild(singleaxis_cyl);
  singleaxis_conetranslation = new SoTranslation; singleaxis_sep->addChild(singleaxis_conetranslation);
  singleaxis_cone = new SoCone; singleaxis_sep->addChild(singleaxis_cone);

  overall_translation = new SoTranslation;

  sep->addChild(overall_translation);
  const bool alternativeRep = VP1QtUtils::environmentVariableIsOn("VP1_GUIDES_SPHERE_INSTEAD_OF_COORDAXES");
  if (alternativeRep) {
    sep->addChild(materialXAxis);
    alternative_Sphere = new SoSphere;
    sep->addChild(alternative_Sphere);
    return;
  }


  sep->addChild(materialYAxis);
  sep->addChild(singleaxis_sep);//Single-axis points towards y by default.

  SoRotationXYZ * rotx = new SoRotationXYZ;
  rotx->axis.setValue(SoRotationXYZ::Z);
  rotx->angle.setValue(-0.5*M_PI);
  sep->addChild(rotx);

  sep->addChild(materialXAxis);
  sep->addChild(singleaxis_sep);//Single-axis points towards y by default.


  SoRotationXYZ * rotz = new SoRotationXYZ;
  rotz->axis.setValue(SoRotationXYZ::X);
  rotz->angle.setValue(0.5*M_PI);
  sep->addChild(rotz);

  sep->addChild(materialZAxis);
  sep->addChild(singleaxis_sep);//Single-axis points towards y by default.

  singleaxis_cyl->parts.setValue(SoCylinder::SIDES|SoCylinder::BOTTOM);
}

//____________________________________________________________________
void VP1CoordinateAxes::Imp::updateFields()
{
  ensureInit3DObjects();
  theclass->messageVerbose("Updating fields");

  const bool save = sep->enableNotify(false);

  const double cylradius = relaxisthick*fabs(axislength);
  const double coneradius = 1.5*cylradius;
  const double coneheight = 2.0*coneradius;
  overall_translation->translation.setValue(origo);

  if (alternative_Sphere) {
    alternative_Sphere->radius.setValue(fabs(axislength));
  } else {
    singleaxis_cyltranslation->translation.setValue(0.0,(axislength<0?0.0:0.5*fabs(axislength))-0.5*coneheight,0.0);
    singleaxis_cyl->radius.setValue(cylradius);
    singleaxis_cyl->height.setValue( (axislength<0 ? 2.0 : 1.0) * fabs(axislength) - coneheight );
    singleaxis_conetranslation->translation.setValue(0.0,(axislength<0?1.0:0.5)*fabs(axislength),0.0);
    singleaxis_cone->bottomRadius.setValue(coneradius);
    singleaxis_cone->height.setValue(coneheight);
  }

  if (save) {
    sep->enableNotify(true);
    sep->touch();
  }
}

//____________________________________________________________________
void VP1CoordinateAxes::setShown(bool b)
{
  messageVerbose("Signal received: setShown("+str(b)+")");
  if (d->shown==b)
    return;
  d->shown=b;
  if (d->shown) {
    d->updateFields();
    if (d->attachSep->findChild(d->sep)<0)
      d->attachSep->addChild(d->sep);
  } else {
    if (d->sep&&d->attachSep->findChild(d->sep)>=0)
      d->attachSep->removeChild(d->sep);
  }
}

//____________________________________________________________________
void VP1CoordinateAxes::setPosition(const SbVec3f& o)
{
  messageVerbose("Signal received: setPosition("+str(o)+")");
  if (d->origo==o)
    return;
  d->origo=o;
  if (d->shown)
    d->updateFields();
}


//____________________________________________________________________
void VP1CoordinateAxes::setLength(const double&l)
{
  messageVerbose("Signal received: setLength("+str(l)+")");
  if (d->axislength==l)
    return;
  d->axislength=l;
  if (d->shown)
    d->updateFields();
}

//____________________________________________________________________
void VP1CoordinateAxes::setRelativeAxisThickness(const double& t)
{
  messageVerbose("Signal received: setRelativeAxisThickness("+str(t)+")");
  if (d->relaxisthick==t)
    return;
  d->relaxisthick=t;
  if (d->shown)
    d->updateFields();
}
