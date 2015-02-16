/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GeoIDSvc.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "GeoIDSvc.h"

// framework includes
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"

// DetectorDescription
#include "AtlasDetDescr/AtlasRegionHelper.h"

// STL includes
#include <algorithm>

/** Constructor **/
ISF::GeoIDSvc::GeoIDSvc(const std::string& name,ISvcLocator* svc) :
  AthService(name,svc),
  m_envDefSvc("ISF_ISFEnvelopeDefSvc", name),
  m_tolerance(1e-5),
  m_zBins(0),
  m_numZBins(0),
  m_radiusBins(0),
  m_maxRBins(AtlasDetDescr::fNumAtlasRegions+1)
{
  declareProperty("EnvelopeDefSvc",
                  m_envDefSvc,
                  "The EnvelopeDefinitionService describing the AtlasRegion boundaries.");
  declareProperty("Tolerance",
                  m_tolerance=1e-5,
                  "Estimated tolerance within which coordinates are considered being equal.");
}


/** Destructor **/
ISF::GeoIDSvc::~GeoIDSvc()
{
}


/** Query the interfaces. */
StatusCode ISF::GeoIDSvc::queryInterface(const InterfaceID& riid, void** ppvInterface){

  if ( ISF::IID_IGeoIDSvc == riid )
    *ppvInterface = (IGeoIDSvc*)this;
  else  {
    // Interface is not directly available: try out a base class
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}


// Athena algtool's Hooks
StatusCode  ISF::GeoIDSvc::initialize()
{
  ATH_MSG_INFO("initialize() ...");

  // retrieve envelope definition service
  if ( m_envDefSvc.retrieve().isFailure() ){
    ATH_MSG_FATAL( "Could not retrieve EnvelopeDefinition service. Abort.");
    return StatusCode::FAILURE;
  }

  // create internal volume representations for the given dimensions
  std::vector<double>                      tmpZBins[AtlasDetDescr::fNumAtlasRegions];
  std::map< size_t, double >               tmpRBins[AtlasDetDescr::fNumAtlasRegions];
  //std::map< size_t, std::array<RadiusGeoIDPair,2> > tmpRBins[AtlasDetDescr::fNumAtlasRegions];

  for (int geoID=AtlasDetDescr::fFirstAtlasRegion; geoID<AtlasDetDescr::fNumAtlasRegions; ++geoID) {

    // retrieve a list of (r,z) pairs for the z>0
    // side of the envelope
    RZPairList *curRZ = prepareRZPairs( AtlasDetDescr::AtlasRegion(geoID) );
    if ( curRZ->size()==0) {
      ATH_MSG_ERROR("Unable to create volume representation for geoID="<<geoID);
      delete curRZ; // makes coverity happy (CID 13321)
      return StatusCode::FAILURE;
    }
    ATH_MSG_VERBOSE("Found " << curRZ->size() << " (r,z) pairs with positive z for geoID=" << geoID);

    // TODO:
    // make sure the list of (r,z) pairs is supported by this GeoIDSvc implementation
    //   -> only cylindrical boundaries, no conical boundaries
    //   -> no undercuts
    //if ( checkRZPairs(curRZ).isFailure()) {
    //  ATH_MSG_ERROR("Unable to create volume representation for geoID="<<geoID);
    //  return StatusCode::FAILURE;
    //}

    ATH_MSG_DEBUG("Preparing " << curRZ->size() << " (r,z) pairs with positive z for geoID=" << geoID);
    while ( curRZ->size()) {
      //double minR = std::min( curRZ.front().first , curRZ.back().first );
      double maxR = std::max( curRZ->front().first , curRZ->back().first );
      double zFront = curRZ->front().second;
      double zBack  = curRZ->back().second;
      double minZ = std::min( zFront, zBack);

      ATH_MSG_VERBOSE(" minz=" << minZ << " zBack="<<zBack<<" zFront="<<zFront<<" maxR="<<maxR<<" curRZ->size()="<<curRZ->size());

      // remove items only on the one side of the list that
      // has the smaller z
      if      (zFront<zBack) curRZ->pop_front();
      else if (zBack<zFront) curRZ->pop_back();
      else {
        // both are equal in z
        curRZ->pop_front();
        if (curRZ->size()>0) curRZ->pop_back();
      }

      // register the boundary value of new bin in z
      size_t curZBin = tmpZBins[geoID].size();
      // skip this entry if minZ did not change
      if ( (curZBin>0) && ( fabs(tmpZBins[geoID][curZBin-1]-minZ) <m_tolerance) ) continue;

      tmpZBins[geoID].push_back(minZ);

      // construct the (radius^2,geoID) pairs
      //std::array<RadiusGeoIDPair, 2> radiusGeoIDs;
      //                radiusGeoIDs[0] = RadiusGeoIDPair(minR*minR, AtlasDetDescr::fUndefinedAtlasRegion);
      //if (curZBin==0) radiusGeoIDs[1] = RadiusGeoIDPair(maxR*maxR, AtlasDetDescr::fUndefinedAtlasRegion);
      //else            radiusGeoIDs[1] = RadiusGeoIDPair(maxR*maxR, AtlasDetDescr::AtlasRegion(geoID));
      //// add the (radius,geoID) pairs to the radiusBins map
      //// with the key curZBin
      //tmpRBins[geoID][curZBin] = radiusGeoIDs;
      tmpRBins[geoID][curZBin] = maxR*maxR;
    }
  } // loop over geoIDs

  ATH_MSG_DEBUG("Combining individual volumen boundaries in one common representation");
  // convert and combine the gathered tmpRBins, tmpZBins into
  // tmpZBinsGlobal, tmpRBinsGlobal which will have the same
  // format as the member variables m_zBins and m_radiusBins
  size_t  tmpZBinIndex[AtlasDetDescr::fNumAtlasRegions];
  for (int geoID=AtlasDetDescr::fFirstAtlasRegion; geoID<AtlasDetDescr::fNumAtlasRegions; ++geoID)
    tmpZBinIndex[geoID] = 0;
  std::vector<double>                    tmpZBinsGlobal;
  std::map< size_t, RadiusGeoIDPairSet > tmpRBinsGlobal;
  RadiusGeoIDPairSet curRadiusGeoIDs;
  double  minZ = 1e99;
  double  prevMinZ;
  while (true) {
    AtlasDetDescr::AtlasRegion  minZGeoID = AtlasDetDescr::fUndefinedAtlasRegion;

    // loop until a bin with minimum z value is found that
    // differs at least m_tolerance from previous minZ
    do {
      ATH_MSG_VERBOSE(" (re)starting search for minZ");
      prevMinZ = minZ;
      minZ = 1e99;
      curRadiusGeoIDs.clear();

      // loop over all geoIDs to find the bin with minimum z
      for (int geoID=AtlasDetDescr::fFirstAtlasRegion; geoID<AtlasDetDescr::fNumAtlasRegions; ++geoID) {
        size_t curTmpZBinIndex = tmpZBinIndex[geoID];
        // ignore the current tmpZBins in case we already reached
        // it's end
        if ( curTmpZBinIndex >= tmpZBins[geoID].size()) continue;
        double curZ = tmpZBins[geoID][curTmpZBinIndex];
        // curZ==0 -> skip this one
        if ( fabs(curZ)<m_tolerance) {
          (tmpZBinIndex[geoID])++;
        }
        // curZ is the smallest z so far
        //   -> register it (minZ, minZGeoID)
        //   -> increment iterator for current tmpZBinIt
        else if ( curZ<minZ) {
          minZ = curZ;
          minZGeoID = AtlasDetDescr::AtlasRegion(geoID);
        }
        double curR2 = tmpRBins[geoID][curTmpZBinIndex];

        ATH_MSG_VERBOSE("inserting pair: r="<< sqrt(curR2) <<
                        " geoID=" << geoID << " for minZ<=" << minZ);
        curRadiusGeoIDs.insert( RadiusGeoIDPair(curR2, AtlasDetDescr::AtlasRegion(geoID)) );
      } // geoID loop
      // iterate the tmpZBinIndex for the one geoID giving the smallest z
      (tmpZBinIndex[minZGeoID])++;
    } while ( fabs(minZ-prevMinZ)<m_tolerance );

    // nothing more to do, all tmpZBins are read out already
    if ( curRadiusGeoIDs.size()==0) break;
    ATH_MSG_VERBOSE("Current minZ=" << minZ <<
                    " curRadiusGeoIDs.size()=" << curRadiusGeoIDs.size() );

    // add the current z bin to the temporary std vector
    size_t curZBinGlobal = tmpZBinsGlobal.size();
    tmpZBinsGlobal.push_back(minZ);
    tmpRBinsGlobal[curZBinGlobal] = curRadiusGeoIDs;
  };

  ATH_MSG_DEBUG("Converting volume representation into faster format");
  // finally write the information in tmpZBinsGlobal and tmpRBinsGlobal
  // into the m_zBins and m_radiusBins
  m_numZBins   = tmpZBinsGlobal.size();
  m_zBins      = new double[m_numZBins];
  m_radiusBins = new RadiusGeoIDPair[(m_numZBins+1)*m_maxRBins];
  // loop over m_zBins
  for (int i=0; i<m_numZBins; i++) {
    m_zBins[i] = tmpZBinsGlobal[i];
    ATH_MSG_VERBOSE(" zBin=" << i << " for z<" << m_zBins[i]);

    RadiusGeoIDPairSet::iterator radIt    = tmpRBinsGlobal[i].begin();
    RadiusGeoIDPairSet::iterator radItEnd = tmpRBinsGlobal[i].end();
    size_t                       ii = 0;
    for ( ; radIt!=radItEnd; radIt++) {
      double curR = sqrt((*radIt).first);
      // skip this one if it has radius==0
      if (curR<m_tolerance) continue;

      ATH_MSG_VERBOSE("   radius<" << curR <<
                      " --> geoID=" << (*radIt).second);
      m_radiusBins[i*m_maxRBins+ii++] = (*radIt);
    }
    // fill the last radius bin with fUndefinedGeoID
    m_radiusBins[i*m_maxRBins+ii] = RadiusGeoIDPair( 1e99, AtlasDetDescr::fUndefinedAtlasRegion) ;
  };

  // validate the internal representation of r and z bins
  //if ( msgLevel(MSG::VERBOSE)) {
  //  ATH_MSG_VERBOSE("Internal (r,z) representation:");
  //  int zBin = 0;
  //  while ( (zBin<m_numZBins) ) {
  //    int radiusBin = 0;
  //    ATH_MSG_VERBOSE(" + zBin=" << zBin << " (z<"<<m_zBins[zBin]<<")");
  //    while ( (m_radiusBins[zBin*m_maxRBins+radiusBin].second!=AtlasDetDescr::fUndefinedAtlasRegion)) {
  //      ATH_MSG_VERBOSE("  - radiusBin=" << radiusBin <<
  //                      " r<"<< sqrt(m_radiusBins[zBin*m_maxRBins+radiusBin].first)<<
  //                      " geoID="<<m_radiusBins[zBin*m_maxRBins+radiusBin].second);
  //      radiusBin++;
  //    }
  //    zBin++;
  //  }
  //}
  // can be used for debugging:
  //HepGeom::Point3D<double> pos(0.,0.,999999.);
  //std::cout << " pos=" << pos << " geoID=" << identifyGeoID(pos)<<std::endl;
  //pos = HepGeom::Point3D<double>(99999., 0., 35.0);
  //std::cout << " pos=" << pos << " geoID=" << identifyGeoID(pos)<<std::endl;
  //pos = HepGeom::Point3D<double>(999999., 0., 35.0);
  //std::cout << " pos=" << pos << " geoID=" << identifyGeoID(pos)<<std::endl;
  //pos = HepGeom::Point3D<double>(0., 0., 3475.0);
  //std::cout << " pos=" << pos << " geoID=" << identifyGeoID(pos)<<std::endl;
  //pos = HepGeom::Point3D<double>(0., 0., 3474.9);
  //std::cout << " pos=" << pos << " geoID=" << identifyGeoID(pos)<<std::endl;
  //pos = HepGeom::Point3D<double>(0., 0., 3474.999);
  //std::cout << " pos=" << pos << " geoID=" << identifyGeoID(pos)<<std::endl;
  //pos = HepGeom::Point3D<double>(0., 0., 3474.);
  //std::cout << " pos=" << pos << " geoID=" << identifyGeoID(pos)<<std::endl;
  //pos = HepGeom::Point3D<double>(1147., 0., 3474.);
  //std::cout << " pos=" << pos << " geoID=" << identifyGeoID(pos)<<std::endl;
  //pos = HepGeom::Point3D<double>(1148., 0., 3475.);
  //std::cout << " pos=" << pos << " geoID=" << identifyGeoID(pos)<<std::endl;
  //pos = HepGeom::Point3D<double>(1149., 0., 3476.);
  //std::cout << " pos=" << pos << " geoID=" << identifyGeoID(pos)<<std::endl;

  ATH_MSG_INFO("initialize() successful");
  return StatusCode::SUCCESS;
}


StatusCode  ISF::GeoIDSvc::finalize() {
  ATH_MSG_INFO("finalize() ...");

  ATH_MSG_INFO("finalize() successful");
  return StatusCode::SUCCESS;
}


ISF::InsideType ISF::GeoIDSvc::inside(const Amg::Vector3D &pos, AtlasDetDescr::AtlasRegion geoID) const {

  // an arbitrary unitary direction with +1 in x,y,z
  // (following this direction will cross any AtlasRegion boundary if position is close to it in the first place)
  const Amg::Vector3D dir(1., 1., 1.);
  const Amg::Vector3D dirUnit( dir.unit() );

  // create particle positions which are a bit forward and a bit aft of the current position
  const Amg::Vector3D posFwd( pos + dirUnit*m_tolerance );
  const Amg::Vector3D posAft( pos - dirUnit*m_tolerance );

  AtlasDetDescr::AtlasRegion geoIDFwd = identifyGeoID(posFwd);
  AtlasDetDescr::AtlasRegion geoIDAft = identifyGeoID(posAft);

  // default case: particle is outside given geoID
  ISF::InsideType where = ISF::fOutside;

  // only if either the fwd or the aft step is inside the given geoID,
  // inside/surface cases need to be resolved
  if ( (geoID == geoIDFwd) || (geoID == geoIDAft) ) {
    // 1. inside
    if ( (geoIDFwd == geoIDAft)  ) {
      where = ISF::fInside;
      // 2. surface
    } else if ( geoIDFwd != geoIDAft ) {
      where = ISF::fSurface;
    }
  }

  return where;
}


AtlasDetDescr::AtlasRegion ISF::GeoIDSvc::identifyGeoID(const Amg::Vector3D &pos) const {

  // (1.) resolve z-bin number for the given z coordinate
  // TODO: test if maybe a binary search is faster than
  //       this linear search
  int zBin = 0;
  double z = fabs(pos.z()) ;
  while ( (zBin<m_numZBins) && (z>=m_zBins[zBin]) ) {
    zBin++;
  }
  //ATH_MSG_VERBOSE(" zBin=" << zBin << " (z<"<<m_zBins[zBin]<<")");

  // (2.) use zBin to resolve the geoID via the m_radiusBins 2d-array
  // TODO: test if maybe a binary search is faster than
  //       this linear search
  int radiusBin = 0;
  double r2 = pos.perp2();
  while ( (m_radiusBins[zBin*m_maxRBins+radiusBin].second!=AtlasDetDescr::fUndefinedAtlasRegion) &&
          (r2 >= m_radiusBins[zBin*m_maxRBins+radiusBin].first) ) {
    radiusBin++;
  }
  //ATH_MSG_VERBOSE(" radiusBin=" << radiusBin <<
  //                " (r<"<< sqrt(m_radiusBins[zBin*m_maxRBins+radiusBin].first)<<")");
  ////                " r>"<<m_radiusBins[zBin*m_maxRBins+radiusBin-1]);
  //ATH_MSG_VERBOSE("  --> geoID=" << m_radiusBins[zBin*m_maxRBins+radiusBin].second);

  // returns the found GeoID
  //  -> returns AtlasDetDescr::fUndefinedAtlasRegion in case not found
  return m_radiusBins[zBin*m_maxRBins+radiusBin].second;
}


AtlasDetDescr::AtlasRegion ISF::GeoIDSvc::identifyNextGeoID(const Amg::Vector3D &pos,
                                                            const Amg::Vector3D &dir) const {

  // push the particle a little and check the volume it's in
  AtlasDetDescr::AtlasRegion geoID = identifyGeoID( pos + dir.unit()*m_tolerance );

  // alternatively: before push-ing check the current position
  //AtlasDetDescr::AtlasRegion geoID = identifyGeoID( pos);
  //if (geoID == AtlasDetDescr::fUndefinedAtlasRegion) {
  //  const HepGeom::Vector3D<double> &posStep = pos + dir.unit();
  //  geoID = identifyGeoID( posStep);
  //}

  return geoID;
}

ISF::RZPairList* ISF::GeoIDSvc::prepareRZPairs( AtlasDetDescr::AtlasRegion geoID) {
  // fill RZPairLists with pairs of only positive z and only negative
  // z values respectively. it is important to keep the pairs ordered
  // in each list
  //
  RZPairList *positiveZ = new RZPairList();
  RZPairList *negativeZ = new RZPairList();

  // ensure a proper numeric value for geoID
  assertAtlasRegion( geoID);

  ATH_MSG_INFO( "Building envelope volume for '" << AtlasDetDescr::AtlasRegionHelper::getName(geoID)
                << "' (GeoID="<< geoID << ").");


  //
  // fill the RZPairLists
  //
  {
    // both RZPairList have to be empty
    if ( positiveZ->size() || negativeZ->size() ) {
      ATH_MSG_ERROR("Can not interpret the (r,z) pairs provided by the EnvelopeDefSvc");
      positiveZ->clear();
      negativeZ->clear();
      delete negativeZ;
      return positiveZ;
    }

    // retrieve the vector of (r,z) values from the EnvelopeDefSvc
    const RZPairVector &rz = m_envDefSvc->getRZBoundary( geoID );

    // used in the loop over the RZPairVector entries
    RZPairList *curRZList = 0;
    RZPairList::iterator curRZInsertIt;
    int signZChanges = 0;     // count the number of pos/neg transitions in z
    int prevZSign    = sign(rz.at(0).second);

    // loop over the (r,z) pairs in the given RZPairVector
    RZPairVector::const_iterator rzIt    = rz.begin();
    RZPairVector::const_iterator rzItEnd = rz.end();
    for (; rzIt!=rzItEnd; rzIt++) {
      const RZPair &curRZ = *rzIt;
      double   curZ = curRZ.second;
      int  curZSign = sign(curZ);

      ATH_MSG_VERBOSE( " signZChanges=" << signZChanges<<
                       " curZSign=" <<curZSign<<
                       " prevZSign=" << prevZSign<<
                       " curZ=" <<curZ <<
                       " curR=" <<curRZ.first );

      // select negative RZPairList to fill
      if ( curZSign==-1)     curRZList = negativeZ;
      // select positive RZPairList to fill
      else if ( curZSign==1) curRZList = positiveZ;
      // ignore 0.
      else {
        ATH_MSG_WARNING("Ignoring an (r,z) pair from the EnvelopeDefSvc with z==0.");
        continue;
      }

      // record if the sign has changed
      if ( curZSign != prevZSign) {
        signZChanges++;
        prevZSign = curZSign;
        // note the begin of the curRZList
        // will be used in case the sign has changed twice
        curRZInsertIt = curRZList->begin();
      }

      // (#) the z sign changed at max once in this (r,z) list so far
      //  -> this is the simple case since we are filling the
      //     two RZPairLists from the to each
      if ( signZChanges<2 ) {
        curRZList->push_back(curRZ);
      }

      // (#) the z sign changed twice in this (r,z) list so far
      //  -> insert the values from the front into the the current
      //     RZPairList
      else if ( signZChanges==2 ) {
        curRZList->insert( curRZInsertIt, curRZ );
      }

      // (#) the z sign changed more than twice
      //  -> no implementation yet to handle this freakin' complicated case
      else {
        ATH_MSG_ERROR("Unable to interpret (r,z) pairs for envelope definition provided by the EnvelopeDefSvc" );
        ATH_MSG_ERROR("  -> provided (r,z) pairs traverse the z==0 plane more than twice" );
        positiveZ->clear();
        negativeZ->clear();
        delete negativeZ;
        return positiveZ;
      }
    }
  }

  //
  // verify that the RZPairLists are symmetric around z==0.
  //
  if ( (!checkSymmetric( *positiveZ, *negativeZ)) ) {
    ATH_MSG_ERROR("(r,z) pairs received from EnvelopeDefSvc are not symmetric around z==0 plane");
    positiveZ->clear();
    negativeZ->clear();
    delete negativeZ;
    return positiveZ;
  }

  // add the z==0 entries to the RZList for the case of
  // volumes crossing the z==0 plane
  double frontR = positiveZ->front().first;
  double backR  = positiveZ->back().first;
  positiveZ->push_front( RZPair(frontR, 0.) );
  positiveZ->push_back ( RZPair(backR , 0.) );

  // clear the RZPairList for negative z values
  negativeZ->clear();
  delete negativeZ;

  // return the RZPairList for positive z values
  return positiveZ;
}


/** Check if given RZPairLists are symmetric around z==0 plane */
bool ISF::GeoIDSvc::checkSymmetric( RZPairList& positiveZ, RZPairList& negativeZ) {
  ATH_MSG_DEBUG("checking symmetry around z==0 plane for given (r,z) pairs");

  // require same size
  if ( positiveZ.size() != negativeZ.size()) {
    ATH_MSG_ERROR("Can not interpret the (r,z) pairs given by the EnvelopeDefSvc");
    ATH_MSG_ERROR("  -> different number of (r,z) pairs on either side of the z==0 plane");
    return false;
  }

  // some debug output
  //if ( msgLevel(MSG::VERBOSE)) {
  //  ATH_MSG_VERBOSE("(r,z) pairs on positive z side");
  //  RZPairList::iterator it    = positiveZ.begin();
  //  RZPairList::iterator itEnd = positiveZ.end();
  //  for ( ; it!=itEnd; it++) ATH_MSG_VERBOSE(" r=" << it->first << " z=" << it->second);
  //  ATH_MSG_VERBOSE("(r,z) pairs on negative z side");
  //  it    = negativeZ.begin();
  //  itEnd = negativeZ.end();
  //  for ( ; it!=itEnd; it++) ATH_MSG_VERBOSE(" r=" << it->first << " z=" << it->second);
  //}

  // iterate over lists to compare (r,z) value pairs
  RZPairList::iterator posIt      = positiveZ.begin();
  RZPairList::iterator posItEnd   = positiveZ.end();
  //RZPairList::iterator negItBegin = negativeZ.begin();
  RZPairList::iterator negIt      = negativeZ.end();

  while ( posIt != posItEnd) {
    negIt--;

    // require that deltaR and deltaZ are the same within bounds
    if ( fabs(negIt->first-posIt->first)>m_tolerance || fabs(negIt->second+posIt->second)>m_tolerance)
      return false;

    posIt++;
  }

  // all fine
  return true;
}
