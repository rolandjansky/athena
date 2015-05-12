/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* Author: Marco Vanadia vanadiam@roma1.infn.it*/

#ifndef _MDTName_H
#define _MDTName_H

#include "MuonCalibIdentifier/MuonFixedId.h" 
#include <string>
#include <sstream>
#include <stdlib.h>
#include "TString.h"

using namespace std;

namespace MuonCalib{

class MDTName{

private:

  TString name;
  int eta_on;
  int eta_off;
  int sector_on;
  int sector_off;
  char side;

public:

  MDTName();
  MDTName(MuonFixedId);
  MDTName(TString);
  MDTName(char*);
  MDTName(string);
  MDTName(string,int,int);
  MDTName(string,int,string,int);
  void MDTName_init(TString);
  string getOnlineName();
  string getOfflineName();
  bool isBarrel();
  bool isEndcap();
  bool isInner();
  bool isMiddle();
  bool isOuter();
  bool isExtra();
  bool isForward();
  bool isBackward();
  bool isLarge();
  bool isSmall();
  int getOnlineSector();
  int getOfflineSector();
  int getOnlineEta();
  int getOfflineEta();
  string getRegion();
  string getStation();
  string getSize();
  string getSide();
  string getName();
  TString OnlineToOfflineName(TString);
  TString OnlineToOfflineName(char*);
  TString OnlineToOfflineName(string);
  TString OfflineToOnlineName(TString);
  TString OfflineToOnlineName(char*);
  TString OfflineToOnlineName(string);

};

} //end of namespace MuonCalib

#endif
