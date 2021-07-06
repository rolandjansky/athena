/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCALOEVENTTPCNV_TRIGCALOEVENTTPCNVDICT_H
#define TRIGCALOEVENTTPCNV_TRIGCALOEVENTTPCNVDICT_H

#include "TrigCaloEventTPCnv/TrigTauClusterContainer_tlp1.h"
#include "TrigCaloEventTPCnv/TrigTauClusterContainer_p1.h"
#include "TrigCaloEventTPCnv/TrigTauCluster_tlp1.h"
#include "TrigCaloEventTPCnv/TrigTauCluster_p1.h"

#include "TrigCaloEventTPCnv/TrigTauCluster_tlp2.h"
#include "TrigCaloEventTPCnv/TrigTauCluster_p2.h"

#include "TrigCaloEventTPCnv/TrigTauClusterContainer_p3.h"
#include "TrigCaloEventTPCnv/TrigTauCluster_p3.h"

#include "TrigCaloEventTPCnv/TrigTauClusterContainer_p4.h"
#include "TrigCaloEventTPCnv/TrigTauCluster_p4.h"

#include "TrigCaloEventTPCnv/TrigTauClusterContainer_p5.h"
#include "TrigCaloEventTPCnv/TrigTauCluster_p5.h"

#include "TrigCaloEventTPCnv/TrigTauClusterDetailsContainer_tlp1.h"
#include "TrigCaloEventTPCnv/TrigTauClusterDetailsContainer_p1.h"
#include "TrigCaloEventTPCnv/TrigTauClusterDetails_tlp1.h"
#include "TrigCaloEventTPCnv/TrigTauClusterDetails_p1.h"
#include "TrigCaloEventTPCnv/TrigTauClusterDetails_p2.h"

#include "TrigCaloEventTPCnv/TrigTauClusterDetailsContainer_p2.h"

#include "TrigCaloEventTPCnv/TrigT2MbtsBits_p3.h"
#include "TrigCaloEventTPCnv/TrigT2MbtsBits_p2.h"
#include "TrigCaloEventTPCnv/TrigT2MbtsBits_p1.h"
#include "TrigCaloEventTPCnv/TrigT2MbtsBitsContainer_p1.h"
#include "TrigCaloEventTPCnv/TrigT2MbtsBitsContainer_p3.h"
#include "TrigCaloEventTPCnv/TrigT2MbtsBitsContainer_tlp1.h" 

#include "TrigCaloEventTPCnv/TrigT2Jet_p3.h"
#include "TrigCaloEventTPCnv/TrigT2JetContainer_p3.h"
#include "TrigCaloEventTPCnv/TrigT2Jet_tlp1.h"
#include "TrigCaloEventTPCnv/TrigT2JetContainer_tlp1.h"
#include "TrigCaloEventTPCnv/TrigT2JetContainer_p1.h"
#include "TrigCaloEventTPCnv/TrigT2Jet_p1.h"
#include "TrigCaloEventTPCnv/TrigT2Jet_p2.h"

#include "TrigCaloEventTPCnv/RingerRingsContainer_tlp1.h"
#include "TrigCaloEventTPCnv/RingerRingsContainer_p1.h"
#include "TrigCaloEventTPCnv/RingerRings_tlp1.h"
#include "TrigCaloEventTPCnv/RingerRings_p1.h"
#include "TrigCaloEventTPCnv/RingerRingsContainer_p2.h"
#include "TrigCaloEventTPCnv/RingerRings_p2.h"

#include "TrigCaloEventTPCnv/TrigEMClusterContainer_tlp1.h"
#include "TrigCaloEventTPCnv/TrigEMClusterContainer_p1.h"
#include "TrigCaloEventTPCnv/TrigEMCluster_tlp1.h"
#include "TrigCaloEventTPCnv/TrigEMCluster_tlp2.h"
#include "TrigCaloEventTPCnv/TrigEMCluster_p1.h"
#include "TrigCaloEventTPCnv/TrigEMCluster_p2.h"
#include "TrigCaloEventTPCnv/TrigCaloCluster_p1.h"
#include "TrigCaloEventTPCnv/TrigCaloCluster_p2.h"

#include "TrigCaloEventTPCnv/TrigCaloClusterContainer_p3.h"
#include "TrigCaloEventTPCnv/TrigCaloCluster_p3.h"
#include "TrigCaloEventTPCnv/TrigT2ZdcSignals_p1.h"
#include "TrigCaloEventTPCnv/TrigT2ZdcSignalsContainer_p1.h"

#include "TrigCaloEventTPCnv/TrigEMClusterContainer_p3.h"
#include "TrigCaloEventTPCnv/TrigEMCluster_p3.h"

#include "TrigCaloEventTPCnv/TrigEMClusterContainer_p4.h"
#include "TrigCaloEventTPCnv/TrigEMCluster_p4.h"

#include "TrigCaloEventTPCnv/TrigRNNOutputContainer_tlp1.h"
#include "TrigCaloEventTPCnv/TrigRNNOutputContainer_p1.h"
#include "TrigCaloEventTPCnv/TrigRNNOutput_tlp1.h"
#include "TrigCaloEventTPCnv/TrigRNNOutput_p1.h"
#include "TrigCaloEventTPCnv/TrigRNNOutputContainer_p2.h"
#include "TrigCaloEventTPCnv/TrigRNNOutput_p2.h"

//dummy instances of all objects using std::vector<TPObjRef> for the dictionary
struct dummy_TrigCaloEventTPCnvDict
{

  std::vector<TrigTauCluster_p1>              m_dummyTrigTauCluster_p1;
  std::vector<TrigTauCluster_p2>              m_dummyTrigTauCluster_p2;
  std::vector<TrigTauCluster_p3>              m_dummyTrigTauCluster_p3;
  std::vector<TrigTauCluster_p4>              m_dummyTrigTauCluster_p4;
  std::vector<TrigTauCluster_p5>              m_dummyTrigTauCluster_p5;
  std::vector<TrigTauClusterDetails_p1>       m_dummyTrigTauClusterDetails_p1;
  std::vector<TrigTauClusterDetails_p2>       m_dummyTrigTauClusterDetails_p2;
  std::vector<TrigT2MbtsBits_p1>              m_dummyT2MbtsBits_p1;
  std::vector<TrigT2MbtsBits_p2>              m_dummyT2MbtsBits_p2;
  std::vector<TrigT2MbtsBits_p3>              m_dummyT2MbtsBits_p3;
  std::vector<TrigT2Jet_p1>                   m_dummyT2Jet_p1;
  std::vector<TrigT2Jet_p2>                   m_dummyT2Jet_p2;
  std::vector<TrigT2Jet_p3>                   m_dummyT2Jet_p3;
  std::vector<TrigEMCluster_p1>               m_dummyEMCluster_p1;
  std::vector<TrigEMCluster_p2>               m_dummyEMCluster_p2;
  std::vector<TrigEMCluster_p3>               m_dummyEMCluster_p3;
  std::vector<TrigEMCluster_p4>               m_dummyEMCluster_p4;
  std::vector<RingerRings_p1>                 m_dummyRingerRings_p1;
  std::vector<RingerRings_p2>                 m_dummyRingerRings_p2;
  std::vector<TrigCaloCluster_p1>             m_dummyCaloCluster_p1;
  std::vector<TrigCaloCluster_p2>             m_dummyCaloCluster_p2;
  std::vector<TrigCaloCluster_p3>             m_dummyCaloCluster_p3;
  std::vector<TrigT2ZdcSignals_p1>            m_dummyT2ZdcSignals_p1;
  std::vector<TrigRNNOutput_p1> 	      m_dummyRNNOutput_p1;
  std::vector<TrigRNNOutput_p2> 	      m_dummyRNNOutput_p2;

};

#endif
