/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILERECUTILS_TILERAWCHANNELBUILDERFITFILTER_H
#define TILERECUTILS_TILERAWCHANNELBUILDERFITFILTER_H

/********************************************************************
 *
 * NAME:     TileRawChannelBuilderFitFilter.h 
 * PACKAGE:  offline/TileCalorimeter/TileRecUtils
 *
 * AUTHOR :  K. Gellerstedt, Algorithm by Richard John Teuscher
 * CREATED:  Dec 03 2003
 *
 * PURPOSE:  Build TileRawChannels from digits using fitting
 *
 *  Input: TileDigitsContainer
 *  Output: TileRawChannelContainer
 *  Parameters: TileRawChannelCont - Name of output container in SG
 *    FrameLength
 *    MaxIterate
 *    RMSChannelNoise
 ********************************************************************/

// Tile includes
#include "TileRecUtils/TileRawChannelBuilder.h"
#include "TileConditions/TilePulseShapes.h"
#include "TileConditions/TileCondToolNoiseSample.h"

class TileBeamInfoProvider;

class TileRawChannelBuilderFitFilter: public TileRawChannelBuilder {
  public:

    // constructor
    TileRawChannelBuilderFitFilter(const std::string& type, const std::string& name,
        const IInterface *parent);
    // destructor
    ~TileRawChannelBuilderFitFilter();

    // virtual methods
    virtual StatusCode initialize();
    virtual StatusCode finalize();

    // Inherited from TileRawChannelBuilder
    virtual TileRawChannel * rawChannel(const TileDigits* digits);

    /**
     * AlgTool InterfaceID
     */
    static const InterfaceID& interfaceID();

  private:

    void pulseFit(const TileDigits *digit, double &amplitude, double &time, double &pedestal, double &chi2);

    // generic functions for pulse interpolation
    double pulse(double x, const std::vector<double> * xvec, const std::vector<double>* yvec, bool zeroOutside = false) const;

    double scaledPulse(double x, const std::vector<double> * xvec, const std::vector<double> * yvec) const {
      return m_fnParameters[1] + m_fnParameters[2] * pulse(x, xvec, yvec, false);
    }

    double derivative(double x, const std::vector<double> * xvec, const std::vector<double> * yvec) const {
      return pulse(x, xvec, yvec, true);
    }

    std::vector<double> m_dummy; // dummy vector which can be passed to the functions above

    // Pulse shape function variables
    double m_t0Fit;
    // Parameters for pulse shape functions
    // [0] - phase, [1] - pedestal, [2] - amplitude
    double m_fnParameters[3];
    int m_iPeak0;
    //double m_phasefrac;
    double m_minTime;
    double m_maxTime;
    double m_minTau;
    double m_maxTau;

    // Precalculated values for 1 iteration
    std::vector<double> m_gPhysLo;
    std::vector<double> m_dgPhysLo;
    std::vector<double> m_gPhysHi;
    std::vector<double> m_dgPhysHi;

    // Parameters
    int m_frameLength;
    int m_channelNoiseRMS;
    int m_maxIterate;
    int m_extraSamplesLeft;
    int m_extraSamplesRight;

    double m_saturatedSample; // defines the saturated sample
    double m_saturatedSampleError; // which error in terms of RMS is assigned to the saturated sample
    double m_zeroSampleError;  // which error in terms of RMS is assigned to the zero sample (== 0)
    double m_noiseThresholdRMS;     // for pedestal-like events only 2-parametric fit is applied
    double m_maxTimeFromPeak; // max.time for fitting the pulse shapes. Samples with t>t0+m_MaxTimeFromPeak are not fitted.

    double m_noiseLow;  // default low gain noise from TileInfo used in simulation
    double m_noiseHigh; // default high gain noise from TileInfo used in simulation

    // Pulse shapes
    TilePulseShapesStruct* m_pulseShapes;

    bool m_disableNegativeAmp;

    ToolHandle<TileCondToolNoiseSample> m_tileToolNoiseSample{this,
        "TileCondToolNoiseSample", "TileCondToolNoiseSample", "Tile sample noise tool"};

};

#define DTIME 25.0  // 25 ns distance between subsequent samples
#endif
