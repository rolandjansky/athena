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
  : GeoVolumeAction (GeoVolumeAction::TOP_DOWN)
  , m_physVol(0)
  , m_shape(Shape)
  , m_transform(Transform)
  , m_copyStack(new std::stack<GeoPhysVol*>)
{
}

GeoCutVolAction::~GeoCutVolAction()
{
  while(m_copyStack->size()>0)
    m_copyStack->pop();
  delete m_copyStack;
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
    const GeoShape& cutShape = (*lvShapeOriginal).subtract(m_shape << m_transform);
    GeoLogVol* lvNew = new GeoLogVol(lvNameOriginal,&cutShape,lvMatOriginal);

    m_physVol = new GeoPhysVol(lvNew);

    // Save the new PV in the copy stack
    m_copyStack->push(m_physVol);
  }
  else
  {
    // determine the parent PV in the copy tree
    while(m_copyStack->size()>pathLen)
      m_copyStack->pop();
    GeoPhysVol* copyParent = m_copyStack->top();

    // Calculate cut transform to the reference frame of current PV
    HepGeom::Transform3D cutTransform = (this->getState()->getAbsoluteTransform()).inverse()*m_transform;

    // Construct new PV
    const GeoShape& cutShape = (*lvShapeOriginal).subtract(m_shape << cutTransform);
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
    m_copyStack->push(pvNew);
  }
}

GeoPhysVol* GeoCutVolAction::getPV()
{
  return m_physVol;
}
