/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1RawDataHandle_PixelRDO         //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: April 2008 (rewritten January 2009)      //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1RawDataSystems/VP1RawDataHandle_PixelRDO.h"
#include "VP1RawDataSystems/VP1RawDataCommonData.h"
#include "VP1Utils/HitsSoNodeManager.h"
#include "VP1Utils/VP1DetInfo.h"
#include "VP1Utils/VP1LinAlgUtils.h"

#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetRawData/PixelRDORawData.h"
#include "InDetIdentifier/PixelID.h"

#include "GeoPrimitives/GeoPrimitives.h"


//____________________________________________________________________
VP1RawDataHandle_PixelRDO::VP1RawDataHandle_PixelRDO(VP1RawDataCollBase* coll,const PixelRDORawData*data)
  : VP1RawDataHandleBase(coll), m_data(data)
{
}

//____________________________________________________________________
VP1RawDataHandle_PixelRDO::~VP1RawDataHandle_PixelRDO()
{
}

//____________________________________________________________________
QStringList VP1RawDataHandle_PixelRDO::clicked(bool verbose) const
{
  static const PixelID * idhelper = VP1DetInfo::pixelIDHelper();
  QStringList l;
  l << " ===> PixelRDORawData";
  l << "   Data word: "+unsignedToHex(m_data->getWord());
  if (verbose) {
    if (idhelper) {
    //Fixme: We should have common identify -> QStringList method in VP1DetInfo!!
      Identifier id(m_data->identify());
      const bool barrel = idhelper->is_barrel(id);
      l << "   Barrel: "+QString(barrel?"Yes":"No");
      l << "   B-Layer: "+QString(idhelper->is_blayer(id)?"Yes":"No");
      l << "   "+QString(barrel?"Layer":"Disk")+": "+QString::number(idhelper->layer_disk(id));
      l << "   Phi module: "+QString::number(idhelper->phi_module(id));
      l << "   Eta module: "+QString::number(idhelper->eta_module(id));
      l << "   Phi index: "+QString::number(idhelper->phi_index(id));
      l << "   Eta index: "+QString::number(idhelper->eta_index(id));
    }
    l << "  Time over Threshold (0..255): "+QString::number(m_data->getToT());
    l << "  Beam Crossing ID: "+QString::number(m_data->getBCID());
    l << "  Level 1 accept (0..15): "+QString::number(m_data->getLVL1A());
    l << "  ATLAS LVL1 ID (0-255): "+QString::number(m_data->getLVL1ID());
  }
  return l;
}

//____________________________________________________________________
SoNode * VP1RawDataHandle_PixelRDO::buildShape()
{
  return common()->nodeManager()->getShapeNode_Point();
}

//____________________________________________________________________
SoTransform * VP1RawDataHandle_PixelRDO::buildTransform()
{
  //Fixme: Do we need more?
  const InDetDD::SiDetectorElement * elem = element();
  if (!elem)
    return common()->nodeManager()->getUnitTransform();//fixme: warn
  Amg::Vector2D localpos = elem->rawLocalPositionOfCell(m_data->identify());//Fixme:raw or lorentz corrected?? Make it option?

//  return VP1LinAlgUtils::toSoTransform(elem->transform() * HepGeom::Translate3D(localpos.x(),localpos.y(),0));
  Amg::Transform3D transf = elem->transform();
  transf = transf * Amg::Translation3D(localpos.x(),localpos.y(),0);
  return VP1LinAlgUtils::toSoTransform( transf );
}

//____________________________________________________________________
const InDetDD::SiDetectorElement * VP1RawDataHandle_PixelRDO::element() const
{
  const InDetDD::PixelDetectorManager * detmgr = VP1DetInfo::pixelDetMgr();
  if (!detmgr)
    return 0;
  return detmgr->getDetectorElement(m_data->identify());
}

// //____________________________________________________________________
// SoMaterial * VP1RawDataHandle_PixelRDO::determineMaterial() const
// {
//   return common()->pixelMaterial();
// }

//____________________________________________________________________
VP1RawDataFlags::InDetPartsFlags VP1RawDataHandle_PixelRDO::inInDetParts() const
{
  const InDetDD::SiDetectorElement * elem = element();
  if (!elem)
    return VP1RawDataFlags::All;
  if (elem->center().z()>=0)
    return elem->isBarrel() ? VP1RawDataFlags::BarrelPositive : VP1RawDataFlags::EndCapPositive;
  else
    return elem->isBarrel() ? VP1RawDataFlags::BarrelNegative : VP1RawDataFlags::EndCapNegative;

}
