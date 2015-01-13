/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUPI0CLUSTERCREATOR_H
#define	TAUREC_TAUPI0CLUSTERCREATOR_H

#include <string>
#include <vector>
#include "tauRec/TauToolBase.h"
#include "xAODPFlow/PFOAuxContainer.h"

using std::vector;

/**
 * @brief Creates Pi0 clusters (Pi0 Finder).
 * 
 * @author Will Davey <will.davey@cern.ch> 
 * @author Benedict Winter <benedict.tobias.winter@cern.ch> 
 * @author Stephanie Yuen <stephanie.yuen@cern.ch>
 */

class TauPi0ClusterCreator : public TauToolBase {
public:
    TauPi0ClusterCreator(const std::string& type,
            const std::string& name,
            const IInterface *parent);
    virtual ~TauPi0ClusterCreator();

    virtual StatusCode initialize();
    virtual StatusCode eventInitialize(TauCandidateData *data);
    virtual StatusCode execute(TauCandidateData *data);
    virtual StatusCode eventFinalize(TauCandidateData *data);
    
private:


    /** @brief fraction of cluster enegry in central EM1 cells */
    float getEM1CoreFrac( const xAOD::CaloCluster* /*pi0Candidate*/);
    
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

    /** @brief get hadronic cluster PFOs*/
    bool setHadronicClusterPFOs(xAOD::TauJet* pTau);

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

    /** @brief new hadronic cluster PFO container and name */
    xAOD::PFOContainer* m_hadronicClusterPFOContainer;
    std::string m_hadronicClusterPFOContainerName;
    xAOD::PFOAuxContainer* m_hadronicClusterPFOAuxStore;
    

    /** @brief pt threshold for pi0 candidate clusters */
    double m_clusterEtCut;

    /** @brief output cluster container */
    xAOD::CaloClusterContainer*  m_pOutputPi0CaloClusterContainer;

};

#endif	/* TAUPI0CLUSTERCREATOR_H */

