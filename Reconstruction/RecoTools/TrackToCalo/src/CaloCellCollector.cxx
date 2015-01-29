/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackToCalo/CaloCellCollector.h"

#include "TrkCaloExtension/CaloExtensionHelpers.h"

#include "CaloUtils/CaloClusterStoreHelper.h"
#include "CaloEvent/CaloCellContainer.h"

#include "CaloUtils/CaloCellList.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloIdManager.h"

#include <tuple>

Rec::CaloCellCollector::CaloCellCollector ()
    :
    // m_doDebug(true)
    m_doDebug(false)
{

    // From Bruno:
    // PS: 0.025x0.06 (rectangle)
    // EM1: 0.03x0.07 (rectangle)
    // EM2: 0.05 (cone)
    // EM3: 0.03 (cone)
    // Tile1: 0.08x0.06 (rectangle)
    // Tile2: 2x1 (number of cells in eta X phi)
    // Tile3: 0.15x0.06 (rectangle)
    // HEC0-3: 0.07 (cone)

    
    // For EM, set explicit window per layer, for rest use input dR
    m_dEtadPhi.resize(CaloSampling::Unknown);
    m_dEtadPhi[CaloSampling::PreSamplerB] = std::pair<float, float>(0.0375,  0.075);  // Ncells eta/phi: 3/1.5 
    m_dEtadPhi[CaloSampling::PreSamplerE] = std::pair<float, float>(0.0375,  0.075);  // Ncells eta/phi: 3/1.5 
    m_dEtadPhi[CaloSampling::EMB1]        = std::pair<float, float>(0.011,   0.1);    // Ncells eta/phi: 7/2
    m_dEtadPhi[CaloSampling::EME1]        = std::pair<float, float>(0.011,   0.1);    // Ncells eta/phi: 7/2
    m_dEtadPhi[CaloSampling::EMB2]        = std::pair<float, float>(0.0625,  0.0625); // Ncells eta/phi: 5/5
    m_dEtadPhi[CaloSampling::EME2]        = std::pair<float, float>(0.0625,  0.0625); // Ncells eta/phi: 5/5
    m_dEtadPhi[CaloSampling::EMB3]        = std::pair<float, float>(0.05,    0.0375); // Ncells eta/phi: 2/1.5
    m_dEtadPhi[CaloSampling::EME3]        = std::pair<float, float>(0.05,    0.0375); // Ncells eta/phi: 2/1.5
    m_dEtadPhi[CaloSampling::TileBar0]    = std::pair<float, float>(0.154,   0.151);  // Ncells eta/phi: 3/3
    m_dEtadPhi[CaloSampling::TileBar1]    = std::pair<float, float>(0.154,   0.151);  // Ncells eta/phi: 3/3
    m_dEtadPhi[CaloSampling::TileBar2]    = std::pair<float, float>(0.308,   0.151);  // Ncells eta/phi: 3/3
    m_dEtadPhi[CaloSampling::TileExt0]    = std::pair<float, float>(0.154,   0.151);  // Ncells eta/phi: 3/3
    m_dEtadPhi[CaloSampling::TileExt1]    = std::pair<float, float>(0.154,   0.151);  // Ncells eta/phi: 3/3
    m_dEtadPhi[CaloSampling::TileExt2]    = std::pair<float, float>(0.308,   0.151);  // Ncells eta/phi: 3/3
    m_dEtadPhi[CaloSampling::TileGap1]    = std::pair<float, float>(0.154,   0.151);  // Ncells eta/phi: 3/3
    m_dEtadPhi[CaloSampling::TileGap2]    = std::pair<float, float>(0.308,   0.151);  // Ncells eta/phi: 3/3
    m_dEtadPhi[CaloSampling::TileGap3]    = std::pair<float, float>(0.308,   0.151);  // Ncells eta/phi: 3/3
    // EM explicit window samples
    m_samples.reserve(8);
    m_samples.push_back(CaloSampling::PreSamplerB);
    m_samples.push_back(CaloSampling::PreSamplerE);
    m_samples.push_back(CaloSampling::EMB1);
    m_samples.push_back(CaloSampling::EME1);
    m_samples.push_back(CaloSampling::EMB2);
    m_samples.push_back(CaloSampling::EME2);
    m_samples.push_back(CaloSampling::EMB3);
    m_samples.push_back(CaloSampling::EME3);
    m_samples.push_back(CaloSampling::TileBar0);
    m_samples.push_back(CaloSampling::TileBar1);
    m_samples.push_back(CaloSampling::TileBar2);
    m_samples.push_back(CaloSampling::TileExt0);
    m_samples.push_back(CaloSampling::TileExt1);
    m_samples.push_back(CaloSampling::TileExt2);
    m_samples.push_back(CaloSampling::TileGap1);
    m_samples.push_back(CaloSampling::TileGap2);
    m_samples.push_back(CaloSampling::TileGap3);
    // samples with dR
    m_samplesForDR.reserve(4);
    m_samplesForDR.push_back(CaloSampling::HEC0);
    m_samplesForDR.push_back(CaloSampling::HEC1);
    m_samplesForDR.push_back(CaloSampling::HEC2);
    m_samplesForDR.push_back(CaloSampling::HEC3);

    if (m_doDebug) std::cout << "Set explicit window sizes " << std::endl;

}

xAOD::CaloCluster* 
Rec::CaloCellCollector::collectCells( const Trk::CaloExtension& extension, 
                                      const CaloCellContainer& cellContainer, 
                                      xAOD::CaloClusterContainer& clusterContainer ) const {


    // Collect cells in explicit window sizes

    std::vector<const CaloCell*> cells;
    
    CaloExtensionHelpers::EntryExitLayerMap  entryExitLayerMap;
    CaloExtensionHelpers::entryExitLayerMap( extension, entryExitLayerMap );

    if (m_doDebug) std::cout << "EntryExitLayerMap " << entryExitLayerMap.size() << std::endl;

    cells.reserve(100);
    CaloCellList myList(&cellContainer);
    // Save sampling midpoints in map
    typedef std::tuple <float, float, float>               SampData;
    typedef std::pair<CaloSampling::CaloSample, SampData > SampDataPair;
    std::map<CaloSampling::CaloSample, SampData > sampleEtaPhiMap;
    float etot = 0;
    unsigned int samplingPattern = 0;
    
    // loop over samples using explicit sample window sizes

    for ( auto samp : m_samples ) {
        auto pos = entryExitLayerMap.find(samp);
        if( pos != entryExitLayerMap.end() ) {
            samplingPattern |= CaloSampling::getSamplingPattern(samp); // add in sampling to pattern for cluster
            auto midPoint = 0.5 * (pos->second.first + pos->second.second);
            myList.select(midPoint.eta(), midPoint.phi(), m_dEtadPhi[samp].first,  m_dEtadPhi[samp].second,  samp);
            cells.insert(cells.end(), myList.begin(), myList.end());
            float e = 0;
            for ( auto cell : myList ) e += cell->energy();
            etot += e;
            sampleEtaPhiMap.insert( SampDataPair(samp, std::make_tuple (e, midPoint.eta(), midPoint.phi())));
            if (m_doDebug) std::cout <<  CaloSampling::getSamplingName(samp) << " e,eta,phi,cells  " << e << "/"
                                     << midPoint.eta() << "/" << midPoint.phi() << "/" << myList.ncells() << " "
                                     << samp << std::endl;
        }
    }

    // loop over samples using fabs(dR) for window
    
    for ( auto samp : m_samplesForDR ) {
        auto pos = entryExitLayerMap.find(samp);
        if( pos != entryExitLayerMap.end() ) {
            samplingPattern |= CaloSampling::getSamplingPattern(samp); // add in sampling to pattern for cluster
            auto midPoint = 0.5 * (pos->second.first + pos->second.second);
            myList.select(midPoint.eta(), midPoint.phi(), 0.1,  samp);
            cells.insert(cells.end(), myList.begin(), myList.end());
            float e = 0;
            for ( auto cell : myList ) e += cell->energy();
            etot += e;
            sampleEtaPhiMap.insert( SampDataPair(samp, std::make_tuple (e, midPoint.eta(), midPoint.phi())));
            if (m_doDebug) std::cout << CaloSampling::getSamplingName(samp) << " e,eta,phi,cells  " << e << "/"
                                     << midPoint.eta() << "/" << midPoint.phi() << "/" << myList.ncells() << " "
                                     << samp << std::endl;
        }
    }

    for ( auto cell : cells ) {
        if (m_doDebug) std::cout << "Input cell "
                      <<  CaloDetDescrManager::instance()->getCalo_Mgr()->getEM_ID()->show_to_string(cell->ID())
                      << " " << cell->eta() << "/" << cell->phi() << std::endl;
    }

    if (m_doDebug) std::cout << "associated cells " << cells.size() << std::endl;

    
    // create cluster
    xAOD::CaloCluster* cluster = CaloClusterStoreHelper::makeCluster(&clusterContainer,&cellContainer);
    if( !cluster ){
        return nullptr;
    }

    // allocate the space for the sampling information
    cluster->setSamplingPattern(samplingPattern); 
    
    // add  cells to cluster
    cluster->setE(etot);
    for ( auto cell : cells ) {
        if( !cell || !cell->caloDDE() ) continue;
        int index = cellContainer.findIndex(cell->caloDDE()->calo_hash());
        if( index == -1 ) continue;
        cluster->addCell(index,1.);
    }
    // Add energy and midpoints as the cluster eta/phi sampling positions
    for ( auto entry : sampleEtaPhiMap ) {
        cluster->setEnergy(entry.first, std::get<0>(entry.second));
        cluster->setEta   (entry.first, std::get<1>(entry.second));
        cluster->setPhi   (entry.first, std::get<2>(entry.second));
        if (m_doDebug) { std::cout << "cluster sample/e/eta/phi " << CaloSampling::getSamplingName(entry.first)
                                   << " " << cluster->eSample(entry.first) << "/"
                                   << cluster->etaSample(entry.first) << "/"
                                   << cluster->phiSample(entry.first) << std::endl;
        }
    }
    
    return cluster;
}
              
