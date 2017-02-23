
#include "SCT_DigitizationRTT.h"
#include "GaudiKernel/ITHistSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include <boost/io/ios_state.hpp>
#include "TH1.h"
#include "TH2.h"

#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetSimEvent/SiHitCollection.h"
#include "InDetSimEvent/SiHit.h"
#include "InDetReadoutGeometry/SCT_BarrelModuleSideDesign.h"
#include "InDetReadoutGeometry/SCT_ForwardModuleSideDesign.h"

/////////////////////////////////////////////////////////////////////////////

StatusCode SCT_DigitizationRTT::PrintG4Hits() {

  ATH_MSG_INFO ("PrintG4Hits()" );

  boost::io::ios_all_saver ias( std::cout );

  std::cout << " G4 hits for event" << std::setw(7) << m_numberOfEventsInput
            << " : run/event=" << m_runNumber << "/" << m_eventNumber
            << std::endl;

  // Find SI hit collection(s)
  const DataHandle<SiHitCollection> begHitColl, endHitColl;
  StatusCode sc = evtStore()->retrieve(begHitColl,endHitColl);
  if (sc.isFailure()) {
    ATH_MSG_WARNING ("Si hit collection not found" );
    return StatusCode::FAILURE;
  }

  std::cout << "  B  L  P  E  S   Trk     time       dE      xS       yS       zS       xE       yE       zE       step   strS    strE     thW   phiW   hitId    xlocG4"
            << std::endl;

  // Loop over Si hit collections
  //  (one for pixel hits, one for SCT hits ??  How to check which is which ?)
  for ( ; begHitColl != endHitColl; ++begHitColl) {

    // Loop over SCT hits
    SiHitCollection::const_iterator hit;
    for (hit = begHitColl->begin(); hit < begHitColl->end(); ++hit) {
      if (hit->isSCT()) {

        // Unpack detailed information on this hit
        const SiHit& g4Hit = *hit;
	unpackG4Hit(g4Hit,g4Hit);

        std::cout << std::setw(3)  << hit->getBarrelEndcap()
                  << std::setw(3)  << hit->getLayerDisk()
                  << std::setw(3)  << hit->getPhiModule()
                  << std::setw(3)  << hit->getEtaModule()
                  << std::setw(3)  << hit->getSide()
                  << std::setw(7)  << hit->trackNumber()
		  << std::fixed << std::showpoint << std::setprecision(5)
		  << std::setw(9)  << hit->meanTime()
		  << std::fixed << std::showpoint << std::setprecision(3)
		  << std::setw(9)  << hit->energyLoss()
		  << std::setw(9)  << hit->localStartPosition().x()
		  << std::setw(9)  << hit->localStartPosition().y()
		  << std::setw(9)  << hit->localStartPosition().z()
		  << std::setw(9)  << hit->localEndPosition().x()
		  << std::setw(9)  << hit->localEndPosition().y()
		  << std::setw(9)  << hit->localEndPosition().z()
		  << std::setw(9)  << m_G4_stepLength
		  << std::fixed << std::showpoint << std::setprecision(2)
		  << std::setw(8)  << m_G4_stripStart
		  << std::setw(8)  << m_G4_stripEnd
		  << std::fixed << std::showpoint << std::setprecision(1)
		  << std::setw(7)  << m_G4_theta
		  << std::setw(7)  << m_G4_phi
                  << std::setw(9)  << hit->identify()
		  << std::fixed << std::showpoint << std::setprecision(4)
		  << std::setw(10)  << m_G4_xlocalG4[0]
                  << std::endl;
	ias.restore();
      }
    }
  }

  // Print wafer->Eloss and strip->Eloss maps
  std::cout << "  B  L  P  E  S  Str      dE       (std::map m_G4_mapWaferToEloss)" << std::endl;

  std::map<Identifier,double>::iterator mapIt;
  for (mapIt = m_G4_mapWaferToEloss.begin(); mapIt != m_G4_mapWaferToEloss.end(); ++mapIt) {
    Identifier waferId = mapIt->first;
    double trueEloss   = mapIt->second;
    std::cout << std::setw(3)  << m_sctID->barrel_ec(waferId)
              << std::setw(3)  << m_sctID->layer_disk(waferId)
              << std::setw(3)  << m_sctID->phi_module(waferId)
              << std::setw(3)  << m_sctID->eta_module(waferId)
              << std::setw(3)  << m_sctID->side(waferId)
              << std::setw(5)  << m_sctID->strip(waferId)
	      << std::fixed << std::showpoint << std::setprecision(3)
              << std::setw(9)  << trueEloss
              << std::endl;
    ias.restore();
  }

  std::cout << "  B  L  P  E  S  Str      dE                               (std::map m_G4_mapStripToEloss)" << std::endl;

  for (mapIt = m_G4_mapStripToEloss.begin(); mapIt != m_G4_mapStripToEloss.end(); ++mapIt) {
    Identifier stripId = mapIt->first;
    double trueEloss   = mapIt->second;
    std::cout << std::setw(3)  << m_sctID->barrel_ec(stripId)
              << std::setw(3)  << m_sctID->layer_disk(stripId)
              << std::setw(3)  << m_sctID->phi_module(stripId)
              << std::setw(3)  << m_sctID->eta_module(stripId)
              << std::setw(3)  << m_sctID->side(stripId)
              << std::setw(5)  << m_sctID->strip(stripId)
	      << std::fixed << std::showpoint << std::setprecision(3)
              << std::setw(9)  << trueEloss
              << std::endl;
    ias.restore();
  }

  // Print wafer->startHit and wafer->endHit maps
  std::cout << "  B  L  P  E  S  Str     tS      tE      xlocG4            (std::map m_G4_mapWaferToStartHit, m_G4_mapWaferToEndHit)" << std::endl;

  std::map<Identifier, const SiHit*>::iterator mapHitIt;
  for (mapHitIt = m_G4_mapWaferToStartHit.begin(); mapHitIt != m_G4_mapWaferToStartHit.end(); ++mapHitIt) {
    Identifier waferId = mapHitIt->first;
    const SiHit* hitStart    = mapHitIt->second;
    const SiHit* hitEnd      = m_G4_mapWaferToEndHit[waferId];
    unpackG4Hit(*hitStart,*hitEnd);
    std::cout << std::setw(3)  << m_sctID->barrel_ec(waferId)
              << std::setw(3)  << m_sctID->layer_disk(waferId)
              << std::setw(3)  << m_sctID->phi_module(waferId)
              << std::setw(3)  << m_sctID->eta_module(waferId)
              << std::setw(3)  << m_sctID->side(waferId)
              << std::setw(5)  << m_sctID->strip(waferId)
	      << std::fixed << std::showpoint << std::setprecision(5)
              << std::setw(9)  << hitStart->meanTime()
              << std::setw(9)  << hitEnd->meanTime()
              << std::setw(10) << m_G4_xlocalG4[0]
              << std::endl;
    ias.restore();
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode SCT_DigitizationRTT::FillSimDataHists() {

  ATH_MSG_DEBUG ("FillSimDataHists()");
 
  NoOuterModules.clear();
  NoShortMiddleModules.clear();
  NoMiddleModules.clear();
  NoInnerModules.clear();
  NoBarrelModules.clear();

  // Retrieve SiHit collection(s)
  const DataHandle<SiHitCollection> begHitColl, endHitColl;
  if (evtStore()->retrieve(begHitColl,endHitColl).isFailure()) 
    {
      ATH_MSG_WARNING ("SiHitContainer not found" );
      return StatusCode::FAILURE;
    }
  
  // Loop over SiHit collections
  for ( ; begHitColl != endHitColl; ++begHitColl) 
    {
      //Only interested in SCT SiHits
      SiHitCollection::const_iterator hit = begHitColl->begin();
      SiHitCollection::const_iterator endhit = begHitColl->end();
      
      if(hit < endhit) 
	{
	  if ( !(hit->isSCT()) ) 
	    { 
	      continue; //sanity check for SCT hit collection
	    }
	}
      else 
	{
	  continue; 
	}
      // Loop over SCT hits 
      for ( ; hit < endhit; ++hit) 
	{
      
	  // Unpack detailed information on this hit
	  int sctBarrel    = hit->getBarrelEndcap();
	  int sctLayer     = hit->getLayerDisk();
	  int sctEtaMod    = hit->getEtaModule();
	  int sctPhiMod    = hit->getPhiModule();
	  int sctSide      = hit->getSide();
	  if(sctBarrel==0)
	    {
	      if(sctEtaMod<0) ++sctEtaMod;
	      sctEtaMod = sctEtaMod + 5;        // puts eta module number in range 0-11
	      int index = 2*(240*sctEtaMod+60*sctLayer+sctPhiMod)+sctSide;
	      NoBarrelModules[index]=1;
	    }
	  if(sctBarrel!= 0) 
	    {
	      //Module_type  <-> Integer
	      //Barrel       <-> 4
	      //Inner        <-> 3
	      //Middles      <-> 2
	      //Short Middles<-> 1
	      //Outer        <-> 0
	      int index = 2*((1000*(2+sctBarrel))+(60*sctLayer+sctPhiMod))+sctSide;
	      if(sctEtaMod==2) 
		{
		  NoInnerModules[index]=1;
		}
	      if(sctEtaMod==1)
		{
		  if(sctLayer==7) 
		    {
		      NoShortMiddleModules[index]=1;
		    }
		  else
		    {
		      NoMiddleModules[index]=1;
		    }
		}
	      if(sctEtaMod==0) 
		{
		  NoOuterModules[index]=1;
		}
	    }
	}
    }

  m_NumberModulesVetoed[0] += NoOuterModules.size();
  m_NumberModulesVetoed[1] += NoShortMiddleModules.size();
  m_NumberModulesVetoed[2] += NoMiddleModules.size();
  m_NumberModulesVetoed[3] += NoInnerModules.size();
  m_NumberModulesVetoed[4] += NoBarrelModules.size();
  ATH_MSG_DEBUG ("Number of Module sides with True Track Hits: Outer(" << NoOuterModules.size() << "), Short Middle(" << NoShortMiddleModules.size() 
		 << "), Middle(" << NoMiddleModules.size() << "), Inner(" << NoInnerModules.size() << "), Barrel(" << NoBarrelModules.size() << ") "); 
  
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode SCT_DigitizationRTT::ProcessG4Hits() {

  ATH_MSG_DEBUG ("ProcessG4Hits()" );

  // Clear all maps
  if (!m_G4_mapWaferToEloss.empty()) {
    m_G4_mapWaferToEloss.erase(m_G4_mapWaferToEloss.begin(),m_G4_mapWaferToEloss.end());
  }
  if (!m_G4_mapStripToEloss.empty()) {
    m_G4_mapStripToEloss.erase(m_G4_mapStripToEloss.begin(),m_G4_mapStripToEloss.end());
  }
  if (!m_G4_mapWaferToStartHit.empty()) {
    m_G4_mapWaferToStartHit.erase(m_G4_mapWaferToStartHit.begin(),m_G4_mapWaferToStartHit.end());
  }
  if (!m_G4_mapWaferToEndHit.empty()) {
    m_G4_mapWaferToEndHit.erase(m_G4_mapWaferToEndHit.begin(),m_G4_mapWaferToEndHit.end());
  }

  Identifier waferId;
  double tLastHit(0.);

  // Retrieve SiHit collection(s)
  const DataHandle<SiHitCollection> begHitColl, endHitColl;
  if (evtStore()->retrieve(begHitColl,endHitColl).isFailure()) {
    ATH_MSG_WARNING ("SiHitContainer not found" );
    return StatusCode::FAILURE;
  }

  // Loop over SiHit collections
  for ( ; begHitColl != endHitColl; ++begHitColl) {

    //Only interested in SCT SiHits
    SiHitCollection::const_iterator hit = begHitColl->begin();
    SiHitCollection::const_iterator endhit = begHitColl->end();

    if(hit < endhit) {
      if ( !(hit->isSCT()) ) { 
	//ATH_MSG_VERBOSE ("ProcessG4Hits() - Found a Pixel SiHitCollection." );
	continue; //sanity check for SCT hit collection
      }
      //ATH_MSG_VERBOSE ("ProcessG4Hits() - Found an SCT SiHitCollection." );
    }
    else {
      //ATH_MSG_VERBOSE ("ProcessG4Hits() - Found an empty SiHitCollection." );
      continue; 
    }
    // Loop over SCT hits 
    for ( ; hit < endhit; ++hit) {
      
      // Unpack detailed information on this hit
      const SiHit& g4Hit = (*hit);
      if(!unpackG4Hit(g4Hit,g4Hit))
	{
	  ATH_MSG_WARNING ("Failed to upack G4Hit in ProcessG4Hits" );
	  return StatusCode::SUCCESS;
	}

      // Save pointers to the first and last hits (steps) of primary track #1 in each wafer it passes through
      // Assumes that the G4 hits are ordered by increasing time
      if (hit->trackNumber()==10001) {
	if (hit->meanTime() < tLastHit) {
	  ATH_MSG_ERROR ("G4 hits are not time-ordered: " << hit->meanTime() << ", " << tLastHit );
	  return StatusCode::FAILURE;
	}
	if (m_G4_waferId != waferId) {
	  if (m_G4_mapWaferToStartHit.find(m_G4_waferId) != m_G4_mapWaferToStartHit.end()) {
	    ATH_MSG_ERROR ("Wafer " << m_sctID->show_to_string(m_G4_waferId) << " already has a primary hit" );
	  }
	  // m_G4_mapWaferToStartHit[m_G4_waferId] = g4Hit;
	  m_G4_mapWaferToStartHit.insert( std::pair<Identifier, const SiHit*> (m_G4_waferId, &g4Hit) );
	}
	// m_G4_mapWaferToEndHit.insert( std::pair<Identifier, const SiHit*> (m_G4_waferId, &g4Hit) );
	m_G4_mapWaferToEndHit[m_G4_waferId] = &g4Hit;
	waferId  = m_G4_waferId;
	tLastHit = hit->meanTime();
      }
      // Build up a map: waferId -> true energy deposited
      m_G4_mapWaferToEloss[m_G4_waferId] += hit->energyLoss();

      // Build up a map: stripId -> true energy deposited
      // If the G4 step crosses one or more strip boundaries, the energy loss is shared amongst strips according to the path length in each strip
      double stripStart = m_G4_corrActiveStripStart;
      double stripEnd   = m_G4_corrActiveStripEnd;
      if (stripStart > 900. || stripEnd > 900.) continue;
      double stripMin  = std::min(stripStart,stripEnd);
      double stripMax  = std::max(stripStart,stripEnd);
      int    iStripMin = static_cast<int>(stripMin);
      int    iStripMax = static_cast<int>(stripMax);
      double fStripMin = double(iStripMin+1) - stripMin;
      double fStripMax = stripMax - double(iStripMax);
      double fTotal    = stripMax - stripMin;
      double eLoss = hit->energyLoss();

      for (int strip = iStripMin; strip <= iStripMax; ++strip) {
	Identifier stripId = m_sctID->strip_id(m_G4_waferId,strip);
	if (iStripMin == iStripMax) {
	  m_G4_mapStripToEloss[stripId] += eLoss;
	} else {
	  double fStrip = 1.;
	  if (strip == iStripMin) fStrip = fStripMin;
	  if (strip == iStripMax) fStrip = fStripMax;
	  m_G4_mapStripToEloss[stripId] += eLoss * (fStrip/fTotal);
	}
      }
    }
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

//StatusCode SCT_DigitizationRTT::unpackG4Hit(const SiHit& hitStart, const SiHit& hitEnd) {
bool SCT_DigitizationRTT::unpackG4Hit(const SiHit& hitStart, const SiHit& hitEnd) {

  ATH_MSG_VERBOSE ("unpackG4Hit()" );
  m_G4_element = 0;
  m_G4_xStep[0]   = 999.;
  m_G4_xStep[1]   = 999.;
  m_G4_xStep[2]   = 999.;
  m_G4_stepLength = 999.;
  m_G4_phi        = 999.;
  m_G4_theta      = 999.;
  m_G4_stripStart = 999.;
  m_G4_stripEnd   = 999.;
  m_G4_xlocFracAcrossStrip  = 999.;
  m_G4_cleanHit   = false;

  if (hitStart.isSCT()) {
    ATH_MSG_VERBOSE ("SCT hit ID=" << hitStart.identify()
		     << ", barrel/endcap =" << hitStart.getBarrelEndcap()
		     << ", layer/disk =" << hitStart.getLayerDisk()
		     << ", eta module =" << hitStart.getEtaModule()
		     << ", phi module =" << hitStart.getPhiModule()
		     << ", side =" << hitStart.getSide()
		     << ", track =" << hitStart.trackNumber()
		     );
    ATH_MSG_VERBOSE ("   "
		     << "start (x,y,z)=(" << hitStart.localStartPosition().x() << "," << hitStart.localStartPosition().y() << "," << hitStart.localStartPosition().z()
		     << "), end (x,y,z)=(" << hitEnd.localEndPosition().x() << "," << hitEnd.localEndPosition().y() << "," << hitEnd.localEndPosition().z()
		     << "), energy loss =" << hitStart.energyLoss()
		     << ", time =" << hitStart.meanTime()
		     );

    // Set up identifier for wafer containing start hit
    int sctBarrel    = hitStart.getBarrelEndcap();
    int sctLayer     = hitStart.getLayerDisk();
    int sctEtaModule = hitStart.getEtaModule();
    int sctPhiModule = hitStart.getPhiModule();
    int sctSide      = hitStart.getSide();
    m_G4_waferId = m_sctID->wafer_id(sctBarrel,sctLayer,sctPhiModule,sctEtaModule,sctSide);

    // Check that the start and end hits are in the same wafer
    sctBarrel    = hitEnd.getBarrelEndcap();
    sctLayer     = hitEnd.getLayerDisk();
    sctEtaModule = hitEnd.getEtaModule();
    sctPhiModule = hitEnd.getPhiModule();
    sctSide      = hitEnd.getSide();
    Identifier idEnd = m_sctID->wafer_id(sctBarrel,sctLayer,sctPhiModule,sctEtaModule,sctSide);
    if (idEnd != m_G4_waferId) {
      ATH_MSG_ERROR ("Start and end hits belong to different wafers: " << m_sctID->show_to_string(m_G4_waferId) << ", " << m_sctID->show_to_string(idEnd)
		     );
      return false;
    }

    // Calculate step vector and step length
    m_G4_xStep[0] = (hitEnd.localEndPosition()).x() - (hitStart.localStartPosition()).x();
    m_G4_xStep[1] = (hitEnd.localEndPosition()).y() - (hitStart.localStartPosition()).y();
    m_G4_xStep[2] = (hitEnd.localEndPosition()).z() - (hitStart.localStartPosition()).z();
    m_G4_stepLength = sqrt(m_G4_xStep[0]*m_G4_xStep[0] + m_G4_xStep[1]*m_G4_xStep[1] + m_G4_xStep[2]*m_G4_xStep[2]);

    // Calculate phi and theta angle of step wrt wafer surface
    if (m_G4_xStep[1]<0.) m_G4_phi   = -90.;
    if (m_G4_xStep[2]<0.) m_G4_theta = -90.;
    if (m_G4_xStep[0]!=0.){
      m_G4_phi   = atan(m_G4_xStep[1]/m_G4_xStep[0])/CLHEP::deg;
      m_G4_theta = atan(m_G4_xStep[2]/m_G4_xStep[0])/CLHEP::deg;
    }

    // Unpack start and end points of G4 hit in simulation local coordinates
    m_G4_localHitStartPoint = hitStart.localStartPosition();
    m_G4_localHitEndPoint   = hitEnd.localEndPosition();

    // Transform to reconstruction local coordinates (different x,y,z ordering and sign conventions compared to simulation coords)
    m_G4_element = m_sctmgr->getDetectorElement(m_G4_waferId);
    if (!m_G4_element) {
      ATH_MSG_ERROR ("Detector element for module " << m_sctID->show_to_string(m_G4_waferId) << " not found !" );
      return false;
    }
    m_G4_localStartPoint = m_G4_element->hitLocalToLocal3D(m_G4_localHitStartPoint);
    m_G4_localEndPoint   = m_G4_element->hitLocalToLocal3D(m_G4_localHitEndPoint);

    // Transform from simulation local coords to global coords
    m_G4_globalHitStartPoint = m_G4_element->globalPositionHit(m_G4_localHitStartPoint);
    m_G4_globalHitEndPoint   = m_G4_element->globalPositionHit(m_G4_localHitEndPoint);

    // Transform from reconstruction local coords to global coords
    //  (which should give same answer as above)
    m_G4_globalStartPoint = m_G4_element->globalPosition(m_G4_localStartPoint);
    m_G4_globalEndPoint   = m_G4_element->globalPosition(m_G4_localEndPoint);

    ATH_MSG_VERBOSE ("   "
		     << "sim local start (x,y,z)=(" << m_G4_localHitStartPoint.x() << "," << m_G4_localHitStartPoint.y() << "," << m_G4_localHitStartPoint.z()
		     << "), sim local end (x,y,z)=(" << m_G4_localHitEndPoint.x() << "," << m_G4_localHitEndPoint.y() << "," << m_G4_localHitEndPoint.z()
		     << ")"
		     );
    ATH_MSG_VERBOSE ("   "
		     << "local start (x,y,z)=(" << m_G4_localStartPoint.x() << "," << m_G4_localStartPoint.y() << "," << m_G4_localStartPoint.z()
		     << "), local end (x,y,z)=(" << m_G4_localEndPoint.x() << "," << m_G4_localEndPoint.y() << "," << m_G4_localEndPoint.z()
		     << ")"
		     );
    ATH_MSG_VERBOSE ("   "
		     << "global start (x,y,z)=(" << m_G4_globalHitStartPoint.x() << "," << m_G4_globalHitStartPoint.y() << "," << m_G4_globalHitStartPoint.z()
		     << "), global end (x,y,z)=(" << m_G4_globalHitEndPoint.x() << "," << m_G4_globalHitEndPoint.y() << "," << m_G4_globalHitEndPoint.z()
		     << ")"
		     );
    ATH_MSG_VERBOSE ("   "
		     << "global start (x,y,z)=(" << m_G4_globalStartPoint.x() << "," << m_G4_globalStartPoint.y() << "," << m_G4_globalStartPoint.z()
		     << "), global end (x,y,z)=(" << m_G4_globalEndPoint.x() << "," << m_G4_globalEndPoint.y() << "," << m_G4_globalEndPoint.z()
		     << ")"
		     );
    // Compute reconstruction local coordinates of G4 hit extrapolated to mid-plane of wafer
    double alpha(0.5);
    if (m_G4_localStartPoint.z() != m_G4_localEndPoint.z()) alpha = m_G4_localStartPoint.z()/(m_G4_localStartPoint.z()-m_G4_localEndPoint.z());
    m_G4_xlocalG4[0] = m_G4_localStartPoint.x() + alpha*(m_G4_localEndPoint.x() - m_G4_localStartPoint.x());
    m_G4_xlocalG4[1] = m_G4_localStartPoint.y() + alpha*(m_G4_localEndPoint.y() - m_G4_localStartPoint.y());
    m_G4_xlocalG4[2] = m_G4_localStartPoint.z() + alpha*(m_G4_localEndPoint.z() - m_G4_localStartPoint.z());     // should always be zero

    // Find (decimal) strip numbers corresponding to start point and end point
    m_G4_stripStart = stripNumberFromPosition(*m_G4_element,m_G4_localHitStartPoint);
    m_G4_stripEnd   = stripNumberFromPosition(*m_G4_element,m_G4_localHitEndPoint);

    // Find fractional distance across strip at mid-plane of wafer
    if (m_G4_stripStart<900. && m_G4_stripEnd<900.) {
      double stripMidPlane = m_G4_stripStart + alpha*(m_G4_stripEnd - m_G4_stripStart);
      m_G4_xlocFracAcrossStrip = stripMidPlane - int(stripMidPlane);
    }

    // Positions and strip numbers after Lorentz drift to readout surface
    m_G4_corrLocalHitStartPoint = correctedLocalPosition(*m_G4_element,m_G4_localHitStartPoint);
    m_G4_corrLocalHitEndPoint   = correctedLocalPosition(*m_G4_element,m_G4_localHitEndPoint);
    m_G4_corrStripStart = stripNumberFromPosition(*m_G4_element,m_G4_corrLocalHitStartPoint);
    m_G4_corrStripEnd   = stripNumberFromPosition(*m_G4_element,m_G4_corrLocalHitEndPoint);

    // Find strip number range for portion of step within active area
    m_G4_corrActiveStripStart = m_G4_corrStripStart;
    m_G4_corrActiveStripEnd   = m_G4_corrStripEnd;
    if ( m_G4_corrStripStart>900. || m_G4_corrStripEnd>900. ) {
      HepGeom::Point3D<double> corrStep = m_G4_corrLocalHitEndPoint - m_G4_corrLocalHitStartPoint;
      double corrStepLength = corrStep.mag();
      double stripLast(0.);
      unsigned int nActive(0);
      unsigned int nslice = std::max( int(corrStepLength/(0.001*CLHEP::mm))+1, 2 );
      for (unsigned int islice=0; islice<=nslice; islice++) {
        HepGeom::Point3D<double> position = m_G4_corrLocalHitStartPoint + (double(islice)/double(nslice))*(m_G4_corrLocalHitEndPoint-m_G4_corrLocalHitStartPoint);
        double strip = stripNumberFromPosition(*m_G4_element,position);
        if (strip<900.) {
          if (m_G4_corrActiveStripStart>900.) m_G4_corrActiveStripStart = strip;
          m_G4_corrActiveStripEnd = strip;
        }
        if (islice>0 && strip<900. && stripLast>900.) nActive++;
        stripLast = strip;
      }
      if (nActive>=2) ATH_MSG_ERROR (" Found " << nActive << " active regions" );
    }
    if ( (m_G4_corrActiveStripStart>900. && m_G4_corrActiveStripEnd<900.) || (m_G4_corrActiveStripStart<900. && m_G4_corrActiveStripEnd>900.) ) {
      ATH_MSG_ERROR (" Active region " << m_G4_corrActiveStripStart << " to " << m_G4_corrActiveStripEnd << " is not valid: wafer="
		     << m_sctID->show_to_string(m_G4_waferId) << ", raw strip range = " << m_G4_stripStart << ", " << m_G4_stripEnd );
    }

    // Find angles theta,phi to wafer surface (alternative method, using global coords) (results not (yet?) stored in m_G4_)
    double xStepGlobal[3];
    xStepGlobal[0] = m_G4_globalHitEndPoint.x() - m_G4_globalHitStartPoint.x();
    xStepGlobal[1] = m_G4_globalHitEndPoint.y() - m_G4_globalHitStartPoint.y();
    xStepGlobal[2] = m_G4_globalHitEndPoint.z() - m_G4_globalHitStartPoint.z();
    double theta(999.),phi(999.);
    findAnglesToWaferSurface (xStepGlobal,m_G4_waferId,theta,phi);

    // Define a (fairly safe) fiducial volume for the G4 hit
    m_G4_withinFidVol = true;
    if (fabs(m_G4_xlocalG4[0]) > 28.0*CLHEP::mm) m_G4_withinFidVol = false;
    if (fabs(m_G4_xlocalG4[1]) > 60.0*CLHEP::mm) m_G4_withinFidVol = false;
    if (fabs(m_G4_xlocalG4[1]) <  3.0*CLHEP::mm) m_G4_withinFidVol = false;

    // Define clean hits passing right through wafer well away from dead regions
    // *** could now use m_G4_stripStart and m_G4_stripEnd to make these cuts, which may be easier in the endcaps ? ***
    m_G4_cleanHit = true;
    if (fabs(m_G4_localStartPoint.x()) > 28.0*CLHEP::mm) m_G4_cleanHit = false;
    if (fabs(m_G4_localStartPoint.y()) > 60.0*CLHEP::mm) m_G4_cleanHit = false;
    if (fabs(m_G4_localStartPoint.y()) <  3.0*CLHEP::mm) m_G4_cleanHit = false;
    if (fabs(m_G4_localEndPoint.x())   > 28.0*CLHEP::mm) m_G4_cleanHit = false;
    if (fabs(m_G4_localEndPoint.y())   > 60.0*CLHEP::mm) m_G4_cleanHit = false;
    if (fabs(m_G4_localEndPoint.y())   <  3.0*CLHEP::mm) m_G4_cleanHit = false;
    if (m_G4_localEndPoint.y()*m_G4_localStartPoint.y() < 0.)             m_G4_cleanHit = false;     // mustn't cross bond gap
    if (fabs(m_G4_localEndPoint.z()-m_G4_localStartPoint.z()) < 0.280*CLHEP::mm) m_G4_cleanHit = false;     // must pass right through wafer
  }

  return true;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

HepGeom::Point3D<double> SCT_DigitizationRTT::correctedLocalPosition ( const InDetDD::SiDetectorElement &element, const HepGeom::Point3D<double> &position ) {

  const double tanLorentz      = element.getTanLorentzAnglePhi();
  const double sensorThickness = element.design().thickness();
  const int    readoutSide     = element.design().readoutSide();

  double xReadoutSurface = 0.5*sensorThickness*readoutSide;
  double xReadout = 0.5*sensorThickness - readoutSide*position[SiHit::xDep];
  double dy = tanLorentz*xReadout;

  return HepGeom::Point3D<double>(xReadoutSurface,position[SiHit::xPhi]+dy,position[SiHit::xEta]);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

double SCT_DigitizationRTT::stripNumberFromPosition ( const InDetDD::SiDetectorElement &element, const HepGeom::Point3D<double> &position ) {

  // Convert to an Amg::Vector2D
  double xEta = position.z();
  double xPhi = position.y();
  Amg::Vector2D locPos = element.hitLocalToLocal(xEta,xPhi);

  // Find the Identifier of the cell (strip) containing the input local position
  // (the identifierOfPosition() method does not correct for the Lorentz angle)
  // (the identifierOfPosition() method requires the point to be inside the active region, ignoring bond-gaps)
  // *** could circumvent this by modifying ylocal to a value guaranteed to be inside the active area ***
  Identifier id = element.identifierOfPosition(locPos);
  if (!id.is_valid()) return 999.;

  // Require the position to be inside the active region (including allowing for bond-gaps)
  double xDepth = position.x();
  InDetDD::SiLocalPosition siLocPos(xEta,xPhi,xDepth);
  const InDetDD::SiDetectorDesign* design = &(element.design());
  const InDetDD::SCT_BarrelModuleSideDesign* barrelDesign = dynamic_cast<const InDetDD::SCT_BarrelModuleSideDesign*>(design);
  if (barrelDesign) {
    if (!(barrelDesign->inActiveArea(siLocPos,true))) return 999.;
  }
  const InDetDD::SCT_ForwardModuleSideDesign* endcapDesign = dynamic_cast<const InDetDD::SCT_ForwardModuleSideDesign*>(design);
  if (endcapDesign) {
    if (!(endcapDesign->inActiveArea(siLocPos,true))) return 999.;
  }

  // Find raw (i.e. no Lorentz correction) local position of centre of cell
  Amg::Vector2D locPosCentre = element.rawLocalPositionOfCell(id);

  // Compute decimal strip number of input position, where strip boundaries are at 0.0, 1.0, 2.0 etc.
  // (e.g. the centre of strip number 14 is 14.5)
  int strip = m_sctID->strip(id);
  double pitch = element.phiPitch(locPos);
  double dstrip = (locPos[Trk::locX] - locPosCentre[Trk::locX])/pitch;
  return double(strip) + dstrip + 0.5;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

const SiHit* SCT_DigitizationRTT::findG4Hit ( const Identifier &id ) 
{

  // for now, build an Identifier with no strip position
  //   (*** could extend to use strip number to select particular G4 hits within module)
  Identifier inputId = m_sctID->wafer_id(id);
  int inputSctBarrel = m_sctID->barrel_ec(inputId);
  int inputSctLayer  = m_sctID->layer_disk(inputId);
  //int inputSctEtaMod = m_sctID->eta_module(inputId);
  //int inputSctPhiMod = m_sctID->phi_module(inputId);
  //int inputSctSide   = m_sctID->side(inputId);

  const SiHit* pG4Hit(0);
  const SiHit* pG4HitSameWafer(0);
  const SiHit* pG4HitSameLayer(0);

  // Loop over Si (G4) hit collections
  const DataHandle<SiHitCollection> begHitColl, endHitColl;
  if (evtStore()->retrieve(begHitColl,endHitColl).isFailure()) 
    {
      //    ATH_MSG_ERROR ("findG4Hit():  Si G4 hit collection not found" );
      return pG4Hit;
    }
  
  // Loop over Si hit collections
  for ( ; begHitColl != endHitColl; ++begHitColl) 
    {
      // Loop over SCT G4 hits from primary true track number 1
      SiHitCollection::const_iterator hit = begHitColl->begin();
      SiHitCollection::const_iterator endhit = begHitColl->end();
      if(hit < endhit)
	{
	  if ( !(hit->isSCT()) ) continue; //sanity check for SCT hit collection
	}
      else continue; //empty SiHitCollection

      // Loop over SCT hits 
      for ( ; hit < endhit; ++hit) 
	{
	  if (hit->trackNumber()==10001) 
	    {
	      int sctBarrel = hit->getBarrelEndcap();
	      int sctLayer  = hit->getLayerDisk();
	      // Is this G4 hit in the required layer/disk?
	      if (inputSctBarrel==sctBarrel && inputSctLayer==sctLayer) 
		{
		  if (!pG4HitSameLayer) pG4HitSameLayer = &(*hit);
		  int sctEtaMod = hit->getEtaModule();
		  int sctPhiMod = hit->getPhiModule();
		  int sctSide   = hit->getSide();
		  Identifier G4HitId = m_sctID->wafer_id(sctBarrel,sctLayer,sctPhiMod,sctEtaMod,sctSide);
		  // Is this G4 hit in the required wafer?
		  if (G4HitId == inputId) 
		    {
		      if (!pG4HitSameWafer) pG4HitSameWafer = &(*hit);
		    }
		}
	    }
	}
      if(pG4HitSameWafer!=0) break; // we've found the hit now so stop
    }

  // return the first G4 hit found in the required wafer if one exists; in the required layer/disk if not
  pG4Hit = pG4HitSameWafer;
  if (!pG4Hit) pG4Hit = pG4HitSameLayer;

  return pG4Hit;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 



