/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKFILTER_H
#define TRACKFILTER_H

#include "GaudiKernel/Algorithm.h"

#include "Identifier/Identifier.h"
#include "GaudiKernel/ToolHandle.h"
#include "D3PDMakerInterfaces/ICollectionGetterTool.h"

#include "egammaEvent/egamma.h"
#include "muonEvent/Muon.h"

class StoreGateSvc; 

namespace D3PD{
class IObjGetterTool;

class TrackFilter: public Algorithm
{
public:    

    TrackFilter(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~TrackFilter();
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

private: 

    std::string     m_inputTracksName;     //track container
    std::string     m_outputTracksName;    //output Track container

    float           m_ptCut;               //min photon/electron pt
    float           m_etaCut;              //max photon/electron eta
    float           m_deltaRCut;           //max difference in dR between photon/electron and track

    long            m_photonIsEM;          //isEM cut to be applied on photons
    long            m_electronIsEM;        //isEM cut to be applied on electrons

    bool            m_storePerpTrk;        //store tracks close to the photon/electron perpendicular region (perp. in phi)

    bool            m_check_gsf;

    bool            m_filterTracksForMET;

    std::vector<float> etas;
    std::vector<float> phis;

    StoreGateSvc* m_storeGate; 

    ToolHandle<D3PD::ICollectionGetterTool> m_ph_getter;
    ToolHandle<D3PD::ICollectionGetterTool> m_el_getter;
    ToolHandle<D3PD::ICollectionGetterTool> m_mu_getter;
    ToolHandle<D3PD::ICollectionGetterTool> m_gsf_getter;
    ToolHandle<D3PD::IObjGetterTool>        m_gsf_assoc_getter;

    float correct_phi(float phi);
    void fillDirection(egamma* obj);
    void fillDirection(Analysis::Muon* obj);

};
}

#endif
