/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PHOTONFILTER_H
#define PHOTONFILTER_H

#include "GaudiKernel/Algorithm.h"

#include "Identifier/Identifier.h"
#include "GaudiKernel/ToolHandle.h"
#include "D3PDMakerInterfaces/ICollectionGetterTool.h"

#include "PhotonAnalysisUtils/IPhotonNeuralNetworkTool.h"

#include "egammaEvent/egamma.h"
#include "egammaEvent/Photon.h"

#include "muonEvent/Muon.h"

class StoreGateSvc; 

namespace D3PD{
class PhotonFilter: public Algorithm
{
public:    

    PhotonFilter(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~PhotonFilter();
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

private: 

    std::string     m_outputCollName;      //output container

    float           m_ptCut;               //min photon/electron pt
    float           m_etaCut;              //max photon/electron eta

    long            m_IsEM;                //isEM cut to be applied on photons
    bool            m_NNCut;               //neural net requirement

    std::vector<int> m_indices;            //only keep a few photons that pass all other requirements, by index

    StoreGateSvc* m_storeGate; 

    ToolHandle<D3PD::ICollectionGetterTool> m_ph_getter;

    bool           accept(const Analysis::Photon* ph);

    /// Property: NN tool
    ToolHandle<IPhotonNeuralNetworkTool> m_PhotonNNTool;

};

}

#endif
