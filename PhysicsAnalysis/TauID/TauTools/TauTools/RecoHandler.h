/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECO_H
#define TAURECO_H

//////////////////////////////////////////////////////////////////////////////////////
/// Name    : RecoHandler.h
/// Package : offline/PhysicsAnalysis/TauID/TauTools
/// Author  : Jyothsna Komaragiri, Dugan O'Neil
/// Created : April 2008

///
/// DESCRIPTION:
///
/// This class is a simple example for selecting good taus.
/// This class was written to be run on events reconstructed in 14.x.x
///
//////////////////////////////////////////////////////////////////////////////////////
#include "tauEvent/TauJetContainer.h"
#include "tauEvent/TauJet.h"
#include "tauEvent/TauCommonDetails.h"
#include "tauEvent/TauPID.h"

#include "CaloEvent/CaloCluster.h"
#include "egammaEvent/ElectronContainer.h"
#include "muonEvent/MuonContainer.h"

#include <string>
#include <math.h>

#include "Gtypes.h"

#define TAU_ALL  4
#define TAU_BOTH 3
#define TAU_1P3P 2
#define TAU_REC  1
#define TAU_NONE 0

class RecoHandler { 
  
 public:
  
  RecoHandler();
  virtual ~RecoHandler();
 
  ////////////////////////////////////////////////
  // Methods related to reconstructed tau Jets
  //
  /** Check the tau type: tauRec, tau1p3p or overlap candidate */
  int TauType(const Analysis::TauJet *tauJet);
  
  /** method selects the reconstructed tauRec Jet with Likelihood > cut for algo=1, TauElTauLikelihood > cut for algo=2, TauJetNeuralNetwork > cut for algo=3 and TauENeuralNetwork > cut for algo=4. */
  bool                        selectTauRecJet( const Analysis::TauJet *tauJet, int algo, float cut, bool useGoodCharge=true );
  Analysis::TauJetContainer*  selectTauRecJet( const Analysis::TauJetContainer *tauJet, int algo, float cut );

// Removed for tauEvent-00-06-00
  /** selects the reconstructed tau1P3P Jet with DiscCut > cut for algo=1, DiscNN > cut for algo=2, EfficNN > cut for algo=3, DiscPDERS > cut for algo=4 and EfficDiscPDERS for algo=5 */
  /*  
  bool                        selectTau1P3PJet( const Analysis::TauJet *tauJet, int algo, float cut, bool useGoodCharge=true, 
                                              bool useEVeto=true, bool useMuVeto=true ); 
  Analysis::TauJetContainer*  selectTau1P3PJet( const Analysis::TauJetContainer *tauJet, int algo, float cut ); 
  */
};

#endif // TAURECO_H
