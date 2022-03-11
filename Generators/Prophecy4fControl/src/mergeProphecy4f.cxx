/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include "Prophecy4fMerger.h"
#include <iostream>


int main(int argc, char*argv[]){

    if(argc<5){
        ATH_MSG_INFO(" Prophecy4fMerger: merge Powheg and Prophecy LHE files! ");
        ATH_MSG_INFO(" Usage:  ./Prophecy4fMerger [Options]");
        ATH_MSG_INFO("         Nedeed options:");
        ATH_MSG_INFO("               --inPowheg [Powheg lhe file]");
        ATH_MSG_INFO("               --inProphecy4e    [Prophecy4f lhe file with 4e decays]");
        ATH_MSG_INFO("               --inProphecy4mu   [Prophecy4f lhe file with 4mu decays]");
        ATH_MSG_INFO("               --inProphecy2e2mu [Prophecy4f lhe file with 2e2mu decays]");
        ATH_MSG_INFO("               --outLHE [Outupt lhe file]");
        ATH_MSG_INFO("               --randomSeed [the seed]");
        ATH_MSG_INFO("         Additional options:");
        ATH_MSG_INFO("               --debug [Enable debug mode]");
        exit(0);
    }
    std::string powheg, prophecy4e, prophecy4mu, prophecy2e2mu, out;
    unsigned long long seed = 0;
    bool debug=false;
    for(int a=1; a<argc; a++){
        if(!strcmp(argv[a],"--inPowheg")){
            powheg = argv[a+1];
        }
        else if(!strcmp(argv[a],"--inProphecy4e")){
            prophecy4e = argv[a+1];
        }
        else if(!strcmp(argv[a],"--inProphecy4mu")){
            prophecy4mu = argv[a+1];
        }
        else if(!strcmp(argv[a],"--inProphecy2e2mu")){
            prophecy2e2mu = argv[a+1];
        }
        else if(!strcmp(argv[a],"--outLHE")){
            out = argv[a+1];
        }
        else if(!strcmp(argv[a],"--randomSeed")){
            seed = std::stoull(argv[a+1]);
        }
        else if(!strcmp(argv[a],"--debug")){
            debug = true;
        }
    }

    Prophecy4fMerger merger;
    merger.setIO(powheg, prophecy4e, prophecy4mu, prophecy2e2mu, out, debug);
    merger.setRandomSeed(seed);
    merger.merge();

    return 0;
  
}
