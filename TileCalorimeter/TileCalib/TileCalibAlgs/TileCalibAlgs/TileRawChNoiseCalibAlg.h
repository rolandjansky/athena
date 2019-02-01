/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILERAWCHNOISECALIBALG_H
#define TILERAWCHNOISECALIBALG_H

// ********************************************************************
//
// NAME:     TileRawChNoiseCalib.h
// PACKAGE:  TileCalib
//
// AUTHOR:   Luca Fiorini (Luca.Fiorini@cern.ch)
// 
// April 2008	
// ********************************************************************

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"

// Athena includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODEventInfo/EventInfo.h"

// Tile includes
#include "TileConditions/TileCablingService.h"
#include "TileConditions/TileCondIdTransforms.h"
#include "TileConditions/TileCondToolEmscale.h"
#include "TileConditions/ITileBadChanTool.h"
#include "TileEvent/TileDQstatus.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"

#include <cmath>
#include <vector>
#include <string>
#include <map>
#include <stdint.h>

#define NSIDES 2
#define NSAMPLES 4
#define NTOWERS 17
#define NCELLGAINS 6
#define NPARS 8

class TileHWID;
class TileBeamElemContByteStreamCnv;
class TileDQstatus;
class TileRawChannel;
class TH1F;

class TileRawChNoiseCalibAlg: public AthAlgorithm {

  public:
    TileRawChNoiseCalibAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~TileRawChNoiseCalibAlg();

    // Functions
    virtual StatusCode initialize() override; //only array initialization is done here
    virtual StatusCode execute() override;
    virtual StatusCode finalize() override;
    void finalRawCh(int rctype);

  private:

    enum RCtype {
      Fit = 0, Fixed = 1, Opt = 2, Dsp = 3, OF1 = 4, MF = 5, RCnum = 6
    };

    enum trigtype {
      Unknown = 0, Phys = 1, Las = 2, Ped = 4, Cis = 8
    };

    StatusCode FirstEvt_initialize(); // real initialization is done in this method

    StatusCode fillRawChannels(const TileDQstatus* dqStatus,
                     const SG::ReadHandleKey<TileRawChannelContainer>& rawChannelContainerKey,
                     RCtype rctype); // raw chans variables is done here
    void StoreRunInfo(const TileDQstatus* dqStatus); // called only at the first event. General variables

    void fillCell(TileRawChannelUnit::UNIT RChUnit, const TileRawChannel * rch);
    void fillCellHist(); //fill HGHG and LGLG histograms
    void finalCell(); //fills the cell variables of the ntuple
    void doFit(TH1F* h, float* gp, bool invert = true); //double gaussian fit

    void deleteHist(); // delete all histograms

    inline int digiChannel2PMT(int ros, int chan) {
      return (abs(m_cabling->channel2hole(ros, chan)) - 1);
    }

    // If data should be put in calib mode
    bool m_calibMode;

    bool m_doFit;
    bool m_doFixed;
    bool m_doOpt;
    bool m_doDsp;
    bool m_doOF1;
    bool m_doMF;
    bool m_saveHist;
    bool m_invertChanRatio;

    int m_UseforCells;

    bool m_maskBadChannels;

    // by default we use channels
    bool m_usePMT;

    bool m_fillidx;

    // Tools / storegate info
    TileBeamElemContByteStreamCnv* m_beamCnv;
    const TileCablingService* m_cabling;
    // Identifiers
    const TileID* m_tileID;
    const TileHWID* m_tileHWID;
    ToolHandle<TileCondToolEmscale> m_tileToolEmscale;
    ToolHandle<ITileBadChanTool> m_tileBadChanTool;
    SG::ReadHandleKey<TileDQstatus> m_dqStatusKey;
    ToolHandle<TileCondIdTransforms> m_tileIdTrans;
    const uint32_t* m_cispar;

    SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this,
      "EventInfo", "EventInfo", "EventInfo key"};
    SG::ReadHandleKey<TileRawChannelContainer> m_rawChannelContainerFixedKey{this,
         "TileRawChannelContainerFixed", "TileRawChannelFixed", 
         "Input Tile raw channel container reconstructed with ATLAS method"};
    SG::ReadHandleKey<TileRawChannelContainer> m_rawChannelContainerFitKey{this,
         "TileRawChannelContainerFit", "TileRawChannelFit", 
         "Input Tile raw channel container reconstructed with Fit method"};
    SG::ReadHandleKey<TileRawChannelContainer> m_rawChannelContainerOptKey{this,
         "TileRawChannelContainerOpt", "TileRawChannelOpt2", 
         "Input Tile raw channel container reconstructed with Opt method"};
    SG::ReadHandleKey<TileRawChannelContainer> m_rawChannelContainerDspKey{this,
         "TileRawChannelContainerDsp", "TileRawChannelCnt", 
         "Input Tile raw channel container from DSP"};
     SG::ReadHandleKey<TileRawChannelContainer> m_rawChannelContainerOF1Key{this,
         "TileRawChannelContainerOF1", "TileRawChannelOF1", 
         "Input Tile raw channel container reconstructed with OF1 method"};     
     SG::ReadHandleKey<TileRawChannelContainer> m_rawChannelContainerMFKey{this,
         "TileRawChannelContainerMF", "TileRawChannelMF", 
         "Input Tile raw channel container reconstructed with MF method"};

    // jobOptions

    // Name of ntuple and file
    std::string m_ntupleID;
    std::string m_file;
    int64_t m_treeSize;
    //unsigned int m_nDrawers;

    // Container Parameters
    std::string m_digitsContainer;
    std::string m_beamElemContainer;
    std::string m_fixedRawChannelContainer;
    std::string m_fitRawChannelContainer;
    std::string m_optRawChannelContainer;
    std::string m_dspRawChannelContainer;
    std::string m_OF1RawChannelContainer;
    std::string m_MFRawChannelContainer;

    using Tile = TileCalibUtils;

    TH1F* (*m_histAmp)[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN];
    TH1F* (*m_histCellAmp)[Tile::MAX_DRAWER][NSAMPLES][NTOWERS][NCELLGAINS];

    // event number
    int m_evtNr;
    int (*m_evt)[Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN];
    uint8_t (*m_ros)[Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN];
    uint8_t (*m_drawer)[Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN];
    uint8_t (*m_channel)[Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN];
    bool (*m_gain)[Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN];
    // Trigger items
    int m_time;
    int m_year;
    int m_month;
    int m_day;
    int m_yday;
    int m_hour;
    int m_min;
    int m_run;
    int m_trigType;
    //int m_nsamples;

    // RawCh items
    float (*m_rc_mean)[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN];
    float (*m_rc_sigma)[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN];
    float (*m_rc_av)[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN];
    float (*m_rc_rms)[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN];
    float (*m_rc_skewness)[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN];
    float (*m_rc_kurtosis)[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN];
    float (*m_rc_mean_err)[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN];
    float (*m_rc_sigma_err)[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN];
    float (*m_rc_chi2)[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN];
    float (*m_rc_ndf)[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN];
    float (*m_rc_probC2)[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN];

    float (*m_rc_ggpar)[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN][NPARS]; // sigma gauss1, rel normalization of the gaussians, sigma gauss2, chi2/ndf, error in sigma1, sigma2 and normalization, correlation between sigma1 and sigma2
    float (*m_rc_gsigma1)[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN];
    float (*m_rc_gsigma2)[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN];
    float (*m_rc_gnorm)[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN];
    float (*m_rc_gchi2)[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN];
    float (*m_rc_gerrsigma1)[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN];
    float (*m_rc_gerrsigma2)[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN];
    float (*m_rc_gerrnorm)[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN];
    float (*m_rc_gcorrsigma1sigma2)[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN];

    //Cell items
    //structure is side:             A=0 C=1,
    //             drawer:           0-63,
    //		 sample:           A=0, BC=1, D=2, E=3
    //             tower:            0-16,
    //             gain combination: LGLG, LG-, -LG, HGHG, HG-, -HG
    bool (*m_side)[Tile::MAX_DRAWER][NSAMPLES][NTOWERS][NCELLGAINS];
    uint8_t (*m_phi)[Tile::MAX_DRAWER][NSAMPLES][NTOWERS][NCELLGAINS];
    uint8_t (*m_sample)[Tile::MAX_DRAWER][NSAMPLES][NTOWERS][NCELLGAINS];
    uint8_t (*m_tower)[Tile::MAX_DRAWER][NSAMPLES][NTOWERS][NCELLGAINS];
    uint8_t (*m_gg)[Tile::MAX_DRAWER][NSAMPLES][NTOWERS][NCELLGAINS];

    float (*m_ecell_av)[Tile::MAX_DRAWER][NSAMPLES][NTOWERS][NCELLGAINS];
    float (*m_ecell_rms)[Tile::MAX_DRAWER][NSAMPLES][NTOWERS][NCELLGAINS];
    uint32_t (*m_ecell_hash)[Tile::MAX_DRAWER][NSAMPLES][NTOWERS];

    int (*m_cell_nch)[Tile::MAX_DRAWER][NSAMPLES][NTOWERS][Tile::MAX_GAIN]; // number of good channels in given cell in given event - 0 or 1 or 2
    float (*m_ecell_ene)[Tile::MAX_DRAWER][NSAMPLES][NTOWERS][Tile::MAX_GAIN];
    float (*m_ggpar)[Tile::MAX_DRAWER][NSAMPLES][NTOWERS][NCELLGAINS][NPARS]; // sigma gauss1, rel normalization of the gaussians, sigma gauss2, chi2/ndf, error in sigma1, sigma2 and normalization, correlation between sigma1 and sigma2
    float (*m_gsigma1)[Tile::MAX_DRAWER][NSAMPLES][NTOWERS][NCELLGAINS];
    float (*m_gsigma2)[Tile::MAX_DRAWER][NSAMPLES][NTOWERS][NCELLGAINS];
    float (*m_gnorm)[Tile::MAX_DRAWER][NSAMPLES][NTOWERS][NCELLGAINS];
    float (*m_gchi2)[Tile::MAX_DRAWER][NSAMPLES][NTOWERS][NCELLGAINS];
    float (*m_gerrsigma1)[Tile::MAX_DRAWER][NSAMPLES][NTOWERS][NCELLGAINS];
    float (*m_gerrsigma2)[Tile::MAX_DRAWER][NSAMPLES][NTOWERS][NCELLGAINS];
    float (*m_gerrnorm)[Tile::MAX_DRAWER][NSAMPLES][NTOWERS][NCELLGAINS];
    float (*m_gcorrsigma1sigma2)[Tile::MAX_DRAWER][NSAMPLES][NTOWERS][NCELLGAINS];

    enum CELL_CHANNEL {OUTER_MBTS_CHANNEL = 4, SPECIAL_C10_CHANNEL = 5, E1_CHANNEL = 12};
};

#endif // TILENOISECALIBALG_H
