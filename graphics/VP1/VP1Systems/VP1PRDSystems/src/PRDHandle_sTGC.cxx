/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1PRDSystems/PRDHandle_sTGC.h"
#include "VP1Utils/HitsSoNodeManager.h"
#include "VP1Utils/VP1DetInfo.h"
#include "VP1Base/VP1Msg.h"
#include "VP1Utils/VP1LinAlgUtils.h"
#include "VP1HEPVis/nodes/SoTransparency.h"

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoTransform.h>

#include "MuonReadoutGeometry/MuonStation.h"

#include "GeoPrimitives/GeoPrimitives.h"

//____________________________________________________________________
PRDHandle_sTGC::PRDHandle_sTGC(PRDCollHandle_sTGC*collhandle, const Muon::sTgcPrepData*stgc)
  : PRDHandleBase(static_cast<PRDCollHandleBase*>(collhandle)), m_stgc(stgc)
{
  SoTransparency::initClass();
}

// fixme: uncomment and update
////____________________________________________________________________
SoTransform * PRDHandle_sTGC::createTransform() const
{
  std::cout<<"creating sTGC transform:"<< std::endl;

  const sTgcIdHelper * idhelper = VP1DetInfo::stgcIDHelper();
  const Muon::sTgcPrepData * prd = stgc();
  if (!prd || !idhelper) return new SoTransform;

  Identifier id = prd->identify();

  const MuonGM::sTgcReadoutElement* detEl = prd->detectorElement();
  if (!detEl)  return new SoTransform;

  const Amg::Transform3D& transf = detEl->transform(id);

  SoTransform * theHitTransform = VP1LinAlgUtils::toSoTransform(transf);
  
  // return the channel position
  Amg::Vector2D locPos;

  if (!detEl->stripPosition(id,locPos) ) {
    return theHitTransform;
  }

  theHitTransform = VP1LinAlgUtils::toSoTransform(transf*Amg::Translation3D(locPos.x(),locPos.y(),0.));
    
  return theHitTransform;
} 


//____________________________________________________________________
void PRDHandle_sTGC::buildShapes(SoNode*&shape_simple, SoNode*&shape_detailed)
{

  VP1Msg::messageDebug("buildShapes()");

  const sTgcIdHelper * idhelper = VP1DetInfo::stgcIDHelper();
  
  if (!idhelper) {
    VP1Msg::messageDebug("idhelper is null. Returning without building the shape...");
    return;
  }
  const Muon::sTgcPrepData * prd = stgc();

  if (!prd) {
    VP1Msg::messageDebug("prd is null. Returning without building the shape...");
    return;
  }
  
  Identifier id = prd->identify();

  const MuonGM::sTgcReadoutElement* detEl = prd->detectorElement();

  if (idhelper->channelType(id)==0) {     // pads
    VP1Msg::messageDebug("Building sTgc pad...");
    const MuonGM::MuonPadDesign* pad_design = detEl->getPadDesign( id );
    
    // use point shape for simple view
//    shape_simple = common()->nodeManager()->getShapeNode_Strip(0.); // FIXME: ORIGINAL FROM SHARKA. Works perfectly but the STGC elements are point-like until very very very close...
    
    // local position of the pad 
    Amg::Vector2D locPad;
    if (!detEl->stripPosition(id,locPad) ) return;
    if (!pad_design) return;
    
    double etaWidth=pad_design->inputRowWidth;
    double phiMinWidth=pad_design->channelWidth(Amg::Vector2D(locPad.x(),locPad.y()-0.5*etaWidth),true);
    double phiMaxWidth=pad_design->channelWidth(Amg::Vector2D(locPad.x(),locPad.y()+0.5*etaWidth),true);
    
    shape_simple = common()->nodeManager()->getShapeNode_Pad(etaWidth,phiMinWidth,phiMaxWidth,0.01); // fixme: now shape_simple == shape_detailed

    // use trapezoid shape for detailed view
    SoSeparator * errDetailed  = new SoSeparator;
    errDetailed->addChild(common()->nodeManager()->getShapeNode_Pad(etaWidth,phiMinWidth,phiMaxWidth,0.01));
    
    shape_detailed = errDetailed;

    return;
  } else if (idhelper->channelType(id)==2) {     // wire gangs
    VP1Msg::messageDebug("Building sTgc wire gang...");
    const MuonGM::MuonChannelDesign* design = detEl->getDesign( id );
    
    // use point shape for simple view
//    shape_simple = common()->nodeManager()->getShapeNode_Strip(0.); // FIXME: ORIGINAL FROM SHARKA. Works perfectly but the STGC elements are point-like until very very very close...

    // local position of the wire gang 
    Amg::Vector2D locWire;
    if (!detEl->stripPosition(id,locWire) ) return;
    if (!design) return;
    
    double etaWidth=design->channelLength(idhelper->channel(id));
    double phiMaxWidth=design->maxYSize/design->nch;
    double phiMinWidth=design->minYSize/design->nch;
    

    shape_simple = common()->nodeManager()->getShapeNode_Wire(etaWidth,phiMinWidth,phiMaxWidth,0.01); // fixme: now shape_simple == shape_detailed

    // use trapezoid shape for detailed view
    SoSeparator * errDetailed  = new SoSeparator;
    errDetailed->addChild(common()->nodeManager()->getShapeNode_Wire(etaWidth,phiMinWidth,phiMaxWidth,0.01));
   
    shape_detailed = errDetailed;
    
    return;
  } else if (idhelper->channelType(id)==1) {     // strips
    VP1Msg::messageDebug("Building sTgc strip...");
    const MuonGM::MuonChannelDesign* design = detEl->getDesign( id );
    
    // use point shape for simple view
//    shape_simple = common()->nodeManager()->getShapeNode_Strip(0.); // FIXME: ORIGINAL FROM SHARKA. Works perfectly but the STGC elements are point-like until very very very close...

    // local position of the wire gang 
    Amg::Vector2D locPos;
    if (!detEl->stripPosition(id,locPos) ) return;
    if (!design) return;
    
    double striplength=design->channelLength(idhelper->channel(id));
    double stripWidth=design->inputWidth;

    shape_simple = common()->nodeManager()->getShapeNode_Strip(striplength,std::min(10.0,stripWidth),0.01); // fixme: now shape_simple == shape_detailed

    // use rectangular shape for detailed view
    SoSeparator * errDetailed  = new SoSeparator;
    errDetailed->addChild(common()->nodeManager()->getShapeNode_Strip(striplength,std::min(10.0,stripWidth),0.01));

    shape_detailed = errDetailed;
    
    return;
  }

 return; 

 }

//____________________________________________________________________
  int PRDHandle_sTGC::regionIndex()
  {
  //Unique for each station.
    const MuonGM::MuonStation* station = m_stgc->detectorElement()->parentMuonStation();
    if (!station) {VP1Msg::message("Warning - sTGC station gives null parent. Something is wrong with the geometry!"); return 0;}
    
    return station->getPhiIndex()-99999*station->getEtaIndex();//hopefully unique.
  }
