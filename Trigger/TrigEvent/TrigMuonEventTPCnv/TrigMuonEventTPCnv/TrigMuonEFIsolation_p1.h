// dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEVENT_TRIGMUONEFISOLATION_P1_H
#define TRIGMUONEVENT_TRIGMUONEFISOLATION_P1_H
/**
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @class  : TrigMuonEFIsolation_p1
 *
 * @brief Persistent partner class for TrigMuonEFIsolation
 *
 * @author Mark Owen <markowen@cern.ch>
 */

class TrigMuonEFIsolation_p1 {
  friend class TrigMuonEFIsolationCnv_p1;
 public:

  TrigMuonEFIsolation_p1() : allTheFloats(), m_trkPos(0) {}
  
  virtual ~TrigMuonEFIsolation_p1() {}

  //private:

  // float m_sumTrkPtCone02;
  // float m_sumTrkPtCone03;
  float allTheFloats[2]; //! array to hold the floats

  int m_trkPos; //! position of EF muon track within the info object

};//class TrigMuonEFIsolation_p1

#endif // TRIGMUONEVENT_TRIGMUONEFISOLATION_P1_H
