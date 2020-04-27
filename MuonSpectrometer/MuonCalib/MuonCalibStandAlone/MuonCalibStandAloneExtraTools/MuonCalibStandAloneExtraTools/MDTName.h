/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/* Author: Marco Vanadia vanadiam@roma1.infn.it*/

#ifndef MUONCALIBSTANDALONEEXTRATOOLS_MDTNAME_H
#define MUONCALIBSTANDALONEEXTRATOOLS_MDTNAME_H

#include "MuonCalibIdentifier/MuonFixedId.h" 
#include <string>
#include <sstream>
#include <stdlib.h>
#include "TString.h"

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
  MDTName(const MuonFixedId&);
  MDTName(const TString&);
  MDTName(const char*);
  MDTName(const std::string&);
  MDTName(const std::string&,const int,const int);
  MDTName(const std::string&,const int,const std::string&,const int);
  void MDTName_init(const TString&);
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
  TString OnlineToOfflineName(const TString&);
  TString OnlineToOfflineName(const char*);
  TString OnlineToOfflineName(const std::string&);
  TString OfflineToOnlineName(const TString&);
  TString OfflineToOnlineName(const char*);
  TString OfflineToOnlineName(const std::string&);

};

} //namespace MuonCalib

#endif
