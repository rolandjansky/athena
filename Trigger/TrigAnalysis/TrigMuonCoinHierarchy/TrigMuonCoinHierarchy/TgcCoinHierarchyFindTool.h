/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGER_TGCCOINHIERARCHYFINDTOOL_H 
#define TRIGGER_TGCCOINHIERARCHYFINDTOOL_H 

#include "TrigMuonCoinHierarchy/ITgcCoinHierarchyFindTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MuonRDO/TgcRdo.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

#include <fstream>

class ITGCcablingSvc;

namespace Trigger {

class TgcCoinHierarchyFindTool : virtual public ITgcCoinHierarchyFindTool, virtual public AthAlgTool {
 public:
  /** Constructor */
  TgcCoinHierarchyFindTool(const std::string& t, const std::string& n, const IInterface* p);

  /** Destructor */
  virtual ~TgcCoinHierarchyFindTool()=default;

  /** Query inteface */
  StatusCode queryInterface(const InterfaceID& riid, void** ppvIf);

  /** Provide InterfaceID */
  static const InterfaceID& interfaceID() { return ITgcCoinHierarchyFindTool::interfaceID(); }

  /** Initialize */
  virtual StatusCode initialize();

  /** Find TgcCoinHierarchy's from a TgcCoinHierarchyTriggerSector */
  virtual StatusCode find(TgcCoinHierarchyTriggerSector* pTrigSectorComb, 
                          std::vector<TgcCoinHierarchy*>* pHierarchies);

 private:
  /** SLB bit position
      /code
      Large R   <--> Small R
      Large phi <--> Small phi for A side forward chambers and C side backward chambers
      Small phi <--> Large phi for A side backward chambers and C side forward chambers
      A-input :  40 -  75,  42 -  73 are valid.
      B-input :  76 - 111,  78 - 109 are valid.
      C-input : 112 - 155, 118 - 149 are valid.
      D-input : 156 - 199, 162 - 193 are valid.
      
      CHannel in this code : Large R                  15  14  13  12  11 ...   0  15 ...   3   2   1   0                 Small R
      (it is better to be reverted to avoid confusion)
      ASD channel order    :  15 ... ...   0 (there are shifts dependent on position)
      PS board channel     :                           0   1   2   3   4 ...  15  16 ...  28  29  30  31
      A-Input              :                  40  41  42  43  44  45  46 ...  57  58 ...  70  71  72  73  74  75
      B-Input              :                  76  77  78  79  80  81  82 ...  93  94 ... 106 107 108 109 110 111
      C-Input              : 112 113 114 115 116 117 118 119 120 121 122 ... 133 134 ... 146 147 148 149 150 151 152 153 154 155
      D-Input              : 156 157 158 159 160 161 162 163 164 165 166 ... 177 178 ... 190 191 192 193 194 195 196 197 198 199
      /endcode
      - https://twiki.cern.ch/twiki/bin/view/Main/TgcDocument
      - https://twiki.cern.ch/twiki/pub/Main/TgcDocument/celladdress2_asic_rev2.pdf
      - https://twiki.cern.ch/twiki/pub/Main/TgcDocument/psboard_table_v070119.xls
      - https://twiki.cern.ch/twiki/pub/Main/TgcDocument/psboard_table_v070119.pdf
  */
  enum BIT_POS { 
    BIT_POS_ASD_SIZE = 16, 
    BIT_POS_NUM_ASD = 2, 
    BIT_POS_INPUT_SIZE = BIT_POS_ASD_SIZE*BIT_POS_NUM_ASD, 
    BIT_POS_OFFSET_LARGE_R = BIT_POS_ASD_SIZE, 
    BIT_POS_OFFSET_LARGE_PHIFOR_A_FWD_C_BWD = BIT_POS_OFFSET_LARGE_R, 
    BIT_POS_A_INPUT_ORIGIN =  73, 
    BIT_POS_B_INPUT_ORIGIN = 109, 
    BIT_POS_C_INPUT_ORIGIN = 149, 
    BIT_POS_D_INPUT_ORIGIN = 193, 
    
    /* CHannel starts from 00 at the small R side (the same as ASD). */ 
    BIT_POS_B_INPUT_LARGE_R_CH15 = BIT_POS_B_INPUT_ORIGIN - BIT_POS_OFFSET_LARGE_R - 15, //  78 
    BIT_POS_A_INPUT_LARGE_R_CH08 = BIT_POS_A_INPUT_ORIGIN - BIT_POS_OFFSET_LARGE_R -  8, //  49 
    BIT_POS_B_INPUT_LARGE_R_CH07 = BIT_POS_B_INPUT_ORIGIN - BIT_POS_OFFSET_LARGE_R -  7, //  86 
    BIT_POS_A_INPUT_LARGE_R_CH00 = BIT_POS_A_INPUT_ORIGIN - BIT_POS_OFFSET_LARGE_R,      //  57 
    BIT_POS_B_INPUT_SMALL_R_CH15 = BIT_POS_B_INPUT_ORIGIN                          - 15, //  94 
    BIT_POS_A_INPUT_SMALL_R_CH08 = BIT_POS_A_INPUT_ORIGIN                          -  8, //  65 
    BIT_POS_B_INPUT_SMALL_R_CH07 = BIT_POS_B_INPUT_ORIGIN                          -  7, // 102 
    BIT_POS_A_INPUT_SMALL_R_CH00 = BIT_POS_A_INPUT_ORIGIN,                               //  73 
    
    BIT_POS_B_INPUT_LARGE_R_CH05 = BIT_POS_B_INPUT_ORIGIN - BIT_POS_OFFSET_LARGE_R -  5, //  88 
    BIT_POS_B_INPUT_SMALL_R_CH05 = BIT_POS_B_INPUT_ORIGIN                          -  5, // 104 
    BIT_POS_A_INPUT_LARGE_R_CH03 = BIT_POS_A_INPUT_ORIGIN - BIT_POS_OFFSET_LARGE_R -  3, //  54 
    BIT_POS_B_INPUT_SMALL_R_CH12 = BIT_POS_B_INPUT_ORIGIN                          - 12, //  97 
    
    BIT_POS_A_INPUT_LARGE_R_CH04 = BIT_POS_A_INPUT_ORIGIN - BIT_POS_OFFSET_LARGE_R -  4, //  53 
    BIT_POS_A_INPUT_SMALL_R_CH12 = BIT_POS_A_INPUT_ORIGIN                          - 12, //  61 
    BIT_POS_A_INPUT_SMALL_R_CH04 = BIT_POS_A_INPUT_ORIGIN                          -  4, //  69 
    BIT_POS_A_INPUT_LARGE_R_CH12 = BIT_POS_A_INPUT_ORIGIN - BIT_POS_OFFSET_LARGE_R - 12, //  45 
    
    /* CHannel starts from 00  
       at the small phi side for A side forward chambers and C side backward chambers and  
       at the large phi side for A side backward chambers and C side forward chambers (the same as ASD). */ 
    BIT_POS_B_INPUT_LARGE_PHI_FOR_A_FWD_C_BWD_CH15 = BIT_POS_B_INPUT_LARGE_R_CH15,   //  78 
    BIT_POS_A_INPUT_LARGE_PHI_FOR_A_FWD_C_BWD_CH08 = BIT_POS_A_INPUT_LARGE_R_CH08,   //  49 
    BIT_POS_B_INPUT_LARGE_PHI_FOR_A_FWD_C_BWD_CH07 = BIT_POS_B_INPUT_LARGE_R_CH07,   //  86 
    BIT_POS_A_INPUT_LARGE_PHI_FOR_A_FWD_C_BWD_CH00 = BIT_POS_A_INPUT_LARGE_R_CH00,   //  57 
    BIT_POS_B_INPUT_SMALL_PHI_FOR_A_FWD_C_BWD_CH15 = BIT_POS_B_INPUT_SMALL_R_CH15,   //  94 
    BIT_POS_A_INPUT_SMALL_PHI_FOR_A_FWD_C_BWD_CH08 = BIT_POS_A_INPUT_SMALL_R_CH08,   //  65 
    BIT_POS_B_INPUT_SMALL_PHI_FOR_A_FWD_C_BWD_CH07 = BIT_POS_B_INPUT_SMALL_R_CH07,   // 102 
    BIT_POS_A_INPUT_SMALL_PHI_FOR_A_FWD_C_BWD_CH00 = BIT_POS_A_INPUT_SMALL_R_CH00,   //  73 
    
    BIT_POS_A_INPUT_LARGE_PHI_FOR_A_FWD_C_BWD_CH12 = BIT_POS_A_INPUT_LARGE_R_CH12,   //  45 
    BIT_POS_A_INPUT_LARGE_PHI_FOR_A_FWD_C_BWD_CH04 = BIT_POS_A_INPUT_LARGE_R_CH04,   //  53 
    BIT_POS_A_INPUT_SMALL_PHI_FOR_A_FWD_C_BWD_CH12 = BIT_POS_A_INPUT_SMALL_R_CH12,   //  61 
    BIT_POS_A_INPUT_SMALL_PHI_FOR_A_FWD_C_BWD_CH04 = BIT_POS_A_INPUT_SMALL_R_CH04    //  69 
  }; 
  
  /** Bit map sizes */
  enum MAP_SIZE { 
    WT_MAP_SIZE = 3*BIT_POS_INPUT_SIZE, 
    ST_MAP_SIZE = 2*BIT_POS_INPUT_SIZE, 
    SD_MAP_SIZE = 2*BIT_POS_INPUT_SIZE, 
    WD_MAP_SIZE = 2*BIT_POS_INPUT_SIZE 
  }; 
  
  /** The numbers of chambers for TGC1 and TGC3 Endcap */
  enum NUM_CHAMBERS { 
    NUM_TGC1_ENDCAP = 4,  
    NUM_TGC3_ENDCAP = 5 
  }; 

  /** Find coincidences of TgcCoinHierarchy's from a TgcCoinHierarchyTriggerSector */
  StatusCode findCoincidence(TgcCoinHierarchyTriggerSector* pTrigSectorComb,
                             std::vector<TgcCoinHierarchy*>* pHierarchies);
  /** Find HiPt's and Tracklet's of TgcCoinHierarchy's from an SL with a TgcCoinHierarchyTriggerSector */
  StatusCode findCoincidenceFromSL(TgcCoinHierarchyTriggerSector* pTrigSectorComb,
                                   TgcCoinHierarchy* pHierarchy);
  /** Find a Tracklet of TgcCoinHierarchy's from a HiPt with a TgcCoinHierarchyTriggerSector */
  StatusCode findCoincidenceFromHiPt(TgcCoinHierarchyTriggerSector* pTrigSectorComb,
                                     TgcCoinHierarchy* pHierarchy, 
                                     const bool isStrip);

  /** Find hits of TgcCoinHierarchy's from a TgcCoinHierarchyTriggerSector */
  StatusCode findHit(TgcCoinHierarchyTriggerSector* pTrigSectorComb,
                     std::vector<TgcCoinHierarchy*>* pHierarchies);
  /** Find TGC1 hits of TgcCoinHierarchy's from a HiPt with a TgcCoinHierarchyTriggerSector */
  StatusCode findTGC1HitFromHiPt(TgcCoinHierarchyTriggerSector* pTrigSectorComb,
                                 TgcCoinHierarchy* pHierarchy, 
                                 const bool isStrip);
  /** Find TGC2 and TGC3 hits of TgcCoinHierarchy's from a Tracklet with a TgcCoinHierarchyTriggerSector */
  StatusCode findTGC23HitFromTracklet(TgcCoinHierarchyTriggerSector* pTrigSectorComb,
                                      TgcCoinHierarchy* pHierarchy, 
                                      const bool isStrip);

  /** Check possible relation between HiPt Strip and Tracklet Strip due to the HiPt board bug */
  bool isPossibleTrackletIdStripAssociation(int trackletIdFromHiPtStrip, int trackletIdFromTrackletStrip);
  /** Show WARNING message on a coincidence for debugging */
  void showWARNINGforSetCoincidence(const Muon::TgcCoinData* coinData, TgcCoinHierarchy::COINCIDENCE coincidence); 
  /** Show WARNING message on a hit for debugging */
  void showWARNINGforSetHit(const Muon::TgcPrepData* hitData, const bool isStrip, TgcCoinHierarchy::STATION station);

  /** Get ReadoutID of HiPt from RDOHighPtID */ 
  bool getHiPtIds(const Identifier hiptChannelId, int &hiptSubDetectorId, int &hiptRodID, int &hiptSswID, int &hiptSbLoc, int &hiptSlbID); 
  
  /** Get delta (sagitta) before converion for HiPt */
  int getDeltaBeforeConvert(const bool isStrip, const int delta) const; 
  
  /** Get bitPos etc of TGC3 wire for HiPt */
  void getBitPosOutWire(int &hitId, int &hsub, int &chip, bool &isForward, int &TGC3slbSubMatrix, int *bitpos_out); 
  /** Get bitPos etc of TGC3 strip for HiPt */
  void getBitPosOutStrip(int &hitId, int &hsub, int &TGC3slbSubMatrix, int *bitpos_out); 
  
  /** Get bitPos etc of TGC1 wire for HiPt */
  void getBitPosInWire(bool &isFoward, const int *bitpos_o, const int slbId_o, const int &sector, const int DeltaBeforeConvert, int *sbLoc_in, int *bitpos_in, int *rdochannel); 
  /** Get bitPos etc of TGC1 strip for HiPt */
  void getBitPosInStrip(int &sector, int &chip, const int *bitpos_o, const int DeltaBeforeConvert, int &sbLoc_i, int *bitpos_in, int *slbchannel); 

  /** Get channel from bitpos and SlbType */
  int getchannel(int bitpos, TgcRawData::SlbType slbType); 
  /** Get channel from bitpos and SlbType for Strip channels */
  int getchannelfromHitStrip(int bitpos, TgcRawData::SlbType slbType); 
  /** Get bitpos from channel and SlbType */
  int getbitpos(int channel, TgcRawData::SlbType slbType); 

  /** Check the combination of HiPt and TGC1 Hit */
  bool isHiPtEndcapStripCombination(int hiPtStationEtaOut, int tgc1HitStationEta) const; 

  /** Get sub variable from ROI number */ 
  int getSubFromRoiNumber(const int roi, const bool isStrip, const bool isForward) const; 
  
  /** Get TGC cabling from TGC cabling service from the server */
  StatusCode getCabling(); 

  /** Read mask2 file */ 
  StatusCode readMask2File(); 

  StatusCode findDummyTGC1HitFromHiPt();
  StatusCode findDummyTGC23HitFromTracklet();

  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

  /** TgcCoinHierarchy instance used for conversions of enum's etc. */ 
  TgcCoinHierarchy m_TCH;
  /* TGC Cabling service */
  const ITGCcablingSvc* m_tgcCabling;

  /* Flag for taking account of dummy hit channel on TGC**/
  bool m_dummyHitChannel;
  
  /** TGC Dummy Hit Channel */
  std::ifstream m_dummyhit_file;
  std::vector<int> m_dummyhit_subsystemNumber;
  std::vector<int> m_dummyhit_octantNumber;
  std::vector<int> m_dummyhit_moduleNumber;
  std::vector<int> m_dummyhit_layerNumber;
  std::vector<int> m_dummyhit_rNumber;
  std::vector<int> m_dummyhit_wireOrStrip;
  std::vector<int> m_dummyhit_channelNumber;
  
  std::string m_mask2FileName;

  /** TIMING */
  TgcCoinHierarchy::TIMING m_bc;
};

} // end of namespace Trigger

#endif // TRIGGER_TGCCOINHIERARCHYFINDTOOL_H
