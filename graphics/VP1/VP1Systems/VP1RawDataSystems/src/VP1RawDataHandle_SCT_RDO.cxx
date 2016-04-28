/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1RawDataHandle_SCT_RDO          //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: April 2008 (rewritten January 2009)      //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1RawDataSystems/VP1RawDataHandle_SCT_RDO.h"
#include "VP1RawDataSystems/VP1RawDataCommonData.h"
#include "VP1Utils/HitsSoNodeManager.h"
#include "VP1Utils/VP1DetInfo.h"
#include "VP1Utils/VP1LinAlgUtils.h"

#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoVertexProperty.h>

#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

//#include "InDetReadoutGeometry/InDetReadoutGeometry/SCT_ModuleSideDesign.h" // old CMT
#include "InDetReadoutGeometry/SCT_ModuleSideDesign.h"   // new CMake

#include "InDetRawData/SCT_RDORawData.h"
#include "InDetIdentifier/SCT_ID.h"

#include "GeoPrimitives/GeoPrimitives.h"


//____________________________________________________________________
VP1RawDataHandle_SCT_RDO::VP1RawDataHandle_SCT_RDO(VP1RawDataCollBase* coll,const SCT_RDORawData*data)
  : VP1RawDataHandleBase(coll), m_data(data)
{
}

//____________________________________________________________________
VP1RawDataHandle_SCT_RDO::~VP1RawDataHandle_SCT_RDO()
{
}

//____________________________________________________________________
QStringList VP1RawDataHandle_SCT_RDO::clicked(bool verbose) const
{
  static const SCT_ID * idhelper = VP1DetInfo::sctIDHelper();
  QStringList l;
  l << " ===> SCT_RDORawData";
  l << "   Data word: "+unsignedToHex(m_data->getWord());
  if (verbose) {
    if (idhelper) {
      Identifier id(m_data->identify());
      bool barrel(false);
      if (idhelper->barrel_ec(id)==-2) l << "   Part: End Cap C";
      else if (idhelper->barrel_ec(id)==2) l << "   Part: End Cap A";
      else { l << "   Part: Barrel"; barrel = true; }
      l << "   "+QString(barrel?"Layer":"Disk")+": "+QString::number(idhelper->layer_disk(id));
      l << "   Phi module: "+QString::number(idhelper->phi_module(id));
      l << "   Eta module: "+QString::number(idhelper->eta_module(id));
      l << "   Side: "+QString(idhelper->side(id)==0 ? "Inner":"Outer");
      l << "   Group Size: "+QString::number(m_data->getGroupSize());
      if (m_data->getGroupSize()>1)
        l << "  Strips: "+QString::number(idhelper->strip(id))+".."+QString::number(idhelper->strip(id)+m_data->getGroupSize()-1);
      else
        l << "  Strip: "+QString::number(idhelper->strip(id));
    }
    //Fixme: Try dynamic cast to SCT3_RawData, and print more information if it succeeds.
  }
  return l;
}

//____________________________________________________________________
SoNode * VP1RawDataHandle_SCT_RDO::buildShape()
{
  static const SCT_ID * idhelper = VP1DetInfo::sctIDHelper();
  const InDetDD::SiDetectorElement * elem = element();
  const int ngroup = m_data->getGroupSize();
  if (!elem||ngroup<1)
    return common()->nodeManager()->getShapeNode_Point();//fixme: warn

  const InDetDD::SCT_ModuleSideDesign& design = dynamic_cast<const InDetDD::SCT_ModuleSideDesign&>(elem->design());

  SoLineSet * line = new SoLineSet;
  SoVertexProperty * vertices = new SoVertexProperty;
  line->vertexProperty = vertices;

  Amg::Transform3D invTransform(elem->transform().inverse());
  const int ifirststrip(idhelper ? idhelper->strip(m_data->identify()) : 0);
  int iver(0);
  std::pair<InDetDD::SiLocalPosition,InDetDD::SiLocalPosition> localEnds;
  for (int i=0;i<ngroup;++i) {
    std::pair< Amg::Vector3D, Amg::Vector3D > globalEnds = elem->endsOfStrip(design.positionFromStrip(ifirststrip+i));
    Amg::Vector3D localA(invTransform*globalEnds.first);
    Amg::Vector3D localB(invTransform*globalEnds.second);
    localEnds = design.endsOfStrip(design.positionFromStrip(ifirststrip+i));
    vertices->vertex.set1Value(iver++,localA.x(),localA.y(),0);
    vertices->vertex.set1Value(iver++,localB.x(),localB.y(),0);
    line->numVertices.set1Value(i,2);
  }
  return line;
}

//____________________________________________________________________
SoTransform * VP1RawDataHandle_SCT_RDO::buildTransform()
{
  const InDetDD::SiDetectorElement * elem = element();
  if (!elem)
    return common()->nodeManager()->getUnitTransform();//fixme: warn
  return VP1LinAlgUtils::toSoTransform(elem->transform());
}

//____________________________________________________________________
const InDetDD::SiDetectorElement * VP1RawDataHandle_SCT_RDO::element() const
{
  const InDetDD::SCT_DetectorManager * detmgr = VP1DetInfo::sctDetMgr();
  if (!detmgr)
    return 0;
  return detmgr->getDetectorElement(m_data->identify());
}

// //____________________________________________________________________
// SoMaterial * VP1RawDataHandle_SCT_RDO::determineMaterial() const
// {
//   return common()->sctMaterial();
// }

//____________________________________________________________________
VP1RawDataFlags::InDetPartsFlags VP1RawDataHandle_SCT_RDO::inInDetParts() const
{
  const InDetDD::SiDetectorElement * elem = element();
  if (!elem)
    return VP1RawDataFlags::All;
  if (elem->center().z()>=0)
    return elem->isBarrel() ? VP1RawDataFlags::BarrelPositive : VP1RawDataFlags::EndCapPositive;
  else
    return elem->isBarrel() ? VP1RawDataFlags::BarrelNegative : VP1RawDataFlags::EndCapNegative;

}
