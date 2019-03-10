/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
/// Filename : TileAANtuple.h
/// Author   : Alexander Solodkov (based on TileTBAANtuple)
/// Created  : April, 2010
///
/// DESCRIPTION
///
///    To create ntuple with TileCal Digits and RawChannels
///
/// Properties (JobOption Parameters):
///
///    TileDigitsContainer     string   key value of Digits in TDS
///    TileDigitsContainerFlt  string   key value of filtered Digits in TDS
///    TileBeamElemContainer   string   key of BeamElems in TDS
///    TileRawChannelContainer          key of RawChannels in TDS
///    TileRawChannelContainerOpt       key of opt filtered RawChannels in TDS
///    TileRawChannelContainerDsp       key of Dsp RawChannels in TDS
///    TileRawChannelContainerFit       key of fit filtered RawChannels in TDS
///    TileRawChannelContainerFitCool   key of fit filtered RawChannels in TDS
///    TileRawChannelOF1                key of OF1 filtered RawChannels in TDS
///    TileLaserObject         string   key of Laser Object in TDS
///    CalibrateEnergy         bool     If calibration should be applied to energy
///    CalibMode               bool     If data is in calibration mode (bi-gain input)
///    BSInput                 bool     If data are taken from bytestream file
///    PMTOrder                bool     If PMT ordering should be used in output ntuple
///    UseDspUnits             bool     if output energy units should be equal to units in DSP
///    OfflineUnits            enum     final energy units in ntuple (0=ADC, 1=pCb, 3=MeV)
///    StreamName              string   name of output stream for ntuple
///    NtupleID                string   ID of ntuple
///    TreeSize                int64_t  max size of ROOT tree
///
/// TODO:
///
/// History:
/// 
/// 
//****************************************************************************
#ifndef TILEREC_TILEAANTUPLE_H
#define TILEREC_TILEAANTUPLE_H

// Tile includes
#include "TileConditions/TileCablingService.h"
#include "TileIdentifier/TileRawChannelUnit.h"
#include "TileEvent/TileLaserObject.h"
#include "TileEvent/TileMuonReceiverContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileDQstatus.h"
#include "TileEvent/TileBeamElemContainer.h"
#include "TileConditions/ITileDCSTool.h"

// Athena includes
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"

// Gauid includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include <string>
#include <stdint.h>

#define N_ROS 4
#define N_ROS2 8
#define N_MODULES 64
#define N_DRAWERS 256
#define N_DMUS 16
#define N_CHANS 48
#define N_SAMPLES 7
#define N_RODS 65
#define N_CISPAR 110
#define N_TMDBCHANS 8 // 4 in EB and 8 in B
#define N_TMDBDECISIONS 4

class TTree;

class StoreGateSvc;

class TileID;
class TileHWID;
class TileInfo;
class TileDetDescrManager;
class TileBeamElemContByteStreamCnv;
class ITileBadChanTool;
class TileCondToolEmscale;
class TileL2Builder;
class ITHistSvc;

class TileAANtuple : public AthAlgorithm {
  public:
    //Constructor
    TileAANtuple(std::string name, ISvcLocator* pSvcLocator);

    //Destructor
    virtual ~TileAANtuple();

    //Gaudi Hooks
    StatusCode ntuple_initialize(const EventContext& ctx,
                                 const TileDQstatus& DQstatus);
    StatusCode ntuple_clear();
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

  private:

    StatusCode storeRawChannels(const EventContext& ctx
                                 , const SG::ReadHandleKey<TileRawChannelContainer>& containerKey
		  	        , float ene[N_ROS2][N_MODULES][N_CHANS]
			        , float time[N_ROS2][N_MODULES][N_CHANS]
			        , float chi2[N_ROS2][N_MODULES][N_CHANS]
			        , float ped[N_ROS2][N_MODULES][N_CHANS]
			        , bool fillAll);
                               
    StatusCode storeMFRawChannels(const EventContext& ctx
                                  , const SG::ReadHandleKey<TileRawChannelContainer>& containerKey
                                  , float ene[N_ROS2][N_MODULES][N_CHANS][N_SAMPLES]
                                  , float time[N_ROS2][N_MODULES][N_CHANS][N_SAMPLES]
                                  , float chi2[N_ROS2][N_MODULES][N_CHANS]
                                  , float ped[N_ROS2][N_MODULES][N_CHANS]
                                  , bool fillAll);

    StatusCode storeDigits(const EventContext& ctx
                           , const SG::ReadHandleKey<TileDigitsContainer>& containerKey
			   , short sample[N_ROS2][N_MODULES][N_CHANS][N_SAMPLES]
			   , short gain[N_ROS2][N_MODULES][N_CHANS]
		  	   , bool fillAll);

    StatusCode storeTMDBDecision(const EventContext& ctx);
    StatusCode storeTMDBDigits(const EventContext& ctx);
    StatusCode storeTMDBRawChannel(const EventContext& ctxx);

    StatusCode storeBeamElements(const TileDQstatus& DQstatus);
    StatusCode storeLaser(const EventContext& ctx);
    StatusCode storeDCS();

    StatusCode initNTuple(const EventContext& ctx);

    void fillCellMap(TTree* ntuplePtr);

    void TRIGGER_addBranch(void);
    void LASER_addBranch(void);
    void CISPAR_addBranch(void);
    void DIGI_addBranch(void);
    void DCS_addBranch(void);
    void TMDB_addBranch(void);

    void TRIGGER_clearBranch(void);
    void LASER_clearBranch(void);
    void CISPAR_clearBranch(void);
    void DIGI_clearBranch(void);
    void TMDB_clearBranch(void); 

    inline int digiChannel2PMT(int fragType, int chan) {
      return (abs(m_cabling->channel2hole(fragType, chan)) - 1);
    }

    /// bit_31 of the DMU header must be 1 and
    /// bit_17 of the DMU header must be 0
    inline short CheckDMUFormat(uint32_t header) {
      if (((header >> 31 & 0x1) == 1) && ((header >> 17 & 0x1) == 0))
        return 0; // no error
      else
        return 1; //error
    }


    /// Parity of the DMU header should be odd

    inline short CheckDMUParity(uint32_t header) {
      uint32_t parity(0);
      for (int i = 0; i < 32; ++i)
        parity += header >> i & 0x1;

      if ((parity % 2) == 1)
        return 0; //no error
      else
        return 1; //error
    }

    int m_evtNr;           //!< event counter

    // Trigger items
    int m_evTime;         //!< event time
    int m_run;            //!< run number
    int m_evt;            //!< event number in a run
    int m_lumiBlock;      //!< lumiblock number in a run
    int m_HHMMSS;         //!< event time in HH:MM:SS
    char m_dateTime[32];  //!< event date and time
    int m_trigType;       //!< trigger type (1=Phys, 2=Laser, 4=Ped, 8=CIS)
    uint32_t m_dspFlags;       //!< DSP flags from BS

    int m_l1ID[N_RODS];   //!< Level1 ID   from ROD header
    int m_l1Type[N_RODS]; //!< Level1 Type from ROD header
    int m_evBCID[N_RODS]; //!< Event  BCID from ROD header
    int m_evType[N_RODS]; //!< Event  Type from ROD header

    // Cispar
    uint32_t m_cispar[N_CISPAR];

    // Laser items
    int m_las_version;
    int m_las_BCID;

    int m_las_Filt;
    float m_las_ReqAmp;
    float m_las_MeasAmp;
    float m_las_Temperature;

    struct Arrays {
      // LASERI
      int   m_las_D_ADC[TileLaserObject::nbGains][TileLaserObject::nbDiodes] = {{0}};
      float m_las_D_Ped[TileLaserObject::nbGains][TileLaserObject::nbDiodes] = {{0}};
      float m_las_D_Ped_RMS[TileLaserObject::nbGains][TileLaserObject::nbDiodes] = {{0}};
      float m_las_D_Alpha[TileLaserObject::nbGains][TileLaserObject::nbDiodes] = {{0}};
      float m_las_D_Alpha_RMS[TileLaserObject::nbGains][TileLaserObject::nbDiodes] = {{0}};
      float m_las_D_AlphaPed[TileLaserObject::nbGains][TileLaserObject::nbDiodes] = {{0}};
      float m_las_D_AlphaPed_RMS[TileLaserObject::nbGains][TileLaserObject::nbDiodes] = {{0}};

      int   m_las_PMT_ADC[TileLaserObject::nbGains][TileLaserObject::nbPmts] = {{0}};
      int   m_las_PMT_TDC[TileLaserObject::nbGains][TileLaserObject::nbPmts] = {{0}};
      float m_las_PMT_Ped[TileLaserObject::nbGains][TileLaserObject::nbPmts] = {{0}};
      float m_las_PMT_Ped_RMS[TileLaserObject::nbGains][TileLaserObject::nbPmts] = {{0}};

      // LASERII
      int m_chan[32] = {0};             // Mean value for monitoring diodes, PMTs, phocal, CIS
      float m_chan_Ped[32] = {0};       // Corresponding pedestal values
      float m_chan_Led[32] = {0};       // Corresponding LED values
      float m_chan_Lin[32] = {0};       // Corresponding linearity values
      float m_chan_Alpha[32] = {0};     // Corresponding alpha peaks
      float m_chan_SPed[32] = {0};      // Sigma of pedestal values
      float m_chan_SLed[32] = {0};      // Sigma of LED values
      float m_chan_SLin[32] = {0};      // Sigma of linearity values
      float m_chan_SAlpha[32] = {0};    // Sigma of alpha peaks

      // Digi/Energy items

      short m_sampleFlt[N_ROS2][N_MODULES][N_CHANS][N_SAMPLES] = {{{{0}}}};
      short m_gainFlt[N_ROS2][N_MODULES][N_CHANS] = {{{0}}};

      short m_sample[N_ROS2][N_MODULES][N_CHANS][N_SAMPLES] = {{{{0}}}};
      short m_gain[N_ROS2][N_MODULES][N_CHANS] = {{{0}}};

      short m_rodBCID[N_ROS][N_MODULES] = {{0}};
      short m_fragSize[N_ROS][N_MODULES] = {{0}};
      uint16_t m_dmuMask[N_ROS][N_MODULES][2] = {{{0}}};
      uint16_t m_slinkCRC[N_ROS][N_MODULES][2] = {{{0}}};

      uint32_t m_DMUheader[N_ROS2][N_MODULES][N_DMUS] = {{{0}}};
      short m_DMUbcid[N_ROS2][N_MODULES][N_DMUS] = {{{0}}};
      short m_DMUformatErr[N_ROS2][N_MODULES][N_DMUS] = {{{0}}};
      short m_DMUparityErr[N_ROS2][N_MODULES][N_DMUS] = {{{0}}};
      short m_DMUmemoryErr[N_ROS2][N_MODULES][N_DMUS] = {{{0}}};
      short m_DMUDstrobeErr[N_ROS2][N_MODULES][N_DMUS] = {{{0}}};
      short m_DMUSstrobeErr[N_ROS2][N_MODULES][N_DMUS] = {{{0}}};
      short m_feCRC[N_ROS2][N_MODULES][N_DMUS] = {{{0}}};
      short m_rodCRC[N_ROS2][N_MODULES][N_DMUS] = {{{0}}};

      float m_ene[N_ROS2][N_MODULES][N_CHANS] = {{{0}}};
      float m_time[N_ROS2][N_MODULES][N_CHANS] = {{{0}}};
      float m_ped[N_ROS2][N_MODULES][N_CHANS] = {{{0}}};
      float m_chi2[N_ROS2][N_MODULES][N_CHANS] = {{{0}}}; 

      float m_eFit[N_ROS2][N_MODULES][N_CHANS] = {{{0}}};
      float m_tFit[N_ROS2][N_MODULES][N_CHANS] = {{{0}}};
      float m_pedFit[N_ROS2][N_MODULES][N_CHANS] = {{{0}}};
      float m_chi2Fit[N_ROS2][N_MODULES][N_CHANS] = {{{0}}};

      float m_eFitc[N_ROS2][N_MODULES][N_CHANS] = {{{0}}};
      float m_tFitc[N_ROS2][N_MODULES][N_CHANS] = {{{0}}};
      float m_pedFitc[N_ROS2][N_MODULES][N_CHANS] = {{{0}}};
      float m_chi2Fitc[N_ROS2][N_MODULES][N_CHANS] = {{{0}}};

      float m_eOpt[N_ROS2][N_MODULES][N_CHANS] = {{{0}}};
      float m_tOpt[N_ROS2][N_MODULES][N_CHANS] = {{{0}}};
      float m_pedOpt[N_ROS2][N_MODULES][N_CHANS] = {{{0}}};
      float m_chi2Opt[N_ROS2][N_MODULES][N_CHANS] = {{{0}}};

      float m_eQIE[N_ROS2][N_MODULES][N_CHANS] = {{{0}}};
      float m_tQIE[N_ROS2][N_MODULES][N_CHANS] = {{{0}}};
      float m_pedQIE[N_ROS2][N_MODULES][N_CHANS] = {{{0}}};
      float m_chi2QIE[N_ROS2][N_MODULES][N_CHANS] = {{{0}}};

      float m_eOF1[N_ROS2][N_MODULES][N_CHANS] = {{{0}}};
      float m_tOF1[N_ROS2][N_MODULES][N_CHANS] = {{{0}}};
      float m_pedOF1[N_ROS2][N_MODULES][N_CHANS] = {{{0}}};
      float m_chi2OF1[N_ROS2][N_MODULES][N_CHANS] = {{{0}}};

      float m_eDsp[N_ROS2][N_MODULES][N_CHANS] = {{{0}}};
      float m_tDsp[N_ROS2][N_MODULES][N_CHANS] = {{{0}}};
      float m_pedDsp[N_ROS2][N_MODULES][N_CHANS] = {{{0}}};
      float m_chi2Dsp[N_ROS2][N_MODULES][N_CHANS] = {{{0}}};

      float m_eMF[N_ROS2][N_MODULES][N_CHANS][N_SAMPLES] = {{{{0}}}};
      float m_tMF[N_ROS2][N_MODULES][N_CHANS][N_SAMPLES] = {{{{0}}}};
      float m_chi2MF[N_ROS2][N_MODULES][N_CHANS] = {{{0}}};
      float m_pedMF[N_ROS2][N_MODULES][N_CHANS] = {{{0}}};

      short m_ROD_GlobalCRC[N_ROS][N_MODULES] = {{0}};
      short m_ROD_BCID[N_ROS][N_MODULES] = {{0}};
      short m_ROD_DMUBCIDErr[N_ROS][N_MODULES][N_DMUS] = {{{0}}};
      short m_ROD_DMUmemoryErr[N_ROS][N_MODULES][N_DMUS] = {{{0}}};
      short m_ROD_DMUSstrobeErr[N_ROS][N_MODULES][N_DMUS] = {{{0}}};
      short m_ROD_DMUDstrobeErr[N_ROS][N_MODULES][N_DMUS] = {{{0}}};
      short m_ROD_DMUHeadformatErr[N_ROS][N_MODULES][N_DMUS] = {{{0}}};
      short m_ROD_DMUHeadparityErr[N_ROS][N_MODULES][N_DMUS] = {{{0}}};
      short m_ROD_DMUDataformatErr[N_ROS][N_MODULES][N_DMUS] = {{{0}}};
      short m_ROD_DMUDataparityErr[N_ROS][N_MODULES][N_DMUS] = {{{0}}};
      short m_ROD_DMUfeCRC[N_ROS][N_MODULES][N_DMUS] = {{{0}}};
      short m_ROD_DMUrodCRC[N_ROS][N_MODULES][N_DMUS] = {{{0}}};
      uint16_t m_ROD_DMUMask[N_ROS][N_MODULES][2] = {{{0}}};

      float m_eTMDB[N_ROS][N_MODULES][N_TMDBCHANS] = {{{0}}}; // TMDB
      unsigned char m_sampleTMDB[N_ROS][N_MODULES][N_TMDBCHANS][N_SAMPLES] = {{{{0}}}}; // TMDB
      unsigned char m_decisionTMDB[N_ROS][N_MODULES][N_TMDBDECISIONS] = {{{0}}}; // TMDB
 
      float m_TEMP[4][64][7] = {{{0}}};
      float m_HV[4][64][48] = {{{0}}};
      float m_HVSET[4][64][48] = {{{0}}};
      int m_DRSTATES[4][64] = {{0}};
      short m_HVSTATUS[4][64][48] = {{{0}}};
      short m_DRSTATUS[4][64] = {{0}};
      short m_CHSTATUS[4][64][48] = {{{0}}};
    };
    std::unique_ptr<Arrays> m_arrays;

    int m_daqtype;
    int m_nBadDr;
    int m_nBadHV;
    int m_nBadDCS;
    int m_nBadDB;
    int m_nBadTotal;

    // jobOptions parameters - container names
    SG::ReadHandleKey<TileDigitsContainer> m_digitsContainerKey;
    SG::ReadHandleKey<TileDigitsContainer> m_fltDigitsContainerKey;
    SG::ReadHandleKey<TileBeamElemContainer> m_beamElemContainerKey;
    SG::ReadHandleKey<TileRawChannelContainer> m_rawChannelContainerKey;
    SG::ReadHandleKey<TileRawChannelContainer> m_fitRawChannelContainerKey;
    SG::ReadHandleKey<TileRawChannelContainer> m_fitcRawChannelContainerKey;
    SG::ReadHandleKey<TileRawChannelContainer> m_optRawChannelContainerKey;
    SG::ReadHandleKey<TileRawChannelContainer> m_qieRawChannelContainerKey;
    SG::ReadHandleKey<TileRawChannelContainer> m_dspRawChannelContainerKey;
    SG::ReadHandleKey<TileRawChannelContainer> m_mfRawChannelContainerKey;
    SG::ReadHandleKey<TileRawChannelContainer> m_of1RawChannelContainerKey;
    SG::ReadHandleKey<TileLaserObject> m_laserObjectKey;
    SG::ReadHandleKey<TileRawChannelContainer> m_tileMuRcvRawChannelContainerKey; // TMDB
    SG::ReadHandleKey<TileDigitsContainer> m_tileMuRcvDigitsContainerKey; // TMDB
    SG::ReadHandleKey<TileMuonReceiverContainer> m_tileMuRcvContainerKey; // TMDB
    SG::ReadHandleKey<TileL2Container> m_l2CntKey;

   // other jobOptions parameters
    bool m_calibrateEnergy; //!< convert energy to new units or use amplitude from RawChannel directly
    bool m_useDspUnits; //!< true if energy should be converted to units used in DSP
    bool m_bsInput;    //!< true if bytestream file is used
    bool m_pmtOrder;   //!< change channel ordering to pmt ordering in ntuple
    int m_finalUnit;  //!< calibrate everything to this level
    bool m_calibMode;  //!< If data should be put in calib mode
    bool m_compareMode;  //!< If two sets of data should be compared (e.g. frag4 and frag5)
    bool m_checkDCS;   //!< if false, do not use TileDCS at all
    int m_DCSBranches;   //!< mask like 110101 - which DCS branches to fill

    // energy units
    TileRawChannelUnit::UNIT m_rchUnit;  //!< Unit for TileRawChannels (ADC, pCb, MeV)
    TileRawChannelUnit::UNIT m_dspUnit;  //!< Unit for TileRawChannels in DSP

    // NTuple parameters
    std::string m_streamName;
    std::string m_ntupleID;
    int64_t m_treeSize;

    // The ntuple
    TTree* m_ntuplePtr;
    TTree* m_DCSntuplePtr;

    // handle to THistSvc
    ServiceHandle<ITHistSvc> m_thistSvc;

    // Identifiers
    const TileID* m_tileID;
    const TileHWID* m_tileHWID;

    // Various tools
    const TileCablingService* m_cabling;  //!< cabling tool
    const TileDetDescrManager* m_tileMgr; //!< Pointer to TileDetDescrManager

    ToolHandle<ITileBadChanTool> m_tileBadChanTool; //!< Tile Bad Channel tool

    ToolHandle<TileCondToolEmscale> m_tileToolEmscale; //!< main Tile Calibration tool

    TileBeamElemContByteStreamCnv* m_beamCnv;

    ToolHandle<ITileDCSTool> m_tileDCS{this, "TileDCSTool", "TileDCSTool", "Tile DCS tool"};

    // variables to check SumEt in frag5
    ToolHandle<TileL2Builder> m_l2Builder;  //<! Pointer to TileL2Builder
    float m_sumEt_xx[N_DRAWERS]; //!< Sum Et recalculated offline using DSP raw channels
    float m_sumEz_xx[N_DRAWERS]; //!< Sum Ez recalculated offline using DSP raw channels
    float m_sumE_xx[N_DRAWERS];  //!< Sum E  recalculated offline using DSP raw channels
    float m_sumEt_yy[N_DRAWERS]; //!< Sum Et calcualted inside DSP
    float m_sumEz_yy[N_DRAWERS]; //!< Sum Ez calcualted inside DSP
    float m_sumE_yy[N_DRAWERS];  //!< Sum E  calcualted inside DSP
    float m_sumEt_zz[N_DRAWERS]; //!< Sum Et recalculated offline using offline OF
    float m_sumEz_zz[N_DRAWERS]; //!< Sum Ez recalculated offline using offline OF
    float m_sumE_zz[N_DRAWERS];  //!< Sum E  recalculated offline using offline OF
    bool m_bad[N_ROS][N_MODULES][N_CHANS];

    int m_skipEvents;

    SG::ReadHandleKey<TileDQstatus> m_DQstatusKey
    { this, "TileDQstatus", "TileDQstatus", "TileDQstatus key" };
};

#endif // TILEREC_TILEAANTUPLE_H
