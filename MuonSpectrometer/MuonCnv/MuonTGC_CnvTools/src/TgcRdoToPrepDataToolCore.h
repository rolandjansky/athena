/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CNVTOOLS_TGCRDOTOPREPDATATOOLCORE_H
#define MUONTGC_CNVTOOLS_TGCRDOTOPREPDATATOOLCORE_H

#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonRDO/TgcRdo.h"
#include "MuonTrigCoinData/TgcCoinDataContainer.h"
#include "MuonRDO/TgcRdoContainer.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "TGCcablingInterface/ITGCcablingSvc.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "CxxUtils/CachedValue.h"

#include <string>
#include <vector>
#include <atomic>

namespace MuonGM 
{
  class TgcReadoutElement; 
}

namespace Muon 
{
  class TgcCoinData;

  /** @class TgcRdoToPrepDataToolCore 
   *  This is the algorithm that convert TGCRdo To TGCPrepdata as a tool.
   * 
   * @author Susumu Oda <Susumu.Oda@cern.ch> 
   * @author  Edward Moyse 
   * 
   * This class was developed by Takashi Kubota. 
   */  

  class TgcRdoToPrepDataToolCore : public extends<AthAlgTool, IMuonRdoToPrepDataTool>
    {
    public:
      /** Constructor */
      TgcRdoToPrepDataToolCore(const std::string& t, const std::string& n, const IInterface* p);
      
      /** Destructor */
      virtual ~TgcRdoToPrepDataToolCore()=default;
      
      /** Standard AthAlgTool initialize method */
      virtual StatusCode initialize() override;
      /** Standard AthAlgTool finalize method */
      virtual StatusCode finalize() override;
      
      /** Decode RDO to PRD  
       *  A vector of IdentifierHash are passed in, and the data corresponding to this list (i.e. in a Region of Interest) are converted.  
       *  @param requestedIdHashVect          Vector of hashes to convert i.e. the hashes of ROD collections in a 'Region of Interest'  
       *  @return selectedIdHashVect This is the subset of requestedIdVect which were actually found to contain data   
       *  (i.e. if you want you can use this vector of hashes to optimise the retrieval of data in subsequent steps.) */ 
      virtual StatusCode decode(std::vector<IdentifierHash>& idVect, std::vector<IdentifierHash>& idWithDataVect) const override;

      /** Print Input RDO for debugging */ 
      virtual void printInputRdo() const override;
      
      
    protected:
      /** The number of recorded Bunch Crossings (BCs) is 3. Previous, current and next BCs */
      enum NBC {
        NBC = 3
      };

      struct State {
        /** TgcPrepRawData (hit PRD) containers */
        TgcPrepDataContainer* m_tgcPrepDataContainer[NBC+1] = {0};
        std::unordered_map<Identifier, TgcPrepDataCollection*> m_tgcPrepDataCollections[NBC+1];
        /** TgcCoinData (coincidence PRD) containers */ 
        TgcCoinDataContainer* m_tgcCoinDataContainer[NBC] = {0};
        std::unordered_map<Identifier, TgcCoinDataCollection*> m_tgcCoinDataCollections[NBC];
      };

      struct CablingInfo {
        const ITGCcablingSvc* m_tgcCabling = nullptr;
        /** Flag to distinguish 12-fold cabling and 8-fold cabling */
        bool m_is12fold = true;
        /** Conversion from hash to onlineId */  
        std::vector<uint16_t> m_hashToOnlineId;
        int m_MAX_N_ROD = 0;
      };
      CxxUtils::CachedValue<CablingInfo> m_cablingInfo;

      /** Sub detector IDs are 103 and 104 for TGC A side and C side, respectively */
      enum SUB_DETCTOR_ID {
        ASIDE = 103,
        CSIDE = 104
      };
      
      /** SLB bit position 
	  /code
	  Large R   <--> Small R
	  Large phi <--> Small phi for A side forward chambers and C side backward chambers
	  Small phi <--> Large phi for A side backward chambers and C side forward chambers
	  A-input :  40 -  75,  42 -  73 are valid.
	  B-input :  76 - 111,  78 - 109 are valid.
	  C-input : 112 - 155, 118 - 149 are valid. 
	  D-input : 156 - 199, 162 - 193 are valid. 

	  Channel in this code : Large R                  15  14  13  12  11 ...   0  15 ...   3   2   1   0                 Small R
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

	BIT_POS_B_INPUT_SMALL_R_CH05 = BIT_POS_B_INPUT_ORIGIN                          -  5, // 104
	BIT_POS_A_INPUT_SMALL_R_CH03 = BIT_POS_A_INPUT_ORIGIN                          -  3, //  70
	BIT_POS_B_INPUT_LARGE_R_CH12 = BIT_POS_B_INPUT_ORIGIN - BIT_POS_OFFSET_LARGE_R - 12, //  81

	BIT_POS_A_INPUT_SMALL_R_CH04 = BIT_POS_A_INPUT_ORIGIN                          -  4, //  69
	BIT_POS_A_INPUT_LARGE_R_CH12 = BIT_POS_A_INPUT_ORIGIN - BIT_POS_OFFSET_LARGE_R - 12, //  45
	BIT_POS_A_INPUT_LARGE_R_CH04 = BIT_POS_A_INPUT_ORIGIN - BIT_POS_OFFSET_LARGE_R -  4, //  53
	BIT_POS_A_INPUT_SMALL_R_CH12 = BIT_POS_A_INPUT_ORIGIN                          - 12, //  61

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


      /** Print PRD for debugging */
      void printPrepDataImpl(const TgcPrepDataContainer* const* tgcPrepDataContainer,
                             const TgcCoinDataContainer* const* tgcCoinDataContainer) const;

      using getHitCollection_func = std::function<TgcPrepDataCollection* (Identifier, int locId)>;
      using getCoinCollection_func = std::function<TgcCoinDataCollection* (Identifier, int locId)>;
      
      /** Select decoder based on RDO type (Hit or Coincidence (Tracklet, HiPt and SL)) */
      void selectDecoder(getHitCollection_func& getHitCollection,
                         getCoinCollection_func& getCoinCollection,
                         const TgcRawData& rd,
                         const TgcRdo* rdoColl) const;

      /** Decode RDO's of Hit */
      StatusCode decodeHits(getHitCollection_func& getHitCollection,
                            const TgcRawData& rd) const;
      /** Decode RDO's of Tracklet */
      StatusCode decodeTracklet(getCoinCollection_func& getCoinCollection,
                                const TgcRawData& rd) const;
      /** Decode RDO's of Tracklet EIFI */
      StatusCode decodeTrackletEIFI(getCoinCollection_func& getCoinCollection,
                                    const TgcRawData& rd) const;
      /** Decode RDO's of HiPt */
      StatusCode decodeHiPt(getCoinCollection_func& getCoinCollection,
                            const TgcRawData& rd) const;
      /** Decode RDO's of Inner */
      StatusCode decodeInner(getCoinCollection_func& getCoinCollection,
                             const TgcRawData& rd) const;
      /** Decode RDO's of SectorLogic */
      StatusCode decodeSL(getCoinCollection_func& getCoinCollection,
                          const TgcRawData& rd,
                          const TgcRdo* rdoColl) const;
      
      /** Get bitpos from channel and SlbType */
      int getbitpos(int channel, TgcRawData::SlbType slbType) const;
      /** Get channel from bitpos and SlbType */
      int getchannel(int bitpos, TgcRawData::SlbType slbType) const;

      /** Get r, phi and eta from x, y and z */
      bool getRPhiEtafromXYZ(const double x, const double y, const double z, double& r,  double& phi, double& eta) const;
      /** Get r from eta and z */
      bool getRfromEtaZ(const double eta, const double z, double& r) const;
      /** Get eta from r and z */
      bool getEtafromRZ(const double r, const double z, double& eta) const;
      
      /** Check the rdo is already converted or not */
      bool isAlreadyConverted(const std::vector<const TgcRdo*>& decodedRdoCollVec,
                              const std::vector<const TgcRdo*>& rdoCollVec,
                              const TgcRdo* rdoColl) const;
      
      /** Check the IdHash is already requested or not */
      bool isRequested(const std::vector<IdentifierHash>& requestedIdHashVect,
                       IdentifierHash tgcHashId) const;
      
      /** Check offline ID is OK for TgcReadoutElement */
      bool isOfflineIdOKForTgcReadoutElement(const MuonGM::TgcReadoutElement* descriptor, const Identifier channelId) const;

      /** Fill selected IdentifierHash vector */  
      void fillIdentifierHashVector(const State& state,
                                    std::vector<IdentifierHash>& selectedIdHashVect) const;

      /** Show IdentifierHash vector */ 
      void showIdentifierHashVector(const State& state, std::vector<IdentifierHash>& idHashVect) const;
      /** Show all IdentifierHash */
      void showIdentifierHash(const State& state) const;
      
      /** Check an IdentifierHash is in any TgcPrepDataContainers */
      bool isIdentifierHashFoundInAnyTgcPrepDataContainer(const State& state,
                                                          const IdentifierHash Hash) const;
      /** Check an IdentifierHash is in any TgcCoinDataContainers */
      bool isIdentifierHashFoundInAnyTgcCoinDataContainer(const State& state,
                                                          const IdentifierHash Hash) const;
      
      /** Retrieve slbId, subMatrix and position from Tracklet RDO */
      bool getTrackletInfo(const TgcRawData& rd,
                           int& tmp_slbId, int& tmp_subMatrix, int& tmp_position) const;
      
      /** Get ROI row from RDO */
      int getRoiRow(const TgcRawData& rd) const;
      /** Check SL RDO is at the chamber boundary */
      bool isIncludedInChamberBoundary(const TgcRawData& rd) const;
      
      /** Get bitPos etc of TGC3 wire for HiPt */
      void getBitPosOutWire(const TgcRawData& rd, int& slbsubMatrix, int* bitpos_o) const;
      /** Get bitPos etc of TGC1 wire for HiPt */
      void getBitPosInWire(const TgcRawData& rd, const int DeltaBeforeConvert,
                           int* bitpos_i, int* slbchannel_i, int* slbId_in, int* sbLoc_in, int& sswId_i,
                           const int* bitpos_o, int* slbchannel_o, const int slbId_o) const;
      /** Get bitPos etc of TGC3 strip for HiPt */
      void getBitPosOutStrip(const TgcRawData& rd, int& slbsubMatrix, int* bitpos_o) const;
      /** Get bitPos etc of TGC1 strip for HiPt */
      void getBitPosInStrip(const TgcRawData& rd, const int DeltaBeforeConvert,
                            int* bitpos_i, int* slbchannel_i, int& sbLoc_i, int& sswId_i,
                            const int* bitpos_o, int* slbchannel_o) const;
      /** Get bitPos etc of wire for SL */
      void getBitPosWire(const TgcRawData& rd, const int hitId_w, const int sub_w, int& subMatrix_w, 
			 int* bitpos_w) const;
      /** Get bitPos etc of strip for SL */
      void getBitPosStrip(const int hitId_s, const int sub_s, int& subMatrix_s, int* bitpos_s) const;
      
      /** Get delta (sagitta) before converion for HiPt */
      int getDeltaBeforeConvert(const TgcRawData& rd) const;
      
      /** Check if a chamber in BigWheel is a backward chamber or a forward chamber */
      bool isBackwardBW(const TgcRawData& rd) const;
      
      /** Get the width of a wire channel in the r direction */
      double getWidthWire(const MuonGM::TgcReadoutElement* descriptor, const int gasGap, const int channel) const;
      /** Get the width of a strip channel in the phi direction */
      double getWidthStrip(const MuonGM::TgcReadoutElement* descriptor, const int gasGap, const int channel, 
			   const Identifier channelId) const;
      /** Get wire geometry (width, r, z) for SL */ 
      bool getSLWireGeometry(const Identifier* channelId_wire, double& width_wire, double& r_wire, double& z_wire) const;
      /** Get strip geometry (width, theta) for SL */ 
      bool getSLStripGeometry(const Identifier* channelId_strip, const bool isBackWard, const bool isAside, 
			      double& width_strip, double& theta_strip) const;
      /** Get position and offline ID of TGC3 wire for HiPt */ 
      bool getPosAndIdWireOut(const MuonGM::TgcReadoutElement** descriptor_o, const Identifier* channelIdOut,
                              const int* gasGap_o, const int* channel_o,
                              double& width_o, double& hit_position_o, Amg::Vector2D& tmp_hitPos_o,
                              Identifier& channelIdOut_tmp) const;
      /** Get position and offline ID of TGC3 strip for HiPt */ 
      bool getPosAndIdStripOut(const MuonGM::TgcReadoutElement** descriptor_o, const Identifier* channelIdOut,
                               const int* gasGap_o, const int* channel_o,
                               double& width_o, double& hit_position_o, Amg::Vector2D& tmp_hitPos_o,
                               Identifier& channelIdOut_tmp,
                               const bool isBackward, const bool isAside) const;
      /** Get position and offline ID of TGC1 wire for HiPt */ 
      bool getPosAndIdWireIn(const MuonGM::TgcReadoutElement** descriptor_i, const Identifier* channelIdIn,
                             const int* gasGap_i, const int* channel_i,
                             double& width_i, double& hit_position_i, Amg::Vector2D& tmp_hitPos_i,
                             Identifier& channelIdIn_tmp) const;
      /** Get position and offline ID of TGC1 strip for HiPt */ 
      bool getPosAndIdStripIn(const MuonGM::TgcReadoutElement** descriptor_i, const Identifier* channelIdIn,
                              const int* gasGap_i, const int* channel_i,
                              double& width_i, double& hit_position_i, Amg::Vector2D& tmp_hitPos_i,
                              Identifier& channelIdIn_tmp,
                              const bool isBackward, const bool isAside) const;
      
      /** Get ReadoutID of HiPt from RDOHighPtID */  
      bool getHiPtIds(const TgcRawData& rd, int& sswId_o, int& sbLoc_o, int& slbId_o) const;

      /** Get ReadoutID of SL from RDO */  
      bool getSLIds(const bool isStrip, const TgcRawData& rd, Identifier* channelId, 
		    int& index, int& chip, int& hitId, int& sub, int& sswId, int& sbLoc, int& subMatrix, int *bitpos, 
		    const bool isBoundary=false, const TgcRdo* rdoColl=0, 
		    const int index_w=-1, const int chip_w=-1, const int hitId_w=-1, const int sub_w=-1) const;
      /** Get strip sbLoc of Endcap chamber boundary from HiPt Strip */
      bool getSbLocOfEndcapStripBoundaryFromHiPt(const TgcRawData& rd,
                                                 int& sbLoc,
						 const TgcRdo* rdoColl, 
						 const int index_w, const int chip_w, const int hitId_w, const int sub_w) const;
      /** Get strip sbLoc of Endcap chamber boundary from Tracklet Strip */
      bool getSbLocOfEndcapStripBoundaryFromTracklet(const TgcRawData& rd,
                                                     int& sbLoc,
						     const TgcRdo* rdoColl, 
						     const int index_w, const int chip_w, const int hitId_w, const int sub_w) const;
      /** Get trackletIds of three Tracklet Strip candidates in the Endcap boudary */ 
      void getEndcapStripCandidateTrackletIds(const int roi, int &trackletIdStripFirst, 
					      int &trackletIdStripSecond, int &trackletIdStripThird) const;

      const CablingInfo*  getCabling() const;

      /** Get SL local position */
      const Amg::Vector2D* getSLLocalPosition(const MuonGM::TgcReadoutElement* readout, const Identifier, const double eta, const double phi) const; 

      SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_muDetMgrKey {this, "DetectorManagerKey", "MuonDetectorManager", "Key of input MuonDetectorManager condition data"}; 

      ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
      
      /* TGC Cabling service */
      const ITGCcablingSvc* m_tgcCabling = nullptr;

      /** TgcPrepRawData container key for current BC */ 
      std::string m_outputCollectionLocation;      
      
      /** TgcCoinData container key for current BC */ 
      std::string m_outputCoinCollectionLocation;
      
      /** Identifier hash offset */
      int m_tgcOffset;

      /** Switch for the decoding of TGC RDO into TgcPrepData */
      bool m_decodeData; 
      /** Switch for the coincince decoding */
      bool m_fillCoinData; 
      
      /** Switch for error message disabling on one invalid channel in 
	  sector A09 seen in 2008 data, at least run 79772 - 91800. 
	  bug #48828: TgcRdoToTgcDigit WARNING ElementID not found for 
	  sub=103 rod=9 ssw=6 slb=20 bitpos=151 +offset=0 orFlag=0
      */
      bool m_show_warning_level_invalid_A09_SSW6_hit;

      /** Flag for dropping PRD's with zero widths */
      bool m_dropPrdsWithZeroWidth;
      /** Cut value for zero widths */ 
      const static double s_cutDropPrdsWithZeroWidth; // 0.1 mm

      /** long to count the numbers of RDOs and PRDs */
      mutable std::atomic<long> m_nHitRDOs;
      mutable std::atomic<long> m_nHitPRDs;
      mutable std::atomic<long> m_nTrackletRDOs; 
      mutable std::atomic<long> m_nTrackletPRDs; 
      mutable std::atomic<long> m_nTrackletEIFIRDOs; 
      mutable std::atomic<long> m_nTrackletEIFIPRDs; 
      mutable std::atomic<long> m_nHiPtRDOs; 
      mutable std::atomic<long> m_nHiPtPRDs; 
      mutable std::atomic<long> m_nSLRDOs;  
      mutable std::atomic<long> m_nSLPRDs; 

      SG::ReadHandleKey<TgcRdoContainer> m_rdoContainerKey;//"TGCRDO"

      SG::WriteHandleKeyArray<Muon::TgcCoinDataContainer> m_outputCoinKeys;
      SG::WriteHandleKeyArray<TgcPrepDataContainer> m_outputprepdataKeys;

      /** Aboid compiler warning **/
      virtual StatusCode decode( const std::vector<uint32_t>& /*robIds*/ ) const override {return StatusCode::FAILURE;}
   }; 
} // end of namespace

#endif // MUONTGC_CNVTOOLS_TGCRDOTOPREPDATATOOLCORE_H
