/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUPI0BONNCLUSTERCREATOR_H
#define	TAUREC_TAUPI0BONNCLUSTERCREATOR_H

#include <string>
#include <vector>
#include "tauRec/TauToolBase.h"
#include "GaudiKernel/ToolHandle.h"
#include "tauRec/TauToolBase.h"
#include "xAODPFlow/PFOAuxContainer.h"

using std::vector;

class IExtrapolateToCaloTool;

/**
 * @brief Creates Pi0 clusters ("Bonn" Pi0 Finder).
 * 
 * @author Veit Scharf
 * @author Will Davey <will.davey@cern.ch> 
 * @author Benedict Winter <benedict.tobias.winter@cern.ch> 
 */

class TauPi0BonnClusterCreator : public TauToolBase {
public:
    TauPi0BonnClusterCreator(const std::string& type,
            const std::string& name,
            const IInterface *parent);
    virtual ~TauPi0BonnClusterCreator();

    virtual StatusCode initialize();
    virtual StatusCode eventInitialize(TauCandidateData *data);
    virtual StatusCode execute(TauCandidateData *data);
    virtual StatusCode eventFinalize(TauCandidateData *data);
    
private:


    /** @brief fraction of cluster enegry in central EM1 cells */
    float getEM1CoreFrac( const xAOD::CaloCluster* /*pi0Candidate*/);
    
    /** @brief asymmetry of cluster energy distribution in EM1 w.r.t. the tracks  */
    float getAsymmetryInEM1WRTTrk( const xAOD::CaloCluster*  /*pi0Candidate*/, 
                                   const vector<vector<float> > /*tracksEtaAtSampling*/, 
                                   const vector<vector<float> > /*tracksPhiAtSampling*/);


    /** @brief number of cells from cluster with positive energy in PS, EM1 and EM2 */
    vector<int> getNPosECells( const xAOD::CaloCluster* /*pi0Candidate*/);

    std::map<unsigned, xAOD::CaloCluster*> getClusterToShotMap(
        const std::vector<const xAOD::PFO*> shotVector,
        const xAOD::CaloClusterContainer* pPi0ClusterContainer,
        xAOD::TauJet *pTau);

    std::vector<unsigned> getShotsMatchedToCluster(
        const std::vector<const xAOD::PFO*> shotVector,
        std::map<unsigned, xAOD::CaloCluster*> clusterToShotMap,
        xAOD::CaloCluster* pPi0Cluster);

    int getNPhotons( const std::vector<const xAOD::PFO*> /*shotVector*/, 
                     std::vector<unsigned> /*shotsInCluster*/);

    /** @brief first eta moment in PS, EM1 and EM2 w.r.t cluster eta: (eta_i - eta_cluster) */
    vector<float> get1stEtaMomWRTCluster( const xAOD::CaloCluster* /*pi0Candidate*/);

    /** @brief second eta moment in PS, EM1 and EM2 w.r.t cluster eta: (eta_i - eta_cluster)^2 */ 
    vector<float> get2ndEtaMomWRTCluster(const xAOD::CaloCluster* /*pi0Candidate*/);

    /** @brief get extrapolated track position at each layer */ 
    void getExtrapolatedPositions( const xAOD::TrackParticle* /*track*/,
                                   vector<float>&            /*trackToCaloEta*/,
                                   vector<float>&            /*trackToCaloPhi*/);



    /** @brief tool handles */
    ToolHandle<IExtrapolateToCaloTool> m_trackToCaloTool;
   
    /** @brief all calo cell container name */
    std::string m_cellContainerName;
    
    /** @brief input cluster container of pi0 candidates */
    // TODO: input cluster container name
    std::string m_inputPi0ClusterContainerName;
    
    /** @brief output cluster container of pi0 candidates */
    // TODO: output cluster container name
    std::string m_outputPi0ClusterContainerName;

    /** @brief new neutral PFO container and name */
    xAOD::PFOContainer* m_neutralPFOContainer;
    std::string m_neutralPFOContainerName;
    xAOD::PFOAuxContainer* m_neutralPFOAuxStore;
    

    /** @brief pt threshold for pi0 candidate clusters */
    double m_clusterEtCut;

    /** @brief output cluster container */
    xAOD::CaloClusterContainer*  m_pOutputPi0CaloClusterContainer;

};

#endif	/* TAUPI0BONNCLUSTERCREATOR_H */

