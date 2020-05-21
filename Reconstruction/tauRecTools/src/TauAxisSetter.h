/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

    virtual StatusCode initialize() override;
    virtual StatusCode execute(xAOD::TauJet& pTau) const override;
    virtual StatusCode finalize() override;

private:

    Gaudi::Property<double> m_clusterCone {this, "ClusterCone", 0.2, "cone of tau candidate"};
    Gaudi::Property<bool> m_doVertexCorrection {this, "VertexCorrection", true, "switch of vertex correction"};
};

#endif
