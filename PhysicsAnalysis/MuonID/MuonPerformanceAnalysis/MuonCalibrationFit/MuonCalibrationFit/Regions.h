/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Regions_h
#define Regions_h

#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>

#include <TFile.h>
#include <TString.h>
#include <TObjArray.h>

#include "MuonCalibrationFit/Tools.h"

class Regions {
private:
  bool                              m_debug;
  TString                           m_FileName;
  std::vector< TString >            m_Variables;
  std::vector< Tools::RegionInfo* > m_Info;
public:
  Regions() { Init(); }
  Regions( TString name, TString root_file_name, int debug );
  ~Regions();
  //:::
  bool Init();
  bool LoadConfiguration( TString name, TString root_file_name = "" );
  void Print();
  //:::
  std::vector< Tools::RegionInfo* > GetRegions() { return m_Info; }
};

#endif
