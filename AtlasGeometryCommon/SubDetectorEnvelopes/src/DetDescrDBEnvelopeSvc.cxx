/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DetDescrDBEnvelopeSvc.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "SubDetectorEnvelopes/DetDescrDBEnvelopeSvc.h"

// framework includes
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"

// CLHEP includes
#include "CLHEP/Units/SystemOfUnits.h"

// Database includes
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"

// AtlasDetDescr
#include "AtlasDetDescr/AtlasRegionHelper.h"

// GeoModel
#include "GeoModelInterfaces/IGeoModelSvc.h"


/** Constructor */
DetDescrDBEnvelopeSvc::DetDescrDBEnvelopeSvc(const std::string& name, ISvcLocator* svc) :
  AthService(name,svc),
  m_dbAccess(0),
  m_geoModel(0),
  m_atlasNode("ATLAS"),
  m_atlasVersionTag("AUTO"),
  m_node(),
  m_rz(),
  m_rposz(),
  m_allowFallback(false),
  m_doFallback(false),
  m_fallbackR(),
  m_fallbackZ()
{
  declareProperty( "DBBeamPipeNode"  , m_node[AtlasDetDescr::fAtlasForward]="BeamPipeEnvelope"  );
  declareProperty( "DBInDetNode"     , m_node[AtlasDetDescr::fAtlasID]="InDetEnvelope"          );
  declareProperty( "DBCaloNode"      , m_node[AtlasDetDescr::fAtlasCalo]="CaloEnvelope"       );
  declareProperty( "DBMSNode"        , m_node[AtlasDetDescr::fAtlasMS]="MuonEnvelope"           );
  declareProperty( "DBCavernNode"    , m_node[AtlasDetDescr::fAtlasCavern]="CavernEnvelope" );

  // in case something goes wrong with the DB, the user can define 
  // fallback RZ values
  declareProperty( "EnableFallback"        , m_allowFallback                 );
  declareProperty( "FallbackBeamPipeR"     , m_fallbackR[AtlasDetDescr::fAtlasForward] );
  declareProperty( "FallbackBeamPipeZ"     , m_fallbackZ[AtlasDetDescr::fAtlasForward] );
  declareProperty( "FallbackInDetR"        , m_fallbackR[AtlasDetDescr::fAtlasID]      );
  declareProperty( "FallbackInDetZ"        , m_fallbackZ[AtlasDetDescr::fAtlasID]      );
  declareProperty( "FallbackCaloR"         , m_fallbackR[AtlasDetDescr::fAtlasCalo]    );
  declareProperty( "FallbackCaloZ"         , m_fallbackZ[AtlasDetDescr::fAtlasCalo]    );
  declareProperty( "FallbackMuonR"         , m_fallbackR[AtlasDetDescr::fAtlasMS]      );
  declareProperty( "FallbackMuonZ"         , m_fallbackZ[AtlasDetDescr::fAtlasMS]      );
  declareProperty( "FallbackCavernR"       , m_fallbackR[AtlasDetDescr::fAtlasCavern]  );
  declareProperty( "FallbackCavernZ"       , m_fallbackZ[AtlasDetDescr::fAtlasCavern]  );
}


DetDescrDBEnvelopeSvc::~DetDescrDBEnvelopeSvc()
{
  // free memory
  // TODO :)
}


/** enable fallback solution (if allowed) */
bool DetDescrDBEnvelopeSvc::enableFallback()
{
  // fallback solution is already enabled
  if (m_doFallback) return true;

  // fallback solution not enabled yet
  if (m_allowFallback) {
    ATH_MSG_INFO("Unable to retrieve envelope definitions from DDDB. Enabling Python-based fallback definitions.");
    m_doFallback=true;
  } else {
    ATH_MSG_ERROR("Unable to retrieve envelope definitions from DDDB and Python-based fallback definitions are disabled.");
    m_doFallback = false;
  }

  return m_doFallback;
}

/** Athena AthService initialize hook */
StatusCode DetDescrDBEnvelopeSvc::initialize()
{
  ATH_MSG_INFO("Initializing ...");

  // retrieve DataBase access service
  ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap
  if ( svcLocator->service("RDBAccessSvc", m_dbAccess).isFailure()) {
    ATH_MSG_ERROR("Could not locate RDBAccessSvc");
    if ( !enableFallback()) return StatusCode::FAILURE;
  }

  // connect to DDDB
  if ( !m_doFallback && m_dbAccess->connect()==false) {
    ATH_MSG_ERROR("Unable not connect to ATLASDD Database");
    if ( !enableFallback()) return StatusCode::FAILURE;
  }

  // retrieve the GeoModelSvc
  if ( !m_doFallback && svcLocator->service("GeoModelSvc", m_geoModel).isFailure()) {
    ATH_MSG_ERROR("Could not locate GeoModelSvc");
    if ( !enableFallback()) return StatusCode::FAILURE;
  }


  // (#) use the DB to retrieve the RZ values
  if ( !m_doFallback) {
    // geo model tag
    m_atlasVersionTag = m_geoModel->atlasVersion();
    if( m_atlasVersionTag == "AUTO") m_atlasVersionTag = "ATLAS-00";

    // cache the volume definitions locally
    for ( int region = AtlasDetDescr::fFirstAtlasRegion; region < AtlasDetDescr::fNumAtlasRegions; region++) {
      RZPairVector *curRZ = retrieveRZBoundaryOptionalFallback( m_node[region] , m_fallbackR[region] , m_fallbackZ[region] );
      if (!curRZ) {
        ATH_MSG_ERROR("Unable to retrieve subdetector envelope for detector region '" << 
                      AtlasDetDescr::AtlasRegionHelper::getName(region) << "'");
        return StatusCode::FAILURE;
      }
      m_rposz[region] = *curRZ;
    }

    // disconnect the DB access svc
    m_dbAccess->disconnect();
  }

  // (#) or use the fallback solution right away
  else {
    // cache the volume definitions locally
    for ( int region = AtlasDetDescr::fFirstAtlasRegion; region < AtlasDetDescr::fNumAtlasRegions; region++) {
      m_rposz[region] = *fallbackRZBoundary( m_fallbackR[region], m_fallbackZ[region] );
    }
  }

  // mirror the RZPairs provided in m_rposz to describe all corner points
  // in (r,z) space for each envelope volume
  for ( int region = AtlasDetDescr::fFirstAtlasRegion; region < AtlasDetDescr::fNumAtlasRegions; region++) {
    ATH_MSG_VERBOSE( "Envelope: positive-z region=" << region);
    m_rz[region] = *mirrorRZ( m_rposz[region] );
  }

  // debugging output:
  if (msgLvl(MSG::VERBOSE)) {
    for ( int region = AtlasDetDescr::fFirstAtlasRegion; region < AtlasDetDescr::fNumAtlasRegions; region++) {
      int numEntries = m_rz[region].size();
      ATH_MSG_VERBOSE( "Envelope: complete region=" << region);
      for ( int num = 0; num<numEntries; num++) {
        ATH_MSG_VERBOSE("   pos=" << num << " r=" << m_rz[region][num].first << " z="<< m_rz[region][num].second);
      }
    }
  }

  ATH_MSG_INFO("Initialize successful.");

  return StatusCode::SUCCESS;
}


/** Athena AthService finalize hook */
StatusCode DetDescrDBEnvelopeSvc::finalize()
{
  return StatusCode::SUCCESS;
}

/** retrieve and store the (r,z) values locally for the given DB node.
    if there are problems with retrieving this from DDDB,
    try the fallback approach if allowed */
RZPairVector *DetDescrDBEnvelopeSvc::retrieveRZBoundaryOptionalFallback(
                                                        std::string &dbNode,
                                                        FallbackDoubleVector   &r,
                                                        FallbackDoubleVector   &z)
{
  // try the DB approach to retrieve the (r,z) values
  RZPairVector *rz = retrieveRZBoundary(dbNode);

  // if 0 return value -> something went wrong
  if (!rz) {
    ATH_MSG_WARNING("Problems retrieving the envelope volume definition from DDDB node '"
                    << dbNode << "'.");
    // try fallback approach
    rz = enableFallback() ? fallbackRZBoundary(r, z) : 0;
    if (rz) ATH_MSG_INFO("Fallback envelope volume definition created for " << dbNode);
  }

  return rz;
}


/** retrieve and store the (r,z) values locally for the given DB node */
RZPairVector *DetDescrDBEnvelopeSvc::retrieveRZBoundary( std::string &node)
{
  // @TODO: implement checks and do output with the actual child tags taken
  // some output about the used tags
  //const std::string &detVersionTag = m_dbAccess->getChildTag( detNode /* child node */,
  //                                                            m_atlasVersionTag /* parent tag */,
  //                                                            m_atlasNode /* parent node */,
  //                                                            false /*don't fetch data*/ );
  //const std::string &parentVersionTag = m_dbAccess->getChildTag( parentNode,
  //                                                               detVersionTag,
  //                                                               detNode,
  //                                                               false);
  //const std::string &envVersionTag = m_dbAccess->getChildTag( envNode,
  //                                                            parentVersionTag,
  //                                                            parentNode,
  //                                                            false );
  //ATH_MSG_INFO( "DDDB tags used for envelope construction: "
  //              << m_atlasVersionTag << " / " << detVersionTag << " / "
  //              << parentVersionTag << " / " << envVersionTag );

  // get a handle on the DB entries (don't delete, handeled internally)
  const IRDBRecordset *envelopeRec = m_dbAccess->getRecordset( node,
                                                               m_atlasVersionTag,
                                                               m_atlasNode );

 /** @TODO: implement positioning, see:
    http://alxr.usatlas.bnl.gov/lxr/source/atlas/AtlasGeometryCommon/BeamPipeGeoModel/src/BeamPipeDetectorFactory.cxx?v=head#054
  const IRDBRecordset *DBPosition = m_dbAccess->getRecordset( m_dbPositionNode, m_atlasVersionTag, m_atlasNode ); */


  // entries in the database table
  size_t numEntries = envelopeRec ? envelopeRec->size() : 0;
  if ( !numEntries) {
    ATH_MSG_WARNING("No entries for table '" << node << "' in Detector Description DB. Will skip this envelope volume.");
    return 0;
  }

  // the current RZPairVector (to be filled in the loop)
  RZPairVector *rzVec = new RZPairVector();

  // retrieve data from the DB 
  IRDBRecordset::const_iterator recIt  = envelopeRec->begin();
  IRDBRecordset::const_iterator recEnd = envelopeRec->end();
  for ( ; recIt!=recEnd; ++recIt) {

    // read-in (r,z) duplet
    double curR = (*recIt)->getDouble("R") * CLHEP::mm;
    double curZ = (*recIt)->getDouble("Z") * CLHEP::mm;
    // store (r,z) duplet locally
    rzVec->push_back( RZPair(curR, curZ) );
  }

  return rzVec;
}


/** retrieve and store the (r,z) values locally for the given DB node */
RZPairVector *DetDescrDBEnvelopeSvc::fallbackRZBoundary( FallbackDoubleVector   &r,
                                                       FallbackDoubleVector   &z)
{
  // TODO: check for same length of r and z vectors!

  // the current RZPairVector (to be filled in the loop)
  RZPairVector *rzVec = new RZPairVector();

  // loop over the given pairs of (r,z) values
  for ( unsigned short pos=0; pos<r.size(); ++pos) {

    // read-in (r,z) duplet
    double curR = r[pos];
    double curZ = z[pos];
    // store (r,z) duplet locally
    rzVec->push_back( RZPair(curR, curZ) );
  }

  return rzVec;
}


/** Query the interfaces. */
StatusCode DetDescrDBEnvelopeSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {

 if ( IID_IEnvelopeDefSvc == riid ) 
    *ppvInterface = (IEnvelopeDefSvc*)this;
 else  {
   // Interface is not directly available: try out a base class
   return Service::queryInterface(riid, ppvInterface);
 }
 addRef();
 return StatusCode::SUCCESS;
}

