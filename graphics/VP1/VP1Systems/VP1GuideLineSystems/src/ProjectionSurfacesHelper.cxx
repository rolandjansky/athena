/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1GuideLineSystems/ProjectionSurfacesHelper.h"
#include "VP1GuideLineSystems/InDetProjParams.h"
#include "VP1HEPVis/nodes/SoTubs.h"

#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoPickStyle.h>

//____________________________________________________________________
ProjectionSurfacesHelper * ProjectionSurfacesHelper::createPixelHelper( SoMaterial* mat,
									SoSeparator * attachsep,
									QObject*parent,
									IVP1System* sys )
{
  return new ProjectionSurfacesHelper( InDetProjParams::surfacethickness(),
				       InDetProjParams::pixel_barrel_inner_radius(),
				       InDetProjParams::pixel_barrel_outer_radius(),
				       InDetProjParams::pixel_barrel_posneg_z(),
				       InDetProjParams::pixel_endcap_surface_z(),
				       InDetProjParams::pixel_endcap_surface_length(),
				       InDetProjParams::pixel_endcap_inner_radius(),
				       InDetProjParams::pixel_endcap_outer_radius(),
				       InDetProjParams::pixel_endcap_zasr_disttobarrelcyl(),
				       InDetProjParams::pixel_endcap_zasr_squeezefact(),
				       mat, attachsep, parent,sys );
}

//____________________________________________________________________
ProjectionSurfacesHelper * ProjectionSurfacesHelper::createSCTHelper( SoMaterial* mat,
								      SoSeparator * attachsep,
								      QObject*parent,
								      IVP1System* sys )
{
  return new ProjectionSurfacesHelper( InDetProjParams::surfacethickness(),
				       InDetProjParams::sct_barrel_inner_radius(),
				       InDetProjParams::sct_barrel_outer_radius(),
				       InDetProjParams::sct_barrel_posneg_z(),
				       InDetProjParams::sct_endcap_surface_z(),
				       InDetProjParams::sct_endcap_surface_length(),
				       InDetProjParams::sct_endcap_inner_radius(),
				       InDetProjParams::sct_endcap_outer_radius(),
				       InDetProjParams::sct_endcap_zasr_disttobarrelcyl(),
				       InDetProjParams::sct_endcap_zasr_squeezefact(),
				       mat, attachsep, parent,sys );
}

//____________________________________________________________________
ProjectionSurfacesHelper * ProjectionSurfacesHelper::createTRTHelper( SoMaterial* mat,
								      SoSeparator * attachsep,
								      QObject*parent,
								      IVP1System* sys )
{
  return new ProjectionSurfacesHelper( InDetProjParams::surfacethickness(),
				       InDetProjParams::trt_barrel_inner_radius(),
				       InDetProjParams::trt_barrel_outer_radius(),
				       InDetProjParams::trt_barrel_posneg_z(),
				       InDetProjParams::trt_endcap_surface_z(),
				       InDetProjParams::trt_endcap_surface_length(),
				       InDetProjParams::trt_endcap_inner_radius(),
				       InDetProjParams::trt_endcap_outer_radius(),
				       InDetProjParams::trt_endcap_zasr_disttobarrelcyl(),
				       InDetProjParams::trt_endcap_zasr_squeezefact(),
				       mat, attachsep, parent,sys );
}

//____________________________________________________________________
class ProjectionSurfacesHelper::Imp {
public:
  Imp(ProjectionSurfacesHelper*tc,SoSeparator * as) : theclass(tc),
						      attachsep(as),
						      parts(InDetProjFlags::NoProjections),
						      material(0),
						      sep(0),
						      barrelcylshape(0),
						      endcapinnershape(0),
						      endcapoutershape(0),
						      endcapzasrcylshape(0),
						      sep_barrel_central(0),
						      sep_barrel_pos(0),
						      sep_barrel_neg(0),
						      sep_endcap_inner_pos(0),
						      sep_endcap_outer_pos(0),
						      sep_endcap_inner_neg(0),
						      sep_endcap_outer_neg(0),
						      sep_endcap_zasr_central(0),
						      sep_endcap_zasr_pos(0),
						      sep_endcap_zasr_neg(0),
						      surfacethickness(0),
						      barrel_inner_radius(0),
						      barrel_outer_radius(0),
						      barrel_posneg_z(0),
						      endcap_surface_z(0),
						      endcap_surface_length(0),
						      endcap_inner_radius(0),
						      endcap_outer_radius(0),
						      endcap_zasr_disttobarrelcyl(0),
						      endcap_zasr_squeezefact(0)
  {
    if (as)
      as->ref();
  }

  ProjectionSurfacesHelper * theclass;
  SoSeparator * attachsep;
  InDetProjFlags::InDetProjPartsFlags parts;

  SoMaterial * material;
  SoSeparator * sep;
  SoShape * barrelcylshape;
  SoShape * endcapinnershape;
  SoShape * endcapoutershape;
  SoShape * endcapzasrcylshape;
  SoShape * barrelCylShape();
  SoShape * endcapInnerShape();
  SoShape * endcapOuterShape();
  SoShape * endcapZAsRCylShape();

  SoSeparator * sep_barrel_central;
  SoSeparator * sep_barrel_pos;
  SoSeparator * sep_barrel_neg;
  SoSeparator * sep_endcap_inner_pos;
  SoSeparator * sep_endcap_outer_pos;
  SoSeparator * sep_endcap_inner_neg;
  SoSeparator * sep_endcap_outer_neg;
  SoSeparator * sep_endcap_zasr_central;
  SoSeparator * sep_endcap_zasr_pos;
  SoSeparator * sep_endcap_zasr_neg;

  void initPartsSep(float zpos, SoSeparator*&sep, SoShape*);

  void ensureDetached(SoSeparator*);
  void ensureAttached(SoSeparator*);

  //The parameters:
  double surfacethickness;
  double barrel_inner_radius;
  double barrel_outer_radius;
  double barrel_posneg_z;
  double endcap_surface_z;
  double endcap_surface_length;
  double endcap_inner_radius;
  double endcap_outer_radius;
  double endcap_zasr_disttobarrelcyl;
  double endcap_zasr_squeezefact;
  static const int nphi = 64;
};

//____________________________________________________________________
ProjectionSurfacesHelper::ProjectionSurfacesHelper( const double& _surfacethickness,
						    const double& _barrel_inner_radius,
						    const double& _barrel_outer_radius,
						    const double& _barrel_posneg_z,
						    const double& _endcap_surface_z,
						    const double& _endcap_surface_length,
						    const double& _endcap_inner_radius,
						    const double& _endcap_outer_radius,
						    const double& _endcap_zasr_disttobarrelcyl,
						    const double& _endcap_zasr_squeezefact,
						    SoMaterial* mat, SoSeparator * attachsep, QObject * parent,IVP1System*sys)
  : QObject(parent), VP1HelperClassBase(sys,"ProjectionSurfacesHelper"), d(new Imp(this,attachsep))
{
  d->material = mat;
  if (d->material)
    d->material->ref();
  d->sep = 0;
  d->barrelcylshape = 0;
  d->endcapinnershape = 0;
  d->endcapoutershape = 0;
  d->endcapzasrcylshape = 0;
  d->sep_barrel_central = 0;
  d->sep_barrel_pos = 0;
  d->sep_barrel_neg = 0;
  d->sep_endcap_inner_pos = 0;
  d->sep_endcap_outer_pos = 0;
  d->sep_endcap_inner_neg = 0;
  d->sep_endcap_outer_neg = 0;
  d->sep_endcap_zasr_central = 0;
  d->sep_endcap_zasr_pos = 0;
  d->sep_endcap_zasr_neg = 0;

  d->surfacethickness = _surfacethickness;
  d->barrel_inner_radius = _barrel_inner_radius;
  d->barrel_outer_radius = _barrel_outer_radius;
  d->barrel_posneg_z = _barrel_posneg_z;
  d->endcap_surface_z = _endcap_surface_z;
  d->endcap_surface_length = _endcap_surface_length;
  d->endcap_inner_radius = _endcap_inner_radius;
  d->endcap_outer_radius = _endcap_outer_radius;
  d->endcap_zasr_disttobarrelcyl = _endcap_zasr_disttobarrelcyl;
  d->endcap_zasr_squeezefact = _endcap_zasr_squeezefact;
}

//____________________________________________________________________
ProjectionSurfacesHelper::~ProjectionSurfacesHelper()
{
  messageVerbose("destructor begin");
  QList<SoNode*> l;

  l << d->attachsep
    << d->material
    << d->sep
    << d->barrelcylshape
    << d->endcapinnershape
    << d->endcapoutershape
    << d->endcapzasrcylshape
    << d->sep_barrel_central
    << d->sep_barrel_pos
    << d->sep_barrel_neg
    << d->sep_endcap_inner_pos
    << d->sep_endcap_outer_pos
    << d->sep_endcap_inner_neg
    << d->sep_endcap_outer_neg
    << d->sep_endcap_zasr_central
    << d->sep_endcap_zasr_pos
    << d->sep_endcap_zasr_neg;

  foreach(SoNode* n, l)
    if (n)
      n->unref();

  delete d;
  messageVerbose("destructor end");
}

//____________________________________________________________________
InDetProjFlags::InDetProjPartsFlags ProjectionSurfacesHelper::shownParts() const
{
  if (verbose())
    messageVerbose("shownParts");
  return d->parts;
}

//____________________________________________________________________
void ProjectionSurfacesHelper::setSurfaces( InDetProjFlags::InDetProjPartsFlags newparts )
{
  messageVerbose("setSurfaces");
  if (d->parts==newparts)
    return;
  messageVerbose("setSurfaces - changed");
  InDetProjFlags::InDetProjPartsFlags changedparts(d->parts ^ newparts);
  d->parts = newparts;
  bool save = d->attachsep->enableNotify(false);

   //////////////////////////////////////////////

  if (changedparts&InDetProjFlags::BarrelPositive) {
    if (newparts&InDetProjFlags::BarrelPositive) {
      if (!d->sep_barrel_pos)
	d->initPartsSep(d->barrel_posneg_z,d->sep_barrel_pos,d->barrelCylShape());
      d->ensureAttached(d->sep_barrel_pos);
    } else {
      d->ensureDetached(d->sep_barrel_pos);
    }
  }
  if (changedparts&InDetProjFlags::BarrelNegative) {
    if (newparts&InDetProjFlags::BarrelNegative) {
      if (!d->sep_barrel_neg)
	d->initPartsSep(-d->barrel_posneg_z,d->sep_barrel_neg,d->barrelCylShape());
      d->ensureAttached(d->sep_barrel_neg);
    } else {
      d->ensureDetached(d->sep_barrel_neg);
    }
  }
  if (changedparts&InDetProjFlags::BarrelCentral) {
    if (newparts&InDetProjFlags::BarrelCentral) {
      if (!d->sep_barrel_central)
	d->initPartsSep(0.0,d->sep_barrel_central,d->barrelCylShape());
      d->ensureAttached(d->sep_barrel_central);
    } else {
      d->ensureDetached(d->sep_barrel_central);
    }
  }
  if (changedparts&InDetProjFlags::EndCapInnerPositive) {
    if (newparts&InDetProjFlags::EndCapInnerPositive) {
      if (!d->sep_endcap_inner_pos)
	d->initPartsSep(d->endcap_surface_z,d->sep_endcap_inner_pos,d->endcapInnerShape());
      d->ensureAttached(d->sep_endcap_inner_pos);
    } else {
      d->ensureDetached(d->sep_endcap_inner_pos);
    }
  }
  if (changedparts&InDetProjFlags::EndCapOuterPositive) {
    if (newparts&InDetProjFlags::EndCapOuterPositive) {
      if (!d->sep_endcap_outer_pos)
	d->initPartsSep(d->endcap_surface_z,d->sep_endcap_outer_pos,d->endcapOuterShape());
      d->ensureAttached(d->sep_endcap_outer_pos);
    } else {
      d->ensureDetached(d->sep_endcap_outer_pos);
    }
  }
  if (changedparts&InDetProjFlags::EndCapInnerNegative) {
    if (newparts&InDetProjFlags::EndCapInnerNegative) {
      if (!d->sep_endcap_inner_neg)
	d->initPartsSep(-d->endcap_surface_z,d->sep_endcap_inner_neg,d->endcapInnerShape());
      d->ensureAttached(d->sep_endcap_inner_neg);
    } else {
      d->ensureDetached(d->sep_endcap_inner_neg);
    }
  }
  if (changedparts&InDetProjFlags::EndCapOuterNegative) {
    if (newparts&InDetProjFlags::EndCapOuterNegative) {
      if (!d->sep_endcap_outer_neg)
	d->initPartsSep(-d->endcap_surface_z,d->sep_endcap_outer_neg,d->endcapOuterShape());
      d->ensureAttached(d->sep_endcap_outer_neg);
    } else {
      d->ensureDetached(d->sep_endcap_outer_neg);
    }
  }
  if (changedparts&InDetProjFlags::TRT_EndCapZToRCentral) {
    if (newparts&InDetProjFlags::TRT_EndCapZToRCentral) {
      if (!d->sep_endcap_zasr_central)
	d->initPartsSep(0.0,d->sep_endcap_zasr_central,d->endcapZAsRCylShape());
      d->ensureAttached(d->sep_endcap_zasr_central);
    } else {
      d->ensureDetached(d->sep_endcap_zasr_central);
    }
  }
  if (changedparts&InDetProjFlags::TRT_EndCapZToRPositive) {
    if (newparts&InDetProjFlags::TRT_EndCapZToRPositive) {
      if (!d->sep_endcap_zasr_pos)
	d->initPartsSep(d->barrel_posneg_z,d->sep_endcap_zasr_pos,d->endcapZAsRCylShape());
      d->ensureAttached(d->sep_endcap_zasr_pos);
    } else {
      d->ensureDetached(d->sep_endcap_zasr_pos);
    }
  }
  if (changedparts&InDetProjFlags::TRT_EndCapZToRNegative) {
    if (newparts&InDetProjFlags::TRT_EndCapZToRNegative) {
      if (!d->sep_endcap_zasr_neg)
	d->initPartsSep(-d->barrel_posneg_z,d->sep_endcap_zasr_neg,d->endcapZAsRCylShape());
      d->ensureAttached(d->sep_endcap_zasr_neg);
    } else {
      d->ensureDetached(d->sep_endcap_zasr_neg);
    }
  }

  //////////////////////////////////////////////
  d->attachsep->enableNotify(save);
  if (save)
    d->attachsep->touch();
  messageVerbose("setSurfaces - changed end");
}

//____________________________________________________________________
void ProjectionSurfacesHelper::Imp::initPartsSep(float zpos, SoSeparator*&sep, SoShape*shape)
{
  theclass->messageVerbose("initPartsSep begin");
  sep = new SoSeparator();
  sep->ref();
  if (zpos!=0.0f) {
    SoTranslation * t = new SoTranslation;
    t->translation.setValue(0.0f,0.0f,zpos);
    sep->addChild(t);
  }
  sep->addChild(shape);
  theclass->messageVerbose("initPartsSep end");
}

//____________________________________________________________________
SoMaterial * ProjectionSurfacesHelper::material()
{
  if (verbose())
    messageVerbose("material");
  return d->material;
}

//____________________________________________________________________
SoShape * ProjectionSurfacesHelper::Imp::barrelCylShape()
{
  theclass->messageVerbose("barrelCylShape");
  if (!barrelcylshape) {
    SoTubs::initClass();
    SoTubs * disc = new SoTubs;
    disc->pRMin = barrel_inner_radius;
    disc->pRMax = barrel_outer_radius;
    disc->pDz = 0.5*surfacethickness;
    disc->pSPhi = 0.0;
    disc->pDPhi = 2*M_PI;
    disc->pOverrideNPhi = nphi;//Ignore complexity and just use a good amount of subdivisions =n*32 (its just a single surface anyway)
    barrelcylshape = disc;
    barrelcylshape->ref();
  }
  return barrelcylshape;
}

//____________________________________________________________________
SoShape * ProjectionSurfacesHelper::Imp::endcapInnerShape()
{
  theclass->messageVerbose("endcapInnerShape");
  if (!endcapinnershape) {
    SoTubs::initClass();
    SoTubs * cyl = new SoTubs;
    cyl->pRMax = endcap_inner_radius;
    cyl->pRMin = endcap_inner_radius-surfacethickness;
    cyl->pDz = 0.5*endcap_surface_length;
    cyl->pSPhi = 0.0;
    cyl->pDPhi = 2*M_PI;
    cyl->pOverrideNPhi = nphi;//Ignore complexity and just use a good amount of subdivisions =n*32 (its just a single surface anyway)
    endcapinnershape = cyl;
    endcapinnershape->ref();
  }
  return endcapinnershape;
}

//____________________________________________________________________
SoShape * ProjectionSurfacesHelper::Imp::endcapOuterShape()
{
  theclass->messageVerbose("endcapOuterShape");
  if (!endcapoutershape) {
    SoTubs::initClass();
    SoTubs * cyl = new SoTubs;
    cyl->pRMax = endcap_outer_radius;
    cyl->pRMin = endcap_outer_radius-surfacethickness;
    cyl->pDz = 0.5*endcap_surface_length;
    cyl->pSPhi = 0.0;
    cyl->pDPhi = 2*M_PI;
    cyl->pOverrideNPhi = nphi;//Ignore complexity and just use a good amount of subdivisions =n*32 (its just a single surface anyway)
    endcapoutershape = cyl;
    endcapoutershape->ref();
  }
  return endcapoutershape;
}

//____________________________________________________________________
SoShape * ProjectionSurfacesHelper::Imp::endcapZAsRCylShape()
{
  theclass->messageVerbose("endcapZAsRCylShape");
  if (!endcapzasrcylshape) {
    SoTubs::initClass();
    SoTubs * disc = new SoTubs;
    double rmin = barrel_outer_radius+endcap_zasr_disttobarrelcyl;
    disc->pRMin = rmin;
    disc->pRMax = rmin + endcap_surface_length/endcap_zasr_squeezefact;
    disc->pDz = 0.5*surfacethickness;
    disc->pSPhi = 0.0;
    disc->pDPhi = 2*M_PI;
    disc->pOverrideNPhi = nphi;//Ignore complexity and just use a good amount of subdivisions =n*32 (its just a single surface anyway)
    endcapzasrcylshape = disc;
    endcapzasrcylshape->ref();
  }
  return endcapzasrcylshape;
}

//____________________________________________________________________
void ProjectionSurfacesHelper::Imp::ensureDetached(SoSeparator*s)
{
  if (theclass->verbose())
    theclass->messageVerbose("ensureDetached");
  if (!sep||!s)
    return;
  if (sep->findChild(s)>-1)
    sep->removeChild(s);
}

//____________________________________________________________________
void ProjectionSurfacesHelper::Imp::ensureAttached(SoSeparator*s)
{
  if (theclass->verbose())
    theclass->messageVerbose("ensureAttached");
  if (!s)
    return;
  if (!sep) {
    sep = new SoSeparator;
    sep->ref();
    SoPickStyle *pickStyle = new SoPickStyle;
    pickStyle->style=SoPickStyle::UNPICKABLE;
    sep->addChild(pickStyle);
    if (material)
      sep->addChild(material);
    attachsep->addChild(sep);
  }
  sep->addChild(s);
  if (theclass->verbose())
    theclass->messageVerbose("ensureAttached done");
}
