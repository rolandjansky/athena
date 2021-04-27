/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigT1TGC_TGCSSCController_h
#define TrigT1TGC_TGCSSCController_h

#include "TrigT1TGC/TGCHighPtChipOut.h"
#include "TrigT1TGC/TGCSSCControllerOut.h"
#include "TrigT1TGC/TGCArguments.h"

namespace LVL1TGCTrigger {

class TGCSectorLogic;

class TGCSSCController
{
 public:
  TGCSSCController( const TGCArguments*, const TGCSectorLogic* sL=0 );
  virtual ~TGCSSCController(){} 

  TGCSSCControllerOut* distribute(TGCHighPtChipOut* wire[], TGCHighPtChipOut* strip);
  void setNumberOfWireHighPtBoard(int iData){m_NumberOfWireHighPtBoard = iData;};
  int getNumberOfWireHighPtBoard(){return m_NumberOfWireHighPtBoard;};
  void setRegion(TGCRegionType regionIn){ m_region=regionIn;};

  const TGCArguments* tgcArgs() const { return m_tgcArgs; }

 private:
  const TGCSectorLogic* m_sectorLogic;
  int getSSCId(int nHPB, int chip, int block) const;
  int convertPhi(int chip, int block, int pos) const;
  int getChamberNo(int chip, int block) const;
  int getPhiPosInSSC(int chip, int block) const;
  int m_NumberOfWireHighPtBoard;
  TGCRegionType m_region;
  const TGCArguments* m_tgcArgs;
};

}   // end of namespace

#endif
