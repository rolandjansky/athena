/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DetDescrDBEnvelopeSvc.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef DETDESCRDBENVELOPESVC_H
#define DETDESCRDBENVELOPESVC_H

// STL includes
#include <string>
#include <vector>
#include <utility>

// GaudiKernel & Athena
#include "AthenaBaseComps/AthService.h"

// interface header file
#include "SubDetectorEnvelopes/IEnvelopeDefSvc.h"

// forward declarations
class IRDBAccessSvc;
class IGeoModelSvc;

/** datatype used for fallback solution */
typedef std::vector< double >           FallbackDoubleVector;


class DetDescrDBEnvelopeSvc : public extends<AthService, IEnvelopeDefSvc> {

  public:
    /** public AthService constructor */
    DetDescrDBEnvelopeSvc(const std::string& name, ISvcLocator* svc);

    /** Destructor */
    ~DetDescrDBEnvelopeSvc();

    /** AthService initialize method.*/
    StatusCode initialize();
    /** AthService finalize method */
    StatusCode finalize();

    /** return a vector of (r,z) pairs, defining the respective envelope */
    const RZPairVector& getRZBoundary( AtlasDetDescr::AtlasRegion region ) const
                      { return m_rz[region]; }

    /** return a vector of (r,z) pairs, defining the envelope on the z>0 region */
    const RZPairVector &getRPositiveZBoundary( AtlasDetDescr::AtlasRegion region ) const
                      { return m_rposz[region]; }

  private:
    /** retrieve and store the (r,z) values locally for the given DB node.
        if there are problems with retrieving this from DDDB,
        try the fallback approach if allowed */
    StatusCode retrieveRZBoundaryOptionalFallback( std::string           &dbNode,
                                                   FallbackDoubleVector  &r,
                                                   FallbackDoubleVector  &z,
                                                   RZPairVector          &rzVec);

    /** retrieve and store the (r,z) values locally for the given DB node */
    StatusCode retrieveRZBoundary( std::string &node, RZPairVector &rzVec);

    /** use the fallback approach (python arguments) to set the (r,z) values */
    StatusCode fallbackRZBoundary( FallbackDoubleVector  &r,
                                   FallbackDoubleVector  &z,
                                   RZPairVector          &rzVec);

    /** enable fallback solution:
     *  @return true if fallback mode is allowed, false if no fallback allowed */
    bool enableFallback();

    /** the DetectorDescription database access method */
    IRDBAccessSvc                     *m_dbAccess;

    /** ATLAS GeoModel */
    IGeoModelSvc                      *m_geoModel;

    /** main DDDB node for the ATLAS detector */
    std::string                        m_atlasNode;
    std::string                        m_atlasVersionTag;

    /** the names of the DB nodes for the respective AtlasRegion */
    std::string                        m_node[AtlasDetDescr::fNumAtlasRegions];

    /** internal (r,z) representation, one RZPairVector for each AtlasRegion */
    RZPairVector                       m_rz[AtlasDetDescr::fNumAtlasRegions];
    /** internal (r,z) representation for the positive z-side only,
     *  one RZPairVector for each AtlasRegion */
    RZPairVector                       m_rposz[AtlasDetDescr::fNumAtlasRegions];

    /** fallback solution, in case something goes wrong with the DB */
    bool                               m_allowFallback; //!< Python flag
    bool                               m_doFallback;
    FallbackDoubleVector               m_fallbackR[AtlasDetDescr::fNumAtlasRegions];
    FallbackDoubleVector               m_fallbackZ[AtlasDetDescr::fNumAtlasRegions];
};

#endif // DETDESCRDBENVELOPESVC_H

