/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1RawDataHandleBase               //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: April 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1RawDataSystems/VP1RawDataHandleBase.h"
#include "VP1RawDataSystems/VP1RawDataCommonData.h"
#include "VP1Base/VP1ExtraSepLayerHelper.h"

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoMaterial.h>

#include <sstream>

//____________________________________________________________________
class VP1RawDataHandleBase::Imp {
public:
  Imp() : sep(0),isAttached(false) {}
  void rebuild3DObjects(VP1RawDataHandleBase*);
  void ensureAttach3DObjects(VP1RawDataHandleBase*theclass);
  void ensureDetach3DObjects(VP1RawDataHandleBase*theclass);
  void ensureInitSepAndTransform(VP1RawDataHandleBase*theclass);
  SoSeparator * sep;//First child is transform, second - if present - is node.
  bool isAttached;

  bool hasShape() const
  {
    return sep&&sep->getNumChildren()>1;
  }

  void clearShape()
  {
    if (hasShape()) {
      sep->removeChild(1);
    }
  }
};

//____________________________________________________________________
VP1RawDataHandleBase::VP1RawDataHandleBase( VP1RawDataCollBase* coll )
  : d(new Imp), m_coll(coll), m_visible(false)//,m_currentmaterial(0)
{

}

//____________________________________________________________________
VP1RawDataHandleBase::~VP1RawDataHandleBase()
{
  if (m_visible)
    d->ensureDetach3DObjects(this);
  d->clearShape();
  if (d->sep)
    d->sep->unref();
  delete d;
}

//____________________________________________________________________
void VP1RawDataHandleBase::setVisible(bool vis)
{
  if (vis==m_visible)
    return;
  m_visible=vis;
  if (vis) {
    if (!d->hasShape())
      d->rebuild3DObjects(this);
    d->ensureAttach3DObjects(this);
  } else {
    d->ensureDetach3DObjects(this);
  }
}

//____________________________________________________________________
void VP1RawDataHandleBase::update3DObjects()
{
  if (m_visible) {
    d->rebuild3DObjects(this);
  } else {
    //Simply clear the present 3D objects. They will only be recreated if/when the handle becomes visible again.
    d->clearShape();
  }
}

//____________________________________________________________________
void VP1RawDataHandleBase::Imp::ensureInitSepAndTransform(VP1RawDataHandleBase*theclass)
{
  if (sep)
    return;
  sep = new SoSeparator;
  sep->ref();
  SoTransform * transform = theclass->buildTransform();
  sep->addChild(transform);
  theclass->common()->registerTransformAndHandle(transform,theclass);

}

//____________________________________________________________________
void VP1RawDataHandleBase::Imp::rebuild3DObjects(VP1RawDataHandleBase*theclass)
{
  if (!sep) {
    if (!theclass->m_visible)
      return;
    ensureInitSepAndTransform(theclass);
    ensureAttach3DObjects(theclass);
  }

  bool save(false);
  if (theclass->m_visible)
    save = sep->enableNotify(false);

  clearShape();

  SoNode * shape = theclass->buildShape();
  sep->addChild(shape);

  theclass->updateShownOutlines();

  if (save) {
    sep->enableNotify(true);
    sep->touch();
  }
}

//____________________________________________________________________
void VP1RawDataHandleBase::Imp::ensureAttach3DObjects(VP1RawDataHandleBase*theclass)
{
  if (isAttached)
    return;
  isAttached = true;

  VP1ExtraSepLayerHelper * sh(theclass->m_coll->sepHelper());
  if (sep && sh)
    sh->addNode(sep);
}

//____________________________________________________________________
void VP1RawDataHandleBase::Imp::ensureDetach3DObjects(VP1RawDataHandleBase*theclass)
{
  if (!isAttached)
    return;
  isAttached = false;

  VP1ExtraSepLayerHelper * sh(theclass->m_coll->sepHelper());
  if (sep && sh)
    sh->removeNode(sep);
}

//____________________________________________________________________
QString VP1RawDataHandleBase::unsignedToHex(unsigned i)
{
  std::ostringstream s;
  s << "0x" << std::hex << i;
  return QString(s.str().c_str()).toUpper();
}

//____________________________________________________________________
Amg::Vector3D VP1RawDataHandleBase::center()
{
  d->ensureInitSepAndTransform(this);
  assert(d->sep
	 && d->sep->getNumChildren()>0
	 && d->sep->getChild(0)->getTypeId().isDerivedFrom(SoTransform::getClassTypeId()));

  float x,y,z;
  static_cast<SoTransform*>(d->sep->getChild(0))->translation.getValue().getValue(x,y,z);
  //NB: We assume that the center part of the transform is zero
  return Amg::Vector3D(x,y,z);
}

//____________________________________________________________________
void VP1RawDataHandleBase::updateShownOutlines()
{
  if (!d->sep)
    return;
  common()->updateVolumeOutlines(d->sep);
}
