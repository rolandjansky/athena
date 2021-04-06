#include "Prophecy4fMerger.h"
#include <iostream>


int main(int argc, char*argv[]){

    if(argc<5){
        std::cout<<" Prophecy4fMerger: merge Powheg and Prophecy LHE files! "<<std::endl;
        std::cout<<" Usage:  ./Prophecy4fMerger [Options]"<<std::endl;
        std::cout<<"         Nedeed options:"<<std::endl;
        std::cout<<"               --inPowheg [Powheg lhe file]"<<std::endl;
        std::cout<<"               --inProphecy4e    [Prophecy4f lhe file with 4e decays]"<<std::endl;
        std::cout<<"               --inProphecy4mu   [Prophecy4f lhe file with 4mu decays]"<<std::endl;
        std::cout<<"               --inProphecy2e2mu [Prophecy4f lhe file with 2e2mu decays]"<<std::endl;
        std::cout<<"               --outLHE [Outupt lhe file]"<<std::endl;
        std::cout<<"               --randomSeed [the seed]"<<std::endl;
        std::cout<<"         Additional options:"<<std::endl;
        std::cout<<"               --debug [Enable debug mode]"<<std::endl;
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
