/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUARAEXAMPLE_H
#define TAUARAEXAMPLE_H

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TauToolCompiledARAaExample.h
/// Package : offline/PhysicsAnalysis/TauTools
/// Author  : Jyothsna Komaragiri and Dugan O'Neil
/// Created : September 2007
///
/// DESCRIPTION:
///           This example shows how to access TauExamples class in ARA C++
///
/// INSTRUCTIONS: Posted on TauTools Wiki page
///               https://twiki.cern.ch/twiki/bin/view/Atlas/TauAnalysis
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TauTools/TauExamples.h"

#include "TTree.h"
#include "TFile.h"

class TauToolCompiledARAExample { 
  
public:

  TauToolCompiledARAExample();
  virtual ~TauToolCompiledARAExample();
  
  /** Get the name of the branches from Transient tree and book histograms */
  void initialize(TTree *tt_in);
  /** Print the summary in a logfile and save histograms in a root file */
  void finalize();
  /** This is the main method which the user needs to invoke */
  void execute();

private:

  ////////////////////////////////////////////////////////////////////////
  // 0: Instantiate TauExamples class
  ////////////////////////////////////////////////////////////////////////
  /** instance of the tauexamples class which will be used */
  TauExamples tauexample;

  /** Pointer to the truth tau container. */
  const TruthParticleContainer *mcpartTES;

  /** Pointer to the reconstructed tau container. */
  const Analysis::TauJetContainer *taurecTES;

  /** Branch acces to the truth tau container. */
  TBranch *br_mcpart;

  /** Branch access to the reconstructed tau container. */
  TBranch *br_taurec;

  /** TTree for branch access. */
  TTree *tt;
  
};

#endif // TAUARAEXAMPLE_H
