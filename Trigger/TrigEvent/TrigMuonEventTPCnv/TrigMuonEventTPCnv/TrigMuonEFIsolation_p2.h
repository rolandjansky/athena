// dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEVENT_TRIGMUONEFISOLATION_P2_H__
#define TRIGMUONEVENT_TRIGMUONEFISOLATION_P2_H__
/**
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @class  : TrigMuonEFIsolation_p2
 *
 * @brief Persistent partner class for TrigMuonEFIsolation
 *
 * @author Mark Owen <markowen@cern.ch>
 */

#include "DataModelAthenaPool/ElementLink_p3.h"

class TrigMuonEFIsolation_p2 {
  friend class TrigMuonEFIsolationCnv_p2;
 public:

  TrigMuonEFIsolation_p2();
  
  virtual ~TrigMuonEFIsolation_p2() {}

  //private:

  // float m_sumTrkPtCone02;
  // float m_sumTrkPtCone03;
  // float m_sumEtCone01;
  // float m_sumEtCone02;
  // float m_sumEtCone03;
  // float m_sumEtCone04;
  float allTheFloats[6]; //! array to hold the floats

  int m_trkPos; //! position of EF muon track within the info object

  ElementLinkInt_p3 m_muonEFLink; //! ElementLink to the Info object

};//class TrigMuonEFIsolation_p2

#endif // TRIGMUONEVENT_TRIGMUONEFISOLATION_P2_H__
