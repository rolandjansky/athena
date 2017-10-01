/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1Utils/HitsSoNodeManager.h"

#include <Inventor/C/errors/debugerror.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoInfo.h>
#include <Inventor/nodes/SoTransform.h>

#include "VP1HEPVis/nodes/SoGenericBox.h"

#include <map>
#include <iostream>

#include "CxxUtils/fpcompare.h"

//____________________________________________________________________
class HitsSoNodeManager::Imp {
public:
  Imp() :  theclass(0), driftdischeight(0.15), cachedshape_point(0), cached_unittransform(0) {}
  HitsSoNodeManager * theclass;
  const double driftdischeight;
  std::map<double,SoNode*,CxxUtils::fpcompare_fn::less> cachedshapes_drifttubes;
  std::map<double,SoNode*,CxxUtils::fpcompare_fn::less> cachedshapes_driftdiscs;
  std::map<double,SoNode*,CxxUtils::fpcompare_fn::less> cachedshapes_projdrifttubes;
  std::map<double,SoNode*,CxxUtils::fpcompare_fn::less> cachedshapes_strips;
  SoNode * cachedshape_point;
  std::map<double,SoNode*,CxxUtils::fpcompare_fn::less> cachedshapes_cross;
  SoTransform * cached_unittransform;

  //Util, for cleaning up:
  template <class T>
  static void unrefValues(T&t) {
    typename T::iterator it(t.begin()), itE(t.end());
    for (;it!=itE;++it)
      it->second->unref();
  }

  void sanitiseParameterValue( double& x ) {
    //Flips sign of parameter if negative, and makes sure it is not of
    //negligible size.
    if ( x != x ) {
      theclass->message("sanitiseParameterValue WARNING - received nan paramter. Putting to 1.0e-5.");
      x = 1.0e-5;
      return;
    }
    if ( x <= 1.0e-5 ) {
      if ( x < 0.0 ) {
        if (theclass->verbose())
          theclass->messageVerbose("sanitiseParameterValue WARNING - received negative parameter. Flipping sign.");
        x = - x;
      }
      if ( x < 1.0e-5 )
        x = 1.0e-5;
    }
  }
  void sanitiseParameterValueAllowZero( double& x ) {
    //Flips sign of parameter if negative, and makes sure it, if
    //non-zero, that it is not of negligible size.
    if ( x != x ) {
      theclass->message("sanitiseParameterValue WARNING - received nan paramter. Putting to 1.0e-5.");
      x = 1.0e-5;
      return;
    }
    if ( x <= 1.0e-5 ) {
      if ( x < 0.0 ) {
        if (theclass->verbose())
          theclass->messageVerbose("sanitiseParameterValue WARNING - received negative parameter. Flipping sign.");
        x = - x;
      }
      if ( x < 1.0e-5 && x != 0.0 )
        x = 1.0e-5;
    }
  }

  void summarise(unsigned n,const QString& shapename) const;

};


//____________________________________________________________________
HitsSoNodeManager::HitsSoNodeManager(IVP1System * sys)
 : VP1HelperClassBase(sys,"HitsSoNodeManager"), m_d(new Imp)
{
  m_d->theclass = this;
  m_d->cachedshape_point = 0;
  m_d->cached_unittransform = 0;
}


//____________________________________________________________________
void HitsSoNodeManager::Imp::summarise(unsigned n,const QString& shapename) const
{
  if (n>100)
    theclass->messageDebug(" WARNING: Created more than 100 ("+QString::number(n)+") different "+shapename+" shapes.");
  else if (theclass->verbose())
    theclass->messageVerbose("Created "+QString::number(n)+" different "+shapename+" shapes.");
}


//____________________________________________________________________
HitsSoNodeManager::~HitsSoNodeManager()
{
  messageVerbose("destructor begin");

  //Make sure we know if we didn't have efficient shared instancing:
  m_d->summarise(m_d->cachedshapes_drifttubes.size(),"drift tube");
  m_d->summarise(m_d->cachedshapes_driftdiscs.size(),"drift disc");
  m_d->summarise(m_d->cachedshapes_projdrifttubes.size(),"projected drift tube");
  m_d->summarise(m_d->cachedshapes_strips.size(),"strip");
  m_d->summarise(m_d->cachedshapes_cross.size(),"cross");

  //unref kept nodes:
  Imp::unrefValues(m_d->cachedshapes_drifttubes);
  Imp::unrefValues(m_d->cachedshapes_driftdiscs);
  Imp::unrefValues(m_d->cachedshapes_projdrifttubes);
  Imp::unrefValues(m_d->cachedshapes_strips);
  Imp::unrefValues(m_d->cachedshapes_cross);
  if (m_d->cachedshape_point)
    m_d->cachedshape_point->unref();
  if (m_d->cached_unittransform)
    m_d->cached_unittransform->unref();

  delete m_d;
  messageVerbose("destructor end");
}

//____________________________________________________________________
SoNode* HitsSoNodeManager::getShapeNode_DriftTube( double halfLength, double radius )
{
  m_d->sanitiseParameterValue(halfLength);
  m_d->sanitiseParameterValueAllowZero(radius);
  //Fixme: discretize by radius!!

  double id(halfLength-999999999.9*radius);
  std::map<double,SoNode*>::const_iterator it = m_d->cachedshapes_drifttubes.find(id);
  if (it!=m_d->cachedshapes_drifttubes.end())
    return it->second;

  if (radius>0) {
    SoSeparator*sep = new SoSeparator;
    SoRotationXYZ * rot = new SoRotationXYZ;
    rot->axis.setValue(SoRotationXYZ::X);
    rot->angle.setValue(M_PI*0.5f);
    sep->addChild(rot);
    SoCylinder * cyl = new SoCylinder;
    cyl->radius.setValue(radius);
    cyl->height.setValue(2.0f*halfLength);
    sep->addChild(cyl);
    m_d->cachedshapes_drifttubes[id] = sep;
    sep->ref();
    return sep;
  } else {
    SoVertexProperty * scatVtxProperty = new SoVertexProperty();
    scatVtxProperty->vertex.set1Value(0, 0.0f,0.0f,-halfLength);
    scatVtxProperty->vertex.set1Value(1, 0.0f,0.0f, halfLength);
    SoLineSet * line = new SoLineSet();
    line->numVertices = 2;
    line->vertexProperty = scatVtxProperty;
    m_d->cachedshapes_drifttubes[id] = line;
    line->ref();
    return line;
  }

}

//____________________________________________________________________
SoNode* HitsSoNodeManager::getShapeNode_DriftDisc( double radius )
{
  m_d->sanitiseParameterValueAllowZero(radius);
  if (radius==0.0)
    return getShapeNode_Point();

  std::map<double,SoNode*>::const_iterator it = m_d->cachedshapes_driftdiscs.find(radius);
  if (it!=m_d->cachedshapes_driftdiscs.end())
    return it->second;

  SoSeparator*sep = new SoSeparator;//fixme: check if sogroup improves performance.
  SoRotationXYZ * rot = new SoRotationXYZ;
  rot->axis.setValue(SoRotationXYZ::X);
  rot->angle.setValue(M_PI*0.5f);
  sep->addChild(rot);
  SoCylinder * cyl = new SoCylinder;
  cyl->radius.setValue(radius);
  cyl->height.setValue(m_d->driftdischeight);
  sep->addChild(cyl);

  m_d->cachedshapes_driftdiscs[radius] = sep;
  sep->ref();
  return sep;
}

//____________________________________________________________________
SoNode* HitsSoNodeManager::getShapeNode_ProjectedDriftTube( double halfLength, double radius,
							   bool inner, bool outer )
{
  m_d->sanitiseParameterValue(halfLength);
  m_d->sanitiseParameterValueAllowZero(radius);
  //Fixme: discretize by radius!!

  double id(halfLength-9999.9*radius-(inner?0.0:-9999799.99)-(outer?0.0:-9999997979.99));//something unique
  std::map<double,SoNode*>::const_iterator it = m_d->cachedshapes_projdrifttubes.find(id);
  if (it!=m_d->cachedshapes_projdrifttubes.end())
    return it->second;

  if (!inner&&!outer) {
    //Should probably never be called. But whatever:
    SoInfo * info = new SoInfo;//something harmless and lightweight.
    info->ref();
    m_d->cachedshapes_projdrifttubes[id] = info;
    return info;
  }

  //Fix for inner projections to put the hits from short barrel straws onto same z as the long barrel straws:
  double halfLengthInner = halfLength<160.0 ? 2*349.3150-halfLength : halfLength;
  m_d->sanitiseParameterValue(halfLengthInner);

  if (radius>0) {
    SoSeparator*sep = new SoSeparator;//fixme: check if sogroup improves performance.
    SoRotationXYZ * rot = new SoRotationXYZ;
    rot->axis.setValue(SoRotationXYZ::X);
    rot->angle.setValue(M_PI*0.5f);
    sep->addChild(rot);
    SoCylinder * cyl = new SoCylinder;
    cyl->radius.setValue(radius);
    cyl->height.setValue(m_d->driftdischeight);
    if (inner) {
      SoTranslation * trans = new SoTranslation;
      trans->translation.setValue(0.0f,halfLengthInner,0.0f);
      sep->addChild(trans);
      sep->addChild(cyl);
    }
    if (outer) {
      SoTranslation * trans = new SoTranslation;
      trans->translation.setValue(0.0f,(inner?-halfLength-halfLengthInner:-halfLength),0.0f);
      sep->addChild(trans);
      sep->addChild(cyl);
    }
    m_d->cachedshapes_projdrifttubes[id] = sep;
    sep->ref();
    return sep;
  } else {
    SoPointSet       * scatPointSet    = new SoPointSet;
    SoVertexProperty * scatVtxProperty = new SoVertexProperty;
    int i(0);
    if (inner)
      scatVtxProperty->vertex.set1Value(i++,0.0f,0.0f,halfLengthInner);
    if (outer)
      scatVtxProperty->vertex.set1Value(i++,0.0f,0.0f,-halfLength);
    scatPointSet->numPoints=i;
    scatPointSet->vertexProperty.setValue(scatVtxProperty);
    m_d->cachedshapes_projdrifttubes[id] = scatPointSet;
    scatPointSet->ref();
    return scatPointSet;
  }
}


//____________________________________________________________________
SoNode* HitsSoNodeManager::getShapeNode_Strip( double length, double width, double depth )
{
  //std::cout << "HitsSoNodeManager::getShapeNode_Strip()" << std::endl;

  m_d->sanitiseParameterValueAllowZero(length);
  m_d->sanitiseParameterValueAllowZero(width);
  m_d->sanitiseParameterValueAllowZero(depth);

  double id(length-width*9999.99-depth*999999799.0);//something unique
  std::map<double,SoNode*>::const_iterator it = m_d->cachedshapes_strips.find(id);
  if (it!=m_d->cachedshapes_strips.end())
    return it->second;

  if (width==0.0&&depth==0) {
    if (length==0.0) {
    	std::cout << "lengthStrip==0.0, returning a point..." << std::endl;
      return getShapeNode_Point();
    }
    //Return a line:
    SoVertexProperty * scatVtxProperty = new SoVertexProperty();
    scatVtxProperty->vertex.set1Value(0, 0.0f,-length*0.5f,0.0f);
    scatVtxProperty->vertex.set1Value(1, 0.0f, length*0.5f,0.0f);
    SoLineSet * line = new SoLineSet();
    line->numVertices = 2;
    line->vertexProperty = scatVtxProperty;
    m_d->cachedshapes_strips[id] = line;
    line->ref();
    return line;
  } else {
    //Return a box.
    SoGenericBox::initClass();
    SoGenericBox* cube = new SoGenericBox;
    m_d->sanitiseParameterValue(length);
    m_d->sanitiseParameterValue(width);
    m_d->sanitiseParameterValue(depth);
    cube->setParametersForBox(0.5*width,0.5*length,0.5*depth);
    m_d->cachedshapes_strips[id] = cube;
    cube->drawEdgeLines = true;
    cube->ref();
    return cube;
  }
  /** Can never reach this line (coverity 31149)
  std::cout << "Returning zero!" << std::endl;
  return 0;
  **/
}

//____________________________________________________________________
SoNode* HitsSoNodeManager::getShapeNode_Wire( double length, double minWidth, double maxWidth, double depth )
{

  //std::cout << "HitsSoNodeManager::getShapeNode_Wire()" << std::endl;

  m_d->sanitiseParameterValueAllowZero(length);
  m_d->sanitiseParameterValueAllowZero(minWidth);
  m_d->sanitiseParameterValueAllowZero(maxWidth);
  m_d->sanitiseParameterValueAllowZero(depth);

  //double id(length-minWidth*9999.99-depth*999999799.0);//something unique
  //std::map<double,SoNode*>::const_iterator it = m_d->cachedshapes_strips.find(id);
  //if (it!=m_d->cachedshapes_strips.end())
  //  return it->second;

  if (maxWidth==0.0&&depth==0) {
    if (length==0.0) {
    	std::cout << "lengthWire==0.0, returning a point..." << std::endl;
      return getShapeNode_Point();
    }
    //Return a line:
    SoVertexProperty * scatVtxProperty = new SoVertexProperty();
    scatVtxProperty->vertex.set1Value(0, 0.0f,-length*0.5f,0.0f);
    scatVtxProperty->vertex.set1Value(1, 0.0f, length*0.5f,0.0f);
    SoLineSet * line = new SoLineSet();
    line->numVertices = 2;
    line->vertexProperty = scatVtxProperty;
    //m_d->cachedshapes_strips[id] = line;
    line->ref();
    return line;
  } else {
    //Return a box.
    SoGenericBox::initClass();
    SoGenericBox* trd = new SoGenericBox;
    m_d->sanitiseParameterValue(length);
    m_d->sanitiseParameterValue(minWidth);
    m_d->sanitiseParameterValue(maxWidth);
    m_d->sanitiseParameterValue(depth);
    trd->setParametersForTrd(0.5*minWidth,0.5*maxWidth,0.5*length,0.5*length,0.5*depth);
    //m_d->cachedshapes_strips[id] = trd;
    trd->drawEdgeLines = true;
    trd->ref();
    return trd;
  }
}
//____________________________________________________________________
SoNode* HitsSoNodeManager::getShapeNode_Pad( double length, double minWidth, double maxWidth, double depth )
{

  //std::cout << "HitsSoNodeManager::getShapeNode_Pad()" << std::endl;

  m_d->sanitiseParameterValueAllowZero(length);
  m_d->sanitiseParameterValueAllowZero(minWidth);
  m_d->sanitiseParameterValueAllowZero(maxWidth);
  m_d->sanitiseParameterValueAllowZero(depth);

  //double id(length-minWidth*9999.99-depth*999999799.0);//something unique
  //std::map<double,SoNode*>::const_iterator it = m_d->cachedshapes_strips.find(id);
  //if (it!=m_d->cachedshapes_strips.end())
  //  return it->second;

  if (maxWidth==0.0 && depth==0) {
    if (length==0.0) {
    	std::cout << "maxWidth==0.0 && depth==0 && lengthPad==0.0, returning a point..." << std::endl;
      return getShapeNode_Point();
    }
    //Return a line:
    //std::cout << "maxWidth==0.0 && depth==0, returning a line..." << std::endl;
    SoVertexProperty * scatVtxProperty = new SoVertexProperty();
    scatVtxProperty->vertex.set1Value(0, 0.0f,-length*0.5f,0.0f);
    scatVtxProperty->vertex.set1Value(1, 0.0f, length*0.5f,0.0f);
    SoLineSet * line = new SoLineSet();
    line->numVertices = 2;
    line->vertexProperty = scatVtxProperty;
    //m_d->cachedshapes_strips[id] = line;
    line->ref();
    return line;
  } else {
    //std::cout << "returning a box..." << std::endl;
    //Return a box.
    SoGenericBox::initClass();
    SoGenericBox* trd = new SoGenericBox;
    m_d->sanitiseParameterValue(length);
    m_d->sanitiseParameterValue(minWidth);
    m_d->sanitiseParameterValue(maxWidth);
    m_d->sanitiseParameterValue(depth);
    trd->setParametersForTrd(0.5*minWidth,0.5*maxWidth,0.5*length,0.5*length,0.5*depth);
    //m_d->cachedshapes_strips[id] = trd;
    trd->drawEdgeLines = true;
    trd->ref();
    return trd;
  }
}

//____________________________________________________________________
SoNode* HitsSoNodeManager::getShapeNode_Point()
{
  if (!m_d->cachedshape_point) {
    SoPointSet       * scatPointSet    = new SoPointSet;
    SoVertexProperty * scatVtxProperty = new SoVertexProperty;
    scatVtxProperty->vertex.set1Value(0,0.0f,0.0f,0.0f);
    scatPointSet->numPoints=1;
    scatPointSet->vertexProperty.setValue(scatVtxProperty);
    m_d->cachedshape_point = scatPointSet;
    m_d->cachedshape_point->ref();
  }
  return m_d->cachedshape_point;
}

//____________________________________________________________________
SoNode* HitsSoNodeManager::getShapeNode_Cross( double extent )
{
  std::map<double,SoNode*>::const_iterator it = m_d->cachedshapes_cross.find(extent);
  if (it!=m_d->cachedshapes_cross.end())
    return it->second;

  SoVertexProperty * vertices = new SoVertexProperty;
  int iver(0);
  vertices->vertex.set1Value(iver++,-extent,0.0,0.0);
  vertices->vertex.set1Value(iver++,+extent,0.0,0.0);
  vertices->vertex.set1Value(iver++,0.0,-extent,0.0);
  vertices->vertex.set1Value(iver++,0.0,+extent,0.0);
  vertices->vertex.set1Value(iver++,0.0,0.0,-extent);
  vertices->vertex.set1Value(iver++,0.0,0.0,+extent);

  SoLineSet * cross = new SoLineSet;
  cross->vertexProperty = vertices;
  int numlines(0);
  cross->numVertices.set1Value(numlines++,2);
  cross->numVertices.set1Value(numlines++,2);
  cross->numVertices.set1Value(numlines++,2);

  m_d->cachedshapes_cross[extent] = cross;
  m_d->cachedshapes_cross[extent]->ref();

  return m_d->cachedshapes_cross[extent];
}

//____________________________________________________________________
SoTransform * HitsSoNodeManager::getUnitTransform()
{
  if (!m_d->cached_unittransform) {
    m_d->cached_unittransform = new SoTransform;
    m_d->cached_unittransform->ref();
  }
  return m_d->cached_unittransform;
}
