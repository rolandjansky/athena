/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETRESOLUTION_JERDEFS_H
#define JETRESOLUTION_JERDEFS_H

/// @file This header contains general definitions for JetResolution software

namespace JER
{

  struct ROOT6_OpenNamespaceWorkaround { };  // workaround for reflex dict generation

  /// Jet algorithms supported by the JER tools
  enum JetAlg
  {
    JETALG_UNDEFINED = -1,
    AKt4EM = 0,
    // AKt4LC,
    //AKt6EM,
    //AKt6LC,
    JETALG_N
  };

  /// JER insitu uncertainties
  /*  enum Uncert
  {
    UNCERT_UNDEFINED = -1,
    JER_NP0 = 0,
    JER_NP1,
    JER_NP2,
    JER_NP3,
    JER_NP4,
    JER_NP5,
    JER_NP6,
    JER_NP7,
    JER_NP8,
    JER_NOISE_FORWARD, //9
    JER_xCALIB_50nsVs25ns, //10
    JER_xCALIB_5Vs4sample, //11
    JER_xCALIB_TopoClustering, //12
    JER_xCALIB_EarlyData, //13
    JER_xCALIB_NoiseThreshold, //14
    JER_xCALIB_UE, //15
    JER_xCALIB_PhiModulation, //16
    JER_NP_ALL, // All NPs combined in quadrature
    UNCERT_N
  };*/

  //JER NPs reduced to 9 inclusing xCalibrations
  enum Uncert
  {
    UNCERT_UNDEFINED = -1,
    JER_NP0 = 0,
    JER_NP1,
    JER_NP2,
    JER_NP3,
    JER_NP4,
    JER_NP5,
    JER_NP6,
    JER_NP7,
    JER_NP8,
    JER_NOISE_FORWARD, //9
    JER_CROSS_CALIB_50nsVs25ns,//10
    JER_CROSS_CALIB_5Vs4sample,
    JER_CROSS_CALIB_TopoClustering,
    JER_CROSS_CALIB_EarlyData,
    JER_CROSS_CALIB_NoiseThreshold,
    JER_CROSS_CALIB_UnderlyingEvent,
    JER_CROSS_CALIB_PhiModulation,//16
    JER_NP_ALL, //17 All NPs combined in quadrature
    JER_CROSS_CALIB_ALL, //18  All xCalib NPs combined in quadrature
    UNCERT_N
  };


  namespace units
  {
    const double GeV = 1.e3;
    const double TeV = 1.e6;
    const double invGeV = 1.e-3;
    const double invTeV = 1.e-6;
  }

}

#endif // JETRESOLUTION_JERDEFS_H
