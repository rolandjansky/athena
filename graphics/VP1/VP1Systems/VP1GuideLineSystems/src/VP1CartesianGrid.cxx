/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1CartesianGrid                  //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: July 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1GuideLineSystems/VP1CartesianGrid.h"
#include "VP1GuideLineSystems/VP1Floor.h"//for calcPars... static method

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/SbColor4f.h>

#include "GaudiKernel/SystemOfUnits.h"

//____________________________________________________________________
class VP1CartesianGrid::Imp {
public:
  Imp(VP1CartesianGrid *,
      SoSeparator * attachsep);
  VP1CartesianGrid * theclass;
  SoSeparator * attachSep;

  bool shown;
  SbColor4f colourAndTransp;
  double extent;
  double spacing;

  SoSeparator * sep;

  void rebuild3DObjects();
  void updateColour();
};

//____________________________________________________________________
VP1CartesianGrid::VP1CartesianGrid(SoSeparator * attachsep,
		   IVP1System * sys,QObject * parent)
  : QObject(parent), VP1HelperClassBase(sys,"VP1CartesianGrid"), m_d(new Imp(this,attachsep))
{
}

//____________________________________________________________________
VP1CartesianGrid::~VP1CartesianGrid()
{
  setShown(false);
  if (m_d->sep)
    m_d->sep->unref();
  m_d->attachSep->unref();
  delete m_d;
}

//____________________________________________________________________
VP1CartesianGrid::Imp::Imp(VP1CartesianGrid *tc,SoSeparator * as)
  : theclass(tc), attachSep(as), shown(false),
    colourAndTransp(SbColor4f(1,1,1,1)),extent(10), spacing(1), sep(0)
{
  attachSep->ref();
}

//____________________________________________________________________
void VP1CartesianGrid::Imp::rebuild3DObjects()
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
  if (!VP1Floor::calcParsFromExtentAndSpacing( theclass, extent, spacing, 40/*max lines*/, nmax, distmax )) {
    nmax = 10;
    distmax = 10*Gaudi::Units::m;
    theclass->message("ERROR: Problems calculating nmax/distmax.");
  }

  SoVertexProperty * grid_cartesian_vertices = new SoVertexProperty();

  int ivert(0);
  int nsublines(0);

  //First, loop over (x,z) values and add one line parallel with the y-axis through each.
  for (int ix = -nmax; ix<=nmax; ++ix)
    for (int iz = -nmax; iz<=nmax; ++iz) {
      double x = ix*spacing;
      double z = iz*spacing;
      grid_cartesian_vertices->vertex.set1Value(ivert++,x,-distmax,z);
      grid_cartesian_vertices->vertex.set1Value(ivert++,x,+distmax,z);
      ++nsublines;
      }

  //Next, loop over y values and add a plane of lines parallel to the x-z plane.
  for (int iy = -nmax; iy<=nmax; ++iy) {
    double y = iy*spacing;
    for (int ix = -nmax; ix<=nmax; ++ix) {
      double x = ix*spacing;
      grid_cartesian_vertices->vertex.set1Value(ivert++,x,y,-distmax);
      grid_cartesian_vertices->vertex.set1Value(ivert++,x,y,+distmax);
      ++nsublines;
    }
    for (int iz = -nmax; iz<=nmax; ++iz) {
      double z = iz*spacing;
      grid_cartesian_vertices->vertex.set1Value(ivert++,-distmax,y,z);
      grid_cartesian_vertices->vertex.set1Value(ivert++,+distmax,y,z);
      ++nsublines;
    }
  }

  grid_cartesian_vertices->materialBinding=SoMaterialBinding::OVERALL;
  grid_cartesian_vertices->normalBinding=SoNormalBinding::OVERALL;
  SoLineSet * line = new SoLineSet();
  line->numVertices.enableNotify(FALSE);
  line->numVertices.setNum(nsublines);
  for (int i=0;i<nsublines;++i)
    line->numVertices.set1Value(i,2);
  line->vertexProperty = grid_cartesian_vertices;
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
void VP1CartesianGrid::Imp::updateColour()
{
  theclass->messageVerbose("Updating packed colour");
  if (!sep||sep->getNumChildren()<1)
    return;
  SoNode * n = sep->getChild(0);
  if (!n||n->getTypeId()!=SoLineSet::getClassTypeId())
    return;
  SoLineSet * line = static_cast<SoLineSet*>(n);
  SoVertexProperty * vertices = static_cast<SoVertexProperty *>(line->vertexProperty.getValue());
  if (!vertices)
    return;
  vertices->orderedRGBA = colourAndTransp.getPackedValue();
}

//____________________________________________________________________
void VP1CartesianGrid::setShown(bool b)
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
void VP1CartesianGrid::setColourAndTransp(const SbColor4f&ct)
{
  messageVerbose("Signal received in setColourAndTransp slot.");
  if (m_d->colourAndTransp==ct)
    return;
  m_d->colourAndTransp=ct;
  if (m_d->shown)
    m_d->updateColour();
}

//____________________________________________________________________
void VP1CartesianGrid::setExtent(const double& e)
{
  messageVerbose("Signal received: setExtent("+str(e)+")");
  if (m_d->extent==e)
    return;
  m_d->extent=e;
  if (m_d->shown)
    m_d->rebuild3DObjects();
}

//____________________________________________________________________
void VP1CartesianGrid::setSpacing(const double&s)
{
  messageVerbose("Signal received: setSpacing("+str(s)+")");
  if (m_d->spacing==s)
    return;
  m_d->spacing=s;
  if (m_d->shown)
    m_d->rebuild3DObjects();
}
