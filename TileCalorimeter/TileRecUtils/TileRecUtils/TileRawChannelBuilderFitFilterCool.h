/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILERECUTILS_TILERAWCHANNELBUILDERFITFILTERCOOL_H
#define TILERECUTILS_TILERAWCHANNELBUILDERFITFILTERCOOL_H

/********************************************************************
 *
 * NAME:     TileRawChannelBuilderFitFilterCool.h 
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
#include "GaudiKernel/IIncidentListener.h"
// Tile includes
#include "TileRecUtils/TileRawChannelBuilder.h"
#include "TileConditions/TilePulseShapes.h"
#include "TileConditions/TileCondToolPulseShape.h"
#include "TileConditions/TileCondToolNoiseSample.h"

#define PHYS_START_T_HI -75.5
#define PHYS_START_T_LO -75.5
#define PHYS_DT_HI 0.5
#define PHYS_DT_LO 0.5
#define CIS_START_T_HI -100.
#define CIS_START_T_LO -100.
#define SCIS_START_T_HI -72.
#define SCIS_START_T_LO -72.
#define CIS_DT_HI 2
#define LEAK_START_T_HI -100.
#define LEAK_START_T_LO -100.
#define SLEAK_START_T_HI -72.
#define SLEAK_START_T_LO -72.
#define CIS_DT_LO 2
#define LAS_START_T_HI -68.
#define LAS_START_T_LO -68.
#define LAS_DT_HI 1.8
#define LAS_DT_LO 1.8

class TileBeamInfoProvider;

class TileRawChannelBuilderFitFilterCool: public TileRawChannelBuilder
                                        , public IIncidentListener {
  public:

    // constructor
    TileRawChannelBuilderFitFilterCool(const std::string& type, const std::string& name,
        const IInterface *parent);
    // destructor
    ~TileRawChannelBuilderFitFilterCool();

    // virtual methods
    virtual StatusCode initialize();
    virtual StatusCode finalize();

    // Inherited from TileRawChannelBuilder
    virtual TileRawChannel * rawChannel(const TileDigits* digits);

    /**
     * AlgTool InterfaceID
     */
    static const InterfaceID& interfaceID();

    virtual void handle(const Incident&);

  private:

    void pulseFit(const TileDigits* digit, double &amplitude, double &time, double &pedestal, double &chi2);

    // generic functions for pulse interpolation
    double pulse(double x, const std::vector<double> * xvec, const std::vector<double> * yvec, bool zeroOutside = false) const;

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

    ToolHandle<TileCondToolPulseShape> m_tileToolPulseShape;
    ToolHandle<TileCondToolPulseShape> m_tileToolLeak100Shape;
    ToolHandle<TileCondToolPulseShape> m_tileToolLeak5p2Shape;
    ToolHandle<TileCondToolPulseShape> m_tileToolPulse5p2Shape;

    ToolHandle<TileCondToolNoiseSample> m_tileToolNoiseSample;
    TilePulseShapesStruct* m_shapes;

};

#define DTIME 25.0  // 25 ns distance between subsequent samples
#endif
