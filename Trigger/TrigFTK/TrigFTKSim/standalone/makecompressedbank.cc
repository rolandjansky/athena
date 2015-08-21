/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 *  Utility program for the ATLAS FTK [makesmallbank]
 *

# sample steering file for makesmallbank
####### general options with FTKSteering,FTKLogging
#
#  level 0: suppress FATAL,ERROR,WARNING,INFO,PROGRESS (no printout at all)
#  level 1: suppress ERROR,WARNING,INFO,PROGRESS
#  level 2: suppress WARNING,INFO,PROGRESS
#  level 3: suppress INFO,PROGRESS
#  level 4: suppress PROGRESS (default)
#  level 5: print everything
LOGGING_PRINT_LEVEL 4
#
#  level 0: always continue
#  level 1: stop if FATAL
#  level 2: stop if FATAL,ERROR (default)
#  level 3: stop if FATAL,ERROR,WARNING
LOGGING_ABORT_LEVEL 2
#
####### options specific to makesmallbank
#
# input files: TTree "Bank" with objects FTKPattern
NINPUTS 1
# first file (text)
bank.pcache.root

# output/input file
#   format written by FTKTSPBank
PCACHEIN patterns.pcache.root
PCACHEOUT cached.root
#
#   binary format (not compatible)
# BINARYIN ssidFile.bin
BINARYOUT ssidFile.bin
#
# binarylookup-tables output file
CCACHEIN ssidFile.root
CCACHEOUT ssidFile.root
#
# for experts:
# select hwradware mode for TSP-SSID and/or DC-SSID encoding
SSHWMODEID_TSP 0
SSHWMODEID_DC 0
#
# list of SSIDs to test pattern finding
#
# TSP_SSID 8 19215 32415 42815 30505 38105 45705 45705 53305
#
# print pattern bank for a given sector
PRINTSECTOR 0
 */

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <unistd.h>
//#include <TString.h>
//#include <TFile.h>
//#include <TTree.h>
//#include <inttypes.h>

#include "TrigFTKSim/FTK_CompressedAMBank.h"
#include "TrigFTKSim/FTKSteering.h"
#include "TrigFTKSim/FTKRootFile.h"
#include "TrigFTKSim/FTKPattern.h"

//#include "TrigFTKSim/FTKPMap.h"
//#include "TrigFTKSim/FTKRegionMap.h"
#include "TrigFTKSim/FTKSSMap.h"

using namespace std;

#define TOWER "TOWER"
#define PCACHEIN "PCACHEIN"
#define PCACHEOUT "PCACHEOUT"
#define PMAP "PMAP"
#define RMAP "RMAP"
#define SSMAP "SSMAP"
#define TSP_SSMAP "TSP_SSMAP"
#define BINARYIN "BINARYIN"
#define BINARYOUT "BINARYOUT"
#define CCACHEIN "CCACHEIN"
#define CCACHEOUT "CCACHEOUT"
#define MAXPATT "MAXPATT"
#define ROOTIN "ROOTIN"
#define TSP_SSID "TSP_SSID"
#define DC_SSID "DC_SSID"
#define PRINTSECTOR "PRINTSECTOR"
#define HWMODESS_TSP "HWMODESS_TSP"
#define HWMODESS_DC "HWMODESS_DC"

class MakeSmallBankSteering : public FTKSteering {
public:
   static MakeSmallBankSteering *Instance(void) {
      if(!fSteering) {
         fSteering=new MakeSmallBankSteering();
         fSteering->AddIntPar(TOWER,1,-1);
         fSteering->AddStringPar(PCACHEIN,1);
         fSteering->AddStringPar(PCACHEOUT,1);
         fSteering->AddStringPar(BINARYIN,1);
         fSteering->AddStringPar(BINARYOUT,1);
         fSteering->AddStringPar(CCACHEIN,1);
         fSteering->AddStringPar(CCACHEOUT,1);
         fSteering->AddIntPar(MAXPATT,1,-1);
         fSteering->AddStringPar(ROOTIN,1);
         fSteering->AddStringPar(PMAP,1,
				 "../config/map_file/raw_8LcIbl123.pmap");
         fSteering->AddStringPar(RMAP,1,
				 "../config/map_file/raw_12Libl.tmap");
         fSteering->AddStringPar(SSMAP,1,
				 "../config/ss_file/raw_30x32x72Ibl.ss");
         fSteering->AddStringPar(TSP_SSMAP,1,
				 "../config/ss_file/raw_15x16x36Ibl.ss");
         fSteering->AddDoublePar(TSP_SSID,0);
         fSteering->AddDoublePar(DC_SSID,0);
         fSteering->AddIntPar(PRINTSECTOR,1,-1);
         fSteering->AddIntPar(HWMODESS_TSP,1,-1);
         fSteering->AddIntPar(HWMODESS_DC,1,-1);
      }
      return fSteering;
   }
   MakeSmallBankSteering(void) {
      fPmap=0;
      fRmap=0;
      fSSmap=0;
      fSSmap_tsp=0;
   }
   const char *GetCCachedBankFileIn(void) const {
      return *(*this)[CCACHEIN];
   }
   const char *GetPCachedBankFileIn(void) const {
      return *(*this)[PCACHEIN];
   }
   const char *GetBinaryFileIn(void) const {
      return *(*this)[BINARYIN];
   }
   const char *GetRootFileIn(void) const {
      return *(*this)[ROOTIN];
   }
   const char *GetBinaryFileOut(void) const {
      return *(*this)[BINARYOUT];
   }
   const char *GetCCacheFileOut(void) const {
      return *(*this)[CCACHEOUT];
   }
   const char *GetPCacheFileOut(void) const {
      return *(*this)[PCACHEOUT];
   }
   const int GetNSSIDdc(void) {
      return (*this)[DC_SSID]->GetActualSize();
   }
   const pair<int,int> GetSSIDdc(int i) {
      double dSSID=(*(*this)[DC_SSID])[i];
      pair<int,int> r;
      r.first=(int)dSSID;
      r.second=(int)((dSSID-r.first)*10.+0.5);
      return r;
   }
   int GetNSSIDtsp(void) {
      return (*this)[TSP_SSID]->GetActualSize();
   }
   const pair<int,int> GetSSIDtsp(int i) {
      double dSSID=(*(*this)[TSP_SSID])[i];
      pair<int,int> r;
      r.first=(int)dSSID;
      r.second=(int)((dSSID-r.first)*10.+0.5);
      return r;
   }
   int GetTower(void) {
      return (*this)[TOWER][0];
   }
   int GetPrintSector(void) {
      return (*this)[PRINTSECTOR][0];
   }
   int GetMaxPatt(void) {
      return (*this)[MAXPATT][0];
   }
   int GetHWModeSS_tsp(void) {
      return (*this)[HWMODESS_TSP][0];
   }
   int GetHWModeSS_dc(void) {
      return (*this)[HWMODESS_DC][0];
   }
   
   FTKSSMap *GetSSmap(void);
   FTKSSMap *GetSSmapTSP(void);
private:
   static MakeSmallBankSteering *fSteering;
   void LoadPmapRmap(void);

   FTKPlaneMap *fPmap;
   FTKRegionMap *fRmap;
   FTKSSMap *fSSmap;
   FTKSSMap *fSSmap_tsp;
};

MakeSmallBankSteering *MakeSmallBankSteering::fSteering=0;

int main(int argc, char const *argv[]) {
   // ============ read steering ==================
   int error=0;
   istream *steering= &cin;
   FTKLogging logging("main");
   if(argc>1) {
      steering=/* ftk_dcap::open_for_read */ new ifstream(argv[1]);
      if(!steering) {
         logging.Fatal("parse")<<"failed to open steering file \""
                               <<argv[1]<<"\"\n";
         error=3;
      }
   }
   if(MakeSmallBankSteering::Instance()->Parse(*steering)<0) {
      logging.Error("parse")<<"errors found in steering\n";
      error=2;
   }
   if(steering!= &cin) delete steering;

   MakeSmallBankSteering::Instance()->Print(cout);

   int tower=MakeSmallBankSteering::Instance()->GetTower();
   if(tower<0) {
      logging.Fatal("tower")<<"tower number not set\n";
   }

   int hwModeSS_tsp=MakeSmallBankSteering::Instance()->GetHWModeSS_tsp();
   int hwModeSS_dc=MakeSmallBankSteering::Instance()->GetHWModeSS_dc();

   char const *pcachedBankFileIn=
      MakeSmallBankSteering::Instance()->GetPCachedBankFileIn();
   FTK_CompressedAMBank *bank[4]={0,0,0,0};
   int nBank=0;
   FTKSSMap *ssMap=MakeSmallBankSteering::Instance()->GetSSmap();
   FTKSSMap *ssMapTSP=MakeSmallBankSteering::Instance()->GetSSmapTSP();
   if((!error)&& pcachedBankFileIn && pcachedBankFileIn[0]) {
      bank[nBank]=new FTK_CompressedAMBank(tower,0,ssMap,ssMapTSP,
                                           hwModeSS_tsp,hwModeSS_dc);
      error=bank[nBank++]->readROOTBankCache(pcachedBankFileIn);
   }
   char const *ccachedBankFileIn=
      MakeSmallBankSteering::Instance()->GetCCachedBankFileIn();
   if((!error)&& ccachedBankFileIn && ccachedBankFileIn[0]) {
      bank[nBank]=new FTK_CompressedAMBank(tower,0,ssMap,ssMapTSP,
                                           hwModeSS_tsp,hwModeSS_dc);
      error=bank[nBank++]->readROOTBankCache(ccachedBankFileIn);
   }
   char const *binaryInputName=
     MakeSmallBankSteering::Instance()->GetBinaryFileIn();
   if((!error) && binaryInputName && binaryInputName[0]) {
      bank[nBank]=new FTK_CompressedAMBank(tower,0,ssMap,ssMapTSP,
                                           hwModeSS_tsp,hwModeSS_dc);
     error=bank[nBank++]->readBinaryFile(binaryInputName);
   }
   char const *rootInputName=
     MakeSmallBankSteering::Instance()->GetRootFileIn();
   int maxPatterns=MakeSmallBankSteering::Instance()->GetMaxPatt();
   if((!error) && rootInputName && rootInputName[0]) {
      bank[nBank]=new FTK_CompressedAMBank(tower,0,ssMap,ssMapTSP,
                                           hwModeSS_tsp,hwModeSS_dc);
      error=bank[nBank++]->readROOTBank(rootInputName,maxPatterns);
   }
   for(int iBank=1;iBank<nBank;iBank++) {
     error=bank[0]->compare(bank[iBank]);
     if(error) {
        logging.Error("main")<<"bank[0] and bank["
                             <<iBank<<"] are not in agreement\n";
     }
   }
   char const *binaryOutputName=
      MakeSmallBankSteering::Instance()->GetBinaryFileOut();
   if((!error) && bank[0] && binaryOutputName && binaryOutputName[0]) {
      error=bank[0]->writeBinaryFile(binaryOutputName);
   }
   char const *ccachedBankOutputName=
      MakeSmallBankSteering::Instance()->GetCCacheFileOut();
   if((!error) && bank[0] && ccachedBankOutputName &&
      ccachedBankOutputName[0]) {
      error=bank[0]->writeCCachedBankFile(ccachedBankOutputName);
   }
   char const *pcachedBankOutputName=
      MakeSmallBankSteering::Instance()->GetPCacheFileOut();
   if((!error) && bank[0] && pcachedBankOutputName && 
      pcachedBankOutputName[0]) {
      error=bank[0]->writePCachedBankFile(pcachedBankOutputName);
   }

   if((!error) && bank[0]) {
#ifdef USE_COMPRESSEDAMBANK_DC
      std::vector<FTK_CompressedAMBank>
#else
         std::vector<std::list<int> >
#endif
         roadFinderInput(bank[0]->getNPlanes());
      for(int i=0;i<MakeSmallBankSteering::Instance()->GetNSSIDdc();i++) {
         pair<int,int> dcSSIDlayer=
            MakeSmallBankSteering::Instance()->GetSSIDdc(i);
         int layer=dcSSIDlayer.second;
         int dcSSID=dcSSIDlayer.first;
         cout<<i<<" L="<<layer<<" dcSSID="<<dcSSID<<" -> tspSSID=[";
         vector<int> tspSSIDvector=bank[0]->getTSPssidVector(layer,dcSSID);
         for(unsigned j=0;j<tspSSIDvector.size();j++) {
            cout<<" "<<tspSSIDvector[j];
         }
         cout<<"]\n";
#ifdef USE_COMPRESSEDAMBANK_DC
         roadFinderInput[layer][dcSSID]=(1<<tspSSIDvector.size())-1;
#else
         for(unsigned j=0;j<tspSSIDvector.size();j++) {
            roadFinderInput[layer].push_back(tspSSIDvector[j]);
         }
#endif
      }
      for(int i=0;i<MakeSmallBankSteering::Instance()->GetNSSIDtsp();i++) {
         pair<int,int> tspSSIDlayer=
            MakeSmallBankSteering::Instance()->GetSSIDtsp(i);
         int layer=tspSSIDlayer.second;
         int tspSSID=tspSSIDlayer.first;
         std::pair<int,int> dcSSIDxy=bank[0]->getDCssid(layer,tspSSID);
         cout<<"L="<<layer<<" tspSSID "<<tspSSID<<" -> dcSSID="<<dcSSIDxy.first
             <<" tspLOCAL="<<dcSSIDxy.second<<"\n";
#ifdef USE_COMPRESSEDAMBANK_DC
         roadFinderInput[layer][dcSSIDxy.first] |= 1<<dcSSIDxy.second;
#else
         roadFinderInput[layer].push_back(tspSSID);
#endif
      }
      bank[0]->init();
      bank[0]->clear();
      bank[0]->data_organizer_r(roadFinderInput);
      bank[0]->am_in_r(roadFinderInput);
      bank[0]->am_output();
      bank[0]->printRoads(bank[0]->getRoads(),-1);
   }
   if((!error) && bank[0]) {
      int printsector=MakeSmallBankSteering::Instance()->GetPrintSector();
      if(printsector>=0) {
         bank[0]->printSector(printsector);
      }
   }

   return error;
}

void MakeSmallBankSteering::LoadPmapRmap(void) {
  if(!fPmap) {
      fPmap = new FTKPlaneMap(*(*this)[PMAP]);
   }
   if(!fRmap) {
      fRmap = new FTKRegionMap(fPmap,*(*this)[RMAP]);
   }
}

FTKSSMap *MakeSmallBankSteering::GetSSmap(void) {
   LoadPmapRmap();
   if(!fSSmap) {
      fSSmap=new FTKSSMap(fRmap,*(*this)[SSMAP],false); // FTKRoadFinderAlgo
   }
   return fSSmap;
}

FTKSSMap *MakeSmallBankSteering::GetSSmapTSP(void) {
   LoadPmapRmap();
   if(!fSSmap_tsp) {
      fSSmap_tsp=new FTKSSMap(fRmap,*(*this)[TSP_SSMAP],false);// FTKRoadFinderAlgo
   }
   return fSSmap_tsp;
}
