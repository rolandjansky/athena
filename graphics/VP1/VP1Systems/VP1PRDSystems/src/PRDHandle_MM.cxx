/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * update:
 *
 * - added MM and sTGC strip lenght from readout geometry
 *   06.05.2013 - Riccardo BIANCHI <rbianchi@cern.ch>
 *
 */


#include "VP1PRDSystems/PRDHandle_MM.h"
#include "VP1Utils/HitsSoNodeManager.h"
#include "VP1PRDSystems/MuonChamberProjectionHelper.h"
#include "VP1Base/VP1Msg.h"
#include "VP1Utils/VP1DetInfo.h"

#include <Inventor/nodes/SoSeparator.h>

#include "TrkSurfaces/CylinderBounds.h"
#include "TrkSurfaces/Surface.h"
#include "MuonReadoutGeometry/MuonStation.h"
#include "MuonReadoutGeometry/MuonChannelDesign.h"
#include "MuonReadoutGeometry/MMReadoutElement.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"

#include "GeoPrimitives/GeoPrimitives.h"


//____________________________________________________________________
PRDHandle_MM::PRDHandle_MM(PRDCollHandle_MM*collhandle, const Muon::MMPrepData*dc)
  : PRDHandleBase(static_cast<PRDCollHandleBase*>(collhandle)), m_prd(dc)
{
}

//____________________________________________________________________
void PRDHandle_MM::buildShapes(SoNode*&shape_simple, SoNode*&shape_detailed)
{
  // std::cout<<"m_prd->detectorElement()->surface(m_prd->identify())"<<m_prd->detectorElement()->surface(m_prd->identify())<<std::endl;
  //const Trk::CylinderBounds* ccbo = dynamic_cast<const Trk::CylinderBounds*>(&(m_prd->detectorElement()->surface(m_prd->identify()).bounds()));
  //assert(ccbo!=0);

  // const Trk::LocalPosition& localpos = m_prd->localPosition();
  

  const MmIdHelper * idhelper = VP1DetInfo::mmIDHelper();
  
  if (!idhelper) {
    VP1Msg::messageDebug("idhelper is null. Returning without building the shape...");
    return;
  }

  const Muon::MMPrepData * prd = mm();

  if (!prd) {
    VP1Msg::messageDebug("prd is null. Returning without building the shape...");
    return;
  }
  
  Identifier id = prd->identify();

  const MuonGM::MMReadoutElement* detEl = prd->detectorElement();

  VP1Msg::messageDebug("Building MM strip...");
  const MuonGM::MuonChannelDesign* design = detEl->getDesign( id );
  
  // using point shape for simple view
  shape_simple = common()->nodeManager()->getShapeNode_Strip(0.);
  
  // local position of the strip
  Amg::Vector2D locPos;
  if (!detEl->stripPosition(id,locPos) ) return;
  if (!design) return;
    
  double striplength=design->channelLength(idhelper->channel(id));
  double stripWidth=design->inputWidth;

  // use rectangular shape for detailed view
  SoSeparator * errDetailed  = new SoSeparator;
  errDetailed->addChild(common()->nodeManager()->getShapeNode_Strip(striplength,std::min(10.0,stripWidth),0.01));
  
  shape_detailed = errDetailed;

}


//____________________________________________________________________
int PRDHandle_MM::regionIndex()
{
  //Unique for each station.
  const MuonGM::MuonStation* station = m_prd->detectorElement()->parentMuonStation();
  if (!station) {VP1Msg::message("Warning - MM station gives null parent. Something is wrong with the geometry!"); return 0;}
  return station->getPhiIndex()-99999*station->getEtaIndex();//hopefully unique.
}

//____________________________________________________________________
QStringList PRDHandle_MM::clicked() const
{
    
    if (!m_prd)
      return QStringList("Null PRD");
    const MdtIdHelper * idhelper = VP1DetInfo::mdtIDHelper();
    if (!idhelper)
      return QStringList("Null PRD");

    std::ostringstream os;
    Identifier id = m_prd->identify();

    os << "MMPrepData with Identifier ["<<id.get_compact()  ;
    os << "] = [" << (idhelper->print_to_string(id)).c_str()  ;
    os << "]\n at global position = [" << m_prd->globalPosition()<<"], local position = ["<<m_prd->localPosition()<<"].";
    // os <<"ADC: "<<m_prd->adc();
    // os <<", TDC: "<<m_prd->tdc();
    // std::string status("Status: ");
    // os<<status;
        
    //QStringList l(QString(os.str().c_str())+driftCircleStatus());
    QStringList l(QString(os.str().c_str()));

    return l;
}

// //____________________________________________________________________
// QString PRDHandle_MM::driftCircleStatus() const
// {
//   QString status;
//   switch (m_prd->status()){
//     case  Muon::MdtStatusMasked:
//       status+="Masked"; 
//       break;
//     case Muon::MdtStatusDriftTime:
//       status+="Drift Time";
//       break;
//     case Muon::MdtStatusBeforeSpectrum:
//       status+="Before Spectrum";
//       break;    
//     case Muon::MdtStatusAfterSpectrum:
//       status+="After Spectrum";
//       break;
//     default:
//       status+=" Unknown with value = "+m_prd->status();
//   }
//   return status;
// }

