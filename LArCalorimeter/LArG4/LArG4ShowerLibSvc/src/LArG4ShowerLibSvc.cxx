/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4ShowerLibSvc/LArG4ShowerLibSvc.h"

// shower lib classes
#include "LArG4ShowerLib/ShowerLibList.h"

#include "PathResolver/PathResolver.h"

#include "AthenaKernel/IAtRndmGenSvc.h"
#include "AthenaKernel/Units.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandGauss.h"



#include <sstream>
#include "TFile.h"
#include "TTree.h"


namespace Units = Athena::Units;


LArG4ShowerLibSvc::LArG4ShowerLibSvc(const std::string& name,ISvcLocator* svc)
  : AthService(name,svc)
  , m_fileNameList()
  , m_rndmEngineName("FROZENSHOWERS")
  , m_rndmGenSvc("AtDSFMTGenSvc", name)
  , m_rndmEngine(nullptr)
{
  declareProperty( "FileNameList",    m_fileNameList,   "List of filenames for direct reading" );
  declareProperty( "RndmEngineName",  m_rndmEngineName, "Name of athena RNG engine" );
  declareProperty( "RndmGenSvc",      m_rndmGenSvc, "Athena RNG service" );

  /* BE SURE THIS ONE IS THE SAME AS IN LArG4FastSimSvc!!! */
  enum DETECTOR {EMB=100000,EMEC=200000,FCAL1=300000,FCAL2=400000,FCAL3=500000,HECLOC=600000,HEC=700000};

  m_detmap["EMB"]=EMB;
  m_detmap["EMEC"]=EMEC;
  m_detmap["FCAL1"]=FCAL1;
  m_detmap["FCAL2"]=FCAL2;
  m_detmap["FCAL3"]=FCAL3;
  m_detmap["HECLOC"]=HECLOC;
  m_detmap["HEC"]=HEC;
}

LArG4ShowerLibSvc::~LArG4ShowerLibSvc()
{
}

StatusCode LArG4ShowerLibSvc::initialize()
{
  ATH_MSG_INFO("Initializing " << name()
               << " - package version " << PACKAGE_VERSION);

  std::vector<std::string>::const_iterator iter;

  // iterate through filenames in list
  for (iter = m_fileNameList.value().begin(); iter != m_fileNameList.value().end(); iter++) {
    std::string resolvedFilename = PathResolverFindCalibFile(*iter);
    if (resolvedFilename.empty()) {
      ATH_MSG_WARNING("Could not resolve input filename " << (*iter) << ". Ignoring!");
      continue;
    } else {
      ATH_MSG_INFO("Resolving input filename to " << resolvedFilename);
    }

    TFile rootfile(resolvedFilename.c_str(),"READ");

    if (rootfile.IsZombie()) {
      ATH_MSG_WARNING("File " << resolvedFilename << " is not a valid ROOT file");
      continue;
    }

    const ShowerLib::IShowerLib* library = nullptr;

    // trying to create a library out of provided file
    library = ShowerLib::iterateTTree(&rootfile);

    // if no library can be created based on the file nullptr is returned
    if (library == nullptr) {
      ATH_MSG_WARNING("File " << resolvedFilename << " is not a valid library file");
      continue;
    }

    if (m_detmap.find(library->detector()) == m_detmap.end()) {
      ATH_MSG_WARNING("Library " << resolvedFilename << " is produced for unknown detector: " << library->detector());
      delete library;
      continue;
    }

    std::stringstream location;
    location << library->detector() << "/" << library->particle_id();
    int key = m_detmap.find(library->detector())->second + library->particle_id();

    // put the library into map
    m_libraryMap[key] = library;
    m_locations[key] = location.str();

    ATH_MSG_DEBUG("Filename: " << resolvedFilename.c_str());
    ATH_MSG_DEBUG("Location: " << location.str());
    ATH_MSG_DEBUG("Release: " << library->release().c_str());
    ATH_MSG_DEBUG("Geant ver: " << library->geantVersion().c_str());
    ATH_MSG_DEBUG("Phys list: " << library->physicsList().c_str());
  }

  // no point in the service with no libraries
  if (m_libraryMap.empty()) {
    ATH_MSG_WARNING("No library files found");
  } else {
    ATH_MSG_INFO("List of loaded libraries:");
    libmap::const_iterator it;
    for(it = m_libraryMap.begin();it != m_libraryMap.end(); it++) {
      ATH_MSG_INFO("      " << m_locations[(*it).first] << ": " << (*it).second->comment());
      m_statisticsMap[(*it).second] = (*it).second->createStatistics();
    }
    // we have loaded some libs, so there is a point in RNG service
    if (m_rndmEngineName.value().length() > 0) {
      if (m_rndmGenSvc.retrieve().isSuccess()) {
        m_rndmEngine = m_rndmGenSvc->GetEngine(m_rndmEngineName.value());
        if (m_rndmEngine)
          ATH_MSG_INFO("Successfully retrieved random number stream " << m_rndmEngineName.value());
        else
          ATH_MSG_WARNING("Couldn't retrieve random number stream " << m_rndmEngineName.value() << ". The simulation result may be biased.");
      }
      else {
        ATH_MSG_WARNING("Couldn't retrieve random number service. The simulation result may be biased.");
      }
    } else {
      ATH_MSG_WARNING("Empty name for random stream. No randomization will be applied.");
    }
  }

  ATH_MSG_INFO("Shower library successfully initialized.");

  return StatusCode::SUCCESS;
}

StatusCode LArG4ShowerLibSvc::finalize()
{

  ATH_MSG_INFO("Finalizing shower library service.");

  libmap::const_iterator iter;

  for (iter = m_libraryMap.begin(); iter != m_libraryMap.end(); ++iter) {
    ATH_MSG_INFO("Found ShowerLib at location " << m_locations[(*iter).first]);
    ATH_MSG_INFO(std::endl << (*iter).second->statistics());
    if (m_statisticsMap.find((*iter).second) != m_statisticsMap.end())
      ATH_MSG_INFO(m_statisticsMap.find((*iter).second)->second->statistics());
    else
      ATH_MSG_INFO("No statistics available for this kind of library");
    // delete the library:
    delete (*iter).second ;
  }

  return StatusCode::SUCCESS;
}

StatusCode
LArG4ShowerLibSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( IID_ILArG4ShowerLibSvc == riid ) {
    *ppvInterface = (ILArG4ShowerLibSvc*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  // Interface is not directly available: try out a base class
  return AthService::queryInterface(riid, ppvInterface);
}


/*
 * Returns library from internal map based on the particle. Returns nullptr if there
 * is no library for needed particle/detector.
 */
const ShowerLib::IShowerLib* LArG4ShowerLibSvc::getShowerLib(G4int particleCode, int detectorTag)
{
  int location;

  location = detectorTag + particleCode;

  libmap::const_iterator iter = m_libraryMap.find(location);
  if (iter != m_libraryMap.end()) {
    return (*iter).second;
  }

  return nullptr;
}

bool
LArG4ShowerLibSvc::checkLibrary(G4int particleCode, int detectorTag)
{
  const ShowerLib::IShowerLib* library = getShowerLib(particleCode, detectorTag);
  if (library != nullptr) return true;

  return false;
}

/*
 * Returns a shower based on the particle. Return empty shower if there is no
 * appropriate showers in the libraries.
 */
std::vector<EnergySpot>
LArG4ShowerLibSvc::getShower(const G4FastTrack& track, int detectorTag)
{
  // get shower lib from the map
  const ShowerLib::IShowerLib* library = getShowerLib(track.GetPrimaryTrack()->GetDefinition()->GetPDGEncoding(), detectorTag);

  if (library == nullptr) {
    // no library in map
    ATH_MSG_ERROR("No library for location: " << detectorTag << "/" << track.GetPrimaryTrack()->GetDefinition()->GetPDGEncoding());
    return std::vector<EnergySpot>();
  }

  // starting point of the shower:
  G4ThreeVector PositionShower = track.GetPrimaryTrack()->GetPosition();

  // get a shower from the library
  int randomShift = 0;
  if (m_rndmEngine) {
    randomShift = (int)(CLHEP::RandGauss::shoot(m_rndmEngine, 0., 2.5)+0.5);
  }
  std::vector<EnergySpot>* shower = library->getShower(track.GetPrimaryTrack(), m_statisticsMap[library], randomShift);

  if (shower == nullptr) {
    return std::vector<EnergySpot>();
  }

  // axis of the shower, in global reference frame:
  G4ThreeVector DirectionShower = track.GetPrimaryTrack()->GetMomentumDirection();

  // time of the track (as in LArBarrelCalculator.cc)
  G4double tof = track.GetPrimaryTrack()->GetGlobalTime() / Units::ns;
  G4double time = tof - ( track.GetPrimaryTrack()->GetPosition().mag()/Units::c_light ) / Units::ns;

  std::vector<EnergySpot>::iterator hit;

  // Create energy spots
  for (hit = shower->begin(); hit != shower->end(); ++hit) {
    G4ThreeVector hitpos = (*hit).GetPosition();
    hitpos.rotateUz(DirectionShower); // rotate the hit to the needed direction
    (*hit).SetPosition(hitpos+PositionShower);
    (*hit).SetTime((*hit).GetTime() + time);
  }

  if (msgSvc()->outputLevel(name()) <= MSG::VERBOSE) {
    ATH_MSG_VERBOSE("Prepared " << shower->size() << " EnergySpot");
    for (std::vector<EnergySpot>::const_iterator iter = shower->begin(); iter != shower->end(); ++iter) {
      ATH_MSG_VERBOSE("EnergySpot: " << iter->GetPosition().x() << " " << iter->GetPosition().y() << " " << iter->GetPosition().z()
                      << " " << iter->GetEnergy() << " " << iter->GetTime());
    }
  }

  return *std::auto_ptr< std::vector<EnergySpot> >(shower);
}

double
LArG4ShowerLibSvc::getContainmentZ(const G4FastTrack& track, int detectorTag)
{
  // get shower lib from the map
  const ShowerLib::IShowerLib* library = getShowerLib(track.GetPrimaryTrack()->GetDefinition()->GetPDGEncoding(), detectorTag);

  if (library == 0) {
    return 0.0;
  }

  return library->getContainmentZ(track.GetPrimaryTrack());
}

double
LArG4ShowerLibSvc::getContainmentR(const G4FastTrack& track, int detectorTag)
{
  // get shower lib from the map
  const ShowerLib::IShowerLib* library = getShowerLib(track.GetPrimaryTrack()->GetDefinition()->GetPDGEncoding(), detectorTag);

  if (library == 0) {
    return 0.0;
  }

  return library->getContainmentR(track.GetPrimaryTrack());
}
