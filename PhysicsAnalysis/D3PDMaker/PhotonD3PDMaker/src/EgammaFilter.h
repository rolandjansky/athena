/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAFILTER_H
#define EGAMMAFILTER_H

#include "GaudiKernel/Algorithm.h"

#include "Identifier/Identifier.h"
#include "GaudiKernel/ToolHandle.h"
#include "D3PDMakerInterfaces/ICollectionGetterTool.h"

#include "PhotonAnalysisUtils/IPhotonNeuralNetworkTool.h"

#include "egammaEvent/egamma.h"
#include "egammaEvent/Photon.h"
#include "egammaEvent/Electron.h"

#include "muonEvent/Muon.h"

class StoreGateSvc; 

namespace D3PD{
class EgammaFilter: public Algorithm
{
public:    

    EgammaFilter(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~EgammaFilter();
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

private: 

    std::string     m_outputPhotonCollName;      //output container

    float           m_photon_ptCut;               //min photon/electron pt
    float           m_photon_etaCut;              //max photon/electron eta
    long            m_photon_IsEM;                //isEM cut to be applied on photons
    bool            m_photon_NNCut;               //neural net requirement

    std::string     m_outputElectronCollName;      //output container

    float           m_electron_ptCut;               //min electron/electron pt
    float           m_electron_etaCut;              //max electron/electron eta
    long            m_electron_IsEM;                //isEM cut to be applied on electrons 

    StoreGateSvc* m_storeGate; 

    ToolHandle<D3PD::ICollectionGetterTool> m_ph_getter;
    ToolHandle<D3PD::ICollectionGetterTool> m_el_getter;

    bool           acceptPhoton  (const Analysis::Photon* ph);
    bool           acceptElectron(const Analysis::Electron* el);

    bool           IsOverlap(const egamma* ph, const egamma* el);

    /// Property: NN tool
    ToolHandle<IPhotonNeuralNetworkTool> m_PhotonNNTool;

};

}

#endif
