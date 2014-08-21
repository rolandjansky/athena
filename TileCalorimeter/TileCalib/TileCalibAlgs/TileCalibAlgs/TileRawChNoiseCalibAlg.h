/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

// Tile includes
#include "TileConditions/TileCablingService.h"
#include "TileConditions/TileCondIdTransforms.h"
#include "TileConditions/TileCondToolEmscale.h"
#include "TileConditions/ITileBadChanTool.h"

#include <cmath>
#include <vector>
#include <string>
#include <map>
#include <stdint.h>

class TileHWID;
class TileBeamInfoProvider;
class TileBeamElemContByteStreamCnv;
class TileDQstatus;
class TileRawChannel;
class TH1F;

class TileRawChNoiseCalibAlg: public AthAlgorithm {

  public:
    TileRawChNoiseCalibAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~TileRawChNoiseCalibAlg();

    // Functions
    StatusCode initialize(); //only array initialization is done here
    StatusCode execute();
    StatusCode finalize();
    void finalRawCh(int rctype);

  private:

    enum RCtype {
      Fit = 0, Fixed = 1, Opt = 2, Dsp = 3, RCnum = 4
    };

    enum trigtype {
      Unknown = 0, Phys = 1, Las = 2, Ped = 4, Cis = 8
    };

    StatusCode FirstEvt_initialize(); // real initialization is done in this method

    StatusCode fillRawChannels(std::string rcCnt, RCtype rctype); // raw chans variables is done here
    void StoreRunInfo(); // called only at the first event. General variables
    void removeRC(RCtype rctype); // if a RawChannel container doesn't exist, it is removed from the list

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
    bool m_saveHist;
    bool m_invertChanRatio;

    int m_UseforCells;

    bool m_maskBadChannels;

    // by default we use channels
    bool m_usePMT;

    bool m_fillidx;

    // Tools / storegate info
    ToolHandle<TileBeamInfoProvider> m_beamInfo;
    TileBeamElemContByteStreamCnv* m_beamCnv;
    TileBeamInfoProvider* m_beamPrv;
    const TileCablingService* m_cabling;
    // Identifiers
    const TileID* m_tileID;
    const TileHWID* m_tileHWID;
    ToolHandle<TileCondToolEmscale> m_tileToolEmscale;
    ToolHandle<ITileBadChanTool> m_tileBadChanTool;
    ToolHandle<TileCondIdTransforms> m_tileIdTrans;
    const uint32_t* m_cispar;

    // jobOptions

    // Name of ntuple and file
    std::string m_ntupleID;
    std::string m_file;
    int64_t m_treeSize;
    unsigned int m_nDrawers;

    // Container Parameters
    std::string m_digitsContainer;
    std::string m_beamElemContainer;
    std::string m_fixedRawChannelContainer;
    std::string m_fitRawChannelContainer;
    std::string m_optRawChannelContainer;
    std::string m_dspRawChannelContainer;

    TH1F* histAmp[RCnum][5][64][48][2];
    TH1F* histCellAmp[2][64][4][17][6];

    // event number
    int m_evtNr;
    int m_evt[5][64][48][2];
    uint8_t m_ros[5][64][48][2];
    uint8_t m_drawer[5][64][48][2];
    uint8_t m_channel[5][64][48][2];
    bool m_gain[5][64][48][2];
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
    float rc_mean[RCnum][5][64][48][2];
    float rc_sigma[RCnum][5][64][48][2];
    float rc_av[RCnum][5][64][48][2];
    float rc_rms[RCnum][5][64][48][2];
    float rc_skewness[RCnum][5][64][48][2];
    float rc_kurtosis[RCnum][5][64][48][2];
    float rc_mean_err[RCnum][5][64][48][2];
    float rc_sigma_err[RCnum][5][64][48][2];
    float rc_chi2[RCnum][5][64][48][2];
    float rc_ndf[RCnum][5][64][48][2];
    float rc_probC2[RCnum][5][64][48][2];

    float rc_ggpar[RCnum][5][64][48][2][8]; // sigma gauss1, rel normalization of the gaussians, sigma gauss2, chi2/ndf, error in sigma1, sigma2 and normalization, correlation between sigma1 and sigma2
    float rc_gsigma1[RCnum][5][64][48][2];
    float rc_gsigma2[RCnum][5][64][48][2];
    float rc_gnorm[RCnum][5][64][48][2];
    float rc_gchi2[RCnum][5][64][48][2];
    float rc_gerrsigma1[RCnum][5][64][48][2];
    float rc_gerrsigma2[RCnum][5][64][48][2];
    float rc_gerrnorm[RCnum][5][64][48][2];
    float rc_gcorrsigma1sigma2[RCnum][5][64][48][2];

    //Cell items
    //structure is side:             A=0 C=1,
    //             drawer:           0-63,
    //		 sample:           A=0, BC=1, D=2, E=3
    //             tower:            0-16,
    //             gain combination: LGLG, LG-, -LG, HGHG, HG-, -HG
    bool m_side[2][64][4][17][6];
    uint8_t m_phi[2][64][4][17][6];
    uint8_t m_sample[2][64][4][17][6];
    uint8_t m_tower[2][64][4][17][6];
    uint8_t m_gg[2][64][4][17][6];

    float ecell_av[2][64][4][17][6];
    float ecell_rms[2][64][4][17][6];
    uint32_t ecell_hash[2][64][4][17];

    int cell_nch[2][64][4][17][2]; // number of good channels in given cell in given event - 0 or 1 or 2
    float ecell_ene[2][64][4][17][2];
    float ggpar[2][64][4][17][6][8]; // sigma gauss1, rel normalization of the gaussians, sigma gauss2, chi2/ndf, error in sigma1, sigma2 and normalization, correlation between sigma1 and sigma2
    float gsigma1[2][64][4][17][6];
    float gsigma2[2][64][4][17][6];
    float gnorm[2][64][4][17][6];
    float gchi2[2][64][4][17][6];
    float gerrsigma1[2][64][4][17][6];
    float gerrsigma2[2][64][4][17][6];
    float gerrnorm[2][64][4][17][6];
    float gcorrsigma1sigma2[2][64][4][17][6];

};

#endif // TILENOISECALIBALG_H
