/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MISSINGETEVENT_MissingEtTruth_H
#define MISSINGETEVENT_MissingEtTruth_H
/********************************************************************

NAME:     MissingEtTruth.cxx
PACKAGE:  offline/Reconstruction/MissingETEvent

AUTHORS:  P. Loch, S. Resconi
CREATED:  Oct 2004

PURPOSE:   
********************************************************************/
#include "CLIDSvc/CLASS_DEF.h"
#include "MissingETEvent/MissingET.h"
#include "MissingETEvent/MissingEtRegions.h"

#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"

#include <vector>


class MissingEtTruth : public MissingET
{
 public:

  enum TruthIndex  
   { 
     Int        = 0,  // all interacting particles till abs(eta) <= 5
     NonInt     = 1,  // non interacting particles
     IntCentral = 2,  // interacting particles in central region : 
                      // till eta of the full sim
     IntFwd     = 3,  // interacting particles in Forward region :
                      // with eta_full_sim < abs(eta) <= 5 
     IntOutCover= 4,  // interacting particles with abs(eta) > 5
     Muons      = 5,  // truth muons ( all )       	      
     Size       = 6,          	      
     NotValid   = 999 
   };  	      
  
  //contructors
  MissingEtTruth();
////  MissingEtTruth(MissingET::Source aSource);
  MissingEtTruth(MissingEtRegions* aRegion);
  MissingEtTruth(MissingET::Source aSource, MissingEtRegions* aRegion);

  virtual ~MissingEtTruth();

  // add a cell
  void addPart(const HepMC::GenParticle* aPart, double etaFull);

  // set Truth contributions
  void setExTruth(TruthIndex aTruth,  double theEx);
  void setEyTruth(TruthIndex aTruth,  double theEy);
  void setEtSumTruth(TruthIndex aTruth, double theEtSum);
  
  // retrieve Truth contributions 
  double exTruth(TruthIndex aTruth) const;
  double eyTruth(TruthIndex aTruth) const;
  double etSumTruth(TruthIndex aTruth) const;
  

 protected:

  std::vector<double> m_exTruth;
  std::vector<double> m_eyTruth;
  std::vector<double> m_etSumTruth;
  
  void setup(); 
 
};

CLASS_DEF(MissingEtTruth, 73891893, 1)

#endif
