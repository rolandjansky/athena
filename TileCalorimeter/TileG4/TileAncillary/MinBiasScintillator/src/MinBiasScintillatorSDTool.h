/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class MinBiasScintillatorSDTool
// Tool for configuring the Sensitive detector for the Minimum Bias Scintillator
//
//************************************************************

#ifndef MINBIASSCINTILLATOR_MINBIASSCINTILLATORSDTOOL_H
#define MINBIASSCINTILLATOR_MINBIASSCINTILLATORSDTOOL_H

// Base class
#include "G4AtlasTools/SensitiveDetectorBase.h"

#include "MinBiasScintSDOptions.h"

class MinBiasScintillatorSDTool: public SensitiveDetectorBase {
  public:
    MinBiasScintillatorSDTool(const std::string& type, const std::string& name, const IInterface *parent);

    ~MinBiasScintillatorSDTool() {
    };

    /** End of an athena event */
    StatusCode Gather() override final; //FIXME would be good to be able to avoid this.

  protected:
    // Make me an SD!
    G4VSensitiveDetector* makeSD() override final;

  private:
    // Options for the SD configuration
    MinBiasScintSDOptions m_options;
};

#endif //MINBIASSCINTILLATOR_MINBIASSCINTILLATORSDTOOL_H
