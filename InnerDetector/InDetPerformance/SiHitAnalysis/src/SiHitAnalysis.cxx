/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iomanip>

#include "SiHitAnalysis/SiHitAnalysis.h"
#include "GaudiKernel/MsgStream.h"
#include "TTree.h"
#include "InDetSimEvent/SiHitCollection.h"
#include "CLHEP/Geometry/Point3D.h"

using namespace std;

SiHitAnalysis::SiHitAnalysis(const std::string &name, ISvcLocator *pSvcLocator):
 AthAlgorithm(name, pSvcLocator),
 m_storeGate("StoreGateSvc", name),
 m_histSvc("THistSvc", name) {
    ATH_MSG_DEBUG("Create SiHitAnalysis");
    declareProperty("doHits", m_doHits = true);
    declareProperty("PixelSimHitContainer", m_pixelSimHitContainerName = "PixelHits");
    declareProperty("StripSimHitContainer", m_StripSimHitContainerName = "SCT_Hits"); 
}

StatusCode SiHitAnalysis::initialize() {
//
//    Get a StoreGate-handle for access to the Event Store
//
    CHECK(m_storeGate.retrieve());
//
//    Get a handle on the NTuple and histogramming service
//
    CHECK(m_histSvc.retrieve());
    m_outputTree = new TTree("hitTree", "hitTree");
    CHECK(m_histSvc->regTree("/HIT_NTUPLE/hitTree", m_outputTree));
//
//    Add branches and leaves to the tree 
//

//
//    Pixels
//
    m_outputTree->Branch("NPixelHits",         &m_numberPixelHits, "NPixelHits/i");

    m_outputTree->Branch("PixelHit_BarrelEC",  &m_pixelHitBarrelEC);
    m_outputTree->Branch("PixelHit_LayerDisk", &m_pixelHitLayer);
    m_outputTree->Branch("PixelHit_EtaModule", &m_pixelHitEta);
    m_outputTree->Branch("PixelHit_PhiModule", &m_pixelHitPhi);

    m_outputTree->Branch("PixelHit_LocalX",    &m_pixelHitLocalX);
    m_outputTree->Branch("PixelHit_LocalY",    &m_pixelHitLocalY);
    m_outputTree->Branch("PixelHit_LocalZ",    &m_pixelHitLocalZ);
    m_outputTree->Branch("PixelHit_Energy",    &m_pixelHitEnergy);
//
//    Strips
//
    m_outputTree->Branch("PixelHit_Time",      &m_pixelHitTime);

    m_outputTree->Branch("NStripHits",         &m_numberStripHits, "NStripHits/i");
    m_outputTree->Branch("StripHit_BarrelEC",  &m_StripHitBarrelEC);
    m_outputTree->Branch("StripHit_LayerDisk", &m_StripHitLayer);
    m_outputTree->Branch("StripHit_EtaModule", &m_StripHitEta);
    m_outputTree->Branch("StripHit_PhiModule", &m_StripHitPhi);
    m_outputTree->Branch("StripHit_Side",      &m_StripHitSide);

    m_outputTree->Branch("StripHit_LocalX",    &m_StripHitLocalX);
    m_outputTree->Branch("StripHit_LocalY",    &m_StripHitLocalY);
    m_outputTree->Branch("StripHit_LocalZ",    &m_StripHitLocalZ);
    m_outputTree->Branch("StripHit_Energy",    &m_StripHitEnergy);
    m_outputTree->Branch("StripHit_Time",      &m_StripHitTime);

    clear();

    return StatusCode::SUCCESS;
}

StatusCode SiHitAnalysis::execute() {
//
//    Pixel hits
//
    const SiHitCollection *pixSiHitTES = 0;
    CHECK(m_storeGate->retrieve(pixSiHitTES, m_pixelSimHitContainerName));
    ATH_MSG_DEBUG("Pixel hit collection retrieved with " << pixSiHitTES->size() << " hits");
  
    for (SiHitConstIterator hit = pixSiHitTES->begin(); hit != pixSiHitTES->end(); ++hit) {
        ATH_MSG_DEBUG("ID = " <<  hex   << setw(8) << hit->identify() << dec << 
                      ", Side = "       << setw(1) << hit->getSide() << 
                      ", PhiModule = "  << setw(2) << hit->getPhiModule() <<
                      ", EtaModule = "  << setw(3) << hit->getEtaModule() <<
                      ", Layer/Disk = " << setw(1) << hit->getLayerDisk() <<
                      ", Barrel/EC = "  << setw(2) << hit->getBarrelEndcap() << 
                      ", is pixel? "    << setw(1) << hit->isPixel());
        m_numberPixelHits++;
        m_pixelHitBarrelEC.push_back(hit->getBarrelEndcap());
        m_pixelHitLayer.push_back(hit->getLayerDisk());
        m_pixelHitEta.push_back(hit->getEtaModule());
        m_pixelHitPhi.push_back(hit->getPhiModule());
        m_pixelHitEnergy.push_back(hit->energyLoss());
        m_pixelHitTime.push_back(hit->meanTime());

        HepGeom::Point3D<double> entryPoint = hit->localStartPosition();
        m_pixelHitLocalX.push_back(entryPoint.x());
        m_pixelHitLocalY.push_back(entryPoint.y());
        m_pixelHitLocalZ.push_back(entryPoint.z());
    }
    ATH_MSG_DEBUG("Total of number of pixel hits is " << m_numberPixelHits);
//
//    Strip Hits
//
    const SiHitCollection *StripSiHitTES = 0;
    CHECK(m_storeGate->retrieve(StripSiHitTES, m_StripSimHitContainerName));
    ATH_MSG_DEBUG("Strip hit collection retrieved with " << StripSiHitTES->size() << " hits");

    for(SiHitConstIterator hit = StripSiHitTES->begin(); hit != StripSiHitTES->end(); ++hit) {
        ATH_MSG_DEBUG("ID = " <<  hex   << setw(8) << hit->identify() << dec << 
                      ", Side = "       << setw(1) << hit->getSide() << 
                      ", PhiModule = "  << setw(2) << hit->getPhiModule() <<
                      ", EtaModule = "  << setw(3) << hit->getEtaModule() <<
                      ", Layer/Disk = " << setw(1) << hit->getLayerDisk() <<
                      ", Barrel/EC = "  << setw(2) << hit->getBarrelEndcap() << 
                      ", is pixel? "    << setw(1) << hit->isPixel());
        m_numberStripHits++;
        m_StripHitBarrelEC.push_back(hit->getBarrelEndcap());
        m_StripHitLayer.push_back(hit->getLayerDisk());
        m_StripHitPhi.push_back(hit->getPhiModule());
        m_StripHitEta.push_back(hit->getEtaModule());
        m_StripHitSide.push_back(hit->getSide());

        m_StripHitEnergy.push_back(hit->energyLoss());
        m_StripHitTime.push_back(hit->meanTime());

        HepGeom::Point3D<double> entryPoint = hit->localStartPosition();
        m_StripHitLocalX.push_back(entryPoint.x());
        m_StripHitLocalY.push_back(entryPoint.y());
        m_StripHitLocalZ.push_back(entryPoint.z());
    }
    ATH_MSG_DEBUG("Total of number of Strip hits is " << m_numberStripHits);

    m_outputTree->Fill();

    clear();
 
    return StatusCode::SUCCESS;
}

StatusCode SiHitAnalysis::finalize() {
    return StatusCode::SUCCESS;
}

void SiHitAnalysis::clear() {
// Prepare for next event
    m_numberPixelHits = 0;
    m_pixelHitBarrelEC.clear();
    m_pixelHitLayer.clear();
    m_pixelHitEta.clear();
    m_pixelHitPhi.clear();

    m_pixelHitLocalX.clear();
    m_pixelHitLocalY.clear();
    m_pixelHitLocalZ.clear();
    m_pixelHitEnergy.clear();
    m_pixelHitTime.clear();

    m_numberStripHits = 0;
    m_StripHitBarrelEC.clear();
    m_StripHitLayer.clear();
    m_StripHitEta.clear();
    m_StripHitPhi.clear();
    m_StripHitSide.clear();

    m_StripHitLocalX.clear();
    m_StripHitLocalY.clear();
    m_StripHitLocalZ.clear();
    m_StripHitEnergy.clear();
    m_StripHitTime.clear();

}