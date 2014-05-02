/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoCutVolAction.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"

GeoCutVolAction::GeoCutVolAction(const GeoShape& Shape,
				 const HepGeom::Transform3D& Transform)
  :GeoVolumeAction (GeoVolumeAction::TOP_DOWN),
   _physVol(0),
   _shape(Shape),
   _transform(Transform),
   _copyStack(new std::stack<GeoPhysVol*>)
{
}

GeoCutVolAction::~GeoCutVolAction()
{
  while(_copyStack->size()>0)
    _copyStack->pop();
  delete _copyStack;
}

void GeoCutVolAction::handleVPhysVol(const GeoVPhysVol *pv)
{
  // Determine path length: 
  // 0  - for the starting PV
  // 1  - for the first level daughters etc.
  unsigned int pathLen = this->getState()->getPath()->getLength();

  // Get Logical Volume details
  const GeoLogVol* lvOriginal = pv->getLogVol();
  const std::string& lvNameOriginal = lvOriginal->getName();
  const GeoMaterial* lvMatOriginal = lvOriginal->getMaterial();
  const GeoShape* lvShapeOriginal = lvOriginal->getShape();

  if(pathLen==0)
  {
    // We are at the first PV. Create the resulting PV
    const GeoShape& cutShape = (*lvShapeOriginal).subtract(_shape << _transform);
    GeoLogVol* lvNew = new GeoLogVol(lvNameOriginal,&cutShape,lvMatOriginal);

    _physVol = new GeoPhysVol(lvNew);

    // Save the new PV in the copy stack
    _copyStack->push(_physVol);
  }
  else
  {
    // determine the parent PV in the copy tree
    while(_copyStack->size()>pathLen)
      _copyStack->pop();
    GeoPhysVol* copyParent = _copyStack->top();

    // Calculate cut transform to the reference frame of current PV
    HepGeom::Transform3D cutTransform = (this->getState()->getAbsoluteTransform()).inverse()*_transform;

    // Construct new PV
    const GeoShape& cutShape = (*lvShapeOriginal).subtract(_shape << cutTransform);
    GeoLogVol* lvNew = new GeoLogVol(lvNameOriginal,&cutShape,lvMatOriginal);
    GeoPhysVol* pvNew = new GeoPhysVol(lvNew);

    // Get its characteristics (name, id, transform to parent)
    std::string pvName = this->getState()->getName();
    if(pvName != "")
      copyParent->add(new GeoNameTag(pvName));

    const Query<int> pvId = this->getState()->getId();
    if(pvId.isValid())
      copyParent->add(new GeoIdentifierTag(pvId));

    copyParent->add(new GeoTransform(this->getState()->getTransform()));
    copyParent->add(pvNew);

    // Save new PV in the copy stack
    _copyStack->push(pvNew);
  }
}

GeoPhysVol* GeoCutVolAction::getPV()
{
  return _physVol;
}
