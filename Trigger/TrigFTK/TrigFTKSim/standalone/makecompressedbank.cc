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
#   if NINPUTS is given
#   then the pcache file is read and the
#   PATTENTSBYSECTOR file is written (patterns ordered by sector)
#
PATTERNSBYSECTOR patternsBySector.root
#
# binary lookup-tables output file
BINARYFILEOUT ssidFile.bin
#
# compressed lookup-tables input file
# BINARYFILEIN ssidFile.bin
#
# binarylookup-tables output file
ROOTFILEOUT ssidFile.root
#
# output in cached bank format
CACHEDBANKFILEOUT cached.root
#
# list of SSIDs to test pattern finding
#
# TSP_SSID 8 19215 32415 42815 30505 38105 45705 45705 53305
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

#define NINPUTS "NINPUTS"
#define PATTERNSBYSECTOR "PATTERNSBYSECTOR"
#define PMAP "PMAP"
#define RMAP "RMAP"
#define SSMAP "SSMAP"
#define TSP_SSMAP "TSP_SSMAP"
#define BINARYFILEIN "BINARYFILEIN"
#define BINARYFILEOUT "BINARYFILEOUT"
#define ROOTFILEIN "ROOTFILEIN"
#define ROOTFILEOUT "ROOTFILEOUT"
#define CACHEDBANKFILEOUT "CACHEDBANKFILEOUT"
#define TSP_SSID "TSP_SSID"
#define DC_SSID "DC_SSID"

class MakeSmallBankSteering : public FTKSteering {
public:
   static MakeSmallBankSteering *Instance(void) {
      if(!fSteering) {
         fSteering=new MakeSmallBankSteering();
         fSteering->AddStringPar(NINPUTS,0);
         fSteering->AddStringPar(PATTERNSBYSECTOR,1);
         fSteering->AddStringPar(BINARYFILEIN,1);
         fSteering->AddStringPar(BINARYFILEOUT,1);
         fSteering->AddStringPar(ROOTFILEIN,1);
         fSteering->AddStringPar(ROOTFILEOUT,1);
         fSteering->AddStringPar(CACHEDBANKFILEOUT,1);
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
      }
      return fSteering;
   }
   MakeSmallBankSteering(void) {
      fPmap=0;
      fRmap=0;
      fSSmap=0;
      fSSmap_tsp=0;
   }
   int GetNInput(void) const {
      return (*this)[NINPUTS]->GetActualSize();
   }
   const char *GetInput(int i) const {
      return (*(*this)[NINPUTS])[i];
   }
   const char *GetPatternsbysector(void) const {
      return *(*this)[PATTERNSBYSECTOR];
   }
   const char *GetBinaryFileIn(void) const {
      return *(*this)[BINARYFILEIN];
   }
   const char *GetRootFileIn(void) const {
      return *(*this)[ROOTFILEIN];
   }
   const char *GetBinaryFileOut(void) const {
      return *(*this)[BINARYFILEOUT];
   }
   const char *GetRootFileOut(void) const {
      return *(*this)[ROOTFILEOUT];
   }
   const char *GetCachedBankFileOut(void) const {
      return *(*this)[CACHEDBANKFILEOUT];
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
   const int GetNSSIDtsp(void) {
      return (*this)[TSP_SSID]->GetActualSize();
   }
   const pair<int,int> GetSSIDtsp(int i) {
      double dSSID=(*(*this)[TSP_SSID])[i];
      pair<int,int> r;
      r.first=(int)dSSID;
      r.second=(int)((dSSID-r.first)*10.+0.5);
      return r;
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

   int nInput=MakeSmallBankSteering::Instance()->GetNInput();
   char const *patternsbysectorName=
      MakeSmallBankSteering::Instance()->GetPatternsbysector();
   FTK_CompressedAMBank *bank[2]={0,0};
   FTKSSMap *ssMap=MakeSmallBankSteering::Instance()->GetSSmap();
   FTKSSMap *ssMapTSP=MakeSmallBankSteering::Instance()->GetSSmapTSP();
   if((!error)&&(nInput>0)&&(patternsbysectorName)&&
      patternsbysectorName[0]) {
      bank[0]=new FTK_CompressedAMBank(0,0,ssMap,ssMapTSP);
      logging.Info("main")<<"pcache -> patternsBySector"<<nInput<<"\n";
      logging.Info("main")<<"number of input files: "<<nInput<<"\n";
      vector<char const *> inputs;
      for(int iInput=0;iInput<nInput;iInput++) {
	inputs.push_back(MakeSmallBankSteering::Instance()->
			 GetInput(iInput));
      }
      error=bank[0]->convertCachedBankFiles(inputs,patternsbysectorName);
   }
   // read sector-ordered patterns
   if((!error) && patternsbysectorName && patternsbysectorName[0]) {
      if(!bank[0]) bank[0]=new FTK_CompressedAMBank(0,0,ssMap,ssMapTSP);
     error=bank[0]->readSectorOrderedFile(patternsbysectorName);
   }
   char const *binaryInputName=
     MakeSmallBankSteering::Instance()->GetBinaryFileIn();
   if((!error) && binaryInputName && binaryInputName[0]) {
     int iBank=0;
     if(bank[0]) iBank=1;
     bank[iBank]=new FTK_CompressedAMBank(0,0,ssMap,ssMapTSP);
     error=bank[iBank]->readBinaryFile(binaryInputName);
   }
   char const *rootInputName=
     MakeSmallBankSteering::Instance()->GetRootFileIn();
   if((!error) && rootInputName && rootInputName[0]) {
     int iBank=0;
     if(bank[0]) iBank=1;
     if(bank[iBank]) {
       logging.Error("main")<<"three input files specified\n";
       error++;
     } else {
        bank[iBank]=new FTK_CompressedAMBank(0,0,ssMap,ssMapTSP);
        error=bank[iBank]->readRootFile(rootInputName);
     }
   }
   if(bank[1]) {
     error=bank[0]->compare(bank[1]);
     if(error) {
       logging.Error("main")<<"the banks are not in agreement\n";
     }
   }
   char const *binaryOutputName=
      MakeSmallBankSteering::Instance()->GetBinaryFileOut();
   if((!error) && bank[0] && binaryOutputName && binaryOutputName[0]) {
      error=bank[0]->writeBinaryFile(binaryOutputName);
   }
   char const *rootOutputName=
      MakeSmallBankSteering::Instance()->GetRootFileOut();
   if((!error) && bank[0] && rootOutputName && rootOutputName[0]) {
      error=bank[0]->writeRootFile(rootOutputName);
   }
   char const *cachedBankOutputName=
      MakeSmallBankSteering::Instance()->GetCachedBankFileOut();
   if((!error) && bank[0] && cachedBankOutputName && cachedBankOutputName[0]) {
      error=bank[0]->writeCachedBankFile(cachedBankOutputName);
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
      bank[0]->clearDOandAM();
      bank[0]->simulateAM(roadFinderInput);
      bank[0]->printRoads(bank[0]->getRoads(),-1);
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
