/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigHLTCellDiagnostics.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigHLTJetRec
//
// AUTHOR:   Valentinos Christodoulou
//
// Description: Hypo for plotting cell histograms
//
// ********************************************************************

#ifndef TRIGHLTJETREC_TRIGHLTCELLDIAGNOSTICS_H
#define TRIGHLTJETREC_TRIGHLTCELLDIAGNOSTICS_H

#include "TrigInterfaces/HypoAlgo.h"
#include <map>

class TH1D;
class TH2D;
	
class TrigHLTCellDiagnostics: public HLT::HypoAlgo {
	
public:
  TrigHLTCellDiagnostics(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigHLTCellDiagnostics();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, bool& pass);

  void addHist(std::map<TString,TH1D*> &hMap, TString tag, const int &bins, double min, double max);
  void addHist(std::map<TString,TH2D*> &hMap, TString tag, const int &binsX, double minX, double maxX,
  	                                                       const int &binsY, double minY, double maxY);
  
private:
  std::string m_chainName;
  std::map<TString,TH1D*> hMap1D;
  std::map<TString,TH2D*> hMap2D;
}; 

#endif
