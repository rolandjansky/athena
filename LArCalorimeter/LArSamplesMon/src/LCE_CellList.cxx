/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#include <vector>
#include <string> 
#include <set>
#include <iostream>

#include "LArSamplesMon/Data.h"
#include "LArSamplesMon/Interface.h"
#include "LArSamplesMon/HistoryIterator.h"
#include "LArSamplesMon/History.h"
#include "LArSamplesMon/LArCellsEmptyMonitoring.h"
#include "LArCafJobs/CellInfo.h"
#include "LArCafJobs/ShapeInfo.h"
#include "LArCafJobs/EventData.h"
#include "LArCafJobs/CaloId.h"

#include "TROOT.h"
#include "TApplication.h"
#include "TSystem.h"

#if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)
#include "Cintex/Cintex.h"
#endif

using namespace LArSamples;
using namespace std;

class LCE_CellList {

public:

  struct thrCounter_t {
      public:
	unsigned onlId;
	unsigned nSeen=0;
	unsigned nAboveSigNoise=0;
	unsigned nAboveAbsE=0;
	unsigned nAboveQ=0;
	unsigned bc_status=0;
	double Esum=0.0;

	short caloid=0;
	short FT=0;
	short slot=0;
	short channel=0;
	short layer=0;

    //Flags:
	bool EventEnergyCut=false;
	bool MeanCellHitCut=false;
	std::set<unsigned> LBs;

      thrCounter_t(const unsigned id) :
	onlId(id) {};
	
      };


  void setFlaggingThresholds(const float hitsPerLB, const unsigned upperCountThr, const double lowerEThr, const unsigned lowerCountThr, const double upperEThr);
  void setListingThresholds(const unsigned minNSeen, const unsigned minAboveSigNoise);


  void readDefectLBList(const char* LBfile);

  inline bool checkBadLBList(const unsigned lumiBlock) const {
    return (m_badLBs.find(lumiBlock)!=m_badLBs.end());
}

  std::vector<LCE_CellList::thrCounter_t> buildList(const char* inputfile, const float nSigma, const float Ethr, const float QThr,  unsigned& nLBsSeen) const;

  void writeList(const char* filename, const std::vector<LCE_CellList::thrCounter_t>& celllist) const;

  void addFlags(std::vector<LCE_CellList::thrCounter_t>& celllist, const unsigned nLBsSeen) const;
      
  bool applySelection(const LCE_CellList::thrCounter_t& counter) const;
      
  std::string partitionName(const short caloId, const short slot) const; 

  void printThresholds() const;
  
private:
  std::set<unsigned> m_badLBs;

  //Flagging thresholds
  float m_hitCountPerLBThreshold=0.01;
  unsigned m_UpperCountThreshold=50;
  unsigned m_LowerCountThreshold=20;
  double m_LowerCellEnergyThreshold=1000.0; //1 GeV
  double m_UpperCellEnergyThreshold=50000.0; //50GeV

  //Listing thresholds
  unsigned m_minNSeen=10;
  unsigned m_minAboveSigNoise=1;

};


void  LCE_CellList::setFlaggingThresholds(const float hitsPerLB, const unsigned upperCountThr, const double lowerEThr, const unsigned lowerCountThr, const double upperEThr){
    m_hitCountPerLBThreshold=hitsPerLB;
    m_UpperCountThreshold = upperCountThr;
    m_LowerCountThreshold = lowerCountThr;
    m_LowerCellEnergyThreshold = lowerEThr;
    m_UpperCellEnergyThreshold = upperEThr;
  }


void  LCE_CellList::setListingThresholds(const unsigned minNSeen, const unsigned minAboveSigNoise) {
  m_minNSeen=minNSeen;
  m_minAboveSigNoise=minAboveSigNoise;
}


void LCE_CellList:: printThresholds() const {
  printf ("Listing Thresholds:\n");
  printf ("\tMin number of appearences in LCE ntuple: %u\n",m_minNSeen);
  printf ("\tMin mumber of events with E> n Sigma Noise: %u\n", m_minAboveSigNoise);
  printf ("Flagging Thresholds:\n");
  printf ("\tMin number of events > sigNoise: %.3f * nLumiBlocks\n",m_hitCountPerLBThreshold);
  printf ("\tUpper count threshold for event energy cut %u\n", m_UpperCountThreshold);
  printf ("\tLower count threshold for event energy cut %u\n", m_LowerCountThreshold);
  printf ("\tUpper mean energy threshold for event energy cut %.2f MeV\n", m_UpperCellEnergyThreshold);
  printf ("\tLower mean energy threshold for event energy cut %.2f MeV\n",m_LowerCellEnergyThreshold);
}



void LCE_CellList::readDefectLBList(const char* LBfile) {

  if (!m_badLBs.empty()) 
    printf("Appending to already-existing list of bad lumi-blocks of size %zu\n",m_badLBs.size());
    

  std::ifstream infile(LBfile);
  std::string line;

  // assume single-line format with coma-separated LBs (from python)                                                                                                                
  std::getline(infile,line,'\n');
  if (line.empty()) {
    printf("No bad LBs found in file %s\n" ,(const char*)LBfile);
    return;
  }

  for (size_t pos=0;pos!=std::string::npos;pos=line.find(',',pos)) {
    if (pos) pos++; //Jump over comma if not the first iteration
    //std::cout << "Parsing " << line.c_str()+pos << std::endl;
    unsigned LB=std::atoi(line.c_str()+pos);
    m_badLBs.insert(LB);
  }
  
  printf("Number of bad lumi-blocks: %d\n",(int)m_badLBs.size());

  return;
}


std::vector< LCE_CellList::thrCounter_t>  LCE_CellList::buildList(const char* inputfile, const float nSigma, const float Ethr, const float Qthr, unsigned& nLBsSeen) const {

  std::vector<thrCounter_t> retvec;
  
  std::set<unsigned> nLBsSeenSet;

  LArSamples::Interface* tuple = (LArSamples::Interface*)Interface::open(inputfile);
  const unsigned nchannels = tuple->nChannels();

  retvec.reserve(nchannels);

  for (unsigned ichan=0;ichan<nchannels;++ichan) {
    const LArSamples::History* hist = tuple->cellHistory(ichan);
    if (!hist) continue;
    const LArSamples::CellInfo* cellInfo = hist->cellInfo();
    const int nEvents=hist->nData();
 
    thrCounter_t cnt(cellInfo->onlid());
    cnt.FT=cellInfo->feedThrough();
    cnt.slot=cellInfo->slot();
    cnt.channel=cellInfo->channel();
    cnt.caloid=cellInfo->calo();
    cnt.layer=cellInfo->layer();

    for (int iEvent=0;iEvent<nEvents;++iEvent) {
      const LArSamples::Data* data = hist->data(iEvent);
      const LArSamples::EventData* Evdata = data->eventData();
      if(!Evdata) continue;
      unsigned lumiBlock = Evdata->lumiBlock();
      if (checkBadLBList(lumiBlock)) continue; //skip bad LBs


      const double energy= data->energy();
      const double noise = data->noise();
      const double quality = data->quality();

      cnt.nSeen++;
      if (energy > nSigma * noise)  cnt.nAboveSigNoise++;
      if (energy > Ethr) cnt.nAboveAbsE++;
      if (quality > Qthr) cnt.nAboveQ++;
      if (!cnt.bc_status) cnt.bc_status=data->status();
      cnt.Esum+=energy;
      cnt.LBs.insert(lumiBlock);
      nLBsSeenSet.insert(lumiBlock);
    }//End loop over events

    if (cnt.nSeen>0) retvec.emplace_back(cnt);
  }//end loop over channels
  
  nLBsSeen=nLBsSeenSet.size();
  std::cout << "Evaluated a total of " << nLBsSeen << "LBs" << std::endl;

  delete tuple;
  return retvec;

}

void  LCE_CellList::addFlags(std::vector<LCE_CellList::thrCounter_t>& celllist, const unsigned nLBsSeen) const {
  for (thrCounter_t& counter : celllist) {
    counter.MeanCellHitCut=(counter.nAboveAbsE> m_hitCountPerLBThreshold*nLBsSeen);
    counter.EventEnergyCut= ((counter.nAboveSigNoise > m_UpperCountThreshold && (counter.Esum/counter.nAboveSigNoise) > m_LowerCellEnergyThreshold) ||
			     (counter.nAboveSigNoise > m_LowerCountThreshold && (counter.Esum/counter.nAboveSigNoise  > m_UpperCellEnergyThreshold)));
      
    }

}



bool LCE_CellList::applySelection(const LCE_CellList::thrCounter_t& counter) const {
  return counter.nSeen > m_minNSeen && counter.nAboveSigNoise > m_minAboveSigNoise;
}


void  LCE_CellList::writeList(const char* textfilename, const std::vector<LCE_CellList::thrCounter_t>& cellList) const {
  FILE* pFile = fopen (textfilename , "w");
  //               0          1        2      3       4         5                 6             7               8         9       10  
  fprintf(pFile,"onlid // partition // FT // Slot // channel // nAboveSigNoise // nAboveAbsE // MeanE [GeV] // fracQ4k // nLBs // Algoflag\n");

  for (const auto& cnt : cellList) {
    if (applySelection(cnt)) {
      unsigned flag=0;
      if (cnt.EventEnergyCut) {
	flag=1;
	if (cnt.MeanCellHitCut) flag=2;
      }
      //Expected format (by WDE):
      //online id Partition FT Slot Chan nEvt>10Sig nEvt>1GeV, MeanE, fracQ4k, nLBs, 
      //             0           1     2     3     4     5     6     7      8       9      10   
      fprintf(pFile,"0x%8.8x \t %7s \t %i \t %i \t %i \t %i \t %i \t %.3f \t %.3f \t %u \t %u", 
	      cnt.onlId, partitionName(cnt.caloid,cnt.layer).c_str(), 
	      cnt.FT, cnt.slot, cnt.channel, cnt.nAboveSigNoise, cnt.nAboveAbsE,
	      cnt.Esum/(1000.0*cnt.nSeen), (float)cnt.nAboveQ/cnt.nSeen, 
	      (unsigned)cnt.LBs.size(), flag );
      //for (const unsigned lb : cnt.LBs) {
      //	fprintf(pFile, "%i ", lb);
      //}

      fprintf(pFile,"\n");
    }
  }
  fclose(pFile);
  return;
}


std::string LCE_CellList::partitionName(const short caloId, const short layer) const {

  std::string name;
  std::string slayer=std::to_string(layer);
  std::string side;

  switch (caloId) {
  case EMB_C:
    name="EMB";
    side="C";
    if (layer==0) slayer="P";
    break;
  case EMB_A:
    name="EMB";
    side="A";
    if (layer==0) slayer="P";
    break;

  case EMEC_INNER_C:
  case EMEC_OUTER_C:
    name="EMEC";
    side="C";
    if (layer==0) slayer="P";
    break;

  case EMEC_INNER_A:
  case EMEC_OUTER_A:
    name="EMEC";
    side="A";
    if (layer==0) slayer="P";
    break;
    
  case HEC_A:
    name="HEC";
    side="A";
    break;

  case HEC_C:
    name="HEC";
    side="C";
    break;
      
  case FCAL_A:
    name="FCAL";
    side="A";
    break;

  case FCAL_C:
    name="FCAL";
    side="C";
    break;

  default:
    name="UNKNOWN";
    slayer="";
  }

  return name+slayer+side;
}
  
      /* FCAL_C = -5, HEC_C = -4, EMEC_INNER_C = -3, EMEC_OUTER_C = -2, EMB_C = -1,
    UNKNOWN_CALO = 0,
    EMB_A = 1, EMEC_OUTER_A = 2, EMEC_INNER_A = 3, HEC_A = 4, FCAL_A = 5,*/



int main(int argc, char** argv) {

  if (argc<3 || (argc>1 && (!strcmp(argv[1],"-h") || !strcmp(argv[1],"--help")))) {
    std::cout << "Syntax:" << std::endl;
    std::cout << "RunLCE.exe inFile outFile <defectLBfile>" << std::endl;
    return -1;
  }
    
  const char* inputFile=argv[1];
  const char* outputFile=argv[2];
  
  char* defectsLBFileName=nullptr;
  if (argc>3)
    defectsLBFileName=argv[3];

  TROOT root ("root", "root");
#if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)
  ROOT::Cintex::Cintex::Enable();
 #endif
 
  gSystem->Load("libLArCafJobsDict.so");
  gSystem->Load("libLArSamplesMonDict.so");
  

  if (gSystem->AccessPathName(inputFile))  {
    printf("Cannot access file %s.\n",inputFile);
    return -1;
  }


  LCE_CellList lceList;

  if (defectsLBFileName) {
    lceList.readDefectLBList(defectsLBFileName);
  }
  
  const float Ethr=1000.0; //1GeV
  const float nSigma=10.0;
  const float Qthr=4000;
  unsigned nLBsSeen=0;

  printf("Thresholds:\n");
  printf("\tAbsolute Energy: %.2f MeV\n",Ethr);
  printf("\tSigma Noise: %.2f\n",nSigma);
  printf("\tQuality Factor: %.2f\n\n",Qthr);

  lceList.printThresholds();
  
  std::vector<LCE_CellList::thrCounter_t> celllist=lceList.buildList(inputFile, nSigma, Ethr, Qthr, nLBsSeen);
  printf("Total number of cells read from LCE ntuple: %zu\n",celllist.size());
  lceList.addFlags(celllist, nLBsSeen);
  lceList.writeList(outputFile,celllist);
 
  return 0;
}
