/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEFIDTrackRoiMaker.h"

// RoI include
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

/**
 * Standard constructor for the algorithm
 */
TrigMuonEFIDTrackRoiMaker::TrigMuonEFIDTrackRoiMaker(const std::string& name, ISvcLocator* pSvcLocator) :
  FexAlgo(name, pSvcLocator),
  m_roiSizeZ0(225.0),//default to large width in mm
  m_inputRoiName("forID"),
  m_outputRoiName("forID2")
{

  declareProperty("Z0Width",m_roiSizeZ0); // should be set in JO to appropriate z-width, ATLAS standard units (mm)
  declareProperty("InputRoiName",m_inputRoiName="forID"); // generally don't expect to need to change this
  declareProperty("OutputRoiName",m_outputRoiName="forID2"); // generally don't expect to need to change this
  
  // variables to monitor
  declareMonitoredVariable("RoIz",m_roiZ);
  declareMonitoredStdContainer("DeltaEta",m_deltaEta,IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("DeltaPhi",m_deltaPhi,IMonitoredAlgo::AutoClear);
}

/**
 * Destructor
 */
TrigMuonEFIDTrackRoiMaker::~TrigMuonEFIDTrackRoiMaker()
{

}

/**
 * Initialize the algo
 */
HLT::ErrorCode TrigMuonEFIDTrackRoiMaker::hltInitialize() {
  // print out the configuration
  ATH_MSG_DEBUG("Initializing TrigMuonEFIDTrackRoiMaker[" << name() << "]");
  ATH_MSG_DEBUG("InputRoiName  = " << m_inputRoiName);
  ATH_MSG_DEBUG("OutputRoiName = " << m_outputRoiName);
  ATH_MSG_DEBUG("Z0Width       = " << m_roiSizeZ0); 

  return HLT::OK;
}//hltInitialize

/** 
 * Finalise the algo - nothing to do yet
 */
HLT::ErrorCode TrigMuonEFIDTrackRoiMaker::hltFinalize() {
  return HLT::OK;
}

/**
 * Execute function - called for each roi
 */
HLT::ErrorCode TrigMuonEFIDTrackRoiMaker::hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE) {

  ATH_MSG_DEBUG("Executing TrigMuonEFIDTrackRoiMaker::hltExecute");

  // get the RoI for the ID tracking
  const TrigRoiDescriptor *roi = 0;
  if (HLT::OK != getFeature(inputTE, roi, m_inputRoiName) ) {
    ATH_MSG_WARNING("Could not get " << m_inputRoiName << " RoI descriptor, cannot update it");
    return HLT::OK;
  }

  // get the xAOD::Muons
  const xAOD::MuonContainer* muons = 0;
  if (HLT::OK != getFeature(outputTE, muons) ) {
    ATH_MSG_WARNING("Could not get muons, cannot update the RoI");
    return HLT::OK;
  }

  // make new RoI
  TrigRoiDescriptor* newroi = createIDtrackRoI(*roi, *muons);

  // save it
  if(newroi!=0) {
    // monitor z position
    m_roiZ = newroi->zed();
    // attached to trigger element
    HLT::ErrorCode hltStatus = attachFeature(outputTE, newroi, m_outputRoiName);
    if(hltStatus!=HLT::OK) {
      msg() << MSG::WARNING << "Attaching roi " << newroi << " with name " << m_outputRoiName << " to TEout: unsuccessful" << endmsg;
      delete newroi;
    } else {
      msg() << MSG::DEBUG << "Attached roi " << newroi << " with name forID2" << endmsg;
    }
    
  }//valid roi
  
  return HLT::OK;
  
}//hltExecute

/**
 * Create a new RoI using the parameters of the old one, updating the z and zWidth
 *
 * @return the new RoiDescriptor, returns 0 in case it could not be created (e.g. no muons with ID tracks)
 */
TrigRoiDescriptor* TrigMuonEFIDTrackRoiMaker::createIDtrackRoI(const TrigRoiDescriptor& roi, const xAOD::MuonContainer& muons) {

  // loop on muons to find the z values & store eta, phi of muon
  std::vector<double> z0values;
  for( auto muon : muons ) {
    ElementLink<xAOD::TrackParticleContainer> idTrackLink = muon->inDetTrackParticleLink();
    if(!idTrackLink.isValid()) continue;
    const Trk::Perigee* idtrk_perigee = &((*idTrackLink)->perigeeParameters());
    if(!idtrk_perigee) {
      ATH_MSG_WARNING("ID track with no Perigee, skip");
      continue;
    }
    
    // monitor how far our muons are from the RoI center
    m_deltaEta.push_back( fabs((*idTrackLink)->eta() - roi.eta()) );
    m_deltaPhi.push_back( fabs(TVector2::Phi_mpi_pi((*idTrackLink)->phi() - roi.phi())) );

    const double z0 = idtrk_perigee->parameters()[Trk::z0];
    z0values.push_back( z0 );
    if(msgLvl(MSG::DEBUG) && idtrk_perigee->covariance()) {
      ATH_MSG_DEBUG("z0 = " << z0 << " +- " << sqrt((*(idtrk_perigee->covariance()))(Trk::z0, Trk::z0)) );
    }
  }// muon loop

  // if no muons, then nothing to do
  if(z0values.size()==0 ) {
    if(!muons.empty()) ATH_MSG_WARNING("No valid z0 found, will not create new RoI, old RoI has zWidth = " << roi.zedPlus() - roi.zedMinus());
    else ATH_MSG_WARNING("Zero muons, so cannot create new RoI");

    return 0;
  }

  // 1 muon, use central z0 with the fixed width
  double zCentral=z0values[0];
  double zMinus = zCentral-m_roiSizeZ0;
  double zPlus = zCentral+m_roiSizeZ0;

  if(z0values.size()>1) {
    //multiple muons, first sort z values so they are low -> high
    std::sort(z0values.begin(), z0values.end());
    // our z low is lowest z - width, z high is highest z + width
    zMinus = z0values[0] - m_roiSizeZ0;
    zPlus = z0values.back() + m_roiSizeZ0;
    // center RoI in the middle of the muons
    zCentral = (z0values[0] + z0values.back()) / 2.0;
  }

  // make new RoI
  ATH_MSG_DEBUG("Create RoI at eta=" << roi.eta() << " phi=" << roi.phi() << " z=" << zCentral << " in range " << zMinus << " -> " << zPlus);
  TrigRoiDescriptor* newroi = new TrigRoiDescriptor(roi.roiWord(), roi.l1Id(), roi.roiId(), roi.eta(), roi.etaMinus(), roi.etaPlus(), roi.phi(), roi.phiMinus(), roi.phiPlus(), zCentral, zMinus, zPlus);

  return newroi;
}// createIDtrackRoI
