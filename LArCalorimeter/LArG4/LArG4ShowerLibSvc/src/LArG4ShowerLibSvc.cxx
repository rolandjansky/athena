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
  : AthService(name,svc),
  m_fileNameList(),
  m_rndmEngineName("FROZENSHOWERS"),
  m_rndmGenSvc("AtDSFMTGenSvc", name),
  m_rndmEngine(0)
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
  msg(MSG::INFO) << "Initializing " << name()
      << " - package version " << PACKAGE_VERSION << endmsg;

  std::vector<std::string>::const_iterator iter;

  // iterate through filenames in list
  for (iter = m_fileNameList.value().begin(); iter != m_fileNameList.value().end(); iter++) {
	  std::string resolvedFilename = PathResolver::find_file (*iter, "DATAPATH");
      if (resolvedFilename.empty()) {
    	  msg(MSG::WARNING) << "Could not resolve input filename " << (*iter) << ". Ignoring!" << endmsg;
    	  continue;
      } else {
    	  msg(MSG::INFO) << "Resolving input filename to " << resolvedFilename << endmsg;
      }

      TFile rootfile(resolvedFilename.c_str(),"READ");

      if (rootfile.IsZombie()) {
         msg(MSG::WARNING) << "File " << resolvedFilename << " is not a valid ROOT file" << endmsg;
         continue;
      }

      const ShowerLib::IShowerLib* library = NULL;

      // trying to create a library out of provided file
      library = ShowerLib::iterateTTree(&rootfile);

      // if no library can be created based on the file NULL is returned
      if (library == NULL) {
    	  msg(MSG::WARNING) << "File " << resolvedFilename << " is not a valid library file" << endmsg;
    	  continue;
      }

      if (m_detmap.find(library->detector()) == m_detmap.end()) {
    	  msg(MSG::WARNING) << "Library " << resolvedFilename << " is produced for unknown detector: " << library->detector() << endmsg;
    	  delete library;
    	  continue;
      }

      std::stringstream location;
      location << library->detector() << "/" << library->particle_id();
      int key = m_detmap.find(library->detector())->second + library->particle_id();

      // put the library into map
      m_libraryMap[key] = library;
      m_locations[key] = location.str();

      msg(MSG::DEBUG) << "Filename: " << resolvedFilename.c_str() << endmsg;
      msg(MSG::DEBUG) << "Location: " << location.str() << endmsg;
      msg(MSG::DEBUG) << "Release: " << library->release().c_str() << endmsg;
      msg(MSG::DEBUG) << "Geant ver: " << library->geantVersion().c_str() << endmsg;
      msg(MSG::DEBUG) << "Phys list: " << library->physicsList().c_str() << endmsg;
  }

  // no point in the service with no libraries
  if (m_libraryMap.empty()) {
	  msg(MSG::WARNING) << "No library files found" << endmsg;
  } else {
    msg(MSG::INFO) << "List of loaded libraries:" << endmsg;
    libmap::const_iterator it;
    for(it = m_libraryMap.begin();it != m_libraryMap.end(); it++) {
	    msg(MSG::INFO) << "      " << m_locations[(*it).first] << ": " << (*it).second->comment() << endmsg;
	    m_statisticsMap[(*it).second] = (*it).second->createStatistics();
    }
    // we have loaded some libs, so there is a point in RNG service
    if (m_rndmEngineName.value().length() > 0) {
      if (m_rndmGenSvc.retrieve().isSuccess()) {
          m_rndmEngine = m_rndmGenSvc->GetEngine(m_rndmEngineName.value());
	  if (m_rndmEngine)
              msg(MSG::INFO) << "Successfully retrieved random number stream " << m_rndmEngineName.value() << endmsg;
	  else
	      msg(MSG::WARNING) << "Couldn't retrieve random number stream " << m_rndmEngineName.value() << ". The simulation result may be biased." << endmsg;
    }
    else {
        msg(MSG::WARNING) << "Couldn't retrieve random number service. The simulation result may be biased." << endmsg;
    }
    } else {
        msg(MSG::WARNING) << "Empty name for random stream. No randomization will be applied." << endmsg;
    }
  }

  msg(MSG::INFO) << "Shower library successfully initialized." << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode LArG4ShowerLibSvc::finalize()
{

  msg(MSG::INFO) << "Finalizing shower library service." << endmsg;

  libmap::const_iterator iter;

  for (iter = m_libraryMap.begin(); iter != m_libraryMap.end(); ++iter) {
    msg(MSG::INFO) << "Found ShowerLib at location " << m_locations[(*iter).first] << endmsg;
    msg(MSG::INFO) << std::endl << (*iter).second->statistics();
    if (m_statisticsMap.find((*iter).second) != m_statisticsMap.end())
    	msg(MSG::INFO) << m_statisticsMap.find((*iter).second)->second->statistics() << endmsg;
    else
    	msg(MSG::INFO) << "No statistics available for this kind of library" << endmsg;
  }

  return StatusCode::SUCCESS;
}

StatusCode
LArG4ShowerLibSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( IID_ILArG4ShowerLibSvc == riid )    {
    *ppvInterface = (ILArG4ShowerLibSvc*)this;
  }
  else  {
    // Interface is not directly available: try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}


/*
 * Returns library from internal map based on the particle. Returns NULL if there
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

	return NULL;
}

bool
LArG4ShowerLibSvc::checkLibrary(G4int particleCode, int detectorTag)
{
	const ShowerLib::IShowerLib* library = getShowerLib(particleCode, detectorTag);
	if (library != NULL) return true;

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

  if (library == 0) {
	  // no library in map
	  msg(MSG::ERROR) << "No library for location: " << detectorTag << "/" << track.GetPrimaryTrack()->GetDefinition()->GetPDGEncoding() << endmsg;
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

  if (shower == NULL) {
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
    msg(MSG::VERBOSE) << "Prepared " << shower->size() << " EnergySpot" << endmsg;
    for (std::vector<EnergySpot>::const_iterator iter = shower->begin(); iter != shower->end(); ++iter) {
      msg(MSG::VERBOSE) << "EnergySpot: " << iter->GetPosition().x() << " " << iter->GetPosition().y() << " " << iter->GetPosition().z()
		<< " " << iter->GetEnergy() << " " << iter->GetTime() << endmsg;
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
