/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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

  int m_priorSign;    // prior sign of dr or dphi in coincidence matrix.
  int m_numberOfData; // number of data  
  int m_nChInBlock;   // number of channel in pivot plane
  int m_maxDev;       // maximum of absolute value of dr,dphi
  int m_posMaxDev;    // maximum of positive value of dr,dphi
  int m_negMaxDev;    // maximum of negative value of dr,dphi
  int m_nChAdj;       // number of channel from adjacent PatchPanel
  int m_iChBase;      //
  int m_MatrixOverlap;
  TGCSignalType m_SType;

};


} //end of namespace bracket

#endif // TGCDoubletSB_hh

