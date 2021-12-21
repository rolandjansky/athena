/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigT1TGC_TGCSlaveBoard_h
#define TrigT1TGC_TGCSlaveBoard_h

#include "TrigT1TGC/TGCSlaveBoardOut.h"
#include "TrigT1TGC/TGCNumbering.h"

#include <bitset>
#include <string>

namespace LVL1TGCTrigger {

class TGCPatchPanel;
class TGCPatchPanelOut;
class TGCSlaveBoardOut;
class TGCHitPattern;

class TGCSlaveBoard 
{
 private:
  // offten use {A,B,C,D}={36,36,44,44} format, but some parts are not used.
  static constexpr int s_pOffset[6] = {0,0,2,2,2,2};// WT,WD,ST,SD,WI,SI
  static constexpr int s_iOffset[6] = {4,0,6,6,6,6};
  static constexpr int s_pLength[6] = {36,36,32,32,32,32};
  static constexpr int s_iLength[6] = {36,44,32,32,32,32};

 public:

  TGCSlaveBoard();
  virtual ~TGCSlaveBoard(); 
  TGCSlaveBoard(const TGCSlaveBoard& right) = delete;
  TGCSlaveBoard& operator=(const TGCSlaveBoard& right) = delete;

  void clockIn(int bidIn);
  TGCSlaveBoardOut* getOutput() { return m_slaveBoardOut; }
  void eraseOutput();

  int getId() const { return m_id;};
  void setId(int idIn){ m_id=idIn;};

  void setPatchPanel(TGCPatchPanel* PPIn);

  int getIdHighPtBoard() const;
  void setIdHighPtBoard(int m_id);

  int getType() const { return m_type;};
  void setType(int typeIn){ m_type=typeIn;};

  TGCRegionType getRegion() const { return m_region;};
  void setRegion(TGCRegionType regionIn){ m_region=regionIn;};

  void showResult()  const;
  std::string getTypeName(int typeIn) const;

  void setPatchPanelOut(TGCPatchPanelOut* PPOut){ m_patchPanelOut=PPOut;};

  void storeSlbIn();
  const std::bitset<200>& GetSlbIn(void) const { return m_slbin; } // for readout

 protected:
  void collectInput();
  void doMaskOperation(){};
  void doDecluster();
  void deletePatchPanelOut();

  virtual void doCoincidence() = 0;
  virtual void createSlaveBoardOut() = 0;

  int m_lengthOfCoincidenceOut;
  TGCHitPattern* m_coincidenceOut;
  TGCSlaveBoardOut* m_slaveBoardOut;

  int m_id;
  int m_bid;
  int m_idHighPtBoard;
  int m_type;
  TGCRegionType m_region;

  TGCPatchPanel* m_patchPanel;
  TGCPatchPanelOut* m_patchPanelOut;
  std::bitset<200> m_slbin; // store SlaveBoard input for readout

};

}   // end of namespace

#endif
