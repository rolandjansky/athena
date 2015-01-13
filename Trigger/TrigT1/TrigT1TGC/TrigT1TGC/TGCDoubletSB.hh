/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCDoubletSB_hh
#define TGCDoubletSB_hh

#include "TrigT1TGC/TGCSlaveBoard.hh"
#include <iostream>

namespace LVL1TGCTrigger {

const int NumberOfDoubletSBData = 2;

class TGCPatchPanelOut;
class TGCHitPattern;

class TGCDoubletSB: public TGCSlaveBoard {
public:
  TGCDoubletSB();
  virtual ~TGCDoubletSB();

protected:
  virtual void doCoincidence();
  virtual void createSlaveBoardOut();

  TGCHitPattern** doLocalCoincidence(const TGCPatchPanelOut* PPOut);
  void do3outof4Coincedence(TGCHitPattern** LCOut);
  virtual void doCoincidenceInSubMatrix(const int block, TGCHitPattern** LCOut);
  bool element(bool A, bool B, bool C, bool D);

  int priorSign;    // prior sign of dr or dphi in coincidence matrix.
  int numberOfData; // number of data  
  int nChInBlock;   // number of channel in pivot plane
  int maxDev;       // maximum of absolute value of dr,dphi
  int posMaxDev;    // maximum of positive value of dr,dphi
  int negMaxDev;    // maximum of negative value of dr,dphi
  int nChAdj;       // number of channel from adjacent PatchPanel
  int iChBase;      //
  int MatrixOverlap;
  TGCSignalType SType;

};


} //end of namespace bracket

#endif // TGCDoubletSB_hh

