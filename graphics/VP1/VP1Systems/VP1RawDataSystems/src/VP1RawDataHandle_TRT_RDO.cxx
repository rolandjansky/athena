/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1RawDataHandle_TRT_RDO          //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: April 2008 (rewritten January 2009)      //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1RawDataSystems/VP1RawDataHandle_TRT_RDO.h"
#include "VP1RawDataSystems/VP1RawDataColl_TRT_RDO.h"
#include "VP1RawDataSystems/VP1RawDataCommonData.h"
#include "VP1RawDataSystems/VP1RawDataSysController.h"
#include "VP1Utils/HitsSoNodeManager.h"
#include "VP1Utils/VP1DetInfo.h"
#include "VP1Utils/VP1LinAlgUtils.h"

#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoMaterial.h>

#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "InDetReadoutGeometry/TRT_BaseElement.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetRawData/TRT_LoLumRawData.h"

#include "GaudiKernel/SystemOfUnits.h"

//____________________________________________________________________
VP1RawDataHandle_TRT_RDO::VP1RawDataHandle_TRT_RDO(VP1RawDataCollBase* coll,const TRT_LoLumRawData*data)
  : VP1RawDataHandleBase(coll), m_data(data)
{
}

//____________________________________________________________________
VP1RawDataHandle_TRT_RDO::~VP1RawDataHandle_TRT_RDO()
{
}

//____________________________________________________________________
QStringList VP1RawDataHandle_TRT_RDO::clicked(bool verbose) const
{
  static const TRT_ID * idhelper = VP1DetInfo::trtIDHelper();
  QStringList l;
  l << " ===> TRT_LoLumRawData";
  l << "   Data word: "+unsignedToHex(m_data->getWord());
  if (verbose) {
    if (idhelper) {
      //Fixme: We should have common identify -> QStringList method in VP1DetInfo!!
      Identifier id(m_data->identify());
      int barrel_ec = idhelper->barrel_ec(id);
      bool barrel(barrel_ec==1||barrel_ec==-1);
      l << QString(barrel?"Barrel":"End Cap")+" "+QString(barrel_ec>0?"A":"C");
      l << "   Phi module: "+QString::number(idhelper->phi_module(id));
      l << "   "+QString(barrel?"Layer":"Wheel")+": "+QString::number(idhelper->layer_or_wheel(id));
      l << "   Straw Layer: "+QString::number(idhelper->straw_layer(id));
      l << "   Straw: "+QString::number(idhelper->straw(id));
    }
    l << "  High Level: "+QString(m_data->highLevel()?"Yes":"No");
    l << "  Time over Threshold (Gaudi::Units::ns): "+QString::number(m_data->timeOverThreshold());
    //   //Fixme:
    //   virtual bool highLevel(int /* BX */) const;
    //   virtual int driftTimeBin() const;   // Position of first leading edge
    //   virtual int trailingEdge() const;   // Position of last trailing edge
    //   virtual bool firstBinHigh() const;  // True if first time bin is high
    //   virtual bool lastBinHigh() const;   // True if last time bin is high
  }
  return l;
}

//____________________________________________________________________
SoNode * VP1RawDataHandle_TRT_RDO::buildShape()
{
  const InDetDD::TRT_BaseElement * elem = element();
  double strawlength = elem ? elem->strawLength() : 200.0;

  SoNode * node = common()->nodeManager()->getShapeNode_DriftTube(0.5*strawlength,0.0/*0 radius for line*/);
  if (highThreshold() && static_cast<VP1RawDataColl_TRT_RDO*>(coll())->useSpecialHTMat()) {
    SoGroup * gr = new SoGroup;
    gr->addChild(coll()->common()->controller()->trtHTMaterial());
    gr->addChild(node);
    return gr;
  } else {
    return node;
  }
}

//____________________________________________________________________
SoTransform * VP1RawDataHandle_TRT_RDO::buildTransform()
{
  const InDetDD::TRT_BaseElement * elem = element();
  const int istraw = strawID();
  if (istraw<0||!elem)
    return new SoTransform;//fixme
  return VP1LinAlgUtils::toSoTransform(elem->getAbsoluteTransform(istraw));
}

//____________________________________________________________________
int VP1RawDataHandle_TRT_RDO::strawID() const
{
  const TRT_ID * idhelper = VP1DetInfo::trtIDHelper();
  if (!idhelper)
    return -1;
  const int s = idhelper->straw(m_data->identify());
  return (s <= 28 && s >= 0) ? s : -1;
}

//____________________________________________________________________
const InDetDD::TRT_BaseElement * VP1RawDataHandle_TRT_RDO::element() const
{
  const InDetDD::TRT_DetectorManager * detmgr = VP1DetInfo::trtDetMgr();
  if (!detmgr)
    return 0;
  return detmgr->getElement(m_data->identify());
}

//____________________________________________________________________
VP1RawDataFlags::InDetPartsFlags VP1RawDataHandle_TRT_RDO::inInDetParts() const
{
  const TRT_ID * idhelper = VP1DetInfo::trtIDHelper();
  if (!idhelper)
    return VP1RawDataFlags::All;
  switch (idhelper->barrel_ec(m_data->identify())) {
  case -2:
    return VP1RawDataFlags::EndCapNegative;
  case 2:
    return VP1RawDataFlags::EndCapPositive;
  case -1:
    return VP1RawDataFlags::BarrelNegative;
  case 1:
    return VP1RawDataFlags::BarrelPositive;
  default:
    return VP1RawDataFlags::All;
  }
}

//____________________________________________________________________
bool VP1RawDataHandle_TRT_RDO::highThreshold() const
{
  return m_data->highLevel();
}

//____________________________________________________________________
double VP1RawDataHandle_TRT_RDO::timeOverThreshold() const
{
  return m_data->timeOverThreshold()*Gaudi::Units::nanosecond;//According to the comments, returned value is in Gaudi::Units::ns.
}
