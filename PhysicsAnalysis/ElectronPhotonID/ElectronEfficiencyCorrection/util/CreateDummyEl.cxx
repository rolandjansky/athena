/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include "CreateDummyEl.h"
#include "Messaging.h"
// EDM include(s):
#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/ElectronContainer.h" 
#include "xAODEgamma/Egamma.h"
#include <xAODCaloEvent/CaloCluster.h>
#include <xAODCaloEvent/CaloClusterContainer.h>
#include <xAODCaloEvent/CaloClusterAuxContainer.h>
#include <xAODEgamma/ElectronContainer.h>
#include <xAODEgamma/ElectronAuxContainer.h>

namespace{

    void fill_cluster( xAOD::CaloCluster* cluster, float eta, float phi, float e){

        const std::set<CaloSampling::CaloSample> samplings {
            CaloSampling::PreSamplerB, CaloSampling::EMB1, CaloSampling::EMB2, CaloSampling::EMB3,
                CaloSampling::PreSamplerE, CaloSampling::EME1, CaloSampling::EME2, CaloSampling::EME3 };

        unsigned sampling_pattern = 0;
        for (auto sample : samplings) { sampling_pattern |= 0x1U << sample; }
        cluster->setSamplingPattern(sampling_pattern);

        for (auto sample : samplings) {
            cluster->setEta(sample, eta);
            cluster->setPhi(sample, phi);
        }

        if (std::abs(eta) < 1.45) {
            cluster->setEnergy(CaloSampling::PreSamplerB, e/4);
            cluster->setEnergy(CaloSampling::EMB1, e/4);
            cluster->setEnergy(CaloSampling::EMB2, e/4);
            cluster->setEnergy(CaloSampling::EMB3, e/4);
        }
        else {
            cluster->setEnergy(CaloSampling::PreSamplerE, e/4);
            cluster->setEnergy(CaloSampling::EME1, e/4);
            cluster->setEnergy(CaloSampling::EME2, e/4);
            cluster->setEnergy(CaloSampling::EME3, e/4);
        }
        cluster->setE(e);
        cluster->setEta(eta);
        cluster->setPhi(phi);
        cluster->auxdata<float>("etaCalo") = eta;
        cluster->auxdata<float>("phiCalo") = phi;
        cluster->insertMoment(xAOD::CaloCluster::ETACALOFRAME, eta);
        cluster->insertMoment(xAOD::CaloCluster::PHICALOFRAME, phi);
    }
}
/* This is a bit weird function at it "creates a dummy pseudo reconstruction sequence"
 * The idea here is basically to record everything to the Store in the order reco would do.
 * Then an analysis code should see the collections needed
 */
StatusCode getElectrons (const std::vector<std::pair<double,double>>& pt_eta, int runNumber,xAOD::TStore& store ){
    //This is what we will return back
    
   //create the EventInfo
    std::unique_ptr<xAOD::EventInfo> eventInfo=std::make_unique<xAOD::EventInfo>();
    eventInfo->makePrivateStore();
    eventInfo->setEventNumber(363636);
    eventInfo->setRunNumber(runNumber);    
    static SG::AuxElement::Decorator<unsigned int> randomrunnumber("RandomRunNumber") ;
    randomrunnumber(*eventInfo)= runNumber;
    if(!store.record(std::move(eventInfo), "EventInfo").isSuccess()){
        MSG_ERROR("Could not record EventInfo");
        return StatusCode::FAILURE;   
    }
    
    size_t numel=pt_eta.size();
    //Create all clusters
    std::unique_ptr<xAOD::CaloClusterContainer> clusters = std::make_unique<xAOD::CaloClusterContainer>();
    std::unique_ptr<xAOD::CaloClusterAuxContainer> clAux = std::make_unique<xAOD::CaloClusterAuxContainer>();
    clusters->setStore(clAux.get());
    clusters->reserve(numel); 
    for (const auto& i : pt_eta){            
        const double pt {i.first};
        const double eta {i.second};
        const double e {pt * cosh(eta)};
        xAOD::CaloCluster* cluster = new xAOD::CaloCluster(); 
        clusters->push_back(cluster);
        fill_cluster(cluster,eta, 0.0, e); 
    }
    
   if(!store.record( std::move(clusters), "MyClusters" ).isSuccess() ||
            !store.record( std::move(clAux), "MyClustersAux.").isSuccess()){
        MSG_ERROR("Could not record clusters");
        return StatusCode::FAILURE;   
    }
   //Create all electrons  one per cluster
    std::vector< ElementLink< xAOD::CaloClusterContainer > > links{};
    std::unique_ptr<xAOD::ElectronContainer> electrons =  std::make_unique<xAOD::ElectronContainer>();
    std::unique_ptr<xAOD::ElectronAuxContainer> electronsAux = std::make_unique <xAOD::ElectronAuxContainer>();
    electrons->setStore(electronsAux.get());    
    electrons->reserve(numel);
    for (const auto& i : pt_eta){
        const double pt {i.first};
        const double eta {i.second};
        xAOD::Electron* el = new xAOD::Electron;
        electrons->push_back(el);
        links.clear();
        links.push_back(ElementLink< xAOD::CaloClusterContainer >("MyClusters",el->index()));  
        el->setCaloClusterLinks(links);
        el->setEta(eta);
        el->setPhi(0.0);
        el->setM(0.511);
        el->setPt(pt);
    }
    if(!store.record( std::move(electrons), "MyElectrons" ).isSuccess() || 
            !store.record( std::move(electronsAux), "MyElectronAux.").isSuccess()){
        MSG_ERROR("Could not record Electrons");
        return StatusCode::FAILURE;   
    }   

    return StatusCode::SUCCESS;
}
