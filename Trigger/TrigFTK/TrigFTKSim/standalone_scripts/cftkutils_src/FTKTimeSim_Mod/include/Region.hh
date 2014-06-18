/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include "IBoardBase.hh"

#ifndef REGION_HH
#define REGION_HH

#include "DataFormatter.hh"
#include "DataOrganizer.hh"
#include "AssociativeMemory.hh"
#include "SecDataOrganizer.hh"
#include "TrackFitter.hh"
#include "SecStageSecDataOrganizer.hh"
// #include "SecAssociativeMemory.hh"   hideyoshi
#include "SecStageTrackFitter.hh"

// #define NDATAFORMATTER 0
// #define NDATAORGANIZER 6
// #define NASSOCIATIVEMEMORY 12
// #define NTRACKFITTER 1

#define RG_T_IN 0
#define RG_T_OUT 0
#define RG_T_PROC 0
#define RG_T_DELAY 0


class Region : public IOTimeBase, public IBoardBase {

private:
  
  std::vector<DataFormatter> cDF;
  std::vector<DataOrganizer> cDO;
  std::vector<AssociativeMemory> cAM;
  std::vector<SecDataOrganizer> cSecDO;
  std::vector<TrackFitter> cTF;
  std::vector<SecStageSecDataOrganizer> cSecStageSecDO;
  std::vector<SecStageTrackFitter> cSecStageTF;

  int region_index;

public:
  Region() {}
  Region(int region_index);
  ~Region() {}

  InputValues iv;

  void SetInputValues(InputValues iv) {this->iv = iv;};

  int GetNDataFormatter() {return NDATAFORMATTER;}
  int GetNDataOrganizer() {return NDATAORGANIZER;}
  int GetNSecDataOrganizer() {return NDATAORGANIZER;}
  int GetNAssociativeMemory() {return NASSOCIATIVEMEMORY;}
  int GetNTrackFitter() {return NTRACKFITTER;}
  int GetNSecStageDataOrganizer() {return NSECSTAGEDATAORGANIZER;}
  int GetNSecStageTrackFitter() {return NSECSTAGETRACKFITTER;} 

  DataFormatter GetDataFormatter(int index) {return this->cDF[index];}
  DataOrganizer GetDataOrganizer(int index) {return this->cDO[index];}
  AssociativeMemory GetAssociativeMemory(int index) {return this->cAM[index];}
  SecDataOrganizer GetSecDataOrganizer(int index) {return this->cSecDO[index];}
  TrackFitter GetTrackFitter(int index) {return this->cTF[index];}
  SecStageSecDataOrganizer GetSecStageSecDataOrganizer(int index) {return this->cSecStageSecDO[index];}
  SecStageTrackFitter GetSecStageTrackFitter(int index) {return this->cSecStageTF[index];} 
  
  //  void Calc(int s_split);
  void Calc();

  ClassDef(Region,2)

};
#endif
