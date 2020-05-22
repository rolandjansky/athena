/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONPRDSELECTIONTOOL_H
#define MUON_MUONPRDSELECTIONTOOL_H

#include "MuonRecToolInterfaces/IMuonPRDSelectionTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonLayerEvent/MuonSystemExtension.h"
#include "MuonLayerEvent/MuonLayerPrepRawData.h"
#include "MuonLayerEvent/MuonLayerROTs.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonRecoValidationTool.h"
#include "MuonPrepRawData/MdtPrepDataCollection.h"

#include <vector>
#include <string>

namespace Muon {

  class MuonPRDSelectionTool : virtual public Muon::IMuonPRDSelectionTool,  public AthAlgTool {
  public:

    /** Default AlgTool functions */
    MuonPRDSelectionTool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~MuonPRDSelectionTool()=default;
    StatusCode initialize();

    /**IMuonPRDSelectionTool interface: calibrateAndSelect */   
    bool calibrateAndSelect( const MuonSystemExtension::Intersection& intersection, const MuonLayerPrepRawData& layerPrepRawData, MuonLayerROTs& layerROTs ) const;

    /** calibrate and select MDTs in a collection */
    bool calibrateAndSelectMdt( const MuonSystemExtension::Intersection& intersection, const MdtPrepDataCollection& prds, std::vector<const MdtDriftCircleOnTrack*>& rots ) const {
      for( MdtPrepDataCollection::const_iterator it = prds.begin(); it != prds.end();++it ){
        const MdtDriftCircleOnTrack* rot = calibrateAndSelect(intersection,**it);
        if( rot ) rots.push_back(rot);
      }
      return true;
    }

    /** calibrate and select clusters in a collection */
    template< class COL >
    bool calibrateAndSelectCluster( const MuonSystemExtension::Intersection& intersection, const COL& prds, std::vector<const MuonClusterOnTrack*>& rots ) const {
      for( typename COL::const_iterator it = prds.begin(); it != prds.end();++it ){
        const MuonClusterOnTrack* rot = calibrateAndSelect(intersection,**it);
        if( rot ) rots.push_back(rot);
      }
      return true;
    }
    
    /** IMuonPRDSelectionTool interface: calibrate and select single MDT */
    const MdtDriftCircleOnTrack* calibrateAndSelect( const MuonSystemExtension::Intersection& intersection, const MdtPrepData& mdt ) const;

    /** IMuonPRDSelectionTool interface: calibrate and select single cluster */
    const MuonClusterOnTrack* calibrateAndSelect( const Trk::TrackParameters& pars, const MuonCluster& clus ) const;
    const MuonClusterOnTrack* calibrateAndSelect( const MuonSystemExtension::Intersection& intersection, const MuonCluster& clus ) const;


  private:
    /** intersect MDT surface or tube plane */
    Amg::Vector3D intersectMDT( const MdtPrepData& mdt, const Amg::Vector3D& position, const Amg::Vector3D& direction, bool usePlane ) const;


    /** calibrate and select MDTs in a collection */
    bool calibrateAndSelectMdt( const MuonSystemExtension::Intersection& intersection, const MdtPrepDataCollection& prds, MuonLayerROTs& layerROTs ) const {
      std::vector<const MdtDriftCircleOnTrack*> rots;
      calibrateAndSelectMdt(intersection,prds,rots);
      if( !rots.empty() ) layerROTs.addMdts(rots);
      return true;
    }

    /** calibrate and select clusters in a collection */
    template< class COL >
    bool calibrateAndSelectCluster( const MuonSystemExtension::Intersection& intersection, const COL& prds, MuonLayerROTs& layerROTs ) const {
      std::vector<const MuonClusterOnTrack*> rots;
      calibrateAndSelectCluster(intersection,prds,rots);
      if( !rots.empty() ) layerROTs.addClusters(rots,m_idHelperSvc->technologyIndex(rots.front()->identify()));
      return true;
    }
    
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    ToolHandle<IMdtDriftCircleOnTrackCreator> m_mdtCreator; //<! pointer to mdt rio ontrack creator
    ToolHandle<IMuonClusterOnTrackCreator>    m_clusterCreator;  //<! pointer to muon cluster rio ontrack creator
    mutable ToolHandle<IMuonRecoValidationTool>       m_recoValidationTool; //<! FIXME!

    /** cuts */
    double m_distanceToTubeCut;
    double m_secondCoordinateCut;

  };
}



#endif
