/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCWireHighPtBoard_hh
#define TGCWireHighPtBoard_hh

#include "TrigT1TGC/TGCHighPtBoard.hh"
#include "TrigT1TGC/TGCHighPtBoardOut.hh"
#include "TrigT1TGC/TGCDoubletSB.hh"
#include "TrigT1TGC/TGCWireTripletSB.hh"
#include "TrigT1TGC/TGCSlaveBoardOut.hh"

namespace LVL1TGCTrigger {

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
