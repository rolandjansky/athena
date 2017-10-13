/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1EtaCone                        //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: April 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1GuideLineSystems/VP1EtaCone.h"

#include <Inventor/C/errors/debugerror.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoRotationXYZ.h>

#include "GaudiKernel/SystemOfUnits.h"

//____________________________________________________________________
class VP1EtaCone::Imp {
public:
  Imp(VP1EtaCone *,
      SoMaterial * mat,
      SoSeparator * attachsep);
  VP1EtaCone * theclass;
  SoMaterial * material;
  SoSeparator * attachSep;

  bool shown;
  double etaval;
  double extent;

  SoSeparator * sep;
  SoCone * cone1;
  SoTranslation * trans1;
  SoCone * innercone1;
  SoTranslation * innertrans1;
  SoCone * cone2;
  SoTranslation * trans2;
  SoCone * innercone2;
  SoTranslation * innertrans2;

  void updateFields();
  void ensureInit3DObjects();
};

//____________________________________________________________________
VP1EtaCone::VP1EtaCone(SoMaterial * mat,SoSeparator * attachsep,
		       IVP1System * sys,QObject * parent)
  : QObject(parent), VP1HelperClassBase(sys,"VP1EtaCone"), m_d(new Imp(this,mat,attachsep))
{
}

//____________________________________________________________________
VP1EtaCone::~VP1EtaCone()
{
  setShown(false);
  if (m_d->sep)
    m_d->sep->unref();
  m_d->material->unref();
  m_d->attachSep->unref();
  delete m_d;
}

//____________________________________________________________________
VP1EtaCone::Imp::Imp(VP1EtaCone *tc,SoMaterial * mat,SoSeparator * as)
  : theclass(tc), material(mat), attachSep(as), shown(false),
    etaval(1), extent(1), sep(0),
    cone1(0), trans1(0), innercone1(0), innertrans1(0),
    cone2(0), trans2(0), innercone2(0), innertrans2(0)
{
  material->ref();
  attachSep->ref();
}

//____________________________________________________________________
void VP1EtaCone::Imp::ensureInit3DObjects()
{
  if (sep)
    return;
  theclass->messageVerbose("Building 3D objects");
  sep = new SoSeparator;
  sep->ref();
  sep->addChild(material);

  SoSeparator * sepfirst = new SoSeparator;
  SoSeparator * sepsecond = new SoSeparator;

  for ( int i = 0;i<2;++i){
    SoRotationXYZ * xf = new SoRotationXYZ();
    xf->axis=SoRotationXYZ::X;
    xf->angle = i==0 ? 90.0*Gaudi::Units::deg : -90*Gaudi::Units::deg;
    SoTranslation  * xl = new SoTranslation();
    SoCone * cone = new SoCone();
    SoTranslation  * innerxl = new SoTranslation();
    SoCone * innercone = new SoCone();

    if (i==0) {
      cone1 = cone;
      trans1 = xl;
      innercone1 = innercone;
      innertrans1 = innerxl;
    } else {
      cone2 = cone;
      trans2 = xl;
      innercone2 = innercone;
      innertrans2 = innerxl;
    }

    cone->removePart(SoCone::BOTTOM);
    innercone->removePart(SoCone::BOTTOM);

    SoSeparator * s(i==0?sepfirst:sepsecond);
    s->addChild(xf);
    s->addChild(xl);
    s->addChild(cone);
    s->addChild(innerxl);
    s->addChild(innercone);
    sep->addChild(s);
  }
}

//____________________________________________________________________
void VP1EtaCone::Imp::updateFields()
{
  ensureInit3DObjects();
  theclass->messageVerbose("Updating fields");

  double theta = 2*atan(exp(-fabs(etaval)));
  double tanfact = tan(theta);
  bool etaconeextentisz = (extent<0);
  double absextent = fabs(extent);
  double coneHeight,bottomRadius;
  if (etaconeextentisz) {
    coneHeight = absextent;
    bottomRadius = coneHeight*tanfact;
  } else {
    bottomRadius = absextent;
    coneHeight=bottomRadius/tanfact;
  }

  theclass->messageVerbose("etaval = "+str(etaval));
  theclass->messageVerbose("extent = "+str(extent));
  theclass->messageVerbose("absextent = "+str(absextent));
  theclass->messageVerbose("tanfact = "+str(tanfact));
  theclass->messageVerbose("bottomRadius = "+str(bottomRadius));
  theclass->messageVerbose("coneHeight = "+str(coneHeight));

  cone1->bottomRadius = bottomRadius;
  cone1->height = coneHeight;
  cone2->bottomRadius = bottomRadius;
  cone2->height = coneHeight;
  trans1->translation.setValue(0, -coneHeight/2, 0);
  trans2->translation.setValue(0, -coneHeight/2, 0);

  innercone1->bottomRadius = bottomRadius;
  innercone1->height = -coneHeight;
  innercone2->bottomRadius = bottomRadius;
  innercone2->height = -coneHeight;
  //To avoid render flicker from the overlapping cones, we move the inner cone slightly away from origo:
  const double epsilon = 0.50*Gaudi::Units::mm;
  innertrans1->translation.setValue(0, coneHeight/2+1.001*coneHeight/2+epsilon, 0);
  innertrans2->translation.setValue(0, coneHeight/2+1.001*coneHeight/2+epsilon, 0);

}

//____________________________________________________________________
void VP1EtaCone::setShown(bool b)
{
  messageVerbose("Signal received: setShown("+str(b)+")");
  if (m_d->shown==b)
    return;
  m_d->shown=b;
  if (m_d->shown) {
    m_d->updateFields();
    if (m_d->attachSep->findChild(m_d->sep)<0)
      m_d->attachSep->addChild(m_d->sep);
  } else {
    if (m_d->sep&&m_d->attachSep->findChild(m_d->sep)>=0)
      m_d->attachSep->removeChild(m_d->sep);
  }
}

//____________________________________________________________________
void VP1EtaCone::setExtent(const double&p)
{
  messageVerbose("Signal received: setExtent("+str(p)+")");
  if (m_d->extent==p)
    return;
  m_d->extent=p;
  if (m_d->shown)
    m_d->updateFields();
}

//____________________________________________________________________
void VP1EtaCone::setEtaValue(const double&p)
{
  messageVerbose("Signal received: setEtaValue("+str(p)+")");
  if (m_d->etaval==p)
    return;
  m_d->etaval=p;
  if (m_d->shown)
    m_d->updateFields();
}
