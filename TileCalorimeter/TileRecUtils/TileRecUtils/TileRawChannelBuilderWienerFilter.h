/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILERECUTILS_TILERAWCHANNELBUILDERWIENERFILTER_H
#define TILERECUTILS_TILERAWCHANNELBUILDERWIENERFILTER_H

//////////////////////////////////////////////////////////////////////
//
//     AUTHOR :  G. Goncalves <ginaciog@cern.ch>
//     CREATED:  May 2019
//
//     TileRawChannelBuilderWienerFilter.h
//
//     implementation of the Wiener Filter for
//          energy/time reconstruction in TileCal
//
//////////////////////////////////////////////////////////////////////

// Tile includes
#include "TileRecUtils/TileRawChannelBuilder.h"
#include "TileConditions/ITileCondToolOfc.h"
#include "TileConditions/TileCondToolTiming.h"
#include "TileConditions/TileCondToolNoiseSample.h"

// Atlas includes
#include "TileConditions/TileWienerFilterWeights.h"
#include "TrigAnalysisInterfaces/IBunchCrossingTool.h"

#include <vector>
#include <string>

/**
 *
 * @class TileRawChannelBuilderWienerFilter
 * @brief Reconstructs Tile digitized pulses (ie, computes amplitude, time and pedestal) as a linear combination of the samples
 *
 * This class implements an energy reconstruction method known as Wiener
 * Filter. It takes as input the digital samples from the digitizer boards
 * in the front-end electronics and outputs the amplitude, time and pedestal
 * of the pulse. Full details and fundaments of the method can be found in
 * the ATL-TILECAL-PROC-2019-002 note. Two different versions of the algorithms
 * are currently used: Wiener Optimal (with uses seven sets of weights, optimized
 * for the first three, the middle and the last three BCIDs in the train) and
 * Wiener General (one set of weights for all BCIDs).
 */
class TileRawChannelBuilderWienerFilter: public TileRawChannelBuilder {
  public:

    TileRawChannelBuilderWienerFilter(const std::string& type, const std::string& name,
        const IInterface *parent); //!< Constructor
    ~TileRawChannelBuilderWienerFilter(); //!< Destructor

    // virtual methods
    virtual StatusCode initialize(); //!< Initialize method
    //virtual StatusCode execute();
    virtual StatusCode finalize(); //!< Finalize method

    // Inherited from TileRawChannelBuilder
    virtual TileRawChannel* rawChannel(const TileDigits* digits);

    /**
     * AlgTool InterfaceID
     */
    static const InterfaceID& interfaceID();

  private:

    ToolHandle<TileCondToolNoiseSample> m_tileToolNoiseSample{this,
        "TileCondToolNoiseSample", "TileCondToolNoiseSample", "Tile noise sample tool"};

    ToolHandle<Trig::IBunchCrossingTool> m_bunchCrossingTool;

    //!< Applies OF algorithm
    double filter(int ros, int drawer, int channel, int &gain, double &pedestal, double &amplitude, double &time, const EventContext &ctx);
    int findMaxDigitPosition();  //!< Finds maximum digit position in the pulse
    //!< Gets pedestal estimation for OF1
    float getPedestal(int ros, int drawer, int channel, int gain, const EventContext &ctx);
    //!< Computes A,time,ped using OF. If iterations are required, the Iterator method is used
    double compute(int ros, int drawer, int channel, int gain, double &pedestal, double &amplitude, double &time, double& phase);
    //!< Gets the BCID index within the train
    int getBCIDIndex();

    int m_maxIterations; //!< maximum number of iteration to perform
    int m_pedestalMode;  //!< pedestal mode to use
    bool m_confTB;       //!< use testbeam configuration
    double m_timeForConvergence; //!< minimum time difference to quit iteration procedure
    bool m_isMC;    //!< bool variable for MC: true=> MC;  false=> data
    bool m_minus1Iter;   //!< bool variable for whether to apply -1 iteration (initial phase guess)
    bool m_correctAmplitude; //!< If true, resulting amplitude is corrected when using weights for tau=0 without iteration
    bool m_correctTimeNI; //!< If true, resulting time is corrected when using method  without iteration

    bool m_bestPhase; // if true, use best phase from COOL DB in "fixed phase" mode (i.e., no iterations)
    bool m_emulateDsp; // if true, emulate DSP reconstruction algorithm
    int m_nSignal; //!< internal counters
    int m_nNegative;  //!< internal counters
    int m_nCenter; //!< internal counters
    int m_nConst;  //!< internal counters

    int m_nSamples; //!< number of samples in the data
    int m_t0SamplePosition;  //!< position of peak sample = (m_nSamples-1)/2
    double m_maxTime; //!< max allowed time = 25*(m_nSamples-1)/2
    double m_minTime; //!< min allowed time = -25*(m_nSamples-1)/2

    TileWienerFilterWeightsStruct *m_weights; //!< structure for Wiener Filter weights

    std::vector<float> m_digits;
};

#endif
