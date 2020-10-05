/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCWireHighPtBoard_hh
#define TGCWireHighPtBoard_hh

#include "TrigT1TGC/TGCHighPtBoard.h"

namespace LVL1TGCTrigger {

class TGCHitPattern;

class TGCWireHighPtBoard: public TGCHighPtBoard {
public:
  TGCWireHighPtBoard();

  virtual ~TGCWireHighPtBoard(){};

protected:
  virtual void decodeSlaveBoardOut( int chip,
				    TGCHitPattern* tHit, TGCHitPattern* dHit);
  virtual void doCoincidenceMatrix( int chip, int block,
				    const TGCHitPattern* tHit, 
				    const TGCHitPattern* dHit);
  virtual void setDecoderIn();
  virtual void clearDecoderIn();
};


} //end of namespace bracket

#endif
