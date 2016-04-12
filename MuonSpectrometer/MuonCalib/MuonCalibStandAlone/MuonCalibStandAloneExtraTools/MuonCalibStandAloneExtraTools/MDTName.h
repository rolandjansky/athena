/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* Author: Marco Vanadia vanadiam@roma1.infn.it*/

#ifndef MUONCALIBSTANDALONEEXTRATOOLS_MDTNAME_H
#define MUONCALIBSTANDALONEEXTRATOOLS_MDTNAME_H

#include "MuonCalibIdentifier/MuonFixedId.h" 
#include <string>
#include <sstream>
#include <stdlib.h>
#include "TString.h"

//using namespace std;

namespace MuonCalib{

class MDTName{

private:
  TString m_name;
  int m_eta_on;
  int m_eta_off;
  int m_sector_on;
  int m_sector_off;
  char m_side;

public:
  MDTName();
  MDTName(MuonFixedId);
  MDTName(TString);
  MDTName(char*);
  MDTName(std::string);
  MDTName(std::string,int,int);
  MDTName(std::string,int,std::string,int);
  void MDTName_init(TString);
  std::string getOnlineName();
  std::string getOfflineName();
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
  std::string getRegion();
  std::string getStation();
  std::string getSize();
  std::string getSide();
  std::string getName();
  TString OnlineToOfflineName(TString);
  TString OnlineToOfflineName(char*);
  TString OnlineToOfflineName(std::string);
  TString OfflineToOnlineName(TString);
  TString OfflineToOnlineName(char*);
  TString OfflineToOnlineName(std::string);

};

} //namespace MuonCalib

#endif
