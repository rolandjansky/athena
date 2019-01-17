/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILERECUTILS_TILERAWCHANNELBUILDEROPT2FILTER_H
#define TILERECUTILS_TILERAWCHANNELBUILDEROPT2FILTER_H

//////////////////////////////////////////////////////////////////////
//
//     Based on the code of Ximo Poveda@cern.ch. June 2007
//     Andrei.Artamonov@cern.ch, July 2008
//
//     TileRawChannelBuilderOpt2Filter.h
//
//     implementation of the Optimal Filtering based on Lagrange multipliers
//       for energy/time reconstruction in TileCal 
//
//////////////////////////////////////////////////////////////////////

// Tile includes
#include "TileRecUtils/TileRawChannelBuilder.h"
#include "TileConditions/ITileCondToolOfc.h"
#include "TileConditions/TileCondToolTiming.h"
#include "TileConditions/TileCondToolNoiseSample.h"

#include <vector>
#include <string>

/**
 *
 * @class TileRawChannelBuilderOpt2Filter
 * @brief Reconstructs Tile digitized pulses (ie, computes amplitude, time and pedestal) as a linear combination of the samples
 *
 * This class implements an energy reconstruction method known as Optimal
 * Filtering. It takes as input the digital samples from the digitizer boards
 * in the front-end electronics and outputs the amplitude, time and pedestal
 * of the pulse. Full details and fundaments of the method can be found in
 * the ATL-TILECAL-2005-001 note. Two different versions of the algorithms
 * are currently used: OF1 (with 2 parameters: amplitude and time) and OF2
 * (with 3 parameters (amplitude, time and pedestal).
 *
 * OFCs are calculated on-the-fly (using TileCondToolOfc) or are extracted
 * from COOL database (TileCondToolOfcCool). In case of non-iterative
 * procedure, optionally, the initial, "best phase", can be extracted
 * from COOL DB by means of TileCondToolTiming.
 */
class TileRawChannelBuilderOpt2Filter: public TileRawChannelBuilder {
  public:

    TileRawChannelBuilderOpt2Filter(const std::string& type, const std::string& name,
        const IInterface *parent); //!< Constructor
    ~TileRawChannelBuilderOpt2Filter(); //!< Destructor

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

    ToolHandle<ITileCondToolOfc> m_tileCondToolOfc{this,
        "TileCondToolOfc", "TileCondToolOfc", "Tile OFC tool"};

    ToolHandle<TileCondToolNoiseSample> m_tileToolNoiseSample{this,
        "TileCondToolNoiseSample", "TileCondToolNoiseSample", "Tile noise sample tool"};

    //!< Applies OF algorithm
    double filter(int ros, int drawer, int channel, int &gain, double &pedestal, double &amplitude, double &time);
    int findMaxDigitPosition();  //!< Finds maximum digit position in the pulse
    //!< Gets pedestal estimation for OF1
    float getPedestal(int ros, int drawer, int channel, int gain);
    //!< Apply the number of iterations needed for reconstruction by calling the Filter method
    int iterate(int ros, int drawer, int channel, int gain, double &pedestal, double &amplitude, double &time, double &chi2);
    //!< Computes A,time,ped using OF. If iterations are required, the Iterator method is used
    double compute(int ros, int drawer, int channel, int gain, double &pedestal, double &amplitude, double &time, double& phase);

    void ofc2int(int nDigits, double* w_off, short* w_int, short& scale); // convert weights to dsp short int format

    int m_maxIterations; //!< maximum number of iteration to perform
    int m_pedestalMode;  //!< pedestal mode to use
    bool m_confTB;       //!< use testbeam configuration
    double m_timeForConvergence; //!< minimum time difference to quit iteration procedure
    bool m_of2;    //!< bool variable for OF method: true=> OF2;  false=> OF1
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


    std::vector<float> m_digits;

    int m_noiseThresholdHG;
    int m_noiseThresholdLG;
};

#endif
