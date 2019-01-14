/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECALIBALG_TILELASERTIMINGTOOL_H
#define TILECALIBALG_TILELASERTIMINGTOOL_H

/**
 * class TileLaserTimingTool
 *
 * Algorithm tool for calculation of laser timing residuals, dskews and pipeline settings.
 *
 * Properties:
 *
 * RawChannelContainer - Input container
 * DigitsContainer     - Input container
 * NtupleID            - Target ID of ntuple in output tree
 * FiberLightSpeed     - Speed of light in laser fibers
 * NSamples            - Number of samples
 * EneLowLimitPulseShape - 
 * ReferenceDrawer     - Drawer to use as reference in each partition (default is 39, i.e. 40 if 1 is the first)
 * UseMeanChannelOffset- Use sample mean of timing difference histogram rather than the mean from a fitted gaussian
 * MaxTimeDiff         - Cutoff
 * UsePMT0AsDskewRef   - If true, use PMT 0 rather than drawer 0 as the timing offset within the drawer
 *
 * TODO:
 * - Clean up further, a lot of histograms may not be needed
 * - Calculate pipeline settings
 * - Take current dskew settings into account
 * - When things work ok, don't store arrays of even/odd time differences
 */

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "Identifier/HWIdentifier.h"
#include "StoreGate/ReadHandleKey.h"

// Tile includes
#include "TileCalibAlgs/ITileCalibTool.h"
#include "TileCalibAlgs/TileSimpleStat.h"
#include "TileConditions/TileCondToolTiming.h"
#include "TileConditions/TileCablingService.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileDigitsContainer.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"

#include <string> 
#include <map>
#include <sstream>

class TileHWID;
class TileRawChannelContainer;

class TFile;
class TF1;

#define TileLaserTimingMon
// #define TileLaserTimingMonPulse
// #defibe TileLaserTimingPMT0Mon

class TileLaserTimingTool: public AthAlgTool, virtual public ITileCalibTool {

  public:
    TileLaserTimingTool(const std::string& type, const std::string& name, const IInterface* pParent);
    virtual ~TileLaserTimingTool();

    // implementation of ITileCalibTool
    virtual StatusCode initialize() override;
    virtual StatusCode initNtuple(int runNumber, int runType, TFile * rootfile) override;
    virtual StatusCode execute() override;
    virtual StatusCode finalizeCalculations() override;
    virtual StatusCode writeNtuple(int runNumber, int runType, TFile * rootfile) override;
    virtual StatusCode finalize() override;

  private:
    /**
     * @class ADCData
     * @brief Data accumulated for one ADC
     * @TODO: different histo range for HI/LOW gain
     */
    struct PMTData {
        PMTData(const std::string &id);

        // time diff wrt pmt 0
        TH1F TimeDiffHisto;
        TH1F TimeDiffNoCFCorrHisto;

#ifdef TileLaserTimingMon
        TH1F timeHisto;
        TH1F eneHisto;
        TH1F gainHisto;
#endif

#ifdef TileLaserTimingPMT0Mon
        // High gain data, diff wrt pmt 0
        TH1F TimeDiffHistoHigh;
        // Low gain data, diff wrt pmt 0
        TH1F TimeDiffHistoLow;
#endif

#ifdef TileLaserTimingMonPulse
        TProfile pulseShapeHigh;
        TProfile pulseShapeLow;
#endif 

        // histogram for time residuals in inter module calib.
        // this is TFit corrected for fiber length and the mean time of digitizer 0
        TH1F TimeDiffResHis;

        // tmp variables
        double time; // abs time
#ifdef TileLaserTimingPMT0
        double dtime; // time diff
        double dtimeNoCFCorr;// time diff w.o. fiber correction
#endif
        double energy; // ev. energy

        // Pedestal mean and sigma, accumulated
        TileSimpleStat pedestal;
    };

    struct DigitizerData {
        DigitizerData();
        void Add(float time);
        int GetTotActive() const { return m_N_active; }
        float GetDigiTime() const { return m_mean_time; }
        float GetDigiTimeRMS() const { return m_mean_rms; }
        void Freeze();

      private:
        float m_ch_time[6];
        int m_N_active;
        float m_mean_time;
        float m_mean_rms;
    };

    struct DrawerData {
        DrawerData(const std::string &id);
        PMTData* pmtd[48];
        DigitizerData digid[8];

        // Time difference for Inter module timing
        // The difference between time in digitizer 0 in this
        // drawer and in the ref. drawer
        TH1F Digi0TimeDiffHisto;

#ifdef TileLaserTimingMon
        // Histo of mean time for digitizer 0
        TH1F Digi0Time;
#endif

        // digitizer 0 mean time (temporary variable)
        float FarDigitizerMeanTime;
    };

    DrawerData* m_drawerData[4][64];

    static const int NROS = 5;

    // ---- Ntuple data ----

    // mean of Gaussian fit to DrawerData.Digi0TimeDiffHisto
    float m_DrawerOffset[NROS][64];
    float m_DrawerOffsetError[NROS][64];

    // time diff wrt Digi0
    float m_ChannelOffset[NROS][64][48];
    float m_ChannelOffsetError[NROS][64][48];
    // Maximum ADC amplitude
    float m_ADCAmplitude[NROS][64][48];

    float m_PedestalMean[NROS][64][48];
    float m_PedestalSigma[NROS][64][48];

    // time diff wrt PMT0
    float m_TimeDiffMean[NROS][64][48];
    float m_TimeDiffMeanError[NROS][64][48];
    float m_TimeDiffSigma[NROS][64][48];

    // mon-->
    float m_MeanOddPmtTdiffPMT0[NROS][64];
    int m_OddPmtCounterPMT0[NROS][64];
    float m_MeanEvenPmtTdiffPMT0[NROS][64];
    int m_EvenPmtCounterPMT0[NROS][64];
    float m_EvenOddTimeDiffPMT0[NROS][64];
    // <-- mon

#ifdef TileLaserTimingPMT0Mon
    float m_TimeDiffHighMean[NROS][64][48];
    float m_TimeDiffHighMeanError[NROS][64][48];
    float m_TimeDiffHighSigma[NROS][64][48];

    float m_TimeDiffLowMean[NROS][64][48];
    float m_TimeDiffLowMeanError[NROS][64][48];
    float m_TimeDiffLowSigma[NROS][64][48];

    float m_TimeDiffNoCFCorrMean[NROS][64][48];
    float m_TimeDiffNoCFCorrMeanError[NROS][64][48];
    float m_TimeDiffNoCFCorrSigma[NROS][64][48];
#endif

    float m_FiberLength[NROS][64][48]; // Not needed

#ifdef TileLaserTimingMon
    float m_TimeDiffPMTDigi0[NROS][64][48]; // Not needed
    float m_FiberCorrection[NROS][64][48]; // Not needed
    int m_IsConnected[NROS][64][48]; // Not needed

    float m_MeanOddPmtTdiff[NROS][64];
    int m_OddPmtCounter[NROS][64];
    float m_MeanEvenPmtTdiff[NROS][64];
    int m_EvenPmtCounter[NROS][64];

    float m_EvenOddTimeDiff[NROS][64];
#endif

    float m_DSkewSet[NROS][64][8];
    float m_DigiMean[NROS][64][8];

    /** @return drawer data */
    inline DrawerData* drawerData(int ros, int drawer) {
      if (!m_drawerData[ros - 1][drawer]) {
        std::ostringstream sout;
        sout << ros << '_' << drawer;
        m_drawerData[ros - 1][drawer] = new DrawerData(sout.str());
      }
      return m_drawerData[ros - 1][drawer];
    }

    //StoreGate
    SG::ReadHandleKey<TileRawChannelContainer> m_RawChannelContainerKey{this,
      "TileRawChannelContainer", "TileRawChannelContainer", "Input Tile raw channel container"};
    SG::ReadHandleKey<TileDigitsContainer> m_DigitsContainerKey{this,
      "TileDigitsContainer", "TileDigitsContainer", "Input Tile digits container"};

    // Tools info
    const TileHWID* m_tileHWID;
    const TileCablingService* m_cabling;
    ToolHandle<TileCondToolTiming> m_tileToolTiming;

    // jobOptions
    std::string m_rawChannelContainerName;
    std::string m_digitsContainerName;
    std::string m_ntupleID;
    double m_fiberLightSpeed;
    unsigned m_nSamples;
    // low limit for energy when adding samples to pulse shape
    double m_eneLowLimitPulseShape;
    // the reference drawer nr
    unsigned int m_refDrawer;
    bool m_useMeanChannelOffset;
    // max allowed abs value for time diff
    float m_maxTimeDiff;
    // if PMT0 or digi0 should be used as reference to dskew settings
    bool m_usePMT0AsDskewRef;

    // the tag for the registered cond objects, default ""
    std::string m_IOVTag;

    // help vars

    /** number of laser events */
    int m_nevts;

    TF1* m_gaussf;

#ifdef TileLaserTimingMonPulse
    // Mon data
    TH1F* m_h1_time_check;
    TH2F* m_h2_time_check;

    TProfile* m_PulseShapeHigh;
    TProfile* m_PulseShapeLow;
#endif

    // fiber lengths
    // first index is barrel(0) and extended barrel(1)
    // second index is PMT-1
    static float s_fiberLength[2][48];

    /** @return if channel is connected */
    inline bool isConnectedChan(int ros, int chan) {
      if (m_cabling->channel2hole(ros, chan) < 0) return false;
      return true;
    }
    /** @return if a PMT is connected */
    bool isConnectedPMT(int ros, int chan);

    /** @return PMT#-1 for given ros and channel */
    inline int channel2PMT(int ros, int chan) {
      return (abs(m_cabling->channel2hole(ros, chan)) - 1);
    }
    /** @return if ros is the barrel, LBA or LBC */
    static inline bool isBarrel(int ros) {
      return ros == 1 || ros == 2;
    }
    /** @return if ros is the extended barrel, EBA or EBC */
    static inline bool isExtBarrel(int ros) {
      return ros == 3 || ros == 4;
    }
    /** @return timing correction for fiber length, for given PMT */
    inline double fiberCorrection(int ros, int pmt) {
      return (s_fiberLength[isExtBarrel(ros)][pmt] - s_fiberLength[isExtBarrel(ros)][0])
          / m_fiberLightSpeed;
    }
    static inline float fiberLength(int ros, int pmt) {
      return s_fiberLength[isExtBarrel(ros)][pmt];
    }
    /** @return HWIdentifier for fragment */
    inline HWIdentifier drawerId(int frag) {
      return m_tileHWID->drawer_id(frag);
    }
    /** @return string for fragment id */
    inline std::string drawerIdStr(int frag) {
      return m_tileHWID->to_string(drawerId(frag));
    }
    /** @return HWIdentifier for ros and drawer */
    inline HWIdentifier drawerId(int ros, int drawer) {
      return m_tileHWID->drawer_id(ros, drawer);
    }

    /**
     * Fit a Gaussian function (m_gaussf) to the given histogram, store mean, rms and mean error
     */
    void fitGauss(TH1F &hi, float &p1, float &p2, float &p1_err);

    /**
     * Fit a Gaussian function with two iterations
     * mean range = hi.mean +- w1|w2 * hi.rms
     */
    void fitGauss2(TH1F &hi, float &p1, float &p2, float &chi2, float &p1_err, float w1, float w2);

    typedef float TPMTArray[5][64][48];
    typedef float TDrawerArrayF[5][64];
    typedef int TDrawerArrayI[5][64];

    /**
     * Correct for differences between even and odd PMTs
     */
    void correctEvenOddPMT(int ros, int drawer, TPMTArray& ChannelOffset,
        TDrawerArrayF& MeanOddPmtTdiff, TDrawerArrayI& OddPmtCounter,
        TDrawerArrayF& MeanEvenPmtTdiff, TDrawerArrayI& EvenPmtCounter,
        TDrawerArrayF& EvenOddTimeDiff);
};

#endif // TILECALIBALG_TILELASERTIMINGTOOL_H
