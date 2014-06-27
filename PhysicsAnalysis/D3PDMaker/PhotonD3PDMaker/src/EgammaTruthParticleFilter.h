/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMATRUTHPARTICLEFILTER_H
#define EGAMMATRUTHPARTICLEFILTER_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "Identifier/Identifier.h"
#include "GaudiKernel/ToolHandle.h"
#include "D3PDMakerInterfaces/ICollectionGetterTool.h"

#include "PhotonAnalysisUtils/IPAUtruthTool.h"

class StoreGateSvc; 
class TruthParticle;
class TruthParticleContainer;

namespace D3PD{
class EgammaTruthParticleFilter: public AthAlgorithm
{
public:    

    EgammaTruthParticleFilter(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~EgammaTruthParticleFilter();
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

private: 

    StatusCode AddTruthParticle(const TruthParticle* tp, TruthParticleContainer* outputCont);

    std::string     m_inputTruthName;     //truth container
    std::string     m_outputTruthName;    //output truth container

    /// Property: truth tool
    ToolHandle<IPAUtruthTool> m_PAUtruthTool;

    ToolHandle<D3PD::ICollectionGetterTool> m_ph_getter;
    ToolHandle<D3PD::ICollectionGetterTool> m_el_getter;
    ToolHandle<D3PD::ICollectionGetterTool> m_truth_getter;

    float m_ptCut;

    std::set<const TruthParticle*> m_kept_truth;

};
}

#endif
