/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUAXISSETTER_H
#define TAUREC_TAUAXISSETTER_H

#include "tauRecTools/TauRecToolBase.h"


/**
 * @brief Set Tau "Detector Axis" and "Intermediate Axis". 
 * 
 *  Note that both axes starts from the barycenter of the cluster associated to the jet seed. 
 *  Then only the 4-vectors of clusters in a cone of dR around these barycenter are summed up, forming the new axis.
 *  For the "Intermediate Axis" the clusters are correct wrt tau vertex in this step (barycenter remains the same).
 *  Using this procedure, the axes are different from the original jet seed axis.
 * 
 * @author Margar Simonyan
 * @author Felix Friedrich
 *                                                                              
 */

class TauAxisSetter : public TauRecToolBase {
public:

    TauAxisSetter(const std::string& name);
    ASG_TOOL_CLASS2(TauAxisSetter, TauRecToolBase, ITauToolBase);
    ~TauAxisSetter();

    virtual StatusCode initialize();
    virtual StatusCode eventInitialize();
    virtual StatusCode finalize();
    virtual StatusCode eventFinalize() { return StatusCode::SUCCESS; }
    virtual StatusCode execute(xAOD::TauJet& pTau);
    virtual StatusCode executeShotFinder(xAOD::TauJet&, xAOD::CaloClusterContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePi0CreateROI(xAOD::TauJet&, CaloCellContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePi0ClusterCreator(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&, xAOD::CaloClusterContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executeVertexVariables(xAOD::TauJet&, xAOD::VertexContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePi0ClusterScaler(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePanTau(xAOD::TauJet&, xAOD::ParticleContainer&) { return StatusCode::SUCCESS; }

    virtual void print() const { }



private:
    std::string m_tauContainerKey;
    
    double m_clusterCone;
    /** 
     * enable cell origin correction 
     * eta and phi of the cells are corrected wrt to the origin of the tau vertex
     */
    bool m_doCellCorrection;
    bool m_doAxisCorrection;
};

#endif
