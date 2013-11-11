///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// File: tauEventTPCnv/tauEventTPCnvDict.h
/// Dictionary file for persistent representation(s) of AOD containers
/// Author: Blake Burghgrave <blake.burghgrave AT cern.ch>
/// Date:   June 2013

#ifndef tauEventTPCnv_tauEventTPCnvCNVDICT_H
#define tauEventTPCnv_tauEventTPCnvCNVDICT_H

#include "tauEventTPCnv/TauJet_p1.h"
#include "tauEventTPCnv/TauJet_p2.h"
#include "tauEventTPCnv/TauJet_p3.h"
#include "tauEventTPCnv/TauJet_p4.h"
#include "tauEventTPCnv/TauJet_p5.h"
#include "tauEventTPCnv/TauJetContainer_p1.h"
#include "tauEventTPCnv/TauJetContainer_p2.h"
#include "tauEventTPCnv/TauJetContainer_p3.h"
#include "tauEventTPCnv/TauJetContainer_p4.h"
#include "tauEventTPCnv/TauJetContainer_p5.h"
#include "tauEventTPCnv/TauDetailsContainer_tlp1.h"
#include "tauEventTPCnv/TauDetailsContainer_tlp2.h"
#include "tauEventTPCnv/TauDetailsContainer_tlp3.h"
#include "tauEventTPCnv/TauDetailsContainer_tlp4.h"
#include "tauEventTPCnv/TauCommonDetails_p1.h"
#include "tauEventTPCnv/TauCommonDetails_p2.h"
#include "tauEventTPCnv/TauCommonExtraDetails_p1.h"
#include "tauEventTPCnv/PanTauDetails_p1.h"
#include "tauEventTPCnv/TauPi0Details_p1.h"
#include "tauEventTPCnv/TauPi0Details_p2.h"
#include "tauEventTPCnv/TauPi0Candidate_p1.h"
#include "tauEventTPCnv/TauPi0Cluster_p1.h"
#include "tauEventTPCnv/TauShot_p1.h"
#include "tauEventTPCnv/TauRecDetails_p1.h"
#include "tauEventTPCnv/TauRecExtraDetails_p1.h"
#include "tauEventTPCnv/Tau1P3PDetails_p1.h"
#include "tauEventTPCnv/Tau1P3PExtraDetails_p1.h"
#include "tauEvent/TauJetContainer.h"
#include "tauEventTPCnv/TauJetContainerCnv_p1.h"
#include "tauEventTPCnv/TauJetContainerCnv_p2.h"
#include "tauEventTPCnv/TauJetContainerCnv_p3.h"
#include "tauEventTPCnv/TauJetContainerCnv_p4.h"
#include "tauEventTPCnv/TauJetContainerCnv_p5.h"
#include "tauEventTPCnv/TauJetContainer_p1_old_cnv.h"
#include "tauEventTPCnv/TauDetailsContainerCnv_tlp1.h"
#include "tauEventTPCnv/TauDetailsContainerCnv_tlp2.h"
#include "tauEventTPCnv/TauDetailsContainerCnv_tlp3.h"
#include "tauEventTPCnv/TauDetailsContainerCnv_tlp4.h"

struct dummy {
  T_TPCnv<Analysis::TauJetContainer, TauJetContainer_p1> m_taucnv_1;
  T_TPCnv<Analysis::TauJetContainer, TauJetContainer_p2> m_taucnv_2;
  T_TPCnv<Analysis::TauJetContainer, TauJetContainer_p3> m_taucnv_3;
  T_TPCnv<Analysis::TauJetContainer, TauJetContainer_p4> m_taucnv_4;
  T_TPCnv<Analysis::TauJetContainer, TauJetContainer_p5> m_taucnv_5;

  T_TPCnv<Analysis::TauDetailsContainer, TauDetailsContainer_tlp1 >  m_tauDetailsCnv;
  T_TPCnv<Analysis::TauDetailsContainer, TauDetailsContainer_tlp2 >  m_tauDetailsCnv_2;
  T_TPCnv<Analysis::TauDetailsContainer, TauDetailsContainer_tlp3 >  m_tauDetailsCnv_3;
  T_TPCnv<Analysis::TauDetailsContainer, TauDetailsContainer_tlp4 >  m_tauDetailsCnv_4;
};

#endif //tauEventTPCnv_tauEventTPCnvCNVDICT_H
