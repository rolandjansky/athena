/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1CylindricalGrid                //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: July 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1GuideLineSystems/VP1CylindricalGrid.h"
#include "VP1GuideLineSystems/VP1Floor.h"//for calcPars... static method
#include "VP1Base/VP1QtInventorUtils.h"

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoPackedColor.h>
#include <Inventor/SbColor4f.h>

#include "GaudiKernel/SystemOfUnits.h"

//____________________________________________________________________
class VP1CylindricalGrid::Imp {
public:
  Imp(VP1CylindricalGrid *,
      SoSeparator * attachsep);
  VP1CylindricalGrid * theclass;
  SoSeparator * attachSep;

  bool shown;
  SbColor4f colourAndTransp;
  double extent;
  double spacing;

  SoSeparator * sep;
  SoPackedColor * circlecol;

  void rebuild3DObjects();
  void updateColour();
};

//____________________________________________________________________
VP1CylindricalGrid::VP1CylindricalGrid(SoSeparator * attachsep,
		   IVP1System * sys,QObject * parent)
  : QObject(parent), VP1HelperClassBase(sys,"VP1CylindricalGrid"), m_d(new Imp(this,attachsep))
{
}

//____________________________________________________________________
VP1CylindricalGrid::~VP1CylindricalGrid()
{
  setShown(false);
  if (m_d->sep)
    m_d->sep->unref();
  if (m_d->circlecol)
    m_d->circlecol->unref();
  m_d->attachSep->unref();
  delete m_d;
}

//____________________________________________________________________
VP1CylindricalGrid::Imp::Imp(VP1CylindricalGrid *tc,SoSeparator * as)
  : theclass(tc), attachSep(as), shown(false),
    colourAndTransp(SbColor4f(1,1,1,1)),extent(10), spacing(1),
    sep(0), circlecol(0)
{
  attachSep->ref();
}

//____________________________________________________________________
void VP1CylindricalGrid::Imp::rebuild3DObjects()
{
  theclass->messageVerbose("(Re)building 3D objects");

  if (sep) {
    sep->removeAllChildren();
  } else {
    sep = new SoSeparator;
    sep->ref();
  }

  const bool save = sep->enableNotify(false);

  int nmax; double distmax;
  if (!VP1Floor::calcParsFromExtentAndSpacing( theclass, extent, spacing, 20/*max lines*/, nmax, distmax )) {
    nmax = 10;
    distmax = 10*Gaudi::Units::m;
    theclass->message("ERROR: Problems calculating nmax/distmax.");
  }

  if (!circlecol) {
    circlecol = new SoPackedColor();
    circlecol->ref();
  }

  //Add circles:
  sep->addChild(circlecol);

  for (int ir=1;ir<=nmax;++ir) {
    SoNode * circle = VP1QtInventorUtils::createCircle(ir*spacing);
    SoSeparator * s = new SoSeparator;
    SoTranslation *t1= new SoTranslation();
    t1->translation.setValue(0.0,0.0,-distmax-spacing);
    s->addChild(t1);
    for (int iz = -nmax; iz<=nmax; ++iz) {
      SoTranslation *t2= new SoTranslation();
      t2->translation.setValue(0.0,0.0,spacing);
      s->addChild(t2);
      s->addChild(circle);
    }
    sep->addChild(s);
  }

  //Add lines:

  SoVertexProperty * grid_cylindrical_vertices = new SoVertexProperty();

  int ivert(0);
  int nsublines(0);
  static const double invsqrttwo = 1.0/sqrt(2.0);
  for (int ir=1;ir<=nmax;++ir) {
    double r = ir*spacing;
    double r2 = r*invsqrttwo;
    grid_cylindrical_vertices->vertex.set1Value(ivert++,r,0,-distmax);
    grid_cylindrical_vertices->vertex.set1Value(ivert++,r,0,+distmax);
    grid_cylindrical_vertices->vertex.set1Value(ivert++,0,r,-distmax);
    grid_cylindrical_vertices->vertex.set1Value(ivert++,0,r,+distmax);
    grid_cylindrical_vertices->vertex.set1Value(ivert++,-r,0,-distmax);
    grid_cylindrical_vertices->vertex.set1Value(ivert++,-r,0,+distmax);
    grid_cylindrical_vertices->vertex.set1Value(ivert++,0,-r,-distmax);
    grid_cylindrical_vertices->vertex.set1Value(ivert++,0,-r,+distmax);
    grid_cylindrical_vertices->vertex.set1Value(ivert++,r2,r2,-distmax);
    grid_cylindrical_vertices->vertex.set1Value(ivert++,r2,r2,+distmax);
    grid_cylindrical_vertices->vertex.set1Value(ivert++,r2,-r2,-distmax);
    grid_cylindrical_vertices->vertex.set1Value(ivert++,r2,-r2,+distmax);
    grid_cylindrical_vertices->vertex.set1Value(ivert++,-r2,r2,-distmax);
    grid_cylindrical_vertices->vertex.set1Value(ivert++,-r2,r2,+distmax);
    grid_cylindrical_vertices->vertex.set1Value(ivert++,-r2,-r2,-distmax);
    grid_cylindrical_vertices->vertex.set1Value(ivert++,-r2,-r2,+distmax);
    nsublines+=8;
  }

  grid_cylindrical_vertices->materialBinding=SoMaterialBinding::OVERALL;
  grid_cylindrical_vertices->normalBinding=SoNormalBinding::OVERALL;
  SoLineSet * line = new SoLineSet();
  line->numVertices.enableNotify(FALSE);
  line->numVertices.setNum(nsublines);
  for (int i=0;i<nsublines;++i)
    line->numVertices.set1Value(i,2);
  line->vertexProperty = grid_cylindrical_vertices;
  line->numVertices.enableNotify(TRUE);
  line->numVertices.touch();

  sep->addChild(line);
  updateColour();

  if (save) {
    sep->enableNotify(true);
    sep->touch();
  }

}

//____________________________________________________________________
void VP1CylindricalGrid::Imp::updateColour()
{
  theclass->messageVerbose("Updating packed colour");
  if (circlecol)
    circlecol->orderedRGBA = colourAndTransp.getPackedValue();
  if (!sep||sep->getNumChildren()<1)
    return;
  SoNode * n = sep->getChild(0);
  if (!n||n->getTypeId()!=SoLineSet::getClassTypeId())
    return;
  SoLineSet * line = static_cast<SoLineSet*>(n);
  SoVertexProperty * vertices = static_cast<SoVertexProperty *>(line->vertexProperty.getValue());
  if (vertices)
    vertices->orderedRGBA = colourAndTransp.getPackedValue();
}

//____________________________________________________________________
void VP1CylindricalGrid::setShown(bool b)
{
  messageVerbose("Signal received: setShown("+str(b)+")");
  if (m_d->shown==b)
    return;
  m_d->shown=b;
  if (m_d->shown) {
    m_d->rebuild3DObjects();
    if (m_d->attachSep->findChild(m_d->sep)<0)
      m_d->attachSep->addChild(m_d->sep);
  } else {
    if (m_d->sep&&m_d->attachSep->findChild(m_d->sep)>=0)
      m_d->attachSep->removeChild(m_d->sep);
  }
}

//____________________________________________________________________
void VP1CylindricalGrid::setColourAndTransp(const SbColor4f&ct)
{
  messageVerbose("Signal received in setColourAndTransp slot.");
  if (m_d->colourAndTransp==ct)
    return;
  m_d->colourAndTransp=ct;
  if (m_d->shown)
    m_d->updateColour();
}

//____________________________________________________________________
void VP1CylindricalGrid::setExtent(const double& e)
{
  messageVerbose("Signal received: setExtent("+str(e)+")");
  if (m_d->extent==e)
    return;
  m_d->extent=e;
  if (m_d->shown)
    m_d->rebuild3DObjects();
}

//____________________________________________________________________
void VP1CylindricalGrid::setSpacing(const double&s)
{
  messageVerbose("Signal received: setSpacing("+str(s)+")");
  if (m_d->spacing==s)
    return;
  m_d->spacing=s;
  if (m_d->shown)
    m_d->rebuild3DObjects();
}
