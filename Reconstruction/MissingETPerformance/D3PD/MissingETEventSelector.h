/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MISSINGETEVENTSELECTOR_H
#define MISSINGETEVENTSELECTOR_H

#include "D3PD.h"

class MissingETEventSelector {
public:
  MissingETEventSelector();
  MissingETEventSelector(const MissingETEventSelector &other);
  ~MissingETEventSelector() {}

  void initialize(D3PD *data) {_data = data;}
  D3PD *_data;

  bool passesN90AndHECFracCut();
  bool passes_fbadQCutAndEmFracCut();
  bool passes_JetTime();
  bool passesL1J15();
  bool passesVertex();
  bool passesMBTSTime();
  bool passesCaloTime();
  bool passes_hecf_quality1();
  bool passes_hecf_quality2();
  bool passes_EmFracCut();
  bool passes_FmaxAndEtaCut();

  bool hasExactlyOneVertex();
  bool hasExactlyTwoVertex();
  bool hasThreeAndMoreVertex();
 
  bool hasNoJets();
  bool hasOneJet();
  bool hasOnlyOneJetAbove20();
  bool hasOneJetAbove20();
  bool hasTwoJetAbove20();
  bool hasTwoJetAbove30();
  bool hasNoJetAbove20();
  
  //bool hasWmunu();
  //bool hasWenu();
};

#endif // MISSINGETEVENTSELECTOR_H
