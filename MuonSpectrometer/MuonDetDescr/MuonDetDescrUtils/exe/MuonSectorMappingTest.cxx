/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <string>

#include "MuonDetDescrUtils/MuonSectorMapping.h"
#include "TFile.h"
#include "TTree.h"

int main(int argc, char* argv[]) {
    if (argc > 1) { std::cout << argv[0] << std::endl; }
    Muon::MuonSectorMapping sectorMapping;

    TFile* output = new TFile("MuonSectorMapping.root", "RECREATE");
    TTree* tree = new TTree("sm", "sm");
    float phi;
    float width;
    float rInSector;
    float rTest;
    float phiInSector;
    float phiTest;
    float phiSector;
    float neighbourPhi;
    float rNeighbour;
    float overlapPhi;
    int id;
    int closeToEdge;
    int neighbours;
    int neighbourId;
    int inside;
    tree->Branch("phi", &phi, "phi/F");
    tree->Branch("width", &width, "width/F");
    tree->Branch("phiInSector", &phiInSector, "phiInSector/F");
    tree->Branch("phiTest", &phiTest, "phiTest/F");
    tree->Branch("rInSector", &rInSector, "rInSector/F");
    tree->Branch("rTest", &rTest, "rTest/F");
    tree->Branch("phiSector", &phiSector, "phiSector/F");
    tree->Branch("neighbourPhi", &neighbourPhi, "neighbourPhi/F");
    tree->Branch("rNeighbour", &rNeighbour, "rNeighbour/F");
    tree->Branch("overlapPhi", &overlapPhi, "overlapPhi/F");
    tree->Branch("id", &id, "id/I");
    tree->Branch("closeToEdge", &closeToEdge, "closeToEdge/I");
    tree->Branch("neighbours", &neighbours, "neighbours/I");
    tree->Branch("neighbourId", &neighbourId, "neighbourId/I");
    tree->Branch("inside", &inside, "inside/I");

    float r = 1000.;
    unsigned int nsamplings = 100000;
    float dphi = 2 * M_PI / nsamplings;
    for (unsigned int i = 0; i < nsamplings; ++i) {
        phi = -M_PI + dphi * i;
        id = sectorMapping.getSector(phi);
        width = sectorMapping.sectorWidth(id);
        phiSector = sectorMapping.sectorPhi(id);
        phiInSector = sectorMapping.transformPhiToSector(phi, id);
        phiTest = sectorMapping.transformPhiToSector(phiInSector, id, false);
        rInSector = sectorMapping.transformRToSector(r, phi, id);
        rTest = sectorMapping.transformRToSector(rInSector, phi, id, false);
        std::vector<int> sectors;
        sectorMapping.getSectors(phi, sectors);
        neighbours = sectors.size();
        neighbourId = -1;
        for (auto val : sectors) {
            if (val != id) neighbourId = val;
        }
        neighbourPhi = -5;
        overlapPhi = -5.;
        rNeighbour = 1100;
        if (neighbourId > 0) {
            neighbourPhi = sectorMapping.sectorPhi(neighbourId);
            overlapPhi = sectorMapping.sectorOverlapPhi(id, neighbourId);
            rNeighbour = sectorMapping.transformRToNeighboringSector(rInSector, id, neighbourId);
        }
        closeToEdge = sectorMapping.closeToSectorBoundary(phi);
        inside = sectorMapping.insideSector(id, phi);
        tree->Fill();
    }
    tree->Write();
    output->Write();
    output->Close();
    return 0;
}
