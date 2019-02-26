/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "xAODEventInfo/EventInfo.h"
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

#define NDIODES 10
#define NDIODES_LASER1 4
#define NMONITORS 4
#define NPMTS 2

#define NGAINS 2
#define NPARTITIONS 4 
#define NDRAWERS 64
#define NCHANNELS 48
#define NDIGI 8

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
        PMTData* pmtd[NCHANNELS];
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

    DrawerData* (*m_drawerData)[NDRAWERS];

    static const int NROS = 5;

    // ---- Ntuple data ----

    // mean of Gaussian fit to DrawerData.Digi0TimeDiffHisto
    float (*m_DrawerOffset)[NDRAWERS];
    float (*m_DrawerOffsetError)[NDRAWERS];

    // time diff wrt Digi0
    float (*m_ChannelOffset)[NDRAWERS][NCHANNELS];
    float (*m_ChannelOffsetError)[NDRAWERS][NCHANNELS];
    // Maximum ADC amplitude
    float (*m_ADCAmplitude)[NDRAWERS][NCHANNELS];

    float (*m_PedestalMean)[NDRAWERS][NCHANNELS];
    float (*m_PedestalSigma)[NDRAWERS][NCHANNELS];

    // time diff wrt PMT0
    float (*m_TimeDiffMean)[NDRAWERS][NCHANNELS];
    float (*m_TimeDiffMeanError)[NDRAWERS][NCHANNELS];
    float (*m_TimeDiffSigma)[NDRAWERS][NCHANNELS];

    // mon-->
    float (*m_MeanOddPmtTdiffPMT0)[NDRAWERS];
    int (*m_OddPmtCounterPMT0)[NDRAWERS];
    float (*m_MeanEvenPmtTdiffPMT0)[NDRAWERS];
    int (*m_EvenPmtCounterPMT0)[NDRAWERS];
    float (*m_EvenOddTimeDiffPMT0)[NDRAWERS];
    // <-- mon

#ifdef TileLaserTimingPMT0Mon
    float (*m_TimeDiffHighMean)[NDRAWERS][NCHANNELS];
    float (*m_TimeDiffHighMeanError)[NDRAWERS][NCHANNELS];
    float (*m_TimeDiffHighSigma)[NDRAWERS][NCHANNELS];

    float (*m_TimeDiffLowMean)[NDRAWERS][NCHANNELS];
    float (*m_TimeDiffLowMeanError)[NDRAWERS][NCHANNELS];
    float (*m_TimeDiffLowSigma)[NDRAWERS][NCHANNELS];

    float (*m_TimeDiffNoCFCorrMean)[NDRAWERS][NCHANNELS];
    float (*m_TimeDiffNoCFCorrMeanError)[NDRAWERS][NCHANNELS];
    float (*m_TimeDiffNoCFCorrSigma)[NDRAWERS][NCHANNELS];
#endif

    float (*m_FiberLength)[NDRAWERS][NCHANNELS]; // Not needed

#ifdef TileLaserTimingMon
    float (*m_TimeDiffPMTDigi0)[NDRAWERS][NCHANNELS]; // Not needed
    float (*m_FiberCorrection)[NDRAWERS][NCHANNELS]; // Not needed
    int (*m_IsConnected)[NDRAWERS][NCHANNELS]; // Not needed

    float (*m_MeanOddPmtTdiff)[NDRAWERS];
    int (*m_OddPmtCounter)[NDRAWERS];
    float (*m_MeanEvenPmtTdiff)[NDRAWERS];
    int (*m_EvenPmtCounter)[NDRAWERS];

    float (*m_EvenOddTimeDiff)[NDRAWERS];
#endif

    float (*m_DSkewSet)[NDRAWERS][NDIGI];
    float (*m_DigiMean)[NDRAWERS][NDIGI];

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
    SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this,
      "EventInfo", "EventInfo", "EventInfo key"};
    SG::ReadHandleKey<TileRawChannelContainer> m_rawChannelContainerKey{this,
      "TileRawChannelContainer", "TileRawChannelFit", "Tile raw channel container"};
    SG::ReadHandleKey<TileDigitsContainer> m_digitsContainerKey{this,
      "TileDigitsContainer", "TileDigitsCnt", "Tile digits container"};

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

    typedef float (*TPMTArray)[NDRAWERS][NCHANNELS];
    typedef float (*TDrawerArrayF)[NDRAWERS];
    typedef int (*TDrawerArrayI)[NDRAWERS];

    /**
     * Correct for differences between even and odd PMTs
     */
    void correctEvenOddPMT(int ros, int drawer, TPMTArray& ChannelOffset,
        TDrawerArrayF& MeanOddPmtTdiff, TDrawerArrayI& OddPmtCounter,
        TDrawerArrayF& MeanEvenPmtTdiff, TDrawerArrayI& EvenPmtCounter,
        TDrawerArrayF& EvenOddTimeDiff);
};

#endif // TILECALIBALG_TILELASERTIMINGTOOL_H
