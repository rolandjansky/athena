/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCStripHighPtBoard_hh
#define TGCStripHighPtBoard_hh

#include "TrigT1TGC/TGCHighPtBoard.h"
#include "TrigT1TGC/TGCDoubletSB.h"
#include "TrigT1TGC/TGCHighPtBoardOut.h"
#include "TrigT1TGC/TGCHighPtChipOut.h"
#include "TrigT1TGC/TGCStripTripletSB.h"
#include "TrigT1TGC/TGCSlaveBoardOut.h"

namespace LVL1TGCTrigger {

class TGCStripHighPtBoard: public TGCHighPtBoard {
public:
  TGCStripHighPtBoard();

  virtual ~TGCStripHighPtBoard(){};

protected:
  virtual void highLowSelector(int chip);

  virtual void decodeSlaveBoardOut( int chip,
				    TGCHitPattern* tHit, 
				    TGCHitPattern* dHit);
  virtual void doCoincidenceMatrix( int chip, int block,
				    const TGCHitPattern* tHit, 
				    const TGCHitPattern* dHit);
  virtual void setDecoderIn();
  virtual void clearDecoderIn();
 
  TGCSlaveBoardOut* divideTSBOut(int iSet, TGCSlaveBoardOut* SBOut);

  int getNonLinearDelta(int dev) const;

};


} //end of namespace bracket

#endif
