/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RunOptions
// 30-Jun-2003 Bill Seligman

#ifndef LArG4_RunOptions_H
#define LArG4_RunOptions_H

// The LAr simulation has a number of user-specified options.  This
// class stores and retrives the options specified by the user.  For
// now, this routine is limited to those options which are in common
// to the stand-alone and Athena versions of LArG4Model; for example, the
// physics-list option is not in this class, because it's a LArG4
// option in stand-alone mode, but a G4Svc option in Athena mode.

// 21-Jul-2004 WGS: Add out-of-time cut option.

#include "LArG4Code/OptionsStore.h"

#include <map>
#include <string>


namespace LArG4 {

    // Define all the user options.  Note that the simulation main
    // routines (e.g., LArG4/LArG4Algs/src/LArG4Run.cxx and
    // LArG4/LArG4App/LArG4App.cc) are responsible for
    // supplying the default values for all these options.  (In
    // Athena, the default values are supplied by the jobOptions
    // service; in stand-alone, they're supplied through the
    // ParseOptions class.)

    enum ERunOptions {

      // Options that related to calibration studies
      kCalibrationRun , // Whether this is a calibration run

      // Options that relate to the fast shower model
      kEmbFastShowerModel ,  // Whether to use a fast-shower-model in the EMB
      kEmecFastShowerModel , // Whether to use a fast-shower-model in the EMEC
      kFcalFastShowerModel , // Whether to use a fast-shower-model in the FCAL
      kHecFastShowerModel ,  // Whether to use a fast-shower-model in the HEC
      kBuildFromNova,        // Whether to build parameters from NOVA database.
      kOutOfTimeCut          // Out-of-time cut applied to energy deposits in hits
    };

  // Define the class that will store the run options.  In this case,
  // the class will be LArG4::RunOptions.

  class RunOptions : public LArG4::OptionsStore<ERunOptions>
  {
  public:
    virtual ~RunOptions() {;}

    // Standard setup for the singleton design pattern.
    static RunOptions* GetInstance()
    {
      static RunOptions s_instance;
      return &s_instance;
    }

  };


} // namespace LArG4


#endif // LArG4_RunOptions_H
