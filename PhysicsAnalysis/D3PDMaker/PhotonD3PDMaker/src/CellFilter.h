/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CELLFILTER_H
#define CELLFILTER_H

#include "GaudiKernel/Algorithm.h"

#include "Identifier/Identifier.h"
#include "GaudiKernel/ToolHandle.h"
#include "D3PDMakerInterfaces/ICollectionGetterTool.h"

#include "egammaEvent/egamma.h"
#include "egammaEvent/egammaPIDdefs.h"
#include "muonEvent/Muon.h"
#include "CaloEvent/CaloCell.h"

class StoreGateSvc; 

namespace D3PD{
class IObjGetterTool;

class CellFilter: public Algorithm
{
public:    

    CellFilter(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~CellFilter();
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

private: 

    std::string     m_inputCellsName;     //cell container
    std::string     m_outputCellsName;    //output Cell container

    float           m_ptCut;               //min photon/electron pt
    float           m_etaCut;              //max photon/electron eta
    float           m_deltaRCut;           //max difference in dR between photon/electron and cell

    long            m_photonID;            //ID cut to be applied on photons
    long            m_electronID;          //ID cut to be applied on electrons

    bool            m_AcceptAll;

    std::vector<float> etas;
    std::vector<float> phis;

    StoreGateSvc* m_storeGate; 

    ToolHandle<D3PD::ICollectionGetterTool> m_ph_getter;
    ToolHandle<D3PD::ICollectionGetterTool> m_el_getter;

    float correct_phi(float phi);
    void fillDirection(egamma* obj);
};
}

#endif
