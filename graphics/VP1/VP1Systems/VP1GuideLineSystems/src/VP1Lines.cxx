/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1GuideLineSystems/VP1Lines.h"

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/SbColor4f.h>

#include "GaudiKernel/SystemOfUnits.h"


//____________________________________________________________________
class VP1Lines::Imp {
public:
  Imp(VP1Lines *,
      SoSeparator * attachsep);
  VP1Lines * theclass;
  SoSeparator * attachSep;

  bool shown;
  SbColor4f colourAndTransp;
  SbVec3f direction;

  SoSeparator * sep;

  void rebuild3DObjects();
  void updateColour();
};

//____________________________________________________________________
VP1Lines::VP1Lines(SoSeparator * attachsep,
		   IVP1System * sys,QObject * parent)
  : QObject(parent), VP1HelperClassBase(sys,"VP1Lines"), m_d(new Imp(this,attachsep))
{
}

//____________________________________________________________________
VP1Lines::~VP1Lines()
{
  setShown(false);
  if (m_d->sep)
    m_d->sep->unref();
  m_d->attachSep->unref();
  delete m_d;
}

//____________________________________________________________________
VP1Lines::Imp::Imp(VP1Lines *tc,SoSeparator * as)
  : theclass(tc), attachSep(as), shown(false),
    colourAndTransp(SbColor4f(1,1,1,1)), direction(SbVec3f(0,0,0)), sep(0)
{
  attachSep->ref();
}

//____________________________________________________________________
void VP1Lines::Imp::rebuild3DObjects()
{
  theclass->messageVerbose("(Re)building 3D objects");

  if (sep) {
    sep->removeAllChildren();
  } else {
    sep = new SoSeparator;
    sep->ref();
  }

  const bool save = sep->enableNotify(false);

  SoVertexProperty * line_vertices = new SoVertexProperty();

  line_vertices->vertex.set1Value(0,0.0,0.0,0.0);
  line_vertices->vertex.set1Value(1,direction[0],direction[1],direction[2]);

  line_vertices->materialBinding=SoMaterialBinding::OVERALL;
  line_vertices->normalBinding=SoNormalBinding::OVERALL;
  SoLineSet * line = new SoLineSet();
  line->numVertices.enableNotify(FALSE);
  // line->numVertices.setNum(1);
  // line->numVertices.set1Value(0,2);
  line->vertexProperty = line_vertices;
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
void VP1Lines::Imp::updateColour()
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
void VP1Lines::setShown(bool b)
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
void VP1Lines::setColourAndTransp(const SbColor4f&ct)
{
  messageVerbose("Signal received in setColourAndTransp slot.");
  if (m_d->colourAndTransp==ct)
    return;
  m_d->colourAndTransp=ct;
  if (m_d->shown)
    m_d->updateColour();
}

//____________________________________________________________________
void VP1Lines::setDirection(const SbVec3f& o)
{
  messageVerbose("Signal received: setDirection("+str(o)+")");
  if (m_d->direction==o)
    return;
  m_d->direction=o;
  if (m_d->shown)
    m_d->rebuild3DObjects();
}
