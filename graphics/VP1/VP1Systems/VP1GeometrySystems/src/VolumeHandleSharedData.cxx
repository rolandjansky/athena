/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1GeometrySystems/VolumeHandleSharedData.h"
#include "VP1GeometrySystems/ZappedVolumeListModel.h"
#include "VP1GeometrySystems/GeoSysController.h"
#include "VP1Utils/SoVisualizeAction.h"

#include "GeoModelKernel/GeoVPhysVol.h"
#include "GeoModelKernel/GeoShapeShift.h"

#include "VP1HEPVis/nodes/SoGenericBox.h"
#include "VP1HEPVis/nodes/SoTubs.h"
#include "VP1HEPVis/nodes/SoPcons.h"
#include "VP1HEPVis/nodes/SoTessellated.h"

#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoSeparator.h>

#include <map>

//____________________________________________________________________
class VolumeHandleSharedData::Imp {
public:
  GeoSysController * controller;
  std::map<const GeoLogVol *, SoShape *> logvol2shape;
  std::map<double, SoNode *> id2shape;
  SoVisualizeAction visaction;
  std::map<SoSeparator*,VolumeHandle*>* sonodesep2volhandle;
  GeoPVConstLink motherpV;
  PhiSectorManager* phisectormanager;
  VP1GeoFlags::SubSystemFlag subsysflag;
  SoMaterial * topMaterial;
  MatVisAttributes *matVisAttributes;
  VolVisAttributes *volVisAttributes;
  ZappedVolumeListModel * zappedvolumelistmodel;
  int ref;
};

//____________________________________________________________________
VolumeHandleSharedData::VolumeHandleSharedData(GeoSysController * controller,
					       VP1GeoFlags::SubSystemFlag flag,std::map<SoSeparator*,VolumeHandle*>* sonodesep2volhandle,
					       const GeoPVConstLink& motherpV,PhiSectorManager*psm, SoMaterial * topMaterial,
					       MatVisAttributes *matVisAttributes,VolVisAttributes *volVisAttributes,
					       ZappedVolumeListModel * zappedvolumelistmodel, VP1GeoTreeView * volbrowser, SoSeparator* textSep )
  : d(new Imp), m_volumebrowser(volbrowser), m_textSep(textSep)
{
  d->ref = 0;
  d->controller=controller;
  d->sonodesep2volhandle = sonodesep2volhandle;
  d->motherpV = motherpV;
  d->phisectormanager = psm;
  d->subsysflag = flag;
  d->topMaterial = topMaterial;
  d->matVisAttributes = matVisAttributes;
  d->volVisAttributes = volVisAttributes;
  d->zappedvolumelistmodel = zappedvolumelistmodel;
}

//____________________________________________________________________
VolumeHandleSharedData::~VolumeHandleSharedData()
{
  std::map<const GeoLogVol *, SoShape *>::iterator it, itE = d->logvol2shape.end();
  for (it=d->logvol2shape.begin();it!=itE;++it) {
    if (it->second)
      it->second->unref();
  }
  std::map<double, SoNode *>::iterator it2, it2E = d->id2shape.end();
  for (it2=d->id2shape.begin();it2!=it2E;++it2) {
    if (it2->second)
      it2->second->unref();
  }
  delete d; d=0;
}

//____________________________________________________________________
void VolumeHandleSharedData::ref()
{
  ++(d->ref);
}

//____________________________________________________________________
void VolumeHandleSharedData::unref()
{
  --(d->ref);
  if (!d->ref)
    delete this;
}

//____________________________________________________________________
MatVisAttributes * VolumeHandleSharedData::matVisAttributes() const
{
  return d->matVisAttributes;
}

//____________________________________________________________________
VolVisAttributes * VolumeHandleSharedData::volVisAttributes() const
{
  return d->volVisAttributes;
}

//____________________________________________________________________
SoMaterial * VolumeHandleSharedData::fallBackTopLevelMaterial() const
{
  return d->topMaterial;
}

//____________________________________________________________________
VP1GeoFlags::SubSystemFlag VolumeHandleSharedData::subSystemFlag() const
{
  return d->subsysflag;
}

//____________________________________________________________________
PhiSectorManager* VolumeHandleSharedData::phiSectorManager() const
{
  return d->phisectormanager;
}

//____________________________________________________________________
GeoSysController* VolumeHandleSharedData::controller() const
{
  return d->controller;
}

//_____________________________________________________________________________________
void VolumeHandleSharedData::registerNodeSepForVolumeHandle(SoSeparator*n,VolumeHandle*vh)
{
  assert(d->sonodesep2volhandle->find(n)==d->sonodesep2volhandle->end());
  (*(d->sonodesep2volhandle))[n]=vh;
  setShowVolumeOutlines(n,d->controller->showVolumeOutLines());
}

//_____________________________________________________________________________________
void VolumeHandleSharedData::setShowVolumeOutlines(SoGroup*nodegroup,bool showvol)
{
  for (int i = 0; i<nodegroup->getNumChildren();++i) {
    SoNode * n = nodegroup->getChild(i);
    if (n->getTypeId().isDerivedFrom(SoGenericBox::getClassTypeId())) {
      if (static_cast<SoGenericBox*>(n)->drawEdgeLines.getValue()!=showvol)
        static_cast<SoGenericBox*>(n)->drawEdgeLines.setValue(showvol);
    } else if (n->getTypeId().isDerivedFrom(SoTubs::getClassTypeId())) {
      if (static_cast<SoTubs*>(n)->drawEdgeLines.getValue()!=showvol){
	static_cast<SoTubs*>(n)->drawEdgeLines.setValue(showvol);
      } 
    } else if (n->getTypeId().isDerivedFrom(SoPcons::getClassTypeId())) {
      if (static_cast<SoPcons*>(n)->drawEdgeLines.getValue()!=showvol){
        static_cast<SoPcons*>(n)->drawEdgeLines.setValue(showvol);
      }
    } else if (n->getTypeId().isDerivedFrom(SoTessellated::getClassTypeId())) {
      if (static_cast<SoTessellated*>(n)->drawEdgeLines.getValue()!=showvol){
        static_cast<SoTessellated*>(n)->drawEdgeLines.setValue(showvol);
      }
    } else if (n->getTypeId().isDerivedFrom(SoGroup::getClassTypeId())) {
      setShowVolumeOutlines(static_cast<SoGroup*>(n),showvol);
    }
  }
}

//_____________________________________________________________________________________
SoNode * VolumeHandleSharedData::toShapeNode(const GeoPVConstLink& pV)
{
  const GeoLogVol * logVolume = pV->getLogVol();

  SoShape * shape (0);
  std::map<const GeoLogVol *, SoShape *>::iterator itShape = d->logvol2shape.find(logVolume);
  if (itShape!=d->logvol2shape.end()) {
    return itShape->second;
  }
  const GeoShape * geoshape = logVolume->getShape();
    
  d->visaction.reset();
  if (geoshape->typeID()==GeoShapeShift::getClassTypeID()) {
    dynamic_cast<const GeoShapeShift*>(geoshape)->getOp()->exec(&(d->visaction));
    //NB: the transformation part of the GeoShapeShift will be applied elsewhere
  } else {
    geoshape->exec(&(d->visaction));
  }
  shape = d->visaction.getShape();
  if (shape)
    shape->ref();
  d->logvol2shape[logVolume] = shape;
  return shape;
}

//____________________________________________________________________
SoNode * VolumeHandleSharedData::getSoCylinderOrientedLikeGeoTube(const double& radius, const double& halfLength)
{
  double id = radius - 9999.0*halfLength;
  std::map<double, SoNode *>::iterator it = d->id2shape.find(id);
  if (it!=d->id2shape.end())
    return it->second;

  SoGroup * group = new SoGroup;
  SoRotationXYZ * rot = new SoRotationXYZ;
  rot->axis.setValue(SoRotationXYZ::X);
  rot->angle.setValue(M_PI*0.5f);
  group->addChild(rot);
  SoCylinder * cyl = new SoCylinder;
  cyl->radius.setValue(radius);
  cyl->height.setValue(2.0*halfLength);
  group->addChild(cyl);
  group->ref();
  d->id2shape[id]=group;
  return group;
}

//____________________________________________________________________
GeoPVConstLink VolumeHandleSharedData::geoPVConstLinkOfTreeTopsMother() const
{
  return d->motherpV;
}

//____________________________________________________________________
void VolumeHandleSharedData::addZappedVolumeToGui( VolumeHandle * handle )
{
  d->zappedvolumelistmodel->addToZappedVolumes(handle);
}

//____________________________________________________________________
void VolumeHandleSharedData::removeZappedVolumesFromGui( VolumeHandle * handle )
{
  d->zappedvolumelistmodel->removeFromZappedVolumes(handle);
}
