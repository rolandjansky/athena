/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TROOT.h"
#include "TSystem.h"

#include <iostream>
#include <stdint.h>
#include <string>
#include <map>


using namespace std;

void setup() {
    // setup the relevant path and load the relevant libraries
    //gSystem->AddIncludePath(" -I/macros  ");

    gROOT->ProcessLine(".L ./NSWHitsTree.c+");
    gROOT->ProcessLine(".L ./ValidationStep.c+");
    gROOT->ProcessLine(".L ./ValidationManager.c+");
    gROOT->ProcessLine(".L ./sTgcValidation.c+");
    gROOT->ProcessLine(".L ./MMValidation.c+");
}

/*
ValidationStep*  get_validation_histo(const char* name, TFile* result) {
    std::cout << "in" << std::endl;

    uint32_t key = HASH(name);
    std::cout << key << std::endl;
    
    std::map <uint32_t,ValidationStep*>::iterator it =  g_validation_histo->find(key);
    if (it == g_validation_histo::end()) {
        std::cout << "not found" << std::endl;
        ValidationStep* step = 0;
        if ( strcmp(name,"sTgc")==0 ) step = new sTgcVal(result,name);
        std::pair < std::map<uint32_t,ValidationStep*>::iterator, bool> r = 
            g_validation_histos->insert(std::pair<uint32_t,ValidationStep*>(key,step));
        if (r.second) return (*r.first);
        else {
            std::cout << "NSWGeomAnalysis: failed to insert " << name << " step. Terminating ..." << std::endl;
        }
    }
    return (*it);
   
  //  return 0;
}
*/

void NSWGeomAnalysis (std::string filename) {

    setup();

    // accessing input file
    TFile* f = 0;
    if ( gSystem->AccessPathName(filename.c_str(),kFileExists) ) {
        std::cout << filename << " is not accessible. Terminating ..." << std::endl;
        return;
    } else {
        f = new TFile(filename.c_str());
    }

    if ( !f->IsOpen() ) {
        std::cout << "cannot open the input file " << filename << ". Terminating ..." << std::endl;
    }

    // getting the Tree
    TTree* validation_tree = (TTree*) f->Get("NSWHitsTree");


    // open the root file for storing the result histograms
    TFile* validation_results = new TFile("NSWValMonitoring.root","RECREATE");


    // get the validation step manager
    ValidationManager* step_manager = ValidationManager::instance();

    // loop over the validation ntuple
    NSWHitsTree validation_data(validation_tree);

    Long64_t nentries = validation_data.fChain->GetEntriesFast();

    Long64_t nbytes = 0, nb = 0;
    for (Long64_t jentry=0; jentry<nentries; jentry++) {
        Long64_t ientry = validation_data.LoadTree(jentry);
        if (ientry < 0) break;
        nb = validation_data.fChain->GetEntry(jentry);   nbytes += nb;

        if ( validation_data.Hits_sTGC_n != 0 ) {
           ValidationStep* sTgc = step_manager->get_step("sTgcHit");
           if (sTgc==0) {
               ValidationStep* step = STGC_STEP_FACTORY("sTgcHit",validation_results);
               sTgc = ValidationManager::instance()->insert(step);
           }
           sTgc->ProcessData(validation_data);
        }

        if ( validation_data.Hits_MM_n != 0 ) {
           ValidationStep* MM = step_manager->get_step("MMHit");
           if (MM==0) {
               ValidationStep* step = MM_STEP_FACTORY("MMHit",validation_results);
               MM = ValidationManager::instance()->insert(step);
           }
           MM->ProcessData(validation_data);
        }

    }


    // save histograms on validation_results
    //step_manager->get_step("sTgc")->WriteToFile();
    validation_results->Write();
    
}
