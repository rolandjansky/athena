/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUAXISSETTER_H
#define TAUREC_TAUAXISSETTER_H

#include "tauRec/TauToolBase.h"


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

class TauAxisSetter : public TauToolBase {
public:

    TauAxisSetter(const std::string& type,
            const std::string& name,
            const IInterface* parent);
    ~TauAxisSetter();

    virtual StatusCode initialize();
    virtual StatusCode eventInitialize(TauCandidateData * data);
    virtual StatusCode finalize();
    virtual StatusCode execute(TauCandidateData *data);

private:
    std::string tauContainerKey;
    
    double m_clusterCone;
    /** 
     * enable cell origin correction 
     * eta and phi of the cells are corrected wrt to the origin of the tau vertex
     */
    bool m_doCellCorrection;
};

#endif
