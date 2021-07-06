/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigT1TGC_TGCASDOut_H
#define TrigT1TGC_TGCASDOut_H

#include "TrigT1TGC/TGCReadoutIndex.h"

namespace LVL1TGCTrigger {

class TGCASDOut {
 protected:
  TGCReadoutIndex m_tgcReadoutIndex;
  TGCSignalType m_signalType{WIRE};
  int m_hitID{0};       // index in a chamber
  int m_channel{0};     // index in a ASD board
  double m_hitToF{0};

 public:
  TGCASDOut() = default;
  TGCASDOut(TGCIndex tgcindex, int ilyr, 
	    TGCSignalType sigtype=WIREGROUP, int id=-1, double tof=0.);
  TGCASDOut(TGCReadoutIndex tgcrindex, 
	    TGCSignalType sigtype=WIREGROUP, int id=-1, double tof=0.);

  int operator == (const TGCASDOut& right) const { return (this==&right); }
  int operator != (const TGCASDOut& right) const { return (this!=&right); }

  // set functons
  void SetParams(TGCSignalType signal_type, int id, double tof);
  void SetChannel(int ch);

  // get functions
  TGCReadoutIndex GetTGCReadoutIndex() const { return m_tgcReadoutIndex; }
  TGCSignalType GetSignalType() const { return m_signalType; }
  int GetHitID() const { return m_hitID; }
  int GetChannel() const { return m_channel; }
  double GetHitToF() const { return m_hitToF; }

  // methods  
  void Print() const;

};


} //end of namespace bracket

#endif
