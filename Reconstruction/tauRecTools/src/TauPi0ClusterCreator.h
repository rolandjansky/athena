/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUPI0CLUSTERCREATOR_H
#define	TAUREC_TAUPI0CLUSTERCREATOR_H

#include <string>
#include <vector>
#include "tauRecTools/TauRecToolBase.h"
#include "xAODPFlow/PFOAuxContainer.h"


/**
 * @brief Creates Pi0 clusters (Pi0 Finder).
 * 
 * @author Will Davey <will.davey@cern.ch> 
 * @author Benedict Winter <benedict.tobias.winter@cern.ch> 
 * @author Stephanie Yuen <stephanie.yuen@cern.ch>
 */

class TauPi0ClusterCreator : public TauRecToolBase {
public:
    TauPi0ClusterCreator(const std::string& name) ;
    ASG_TOOL_CLASS2(TauPi0ClusterCreator, TauRecToolBase, ITauToolBase);
    virtual ~TauPi0ClusterCreator();

    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode eventInitialize();
    virtual StatusCode execute(xAOD::TauJet& pTau);
    virtual StatusCode eventFinalize();

    virtual void print() const { }

    
private:


    /** @brief fraction of cluster enegry in central EM1 cells */
    float getEM1CoreFrac( const xAOD::CaloCluster* /*pi0Candidate*/);
    
    /** @brief number of cells from cluster with positive energy in PS, EM1 and EM2 */
    std::vector<int> getNPosECells( const xAOD::CaloCluster* /*pi0Candidate*/);

    std::map<unsigned, xAOD::CaloCluster*> getClusterToShotMap(
        const std::vector<const xAOD::PFO*> shotVector,
        const xAOD::CaloClusterContainer& pPi0ClusterContainer,
        const xAOD::TauJet &pTau);

    std::vector<unsigned> getShotsMatchedToCluster(
        const std::vector<const xAOD::PFO*> shotVector,
        std::map<unsigned, xAOD::CaloCluster*> clusterToShotMap,
        xAOD::CaloCluster* pPi0Cluster);

    int getNPhotons( const std::vector<const xAOD::PFO*> /*shotVector*/, 
                     std::vector<unsigned> /*shotsInCluster*/);

    /** @brief first eta moment in PS, EM1 and EM2 w.r.t cluster eta: (eta_i - eta_cluster) */
    std::vector<float> get1stEtaMomWRTCluster( const xAOD::CaloCluster* /*pi0Candidate*/);

    /** @brief second eta moment in PS, EM1 and EM2 w.r.t cluster eta: (eta_i - eta_cluster)^2 */ 
    std::vector<float> get2ndEtaMomWRTCluster(const xAOD::CaloCluster* /*pi0Candidate*/);

    /** @brief get hadronic cluster PFOs*/
    bool setHadronicClusterPFOs(xAOD::TauJet& pTau);

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

    /** @brief in AODMode (probaby never true) */
    bool m_AODmode;

    /** @brief output cluster container */
    xAOD::CaloClusterContainer*  m_pOutputPi0CaloClusterContainer;

};

#endif	/* TAUPI0CLUSTERCREATOR_H */

