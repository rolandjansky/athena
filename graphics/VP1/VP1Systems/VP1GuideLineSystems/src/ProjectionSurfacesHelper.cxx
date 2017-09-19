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
ProjectionSurfacesHelper::ProjectionSurfacesHelper( double surfacethickness,
						    double barrel_inner_radius,
						    double barrel_outer_radius,
						    double barrel_posneg_z,
						    double endcap_surface_z,
						    double endcap_surface_length,
						    double endcap_inner_radius,
						    double endcap_outer_radius,
						    double endcap_zasr_disttobarrelcyl,
						    double endcap_zasr_squeezefact,
						    SoMaterial* mat, SoSeparator * attachsep, QObject * parent,IVP1System*sys)
  : QObject(parent), VP1HelperClassBase(sys,"ProjectionSurfacesHelper"), m_d(new Imp(this,attachsep))
{
  m_d->material = mat;
  if (m_d->material)
    m_d->material->ref();
  m_d->sep = 0;
  m_d->barrelcylshape = 0;
  m_d->endcapinnershape = 0;
  m_d->endcapoutershape = 0;
  m_d->endcapzasrcylshape = 0;
  m_d->sep_barrel_central = 0;
  m_d->sep_barrel_pos = 0;
  m_d->sep_barrel_neg = 0;
  m_d->sep_endcap_inner_pos = 0;
  m_d->sep_endcap_outer_pos = 0;
  m_d->sep_endcap_inner_neg = 0;
  m_d->sep_endcap_outer_neg = 0;
  m_d->sep_endcap_zasr_central = 0;
  m_d->sep_endcap_zasr_pos = 0;
  m_d->sep_endcap_zasr_neg = 0;

  m_d->surfacethickness = surfacethickness;
  m_d->barrel_inner_radius = barrel_inner_radius;
  m_d->barrel_outer_radius = barrel_outer_radius;
  m_d->barrel_posneg_z = barrel_posneg_z;
  m_d->endcap_surface_z = endcap_surface_z;
  m_d->endcap_surface_length = endcap_surface_length;
  m_d->endcap_inner_radius = endcap_inner_radius;
  m_d->endcap_outer_radius = endcap_outer_radius;
  m_d->endcap_zasr_disttobarrelcyl = endcap_zasr_disttobarrelcyl;
  m_d->endcap_zasr_squeezefact = endcap_zasr_squeezefact;
}

//____________________________________________________________________
ProjectionSurfacesHelper::~ProjectionSurfacesHelper()
{
  messageVerbose("destructor begin");
  QList<SoNode*> l;

  l << m_d->attachsep
    << m_d->material
    << m_d->sep
    << m_d->barrelcylshape
    << m_d->endcapinnershape
    << m_d->endcapoutershape
    << m_d->endcapzasrcylshape
    << m_d->sep_barrel_central
    << m_d->sep_barrel_pos
    << m_d->sep_barrel_neg
    << m_d->sep_endcap_inner_pos
    << m_d->sep_endcap_outer_pos
    << m_d->sep_endcap_inner_neg
    << m_d->sep_endcap_outer_neg
    << m_d->sep_endcap_zasr_central
    << m_d->sep_endcap_zasr_pos
    << m_d->sep_endcap_zasr_neg;

  foreach(SoNode* n, l)
    if (n)
      n->unref();

  delete m_d;
  messageVerbose("destructor end");
}

//____________________________________________________________________
InDetProjFlags::InDetProjPartsFlags ProjectionSurfacesHelper::shownParts() const
{
  if (verbose())
    messageVerbose("shownParts");
  return m_d->parts;
}

//____________________________________________________________________
void ProjectionSurfacesHelper::setSurfaces( InDetProjFlags::InDetProjPartsFlags newparts )
{
  messageVerbose("setSurfaces");
  if (m_d->parts==newparts)
    return;
  messageVerbose("setSurfaces - changed");
  InDetProjFlags::InDetProjPartsFlags changedparts(m_d->parts ^ newparts);
  m_d->parts = newparts;
  bool save = m_d->attachsep->enableNotify(false);

   //////////////////////////////////////////////

  if (changedparts&InDetProjFlags::BarrelPositive) {
    if (newparts&InDetProjFlags::BarrelPositive) {
      if (!m_d->sep_barrel_pos)
	m_d->initPartsSep(m_d->barrel_posneg_z,m_d->sep_barrel_pos,m_d->barrelCylShape());
      m_d->ensureAttached(m_d->sep_barrel_pos);
    } else {
      m_d->ensureDetached(m_d->sep_barrel_pos);
    }
  }
  if (changedparts&InDetProjFlags::BarrelNegative) {
    if (newparts&InDetProjFlags::BarrelNegative) {
      if (!m_d->sep_barrel_neg)
	m_d->initPartsSep(-m_d->barrel_posneg_z,m_d->sep_barrel_neg,m_d->barrelCylShape());
      m_d->ensureAttached(m_d->sep_barrel_neg);
    } else {
      m_d->ensureDetached(m_d->sep_barrel_neg);
    }
  }
  if (changedparts&InDetProjFlags::BarrelCentral) {
    if (newparts&InDetProjFlags::BarrelCentral) {
      if (!m_d->sep_barrel_central)
	m_d->initPartsSep(0.0,m_d->sep_barrel_central,m_d->barrelCylShape());
      m_d->ensureAttached(m_d->sep_barrel_central);
    } else {
      m_d->ensureDetached(m_d->sep_barrel_central);
    }
  }
  if (changedparts&InDetProjFlags::EndCapInnerPositive) {
    if (newparts&InDetProjFlags::EndCapInnerPositive) {
      if (!m_d->sep_endcap_inner_pos)
	m_d->initPartsSep(m_d->endcap_surface_z,m_d->sep_endcap_inner_pos,m_d->endcapInnerShape());
      m_d->ensureAttached(m_d->sep_endcap_inner_pos);
    } else {
      m_d->ensureDetached(m_d->sep_endcap_inner_pos);
    }
  }
  if (changedparts&InDetProjFlags::EndCapOuterPositive) {
    if (newparts&InDetProjFlags::EndCapOuterPositive) {
      if (!m_d->sep_endcap_outer_pos)
	m_d->initPartsSep(m_d->endcap_surface_z,m_d->sep_endcap_outer_pos,m_d->endcapOuterShape());
      m_d->ensureAttached(m_d->sep_endcap_outer_pos);
    } else {
      m_d->ensureDetached(m_d->sep_endcap_outer_pos);
    }
  }
  if (changedparts&InDetProjFlags::EndCapInnerNegative) {
    if (newparts&InDetProjFlags::EndCapInnerNegative) {
      if (!m_d->sep_endcap_inner_neg)
	m_d->initPartsSep(-m_d->endcap_surface_z,m_d->sep_endcap_inner_neg,m_d->endcapInnerShape());
      m_d->ensureAttached(m_d->sep_endcap_inner_neg);
    } else {
      m_d->ensureDetached(m_d->sep_endcap_inner_neg);
    }
  }
  if (changedparts&InDetProjFlags::EndCapOuterNegative) {
    if (newparts&InDetProjFlags::EndCapOuterNegative) {
      if (!m_d->sep_endcap_outer_neg)
	m_d->initPartsSep(-m_d->endcap_surface_z,m_d->sep_endcap_outer_neg,m_d->endcapOuterShape());
      m_d->ensureAttached(m_d->sep_endcap_outer_neg);
    } else {
      m_d->ensureDetached(m_d->sep_endcap_outer_neg);
    }
  }
  if (changedparts&InDetProjFlags::TRT_EndCapZToRCentral) {
    if (newparts&InDetProjFlags::TRT_EndCapZToRCentral) {
      if (!m_d->sep_endcap_zasr_central)
	m_d->initPartsSep(0.0,m_d->sep_endcap_zasr_central,m_d->endcapZAsRCylShape());
      m_d->ensureAttached(m_d->sep_endcap_zasr_central);
    } else {
      m_d->ensureDetached(m_d->sep_endcap_zasr_central);
    }
  }
  if (changedparts&InDetProjFlags::TRT_EndCapZToRPositive) {
    if (newparts&InDetProjFlags::TRT_EndCapZToRPositive) {
      if (!m_d->sep_endcap_zasr_pos)
	m_d->initPartsSep(m_d->barrel_posneg_z,m_d->sep_endcap_zasr_pos,m_d->endcapZAsRCylShape());
      m_d->ensureAttached(m_d->sep_endcap_zasr_pos);
    } else {
      m_d->ensureDetached(m_d->sep_endcap_zasr_pos);
    }
  }
  if (changedparts&InDetProjFlags::TRT_EndCapZToRNegative) {
    if (newparts&InDetProjFlags::TRT_EndCapZToRNegative) {
      if (!m_d->sep_endcap_zasr_neg)
	m_d->initPartsSep(-m_d->barrel_posneg_z,m_d->sep_endcap_zasr_neg,m_d->endcapZAsRCylShape());
      m_d->ensureAttached(m_d->sep_endcap_zasr_neg);
    } else {
      m_d->ensureDetached(m_d->sep_endcap_zasr_neg);
    }
  }

  //////////////////////////////////////////////
  m_d->attachsep->enableNotify(save);
  if (save)
    m_d->attachsep->touch();
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
  return m_d->material;
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
