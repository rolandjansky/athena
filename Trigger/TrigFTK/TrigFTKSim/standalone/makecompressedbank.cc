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
####### options specific to makecompressedbank
#
TOWER 0
# output/input file
#   format written by FTKTSPBank
PCACHEIN patterns.pcache.root
PCACHEOUT cached.root
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
#define IBLMODE "IBLMODE"
#define MODULELUTPATH "MODULELUTPATH"
#define TSP_SSMAP "TSP_SSMAP"
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
      if(!s_fsteering) {
         s_fsteering=new MakeSmallBankSteering();
         s_fsteering->AddIntPar(TOWER,1,-1);
         s_fsteering->AddStringPar(PCACHEIN,0,"");
         s_fsteering->AddStringPar(PCACHEOUT,1,"");
         s_fsteering->AddStringPar(CCACHEIN,0,"");
         s_fsteering->AddStringPar(CCACHEOUT,1,"");
         s_fsteering->AddIntPar(MAXPATT,1,-1);
         s_fsteering->AddIntPar(IBLMODE,1,1);
         s_fsteering->AddStringPar(ROOTIN,1);
         s_fsteering->AddStringPar(PMAP,1,"");
         s_fsteering->AddStringPar(RMAP,1,"");
         s_fsteering->AddStringPar(SSMAP,1,"");
         s_fsteering->AddStringPar(MODULELUTPATH,1,"");
         s_fsteering->AddStringPar(TSP_SSMAP,1,"");
         s_fsteering->AddDoublePar(TSP_SSID,0);
         s_fsteering->AddDoublePar(DC_SSID,0);
         s_fsteering->AddIntPar(PRINTSECTOR,1,-1);
         s_fsteering->AddIntPar(HWMODESS_TSP,1,2);
         s_fsteering->AddIntPar(HWMODESS_DC,1,2);
      }
      return s_fsteering;
   }
   MakeSmallBankSteering(void) {
      m_fPmap=0;
      m_fRmap=0;
      m_fSSmap=0;
      m_fSSmap_tsp=0;
   }
   int GetNumCCachedBankFileIn(void) const {
      return (*this)[CCACHEIN]->GetActualSize();
   }
   const char *GetCCachedBankFileIn(int ind) const {
      return (*(*this)[CCACHEIN])[ind];
   }
   int GetNumPCachedBankFileIn(void) const {
      return (*this)[PCACHEIN]->GetActualSize();
   }
   const char *GetPCachedBankFileIn(int ind) const {
      return (*(*this)[PCACHEIN])[ind];
   }
   const char *GetRootFileIn(void) const {
      return *(*this)[ROOTIN];
   }
   const char *GetCCacheFileOut(void) const {
      return *(*this)[CCACHEOUT];
   }
   const char *GetPCacheFileOut(void) const {
      return *(*this)[PCACHEOUT];
   }
   int GetNumSSIDdc(void) const {
      return (*this)[DC_SSID]->GetActualSize();
   }
   const pair<int,int> GetSSIDdc(int i) const {
      double dSSID=(*(*this)[DC_SSID])[i];
      pair<int,int> r;
      r.first=(int)dSSID;
      r.second=(int)((dSSID-r.first)*10.+0.5);
      return r;
   }
   int GetNumSSIDtsp(void) const {
      return (*this)[TSP_SSID]->GetActualSize();
   }
   const pair<int,int> GetSSIDtsp(int i) const {
      /// floating point input:  SSID.layer
      double dSSID=(*(*this)[TSP_SSID])[i];
      pair<int,int> r;
      r.first=(int)dSSID;
      r.second=(int)((dSSID-r.first)*10.+0.5);
      return r;
   }
   int GetTower(void) const {
      return (*this)[TOWER][0];
   }
   int GetPrintSector(void) const {
      return (*this)[PRINTSECTOR][0];
   }
   int GetMaxPatt(void) const {
      return (*this)[MAXPATT][0];
   }
   int GetHWModeSS_tsp(void) const {
      return (*this)[HWMODESS_TSP][0];
   }
   int GetHWModeSS_dc(void) const {
      return (*this)[HWMODESS_DC][0];
   }
   int GetIBLMODE(void) const {
      return (*this)[IBLMODE][0];
   }
   
   FTKSSMap *GetSSmap(void);
   FTKSSMap *GetSSmapTSP(void);
private:
   static MakeSmallBankSteering *s_fsteering;
   void LoadPmapRmap(void);

   FTKPlaneMap *m_fPmap;
   FTKRegionMap *m_fRmap;
   FTKSSMap *m_fSSmap;
   FTKSSMap *m_fSSmap_tsp;
};

MakeSmallBankSteering *MakeSmallBankSteering::s_fsteering=0;

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
      } else {
         logging.Info("parse")<<"opened steering file \""
                              <<argv[1]<<"\"\n";
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

   vector<FTK_CompressedAMBank *> banks;
   FTKSetup &ftkset = FTKSetup::getFTKSetup();
   ftkset.setHWModeSS(2);
   ftkset.setIBLMode(MakeSmallBankSteering::Instance()->GetIBLMODE());
   ftkset.setITkMode(0);

   FTKSSMap *ssMap=MakeSmallBankSteering::Instance()->GetSSmap();
   FTKSSMap *ssMapTSP=MakeSmallBankSteering::Instance()->GetSSmapTSP();
   for(int ibank=0;
       ibank<MakeSmallBankSteering::Instance()->GetNumPCachedBankFileIn();
       ibank++) {
      if(error) break;
      FTK_CompressedAMBank *bank=
         new FTK_CompressedAMBank(tower,0,ssMap,ssMapTSP,
                                  hwModeSS_tsp,hwModeSS_dc);
      bank->setCompressionScheme(FTK_CompressedAMBank::COMPRESSION_DELTA);
      TDirectory *pcacheFile=FTKRootFile::Instance()->OpenRootFileReadonly
         (MakeSmallBankSteering::Instance()->GetPCachedBankFileIn(ibank));
      if(pcacheFile) {
         error=bank->readPCachedBank(pcacheFile,0);
         banks.push_back(bank);
         delete pcacheFile;
      } else {
         logging.Error("pcache")
            <<"could not open "
            <<MakeSmallBankSteering::Instance()->GetPCachedBankFileIn(ibank)
            <<"\n";
         delete bank;
      }
   }
   for(int ibank=0;
       ibank<MakeSmallBankSteering::Instance()->GetNumCCachedBankFileIn();
       ibank++) {
      if(error) break;
      FTK_CompressedAMBank *bank=
         new FTK_CompressedAMBank(tower,0,ssMap,ssMapTSP,
                                  hwModeSS_tsp,hwModeSS_dc);
      bank->setCompressionScheme(FTK_CompressedAMBank::COMPRESSION_DELTA);
      error=bank->readROOTBankCache
         (MakeSmallBankSteering::Instance()->GetCCachedBankFileIn(ibank));
      banks.push_back(bank);
   }
   char const *rootInputName=
     MakeSmallBankSteering::Instance()->GetRootFileIn();
   int maxPatterns=MakeSmallBankSteering::Instance()->GetMaxPatt();
   if((!error) && rootInputName && rootInputName[0]) {
      FTK_CompressedAMBank *bank=
         new FTK_CompressedAMBank(tower,0,ssMap,ssMapTSP,
                                  hwModeSS_tsp,hwModeSS_dc);
      bank->setCompressionScheme(FTK_CompressedAMBank::COMPRESSION_DELTA);
      error=bank->readROOTBank(rootInputName,maxPatterns);
      banks.push_back(bank);
   }
   for(size_t iBank=1;iBank<banks.size();iBank++) {
     error=banks[0]->compare(banks[iBank]);
     if(error) {
        logging.Error("main")<<"bank[0] and bank["
                             <<iBank<<"] are not in agreement\n";
     } else {
        logging.Info("main")<<"bank[0] and bank["
                            <<iBank<<"] are in agreement\n";
     }
   }
   char const *ccachedBankOutputName=
      MakeSmallBankSteering::Instance()->GetCCacheFileOut();
   if((!error) && banks[0] && ccachedBankOutputName &&
      ccachedBankOutputName[0]) {
      error=banks[0]->writeCCachedBankFile(ccachedBankOutputName);
   }
   char const *pcachedBankOutputName=
      MakeSmallBankSteering::Instance()->GetPCacheFileOut();
   if((!error) && banks[0] && pcachedBankOutputName && 
      pcachedBankOutputName[0]) {
      error=banks[0]->writePCachedBankFile(pcachedBankOutputName);
   }

   if((!error) && banks[0]) {
      FTK_CompressedAMBank *bank=banks[0];
      std::vector<std::list<int> > roadFinderInput(bank->getNPlanes());
      for(int i=0;i<MakeSmallBankSteering::Instance()->GetNumSSIDdc();i++) {
         pair<int,int> dcSSIDlayer=
            MakeSmallBankSteering::Instance()->GetSSIDdc(i);
         int layer=dcSSIDlayer.second;
         int dcSSID=dcSSIDlayer.first;
         cout<<i<<" L="<<layer<<" dcSSID="<<dcSSID<<" -> tspSSID=[";
         vector<int> tspSSIDvector=bank->getTSPssidVector(layer,dcSSID);
         for(unsigned j=0;j<tspSSIDvector.size();j++) {
            cout<<" "<<tspSSIDvector[j];
         }
         cout<<"]\n";
         for(unsigned j=0;j<tspSSIDvector.size();j++) {
            roadFinderInput[layer].push_back(tspSSIDvector[j]);
         }
      }
      for(int i=0;i<MakeSmallBankSteering::Instance()->GetNumSSIDtsp();i++) {
         pair<int,int> tspSSIDlayer=
            MakeSmallBankSteering::Instance()->GetSSIDtsp(i);
         int layer=tspSSIDlayer.second;
         int tspSSID=tspSSIDlayer.first;
         std::pair<int,int> dcSSIDxy=bank->getDCssid(layer,tspSSID);
         cout<<"L="<<layer<<" tspSSID "<<tspSSID<<" -> dcSSID="<<dcSSIDxy.first
             <<" tspLOCAL="<<dcSSIDxy.second<<"\n";
         roadFinderInput[layer].push_back(tspSSID);
      }
      bank->init();
      bank->clear();
      bank->data_organizer_r(roadFinderInput);
      bank->am_in_r(roadFinderInput);
      bank->am_output();
      bank->printRoads(bank->getRoads(),-1);
   }
   if((!error) && banks[0]) {
      int printsector=MakeSmallBankSteering::Instance()->GetPrintSector();
      if(printsector>=0) {
         banks[0]->printSector(printsector);
      }
   }

   return error;
}

void MakeSmallBankSteering::LoadPmapRmap(void) {
  if(!m_fPmap) {
      m_fPmap = new FTKPlaneMap(*(*this)[PMAP]);
   }
   if(!m_fRmap) {
      m_fRmap = new FTKRegionMap(m_fPmap,*(*this)[RMAP]);
   }
   if((GetHWModeSS_tsp()==2)||(GetHWModeSS_dc()==2)) {
      m_fRmap->loadModuleIDLUT(*(*this)[MODULELUTPATH]);
   }
}

FTKSSMap *MakeSmallBankSteering::GetSSmap(void) {
   LoadPmapRmap();
   if(!m_fSSmap) {
      m_fSSmap=new FTKSSMap(m_fRmap,*(*this)[SSMAP],false); // FTKRoadFinderAlgo
   }
   return m_fSSmap;
}

FTKSSMap *MakeSmallBankSteering::GetSSmapTSP(void) {
   LoadPmapRmap();
   if(!m_fSSmap_tsp) {
      m_fSSmap_tsp=new FTKSSMap(m_fRmap,*(*this)[TSP_SSMAP],false);// FTKRoadFinderAlgo
   }
   return m_fSSmap_tsp;
}
