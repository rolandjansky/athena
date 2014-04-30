/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCSSCController_hh
#define TGCSSCController_hh

#include "TrigT1TGC/TGCHighPtChipOut.hh"
#include "TrigT1TGC/TGCSSCControllerOut.hh"

namespace LVL1TGCTrigger {
class TGCSectorLogic;

// const int NumberOfWireHighPtBoard = 2;

class TGCSSCController {
public:
  TGCSSCController( const TGCSectorLogic* sL=0);  
  virtual ~TGCSSCController(){} 

  TGCSSCControllerOut* distribute(TGCHighPtChipOut* wire[], TGCHighPtChipOut* strip);
  void setNumberOfWireHighPtBoard(int iData){NumberOfWireHighPtBoard = iData;};
  int getNumberOfWireHighPtBoard(){return NumberOfWireHighPtBoard;};
  void setRegion(TGCRegionType regionIn){ region=regionIn;};

private:
  const TGCSectorLogic* sectorLogic;
  int getSSCId(int nHPB, int chip, int block) const;
  int convertPhi(int chip, int block, int pos) const;
  int getChamberNo(int chip, int block) const;
  int getPhiPosInSSC(int chip, int block) const;
  int NumberOfWireHighPtBoard;
  TGCRegionType region;
};

} //end of namespace bracket

#endif
