/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// GeoIDSvc.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_SERVICES_GEOIDSVC_H
#define ISF_SERVICES_GEOIDSVC_H 1

// framework includes
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthService.h"

// STL includes
#include <vector>
#include <list>
#include <set>

// DetectorDescription
#include "AtlasDetDescr/AtlasRegion.h"

// ISF includes
#include "ISF_Interfaces/IGeoIDSvc.h"

// EnvelopeDefinitionService
#include "SubDetectorEnvelopes/IEnvelopeDefSvc.h" //Required for RZPair definition.

namespace ISF {

  typedef std::pair<double, AtlasDetDescr::AtlasRegion>          RadiusGeoIDPair;
  typedef std::list<RZPair>                                      RZPairList;

  struct SortByRadius {
      bool operator() (const RadiusGeoIDPair& lhs, const RadiusGeoIDPair& rhs) const
          {return lhs.first<rhs.first;}
  };
  typedef std::set<RadiusGeoIDPair, SortByRadius> RadiusGeoIDPairSet;

  /** @class GeoIDSvc
  
      A fast Athena service identifying the AtlasRegion a given position/particle is in.
  
      @author Elmar.Ritsch -at- cern.ch
     */
  class GeoIDSvc : public AthService, public ISF::IGeoIDSvc { 
    public: 
     /** Constructor with parameters */
     GeoIDSvc(const std::string& name,ISvcLocator* svc);

     /** Destructor */
     ~GeoIDSvc();

     /** Query the interfaces. */
     StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );

     // Athena algtool's Hooks
     StatusCode  initialize();
     StatusCode  finalize();

     /** A static filter that returns the SimGeoID of the given position */
     AtlasDetDescr::AtlasRegion    identifyGeoID(const Amg::Vector3D &pos) const;

     /** Checks if the given position (or ISFParticle) is inside a given SimGeoID */
     ISF::InsideType inside(const Amg::Vector3D &pos, AtlasDetDescr::AtlasRegion geoID) const;

     /** Find the SimGeoID that the particle will enter with its next infinitesimal step
         along the given direction */
     AtlasDetDescr::AtlasRegion identifyNextGeoID(const Amg::Vector3D &pos, const Amg::Vector3D &dir) const;

    private:
     /** service providing the envelope dimensions for the different sub-detectors */
     ServiceHandle<IEnvelopeDefSvc>    m_envDefSvc;

     RZPairList* prepareRZPairs( AtlasDetDescr::AtlasRegion geoID);

     /** Check if given RZPairLists are symmetric around z==0 plane */
     bool       checkSymmetric( RZPairList& positiveZ, RZPairList& negativeZ);

     template <typename T> inline int sign(T val);

     /** (estimated) tolerance within which coordinates are considered equal */
     double                 m_tolerance;

     /** */
     double                *m_zBins;
     int                    m_numZBins;
     /** m_radiusBins[m_numZBins][numCurRadiusBins] */
     RadiusGeoIDPair       *m_radiusBins;
     int                    m_maxRBins;
   }; 
  
//
// inline functions
//

/** a branchless signum function */
template <typename T> inline int ISF::GeoIDSvc::sign(T val) {
      return (T(0) < val) - (val < T(0));
}

} // ISF namespace

#endif //> !ISF_SERVICES_GEOIDSVC_H
