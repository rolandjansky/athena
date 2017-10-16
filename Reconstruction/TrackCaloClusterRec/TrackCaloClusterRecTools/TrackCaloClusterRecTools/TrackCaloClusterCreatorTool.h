/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
TrackCaloClusterCreatorTool.h  -  Description
-------------------
begin   : March 2017
authors : Roland Jansky
***************************************************************************/
#ifndef TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECTOOLS_TRACKCALOCLUSTERCREATORTOOL_H
#define TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECTOOLS_TRACKCALOCLUSTERCREATORTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h" //included under assumption you'll want to use some tools! Remove if you don't!
#include "xAODPFlow/TrackCaloClusterContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODAssociations/TrackParticleClusterAssociationContainer.h"

#include "TrackVertexAssociationTool/ITrackVertexAssociationTool.h"

#include "TrackCaloClusterRecInterfaces/IClusterFilterTool.h"

class TrackCaloClusterCreatorTool : public AthAlgTool {
  public:
    TrackCaloClusterCreatorTool(const std::string&,const std::string&,const IInterface*);
    virtual ~TrackCaloClusterCreatorTool();

    virtual StatusCode initialize();
    virtual StatusCode finalize();
    
    /// Definition of the 4-momentum type.
    typedef xAOD::IParticle::FourMom_t FourMom_t;

    /** Method to calculate weights for TCC
     */
    void createCombinedTCCs(xAOD::TrackCaloClusterContainer* tccContainer, 
			    const xAOD::TrackParticleClusterAssociationContainer* assocContainer, 
			    std::map <const xAOD::TrackParticle*, FourMom_t>* TrackTotalClusterPt, 
			    std::map <const xAOD::CaloCluster*, FourMom_t>* clusterToTracksWeightMap    );
    
    void createNeutralTCCs(xAOD::TrackCaloClusterContainer* tccContainer,
			   const xAOD::CaloClusterContainer* assocContainer,
			   std::map <const xAOD::CaloCluster*, FourMom_t>* clusterToTracksWeightMap    );
    
    void createChargedTCCs(xAOD::TrackCaloClusterContainer* tccContainer,
			   const xAOD::TrackParticleContainer* assocContainer, 
			   std::map <const xAOD::TrackParticle*, FourMom_t>* TrackTotalClusterPt   );
   
  private:  
    
    void computeVertexCorr(double& eta, double& phi, const Amg::Vector3D& vertex, double radius);
    
//     double DetectorEta(const xAOD::CaloCluster& cluster, const Amg::Vector3D& vertex);
    
    // ToolHandle<IJetFromPseudojet> m_bld;  // Tool to build jets.
    ToolHandle<CP::ITrackVertexAssociationTool> m_loosetrackvertexassoTool;
    
    /// Vertex container's name
    std::string m_vertexContname;
    
    /// Particle to CaloEntry container name
    std::string m_caloEntryMapName;
    
    /// use cluster energy or pt?
    bool m_useEnergy;
    
    // enable origin correction
    bool m_doOriginCorrection;
    bool m_storeCorrectedPosition;
    
    // enable cluster filtering on the neutralOnly collection
    bool m_applyFilter;
    ToolHandle<IClusterFilterTool> m_clusterFilterTool;
    
    // this is saving the detector eta to be used in calibration
    // --> For charged and combined TCC this is using eta of the extrapolated position at the calo entry
    //     in the ATLAS global frame, i.e. origin (0., 0., 0.)
    // --> For neutral TCC this is evaluated using the cluster moment CaloCluster::CENTER_MAG
    //     inverting what is done for CaloVertexedTopoCluster objects
    bool m_saveDetectorEta;
    
};


#endif // TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECTOOLS_TRACKCALOCLUSTERCREATORTOOL_H
