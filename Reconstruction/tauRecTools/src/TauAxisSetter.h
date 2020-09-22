/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUAXISSETTER_H
#define TAUREC_TAUAXISSETTER_H

#include "tauRecTools/TauRecToolBase.h"
#include "tauRecTools/ITauVertexCorrection.h"

#include "AsgTools/ToolHandle.h"

/**
 * @brief Set Tau "Detector Axis" and "Intermediate Axis". 
 * 
 *  Note that both axes starts from the barycenter of the cluster associated to the jet seed. 
 *  Then only the 4-vectors of clusters in a cone of dR around these barycenter are summed up, forming the detector axis.
 *  For the "Intermediate Axis", the clusters are corrected wrt tau vertex in this step (barycenter remains the same).
 *  Using this procedure, the axes are different from the original jet seed axis.
 * 
 * @author Margar Simonyan
 * @author Felix Friedrich
 *                                                                              
 */

class TauAxisSetter : public TauRecToolBase {

  public:
    
    ASG_TOOL_CLASS2(TauAxisSetter, TauRecToolBase, ITauToolBase);
    
    /** @brief Constructor */ 
    TauAxisSetter(const std::string& name);

    /** @brief Destructor */
    ~TauAxisSetter();

    /** @brief Initialization of this tool */ 
    virtual StatusCode initialize() override;
    
    /** @brief Execution of this tool */ 
    virtual StatusCode execute(xAOD::TauJet& pTau) const override;

  private:

    Gaudi::Property<double> m_clusterCone {this, "ClusterCone", 0.2, "cone of tau candidate"};
    Gaudi::Property<bool> m_doVertexCorrection {this, "VertexCorrection", true, "switch of tau vertex correction"};
    Gaudi::Property<bool> m_incShowerSubtr {this, "IncShowerSubtr", true, "use shower subtracted clusters in calo calculations"};

    ToolHandle<ITauVertexCorrection> m_tauVertexCorrection { this, 
      "TauVertexCorrection", "TauVertexCorrection", "Tool to perform the vertex correction"};
};

#endif
