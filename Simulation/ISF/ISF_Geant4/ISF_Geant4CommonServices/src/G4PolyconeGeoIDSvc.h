/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// G4PolyconeGeoIDSvc.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_GEANT4COMMONSERVICES_G4POLYCONEGEOIDSVC_H
#define ISF_GEANT4COMMONSERVICES_G4POLYCONEGEOIDSVC_H 1

// framework includes
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthService.h"

// stl includes
#include <vector>
#include <string>

// ISF includes
#include "ISF_Interfaces/IGeoIDSvc.h"
// DetectorDescription
#include "AtlasDetDescr/AtlasRegion.h"

// forward declarations
class G4VSolid;
class IEnvelopeDefSvc;


namespace ISF {

  typedef std::vector<double>                DoubleVector;

  /** @class G4PolyconeGeoIDSvc

      @todo

      @author Elmar.Ritsch -at- cern.ch
     */
  class G4PolyconeGeoIDSvc : public extends<AthService, ISF::IGeoIDSvc> {
    public:
     /** Constructor with parameters */
     G4PolyconeGeoIDSvc(const std::string& name,ISvcLocator* svc);

     /** Destructor */
     ~G4PolyconeGeoIDSvc();

     // Athena algtool's Hooks
     StatusCode  initialize();
     StatusCode  finalize();

     /** A static filter that returns the AtlasRegion of the given position */
     AtlasDetDescr::AtlasRegion identifyGeoID(const Amg::Vector3D &pos) const;

     /** Checks if the given position (ISFParticle) is inside a given AtlasRegion */
     ISF::InsideType inside(const Amg::Vector3D &pos, AtlasDetDescr::AtlasRegion geoID) const;

     /** Find the AtlasRegion that the particle will enter with its next infinitesimal step
         along the given direction */
     AtlasDetDescr::AtlasRegion identifyNextGeoID(const Amg::Vector3D &pos, const Amg::Vector3D &dir) const;

    private:
     /** Retrieve and fill in the dimensions for the different AtlasRegion */
     StatusCode fillDimensions();

     /** @todo */
     void pushDimension(AtlasDetDescr::AtlasRegion geoID, double rin, double rou, double z);

     /** @todo */
     void pushDimension(AtlasDetDescr::AtlasRegion geoID, double r, double z);

     /** Retrieve and fill in the dimensions for the different AtlasRegion */
     StatusCode createVolume(AtlasDetDescr::AtlasRegion geoID);

     /** service providing the envelope dimensions for the different sub-detectors */
     ServiceHandle<IEnvelopeDefSvc>    m_envDefSvc;

     /** @todo: */
     G4VSolid                         *m_volume[AtlasDetDescr::fNumAtlasRegions];

     /** a quick way to convert G4 enum EInside to ISF::InsideType */
     ISF::InsideType                   m_typeConverter[ISF::fNumInsideTypes];
   };

}


#endif //> !ISF_GEANT4COMMONSERVICES_G4POLYCONEGEOIDSVC_H
