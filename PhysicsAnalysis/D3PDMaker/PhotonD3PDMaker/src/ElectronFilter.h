/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ELECTRONFILTER_H
#define ELECTRONFILTER_H

#include "GaudiKernel/Algorithm.h"

#include "Identifier/Identifier.h"
#include "GaudiKernel/ToolHandle.h"
#include "D3PDMakerInterfaces/ICollectionGetterTool.h"

#include "egammaEvent/egamma.h"
#include "egammaEvent/Electron.h"

#include "muonEvent/Muon.h"

class StoreGateSvc; 

namespace D3PD{
class ElectronFilter: public Algorithm
{
public:    

    ElectronFilter(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~ElectronFilter();
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

private: 

    std::string     m_outputCollName;      //output container

    float           m_ptCut;               //min photon/electron pt
    float           m_etaCut;              //max photon/electron eta

    long            m_IsEM;                //isEM cut to be applied on photons

    std::vector<int> m_indices;            //only keep a few photons that pass all other requirements, by index

    StoreGateSvc* m_storeGate; 

    bool            m_RejectAll;
    bool            m_AcceptAll;

    ToolHandle<D3PD::ICollectionGetterTool> m_el_getter;

    bool           accept(const Analysis::Electron* el);

};

}

#endif
