/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TRTTimeCorrection.h"

//TRT detector information:
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "InDetReadoutGeometry/TRT_Numerology.h"

#include "TRTDigSettings.h"

#include "GeoPrimitives/GeoPrimitives.h"
//For speed of light:
#include "CLHEP/Units/PhysicalConstants.h"

#include "Identifier/Identifier.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "GaudiKernel/IService.h"

#include "TRT_ConditionsServices/ITRT_CalDbSvc.h"
//__________________________________________________________________________________________________________
TRTTimeCorrection::TRTTimeCorrection(const std::string& name,
				     const TRTDigSettings* digset,
				     const InDetDD::TRT_DetectorManager* detmgr,
				     const TRT_ID* trt_id)
  : m_settings(digset), m_detmgr(detmgr), m_trt_id(trt_id),
    m_subdetectorMask(0x00200000), m_right5Bits(0x0000001F),
    m_shift5Bits(5), m_shift10Bits(10), m_shift15Bits(15), m_notInitVal(-999999.0), m_trtcaldbsvc("TRT_CalDbSvc",name), m_msg("TRTTimeCorrection")
{
  Initialize();
}


//__________________________________________________________________________________________________________
TRTTimeCorrection::~TRTTimeCorrection() {}

//__________________________________________________________________________________________________________
void TRTTimeCorrection::Initialize() {

  m_signalPropagationSpeed = m_settings->signalPropagationSpeed() ;
  m_lengthDeadRegion = m_settings->lengthOfDeadRegion();
  m_maxVertexDisplacement = m_settings->maxVertexDisplacement();
  m_timeShiftPhiSectSymmetry = m_settings->timeshiftsSymmetricForPhiSectors();
  m_getT0FromData = m_settings->getT0FromData();

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "TRTTimeCorrection::Initialize()" << endmsg;

  if ( (m_getT0FromData) && (m_trtcaldbsvc.retrieve().isFailure()) ) {
    if (msgLevel(MSG::ERROR)) msg(MSG::ERROR) << "Could not find TRT_CalDbSvc => cannot use t0 of data." << endmsg;
    m_getT0FromData=false;
  }

  const InDetDD::TRT_Numerology *numerology(m_detmgr->getNumerology());

  const unsigned int nbarrelphi(m_timeShiftPhiSectSymmetry ? 1 : numerology->getNBarrelPhi());
  const unsigned int nendcapphi(m_timeShiftPhiSectSymmetry ? 1 : numerology->getNEndcapPhi());

  //Initialize barrel max timeshift arrays:
  m_timeShiftForBarrelStraws.resize(nbarrelphi);
  for (unsigned int iPhi = 0; iPhi < m_timeShiftForBarrelStraws.size(); ++iPhi) {
    m_timeShiftForBarrelStraws[iPhi].resize(numerology->getNBarrelRings());
    for (unsigned int iRing = 0; iRing < m_timeShiftForBarrelStraws[iPhi].size(); ++iRing) {
      m_timeShiftForBarrelStraws[iPhi][iRing].resize(numerology->getNBarrelLayers(iRing));
      for (unsigned int iLayer = 0; iLayer < m_timeShiftForBarrelStraws[iPhi][iRing].size(); ++iLayer) {
	if (m_detmgr->getBarrelElement(0,iRing,0,iLayer)) {
	  unsigned int nstraws_in_layer = m_detmgr->getBarrelElement(0,iRing,0,iLayer)->nStraws();
	  m_timeShiftForBarrelStraws[iPhi][iRing][iLayer].assign(nstraws_in_layer,m_notInitVal);
	}
      }
    }
  }

  //Initialize endcap max timeshift arrays:
  m_timeShiftForEndCapPlanes.resize(nendcapphi);
  for (unsigned int iPhi = 0; iPhi < m_timeShiftForEndCapPlanes.size(); ++iPhi) {
    m_timeShiftForEndCapPlanes[iPhi].resize(numerology->getNEndcapWheels());
    for (unsigned int iWheel = 0; iWheel < m_timeShiftForEndCapPlanes[iPhi].size(); ++iWheel) {
      m_timeShiftForEndCapPlanes[iPhi][iWheel].assign(numerology->getNEndcapLayers(iWheel),m_notInitVal);
    }
  }

  //Initialize barrel direct/reflected distances array
  m_directDistsForBarrelLayers.resize(numerology->getNBarrelRings());
  m_reflectedDistsForBarrelLayers.resize(numerology->getNBarrelRings());
  for (unsigned int iRing = 0; iRing < m_directDistsForBarrelLayers.size(); ++iRing) {
    m_directDistsForBarrelLayers[iRing].assign(numerology->getNBarrelLayers(iRing),m_notInitVal);
    m_reflectedDistsForBarrelLayers[iRing].assign(numerology->getNBarrelLayers(iRing),m_notInitVal);
  };

  //Initialize endcap direct/reflected distances array
  m_directDistsForEndCapWheels.assign(numerology->getNEndcapWheels(),m_notInitVal);
  m_reflectedDistsForEndCapWheels.assign(numerology->getNEndcapWheels(),m_notInitVal);

}

//__________________________________________________________________________________________________________
double TRTTimeCorrection::TimeShift(const int& strawID) {

  //TODO: Use hit id helpers (but resolve efficiency issues first).

  double timeshift=0.;

  //Layer and phi index are needed for both endcap and barrel:
  const unsigned int iLayer((strawID >> m_shift5Bits) & m_right5Bits);
  const unsigned int iPhi(m_timeShiftPhiSectSymmetry ? 0 : ( (strawID >> m_shift10Bits) & m_right5Bits ));

  if (strawID & m_subdetectorMask) {

    //===// EndCap //===//

    const unsigned int iWheel((strawID >> m_shift15Bits) & m_right5Bits);

    //Sanity check:
    if (iPhi>=m_timeShiftForEndCapPlanes.size()||
	iWheel>=m_timeShiftForEndCapPlanes[iPhi].size()||
	iLayer>=m_timeShiftForEndCapPlanes[iPhi][iWheel].size()) {
      if (m_timeShiftPhiSectSymmetry) {
	if (msgLevel(MSG::ERROR)) msg(MSG::ERROR) << "TimeCorrection::TimeShift: (iWheel,iLayer) = ("
						  << iWheel << ", " << iLayer << ") out of bounds! Returning 0." << endmsg;
      } else {
	if (msgLevel(MSG::ERROR)) msg(MSG::ERROR) << "TimeCorrection::TimeShift: (iPhi,iWheel,iLayer) = ("
						  << iPhi << ", " << iWheel << ", " << iLayer << ") out of bounds! Returning 0." << endmsg;
      }
      return 0.0;
    }

    timeshift = m_timeShiftForEndCapPlanes[iPhi][iWheel][iLayer];

    if (timeshift==m_notInitVal) {
      //We need to initialize
      timeshift = calculateTimeShift_EndCap(iPhi,iWheel,iLayer,strawID);
      m_timeShiftForEndCapPlanes[iPhi][iWheel][iLayer] = timeshift;
    }

  } else {

    //===// Barrel //===//

    const unsigned int iRing((strawID >> m_shift15Bits) & m_right5Bits);
    const unsigned int iStraw(strawID & m_right5Bits);

    //Sanity check:
    if (iPhi>=m_timeShiftForBarrelStraws.size()||
	iRing>=m_timeShiftForBarrelStraws[iPhi].size()||
	iLayer>=m_timeShiftForBarrelStraws[iPhi][iRing].size()||
	iStraw>=m_timeShiftForBarrelStraws[iPhi][iRing][iLayer].size()) {
      if (m_timeShiftPhiSectSymmetry) {
	if (msgLevel(MSG::ERROR)) msg(MSG::ERROR) << "TimeCorrection::TimeShift: (iRing,iLayer,iStraw) = ("
						  << iRing << ", " << iLayer << ", " << iStraw
						  << ") out of bounds! Returning 0." << endmsg;
      } else {
	if (msgLevel(MSG::ERROR)) msg(MSG::ERROR) << "TimeCorrection::TimeShift: (iPhi,iRing,iLayer,iStraw) = ("
						  << iPhi << ", " << iRing << ", " << iLayer << ", " << iStraw
						  << ") out of bounds! Returning 0." << endmsg;
      }
      return 0.0;
    }

    timeshift = m_timeShiftForBarrelStraws[iPhi][iRing][iLayer][iStraw];

    if (timeshift==m_notInitVal) {
      //We need to initialize
      timeshift = calculateTimeShift_Barrel(iPhi,iRing,iLayer,iStraw,strawID);
	m_timeShiftForBarrelStraws[iPhi][iRing][iLayer][iStraw] = timeshift;
    }

  }

  return timeshift;
}

//__________________________________________________________________________________________________________
double TRTTimeCorrection::calculateTimeShift_Barrel( const unsigned int& iPhi,
						     const unsigned int& iRing,
						     const unsigned int& iLayer,
						     const unsigned int& iStraw,
						     const int strawID)  {

  const InDetDD::TRT_BarrelElement * barrel_element(m_detmgr->getBarrelElement(0/*positive*/,
									       iRing, iPhi, iLayer ));

  //Sanity checks:
  if (!barrel_element) {
    if (msgLevel(MSG::ERROR)) msg(MSG::ERROR)<< "calculateTimeShift_Barrel: Could not get element for iRing = "
					     << iRing <<" and iLayer = "<<iLayer<<". Timeshift becomes 0." <<endmsg;
    return 0.0;
  }

  if (iStraw >= barrel_element->nStraws()) {
     if (msgLevel(MSG::ERROR)) msg(MSG::ERROR) << "calculateTimeShift_Barrel: Trying to access iStraw "
					       << iStraw <<" in an element with "<<barrel_element->nStraws()<<" straws (iRing="
					       << iRing <<",iLayer="<<iLayer<<"). Timeshift becomes 0." <<endmsg;
    return 0.0;
  }

  //Straw endpoints for the straw lying along the z-axis:
  Amg::Vector3D strawend1(0,0,   barrel_element->strawLength()*0.5  );
  Amg::Vector3D strawend2(0,0, -(barrel_element->strawLength()*0.5) );

  //And here put into their proper global place:
  strawend1 = barrel_element->strawTransform(iStraw) * strawend1;
  strawend2 = barrel_element->strawTransform(iStraw) * strawend2;

  return calculateTimeShiftFromStrawEnds(strawend1,strawend2,strawID);

}

//__________________________________________________________________________________________________________
double TRTTimeCorrection::calculateTimeShift_EndCap( const unsigned int& iPhi,
						     const unsigned int& iWheel,
						     const unsigned int& iLayer,
						     const int strawID) {

  const InDetDD::TRT_EndcapElement * ec_element(m_detmgr->getEndcapElement(0/*positive*/,
									   iWheel, iLayer, iPhi ));

  //Sanity check:
  if (!ec_element) {
    if (msgLevel(MSG::ERROR)) msg(MSG::ERROR) << "calculateTimeShift_EndCap: Could not get element for iWheel = "
					      << iWheel <<" and iLayer = "<<iLayer<<". Timeshift becomes 0." <<endmsg;
    return 0.0;
  }

  //Straw endpoints for the straw lying along the z-axis:
  Amg::Vector3D strawend1(0,0, ec_element->strawLength() * 0.5 );
  Amg::Vector3D strawend2(0,0, ec_element->strawLength() * (-0.5) );

  //And here put into their proper global place:
  strawend1 = ec_element->strawTransform(0) * strawend1;
  strawend2 = ec_element->strawTransform(0) * strawend2;

  return calculateTimeShiftFromStrawEnds(strawend1,strawend2,strawID); //,m_lvl

}

//__________________________________________________________________________________________________________
double TRTTimeCorrection::calculateTimeShiftFromStrawEnds( const Amg::Vector3D& strawend1_globalcoord,
							   const Amg::Vector3D& strawend2_globalcoord,
							   const int strawID )  {

  //The two (hopefully relevant) extreme points of the vertex region:
  Amg::Vector3D vertexExtension1( m_settings->timeOffsetCalcVertexX(),
                                  m_settings->timeOffsetCalcVertexY(),
                                  m_settings->timeOffsetCalcVertexZ() + m_maxVertexDisplacement);
  Amg::Vector3D vertexExtension2( m_settings->timeOffsetCalcVertexX(),
                                  m_settings->timeOffsetCalcVertexY(),
                                  m_settings->timeOffsetCalcVertexZ() - m_maxVertexDisplacement);

  //Minimum distance between vertex region and the straw ends:
  // const double mindisttoend1(std::min(strawend1_globalcoord.distance(vertexExtension1),
  //       			      strawend1_globalcoord.distance(vertexExtension2)));
  const double mindisttoend1(std::min((strawend1_globalcoord-vertexExtension1).mag(),
                                      (strawend1_globalcoord-vertexExtension2).mag()));
  // const double mindisttoend2(std::min(strawend2_globalcoord.distance(vertexExtension1),
  //       			      strawend2_globalcoord.distance(vertexExtension2)));
  const double mindisttoend2(std::min((strawend2_globalcoord-vertexExtension1).mag(),
                                      (strawend2_globalcoord-vertexExtension2).mag()));

  //Just a sanity check here:
  if ( (mindisttoend2<mindisttoend1) == m_settings->electronicsAreAtFarEnd() ) {
    if (msgLevel(MSG::WARNING)) msg(MSG::WARNING)  << "It would seem that the local z-coordinate of a test straw grows TOWARDS"
						   <<" the electronics ends. This will give trouble elsewhere!!" << endmsg;
  }

  double shift = 1.0; // 1 ns (negative) overall shift for the whole TRT detector. Now set in stone.
                      // Used to be set with overallT0Shift() & overallT0ShiftShortBarrel()
                      // Note: if you change this then you need to set ToolSvc.InDetTRT_DriftFunctionTool.MCTuningShift

  if (m_settings->getT0FromData()) {
    bool identifierOK;
    const Identifier idStraw(getIdentifier(strawID, identifierOK));
    if (identifierOK) {
      shift = m_trtcaldbsvc->getT0(idStraw);
    } else {
      if (msgLevel(MSG::ERROR)) msg(MSG::ERROR)
         << "Attempt to use t0 from data failed: TRTCalDbSvc was not able to supply t0 for straw with identifier: "
         << idStraw << ". Please set getT0FromData=false in jobOptions and run again" << endmsg;
    }
  }

  if (m_settings->electronicsAreAtFarEnd())
    return std::max(mindisttoend1,mindisttoend2) / (m_settings->distanceToTimeFactor() * CLHEP::c_light) - shift;
  else
    return std::min(mindisttoend1,mindisttoend2) / (m_settings->distanceToTimeFactor() * CLHEP::c_light) - shift;

}

//__________________________________________________________________________________________________________
void TRTTimeCorrection::PropagationTime(const int& strawID, const double& meanZ,
					double& propagationTime1, double& propagationTime2) {

  double direct_distance, reflect_distance;

  if (strawID & m_subdetectorMask) {

    //===// EndCap //===//
    const unsigned int iWheel((strawID >> m_shift15Bits) & m_right5Bits);
    direct_distance  = m_directDistsForEndCapWheels[iWheel];
    reflect_distance = m_reflectedDistsForEndCapWheels[iWheel];

    if (direct_distance==m_notInitVal) {
      //We need to initialize
      calculateSignalDists_EndCap(iWheel,direct_distance,reflect_distance);
      m_directDistsForEndCapWheels[iWheel] = direct_distance;
      m_reflectedDistsForEndCapWheels[iWheel] = reflect_distance;
    };

    //Z is counted positive AWAY from the electronics:
    propagationTime1 = (direct_distance  + meanZ) / m_signalPropagationSpeed;
    propagationTime2 = (reflect_distance - meanZ) / m_signalPropagationSpeed;

  } else {

    //===// Barrel //===//
    const unsigned int iRing((strawID >> m_shift15Bits) & m_right5Bits);
    const unsigned int iLayer((strawID >> m_shift5Bits) & m_right5Bits);
    direct_distance  = m_directDistsForBarrelLayers[iRing][iLayer];
    reflect_distance = m_reflectedDistsForBarrelLayers[iRing][iLayer];

    if (direct_distance==m_notInitVal) {
      //We need to initialize
      calculateSignalDists_Barrel(iRing,iLayer,direct_distance,reflect_distance);
      m_directDistsForBarrelLayers[iRing][iLayer] = direct_distance;
      m_reflectedDistsForBarrelLayers[iRing][iLayer] = reflect_distance;
    };

    //Z is counted positive AWAY from the electronics:
    propagationTime1 = (direct_distance  + meanZ) / m_signalPropagationSpeed;
    propagationTime2 = (reflect_distance - meanZ) / m_signalPropagationSpeed;

  }

  return;
}

//__________________________________________________________________________________________________________
void TRTTimeCorrection::calculateSignalDists_Barrel(const unsigned int& iRing, const unsigned int& iLayer,
						    double& direct_dist, double& reflect_dist ) const {

  //We need to calculate the distance along the wire that the signal
  //has to travel before it reaches the electronics. Both if it goes
  //directly to the electronics and if it goes via a reflection in the
  //other end of the wire.
  //
  //The signal starts in the middle of the active region.
  //
  //In addition to the length of the active gas, the signal also has
  //to go through the little dead region at the end.

  const InDetDD::TRT_BarrelElement * barrel_element(m_detmgr->getBarrelElement(0,//positive,
									       iRing,//moduleIndex,
									       0,//int phiIndex,
									       iLayer));//strawLayerIndex

  direct_dist  = 0.5*barrel_element->strawLength() + m_lengthDeadRegion;
  reflect_dist = 1.5*barrel_element->strawLength() + 3*m_lengthDeadRegion;

  return;
}

//__________________________________________________________________________________________________________
void TRTTimeCorrection::calculateSignalDists_EndCap(const unsigned int& iWheel,
						    double& direct_dist,
						    double& reflect_dist ) const {

  //For an explanation, please read the comment in calculateSignalDists_Barrel

  const InDetDD::TRT_EndcapElement * ec_element(m_detmgr->getEndcapElement(0,//positive,
									   iWheel,//wheelIndex,
									   0,//strawLayerIndex,
									   0));//phiIndex

  direct_dist  = 0.5*ec_element->strawLength() + m_lengthDeadRegion;
  reflect_dist = 1.5*ec_element->strawLength() + 3*m_lengthDeadRegion;

  return;
}

//_____________________________________________________________________________
Identifier TRTTimeCorrection::getIdentifier ( int hitID,
					      bool & statusok)
{
  statusok = true;

  Identifier IdStraw;
  Identifier IdLayer;

  const int mask(0x0000001F);
  const int word_shift(5);
  int trtID, ringID, moduleID, layerID, strawID;
  int wheelID, planeID, sectorID;

  const InDetDD::TRT_BarrelElement *barrelElement;
  const InDetDD::TRT_EndcapElement *endcapElement;

  if ( !(hitID & 0x00200000) ) {      // barrel
    strawID   = hitID & mask;
    hitID   >>= word_shift;
    layerID   = hitID & mask;
    hitID   >>= word_shift;
    moduleID  = hitID & mask;
    hitID   >>= word_shift;
    ringID    = hitID & mask;
    trtID     = hitID >> word_shift;

    barrelElement =
        m_detmgr->getBarrelElement(trtID, ringID, moduleID, layerID);
    if ( barrelElement ) {

      IdLayer = barrelElement->identify();
      IdStraw = m_trt_id->straw_id(IdLayer, strawID);
    } else {
      if (msgLevel(MSG::ERROR)) msg(MSG::ERROR)
	<< "Could not find detector element for barrel identifier with "
	<< "(ipos,iring,imod,ilayer,istraw) = ("
      	<< trtID << ", " << ringID << ", " << moduleID << ", "
      	<< layerID << ", " << strawID << ")"
      	<< endmsg;
      statusok = false;
    }
  } else {                           // endcap
    strawID   = hitID & mask;
    hitID   >>= word_shift;
    planeID   = hitID & mask;
    hitID   >>= word_shift;
    sectorID  = hitID & mask;
    hitID   >>= word_shift;
    wheelID   = hitID & mask;
    trtID     = hitID >> word_shift;

    // change trtID (which is 2/3 for endcaps) to use 0/1 in getEndcapElement
    if (trtID == 3) trtID = 0;
    else            trtID = 1;

    endcapElement =
      m_detmgr->getEndcapElement(trtID, wheelID, planeID, sectorID);

    if ( endcapElement ) {
      IdLayer = endcapElement->identify();
      IdStraw = m_trt_id->straw_id(IdLayer, strawID);
    } else {
      if (msgLevel(MSG::ERROR)) msg(MSG::ERROR)
	<< "Could not find detector element for endcap identifier with "
	<< "(ipos,iwheel,isector,iplane,istraw) = ("
	<< trtID << ", " << wheelID << ", " << sectorID << ", "
	<< planeID << ", " << strawID << ")"
	<< endmsg;
      if (msgLevel(MSG::ERROR)) msg(MSG::ERROR)
	<< "If this happens very rarely, don't be alarmed (it is a Geant4 'feature')" << endmsg;
      if (msgLevel(MSG::ERROR)) msg(MSG::ERROR)
	<< "If it happens a lot, you probably have misconfigured geometry in the sim. job." << endmsg;
      statusok = false;
    }

  }

  return IdStraw;
}
