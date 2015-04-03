/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

    double scaledpulse(double x, const std::vector<double> * xvec, const std::vector<double> * yvec) const {
      return m_fnpar[1] + m_fnpar[2] * pulse(x, xvec, yvec, false);
    }

    double deriv(double x, const std::vector<double> * xvec, const std::vector<double> * yvec) const {
      return pulse(x, xvec, yvec, true);
    }

    std::vector<double> m_dummy; // dummy vector which can be passed to the functions above

    // Pulse shape function variables
    double m_t0fit;
    double m_fnpar[3];
    int m_ipeak0;
    //double m_phasefrac;
    double m_min_time;
    double m_max_time;
    double m_min_tau;
    double m_max_tau;

    // Precalculated values for 1 iter
    std::vector<double> m_gphyslo;
    std::vector<double> m_dgphyslo;
    std::vector<double> m_gphyshi;
    std::vector<double> m_dgphyshi;

    // Parameters
    int m_frameLength;
    int m_RMSChannelNoise;
    int m_maxIterate;
    int m_extraSamplesLeft;
    int m_extraSamplesRight;

    double m_SaturatedSampleError; // which error in terms of RMS is assigned to the saturated sample
    double m_ZeroSampleError;  // which error in terms of RMS is assigned to the zero sample (== 0)
    double m_NoiseThresholdRMS;     // for pedestal-like events only 2-parametric fit is applied
    double m_MaxTimeFromPeak; // max.time for fitting the pulse shapes. Samples with t>t0+m_MaxTimeFromPeak are not fitted.

    double m_noiseLow;  // default low gain noise from TileInfo used in simulation
    double m_noiseHigh; // default high gain noise from TileInfo used in simulation

    // Pulse shapes
    TilePulseShapesStruct* m_pulsevar;
};

#define DTIME 25.0  // 25 ns distance between subsequent samples
#endif
