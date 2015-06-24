/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigHLTJetDiagnostics.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigHLTJetRec
//
// AUTHOR:   Valentinos Christodoulou
//
// Description: Hypo for plotting jet histograms
//
// ********************************************************************

#ifndef TRIGHLTJETREC_TRIGHLTJETDIAGNOSTICS_H
#define TRIGHLTJETREC_TRIGHLTJETDIAGNOSTICS_H

#include "TrigInterfaces/HypoAlgo.h"
#include <map>

class TH1D;
class TH2D;
	
class TrigHLTJetDiagnostics: public HLT::HypoAlgo {
	
public:
  TrigHLTJetDiagnostics(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigHLTJetDiagnostics();

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
