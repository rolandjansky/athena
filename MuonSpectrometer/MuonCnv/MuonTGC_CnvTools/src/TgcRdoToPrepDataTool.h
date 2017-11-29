/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TgcRdoToPrepDataTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef MUONTGC_CNVTOOLS_TGCRDOTOPREPDATATOOL_H
#define MUONTGC_CNVTOOLS_TGCRDOTOPREPDATATOOL_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"

#include "GaudiKernel/ToolHandle.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonRDO/TgcRdo.h"
#include "MuonTrigCoinData/TgcCoinDataContainer.h"

#include "MuonRDO/TgcRdoContainer.h"

class AtlasDetectorID;
class Identifier;

class ITGCcablingSvc;
class TgcIdHelper;

namespace MuonGM 
{
  class MuonDetectorManager;
  class TgcReadoutElement; 
}

namespace Muon 
{
  class IMuonRawDataProviderTool;
  class TgcCoinData;

  /** @class TgcRdoToPrepDataTool 
   *  This is the algorithm that convert TGCRdo To TGCPrepdata as a tool.
   * 
   * @author Susumu Oda <Susumu.Oda@cern.ch> 
   * @author  Edward Moyse 
   * 
   * This class was developed by Takashi Kubota. 
   */  

  class TgcRdoToPrepDataTool : virtual public IMuonRdoToPrepDataTool, virtual public AthAlgTool
    {
    public:
      /** Constructor */
      TgcRdoToPrepDataTool(const std::string& t, const std::string& n, const IInterface* p);
      
      /** Destructor */
      virtual ~TgcRdoToPrepDataTool();
      
      /** Query the IMuonRdoToPrepDataTool interface */
      virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIf) override;
      
      /** Standard AthAlgTool initialize method */
      virtual StatusCode initialize() override;
      /** Standard AthAlgTool finalize method */
      virtual StatusCode finalize() override;
      
      /** Decode RDO to PRD  
       *  A vector of IdentifierHash are passed in, and the data corresponding to this list (i.e. in a Region of Interest) are converted.  
       *  @param requestedIdHashVect          Vector of hashes to convert i.e. the hashes of ROD collections in a 'Region of Interest'  
       *  @return selectedIdHashVect This is the subset of requestedIdVect which were actually found to contain data   
       *  (i.e. if you want you can use this vector of hashes to optimise the retrieval of data in subsequent steps.) */ 
      virtual StatusCode decode(std::vector<IdentifierHash>& idVect, std::vector<IdentifierHash>& idWithDataVect) override;

      /** Print Input RDO for debugging */ 
      virtual void printInputRdo() override;
      
      /** Print PRD for debugging */
      virtual void printPrepData() override;

      /** Resolve possible conflicts with IProperty::interfaceID() */
      static const InterfaceID& interfaceID() { return IMuonRdoToPrepDataTool::interfaceID(); }
      
    private:
      /** The number of recorded Bunch Crossings (BCs) is 3. Previous, current and next BCs */
      enum NBC {
        NBC = 3
      };

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

      /** Select decoder based on RDO type (Hit or Coincidence (Tracklet, HiPt and SL)) */
      void selectDecoder(const TgcRdo::const_iterator& itD, const TgcRdo* rdoColl); 
      
      /** Decode RDO's of Hit */
      StatusCode decodeHits(const TgcRdo::const_iterator& itD);
      /** Decode RDO's of Tracklet */
      StatusCode decodeTracklet(const TgcRdo::const_iterator& itD);
      /** Decode RDO's of Tracklet EIFI */
      StatusCode decodeTrackletEIFI(const TgcRdo::const_iterator& itD);
      /** Decode RDO's of HiPt */
      StatusCode decodeHiPt(const TgcRdo::const_iterator& itD);
      /** Decode RDO's of Inner */
      StatusCode decodeInner(const TgcRdo::const_iterator& itD);
      /** Decode RDO's of SectorLogic */
      StatusCode decodeSL(const TgcRdo::const_iterator& itD, const TgcRdo* rdoColl);
      
      /** Get bitpos from channel and SlbType */
      int getbitpos(int channel, TgcRawData::SlbType slbType);
      /** Get channel from bitpos and SlbType */
      int getchannel(int bitpos, TgcRawData::SlbType slbType);

      /** Get r, phi and eta from x, y and z */
      bool getRPhiEtafromXYZ(const double x, const double y, const double z, double& r,  double& phi, double& eta);
      /** Get r from eta and z */
      bool getRfromEtaZ(const double eta, const double z, double& r);
      /** Get eta from r and z */
      bool getEtafromRZ(const double r, const double z, double& eta);
      
      /** Check the rdo is already converted or not */
      bool isAlreadyConverted(std::vector<const TgcRdo*>& rdoCollVec, const TgcRdo* rdoColl) const;
      
      /** Check the IdHash is already requested or not */
      bool isRequested(std::vector<IdentifierHash>& requestedIdHashVect, IdentifierHash tgcHashId) const;
      
      /** Check offline ID is OK for TgcReadoutElement */
      bool isOfflineIdOKForTgcReadoutElement(const MuonGM::TgcReadoutElement* descriptor, const Identifier channelId);

      /** Fill selected IdentifierHash vector */  
      void fillIdentifierHashVector(std::vector<IdentifierHash>& selectedIdHashVect);

      /** Show IdentifierHash vector */ 
      void showIdentifierHashVector(std::vector<IdentifierHash>& idHashVect);
      /** Show all IdentifierHash */
      void showIdentifierHash();
      
      /** Check an IdentifierHash is in any TgcPrepDataContainers */
      bool isIdentifierHashFoundInAnyTgcPrepDataContainer(const IdentifierHash Hash) const;
      /** Check an IdentifierHash is in any TgcCoinDataContainers */
      bool isIdentifierHashFoundInAnyTgcCoinDataContainer(const IdentifierHash Hash) const;
      
      /** Retrieve slbId, subMatrix and position from Tracklet RDO */
      bool getTrackletInfo(const TgcRdo::const_iterator& itD, int& tmp_slbId, int& tmp_subMatrix, int& tmp_position);
      
      /** Get ROI row from RDO */
      int getRoiRow(const TgcRdo::const_iterator& itD) const;
      /** Check SL RDO is at the chamber boundary */
      bool isIncludedInChamberBoundary(const TgcRdo::const_iterator& itD) const;
      
      /** Get bitPos etc of TGC3 wire for HiPt */
      void getBitPosOutWire(const TgcRdo::const_iterator& itD, int& slbsubMatrix, int* bitpos_o);
      /** Get bitPos etc of TGC1 wire for HiPt */
      void getBitPosInWire(const TgcRdo::const_iterator& itD, const int DeltaBeforeConvert,
                           int* bitpos_i, int* slbchannel_i, int* slbId_in, int* sbLoc_in, int& sswId_i,
                           const int* bitpos_o, int* slbchannel_o, const int slbId_o);
      /** Get bitPos etc of TGC3 strip for HiPt */
      void getBitPosOutStrip(const TgcRdo::const_iterator& itD, int& slbsubMatrix, int* bitpos_o);
      /** Get bitPos etc of TGC1 strip for HiPt */
      void getBitPosInStrip(const TgcRdo::const_iterator& itD, const int DeltaBeforeConvert,
                            int* bitpos_i, int* slbchannel_i, int& sbLoc_i, int& sswId_i,
                            const int* bitpos_o, int* slbchannel_o);
      /** Get bitPos etc of wire for SL */
      void getBitPosWire(const TgcRdo::const_iterator& itD, const int hitId_w, const int sub_w, int& subMatrix_w, 
			 int* bitpos_w);
      /** Get bitPos etc of strip for SL */
      void getBitPosStrip(const int hitId_s, const int sub_s, int& subMatrix_s, int* bitpos_s);
      
      /** Get delta (sagitta) before converion for HiPt */
      int getDeltaBeforeConvert(const TgcRdo::const_iterator& itD) const;
      
      /** Check if a chamber in BigWheel is a backward chamber or a forward chamber */
      bool isBackwardBW(const TgcRdo::const_iterator& itD) const;
      
      /** Get the width of a wire channel in the r direction */
      double getWidthWire(const MuonGM::TgcReadoutElement* descriptor, const int gasGap, const int channel) const;
      /** Get the width of a strip channel in the phi direction */
      double getWidthStrip(const MuonGM::TgcReadoutElement* descriptor, const int gasGap, const int channel, 
			   const Identifier channelId) const;
      /** Get wire geometry (width, r, z) for SL */ 
      bool getSLWireGeometry(const Identifier* channelId_wire, double& width_wire, double& r_wire, double& z_wire);
      /** Get strip geometry (width, theta) for SL */ 
      bool getSLStripGeometry(const Identifier* channelId_strip, const bool isBackWard, const bool isAside, 
			      double& width_strip, double& theta_strip);
      /** Get position and offline ID of TGC3 wire for HiPt */ 
      bool getPosAndIdWireOut(const MuonGM::TgcReadoutElement** descriptor_o, const Identifier* channelIdOut,
                              const int* gasGap_o, const int* channel_o,
                              double& width_o, double& hit_position_o, Amg::Vector2D& tmp_hitPos_o,
                              Identifier& channelIdOut_tmp);
      /** Get position and offline ID of TGC3 strip for HiPt */ 
      bool getPosAndIdStripOut(const MuonGM::TgcReadoutElement** descriptor_o, const Identifier* channelIdOut,
                               const int* gasGap_o, const int* channel_o,
                               double& width_o, double& hit_position_o, Amg::Vector2D& tmp_hitPos_o,
                               Identifier& channelIdOut_tmp,
                               const bool isBackward, const bool isAside);
      /** Get position and offline ID of TGC1 wire for HiPt */ 
      bool getPosAndIdWireIn(const MuonGM::TgcReadoutElement** descriptor_i, const Identifier* channelIdIn,
                             const int* gasGap_i, const int* channel_i,
                             double& width_i, double& hit_position_i, Amg::Vector2D& tmp_hitPos_i,
                             Identifier& channelIdIn_tmp);
      /** Get position and offline ID of TGC1 strip for HiPt */ 
      bool getPosAndIdStripIn(const MuonGM::TgcReadoutElement** descriptor_i, const Identifier* channelIdIn,
                              const int* gasGap_i, const int* channel_i,
                              double& width_i, double& hit_position_i, Amg::Vector2D& tmp_hitPos_i,
                              Identifier& channelIdIn_tmp,
                              const bool isBackward, const bool isAside);
      
      /** Get ReadoutID of HiPt from RDOHighPtID */  
      bool getHiPtIds(const TgcRdo::const_iterator& itD, int& sswId_o, int& sbLoc_o, int& slbId_o);

      /** Get ReadoutID of SL from RDO */  
      bool getSLIds(const bool isStrip, const TgcRdo::const_iterator& itD, Identifier* channelId, 
		    int& index, int& chip, int& hitId, int& sub, int& sswId, int& sbLoc, int& subMatrix, int *bitpos, 
		    const bool isBoundary=false, const TgcRdo* rdoColl=0, 
		    const int index_w=-1, const int chip_w=-1, const int hitId_w=-1, const int sub_w=-1);
      /** Get strip sbLoc of Endcap chamber boundary from HiPt Strip */
      bool getSbLocOfEndcapStripBoundaryFromHiPt(const TgcRdo::const_iterator& itD, int& sbLoc, 
						 const TgcRdo* rdoColl, 
						 const int index_w, const int chip_w, const int hitId_w, const int sub_w);
      /** Get strip sbLoc of Endcap chamber boundary from Tracklet Strip */
      bool getSbLocOfEndcapStripBoundaryFromTracklet(const TgcRdo::const_iterator& itD, int& sbLoc, 
						     const TgcRdo* rdoColl, 
						     const int index_w, const int chip_w, const int hitId_w, const int sub_w);
      /** Get trackletIds of three Tracklet Strip candidates in the Endcap boudary */ 
      void getEndcapStripCandidateTrackletIds(const int roi, int &trackletIdStripFirst, 
					      int &trackletIdStripSecond, int &trackletIdStripThird) const;

      StatusCode getCabling();

      /** Get SL local position */
      const Amg::Vector2D* getSLLocalPosition(const MuonGM::TgcReadoutElement* readout, const Identifier, const double eta, const double phi) const; 

      /** muon detector manager */
      const MuonGM::MuonDetectorManager * m_muonMgr;

      /** TGC identifier helper */
      const TgcIdHelper* m_tgcHelper;
      
      /* TGC Cabling service */
      const ITGCcablingSvc* m_tgcCabling;

      /** TgcPrepRawData (hit PRD) containers */
      TgcPrepDataContainer* m_tgcPrepDataContainer[NBC+1];
      /** TgcPrepRawData container key for current BC */ 
      std::string m_outputCollectionLocation;      
      
      /** TgcCoinData (coincidence PRD) containers */ 
      TgcCoinDataContainer* m_tgcCoinDataContainer[NBC];
      /** TgcCoinData container key for current BC */ 
      std::string m_outputCoinCollectionLocation;
      
      /** Identifier hash offset */
      int m_tgcOffset;

      /** Switch for the decoding of TGC BS into RDO for EF */
      bool m_useBStoRdoTool;                
      /** Switch for the decoding of TGC RDO into TgcPrepData */
      bool m_decodeData; 
      /** Switch for the coincince decoding */
      bool m_fillCoinData; 
      
      /** ToolHandle of the TGC_RawDataProviderTool */
      ToolHandle<IMuonRawDataProviderTool> m_rawDataProviderTool;

      /** Switch for keeping Track of FullEvent Decoding */
      bool m_fullEventDone[NBC+1];

      /** Flag to check the ROB with an onlineId is already decoded */  
      std::vector<bool> m_decodedOnlineId;  

      /** Conversion from hash to onlineId */  
      std::vector<uint16_t> m_hashToOnlineId;

      /** Vector of RDO collections already decoded in the event */
      std::vector<const TgcRdo*> m_decodedRdoCollVec;

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
      long m_nHitRDOs;
      long m_nHitPRDs;
      long m_nTrackletRDOs; 
      long m_nTrackletPRDs; 
      long m_nTrackletEIFIRDOs; 
      long m_nTrackletEIFIPRDs; 
      long m_nHiPtRDOs; 
      long m_nHiPtPRDs; 
      long m_nSLRDOs;  
      long m_nSLPRDs; 

      /** Flag to distinguish 12-fold cabling and 8-fold cabling */
      bool m_is12fold;

      SG::ReadHandleKey<TgcRdoContainer> m_rdoContainerKey;//"TGCRDO"

      SG::WriteHandleKeyArray<Muon::TgcCoinDataContainer> m_outputCoinKeys;
      SG::WriteHandleKeyArray<TgcPrepDataContainer> m_outputprepdataKeys;

      /** Aboid compiler warning **/
      virtual StatusCode decode( const std::vector<uint32_t>& /*robIds*/ ) override {return StatusCode::FAILURE;}
   }; 
} // end of namespace

#endif // MUONTGC_CNVTOOLS_TGCRDOTOPREPDATATOOL_H
