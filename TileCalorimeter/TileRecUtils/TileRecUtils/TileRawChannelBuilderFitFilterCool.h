/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
    double m_minTime;
    double m_maxTime;
    double m_minTau;
    double m_maxTau;

    // Precalculated values for 1 iter
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

    double m_saturatedSampleError; // which error in terms of RMS is assigned to the saturated sample
    double m_zeroSampleError;  // which error in terms of RMS is assigned to the zero sample (== 0)
    double m_noiseThresholdRMS;     // for pedestal-like events only 2-parametric fit is applied
    double m_maxTimeFromPeak; // max.time for fitting the pulse shapes. Samples with t>t0+m_MaxTimeFromPeak are not fitted.

    double m_noiseLow;  // default low gain noise from TileInfo used in simulation
    double m_noiseHigh; // default high gain noise from TileInfo used in simulation

    // Pulse shapes
    TilePulseShapesStruct* m_pulseShapes;

    ToolHandle<TileCondToolPulseShape> m_tileToolPulseShape{this,
        "TileCondToolPulseShape", "TileCondToolPulseShape", "Tile pulse shape tool"};

    ToolHandle<TileCondToolPulseShape> m_tileToolLeak100Shape{this,
        "TileCondToolLeak100Shape", "", "Tile CIS leakage shape for 100 pF capacitor tool"};

    ToolHandle<TileCondToolPulseShape> m_tileToolLeak5p2Shape{this,
        "TileCondToolLeak5p2Shape", "", "Tile CIS leakage shape for 5.2 pF capacitor tool"};

    ToolHandle<TileCondToolPulseShape> m_tileToolPulse5p2Shape{this,
        "TileCondToolPulse5p2Shape", "", "Tile CIS pulse shape for 5.2 pF capacitor tool"};

    ToolHandle<TileCondToolNoiseSample> m_tileToolNoiseSample{this,
        "TileCondToolNoiseSample", "TileCondToolNoiseSample", "Tile sample noise tool"};

    TilePulseShapesStruct* m_shapes;

};

#define DTIME 25.0  // 25 ns distance between subsequent samples
#endif
