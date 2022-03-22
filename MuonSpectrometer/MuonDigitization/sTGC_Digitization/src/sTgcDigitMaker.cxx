/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "sTGC_Digitization/sTgcDigitMaker.h"

#include "MuonDigitContainer/sTgcDigitCollection.h"
#include "MuonSimEvent/sTgcHitIdHelper.h"
#include "MuonSimEvent/sTgcSimIdToOfflineId.h"
#include "MuonIdHelpers/sTgcIdHelper.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"
#include "TrkEventPrimitives/LocalDirection.h"
#include "TrkSurfaces/Surface.h"
#include "GaudiKernel/MsgStream.h"
#include "PathResolver/PathResolver.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGaussZiggurat.h"
#include "CLHEP/Random/RandGamma.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "AthenaBaseComps/AthCheckMacros.h"

#include "TF1.h" 
#include <cmath>
#include <iostream>
#include <fstream>

//---------------------------------------------------
//  Constructor and Destructor
//---------------------------------------------------

//----- Constructor
sTgcDigitMaker::sTgcDigitMaker(const sTgcHitIdHelper* hitIdHelper, 
                               const MuonGM::MuonDetectorManager* mdManager, 
                               bool doEfficiencyCorrection)
  : AthMessaging (Athena::getMessageSvc(), "sTgcDigitMaker")
{
  m_hitIdHelper             = hitIdHelper;
  m_mdManager               = mdManager;
  m_doEfficiencyCorrection  = doEfficiencyCorrection;
  m_doTimeCorrection        = true;
  m_doTimeOffsetStrip       = false;
  //m_timeWindowPad          = 30.; // TGC  29.32; // 29.32 ns = 26 ns +  4 * 0.83 ns
  //m_timeWindowStrip         = 30.; // TGC  40.94; // 40.94 ns = 26 ns + 18 * 0.83 ns
  //m_bunchCrossingTime       = 24.95; // 24.95 ns =(40.08 MHz)^(-1)
  //m_timeJitterElectronicsStrip   = 2.; // 2ns jitter of electronics from the threshold to peak
  //m_timeJitterElectronicsPad     = 2.; // 2ns jitter of electronics for passing the threshold
  m_GausMean                = 2.27;  //mm; VMM response from Oct/Nov 2013 test beam
  m_GausSigma               = 0.1885;//mm; VMM response from Oct/Nov 2013 test beam
  m_IntegralTimeOfElectr    = 20.00; // ns
  m_CrossTalk               = 0.00; // Turn off cross-talk. Old guesstimate was 0.03: Alexandre Laurier 2020-10-11 
  m_StripResolution         = 0.07; // Angular strip resolution parameter
  m_ChargeSpreadFactor      = 0.;
  m_channelTypes            = 3; // 1 -> strips, 2 -> strips+pad, 3 -> strips/wires/pads
  m_theta = 0.8; // theta=0.8 value best matches the PDF
  m_mean = 2E5;  // mean gain estimated from ATLAS note "ATL-MUON-PUB-2014-001" 
}

//----- Destructor
sTgcDigitMaker::~sTgcDigitMaker()
{}
//------------------------------------------------------
// Initialize
//------------------------------------------------------
StatusCode sTgcDigitMaker::initialize(const int channelTypes)
{
  // Initialize TgcIdHelper
  if (m_hitIdHelper == nullptr) {
    m_hitIdHelper = sTgcHitIdHelper::GetHelper();
  }

  ////set the flag of timeCorrection
  //m_doTimeCorrection = doTimeCorrection;

  //set the flag of channelTypes which will be digitized
  m_channelTypes = channelTypes; 

  // initialize the TGC identifier helper
  m_idHelper = m_mdManager->stgcIdHelper();

  // Read share/sTGC_Digitization_energyThreshold.dat file and store values in m_energyThreshold.
  // Currently no point in wasting memory to read an empty file for energy threshold.
  // We have no gap-by-gap energy threshold currently for the sTGC
  // Alexandre Laurier - April 13 2021
  //ATH_CHECK(readFileOfEnergyThreshold());

  //// Read share/sTGC_Digitization_crossTalk.dat file and store values in m_crossTalk.
  //ATH_CHECK(readFileOfCrossTalk());

  // Read share/sTGC_Digitization_deadChamber.dat file and store values in m_isDeadChamber.
  ATH_CHECK(readFileOfDeadChamber());

  // Read share/sTGC_Digitization_effChamber.dat file and store values in m_ChamberEfficiency.
  ATH_CHECK(readFileOfEffChamber());

  // Read share/sTGC_Digitization_timeWindowOffset.dat file and store values in m_timeWindowOffset.
  ATH_CHECK(readFileOfTimeWindowOffset());

  //// Read share/sTGC_Digitization_alignment.dat file and store values in m_alignmentZ, m_alignmentT, m_alignmentS, m_alignmentTHS
  //ATH_CHECK(readFileOfAlignment());

  // Read share/sTGC_Digitization_timeArrivale.dat, containing the digit time of arrival
  ATH_CHECK(readFileOfTimeArrival());
  
  // Read share/sTGC_Digitization_timeOffsetStrip.dat if the the strip time correction is enable
  if (m_doTimeOffsetStrip) {
    ATH_CHECK(readFileOfTimeOffsetStrip());
  }

  return StatusCode::SUCCESS;
}

//---------------------------------------------------
// Execute Digitization
//---------------------------------------------------
std::unique_ptr<sTgcDigitCollection> sTgcDigitMaker::executeDigi(const sTGCSimHit* hit, 
                                                                 const float /*globalHitTime*/, 
                                                                 CLHEP::HepRandomEngine* rndmEngine) 
{ 

  // check the digitization channel type
  if(m_channelTypes!=1 && m_channelTypes!=2 && m_channelTypes!=3){
    ATH_MSG_ERROR("Invalid ChannelTypes : " << m_channelTypes << " (valid values are : 1 --> strips ; 2 --> strips / wires ; 3 --> strips / wires / pads)"); 
  }

  // SimHits without energy loss are not recorded.
  double energyDeposit = hit->depositEnergy(); // Energy deposit in MeV 
  if(energyDeposit==0.) return nullptr;

  //////////  convert ID for this digitizer system 
  sTgcSimIdToOfflineId simToOffline(m_idHelper);  
  int simId = hit->sTGCId();
  Identifier offlineId = simToOffline.convert(simId);
  std::string stationName= m_idHelper->stationNameString(m_idHelper->stationName(offlineId));
  int stationEta = m_idHelper->stationEta(offlineId);
  int stationPhi  = m_idHelper->stationPhi(offlineId);
  int multiPlet = m_idHelper->multilayer(offlineId);
  int gasGap = m_idHelper->gasGap(offlineId);
  Identifier layid = m_idHelper->channelID(m_idHelper->stationName(offlineId), stationEta, stationPhi, 
                                           multiPlet, gasGap, sTgcIdHelper::sTgcChannelTypes::Wire, 1);

  ATH_MSG_VERBOSE("sTgc hit:  time " << hit->globalTime() << " position " << hit->globalPosition().x() << "  " << hit->globalPosition().y() << "  " << hit->globalPosition().z() << " mclink " << hit->particleLink() << " PDG ID " << hit->particleEncoding() );

  int isSmall = stationName[2] == 'S';

  ATH_MSG_DEBUG("Retrieving detector element for: isSmall " << isSmall << " eta " << stationEta << " phi " << stationPhi << " ml " << multiPlet << " energyDeposit "<<energyDeposit );

  const MuonGM::sTgcReadoutElement* detEl = m_mdManager->getsTgcReadoutElement(layid);
  if( !detEl ){
    ATH_MSG_WARNING("Failed to retrieve detector element for: isSmall " << isSmall << " eta " << stationEta << " phi " << stationPhi << " ml " << multiPlet );
    return nullptr;
  }
 
  // DO THE DIGITIZATTION HERE ////////

  // Required precision on length in mm
  constexpr double length_tolerance = 0.01;

  // Retrieve the wire surface
  int surfHash_wire = detEl->surfaceHash(gasGap, sTgcIdHelper::sTgcChannelTypes::Wire);
  const Trk::PlaneSurface& SURF_WIRE = detEl->surface(surfHash_wire); // get the wire surface

  // Hit post-Step global position
  const Amg::Vector3D& GPOS{hit->globalPosition()};
  // Hit pre-Step global position
  const Amg::Vector3D& pre_pos{hit->globalPrePosition()};
  // Hit global direction
  const Amg::Vector3D& GLODIRE{hit->globalDirection()};

  // Hit position in the wire surface's coordinate frame 
  Amg::Vector3D hitOnSurface_wire = SURF_WIRE.transform().inverse() * GPOS;
  Amg::Vector3D pre_pos_wire_surf = SURF_WIRE.transform().inverse() * pre_pos;
  Amg::Vector2D posOnSurf_wire(0.5 * (hitOnSurface_wire.x() + pre_pos_wire_surf.x()),
                               0.5 * (hitOnSurface_wire.y() + pre_pos_wire_surf.y()));

  /* Determine the closest wire and the distance of closest approach
   * Since most particles pass through the the wire plane between two wires,
   * the nearest wire should be one of these two wire. Otherwise, the particle's
   * trajectory is uncommon, and such rare case is not supported yet.
   *
   * Finding that nearest wire follows the following steps:
   * - Compute the distance to the wire at the center of the current wire pitch
   * - Compute the distance to the other adjacent wire 
   */

  // Wire number of the current wire pitch
  int wire_number = detEl->getDesign(layid)->wireNumber(posOnSurf_wire);

  // If wire number is invalid, verify if hit is near the edge of the chamber. 
  const int number_wires = detEl->numberOfWires(layid);
  if ((wire_number < 1) || (wire_number > number_wires)) {
    // Compute the wire number using either the pos-step position or 
    // the pre-step position, whichever yields a valid wire number. 
    Amg::Vector2D new_posOnSurf(hitOnSurface_wire.x(), hitOnSurface_wire.y());
    wire_number = detEl->getDesign(layid)->wireNumber(new_posOnSurf);
    if ((wire_number < 1) || (wire_number > number_wires)) {
      new_posOnSurf = Amg::Vector2D(pre_pos_wire_surf.x(), pre_pos_wire_surf.y());
      wire_number = detEl->getDesign(layid)->wireNumber(new_posOnSurf);
    }
  }
  
  // Compute the position of the ionization and its distance relative to a given sTGC wire.
  Ionization ionization = pointClosestApproach(layid, wire_number, pre_pos_wire_surf, hitOnSurface_wire);
  double dist_wire = ionization.distance;
  if (dist_wire > 0.) {
    // Determine the other adjacent wire, which is
    //  -1 if particle crosses the wire surface between wire_number-1 and wire_number
    //  +1 if particle crosses the wire surface between wire_number and wire_number+1 
    int adjacent = 1;
    if (ionization.posOnSegment.x() < ionization.posOnWire.x()) {adjacent = -1;}

    // Find the position of the ionization with respect to the adjacent wire
    Ionization ion_adj = pointClosestApproach(layid, wire_number+adjacent, pre_pos_wire_surf, hitOnSurface_wire);
    // Keep the closest point 
    double dist_wire_adj = ion_adj.distance;
    if ((dist_wire_adj > 0.) && (dist_wire_adj < dist_wire)) {
      dist_wire = dist_wire_adj;
      wire_number += adjacent;
      ionization = std::move(ion_adj);
    }
  } else {
    ATH_MSG_DEBUG("Failed to get the distance between the wire number = " << wire_number 
                    << " and hit at (" << hitOnSurface_wire.x() << ", " << hitOnSurface_wire.y() << ")"
                    << ". Number of wires = " << number_wires
                    << ", chamber stationName: " << stationName 
                    << ", stationEta: " << stationEta
                    << ", stationPhi: " << stationPhi
                    << ", multiplet:" << multiPlet
                    << ", gas gap: " << gasGap);
    return nullptr;
  }

  // Update the position of ionization on the wire surface
  posOnSurf_wire = Amg::Vector2D(ionization.posOnWire.x(), ionization.posOnWire.y());
  // Position of the ionization in the global coordinate frame
  const Amg::Vector3D glob_ionization_pos = SURF_WIRE.transform() * ionization.posOnWire;

  ATH_MSG_VERBOSE("Ionization_info: distance: " << ionization.distance 
    << " posOnTrack: " << ionization.posOnSegment.x() << " " 
                        << ionization.posOnSegment.y() << " " 
                        << ionization.posOnSegment.z() << " "
    << " posOnWire: " << ionization.posOnWire.x() << " " 
                       << ionization.posOnWire.y() << " " 
                       << ionization.posOnWire.z() << " "
    << " hitGPos: " << GPOS.x() <<" "<< GPOS.y() <<" "<< GPOS.z() << " "
    << " hitPrePos: " << pre_pos.x() <<" "<< pre_pos.y() <<" "<< pre_pos.z()  << " "
    << " EDep: " << hit->depositEnergy() << " EKin: " << hit->kineticEnergy() 
    << " pdgId: " << hit->particleEncoding()
    << " stationName: " << stationName
    << " stationEta: " << stationEta
    << " stationPhi: " << stationPhi
    << " multiplet: " << multiPlet
    << " gasgap: " << gasGap);

  // Distance should be in the range [0, 0.9] mm, excepting
  // - particles pass through the wire plane near the edges
  // - secondary particles created inside the gas gap that go through the gas gap partially. 
  //   Most of such particles are not muons and have low kinetic energy. 
  // - particle with trajectory parallel to the sTGC wire plane
  const double wire_pitch = detEl->wirePitch();
  if ((dist_wire > 0.) && (std::abs(hit->particleEncoding()) == 13) && (dist_wire > (wire_pitch/2))) {
    ATH_MSG_DEBUG("Distance to the nearest wire (" << dist_wire << ") is greater than expected.");
    ATH_MSG_DEBUG("Hit globalPos: (" << GPOS.x() <<", "<< GPOS.y() <<", "<< GPOS.z() << ") "
                 << " globalPrePos: (" << pre_pos.x() <<", "<< pre_pos.y() <<", "<< pre_pos.z()  << ") "
                 << " EDeposited: " << hit->depositEnergy() << " EKinetic: " << hit->kineticEnergy() 
                 << " pdgID: " << hit->particleEncoding()
                 << " stationName = " << stationName
                 << " stationEta = " << stationEta
                 << " stationPhi = " << stationPhi
                 << " multiplet = " << multiPlet
                 << " gas gap = " << gasGap);
  }

  // Do not digitize hits that are too far from the nearest wire
  if (dist_wire > wire_pitch) {
    return nullptr;
  }

  // Get the gamma pdf parameters associated with the distance of closest approach.
  const GammaParameter gamParam = getGammaParameter(dist_wire);
  const double par_kappa = gamParam.kParameter; 
  const double par_theta = gamParam.thetaParameter; 
  const double most_prob_time = getMostProbableArrivalTime(dist_wire);
  // Compute the most probable value of the gamma pdf
  double gamma_mpv = (par_kappa - 1) * par_theta;
  // If the most probable value is less than zero, then set it to zero
  if (gamma_mpv < 0.) {gamma_mpv = 0.;}
  const double t0_par = most_prob_time - gamma_mpv;

  // Digit time follows a gamma distribution, so a value val is 
  // chosen using a gamma random generator then is shifted by t0
  // to account for drift time.
  // Note: CLHEP::RandGamma takes the parameters k and lambda, 
  // where lambda = 1 / theta.
  double digit_time = t0_par + CLHEP::RandGamma::shoot(rndmEngine, par_kappa, 1/par_theta);

  // Sometimes, digit_time is negative because t0_par can be negative. 
  // In such case, discard the negative value and shoot RandGamma for another value.
  // However, if that has already been done many times then set digit_time to zero 
  // in order to avoid runaway loop.
  constexpr int shoot_limit = 4;
  int shoot_counter = 0;
  while (digit_time < 0.) {
    if (shoot_counter > shoot_limit) {
      digit_time = 0.;
      break;
    }
    digit_time = t0_par + CLHEP::RandGamma::shoot(rndmEngine, par_kappa, 1/par_theta);
    ++shoot_counter;
  }

  ATH_MSG_DEBUG("sTgcDigitMaker distance = " << dist_wire 
                << ", time = " << digit_time
                << ", k parameter = " << par_kappa
                << ", theta parameter = " << par_theta
                << ", most probable time = " << most_prob_time);
  
  bool isValid = false;
  //// HV efficiency correction
  if (m_doEfficiencyCorrection){
    Identifier tempId = m_idHelper->channelID(m_idHelper->parentID(layid), multiPlet, gasGap, sTgcIdHelper::sTgcChannelTypes::Wire, 1, isValid);
    if (!isValid) return nullptr;
    // Transform STL and STS to 0 and 1 respectively
    int stNameInt = (stationName=="STL") ? 0 : 1;
    // If inside eta0 bin of QL1/QS1, remove 1 from eta index
    int etaZero = detEl->isEtaZero(tempId, hitOnSurface_wire.y()) ? 1 : 0;
    float efficiency = getChamberEfficiency(stNameInt, std::abs(stationEta)-etaZero, stationPhi-1, multiPlet-1, gasGap-1);
    // Lose Hits to match HV efficiency
    if (CLHEP::RandFlat::shoot(rndmEngine,0.0,1.0) > efficiency) return nullptr;
  }

  //// Check the chamber is dead or not.
  if(isDeadChamber(stationName, stationEta, stationPhi, multiPlet, gasGap)) return nullptr;

  //***************************** check effeciency ******************************** 
  // use energyDeposit to implement detector effeciency 
  // Currently, we do not have a gap-by-gap minimum energy deposit threshold
  // If and when this is implemented, the value must be read from a database
  //if(!efficiencyCheck(stationName, stationEta, stationPhi, multiPlet, gasGap, 1, energyDeposit)) return 0; 
  
  IdentifierHash coll_hash;
  // contain (name, eta, phi, multiPlet)
  m_idHelper->get_detectorElement_hash(layid, coll_hash);
  //ATH_MSG_DEBUG(" looking up collection using hash " << (int)coll_hash << " " << m_idHelper->print_to_string(layid) );

  auto digits = std::make_unique<sTgcDigitCollection>(layid, coll_hash);

 

  // #################################################################
  //***************************** BCTAGGER ******************************** 
  // #################################################################

  // use energyDeposit to implement detector effeciency 
  // Time of flight correction for each chamber
  // the offset is set to 0 for ASD located at the postion where tof is minimum in each chamber,
  // i.e. the ASD at the smallest R in each chamber
  //float tofCorrection = hit->globalPosition().mag()/CLHEP::c_light; //FIXME use CLHEP::c_light
  //float tofCorrection = globalHitTime; 

  //// bunch time
  //float bunchTime = globalHitTime - tofCorrection;


  //const float stripPropagationTime = 3.3*CLHEP::ns/CLHEP::m * detEl->distanceToReadout(posOnSurf_strip, elemId); // 8.5*ns/m was used until MC10. 
  const float stripPropagationTime = 0.; // 8.5*ns/m was used until MC10. 

  float sDigitTimeWire = digit_time;
  float sDigitTimePad = sDigitTimeWire;
  float sDigitTimeStrip = sDigitTimeWire + stripPropagationTime;


  //if(m_doTimeCorrection) sDigitTime = bunchTime + timeJitterDetector + timeJitterElectronics + stripPropagationTime;
  //else {
  //  sDigitTime = timeJitterDetector + timeJitterElectronics + stripPropagationTime;
  //  ATH_WARNING("Did not do Time Correction for this hit!");
  //}

  uint16_t bctag = 0;

  //if(sDigitTime < -m_bunchCrossingTime+m_timeWindowOffsetStrip || +m_bunchCrossingTime+m_timeWindowOffsetStrip+ m_timeWindowStrip < sDigitTime) {
  //    ATH_MSG_DEBUG("Strip: Digitized time is outside of time window. " << sDigitTime
  //      	      << " bunchTime: " << bunchTime 
  //                    << " time jitter from detector response : " << timeJitterDetector  
  //                    << " time jitter from electronics response : " << timeJitterElectronics  
  //      	      << " propagation time: " << stripPropagationTime )
  //  return digits;
  //}

  //currently do not give any bcId information, just give the acurate time
  //bctag = bcTagging(sDigitTime, channelType);


  //##################################################################################
  //######################################### strip readout ##########################
  //##################################################################################
  ATH_MSG_DEBUG("sTgcDigitMaker::strip response ");
  int channelType = sTgcIdHelper::sTgcChannelTypes::Strip;

  Identifier newId = m_idHelper->channelID(m_idHelper->parentID(layid), multiPlet, gasGap, channelType, 1, isValid);

  // get strip surface 
  int surfHash_strip =  detEl->surfaceHash(gasGap, 1);
  const Trk::PlaneSurface& SURF_STRIP = detEl->surface(surfHash_strip); // get the strip surface

  const Amg::Vector3D hitOnSurface_strip = SURF_STRIP.transform().inverse()*glob_ionization_pos;

  const Amg::Vector2D posOnSurf_strip(hitOnSurface_strip.x(),hitOnSurface_strip.y());
  bool insideBounds = SURF_STRIP.insideBounds(posOnSurf_strip);
  if(!insideBounds) { 
    ATH_MSG_DEBUG("Outside of the strip surface boundary : " <<  m_idHelper->print_to_string(newId) << "; local position " <<posOnSurf_strip ); 
    return nullptr;
  }

  //************************************ find the nearest readout element ************************************** 
       
  int stripNumber = detEl->stripNumber(posOnSurf_strip, newId);
  if( stripNumber == -1 ){
    // Verify if the energy deposit is at the boundary
    const float new_posX = (posOnSurf_strip.x() > 0.0)? posOnSurf_strip.x() - length_tolerance
                                                      : posOnSurf_strip.x() + length_tolerance;
    const Amg::Vector2D new_position(new_posX, posOnSurf_strip.y());
    stripNumber = detEl->stripNumber(new_position, newId);
    // Skip hit if still unable to obtain strip number
    if (stripNumber < 1) {
      ATH_MSG_WARNING("Failed to obtain strip number " << m_idHelper->print_to_string(newId) );
      ATH_MSG_WARNING("Position on strip surface = (" << posOnSurf_strip.x() << ", " << posOnSurf_strip.y() << ")");
      return nullptr;
    }
  }
  newId = m_idHelper->channelID(m_idHelper->parentID(layid), multiPlet, gasGap, channelType, stripNumber, isValid);
  if(!isValid && stripNumber != -1) {
    ATH_MSG_ERROR("Failed to obtain identifier " << m_idHelper->print_to_string(newId) ); 
    return nullptr;
  }

  int NumberOfStrips = detEl->numberOfStrips(newId); 
  double stripHalfPitch = detEl->channelPitch(newId)*0.5; // 3.2/2 = 1.6 mm

  //************************************ conversion of energy to charge **************************************

  // Constant determined from ionization study within Garfield
  // Note titled Charge Energy Relation which outlines conversion can be found here https://cernbox.cern.ch/index.php/apps/files/?dir=/__myshares/Documents (id:274113) 
  double ionized_charge = (5.65E-6)*energyDeposit/CLHEP::keV; // initial ionized charge in pC per keV deposited

 // To get avalanche gain, polya function is taken from Blum paper https://inspirehep.net/literature/807304
 // m_polyaFunction = new TF1("m_polyaFunction","(1.0/[1])*(TMath::Power([0]+1,[0]+1)/TMath::Gamma([0]+1))*TMath::Power(x/[1],[0])*TMath::Exp(-([0]+1)*x/[1])",0,3000000);
  float gain =  CLHEP::RandGamma::shoot(rndmEngine, 1. + m_theta, (1. + m_theta)/m_mean); // mean value for total gain due to E field; To calculate this gain from polya distibution we replace "alpha = 1+theta and beta = 1+theta/mean" in gamma PDF. With this gamma PDF gives us same sampling values as we get from polya PDF. 
  double total_charge = gain*ionized_charge; // total charge after avalanche

  //************************************ spread charge among readout element ************************************** 
  //spread charge to a gaussian distribution
  float charge_width = CLHEP::RandGaussZiggurat::shoot(rndmEngine, m_GausMean, m_GausSigma);
  float norm = 0.5*total_charge/(charge_width*std::sqrt(2.*M_PI)); // each readout plane reads about half the total charge produced on the wire
  std::unique_ptr<TF1> charge_spread = std::make_unique<TF1>("fgaus", "gaus(0)", -1000., 1000.); 
  charge_spread->SetParameters(norm, posOnSurf_strip.x(), charge_width);
  
  // Charge Spread including tan(theta) resolution term.
  double tan_theta = GLODIRE.perp()/GLODIRE.z();
  // The angle dependance on strip resolution goes as tan^2(angle)
  // We add the resolution in quadrature following sTGC test beam papers
  m_ChargeSpreadFactor = m_StripResolution*std::sqrt(1+12.*tan_theta*tan_theta);

  // Get the nominal strip width, which is 2.7 mm, while the strip pitch is 3.2 mm.
  double stripWidth = detEl->getDesign(newId)->inputWidth;

  // Lower limit on strip charge, in pC, which has the same units as the parameter ionized_charge
  double tolerance_charge = 0.0005;

  // Position of the strip closest to the hit
  Amg::Vector2D middleStrip_pos(0., 0.);
  detEl->stripPosition(newId, middleStrip_pos);
  
  // Determine the middle strips
  // Usually, the middle strip is the strip nearest to the hit.
  // If a hit is placed between two strips, then these two strips 
  // are considered as middle strips.
  int middleStrip[2] = {0, 0};
  double hitRelativeLocation = posOnSurf_strip.x() - middleStrip_pos.x();
  if (std::abs(hitRelativeLocation) < (stripWidth/2)) {
    // Only one middle strip, so add the same strip number in the array
    middleStrip[0] = stripNumber;
    middleStrip[1] = stripNumber;
  } else if (hitRelativeLocation < 0.0) {
    middleStrip[0] = stripNumber-1;
    middleStrip[1] = stripNumber;
  } else if (hitRelativeLocation > 0.0) {
    middleStrip[0] = stripNumber;
    middleStrip[1] = stripNumber+1;
  }

  /* While-loop to spread charge on the strips around the hit
   * The strips adjacent to the hit are considered one by one until the charge 
   * to be spread on the strip is below a tolerance. 
   */
  unsigned int neighbor = 0;
  // Flags to stop spreading charge on neighbour strips
  bool createNeighbor1 = true;
  bool createNeighbor2 = true;
  unsigned int counter_strip = 0;
  // Set a maximum number of neighbour strips to avoid very long loop
  const unsigned int max_neighbor = 10;
  while (createNeighbor1 || createNeighbor2) {
    // Strip numbers to be considered  
    std::vector<int> tmpStripNumbers;
    if (neighbor == 0) {
      tmpStripNumbers.push_back(stripNumber);
    } else if (neighbor > 0) {
      if (createNeighbor1) tmpStripNumbers.push_back(stripNumber - neighbor);
      if (createNeighbor2) tmpStripNumbers.push_back(stripNumber + neighbor);
    }

    // Terminate the loop if zero strip digits
    if (tmpStripNumbers.empty()) break;

    // Skip spreading charge on out-of-range strips 
    for (int stripnum: tmpStripNumbers) {
      // Verify if strip number is valid
      if(stripnum < 1) {
        createNeighbor1 = false;
        continue;
      }
      if(stripnum > NumberOfStrips) {
        createNeighbor2 = false;
        continue;
      }

      // Get the strip identifier and create the digit 
      newId = m_idHelper->channelID(m_idHelper->parentID(layid), multiPlet, gasGap, channelType, stripnum, isValid);
      if (isValid) {
        Amg::Vector2D locpos(0., 0.);
        if (!detEl->stripPosition(newId, locpos)) {
          ATH_MSG_WARNING("Failed to obtain local position for identifier " << m_idHelper->print_to_string(newId) );
        }

        // Estimate the digit charge
        double xmax = locpos.x() + stripHalfPitch;
        double xmin = locpos.x() - stripHalfPitch;
        double charge = charge_spread->Integral(xmin, xmax);
        // If charge is too small, stop creating neighbor strip
        if (charge < tolerance_charge) {
          if (stripnum <= middleStrip[0]) createNeighbor1 = false;
          if (stripnum >= middleStrip[1]) createNeighbor2 = false;
          continue;
        }
        charge = CLHEP::RandGaussZiggurat::shoot(rndmEngine, charge, m_ChargeSpreadFactor*charge);

        // Estimate digit time
        int indexFromMiddleStrip = std::abs(stripnum - middleStrip[0]);
        if (std::abs(stripnum - middleStrip[1]) < indexFromMiddleStrip) {
          indexFromMiddleStrip = std::abs(stripnum - middleStrip[1]);
        }
        double strip_time = sDigitTimeStrip;
        // Strip time response can be delayed due to the resistive layer. 
        // A correction would be required if the actual VMM front-end doesn't re-align the strip timing.
        if (m_doTimeOffsetStrip) {
          strip_time += getTimeOffsetStrip(indexFromMiddleStrip);
        }
      
        addDigit(digits.get(),newId, bctag, strip_time, charge, channelType);

        ATH_MSG_VERBOSE("Created a strip digit: strip number = " << stripnum << ", charge = " << charge 
                        << ", time = " << strip_time << ", time offset = " << strip_time-sDigitTimeStrip 
                        << ", neighbor index = " << neighbor
                        << ", charge stdev = " << m_ChargeSpreadFactor
                        << ", strip position = (" << locpos.x() << "," << locpos.y() << ")");

        ++counter_strip;
      }
    }

    // Increment neighbor parameter
    ++neighbor;
    if (neighbor > max_neighbor) break;
  }
  ATH_MSG_DEBUG("Number of strip digits created = " << counter_strip);

  // end of strip digitization

  if(m_channelTypes==1) {
    ATH_MSG_WARNING("Only digitize strip response !");
    return digits;
  }

  //##################################################################################
  //######################################### pad readout ##########################
  //##################################################################################
  ATH_MSG_DEBUG("sTgcDigitMaker::pad response ");
  channelType = sTgcIdHelper::sTgcChannelTypes::Pad;
  
  //************************************ find the nearest readout element ************************************** 
  int  surfHash_pad =  detEl->surfaceHash(gasGap, 0);
  const Trk::PlaneSurface& SURF_PAD = detEl->surface(surfHash_pad); // get the pad surface

  const Amg::Vector3D hitOnSurface_pad = SURF_PAD.transform().inverse()*glob_ionization_pos;
  const Amg::Vector2D posOnSurf_pad(hitOnSurface_pad.x(), hitOnSurface_pad.y());

  Identifier PAD_ID = m_idHelper->channelID(m_idHelper->parentID(layid), multiPlet, gasGap, channelType, 1);// find the a pad id

  insideBounds = SURF_PAD.insideBounds(posOnSurf_pad);

  if(insideBounds) { 
    int padNumber = detEl->stripNumber(posOnSurf_pad, PAD_ID);
    if( padNumber == -1 ){
      // Verify if the energy deposit is at the boundary
      const float new_posX = (posOnSurf_pad.x()>0.0)? posOnSurf_pad.x()-length_tolerance
                                                    : posOnSurf_pad.x()+length_tolerance;
      const float new_posY = (posOnSurf_pad.y()>0.0)? posOnSurf_pad.y()-length_tolerance
                                                    : posOnSurf_pad.y()+length_tolerance;
      const Amg::Vector2D new_position(new_posX, new_posY);
      padNumber = detEl->stripNumber(new_position, PAD_ID);
      // Skip hit if still unable to obtain pad number
      if (padNumber < 1) {
        ATH_MSG_WARNING("Failed to obtain pad number " << m_idHelper->print_to_string(PAD_ID) );
        ATH_MSG_WARNING("Position on pad surface = (" << posOnSurf_pad.x() << ", " << posOnSurf_pad.y() << ")");
        return digits;
      }
    }  
    newId = m_idHelper->channelID(m_idHelper->parentID(layid), multiPlet, gasGap, channelType, padNumber, isValid);
    if(isValid) {  
      addDigit(digits.get(), newId, bctag, sDigitTimePad, channelType);
    }
    else if(padNumber != -1) {  
      ATH_MSG_ERROR("Failed to obtain identifier " << m_idHelper->print_to_string(newId) ); 
    }
  }
  else {
    ATH_MSG_DEBUG("Outside of the pad surface boundary :" << m_idHelper->print_to_string(PAD_ID)<< " local position " <<posOnSurf_pad ); 
  }
  
  if(m_channelTypes==2) {
    ATH_MSG_WARNING("Only digitize strip/pad response !");
    return digits;
  }
   
 
  //##################################################################################
  //######################################### wire readout ##########################
  //##################################################################################
  ATH_MSG_DEBUG("sTgcDigitMaker::wire response ");
  channelType = sTgcIdHelper::sTgcChannelTypes::Wire;

    // Find the ID of the first wiregroup
    Identifier WIREGP_ID = m_idHelper->channelID(m_idHelper->parentID(layid), multiPlet, gasGap, channelType, 1);
        
    //************************************ find the nearest readout element ************************************** 
    insideBounds = SURF_WIRE.insideBounds(posOnSurf_wire);

    if(insideBounds) {
        // Determine the wire number

        int wiregroupNumber = detEl->stripNumber(posOnSurf_wire, WIREGP_ID);
        if( wiregroupNumber == -1 ){
          // Verify if the energy deposit is at the boundary
          const float new_posX = (posOnSurf_wire.x() > 0.0)? posOnSurf_wire.x() - length_tolerance
                                                           : posOnSurf_wire.x() + length_tolerance;
          const Amg::Vector2D new_position(new_posX, posOnSurf_wire.y());
          wiregroupNumber = detEl->stripNumber(new_position, WIREGP_ID);
          // Skip hit if still unable to obtain pad number
          if (wiregroupNumber < 1) {
            ATH_MSG_WARNING("Failed to obtain wire number " << m_idHelper->print_to_string(WIREGP_ID) );
            ATH_MSG_WARNING("Position on wire surface = (" << posOnSurf_wire.x() << ", " << posOnSurf_wire.y() << ")");
            return digits;
          }
        }
  
        // Find ID of the actual wiregroup
        newId = m_idHelper->channelID(m_idHelper->parentID(layid), multiPlet, gasGap, channelType, wiregroupNumber, isValid);
  
        if(isValid) {
          int NumberOfWiregroups = detEl->numberOfStrips(newId);
          if(wiregroupNumber>=1&&wiregroupNumber<=NumberOfWiregroups) addDigit(digits.get(), newId, bctag, sDigitTimeWire, channelType);
        } // end of if(isValid)
        else if (wiregroupNumber != -1){
          ATH_MSG_ERROR("Failed to obtain wiregroup identifier " << m_idHelper->print_to_string(newId) );
        }
    }
    else {
      ATH_MSG_DEBUG("Outside of the wire surface boundary :" << m_idHelper->print_to_string(WIREGP_ID)<< " local position " <<posOnSurf_wire );
    }
    // end of wire digitization

  return digits;
}

//+++++++++++++++++++++++++++++++++++++++++++++++
double sTgcDigitMaker::distanceToWire(Amg::Vector3D& position, Amg::Vector3D& direction, Identifier id, int wire_number) const
{
  // Wire number should be one or greater
  if (wire_number < 1) {
    return -9.99;
  }

  // Get the current sTGC element (a four-layer chamber)
  const MuonGM::sTgcReadoutElement* detEl = m_mdManager->getsTgcReadoutElement(id);

  // Wire number too large
  if (wire_number > detEl->numberOfWires(id)) {
    return -9.99;
  }

  // Wire pitch
  double wire_pitch = detEl->wirePitch();
  // Wire local position on the wire plane, the y-coordinate is arbitrary and z-coordinate is zero
  double wire_posX = detEl->positionFirstWire(id) + (wire_number - 1) * wire_pitch;
  Amg::Vector3D wire_position(wire_posX, position.y(), 0.);
  // The wires are parallel to Y in the wire plane's coordinate frame
  Amg::Vector3D wire_direction(0., 1., 0.);

  // Determine the sign of the distance, which is: 
  //  - negative if particle crosses the wire surface on the wire_number-1 side and 
  //  + positive if particle crosses the wire surface on the wire_number+1 side
  double sign = 1.0;
  if ((position.x() - wire_posX) < 0.) {
    sign = -1.0;
  }

  // Distance of closest approach is the distance between the two lines: 
  //      - particle's segment
  //      - wire line

  // Find a line perpendicular to both hit direction and wire direction
  Amg::Vector3D perp_line = direction.cross(wire_direction);
  double norm_line = std::sqrt(perp_line.dot(perp_line));
  if (norm_line < 1.0e-5) {
    ATH_MSG_DEBUG("Unable to compute the distance of closest approach," 
                    << " a negative value is assumed to indicate the error.");
    return -9.99;
  }
  // Compute the distance of closest approach, which is given by the projection of 
  // the vector going from hit position to wire position onto the perpendicular line
  double distance = std::abs((position - wire_position).dot(perp_line) / norm_line);
     
  return (sign * distance);
}

//+++++++++++++++++++++++++++++++++++++++++++++++
sTgcDigitMaker::Ionization sTgcDigitMaker::pointClosestApproach(const Identifier& id, int wireNumber,
                                                                Amg::Vector3D& preStepPos,
                                                                Amg::Vector3D& postStepPos
                                                                ) const
{
  // tolerance in mm
  constexpr float tolerance = 0.001;
  // minimum segment length
  constexpr float min_length = 0.1;

  // Position of the ionization
  Ionization ionization;

  // Wire number should be one or greater
  if (wireNumber < 1) return ionization;

  // Get the current sTGC element (a four-layer chamber)
  const MuonGM::sTgcReadoutElement* detEl = m_mdManager->getsTgcReadoutElement(id);

  // Wire number too large
  if (wireNumber > detEl->numberOfWires(id)) return ionization;

  // Finding smallest distance and the points at the smallest distance.
  //  The smallest distance between two lines is perpendicular to both lines.
  //  We can construct two lines in the wire surface local coordinate frame: 
  //  - one for the hit segment with equation h0 + t * v_h, where h0 is a point 
  //    and v_h is the unit vector of the hit segment
  //  - another for the wire with similar equation w0 + s * v_w, where w0 is a
  //    point and v_w is the unit vector of the wire line
  //  Then it is possible to determine the factors s and t by requiring the 
  //  dot product to be zero:
  //   1. (h0 - w0) \dot v_h = 0
  //   2. (h0 - w0) \dot v_w = 0

  // Wire pitch
  const double wire_pitch = detEl->wirePitch();
  // Wire local position on the wire plane, the y-coordinate is arbitrary and z-coordinate is zero
  const double wire_posX = detEl->positionFirstWire(id) + (wireNumber - 1) * wire_pitch;
  const Amg::Vector3D wire_position(wire_posX, postStepPos.y(), 0.);
  // The wires are parallel to Y in the wire plane's coordinate frame
  const Amg::Vector3D wire_direction(0., 1., 0.);

  // particle trajectory
  Amg::Vector3D hit_direction(postStepPos.x() - preStepPos.x(),
                              postStepPos.y() - preStepPos.y(), 
                              postStepPos.z() - preStepPos.z());
  const double seg_length = hit_direction.mag();
  if (seg_length > tolerance) hit_direction /= seg_length;

  // Find the point on the track segment that is closest to the wire
  if (seg_length < min_length) {
    ionization.posOnSegment = postStepPos;
    ionization.posOnWire = wire_position;
    ionization.distance = std::hypot(postStepPos.x() - wire_posX, postStepPos.z());
    return ionization;
  }

  // Dot product between the wire and hit direction
  const double cos_theta = wire_direction.dot(hit_direction);
  // distance between the hit and wire
  const Amg::Vector3D dist_wire_hit = postStepPos - wire_position;

  // Verifier the special case where the two lines are parallel
  if (std::abs(cos_theta - 1.0) < tolerance) {
    ATH_MSG_DEBUG("The track segment is parallel to the wire, position of digit is undefined");
    ionization.posOnSegment = 0.5 * (postStepPos + preStepPos);
    ionization.posOnWire = Amg::Vector3D(wire_posX, ionization.posOnSegment.y(), 0.0);
    ionization.distance = std::hypot(ionization.posOnSegment.x() - wire_posX, 
                                     ionization.posOnSegment.z());
    return ionization;
  }

  // Perpendicular component squared
  const double sin_theta_2 = 1.0 - cos_theta * cos_theta;

  //* Point on the hit segment
  const float factor_hit = (-dist_wire_hit.dot(hit_direction) 
                           + dist_wire_hit.dot(wire_direction) * cos_theta) / sin_theta_2;
  Amg::Vector3D ionization_pos = postStepPos + factor_hit * hit_direction;

  // If the point is on the track segment, then compute the other point on the wire.
  // Otherwise, set the ionization at the pre-step position and compute where it 
  // should be on the wire.
  Amg::Vector3D pos_on_wire(0., 0., 0.);
  if (factor_hit < seg_length) {
    //* Point on the wire line
    const float factor_wire = (dist_wire_hit.dot(wire_direction)
                              - dist_wire_hit.dot(hit_direction) * cos_theta) / sin_theta_2;
    pos_on_wire = wire_position + factor_wire * wire_direction;
  } else {
    ionization_pos = preStepPos;
    pos_on_wire = wire_position - (seg_length * cos_theta) * wire_direction;
  }

  // Save the distance and ionization position
  ionization.distance = (ionization_pos - pos_on_wire).mag();
  ionization.posOnSegment = ionization_pos;
  ionization.posOnWire = pos_on_wire;

  return ionization;
};

//+++++++++++++++++++++++++++++++++++++++++++++++
bool sTgcDigitMaker::efficiencyCheck(const std::string& stationName, const int stationEta, const int stationPhi,const int multiPlet, const int gasGap, const int channelType, const double energyDeposit) const {
  // If the energy deposit is equal to or greater than the threshold value of the chamber,
  // return true.
  return (energyDeposit >= getEnergyThreshold(stationName, stationEta, stationPhi, multiPlet, gasGap, channelType));
}
//+++++++++++++++++++++++++++++++++++++++++++++++
//uint16_t sTgcDigitMaker::bcTagging(const float digitTime, const int channelType) const {
//
//  uint16_t bctag = 0;
//
//  double offset, window;
//  if(channelType == 1) { //strips 
//    offset = m_timeWindowOffsetStrip;
//    window = m_timeWindowStrip;
//  }
//  else { // wire gangs or pads
//    offset = m_timeWindowOffsetPad;
//    window = m_timeWindowPad;
//  }
//
//  if(-m_bunchCrossingTime+offset < digitTime && digitTime < -m_bunchCrossingTime+offset+window) {
//    bctag = (bctag | 0x1);
//  }
//  if(                     offset < digitTime && digitTime <                      offset+window) {
//    bctag = (bctag | 0x2);
//  }
//  if(+m_bunchCrossingTime+offset < digitTime && digitTime < +m_bunchCrossingTime+offset+window) {
//    bctag = (bctag | 0x4);
//  }
//
//  return bctag;
//}
//+++++++++++++++++++++++++++++++++++++++++++++++
void sTgcDigitMaker::addDigit(sTgcDigitCollection* digits, const Identifier id, const uint16_t bctag, const float digittime, int channelType) const {

  if((channelType!=sTgcIdHelper::sTgcChannelTypes::Pad) && (channelType!=sTgcIdHelper::sTgcChannelTypes::Wire)) {
    ATH_MSG_WARNING("Wrong sTgcDigit object with channelType" << channelType );
  }

  bool duplicate = false;
  for(sTgcDigitCollection::const_iterator it=digits->begin(); it!=digits->end(); ++it) {
    if(id==(*it)->identify() && digittime==(*it)->time()) {
      duplicate = true;
      break;
    }
  }
  if(!duplicate) {
    digits->push_back(new sTgcDigit(id, bctag, digittime, -1, 0, 0));
  }

  return;
}

void sTgcDigitMaker::addDigit(sTgcDigitCollection* digits, const Identifier id, const uint16_t bctag, const float digittime, float charge, int channelType) const {

  if(channelType!=sTgcIdHelper::sTgcChannelTypes::Strip) {
    ATH_MSG_WARNING("Wrong sTgcDigit object with channelType" << channelType );
  }

  bool duplicate = false;
  for(sTgcDigitCollection::iterator it=digits->begin(); it!=digits->end(); ++it) {
    if(id==(*it)->identify() && digittime==(*it)->time()) {
      (*it)->set_charge(charge+(*it)->charge());  
      duplicate = true;
      break;
    }
  }
  if(!duplicate) {
    digits->push_back(new sTgcDigit(id, bctag, digittime, charge, 0, 0));
  }

  return;
}

//+++++++++++++++++++++++++++++++++++++++++++++++
StatusCode sTgcDigitMaker::readFileOfEnergyThreshold() {
  // Indices to be used
  int iStationName, stationEta, stationPhi, multiPlet, gasGap, channelType;

  for(iStationName=0; iStationName<N_STATIONNAME; iStationName++) {
    for(stationEta=0; stationEta<N_STATIONETA; stationEta++) {
      for(stationPhi=0; stationPhi<N_STATIONPHI; stationPhi++) {
        for(multiPlet=0; multiPlet<N_MULTIPLET; multiPlet++) {
          for(gasGap=0; gasGap<N_GASGAP; gasGap++) {
            for(channelType=0; channelType<N_CHANNELTYPE; channelType++) {
              m_energyThreshold[iStationName][stationEta][stationPhi][multiPlet][gasGap][channelType] = 0.00052; //MeV
            }
          }
        }
      }
    }
  }

  // Find path to the sTGC_Digitization_energyThreshold.dat file
  const std::string fileName = "sTGC_Digitization_energyThreshold.dat";
  std::string fileWithPath = PathResolver::find_file(fileName.c_str(), "DATAPATH");
  if(fileWithPath.empty()) {
    ATH_MSG_FATAL("readFileOfEnergyThreshold(): Could not find file " << fileName.c_str() );
    return StatusCode::FAILURE;
  }

  // Open the sTGC_Digitization_energyThreshold.dat file
  std::ifstream ifs;
  ifs.open(fileWithPath.c_str(), std::ios::in);
  if(ifs.bad()) {
    ATH_MSG_FATAL("readFileOfEnergyThreshold(): Could not open file " << fileName.c_str() );
    return StatusCode::FAILURE;
  }

  double energyThreshold;
  // Read the sTGC_Digitization_energyThreshold.dat file
  while(ifs.good()&& !ifs.eof() ) {
    ifs >> iStationName >> stationEta
        >> stationPhi   >> multiPlet
        >> gasGap >> channelType >> energyThreshold;
    ATH_MSG_DEBUG(  "sTgcDigitMaker::readFileOfEnergyThreshold"
                    << " stationName= " << iStationName
                    << " stationEta= " << stationEta
                    << " stationPhi= " << stationPhi
                    << " multiPlet= " << multiPlet
                    << " gasGap= " << gasGap
                    << " channelType= " << channelType
                    << " energyThreshold(MeV)= " << energyThreshold );

    // Subtract offsets to use indices of energyThreshold array
    iStationName -= OFFSET_STATIONNAME;
    stationEta   -= OFFSET_STATIONETA;
    stationPhi   -= OFFSET_STATIONPHI;
    multiPlet    -= OFFSET_MULTIPLET;
    gasGap       -= OFFSET_GASGAP;
    channelType  -= OFFSET_CHANNELTYPE;

    // Check the indices are valid
    if(iStationName<0 || iStationName>=N_STATIONNAME) continue;
    if(stationEta  <0 || stationEta  >=N_STATIONETA ) continue;
    if(stationPhi  <0 || stationPhi  >=N_STATIONPHI ) continue;
    if(multiPlet   <0 || multiPlet   >=N_MULTIPLET  ) continue;
    if(gasGap      <0 || gasGap      >=N_GASGAP     ) continue;
    if(channelType <0 || channelType >=N_CHANNELTYPE    ) continue;

    m_energyThreshold[iStationName][stationEta][stationPhi][multiPlet][gasGap][channelType] = energyThreshold;

    // If it is the end of the file, get out from while loop.
    if(ifs.eof()) break;
  }

  // Close the sTGC_Digitization_energyThreshold.dat file
  ifs.close();
  return StatusCode::SUCCESS;
}

//+++++++++++++++++++++++++++++++++++++++++++++++
StatusCode sTgcDigitMaker::readFileOfDeadChamber() {
  // Indices to be used
  int iStationName, stationEta, stationPhi, multiPlet, gasGap;

  for(iStationName=0; iStationName<N_STATIONNAME; iStationName++) {
    for(stationEta=0; stationEta<N_STATIONETA; stationEta++) {
      for(stationPhi=0; stationPhi<N_STATIONPHI; stationPhi++) {
        for(multiPlet=0; multiPlet<N_MULTIPLET; multiPlet++) {
          for(gasGap=0; gasGap<N_GASGAP; gasGap++) {
            m_isDeadChamber[iStationName][stationEta][stationPhi][multiPlet][gasGap] = false;
          }
        } 
      }
    }
  }

  // Find path to the sTGC_Digitization_deadChamber.dat file
  const std::string fileName = "sTGC_Digitization_deadChamber.dat";
  std::string fileWithPath = PathResolver::find_file(fileName.c_str(), "DATAPATH");
  if(fileWithPath.empty()) {
    ATH_MSG_FATAL("readFileOfDeadChamber(): Could not find file " << fileName.c_str() );
    return StatusCode::FAILURE;
  }

  // Open the sTGC_Digitization_deadChamber.dat file
  std::ifstream ifs;
  ifs.open(fileWithPath.c_str(), std::ios::in);
  if(ifs.bad()) {
    ATH_MSG_FATAL("readFileOfDeadChamber(): Could not open file " << fileName.c_str() );
    return StatusCode::FAILURE;
  }

  // Read the sTGC_Digitization_deadChamber.dat file
  unsigned int nDeadChambers = 0;
  std::string comment;
  while(ifs.good()) {
    ifs >> iStationName >> stationEta >> stationPhi >> multiPlet >> gasGap;
    bool valid = getline(ifs, comment).good();
    if(!valid) break;

    ATH_MSG_DEBUG( "sTgcDigitMaker::readFileOfDeadChamber"
                    << " stationName= " << iStationName
                    << " stationEta= " << stationEta
                    << " stationPhi= " << stationPhi
                    << " multiPlet= "  << multiPlet 
                    << " gasGap= " << gasGap
                    << " comment= " << comment );

    // Subtract offsets to use indices of isDeadChamber array
    iStationName -= OFFSET_STATIONNAME;
    stationEta   -= OFFSET_STATIONETA;
    stationPhi   -= OFFSET_STATIONPHI;
    multiPlet    -= OFFSET_MULTIPLET;
    gasGap       -= OFFSET_GASGAP;

    // Check the indices are valid
    if(iStationName<0 || iStationName>=N_STATIONNAME) continue;
    if(stationEta  <0 || stationEta  >=N_STATIONETA ) continue;
    if(stationPhi  <0 || stationPhi  >=N_STATIONPHI ) continue;
    if(multiPlet   <0 || multiPlet   >=N_MULTIPLET  ) continue;
    if(gasGap      <0 || gasGap      >=N_GASGAP     ) continue;

    m_isDeadChamber[iStationName][stationEta][stationPhi][multiPlet][gasGap] = true;
    nDeadChambers++;

    // If it is the end of the file, get out from while loop.
    if(ifs.eof()) break;
  }

  // Close the sTGC_Digitization_deadChamber.dat file
  ifs.close();

  ATH_MSG_VERBOSE("sTgcDigitMaker::readFileOfDeadChamber: the number of dead chambers = " << nDeadChambers );
  return StatusCode::SUCCESS;
}

StatusCode sTgcDigitMaker::readFileOfEffChamber() {
  // Indices to be used
  int iStationName, stationEta, stationPhi, multiPlet, gasGap;
  float eff;

  for(iStationName=0; iStationName<2; iStationName++) { // Small - Large
    for(stationEta=0; stationEta<4; stationEta++) { // 4 eta
      for(stationPhi=0; stationPhi<8; stationPhi++) { // 8 phi sectors
        for(multiPlet=0; multiPlet<2; multiPlet++) { // pivot- confirm
          for(gasGap=0; gasGap<4; gasGap++) { // 4 layers
            m_ChamberEfficiency[iStationName][stationEta][stationPhi][multiPlet][gasGap] = 1.;
          }
        } 
      }
    }
  }

  // Find path to the sTGC_Digitization_EffChamber.dat file
  const std::string fileName = "sTGC_Digitization_EffChamber.dat";
  std::string fileWithPath = PathResolver::find_file(fileName.c_str(), "DATAPATH");
  if(fileWithPath.empty()) {
    ATH_MSG_FATAL("readFileOfEffChamber(): Could not find file " << fileName.c_str() );
    return StatusCode::FAILURE;
  }

  // Open the sTGC_Digitization_EffChamber.dat file
  std::ifstream ifs;
  ifs.open(fileWithPath.c_str(), std::ios::in);
  if(ifs.bad()) {
    ATH_MSG_FATAL("readFileOfEffChamber(): Could not open file " << fileName.c_str() );
    return StatusCode::FAILURE;
  }

  // Read the sTGC_Digitization_EffChamber.dat file
  /* Each line has 6 values:
     value #1 : Large (0) or Small (1)
     value #2 : Eta 0,1,2,3 : Mirroring A side efficiency to C side for now.
     value #3 : Phi 1 to 8
     value #4 : Multiplet 0 for Large pivot or small confirm.
                Multiplet 1 for Large Confirm or Small Pivot
     value #5 : gasGap (1-4)
     value #6 : Efficiency
  */
  unsigned int nDeadChambers = 0;
  std::string comment;
  // This is just to skip the first line which describes the format
  getline(ifs, comment);
  while(ifs.good()) {
    ifs >> iStationName >> stationEta >> stationPhi >> multiPlet >> gasGap >> eff;
    bool valid = getline(ifs, comment).good();
    if(!valid) break;

    ATH_MSG_DEBUG( "sTgcDigitMaker::readFileOfEffChamber"
                    << " stationName= " << iStationName
                    << " stationEta= " << stationEta
                    << " stationPhi= " << stationPhi
                    << " multiPlet= "  << multiPlet
                    << " gasGap= " << gasGap
                    << " efficiency= " << eff
                    << " comment= " << comment );

    // Subtract offsets to use indices of efficiency array
    stationPhi = stationPhi - 1;
    gasGap = gasGap - 1;

    // Check the indices are valid
    if(iStationName<0 || iStationName>=2) continue;
    if(stationEta  <0 || stationEta  >=4 ) continue;
    if(stationPhi  <0 || stationPhi  >=8 ) continue;
    if(multiPlet   <0 || multiPlet   >=2  ) continue;
    if(gasGap      <0 || gasGap      >=4     ) continue;

    m_ChamberEfficiency[iStationName][stationEta][stationPhi][multiPlet][gasGap] = eff;
    if (eff==0) nDeadChambers++;

    // If it is the end of the file, get out from while loop.
    if(ifs.eof()) break;
  }

  // Close the sTGC_Digitization_deadChamber.dat file
  ifs.close();

  ATH_MSG_VERBOSE("sTgcDigitMaker::readFileOfEffChamber: the number of dead chambers = " << nDeadChambers );
  return StatusCode::SUCCESS;
}
//+++++++++++++++++++++++++++++++++++++++++++++++
StatusCode sTgcDigitMaker::readFileOfTimeWindowOffset() {
  // Indices to be used
  int iStationName, stationEta, channelType;

  for(iStationName=0; iStationName<N_STATIONNAME; iStationName++) {
    for(stationEta=0; stationEta<N_STATIONETA; stationEta++) {
      for(channelType=0; channelType<N_CHANNELTYPE; channelType++) {
        m_timeWindowOffset[iStationName][stationEta][channelType] = 0.;
      }
    }
  }

  // Find path to the sTGC_Digitization_timeWindowOffset.dat file
  const std::string fileName = "sTGC_Digitization_timeWindowOffset.dat";
  std::string fileWithPath = PathResolver::find_file(fileName.c_str(), "DATAPATH");
  if(fileWithPath.empty()) {
    ATH_MSG_FATAL("readFileOfTimeWindowOffset(): Could not find file " << fileName.c_str() );
    return StatusCode::FAILURE;
  }

  // Open the sTGC_Digitization_timeWindowOffset.dat file
  std::ifstream ifs;
  ifs.open(fileWithPath.c_str(), std::ios::in);
  if(ifs.bad()) {
    ATH_MSG_FATAL("readFileOfTimeWindowOffset(): Could not open file " << fileName.c_str() );
    return StatusCode::FAILURE;
  }

  // Read the sTGC_Digitization_timeWindowOffset.dat file
  double timeWindowOffset;
  while(ifs.good()) {
    ifs >> iStationName >> stationEta >> channelType >> timeWindowOffset;
    ATH_MSG_DEBUG(  "sTgcDigitMaker::readFileOfTimeWindowOffset"
                    << " stationName= " << iStationName
                    << " stationEta= " << stationEta
                    << " channelType= " << channelType
                    << " timeWindowOffset= " << timeWindowOffset );

    // Subtract offsets to use indices of isDeadChamber array
    iStationName -= OFFSET_STATIONNAME;
    stationEta   -= OFFSET_STATIONETA;
    channelType  -= OFFSET_CHANNELTYPE;

    // Check the indices are valid
    if(iStationName<0 || iStationName>=N_STATIONNAME) continue;
    if(stationEta  <0 || stationEta  >=N_STATIONETA ) continue;
    if(channelType     <0 || channelType     >=N_CHANNELTYPE    ) continue;

    m_timeWindowOffset[iStationName][stationEta][channelType] = timeWindowOffset;

    // If it is the end of the file, get out from while loop.
    if(ifs.eof()) break;
  }

  // Close the sTGC_Digitization_timeWindowOffset.dat file
  ifs.close();
  return StatusCode::SUCCESS;
}

//+++++++++++++++++++++++++++++++++++++++++++++++
double sTgcDigitMaker::getEnergyThreshold(const std::string& stationName, int stationEta, int stationPhi, int multiPlet, int gasGap, int channelType) const {
  // Convert std::string stationName to int iStationName from 41 to 48
  int iStationName = getIStationName(stationName);

  // Subtract offsets to use these as the indices of the energyThreshold array
  iStationName -= OFFSET_STATIONNAME;
  stationEta   -= OFFSET_STATIONETA;
  stationPhi   -= OFFSET_STATIONPHI;
  multiPlet    -= OFFSET_MULTIPLET;
  gasGap       -= OFFSET_GASGAP;
  channelType  -= OFFSET_CHANNELTYPE;

  double energyThreshold = +999999.;

  // If the indices are valid, the energyThreshold array is fetched.
  if((iStationName>=0 && iStationName<N_STATIONNAME) &&
     (stationEta  >=0 && stationEta  <N_STATIONETA ) &&
     (stationPhi  >=0 && stationPhi  <N_STATIONPHI ) &&
     (multiPlet   >=0 && multiPlet   <N_MULTIPLET ) &&
     (gasGap      >=0 && gasGap      <N_GASGAP     ) &&
     (channelType >=0 && channelType <N_CHANNELTYPE    )) {
    energyThreshold = m_energyThreshold[iStationName][stationEta][stationPhi][multiPlet][gasGap][channelType];
  }

  // Show the energy threshold value
  ATH_MSG_VERBOSE( "sTgcDigitMaker::getEnergyThreshold"
                    << " stationName= " << iStationName+OFFSET_STATIONNAME
                    << " stationEta= " << stationEta+OFFSET_STATIONETA
                    << " stationPhi= " << stationPhi+OFFSET_STATIONPHI
                    << " multiPlet= " << multiPlet+OFFSET_MULTIPLET
                    << " gasGap= "      << gasGap+OFFSET_GASGAP
                    << " channelType= " << channelType+OFFSET_CHANNELTYPE
                    << " energyThreshold(MeV)= " << energyThreshold );

  return energyThreshold;
}

//+++++++++++++++++++++++++++++++++++++++++++++++
bool sTgcDigitMaker::isDeadChamber(const std::string& stationName, int stationEta, int stationPhi, int multiPlet, int gasGap) {
  bool v_isDeadChamber = true;

  // Convert std::string stationName to int iStationName from 41 to 48
  int iStationName = getIStationName(stationName);

  // Subtract offsets to use these as the indices of the energyThreshold array
  iStationName -= OFFSET_STATIONNAME;
  stationEta   -= OFFSET_STATIONETA;
  stationPhi   -= OFFSET_STATIONPHI;
  multiPlet    -= OFFSET_MULTIPLET;
  gasGap       -= OFFSET_GASGAP;

  // If the indices are valid, the energyThreshold array is fetched.
  if((iStationName>=0 && iStationName<N_STATIONNAME) &&
     (stationEta  >=0 && stationEta  <N_STATIONETA ) &&
     (stationPhi  >=0 && stationPhi  <N_STATIONPHI ) &&
     (multiPlet   >=0 && multiPlet   <N_MULTIPLET  ) && 
     (gasGap      >=0 && gasGap      <N_GASGAP     )) {
    v_isDeadChamber = m_isDeadChamber[iStationName][stationEta][stationPhi][multiPlet][gasGap];
  }

  // Show the energy threshold value
  ATH_MSG_VERBOSE(  "sTgcDigitMaker::isDeadChamber"
                    << " stationName= " << iStationName+OFFSET_STATIONNAME
                    << " stationEta= " << stationEta+OFFSET_STATIONETA
                    << " stationPhi= " << stationPhi+OFFSET_STATIONPHI
                    << " multiPlet = " << multiPlet +OFFSET_MULTIPLET
                    << " gasGap= " << gasGap+OFFSET_GASGAP
                    << " isDeadChamber= " << v_isDeadChamber );

  return v_isDeadChamber;
}

//+++++++++++++++++++++++++++++++++++++++++++++++
float sTgcDigitMaker::getChamberEfficiency(int stationName, int stationEta, int stationPhi, int multiPlet, int gasGap) {

  // If the indices are valid, the energyThreshold array is fetched.
  if((stationName>=0 && stationName<2 ) &&
     (stationEta  >=0 && stationEta<4 ) &&
     (stationPhi  >=0 && stationPhi<8 ) &&
     (multiPlet   >=0 && multiPlet<2  ) &&
     (gasGap      >=0 && gasGap<4     )) {
    return m_ChamberEfficiency[stationName][stationEta][stationPhi][multiPlet][gasGap];
  }
  else ATH_MSG_INFO("sTGC getChamberEfficiency bug! Indexes not ok!");

  return 1.;
}

//+++++++++++++++++++++++++++++++++++++++++++++++
double sTgcDigitMaker::getTimeWindowOffset(const std::string& stationName, int stationEta, int channelType) const {
  // Convert std::string stationName to int iStationName from 41 to 48
  int iStationName = getIStationName(stationName);

  // Subtract offsets to use these as the indices of the energyThreshold array
  iStationName -= OFFSET_STATIONNAME;
  stationEta   -= OFFSET_STATIONETA;
  channelType  -= OFFSET_CHANNELTYPE;

  // Check the indices are valid
  if(iStationName<0 || iStationName >=N_STATIONNAME) return 0.;
  if(stationEta  <0 || stationEta   >=N_STATIONETA ) return 0.;
  if(channelType <0 || channelType  >=N_CHANNELTYPE    ) return 0.;

  // Values were determined to reproduce the fraction of Previous BC hit fraction in Z->mumu data during Periods B,D,E in 2011.
  return m_timeWindowOffset[iStationName][stationEta][channelType];
}

int sTgcDigitMaker::getIStationName(const std::string& stationName) const {
  int iStationName = 0;
  if(     stationName=="STS") iStationName = 0;
  else if(stationName=="STL") iStationName = 1;

  return iStationName;
}

//+++++++++++++++++++++++++++++++++++++++++++++++
StatusCode sTgcDigitMaker::readFileOfTimeArrival() {
  // Verify the file sTGC_Digitization_timeArrival.dat exists
  const std::string file_name = "sTGC_Digitization_timeArrival.dat";
  std::string file_path = PathResolver::find_file(file_name.c_str(), "DATAPATH");
  if(file_path.empty()) {
    ATH_MSG_FATAL("readFileOfTimeWindowOffset(): Could not find file " << file_name.c_str() );
    return StatusCode::FAILURE;
  }

  // Open the sTGC_Digitization_timeArrival.dat file
  std::ifstream ifs;
  ifs.open(file_path.c_str(), std::ios::in);
  if(ifs.bad()) {
    ATH_MSG_FATAL("sTgcDigitMaker: Failed to open time of arrival file " << file_name.c_str() );
    return StatusCode::FAILURE;
  }

  // Read the sTGC_Digitization_timeWindowOffset.dat file
  std::string line;
  GammaParameter param{};

  while (std::getline(ifs, line)) {
    std::string key;
    std::istringstream iss(line);
    iss >> key;
    if (key.compare("bin") == 0) {
      iss >> param.lowEdge >> param.kParameter >> param.thetaParameter;
      m_gammaParameter.push_back(param);
    } else if (key.compare("mpv") == 0)  {
      double mpt;
      while (iss >> mpt) {m_mostProbableArrivalTime.push_back(mpt);}
    }
  }

  // Close the file
  ifs.close();
  return StatusCode::SUCCESS;
}

//+++++++++++++++++++++++++++++++++++++++++++++++
sTgcDigitMaker::GammaParameter sTgcDigitMaker::getGammaParameter(double distance) const {

  double d = distance;
  if (d < 0.) {
    ATH_MSG_DEBUG("getGammaParameter: expecting a positive distance, but got a negative value: " << d
                     << ". Proceed to the calculation using its absolute value.");
    d = -1.0 * d;
  }

  // Find the parameters assuming the container is sorted in ascending order of 'lowEdge'
  int index{-1};
  for (auto& par: m_gammaParameter) {
    if (d < par.lowEdge) {
      break;
    }
    ++index;
  }
  return m_gammaParameter.at(index);
}

//+++++++++++++++++++++++++++++++++++++++++++++++
double sTgcDigitMaker::getMostProbableArrivalTime(double distance) const {

  double d = distance;
  if (d < 0.) {
    ATH_MSG_DEBUG("getMostProbableArrivalTime: expecting a positive distance, but got a negative value: " << d
                     << ". Proceed to the calculation using its absolute value.");
    d = -1.0 * d;
  }

  double mpt = m_mostProbableArrivalTime.at(0) 
               + m_mostProbableArrivalTime.at(1) * d
               + m_mostProbableArrivalTime.at(2) * d * d 
               + m_mostProbableArrivalTime.at(3) * d * d * d
               + m_mostProbableArrivalTime.at(4) * d * d * d * d;
  return mpt;
}

//+++++++++++++++++++++++++++++++++++++++++++++++
StatusCode sTgcDigitMaker::readFileOfTimeOffsetStrip() {
  // Verify the file sTGC_Digitization_timeOffsetStrip.dat exists
  const std::string file_name = "sTGC_Digitization_timeOffsetStrip.dat";
  std::string file_path = PathResolver::find_file(file_name.c_str(), "DATAPATH");
  if(file_path.empty()) {
    ATH_MSG_FATAL("readFileOfTimeWindowOffset(): Could not find file " << file_name.c_str() );
    return StatusCode::FAILURE;
  }

  // Open the sTGC_Digitization_timeOffsetStrip.dat file
  std::ifstream ifs;
  ifs.open(file_path.c_str(), std::ios::in);
  if(ifs.bad()) {
    ATH_MSG_FATAL("sTgcDigitMaker: Failed to open time of arrival file " << file_name.c_str() );
    return StatusCode::FAILURE;
  }

  // Initialize the container to store the time offset.
  // The number of parameters, 6, corresponds to the number of lines to be read 
  // from sTGC_Digitization_timeOffsetStrip.dat.
  // Setting the default offset to 0 ns.
  const int N_PAR = 6;
  m_timeOffsetStrip.resize(N_PAR, 0.0);

  // Read the input file
  std::string line;
  int index{0};
  double value{0.0};
  while (std::getline(ifs, line)) {
    std::string key;
    std::istringstream iss(line);
    iss >> key;
    if (key.compare("strip") == 0) {
      iss >> index >> value;
      if ((index < 0) || (index >= N_PAR)) continue;
      m_timeOffsetStrip.at(index) = value;
    }
  }

  // Close the file
  ifs.close();
  return StatusCode::SUCCESS;
}

//+++++++++++++++++++++++++++++++++++++++++++++++
double sTgcDigitMaker::getTimeOffsetStrip(int neighbor_index) const {
  if ((!m_timeOffsetStrip.empty()) && (neighbor_index >= 0)) {
    // Return the last element if out of range
    if (neighbor_index >= static_cast<int>(m_timeOffsetStrip.size()) ) {
      return m_timeOffsetStrip.back();
    }
    return m_timeOffsetStrip.at(neighbor_index);
  } else {
    ATH_MSG_DEBUG("either attempting to get strip's time offset with negative "
                  "neighbor index," << neighbor_index 
                  << ", or time offset container is empty: " << m_timeOffsetStrip.size()
                  << ". Returning an offset of 0 ns.");
  }
  return 0.0;
}

////+++++++++++++++++++++++++++++++++++++++++++++++
//void sTgcDigitMaker::adHocPositionShift(const std::string stationName, int stationEta, int stationPhi,
//                                       const Amg::Vector3D direCos, Amg::Vector3D &localPos) const {
//  int iStationName = getIStationName(stationName);
//  iStationName -= OFFSET_STATIONNAME;
//  stationEta   -= OFFSET_STATIONETA;
//  stationPhi   -= OFFSET_STATIONPHI;
//  // Check the indices are valid
//  if(iStationName<0 || iStationName>=N_STATIONNAME) return;
//  if(stationEta  <0 || stationEta  >=N_STATIONETA ) return;
//  if(stationPhi  <0 || stationPhi  >=N_STATIONPHI ) return;
//
//  // Local +x (-x) direction is global +z direction on A-side (C-side).
//  double localDisplacementX = m_alignmentT[iStationName][stationEta][stationPhi];
//
//  // Local +z direction is global +r direction.
//  double localDisplacementZ = m_alignmentZ[iStationName][stationEta][stationPhi];
//
//  // Local +s direction is global +phi direction.
//  double localDisplacementY = m_alignmentS[iStationName][stationEta][stationPhi];
//
//  // Rotation around the s-axis is not implemented yet (2011/11/29).
//  // m_alignmentTHS[tmpStationName][tmpStationEta][tmpStationPhi];
//
//  // Convert local x translation to local y and z translations
//  double localDisplacementYByX = 0.;
//  double localDisplacementZByX = 0.;
//  if(fabs(localDisplacementX)>1.0E-12) { // local y and z translations are non-zero only if local x translation is non-zero.
//    if(fabs(direCos[0])<1.0E-12) return; // To avoid zero-division
//    localDisplacementYByX = direCos[1]/direCos[0]*localDisplacementX;
//    localDisplacementZByX = direCos[2]/direCos[0]*localDisplacementX;
//  }
//  localPos.y() = localPos.y()+localDisplacementYByX+localDisplacementY;
//  localPos.z() = localPos.z()+localDisplacementZByX-localDisplacementZ;
//}

