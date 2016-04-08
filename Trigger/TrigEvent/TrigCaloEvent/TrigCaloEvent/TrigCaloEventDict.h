/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKCALOEVENT_H
#define TRACKCALOEVENT_H

#include "TrigCaloEvent/TrigCaloCluster.h"
#include "TrigCaloEvent/TrigEMCluster.h"
#include "TrigCaloEvent/TrigTauCluster.h"
#include "TrigCaloEvent/TrigTauClusterDetails.h"
#include "TrigCaloEvent/Trig3Momentum.h"
#include "TrigCaloEvent/TrigT2Jet.h"
//#include "TrigCaloEvent/TrigMissingET.h"
//#include "TrigCaloEvent/TrigMissingETContainer.h"
#include "TrigCaloEvent/TrigCaloClusterContainer.h"
#include "TrigCaloEvent/TrigEMClusterContainer.h"
#include "TrigCaloEvent/TrigTauClusterContainer.h"
#include "TrigCaloEvent/TrigTauClusterDetailsContainer.h"
#include "TrigCaloEvent/TrigT2JetContainer.h"
#include "TrigCaloEvent/TrigT2MbtsBits.h"
#include "TrigCaloEvent/TrigT2MbtsBitsContainer.h"
#include "TrigCaloEvent/TrigT2ZdcSignals.h"
#include "TrigCaloEvent/TrigT2ZdcSignalsContainer.h"
#include "TrigCaloEvent/RingerRings.h"
#include "TrigCaloEvent/RingerRingsContainer.h"
#include "TrigCaloEvent/TrigRNNOutput.h"
#include "TrigCaloEvent/TrigRNNOutputContainer.h"

// These packages are referenced only by generated sercnv code, which leads
// to spurious checkreq warnings.  Add references here to get rid of those.
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODTrigMinBias/TrigT2MbtsBitsContainer.h"


void dummy_function_for_TrigCaloEvent_which_forces_dictionaries_generation ( /*
									     DataVector<TrigMissingET> a,
									     DataVector<TrigMissingETContainer> b,*/	     
									     DataVector<TrigCaloClusterContainer> c1,
									     DataVector<TrigEMClusterContainer> c,
									     DataVector<TrigTauClusterContainer> d,
                                                                             DataVector<TrigTauClusterDetailsContainer> d2,
									     DataVector<TrigT2JetContainer> e,
                                                                             DataVector<TrigT2MbtsBitsContainer> h,
									     DataVector<TrigT2ZdcSignalsContainer> o,
                                                                             DataVector<RingerRings> f,
									     DataVector<RingerRingsContainer> g,
                                                                             DataVector<TrigRNNOutput> i,
                                                                             DataVector<TrigRNNOutputContainer> j
									     ) {
  /*  DataVector<TrigMissingET> aa = a;  
      DataVector<TrigMissingETContainer> bb = b;*/
  DataVector<TrigEMClusterContainer> cc = c;
  DataVector<TrigCaloClusterContainer> cc1 = c1;
  DataVector<TrigTauClusterContainer> dd = d;
  DataVector<TrigTauClusterDetailsContainer> dd2 = d2; 
  DataVector<TrigT2JetContainer> ee = e;
  DataVector<TrigT2MbtsBitsContainer> hh = h;
  DataVector<TrigT2ZdcSignalsContainer> oo = o;
  DataVector<RingerRings> ff = f;
  DataVector<RingerRingsContainer> gg = g;
  DataVector<TrigRNNOutput> ii = i;
  DataVector<TrigRNNOutputContainer> jj = j;

  std::vector<const TrigEMClusterContainer *> ccc; ccc.size();
  std::vector<const TrigCaloClusterContainer *> ccc1; ccc1.size();
  std::vector<const TrigTauClusterContainer *> ddd; ddd.size(); 
  std::vector<const TrigTauClusterDetailsContainer *> ddd2; ddd2.size();
  std::vector<const TrigT2JetContainer *> eee; eee.size();
  std::vector<const TrigT2MbtsBitsContainer *> hhh; hhh.size();
  std::vector<const TrigT2ZdcSignalsContainer *> ooo; ooo.size();
  std::vector<const RingerRings *> fff; fff.size();
  std::vector<const RingerRingsContainer *> ggg; ggg.size();
  std::vector<const TrigRNNOutput *> iii; iii.size();
  std::vector<const TrigRNNOutputContainer *> jjj; jjj.size();
  std::vector<const TrigCaloCluster*> kkk1; kkk1.size();
  std::vector<const TrigEMCluster*> kkk; kkk.size();
  std::vector<const TrigTauCluster*> lll; lll.size();
  std::vector<const TrigTauClusterDetails*> mmm; mmm.size();
  std::vector<const TrigT2Jet*> nnn; nnn.size();
}
#endif
