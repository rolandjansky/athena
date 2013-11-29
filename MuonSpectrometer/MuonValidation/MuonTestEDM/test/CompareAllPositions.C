/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ComparePrdPositions.C"
#include <string>
#include <iostream>

int main( int argc,char *argv[] ){

    std::string type="RPC"; // RPC by default
    std::string file="ReadOut150401.root"; // RPC by default
    std::string ref="/afs/cern.ch/user/e/emoyse/public/ReferenceESD/WriteESDfromRDO.15.4.1.root"; // RPC by default
    bool checkLocal=true;
    bool checkGlobal= true;
    
    int optind=1;
    // decode arguments
    while ((optind < argc) && (argv[optind][0]=='-')) {
        std::string sw = argv[optind];
        if (sw=="--prd") {
            optind++;
            type = argv[optind];
        }
        else if (sw=="--file") {
            optind++;
            type = argv[optind];
        }
        else if (sw=="--ref") {
            optind++;
            type = argv[optind];
        } else if (sw=="-g") {
            checkGlobal=false;
        } else if (sw=="-G") {
            checkGlobal=true;
        } else if (sw=="-l") {
            checkLocal=false;
        } else if (sw=="-L") {
            checkLocal=true;
        }
        else
            std::cout << "Unknown switch: " 
                 << argv[optind] << std::endl;
        optind++;
    }
    std::cout<<"Running compare PRDs with type="<<type<<", file="<<file<<", and ref="<<ref<<std::endl;
    std::cout<<"checkLocal="<<checkLocal<<", checkGlobal="<<checkGlobal<<std::endl;

    int success=0;
    ComparePrdPositions rpc(type.c_str(),ref.c_str(),file.c_str());
    if (rpc.Loop(checkLocal, checkGlobal) == false ){
        std::cerr<<type<<" test found problems"<<std::endl;
        success = 1;
    }
    
    // ComparePrdPositions mdt("MDT","ReadOut150401.root","/afs/cern.ch/user/e/emoyse/public/ReferenceESD/WriteESDfromRDO.15.4.1.root");
    // if (rpc.Loop() == false ){
    //     std::cerr<<"MDT test found problems"<<std::endl;
    //     success = 1;
    // }
    // 
    // ComparePrdPositions tgc("TGC","ReadOut150401.root","/afs/cern.ch/user/e/emoyse/public/ReferenceESD/WriteESDfromRDO.15.4.1.root");
    // if (tgc.Loop() == false ){
    //     std::cerr<<"TGC test found problems"<<std::endl;
    //     success = 1;
    // }
    // 
    // ComparePrdPositions csc("CSC","ReadOut150401.root","/afs/cern.ch/user/e/emoyse/public/ReferenceESD/WriteESDfromRDO.15.4.1.root");
    // if (csc.Loop() == false ){
    //     std::cerr<<"CSC test found problems"<<std::endl;
    //     success = 1;
    // }
    // ComparePrdPositions rpc2("RPC","/afs/cern.ch/user/e/emoyse/public/ReferenceESD/RDOtoESD_15.5.1_wOldReadoutGeom.root","ReadOut150501.root");   
    // if (rpc2.Loop() == false ){
    //     std::cerr<<"RPC test found problems"<<std::endl;
    //     success = 1;
    // }
    

    return success;
}
