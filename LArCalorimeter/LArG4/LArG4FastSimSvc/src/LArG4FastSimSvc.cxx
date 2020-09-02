/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// include header file
#include "LArG4FastSimSvc/LArG4FastSimSvc.h"

// athena includes
#include "GaudiKernel/ServiceHandle.h"

// CLHEP includes
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include "AtlasHepMC/IO_GenEvent.h"
#include "AtlasHepMC/GenEvent.h"

// default constructor
LArG4FastSimSvc::LArG4FastSimSvc(const std::string& name, ISvcLocator* sl)
  : AthService(name,sl),
    m_generated_starting_points_file(),
    m_generate_starting_points(false),
    m_starting_points_file(),
    m_detector_tag(0)
{
  declareProperty("EFlagToShowerLib", m_e_FlagShowerLib = true, "Switch for e+/- frozen showers");
  declareProperty("EMinEneShowerLib", m_e_MinEneShowerLib = 0.0*CLHEP::GeV, "Minimum energy for e+/- frozen showers");
  declareProperty("EMaxEneShowerLib", m_e_MaxEneShowerLib = 1.0*CLHEP::GeV, "Maximum energy for e+/- frozen showers");

  declareProperty("GFlagToShowerLib", m_g_FlagShowerLib = true, "Switch for photon frozen showers");
  declareProperty("GMinEneShowerLib", m_g_MinEneShowerLib = 0.*CLHEP::GeV, "Minimum energy for photon frozen showers");
  declareProperty("GMaxEneShowerLib", m_g_MaxEneShowerLib = 0.010*CLHEP::GeV, "Maximum energy for photon frozen showers");

  declareProperty("NeutFlagToShowerLib", m_Neut_FlagShowerLib = true, "Switch for neutron frozen showers");
  declareProperty("NeutMinEneShowerLib", m_Neut_MinEneShowerLib = 0.0*CLHEP::GeV, "Minimum energy for neutron frozen showers");
  declareProperty("NeutMaxEneShowerLib", m_Neut_MaxEneShowerLib = 0.1*CLHEP::GeV, "Maximum energy for neutron frozen showers");

  declareProperty("PionFlagToShowerLib", m_Pion_FlagShowerLib = true, "Switch for neutron frozen showers");
  declareProperty("PionMinEneShowerLib", m_Pion_MinEneShowerLib = 0.0*CLHEP::GeV, "Minimum energy for neutron frozen showers");
  declareProperty("PionMaxEneShowerLib", m_Pion_MaxEneShowerLib = 2.0*CLHEP::GeV, "Maximum energy for neutron frozen showers");

  declareProperty("ContainLow", m_containLow = true, "Switch for containment at low eta");
  declareProperty("ContainHigh", m_containHigh = true, "Switch for containment at high eta");
  declareProperty("ContainCrack", m_containCrack = true, "Switch for containment in the crack region");

  declareProperty("GeneratedStartingPointsFile", m_generated_starting_points_file = "", "Name of file for generated SPs. Do not touch until you want to produce a new library");
  declareProperty("GeneratedStartingPointsRatio", m_generated_starting_points_ratio = 0.02, "Ratio of SPs that goes to output");

  /* BE SURE THIS ONE IS THE SAME AS IN LArG4ShowerLibSvc!!! */
  enum DETECTOR {EMB=100000,EMEC=200000,FCAL1=300000,FCAL2=400000,FCAL3=500000,HECLOC=600000,HEC=700000};

  m_detmap["EMB"]=EMB;
  m_detmap["EMEC"]=EMEC;
  m_detmap["FCAL1"]=FCAL1;
  m_detmap["FCAL2"]=FCAL2;
  m_detmap["FCAL3"]=FCAL3;
  m_detmap["HECLOC"]=HECLOC;
  m_detmap["HEC"]=HEC;

}


// default destructor
LArG4FastSimSvc::~LArG4FastSimSvc()
{
}

// initialization
StatusCode LArG4FastSimSvc::initialize()
{
  msg(MSG::INFO) << "Initializing " << name()
      << " - package version " << PACKAGE_VERSION << endmsg;

  if (name().find("EMB") != std::string::npos) {
 	  m_detector_tag_str = "EMB";
  } else if (name().find("EMEC") != std::string::npos) {
 	  m_detector_tag_str = "EMEC";
  } else if (name().find("FCAL2") != std::string::npos) {
 	  m_detector_tag_str = "FCAL2";
  } else if (name().find("FCAL") != std::string::npos) {
 	  m_detector_tag_str = "FCAL1";
  } else if (name().find("HEC") != std::string::npos) {
      m_detector_tag_str = "HEC";
  } else if (name().find("DeadMaterial") != std::string::npos) {
      m_detector_tag_str = "DM";
  } else {
	  m_detector_tag_str = "UNKNOWN";
  }
  m_detector_tag = 0;
  if (m_detmap.find(m_detector_tag_str) != m_detmap.end()){
	  m_detector_tag = m_detmap.find(m_detector_tag_str)->second;
  }
  m_generate_starting_points = (!m_generated_starting_points_file.value().empty());

  if (m_generate_starting_points) {
	  m_starting_points_file = new HepMC::IO_GenEvent(m_generated_starting_points_file.value().c_str(),std::ios::out);
  }
  // list all properties
  this->print();
  
  return StatusCode::SUCCESS;
}


StatusCode LArG4FastSimSvc::finalize() 
{
  msg(MSG::INFO) << "Finalizing fast simulation service. " << endmsg;

  if (m_generate_starting_points) {
	  delete m_starting_points_file;
  }
  return StatusCode::SUCCESS;
}


// query interface
StatusCode LArG4FastSimSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( IID_ILArG4FastSimSvc == riid )    {
    *ppvInterface = (ILArG4FastSimSvc*)this;
  }
  else  {
    // Interface is not directly available: try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}


void LArG4FastSimSvc::print() const
{
  msg(MSG::INFO) << "List of Frozen Shower options:" << endmsg;
  msg(MSG::INFO) << "Detector tag is " << m_detector_tag_str << endmsg;
  if (m_generate_starting_points) {\
	  msg(MSG::INFO) << "Starting points will be saved to " << m_generated_starting_points_file.value() << endmsg;
  }

  if (this->m_e_FlagShowerLib) {
	msg(MSG::INFO) << "Electrons energy range [MeV] for Frozen Showers ["
	  << this->m_e_MinEneShowerLib << " -- " << this->m_e_MaxEneShowerLib << "]" << endmsg;
  } else {
	  msg(MSG::INFO) << "Electrons wont be parameterized" << endmsg;
  }

  if (this->m_g_FlagShowerLib) {
	msg(MSG::INFO) << "Photons   energy range [MeV] for Frozen Showers ["
	  << this->m_g_MinEneShowerLib << " -- " << this->m_g_MaxEneShowerLib << "]" << endmsg;
  } else {
	  msg(MSG::INFO) << "Photons   wont be parameterized" << endmsg;
  }

  if (this->m_Neut_FlagShowerLib) {
	msg(MSG::INFO) << "Neutrons  energy range [MeV] for Frozen Showers ["
	  << this->m_Neut_MinEneShowerLib << " -- " << this->m_Neut_MaxEneShowerLib << "]" << endmsg;
  } else {
	  msg(MSG::INFO) << "Neutrons  wont be parameterized" << endmsg;
  }

  // the containment checks
  msg(MSG::INFO) << "Containment check at low eta:   " << (this->m_containLow   ? "YES" : "NO") << endmsg;
  msg(MSG::INFO) << "Containment check at crack eta: " << (this->m_containCrack ? "YES" : "NO") << endmsg;
  msg(MSG::INFO) << "Containment check at high eta:  " << (this->m_containHigh  ? "YES" : "NO") << endmsg;
}

bool LArG4FastSimSvc::generateFSStartingPoint( const HepMC::GenEvent * ge ) const
{
	if (!m_generate_starting_points)
			return false;
	m_starting_points_file->write_event(ge);
	return true;
}
