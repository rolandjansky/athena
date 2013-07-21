/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*************************************************************************************************************************/
// class analyseNtuple works on Track Validation Ntuples. It prouces histogram of efficiencies and fake rates from       //
// of EF TrackParticle collections (EF collection is matched to offline collection). The RoI is not used in the class    //
// class  should be compiled by .L AnalyseNtuple.C++                                                                     //
// and histograms are produce after following steps:                                                                     //
// AnalyseNtuple analyse;                                                                                                //
// analyse.initialize();                                                                                                 //
// analyse.execute();                                                                                                    //
// analyse.finalize();                                                                                                   //
/*************************************************************************************************************************/

#ifndef ANALYSENTUPLE_H
#define ANALYSENTUPLE_H

#include <string>

class TFile; 
class TTree;
class TH1F;

class AnalyseNtuple
{
 private:
  void regHistos();                                         // histograms are created here
  void setBrancheAddresses();                               // set branch adresses from the trees 
  bool isMatchedToEF();                                     // returns true if Offline track matches to EF track
  bool isMatchedToOff();                                    // returns true if EF track matches to Offline track
  bool passedOfflineCuts();                                 // returns true if Offline track passed quality cuts
  bool passedEFCuts();                                      // returns true if RF track passed quality cuts
  void fillEfficiency();                                    // fill efficiency histograms
  void fillFakeRate();                                      // fill fake rate histograms
  void finalizeHistos();                                    // divide histograms and save them or plot them
  void drawHistos();                                        // draw histograms on the screen
  void printTracksInEvent();                                // print all track in current event for bot offline and EF data
  
 TFile* file;                              // input file
 std::string offTreeName;                  // name of offline colection
 std::string efTreeName;                   // name of EF collection
 std::string fileName;                     // name of the input ROOT file
 
 // retrieve Trees for collection and event property 
 TTree *eventPropertyTree;         // tree with information about event 
 TTree *offTrkParticleTree;        // tree with ofline tracks
 TTree *efTrkParticleTree;         // tree with EF tracks

 Int_t numOfOffTrkPerEvent, numOfEFTrkPerEvent; // number of tracks in current event
 Int_t offTrkIndexBegin, efTrkIndexBegin; //index of first particle in current event
 Int_t eventNumberOff, eventNumberEF;

 // perigee and summaary of offline tracks
 Int_t numOfPixOff, numOfSCTOff;
 Float_t d0Off, z0Off, etaOff, phiOff;

 // perigee of EF tracks
 Int_t numOfPixEF, numOfSCTEF;
 Float_t d0EF, z0EF, etaEF, phiEF;

  // match tolerance parameters
 Float_t deltaEta, deltaPhi;

 // define cuts for offline tracks d0 and z0 are in mm
 Float_t z0OffCut, d0OffCut;
 Float_t z0EFCut, d0EFCut; 

 Int_t nHitOffCut;       // nHitOffCut = numOfPixOff + numOfSCTOff/2
 Int_t nHitEFCut;        // nHitEFCut = numOfPixEF + numOfSCTEF/2

 Int_t numOfEvents;      // total number of events in the Ntuple

 //histograms 
 TH1F *z0ResEfficiencyHist, *d0ResEfficiencyHist, *z0ffHist, *d0ffHist;
 TH1F *z0ResFakeRateHist, *d0ResFakeRateHist, *z0EFHist, *d0EFHist;

 public:
  AnalyseNtuple(std::string fileName_);   // constructor
  void initialize(); // initialize histograms histograms and variables 
  void execute();    // loop over events end fill histograms
  void finalize();   // write or plot histograms 
  void setOffCollectionName(std::string offColName);        // sets name of TTree where the offline tracks are stored 
  void setEFCollectionName(std::string  efColName);         // sets name of TTree where the EF tracks are stored 
  void setOfflineCuts(Float_t d0, Float_t z0, Int_t nHits); // offline track quality cuts on impact parameters d0, z0 and number of hits nHits= nPixHits + nSCTHits/2
  void setEFCuts(Float_t d0, Float_t z0, Int_t nHits);      // EF track quality cuts on impact parameters d0, z0 and number of hits nHits= nPixHits + nSCTHits/2
};

inline void AnalyseNtuple::setOffCollectionName(std::string offColName){offTreeName=offColName;}

inline void AnalyseNtuple::setEFCollectionName(std::string efColName){efTreeName=efColName;}

inline void AnalyseNtuple::setOfflineCuts(Float_t d0, Float_t z0, Int_t nHits){z0OffCut = z0; d0OffCut = d0; nHitEFCut = nHits;}

inline void AnalyseNtuple::setEFCuts(Float_t d0, Float_t z0, Int_t nHits) {z0EFCut=z0; d0EFCut=d0; nHitEFCut=nHits;}

// Exception classes
namespace AnalyseNtupleError{

class LoadTFileError
{
 std::string fileName; 

 public:
 LoadTFileError(std::string fileName_):fileName(fileName_){};
 inline std::string getFileName(){return fileName;};
};

class LoadTTreeError
{
 std::string treeName;
 
 public:
 LoadTTreeError(std::string treeName_):treeName(treeName_){};
 inline std::string getTreeName(){return treeName;};
};

}// end of AnalyseNtupleError namespace


#endif
