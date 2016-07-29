/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ====================================================================
/*
        TGCASDOut.h

	ver.1.0
	ver.2.0   based on new numbering sheme (TGCIndex -> TGCReadoutIndex) 
	          use index in TGC readout system

                                      QCJP, 1999
*/
// ====================================================================
#ifndef TGC_ASDOUT_H
#define TGC_ASDOUT_H

//#include "TrigT1TGC/TGCIndex.h"
#include "TrigT1TGC/TGCReadoutIndex.h"

namespace LVL1TGCTrigger {

// ====================================================================
//
// class definition
//
// ====================================================================
 
class TGCASDOut {
protected:
  TGCReadoutIndex tgcReadoutIndex;
  TGCSignalType signalType;
  int hitID;       // index in a chamber
  int channel;     // index in a ASD board 
  double hitToF;

public:
  TGCASDOut();
  TGCASDOut(TGCIndex tgcindex, int ilyr, 
	    TGCSignalType sigtype=WIREGROUP, int id=-1, double tof=0.);
  TGCASDOut(TGCReadoutIndex tgcrindex, 
	    TGCSignalType sigtype=WIREGROUP, int id=-1, double tof=0.);

  virtual ~TGCASDOut() { }
 
  TGCASDOut(const TGCASDOut& right);
 
  TGCASDOut& operator=(const TGCASDOut& right);
 
  int operator==(const TGCASDOut& right) const
  {
    return (this==&right);
  }
 
  int operator!=(const TGCASDOut& right) const
  {
    return (this!=&right);
  }

  // set functons
  void SetParams(TGCSignalType signal_type, int id, double tof);
  void SetChannel(int ch);

  // get functions
  TGCReadoutIndex GetTGCReadoutIndex() const { return tgcReadoutIndex; }
  TGCSignalType GetSignalType() const { return signalType; }
  int GetHitID() const { return hitID; }
  int GetChannel() const { return channel; }
  double GetHitToF() const { return hitToF; }

  // methods  
  void Print() const;

};


} //end of namespace bracket

#endif
