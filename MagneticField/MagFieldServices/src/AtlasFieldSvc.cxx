/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// AtlasFieldSvc.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include <iostream>

// ISF_Services include
#include "MagFieldServices/AtlasFieldSvc.h"

// PathResolver
#include "PathResolver/PathResolver.h"

// StoreGate
#include "StoreGate/StoreGateSvc.h"

// Athena Pool
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

// IncidentSvc
#include "GaudiKernel/IIncidentSvc.h"

// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"

// ROOT
#include "TFile.h"
#include "TTree.h"

/** Constructor **/
MagField::AtlasFieldSvc::AtlasFieldSvc(const std::string& name,ISvcLocator* svc) :
    AthService(name,svc),
    IMagFieldSvc(),
    m_fullMapFilename("MagneticFieldMaps/bfieldmap_7730_20400_14m.root"),
    m_soleMapFilename("MagneticFieldMaps/bfieldmap_7730_0_14m.root"),
    m_toroMapFilename("MagneticFieldMaps/bfieldmap_0_20400_14m.root"),
    m_mapSoleCurrent(7730.),
    m_mapToroCurrent(20400.),
    m_soleMinCurrent(1.0),
    m_toroMinCurrent(1.0),
    m_useDCS(false),
    m_coolCurrentsFolderName("/EXT/DCS/MAGNETS/SENSORDATA"),
    m_useMapsFromCOOL(true),
    m_coolMapsFolderName("/GLOBAL/BField/Maps"),
    m_useSoleCurrent(7730.),
    m_useToroCurrent(20400.),
    m_lockMapCurrents(false),
    m_mapHandle(),
    m_currentHandle(),
    m_zone(),
    m_meshZR(nullptr),
    m_edge(),
    m_edgeLUT(),
    m_invq(),
    m_zoneLUT(),
    m_zmin(0.),
    m_zmax(0.),
    m_nz(0),
    m_rmax(0.),
    m_nr(0),
    m_nphi(0)
    /* ,
       m_doManipulation(false),
       m_manipulator("undefined") */
{
    declareProperty("FullMapFile", m_fullMapFilename, "File storing the full magnetic field map");
    declareProperty("SoleMapFile", m_soleMapFilename, "File storing the solenoid-only magnetic field map");
    declareProperty("ToroMapFile", m_toroMapFilename, "File storing the toroid-only magnetic field map");
    declareProperty("MapSoleCurrent", m_mapSoleCurrent, "Nominal solenoid current (A)");
    declareProperty("MapToroCurrent", m_mapToroCurrent, "Nominal toroid current (A)");
    declareProperty("SoleMinCurrent", m_soleMinCurrent, "Minimum solenoid current (A) for which solenoid is considered ON");
    declareProperty("ToroMinCurrent", m_toroMinCurrent, "Minimum toroid current (A) for which toroid is considered ON");
    declareProperty("UseDCS", m_useDCS, "Get magnet currents from DCS through COOL");
    declareProperty("COOLCurrentsFolderName", m_coolCurrentsFolderName, "Name of the COOL folder containing magnet currents");
    declareProperty("UseMapsFromCOOL", m_useMapsFromCOOL, "Get magnetic field map filenames from COOL");
    declareProperty("COOLMapsFolderName", m_coolMapsFolderName, "Name of the COOL folder containing field maps");
    declareProperty("UseSoleCurrent", m_useSoleCurrent, "Set actual solenoid current (A)");
    declareProperty("UseToroCurrent", m_useToroCurrent, "Set actual toroid current (A)");
    declareProperty("LockMapCurrents", m_lockMapCurrents, "Do not rescale currents (use the map values)");
    /* declareProperty("DoManipulation", m_doManipulation, "Apply field manipulation");
       declareProperty("ManipulatorTool", m_manipulator, "Tool handle for field manipulation"); */
}

MagField::AtlasFieldSvc::~AtlasFieldSvc()
{
    delete m_meshZR;
}

/** framework methods */
StatusCode MagField::AtlasFieldSvc::initialize()
{
    ATH_MSG_INFO( "initialize() ..." );

    // determine map location from COOL, if available
    if ( m_useMapsFromCOOL ) {
      // Register callback
      StoreGateSvc* detStore;
      if ( service( "DetectorStore", detStore ).isFailure() ) {
          ATH_MSG_FATAL( "Could not get detector store" );
          return StatusCode::FAILURE;
      }
      std::string folder( m_coolMapsFolderName );
      ATH_MSG_INFO("maps will be chosen reading COOL folder " << folder);
      if ( detStore->regFcn( &MagField::AtlasFieldSvc::updateMapFilenames, this,
                                 m_mapHandle, folder ).isFailure() ) {
              ATH_MSG_FATAL( "Could not book callback for " << folder );
              return StatusCode::FAILURE;
      }
    }

    // are we going to get the magnet currents from DCS?
    if ( m_useDCS ) {
        // Register callback
        StoreGateSvc* detStore;
        if ( service( "DetectorStore", detStore ).isFailure() ) {
            ATH_MSG_FATAL( "Could not get detector store" );
            return StatusCode::FAILURE;
        }
        std::string folder( m_coolCurrentsFolderName );
        ATH_MSG_INFO("magnet currents will be read from COOL folder " << folder);
        if ( detStore->regFcn( &MagField::AtlasFieldSvc::updateCurrent, this,
                               m_currentHandle, folder ).isFailure() ) {
            ATH_MSG_FATAL( "Could not book callback for " << folder );
            return StatusCode::FAILURE;
        }
        ATH_MSG_INFO( "Booked callback for " << folder );
        // actual initalization has to wait for the fist callback
    } else {
        ATH_MSG_INFO( "Currents are set-up by jobOptions - delaying map initialization until BeginRun incident happens" );

        ServiceHandle<IIncidentSvc> incidentSvc("IncidentSvc", name());
        if (incidentSvc.retrieve().isFailure()) {
            ATH_MSG_FATAL( "Unable to retrieve the IncidentSvc" );
            return StatusCode::FAILURE;
        } else {
            incidentSvc->addListener( this, IncidentType::BeginRun );
            ATH_MSG_INFO( "Added listener to BeginRun incident" );
        }
    }

    // retrieve thread-local storage
    AtlasFieldSvcTLS &tls = getAtlasFieldSvcTLS();

    // clear the map for zero field
    clearMap(tls);
    setSolenoidCurrent(0.0);
    setToroidCurrent(0.0);

    /* // retrieve the manipulator tool
       if (m_doManipulation) {
       ATH_MSG_INFO( "field will be manipulated, retrieving tool" );
       if (m_manipulator.retrieve().isFailure()) {
       ATH_MSG_FATAL( "unable to retrieve manipulation tool" );
       } else {
       ATH_MSG_INFO( "manipulation tool retrieved" );
       getFieldActual = &MagField::AtlasFieldSvc::getFieldManipulated;
       }
       } else {
       ATH_MSG_INFO( "no manipulation set up" );
       getFieldActual = &MagField::AtlasFieldSvc::getFieldStandard;
       } */

    ATH_MSG_INFO( "initialize() successful" );
    return StatusCode::SUCCESS;
}

void MagField::AtlasFieldSvc::handle(const Incident& runIncident)
{
    ATH_MSG_INFO( "handling incidents ..." );
    if ( !m_useDCS && runIncident.type() == IncidentType::BeginRun) {
        // get thread-local storage
        AtlasFieldSvcTLS &tls = getAtlasFieldSvcTLS();

        if ( importCurrents(tls).isFailure() ) {
            ATH_MSG_FATAL( "Failure in manual setting of currents" );
        } else {
            ATH_MSG_INFO( "BeginRun incident handled" );
	}
    }
    ATH_MSG_INFO( "incidents handled successfully" );
}

StatusCode MagField::AtlasFieldSvc::importCurrents(AtlasFieldSvcTLS &tls)
{
    ATH_MSG_INFO( "importCurrents() ..." );

    // take the current values from JobOptions
    double solcur(m_useSoleCurrent);
    double torcur(m_useToroCurrent);
    if ( solcur < m_soleMinCurrent ) {
        solcur = 0.0;
        ATH_MSG_INFO( "Solenoid is off" );
    }
    if ( torcur < m_toroMinCurrent) {
        torcur = 0.0;
        ATH_MSG_INFO( "Toroids are off" );
    }
    setSolenoidCurrent(solcur);
    setToroidCurrent(torcur);
    // read the map file
    if ( initializeMap(tls).isFailure() ) {
        ATH_MSG_FATAL( "Failed to initialize field map" );
        return StatusCode::FAILURE;
    }

    ATH_MSG_INFO( "Currents imported and map initialized" );
    return StatusCode::SUCCESS;
}

/** callback for possible magnet current update **/
StatusCode MagField::AtlasFieldSvc::updateCurrent(IOVSVC_CALLBACK_ARGS)
{
    // get magnet currents from DCS
    double solcur(0.);
    double torcur(0.);
    bool gotsol(false);
    bool gottor(false);

    // due to inconsistencies between CONDBR2 and OFLP200/COMP200 (the former includes channel names
    // in the /EXT/DCS/MAGNETS/SENSORDATA folder, the latter don't), we try to read currents in
    // both ways
    bool hasChanNames(false);
 
    ATH_MSG_INFO( "Attempt 1 at reading currents from DCS (using channel name)" );
    for ( CondAttrListCollection::const_iterator itr = m_currentHandle->begin();
	  itr != m_currentHandle->end(); ++itr ) {

          std::string name = m_currentHandle->chanName(itr->first);
          ATH_MSG_INFO( "Trying to read from DCS: [channel name, index, value] " << name << " , " << itr->first << " , " << itr->second["value"].data<float>() );

	  if (name.compare("") != 0) {
	    hasChanNames = true;
	  }

          if ( name.compare("CentralSol_Current") == 0 ) {
              // channel 1 is solenoid current
              solcur = itr->second["value"].data<float>();
              gotsol = true;
          } else if ( name.compare("Toroids_Current") == 0 ) {
              // channel 3 is toroid current
              torcur = itr->second["value"].data<float>();
              gottor = true;
          }
    }
    if ( !hasChanNames ) {
        ATH_MSG_INFO( "Attempt 2 at reading currents from DCS (using channel index)" );
        // in no channel is named, try again using channel index instead
        for ( CondAttrListCollection::const_iterator itr = m_currentHandle->begin();
              itr != m_currentHandle->end(); ++itr ) {

              if ( itr->first == 1 ) {
                  // channel 1 is solenoid current
                  solcur = itr->second["value"].data<float>();
                  gotsol = true;
              } else if ( itr->first == 3 ) {
                  // channel 3 is toroid current
                  torcur = itr->second["value"].data<float>();
                  gottor = true;
              }
        }
    }

    if ( !gotsol || !gottor ) {
        if ( !gotsol ) ATH_MSG_ERROR( "Missing solenoid current in DCS information" );
        if ( !gottor ) ATH_MSG_ERROR( "Missing toroid current in DCS information" );
        return StatusCode::FAILURE;
    }
    ATH_MSG_INFO( "Currents read from DCS: solenoid " << solcur << " toroid " << torcur );
    // round to zero if close to zero
    if ( solcur < m_soleMinCurrent) {
        solcur = 0.0;
        ATH_MSG_INFO( "Solenoid is off" );
    }
    if ( torcur < m_toroMinCurrent) {
        torcur = 0.0;
        ATH_MSG_INFO( "Toroids are off" );
    }
    // did solenoid/toroid change status between on and off?
    bool solWasOn( solenoidOn() );
    bool torWasOn( toroidOn() );
    setSolenoidCurrent( solcur );
    setToroidCurrent( torcur );
    if ( solenoidOn() != solWasOn || toroidOn() != torWasOn ) {
        // get thread-local storage
        AtlasFieldSvcTLS &tls = getAtlasFieldSvcTLS();

        // map has changed. re-initialize the map
        if ( initializeMap(tls).isFailure() ) {
            ATH_MSG_ERROR( "Failed to re-initialize field map" );
            return StatusCode::FAILURE;
        }
    } else {
        // map is still valid. just scale the currents
        if (!m_lockMapCurrents)
          scaleField();
        else
          ATH_MSG_INFO( "Currents are NOT scaled - using map values sole=" << m_mapSoleCurrent << " toro=" << m_mapToroCurrent );
    }

    return StatusCode::SUCCESS;
}

/** callback for possible field map filenames update **/
StatusCode MagField::AtlasFieldSvc::updateMapFilenames(IOVSVC_CALLBACK_ARGS)
{
    ATH_MSG_INFO( "reading magnetic field map filenames from COOL" );

    std::string fullMapFilename("");
    std::string soleMapFilename("");
    std::string toroMapFilename("");

    for (CondAttrListCollection::const_iterator itr = m_mapHandle->begin(); itr != m_mapHandle->end(); ++itr) {
        const coral::AttributeList &attr = itr->second;
        const std::string &mapType = attr["FieldType"].data<std::string>();
        const std::string &mapFile = attr["MapFileName"].data<std::string>();
        const float soleCur = attr["SolenoidCurrent"].data<float>();
        const float toroCur = attr["ToroidCurrent"].data<float>();
        
        ATH_MSG_INFO("found map of type " << mapType << " with soleCur=" << soleCur << " toroCur=" << toroCur << " (path " << mapFile << ")");

	// first 5 letters are reserved (like "file:")
	const std::string mapFile_decoded = mapFile.substr(5);
	if (mapType == "GlobalMap") {
	  fullMapFilename = mapFile_decoded;
	  m_mapSoleCurrent = soleCur;
	  m_mapToroCurrent = toroCur;
	} else if (mapType == "SolenoidMap") {
	  soleMapFilename = mapFile_decoded;
	} else if (mapType == "ToroidMap") {
	  toroMapFilename = mapFile_decoded;
	}
	// note: the idea is that the folder contains exactly three maps
	// (if it contains more than 3 maps, then this logic doesn't work perfectly)
	// nominal currents are read from the global map
    }

    if (fullMapFilename == "" || soleMapFilename == "" || toroMapFilename == "") {
      ATH_MSG_ERROR("unexpected content in COOL field map folder");
      return StatusCode::FAILURE;
    }

    // check if maps really changed
    if (fullMapFilename != m_fullMapFilename || soleMapFilename != m_soleMapFilename || toroMapFilename != m_toroMapFilename) {
      ATH_MSG_INFO( "map set is new! reinitializing map");
      m_fullMapFilename = fullMapFilename;
      m_soleMapFilename = soleMapFilename;
      m_toroMapFilename = toroMapFilename;

      // retrieve the thread-local storage
      AtlasFieldSvcTLS &tls = getAtlasFieldSvcTLS();

      // trigger map reinitialization
      if ( initializeMap(tls).isFailure() ) {
         ATH_MSG_ERROR( "failed to re-initialize field map" );
         return StatusCode::FAILURE;
      }
    } else { 
      ATH_MSG_INFO( "no need to update map set");
    }

    return StatusCode::SUCCESS;
}

//
//  read and initialize map
//
StatusCode MagField::AtlasFieldSvc::initializeMap(AtlasFieldSvcTLS &tls)
{
    ATH_MSG_INFO( "Initializing the field map (solenoidCurrent=" << solenoidCurrent() << " toroidCurrent=" << toroidCurrent() << ")" );
    // empty the current map first
    clearMap(tls);

    // determine the map to load
    std::string mapFile("");
    if ( solenoidOn() && toroidOn() ) {
        mapFile = m_fullMapFilename;
    } else if ( solenoidOn() ) {
        mapFile = m_soleMapFilename;
    } else if ( toroidOn() ) {
        mapFile = m_toroMapFilename;
    } else {
        // all magnets OFF. no need to read map
        return StatusCode::SUCCESS;
    }
    // find the path to the map file
    std::string resolvedMapFile = PathResolver::find_file( mapFile.c_str(), "DATAPATH" );
    if ( resolvedMapFile == "" ) {
        ATH_MSG_ERROR( "Field map file " << mapFile << " not found" );
        return StatusCode::FAILURE;
    }
    // read the map file
    if ( readMap( resolvedMapFile.c_str() ).isFailure() ) {
        ATH_MSG_ERROR( "Something went wrong while trying to read the field map " << resolvedMapFile );
        return StatusCode::FAILURE;
    }
    ATH_MSG_INFO( "Initialized the field map from " << resolvedMapFile );
    // scale magnet current as needed
    if (!m_lockMapCurrents)
      scaleField();
    else
      ATH_MSG_INFO( "Currents are NOT scaled - using map values sole=" << m_mapSoleCurrent << " toro=" << m_mapToroCurrent );

    return StatusCode::SUCCESS;
}

void MagField::AtlasFieldSvc::scaleField()
{
    BFieldZone *solezone(0);
    //
    if ( solenoidOn() ) {
        solezone = findZoneSlow( 0.0, 0.0, 0.0 );
        if ( m_mapSoleCurrent > 0.0 &&
             std::abs( solenoidCurrent()/m_mapSoleCurrent - 1.0 ) > 0.001 ) {
            // scale the field in the solenoid zone
            double factor = solenoidCurrent()/m_mapSoleCurrent;
            solezone->scaleField( factor );
            // remake the fast map
            buildZR();
            ATH_MSG_INFO( "Scaled the solenoid field by a factor " << factor );
        }
    }
    //
    if ( toroidOn() ) {
        if ( m_mapToroCurrent > 0.0 &&
             std::abs( toroidCurrent()/m_mapToroCurrent - 1.0 ) > 0.001 ) {
            // scale the field in all zones except for the solenoid zone
            double factor = toroidCurrent()/m_mapToroCurrent;
            for ( unsigned i = 0; i < m_zone.size(); i++ ) {
                if ( &(m_zone[i]) != solezone ) {
                    m_zone[i].scaleField( factor );
                }
            }
            ATH_MSG_INFO( "Scaled the toroid field by a factor " << factor );
        }
    }
}

/** framework methods */
StatusCode MagField::AtlasFieldSvc::finalize()
{
    //ATH_MSG_INFO( "finalize() ..." );
    //
    // finalization code would go here
    //
    ATH_MSG_INFO( "finalize() successful" );
    return StatusCode::SUCCESS;
}

/* void MagField::AtlasFieldSvc::getFieldStandard(const double *xyz, double *bxyz, double *deriv) */
void MagField::AtlasFieldSvc::getField(const double *xyz, double *bxyz, double *deriv) const
{
  const double &x(xyz[0]);
  const double &y(xyz[1]);
  const double &z(xyz[2]);
  double r = std::sqrt(x * x + y * y);
  double phi = std::atan2(y, x);

  // retrieve the thread-local storage
  AtlasFieldSvcTLS &tls = getAtlasFieldSvcTLS();
  BFieldCache &cache = tls.cache;

  // test if the TLS was initialized and the cache is valid
  if ( !tls.isInitialized || !cache.inside(z, r, phi) ) {
    // cache is invalid -> refresh cache
    if (!fillFieldCache(z, r, phi, tls)) {
      // caching failed -> outside the valid map volume
      // return default field (0.1 gauss)
      const double defaultB(0.1*CLHEP::gauss);
      bxyz[0] = bxyz[1] = bxyz[2] = defaultB;
      // return zero gradient if requested
      if ( deriv ) {
          for ( int i = 0; i < 9; i++ ) {
            deriv[i] = 0.;
          }
      }
      // check NaN in input
      if ( x!=x || y!=y || z!=z ) {
        ATH_MSG_WARNING( "getField() was called for xyz = " << x << "," << y << "," << z );
      }
      return;
    }
  }

  // do interpolation
  cache.getB(xyz, r, phi, bxyz, deriv);

  // add biot savart component
  if (tls.cond) {
    const size_t condSize = tls.cond->size();
    for (size_t i = 0; i < condSize; i++) {
      (*tls.cond)[i].addBiotSavart(xyz, bxyz, deriv);
    }
  }
} 

/*
void MagField::AtlasFieldSvc::getFieldManipulated(const double *xyz, double *bxyz, double *deriv)
{
  // this operation involves three steps:
  //   - first we move the point at which the field is evaluated
  //        ex1: solenoid translation by vector +a  =>  xyz -= a
  //        ex2: solenoid rotation R by angle +phi  =>  rotate xyz by -phi (inverse rotation R_inv)
  //   - then, we evaluate B in this new point
  //        ex1: B(-a)
  //        ex2: B(R_inv(xyz))
  //   - then, we change the field
  //        ex1: identity transformation 
  //        ex2: rotate the field properly
  
  // step 1
  double xyz_new[3];
  m_manipulator->modifyPosition(xyz, xyz_new);

  // step 2
  getFieldStandard(xyz_new, bxyz, deriv);

  // step 3
  m_manipulator->modifyField(bxyz, deriv);
}

void MagField::AtlasFieldSvc::getField(const double *xyz, double *bxyz, double *deriv) {
  (this->*this->getFieldActual)(xyz, bxyz, deriv);
}
*/

void MagField::AtlasFieldSvc::getFieldZR(const double *xyz, double *bxyz, double *deriv) const
{
  const double &x(xyz[0]);
  const double &y(xyz[1]);
  const double &z(xyz[2]);
  double r = sqrt(x * x + y * y);

  // get thread-local storage
  AtlasFieldSvcTLS &tls = getAtlasFieldSvcTLS();
  BFieldCacheZR &cacheZR = tls.cacheZR;

  // test if the TLS was initialized and the cache is valid
  if ( !tls.isInitialized || !cacheZR.inside(z, r) ) {
    // cache is invalid -> refresh cache
    if (!fillFieldCacheZR(z, r, tls)) {
      // caching failed -> outside the valid z-r map volume
      // call the full version of getField()
      getField(xyz, bxyz, deriv);
      return;
    }
  }

  // do interpolation
  cacheZR.getB(xyz, r, bxyz, deriv);
}

/** Query the interfaces. */
StatusCode MagField::AtlasFieldSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
    if ( IIncidentListener::interfaceID().versionMatch(riid) ) {
        *ppvInterface = dynamic_cast<IIncidentListener*>(this);
    } else if ( MagField::IMagFieldSvc::interfaceID().versionMatch(riid) ) {
        *ppvInterface = dynamic_cast<MagField::IMagFieldSvc*>(this);
    } else {
        // Interface is not directly available: try out a base class
        return Service::queryInterface(riid, ppvInterface);
    }
    addRef();
    return StatusCode::SUCCESS;
}

//
// Clear the map.
// Subsequent call should return zero magnetic field.
//
void MagField::AtlasFieldSvc::clearMap(AtlasFieldSvcTLS &tls)
{
    tls.cache.invalidate();
    tls.cacheZR.invalidate();

    tls.cond = nullptr;
    // Next lines clear m_zone, m_edge[3], m_edgeLUT[3], and m_zoneLUT and deallocate their memory.
    std::vector<BFieldZone>().swap(m_zone);
    for ( int i = 0; i < 3; i++ ) {
        std::vector<double>().swap(m_edge[i]);
        std::vector<int>().swap(m_edgeLUT[i]);
    }
    std::vector<const BFieldZone*>().swap(m_zoneLUT);
    // Next lines ensure findZone() will fail
    m_zmin = 0.0;
    m_zmax = -1.0;
    m_rmax = -1.0;
    m_nz = m_nr = m_nphi = 0;
    delete m_meshZR;
    m_meshZR = nullptr;
}

//
// Read the solenoid map from file.
// The filename must end with ".root".
//
StatusCode MagField::AtlasFieldSvc::readMap( const char* filename )
{
    if ( strstr(filename, ".root") == 0 ) {
        ATH_MSG_ERROR("input file name '" << filename << "' does not end with .root");
        return StatusCode::FAILURE;
    } 
    TFile* rootfile = new TFile(filename, "OLD");
    if ( ! rootfile ) {
        ATH_MSG_ERROR("failed to open " << filename);
        return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("reading the map from " << filename);
    if ( readMap(rootfile).isFailure() ) {
        ATH_MSG_ERROR("something went wrong while trying to read the ROOT field map file");
        return StatusCode::FAILURE;
    }

    rootfile->Close();
    delete rootfile;

    return StatusCode::SUCCESS;
}

//
// read an ASCII field map from istream
// return 0 if successful
// convert units m -> mm, and T -> kT
//
StatusCode MagField::AtlasFieldSvc::readMap( std::istream& input )
{
    const std::string myname("readMap()");
    // first line contains version, date, time
    std::string word;
    int version;
    int date;
    int time;
    input >> word >> version;
    if ( word != "FORMAT-VERSION" ) {
        ATH_MSG_ERROR( myname << ": found '" << word << "' instead of 'FORMAT-VERION'");
        return StatusCode::FAILURE;
    }
    if ( version < 5 || version > 6) {
        ATH_MSG_ERROR( myname << ": version number is " << version << " instead of 5 or 6");
        return StatusCode::FAILURE;
    }
    input >> word >> date;
    if ( word != "DATE" ) {
        ATH_MSG_ERROR( myname << ": found '" << word << "' instead of 'DATE'" );
        return 1;
    }
    input >> word >> time;
    if ( word != "TIME" ) {
        ATH_MSG_ERROR( myname << ": found '" << word << "' instead of 'TIME'" );
        return StatusCode::FAILURE;
    }

    // read and skip header cards
    int nheader;
    input >> word >> nheader;
    if ( word != "HEADERS" ) {
        ATH_MSG_ERROR( myname << ": found '" << word << "' instead of 'HEADERS'" );
        return StatusCode::FAILURE;
    }
    std::string restofline;
    getline( input, restofline );
    for ( int i = 0; i < nheader; i++ ) {
        std::string header;
        getline( input, header );
    }

    // read zone definitions
    int nzone;
    input >> word >> nzone;
    if ( word != "ZONES" ) {
        ATH_MSG_ERROR( myname << ": found '" << word << "' instead of 'ZONES'" );
        return StatusCode::FAILURE;
    }
    std::vector<int> jz(nzone), nz(nzone);
    std::vector<int> jr(nzone), nr(nzone);
    std::vector<int> jphi(nzone), nphi(nzone);
    std::vector<int> jbs(nzone), nbs(nzone);
    std::vector<int> jcoil(nzone), ncoil(nzone);
    std::vector<int> jfield(nzone), nfield(nzone);
    std::vector<int> jaux(nzone), naux(nzone);

    for ( int i = 0; i < nzone; i++ )
    {
        int id;
        int nrep, map; // unused
        double z1, z2, r1, r2, phi1, phi2;
        int nzrphi0; // unused
        double tol; // unused
        int mzn, mxsym, mrefl, mback; // unused
        double qz, qr, qphi; // unused
        double bscale;
        input >> id >> nrep;
	if ( version == 6 ) input >> map;
        input >> z1 >> z2 >> nz[i]
              >> r1 >> r2 >> nr[i]
              >> phi1 >> phi2 >> nphi[i]
              >> nzrphi0 >> tol
              >> jbs[i] >> nbs[i]
              >> jcoil[i] >> ncoil[i]
              >> jz[i] >> jr[i] >> jphi[i]
              >> jfield[i] >> nfield[i]
              >> mzn;
	if ( version == 6 ) input >> mxsym;
        input >> mrefl >> mback
              >> jaux[i] >> naux[i]
              >> qz >> qr >> qphi >> bscale;
        if ( id >= 0 ) { // remove dummy zone
            z1 *= CLHEP::meter;
            z2 *= CLHEP::meter;
            r1 *= CLHEP::meter;
            r2 *= CLHEP::meter;
            phi1 *= CLHEP::degree;
            phi2 *= CLHEP::degree;
            bscale *= CLHEP::tesla;
            BFieldZone zone( id, z1, z2, r1, r2, phi1, phi2, bscale );
            m_zone.push_back(zone);
        }
    }

    // read Biot-Savart data
    int nbiot;
    input >> word >> nbiot;
    if ( word != "BIOT" ) {
        ATH_MSG_ERROR( myname << ": found '" << word << "' instead of 'BIOT'" );
        return StatusCode::FAILURE;
    }
    std::vector<BFieldCond> bslist;
    for ( int i = 0; i < nbiot; i++ ) {
        char dummy; // unused
        char cfinite;
        double xyz1[3], xyz2[3];
        double phirot; // unused
        double curr;
        input >> dummy >> cfinite
              >> xyz1[0] >> xyz1[1] >> xyz1[2]
              >> xyz2[0] >> xyz2[1] >> xyz2[2]
              >> phirot >> curr;
        bool finite = ( cfinite == 'T' );
        for ( int j = 0; j < 3; j++ ) {
            xyz1[j] *= CLHEP::meter;
            if ( finite ) xyz2[j] *= CLHEP::meter;
        }
        BFieldCond bs( finite, xyz1, xyz2, curr );
        bslist.push_back(bs);
    }
    // attach them to the zones
    for ( unsigned i = 0; i < m_zone.size(); i++ ) {
        // copy the range that belongs to this zone
        for ( int j = 0; j < nbs[i]; j++ ) {
            // Fortran -> C conversion requires "-1"
            m_zone[i].appendCond( bslist[jbs[i]+j-1] );
        }
    }

    // read and skip coil data
    int nc;
    input >> word >> nc;
    if ( word != "COIL" ) {
        ATH_MSG_ERROR( myname << ": found '" << word << "' instead of 'COIL'" );
        return StatusCode::FAILURE;
    }
    getline( input, restofline );
    for ( int i = 0; i < nc; i++ ) {
        std::string coildata;
        getline( input, coildata );
    }

    // read and skip auxiliary array = list of subzones
    int nauxarr;
    input >> word >> nauxarr;
    if ( word != "AUXARR" ) {
        ATH_MSG_ERROR( myname << ": found '" << word << "' instead of 'AUXARR'" );
        return StatusCode::FAILURE;
    }
    if ( version == 6 ) input >> word; // skip 'T'
    for ( int i = 0; i < nauxarr; i++ ) {
        int aux;
        input >> aux;
    }

    // read mesh definition
    int nmesh;
    input >> word >> nmesh;
    if ( word != "MESH" ) {
        ATH_MSG_ERROR( myname << ": found '" << word << "' instead of 'MESH'" );
        return StatusCode::FAILURE;
    }
    std::vector<double> meshlist;
    for ( int i = 0; i < nmesh; i++ ) {
        double mesh;
        input >> mesh;
        meshlist.push_back(mesh);
    }
    // attach them to the zones
    for ( unsigned i = 0; i < m_zone.size(); i++ ) {
        m_zone[i].reserve( nz[i], nr[i], nphi[i] );
        for ( int j = 0; j < nz[i]; j++ ) {
            m_zone[i].appendMesh( 0, meshlist[jz[i]+j-1]*CLHEP::meter );
        }
        for ( int j = 0; j < nr[i]; j++ ) {
            m_zone[i].appendMesh( 1, meshlist[jr[i]+j-1]*CLHEP::meter );
        }
        for ( int j = 0; j < nphi[i]; j++ ) {
            m_zone[i].appendMesh( 2, meshlist[jphi[i]+j-1] );
        }
    }

    // read field values
    int nf, nzlist;
    std::string ftype, bytype;
    input >> word >> nf >> nzlist >> ftype >> bytype;
    if ( word != "FIELD" ) {
        ATH_MSG_ERROR( myname << ": found '" << word << "' instead of 'FIELD'" );
        return StatusCode::FAILURE;
    }
    if ( ftype != "I2PACK" ) {
        ATH_MSG_ERROR( myname << ": found '" << ftype << "' instead of 'I2PACK'" );
        return StatusCode::FAILURE;
    }
    if ( bytype != "FBYTE" ) {
        ATH_MSG_ERROR( myname << ": found '" << bytype << "' instead of 'FBYTE'" );
        return StatusCode::FAILURE;
    }
    // read zone by zone
    for ( int i = 0; i < nzlist; i++ ) {
        int izone, idzone, nfzone;
        input >> izone >> idzone >> nfzone;
        izone--; // fortran -> C++
        if ( idzone != m_zone[izone].id() ) {
            ATH_MSG_ERROR( myname << ": zone id " << idzone << " != " << m_zone[izone].id() );
            return 2;
        }

        std::vector<int> data[3];

        // for field data in 2 bytes
        for ( int j = 0; j < 3; j++ ) { // repeat z, r, phi
            int ierr = read_packed_data( input, data[j] );
            if ( ierr != 0 ) return ierr;
            for ( int k = 0; k < nfzone; k++ ) {
                // recover sign
                data[j][k] = ( data[j][k]%2==0 ) ? data[j][k]/2 : -(data[j][k]+1)/2;
                // second-order diff
                if ( k >= 2 ) data[j][k] += 2*data[j][k-1] - data[j][k-2];
            }
        }
        // store
        for ( int k = 0; k < nfzone; k++ ) {
            BFieldVector<short> B( data[0][k], data[1][k], data[2][k] );
            m_zone[izone].appendField( B );
        }

        // skip fbyte
        char c;
        while ( input.good() ) {
            input >> c;
            if ( input.eof() || c == '}' ) break;
        }
    }

    // build the LUTs and ZR zone
    buildLUT();
    buildZR();

    return StatusCode::SUCCESS;
}

//
// wrire the map to a ROOT file
//
void MagField::AtlasFieldSvc::writeMap( TFile* rootfile ) const
{
    if ( rootfile == 0 ) return; // no file
    if ( rootfile->cd() == false ) return; // could not make it current directory
    // define the tree
    TTree* tree = new TTree( "BFieldMap", "BFieldMap version 5" );
    TTree* tmax = new TTree( "BFieldMapSize", "Buffer size information" );
    int id;
    double zmin, zmax, rmin, rmax, phimin, phimax;
    double bscale;
    int ncond;
    bool *finite;
    double *p1x, *p1y, *p1z, *p2x, *p2y, *p2z;
    double *curr;
    int nmeshz, nmeshr, nmeshphi;
    double *meshz, *meshr, *meshphi;
    int nfield;
    short *fieldz, *fieldr, *fieldphi;

    // prepare arrays - need to know the maximum sizes
    unsigned maxcond(0), maxmeshz(0), maxmeshr(0), maxmeshphi(0), maxfield(0);
    for ( unsigned i = 0; i < m_zone.size(); i++ ) {
        maxcond = std::max( maxcond, m_zone[i].ncond() );
        maxmeshz = std::max( maxmeshz, m_zone[i].nmesh(0) );
        maxmeshr = std::max( maxmeshr, m_zone[i].nmesh(1) );
        maxmeshphi = std::max( maxmeshphi, m_zone[i].nmesh(2) );
        maxfield = std::max( maxfield, m_zone[i].nfield() );
    }
    // store the maximum sizes
    tmax->Branch( "maxcond", &maxcond, "maxcond/i");
    tmax->Branch( "maxmeshz", &maxmeshz, "maxmeshz/i");
    tmax->Branch( "maxmeshr", &maxmeshr, "maxmeshr/i");
    tmax->Branch( "maxmeshphi", &maxmeshphi, "maxmeshphi/i");
    tmax->Branch( "maxfield", &maxfield, "maxfield/i");
    tmax->Fill();
    // prepare buffers
    finite = new bool[maxcond];
    p1x = new double[maxcond];
    p1y = new double[maxcond];
    p1z = new double[maxcond];
    p2x = new double[maxcond];
    p2y = new double[maxcond];
    p2z = new double[maxcond];
    curr = new double[maxcond];
    meshz = new double[maxmeshz];
    meshr = new double[maxmeshr];
    meshphi = new double[maxmeshphi];
    fieldz = new short[maxfield];
    fieldr = new short[maxfield];
    fieldphi = new short[maxfield];
    // define the tree branches
    tree->Branch( "id", &id, "id/I" );
    tree->Branch( "zmin", &zmin, "zmin/D" );
    tree->Branch( "zmax", &zmax, "zmax/D" );
    tree->Branch( "rmin", &rmin, "rmin/D" );
    tree->Branch( "rmax", &rmax, "rmax/D" );
    tree->Branch( "phimin", &phimin, "phimin/D" );
    tree->Branch( "phimax", &phimax, "phimax/D" );
    tree->Branch( "bscale", &bscale, "bscale/D" );
    tree->Branch( "ncond", &ncond, "ncond/I" );
    tree->Branch( "finite", finite, "finite[ncond]/O" );
    tree->Branch( "p1x", p1x, "p1x[ncond]/D" );
    tree->Branch( "p1y", p1y, "p1y[ncond]/D" );
    tree->Branch( "p1z", p1z, "p1z[ncond]/D" );
    tree->Branch( "p2x", p2x, "p2x[ncond]/D" );
    tree->Branch( "p2y", p2y, "p2y[ncond]/D" );
    tree->Branch( "p2z", p2z, "p2z[ncond]/D" );
    tree->Branch( "curr", curr, "curr[ncond]/D" );
    tree->Branch( "nmeshz", &nmeshz, "nmeshz/I" );
    tree->Branch( "meshz", meshz, "meshz[nmeshz]/D" );
    tree->Branch( "nmeshr", &nmeshr, "nmeshr/I" );
    tree->Branch( "meshr", meshr, "meshr[nmeshr]/D" );
    tree->Branch( "nmeshphi", &nmeshphi, "nmeshphi/I" );
    tree->Branch( "meshphi", meshphi, "meshphi[nmeshphi]/D" );
    tree->Branch( "nfield", &nfield, "nfield/I" );
    tree->Branch( "fieldz", fieldz, "fieldz[nfield]/S" );
    tree->Branch( "fieldr", fieldr, "fieldr[nfield]/S" );
    tree->Branch( "fieldphi", fieldphi, "fieldphi[nfield]/S" );
    // loop over zones to write
    for ( unsigned i = 0; i < m_zone.size(); i++ ) {
        const BFieldZone z = m_zone[i];
        id = z.id();
        zmin = z.zmin(); zmax = z.zmax();
        rmin = z.rmin(); rmax = z.rmax();
        phimin = z.phimin(); phimax = z.phimax();
        bscale = z.bscale();
        ncond = z.ncond();
        for ( int j = 0; j < ncond; j++ ) {
            const BFieldCond c = z.cond(j);
            finite[j] = c.finite();
            p1x[j] = c.p1(0);
            p1y[j] = c.p1(1);
            p1z[j] = c.p1(2);
            p2x[j] = c.p2(0);
            p2y[j] = c.p2(1);
            p2z[j] = c.p2(2);
            curr[j] = c.curr();
        }
        nmeshz = z.nmesh(0);
        for ( int j = 0; j < nmeshz; j++ ) {
            meshz[j] = z.mesh(0,j);
        }
        nmeshr = z.nmesh(1);
        for ( int j = 0; j < nmeshr; j++ ) {
            meshr[j] = z.mesh(1,j);
        }
        nmeshphi = z.nmesh(2);
        for ( int j = 0; j < nmeshphi; j++ ) {
            meshphi[j] = z.mesh(2,j);
        }
        nfield = z.nfield();
        for ( int j = 0; j < nfield; j++ ) {
            const BFieldVector<short> f = z.field(j);
            fieldz[j] = f.z();
            fieldr[j] = f.r();
            fieldphi[j] = f.phi();
        }
        tree->Fill();
    }
    rootfile->Write();
    // clean up
    delete[] finite;
    delete[] p1x;
    delete[] p1y;
    delete[] p1z;
    delete[] p2x;
    delete[] p2y;
    delete[] p2z;
    delete[] curr;
    delete[] meshz;
    delete[] meshr;
    delete[] meshphi;
    delete[] fieldz;
    delete[] fieldr;
    delete[] fieldphi;
}

//
// read the map from a ROOT file.
// returns 0 if successful.
//
StatusCode MagField::AtlasFieldSvc::readMap( TFile* rootfile )
{
    if ( rootfile == 0 ) {
      // no file
      ATH_MSG_ERROR("readMap(): unable to read field map, no TFile given");
      return StatusCode::FAILURE;
    }
    if ( rootfile->cd() == false ) {
      // could not make it current directory
      ATH_MSG_ERROR("readMap(): unable to cd() into the ROOT field map TFile");
      return StatusCode::FAILURE; 
    }
    // open the tree
    TTree* tree = (TTree*)rootfile->Get("BFieldMap");
    if ( tree == 0 ) {
      // no tree
      ATH_MSG_ERROR("readMap(): TTree 'BFieldMap' does not exist in ROOT field map");
      return StatusCode::FAILURE;
    }
    int id;
    double zmin, zmax, rmin, rmax, phimin, phimax;
    double bscale;
    int ncond;
    bool *finite;
    double *p1x, *p1y, *p1z, *p2x, *p2y, *p2z;
    double *curr;
    int nmeshz, nmeshr, nmeshphi;
    double *meshz, *meshr, *meshphi;
    int nfield;
    short *fieldz, *fieldr, *fieldphi;
    // define the fixed-sized branches first
    tree->SetBranchAddress( "id", &id );
    tree->SetBranchAddress( "zmin", &zmin );
    tree->SetBranchAddress( "zmax", &zmax );
    tree->SetBranchAddress( "rmin", &rmin );
    tree->SetBranchAddress( "rmax", &rmax );
    tree->SetBranchAddress( "phimin", &phimin );
    tree->SetBranchAddress( "phimax", &phimax );
    tree->SetBranchAddress( "bscale", &bscale );
    tree->SetBranchAddress( "ncond", &ncond );
    tree->SetBranchAddress( "nmeshz", &nmeshz );
    tree->SetBranchAddress( "nmeshr", &nmeshr );
    tree->SetBranchAddress( "nmeshphi", &nmeshphi );
    tree->SetBranchAddress( "nfield", &nfield );
    // prepare arrays - need to know the maximum sizes
    // open the tree of buffer sizes (may not exist in old maps)
    unsigned maxcond(0), maxmeshz(0), maxmeshr(0), maxmeshphi(0), maxfield(0);
    TTree* tmax = (TTree*)rootfile->Get("BFieldMapSize");
    if ( tmax != 0 ) {
        tmax->SetBranchAddress( "maxcond", &maxcond );
        tmax->SetBranchAddress( "maxmeshz", &maxmeshz );
        tmax->SetBranchAddress( "maxmeshr", &maxmeshr );
        tmax->SetBranchAddress( "maxmeshphi", &maxmeshphi );
        tmax->SetBranchAddress( "maxfield", &maxfield );
        tmax->GetEntry(0);
    } else { // "BFieldMapSize" tree does not exist
        for ( int i = 0; i < tree->GetEntries(); i++ ) {
            tree->GetEntry(i);
            maxcond = std::max( maxcond, (unsigned)ncond );
            maxmeshz = std::max( maxmeshz, (unsigned)nmeshz );
            maxmeshr = std::max( maxmeshr, (unsigned)nmeshr );
            maxmeshphi = std::max( maxmeshphi, (unsigned)nmeshphi );
            maxfield = std::max( maxfield, (unsigned)nfield );
        }
    }
    finite = new bool[maxcond];
    p1x = new double[maxcond];
    p1y = new double[maxcond];
    p1z = new double[maxcond];
    p2x = new double[maxcond];
    p2y = new double[maxcond];
    p2z = new double[maxcond];
    curr = new double[maxcond];
    meshz = new double[maxmeshz];
    meshr = new double[maxmeshr];
    meshphi = new double[maxmeshphi];
    fieldz = new short[maxfield];
    fieldr = new short[maxfield];
    fieldphi = new short[maxfield];
    // define the variable length branches
    tree->SetBranchAddress( "finite", finite );
    tree->SetBranchAddress( "p1x", p1x );
    tree->SetBranchAddress( "p1y", p1y );
    tree->SetBranchAddress( "p1z", p1z );
    tree->SetBranchAddress( "p2x", p2x );
    tree->SetBranchAddress( "p2y", p2y );
    tree->SetBranchAddress( "p2z", p2z );
    tree->SetBranchAddress( "curr", curr );
    tree->SetBranchAddress( "meshz", meshz );
    tree->SetBranchAddress( "meshr", meshr );
    tree->SetBranchAddress( "meshphi", meshphi );
    tree->SetBranchAddress( "fieldz", fieldz );
    tree->SetBranchAddress( "fieldr", fieldr );
    tree->SetBranchAddress( "fieldphi", fieldphi );

    // reserve the space for m_zone so that it won't move as the vector grows
    m_zone.reserve( tree->GetEntries() );
    // read all tree and store
    for ( int i = 0; i < tree->GetEntries(); i++ ) {
        tree->GetEntry(i);
        BFieldZone z( id, zmin, zmax, rmin, rmax, phimin, phimax, bscale );
        m_zone.push_back(z);
        m_zone.back().reserve( nmeshz, nmeshr, nmeshphi );
        for ( int j = 0; j < ncond; j++ ) {
            double p1[3], p2[3];
            p1[0] = p1x[j];
            p1[1] = p1y[j];
            p1[2] = p1z[j];
            p2[0] = p2x[j];
            p2[1] = p2y[j];
            p2[2] = p2z[j];
            BFieldCond cond( finite[j], p1, p2, curr[j] );
            m_zone.back().appendCond(cond);
        }
        for ( int j = 0; j < nmeshz; j++ ) {
            m_zone.back().appendMesh( 0, meshz[j] );
        }
        for ( int j = 0; j < nmeshr; j++ ) {
            m_zone.back().appendMesh( 1, meshr[j] );
        }
        for ( int j = 0; j < nmeshphi; j++ ) {
            m_zone.back().appendMesh( 2, meshphi[j] );
        }
        for ( int j = 0; j < nfield; j++ ) {
            BFieldVector<short> field( fieldz[j], fieldr[j], fieldphi[j] );
            m_zone.back().appendField( field );
        }
    }
    // clean up
    tree->Delete();
    delete[] finite;
    delete[] p1x;
    delete[] p1y;
    delete[] p1z;
    delete[] p2x;
    delete[] p2y;
    delete[] p2z;
    delete[] curr;
    delete[] meshz;
    delete[] meshr;
    delete[] meshphi;
    delete[] fieldz;
    delete[] fieldr;
    delete[] fieldphi;
    // build the LUTs
    buildLUT();
    buildZR();

    return StatusCode::SUCCESS;
}

//
// utility function used by readMap()
//
int MagField::AtlasFieldSvc::read_packed_data( std::istream& input, std::vector<int>& data ) const
{
    const std::string myname("BFieldMap::read_packed_data()");

    data.resize(0);
    char mode = 'u';
    char c;
    while ( input.good() ) {
        input >> c;
        if ( input.eof() ) return 0;
        else if (c == '}') { // end of record
            return 0;
        }
        else if (c == 'z') { // series of zeros
            int n;
            int ierr = read_packed_int( input, n );
            if ( ierr != 0 ) return ierr;
            for ( int i = 0; i < n; i++ ) {
                data.push_back(0);
            }
        }
        else if (c >= 'u' && c <= 'y') { // mode change
            mode = c;
        }
        else if (c <= ' ' || c > 'z') {
            ATH_MSG_ERROR( myname << ": unexpected letter '" << c << "' in input" );
            return 3;
        }
        else { // normal letter in the range '!' - 't'
            switch (mode) {
            case 'u':
                {
                    input.putback( c );
                    int n;
                    int ierr = read_packed_int( input, n );
                    if ( ierr != 0 ) return ierr;
                    data.push_back(n);
                }
                break;
            case 'v':
                {
                    int n = c - '!';
                    for ( int i = 0; i < 4; i++ ) {
                        input >> c;
                        data.push_back(c - '!' + 84*(n%3));
                        n = n/3;
                    }
                }
                break;
            case 'w':
                data.push_back(c - '!');
                break;
            case 'x':
                {
                    int n = c - '!';
                    data.push_back(n/9);
                    data.push_back(n%9);
                }
                break;
            case 'y':
                {
                    int n = c - '!';
                    data.push_back(n/27);
                    data.push_back((n/9)%3);
                    data.push_back((n/3)%3);
                    data.push_back(n%3);
                }
                break;
            }
        }
    }
    return 0;
}

//
// utility function used by read_packed_data()
//
int MagField::AtlasFieldSvc::read_packed_int( std::istream &input, int &n ) const
{
    const std::string myname("BFieldMap::read_packed_int()");
    n = 0;
    char c;
    input >> c;
    while ( c >= '!' && c <= 'J') {
        n = 42*n + c - '!';
        input >> c;
    }
    if ( c >= 'K' && c <= 't' ) {
        n = 42*n + c - 'K';
    } else {
        ATH_MSG_ERROR( myname << ": unexpected letter '" << c << "' in input" );
        return 4;
    }
    return 0;
}

//
// Search for the zone that contains a point (z, r, phi)
// This is a linear-search version, used only to construct the LUT.
//
BFieldZone* MagField::AtlasFieldSvc::findZoneSlow( double z, double r, double phi )
{
    for ( int j = m_zone.size()-1; j >= 0; --j ) {
        if ( m_zone[j].inside( z, r, phi ) ) return &m_zone[j];
    }
    return 0;
}

//
// Build the look-up table used by FindZone().
// Called by readMap()
// It also calls buildLUT() for all zones.
//
void MagField::AtlasFieldSvc::buildLUT()
{
    // make lists of (z,r,phi) edges of all zones
    for ( int j = 0; j < 3; j++ ) { // z, r, phi
        for ( unsigned i = 0; i < m_zone.size(); i++ ) {
            double e[2];
            e[0] = m_zone[i].min(j);
            e[1] = m_zone[i].max(j);
            for ( int k = 0; k < 2; k++ ) {
                // for the phi edge, fit into [-pi,pi]
                if ( j==2 && e[k] > M_PI ) e[k] -= 2.0*M_PI;
                m_edge[j].push_back(e[k]);
            }
        }
        // add -pi and +pi to phi, just in case
        m_edge[2].push_back(-M_PI);
        m_edge[2].push_back(M_PI);
        // sort
        sort( m_edge[j].begin(), m_edge[j].end() );
        // remove duplicates
        // must do this by hand to allow small differences due to rounding in phi
        int i = 0;
        for ( unsigned k = 1; k < m_edge[j].size(); k++ ) {
            if ( fabs(m_edge[j][i] - m_edge[j][k]) < 1.0e-6 ) continue;
            m_edge[j][++i] = m_edge[j][k];
        }
        m_edge[j].resize( i+1 );
        // because of the small differences allowed in the comparison above,
        // m_edge[][] values do not exactly match the m_zone[] boundaries.
        // we have to fix this up in order to avoid invalid field values
        // very close to the zone boundaries.
        for ( unsigned i = 0; i < m_zone.size(); i++ ) {
            for ( unsigned k = 0; k < m_edge[j].size(); k++ ) {
                if ( fabs(m_zone[i].min(j) - m_edge[j][k]) < 1.0e-6 ) {
                    m_zone[i].adjustMin(j,m_edge[j][k]);
                }
                if ( fabs(m_zone[i].max(j) - m_edge[j][k]) < 1.0e-6 ) {
                    m_zone[i].adjustMax(j,m_edge[j][k]);
                }
            }
        }
    }
    // build LUT for edge finding
    for ( int j = 0; j < 3; j++ ) { // z, r, phi
        // find the size of the smallest interval
        double width = m_edge[j].back() - m_edge[j].front();
        double q(width);
        for ( unsigned i = 0; i < m_edge[j].size()-1; i++ ) {
            q = std::min( q, m_edge[j][i+1] - m_edge[j][i] );
        }
        // find the number of cells in the LUT
        int n = int(width/q) + 1;
        q = width/(n+0.5);
        m_invq[j] = 1.0/q;
        n++;
        // fill the LUT
        int m = 0;
        for ( int i = 0; i < n; i++ ) { // index of LUT
            if ( q*i+m_edge[j].front() > m_edge[j][m+1] ) m++;
            m_edgeLUT[j].push_back(m);
        }
    }
    // store min/max for speedup
    m_zmin=m_edge[0].front();
    m_zmax=m_edge[0].back();
    m_rmax=m_edge[1].back();
    // build LUT for zone finding
    m_nz = m_edge[0].size() - 1;
    m_nr = m_edge[1].size() - 1;
    m_nphi = m_edge[2].size() - 1;
    m_zoneLUT.reserve( m_nz*m_nr*m_nphi );
    for ( int iz = 0; iz < m_nz; iz++ ) {
        double z = 0.5*(m_edge[0][iz]+m_edge[0][iz+1]);
        for ( int ir = 0; ir < m_nr; ir++ ) {
            double r = 0.5*(m_edge[1][ir]+m_edge[1][ir+1]);
            for ( int iphi = 0; iphi < m_nphi; iphi++ ) {
                double phi = 0.5*(m_edge[2][iphi]+m_edge[2][iphi+1]);
                const BFieldZone* zone = findZoneSlow( z, r, phi );
                m_zoneLUT.push_back( zone );
            }
        }
    }
    // build LUT in each zone
    for ( unsigned i = 0; i < m_zone.size(); i++ ) {
        m_zone[i].buildLUT();
    }
}

//
// Build the z-r 2d map for fast solenoid field
//
void MagField::AtlasFieldSvc::buildZR()
{
    // delete if previously allocated
    delete m_meshZR;

    // find the solenoid zone
    // solenoid zone always covers 100 < R < 1000, but not necessarily R < 100
    // so we search for the zone that contains a point at R = 200, Z = 0
    const BFieldZone *solezone = findZone( 0.0, 200.0, 0.0 );

    // instantiate the new ZR map with the same external coverage as the solenoid zone
    // make sure R = 0 is covered
    m_meshZR = new BFieldMeshZR( solezone->zmin(), solezone->zmax(), 0.0, solezone->rmax() );

    // reserve the right amount of memroy
    unsigned nmeshz = solezone->nmesh(0);
    unsigned nmeshr = solezone->nmesh(1);
    if ( solezone->rmin() > 0.0 ) nmeshr++;
    m_meshZR->reserve( nmeshz, nmeshr );

    // copy the mesh structure in z/r
    // take care of R = 0 first
    if ( solezone->rmin() > 0.0 ) {
        m_meshZR->appendMesh( 1, 0.0 );
    }
    // copy the rest
    for ( int i = 0; i < 2; i++ ) { // z, r
        for ( unsigned j = 0; j < solezone->nmesh(i); j++ ) { // loop over mesh points
            m_meshZR->appendMesh( i, solezone->mesh( i, j ) );
        }
    }

    // loop through the mesh and compute the phi-averaged field
    for ( unsigned iz = 0; iz < m_meshZR->nmesh(0); iz++ ) { // loop over z
        double z = m_meshZR->mesh( 0, iz );
        for ( unsigned ir = 0; ir < m_meshZR->nmesh(1); ir++ ) { // loop over r
            double r = m_meshZR->mesh( 1, ir );
            const int nphi(200); // number of phi slices to average
            double Br = 0.0;
            double Bz = 0.0;
            for ( int iphi = 0; iphi < nphi; iphi++ ) {
                double phi = double(iphi)/double(nphi)*2.*M_PI;
                double xyz[3];
                xyz[0] = r*cos(phi);
                xyz[1] = r*sin(phi);
                xyz[2] = z;
                double B[3];
                solezone->getB( xyz, B, 0 );
                Br += B[0]*cos(phi) + B[1]*sin(phi);
                Bz += B[2];
            }
            Br *= 1.0/double(nphi);
            Bz *= 1.0/double(nphi);
            m_meshZR->appendField( BFieldVectorZR( Bz, Br ) );
        }
    }

    // build the internal LUT
    m_meshZR->buildLUT();
}

//
// Approximate memory footprint
//
int MagField::AtlasFieldSvc::memSize() const
{
    int size = 0;
    size += sizeof(BFieldCache);
    size += sizeof(BFieldCacheZR);
    for ( unsigned i = 0; i < m_zone.size(); i++ ) {
        size += m_zone[i].memSize();
    }
    for ( int i = 0; i < 3; i++ ) {
        size += sizeof(double)*m_edge[i].capacity();
        size += sizeof(int)*m_edgeLUT[i].capacity();
    }
    size += sizeof(BFieldZone*)*m_zoneLUT.capacity();
    if ( m_meshZR ) {
        size += m_meshZR->memSize();
    }
    return size;
}

