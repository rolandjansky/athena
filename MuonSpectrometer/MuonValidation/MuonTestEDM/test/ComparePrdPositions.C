/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define ComparePrdPositions_cxx
#include "ComparePrdPositions.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <cmath>

bool ComparePrdPositions::Loop( bool checkLocal, bool checkGlobal)
{
   if (fChain == 0) return false;

   Long64_t nentries = fChain->GetEntriesFast();
   Long64_t nentries2 = fChain2->GetEntriesFast();

   if (nentries!=nentries2) std::cout<<"WARNING nentries differ! "<<nentries<<" / "<<nentries2<<std::endl;
   
   unsigned int numLocalErrs=0;
   unsigned int numGlobalErrs=0;
   
   bool allOkay=true;
   Long64_t nbytes = 0, nb = 0;
   Long64_t jentry=0;
   for (; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree1(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
       ientry = LoadTree2(jentry);
      if (ientry < 0) break;
      nb = fChain2->GetEntry(jentry);   nbytes += nb;
      double dist = sqrt( pow(RPC_LocalX - RPC_LocalX,2) + pow(RPC_LocalX - RPC_LocalX2,2) );
      
      if ( checkLocal && ( dist>0.001) ) {
          std::cout<<"Local Match fails for #"<<jentry<<" x diff: "<<fabs(RPC_LocalX - RPC_LocalX2)
                   <<", y diff: "<<fabs(RPC_LocalY - RPC_LocalY2)
                   <<" in station:\t"<<RPC_StationName2<<std::endl;
          // std::cout<<"1: "<< (fabs(RPC_LocalX - RPC_LocalX2)>0.0000001)<<std::endl;
          std::cout<<"X1 : "<<RPC_LocalX<<", \t\tX2 : "<<RPC_LocalX2<<std::endl;
          std::cout<<"Y1 : "<<RPC_LocalY<<", \t\tY2 : "<<RPC_LocalY2<<std::endl;
          numLocalErrs++;
      }
      
      dist =  sqrt( pow(RPC_HitX - RPC_HitX2,2) + pow(RPC_HitY - RPC_HitY2,2) + pow(RPC_HitZ - RPC_HitZ2,2) );
      if ( checkGlobal && dist > 0.001) {
          std::cout<<"Global Match fails for #"<<jentry<<std::endl;
          std::cout<<"X : \t "<<RPC_HitX<<" -> \t"<<RPC_HitX2<<" Delta: \t"<<(RPC_HitX-RPC_HitX2)<<std::endl;
          std::cout<<"Y : \t "<<RPC_HitY<<" -> \t"<<RPC_HitY2<<" Delta: \t"<<(RPC_HitY-RPC_HitY2)<<std::endl;
          std::cout<<"Z : \t "<<RPC_HitZ<<" -> \t"<<RPC_HitZ2<<" Delta: \t"<<(RPC_HitZ-RPC_HitZ2)<<std::endl;
          std::cout<<"Station:\t"<<RPC_StationName2<<std::endl;
          allOkay=false;
          numGlobalErrs++;
      }
   }
   std::cout<<"Processed "<<jentry<<" PRDs" <<std::endl;
   if ( checkLocal ) std::cout<<"Local errors found = "<<numLocalErrs<<std::endl;
   if ( checkGlobal ) std::cout<<"Global errors found = "<<numGlobalErrs<<std::endl;
   return allOkay;
}


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
            file = argv[optind];
        }
        else if (sw=="--ref") {
            optind++;
            ref = argv[optind];
        } else if (sw=="-g") {
            checkGlobal=false;
        } else if (sw=="-G") {
            checkGlobal=true;
        } else if (sw=="-l") {
            checkLocal=false;
        } else if (sw=="-L") {
            checkLocal=true;
        } else if (sw=="-h") {
            std::cout << "Compares PRD positions. Options are: \n-h (help),\n--prd PRD (where PRD=RPC,CSC,TGC,MDT),"
            <<"\n --file FILE\n --ref REFERENCE FILE\n -g/G (don't/do check global positions),\n -l/L (don't/do check local positions)"
            << std::endl; 
            return 0;
        }else {
            std::cout << "Unknown switch: " 
                 << argv[optind] << std::endl;
            return 1;
        }
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

