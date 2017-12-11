/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONSYSTEMEXTENSIONTOOL_H
#define MUON_MUONSYSTEMEXTENSIONTOOL_H

#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "xAODTracking/TrackParticle.h"

#include "TrkParameters/TrackParameters.h"

#include "MuonIdHelpers/MuonStationIndex.h"
#include "MuonLayerEvent/MuonLayerSurface.h"
#include "MuonDetDescrUtils/MuonSectorMapping.h"
#include "MuonRecToolInterfaces/IMuonSystemExtensionTool.h"



namespace Trk {
  class Surface;
  class IExtrapolator;
  class IParticleCaloExtensionTool;
}

namespace Muon {

  class MuonIdHelperTool;
  class MuonSystemExtension;


  class MuonSystemExtensionTool :  virtual public IMuonSystemExtensionTool, public AthAlgTool {
  public:
    typedef std::vector< MuonLayerSurface > SurfaceVec;

    /** Default AlgTool functions */
    MuonSystemExtensionTool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~MuonSystemExtensionTool();
    StatusCode initialize();
    StatusCode finalize();

    /** get muon system extension */
    bool muonSystemExtension(  const xAOD::TrackParticle& indetTrackParticle, const MuonSystemExtension*& muonSystemExtention ) const;

  private:
    /** initialize geometry */    
    bool initializeGeometry();
    bool initializeGeometryBarrel( int sector, const Amg::AngleAxis3D& sectorRotation );
    bool initializeGeometryEndcap( int sector, MuonStationIndex::DetectorRegionIndex regionIndex, const Amg::AngleAxis3D& sectorRotation );


    /** get surfaces to be intersected for a given start parameters */
    SurfaceVec getSurfacesForIntersection( const Trk::TrackParameters& muonEntryPars ) const;


    /** tool handles */
    ToolHandle<MuonIdHelperTool>   m_idHelper; 
    ToolHandle <Trk::IParticleCaloExtensionTool> m_caloExtensionTool; 
    ToolHandle<Trk::IExtrapolator> m_extrapolator; 
    
    /** reference surfaces per region and sector */
    std::vector< std::vector< SurfaceVec > > m_referenceSurfaces;

    /** sector mapping helper */
    MuonSectorMapping m_sectorMapping;

  };
}



#endif
 

