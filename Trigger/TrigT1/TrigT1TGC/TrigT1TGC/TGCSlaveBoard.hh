/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCSlaveBoard_hh
#define TGCSlaveBoard_hh

#include "TrigT1TGC/TGCSlaveBoardOut.hh"
#include "TrigT1TGC/TGCPatchPanel.hh"
#include "TrigT1TGC/TGCPatchPanelOut.hh"
#include "TrigT1TGC/TGCHitPattern.hh"
#include "TrigT1TGC/TGCConnectionSBToHPB.hh"
#include <limits.h>
#include <bitset>
#include <iostream>
#include <fstream>
#include <string>

namespace LVL1TGCTrigger {

class TGCSlaveBoardOut;

// offten use {A,B,C,D}={36,36,44,44} format, but some parts are not used.
const int pOffset[6] = {0,0,2,2,2,2};// WT,WD,ST,SD,WI,SI
const int iOffset[6] = {4,0,6,6,6,6};
const int pLength[6] = {36,36,32,32,32,32};
const int iLength[6] = {36,44,32,32,32,32};
class TGCSlaveBoard {

public:

  TGCSlaveBoard(); 
  virtual ~TGCSlaveBoard(); 
  TGCSlaveBoard(const TGCSlaveBoard& right); 
  TGCSlaveBoard& operator=(const TGCSlaveBoard& right); 

  void clockIn(int bidIn);
  TGCSlaveBoardOut* getOutput() const;
  void eraseOutput();

  int getId() const { return id;};
  void setId(int idIn){ id=idIn;};

  void setPatchPanel(TGCPatchPanel* PPIn);

  int getIdHighPtBoard() const;
  void setIdHighPtBoard(int id);

  int getType() const { return type;};
  void setType(int typeIn){ type=typeIn;};

  TGCRegionType getRegion() const { return region;};
  void setRegion(TGCRegionType regionIn){ region=regionIn;};

  void showResult()  const;
  std::string getTypeName(int typeIn) const;

  void setPatchPanelOut(TGCPatchPanelOut* PPOut){ patchPanelOut=PPOut;};

  void storeSlbIn();
  const std::bitset<200>& GetSlbIn(void) const { return slbin; } // for readout

protected:
  void collectInput();
  void doMaskOperation(){};
  void doDecluster();
  void deletePatchPanelOut();

  virtual void doCoincidence() = 0;
  virtual void createSlaveBoardOut() = 0;

  int lengthOfCoincidenceOut;
  TGCHitPattern* coincidenceOut;
  TGCSlaveBoardOut* slaveBoardOut;

  int id;
  int bid;
  int idHighPtBoard;
  int type;
  TGCRegionType region;

  TGCPatchPanel* patchPanel;
  TGCPatchPanelOut* patchPanelOut;
  std::bitset<200> slbin; // store SlaveBoard input for readout
};

} //end of namespace bracket

#endif // TGCSlaveBoard_hh
