/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DetDescrDBEnvelopeSvc.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "DetDescrDBEnvelopeSvc.h"

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
  base_class(name,svc),
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
      StatusCode sc = retrieveRZBoundaryOptionalFallback( m_node[region],
                                                          m_fallbackR[region],
                                                          m_fallbackZ[region],
                                                          m_rposz[region] );

      if (sc.isFailure()) {
        ATH_MSG_ERROR("Unable to retrieve subdetector envelope for detector region '" << 
                      AtlasDetDescr::AtlasRegionHelper::getName(region) << "'");
        return StatusCode::FAILURE;
      }
    }

  }

  // (#) or use the fallback solution right away
  else {
    // cache the volume definitions locally
    for ( int region = AtlasDetDescr::fFirstAtlasRegion; region < AtlasDetDescr::fNumAtlasRegions; region++) {
      StatusCode sc = fallbackRZBoundary( m_fallbackR[region], m_fallbackZ[region], m_rposz[region] );

      if (sc.isFailure()) {
        ATH_MSG_ERROR("Unable to retrieve sub-detector envelope in (r,z)-space for detector region '" <<
                      AtlasDetDescr::AtlasRegionHelper::getName(region) << "'");
        return StatusCode::FAILURE;
      }
    }
  }

  // mirror the RZPairs provided in m_rposz to describe all corner points
  // in (r,z) space for each envelope volume
  for ( int region = AtlasDetDescr::fFirstAtlasRegion; region < AtlasDetDescr::fNumAtlasRegions; region++) {
    ATH_MSG_VERBOSE( "Envelope: positive-z region=" << region);
    mirrorRZ( m_rposz[region], m_rz[region] );
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
StatusCode DetDescrDBEnvelopeSvc::retrieveRZBoundaryOptionalFallback( std::string           &dbNode,
                                                                      FallbackDoubleVector  &r,
                                                                      FallbackDoubleVector  &z,
                                                                      RZPairVector          &rzVec)
{
  // clear the output RZPairVector
  rzVec.clear();

  // try the DB approach to retrieve the (r,z) values
  StatusCode sc = retrieveRZBoundary(dbNode, rzVec);

  // if empty vector returned -> unsuccessfully read DDDB
  if (sc.isFailure()) {
    ATH_MSG_DEBUG("Will try reading Python-based envelope definition for '" << dbNode << "'.");

    // try fallback approach
    sc = enableFallback() ? fallbackRZBoundary(r, z, rzVec) : StatusCode::FAILURE;
    if (sc.isFailure()) {
      ATH_MSG_WARNING("Could not create envelope volume for '" << dbNode << "'.");
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO("Sucessfully read Python-based envelope definition for '" << dbNode << "'.");
    }
  }

  return StatusCode::SUCCESS;
}


/** retrieve and store the (r,z) values locally for the given DB node */
StatusCode DetDescrDBEnvelopeSvc::retrieveRZBoundary( std::string  &node,
                                                      RZPairVector &rzVec)
{
  // clear the output RZPairVector
  rzVec.clear();

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
  IRDBRecordset_ptr envelopeRec = m_dbAccess->getRecordsetPtr( node,
                                                               m_atlasVersionTag,
                                                               m_atlasNode );

 /** @TODO: implement positioning, see:
    http://alxr.usatlas.bnl.gov/lxr/source/atlas/AtlasGeometryCommon/BeamPipeGeoModel/src/BeamPipeDetectorFactory.cxx?v=head#054
  const IRDBRecordset *DBPosition = m_dbAccess->getRecordset( m_dbPositionNode, m_atlasVersionTag, m_atlasNode ); */


  // entries in the database table
  size_t numEntries = envelopeRec ? envelopeRec->size() : 0;
  if ( !numEntries) {
    ATH_MSG_INFO("No entries for table '" << node << "' in Detector Description Database (DDDB). Maybe you are using Python-based envelope definitions...");
    return StatusCode::RECOVERABLE;
  }

  // retrieve data from the DB 
  IRDBRecordset::const_iterator recIt  = envelopeRec->begin();
  IRDBRecordset::const_iterator recEnd = envelopeRec->end();
  for ( ; recIt!=recEnd; ++recIt) {

    // read-in (r,z) duplet
    double curR = (*recIt)->getDouble("R") * CLHEP::mm;
    double curZ = (*recIt)->getDouble("Z") * CLHEP::mm;
    // store (r,z) duplet locally
    rzVec.push_back( RZPair(curR, curZ) );
  }

  return StatusCode::SUCCESS;
}


/** retrieve and store the (r,z) values locally for the given DB node */
StatusCode DetDescrDBEnvelopeSvc::fallbackRZBoundary( FallbackDoubleVector &r,
                                                      FallbackDoubleVector &z,
                                                      RZPairVector &rzVec)
{
  unsigned short len = r.size();

  // r and z vectors must have same length
  if ( len != z.size() ) {
    ATH_MSG_ERROR("Unable to construct fallback envelope definition in (r,z) space, as the provided r and z vectors have different length");
    rzVec.clear();
    return StatusCode::FAILURE;
  }

  // loop over the given pairs of (r,z) values
  for ( unsigned short pos=0; pos<len; ++pos) {

    // read-in (r,z) duplet
    double curR = r[pos];
    double curZ = z[pos];
    // store (r,z) duplet locally
    rzVec.push_back( RZPair(curR, curZ) );
  }

  return StatusCode::SUCCESS;
}


