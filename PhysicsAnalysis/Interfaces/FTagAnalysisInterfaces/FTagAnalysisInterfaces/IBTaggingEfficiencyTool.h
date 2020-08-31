// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IBTaggingEfficiencyTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef CPIBTAGGINGEFFICIENCYTOOL_H
#define CPIBTAGGINGEFFICIENCYTOOL_H

#include "AsgTools/IAsgTool.h"
#include "PATInterfaces/ISystematicsTool.h"
#include "PATInterfaces/CorrectionCode.h"
#include "PATInterfaces/SystematicCode.h"
#include "PATInterfaces/SystematicSet.h"

#include "xAODJet/Jet.h"

#include <vector>
#include <string>
#include <set>

#include "CalibrationDataInterface/CalibrationDataVariables.h"
#include "CalibrationDataInterface/CalibrationDataInterfaceROOT.h"

class IBTaggingEfficiencyTool : virtual public CP::ISystematicsTool {

  /// Declare the interface that the class provides
  ASG_TOOL_INTERFACE( IBTagEfficiencyTool )

  public:


  virtual CP::CorrectionCode getScaleFactor( const xAOD::Jet & jet,
               float & sf) const =0 ;

  virtual CP::CorrectionCode getEfficiency( const xAOD::Jet & jet,
              float & eff) const=0;

  virtual CP::CorrectionCode getInefficiency( const xAOD::Jet & jet,
                float & eff) const=0;

  virtual CP::CorrectionCode getInefficiencyScaleFactor( const xAOD::Jet & jet,
              float & sf) const=0;

  virtual CP::CorrectionCode getMCEfficiency( const xAOD::Jet & jet,
                float & eff) const=0;


  virtual CP::CorrectionCode getScaleFactor( int flavour, const Analysis::CalibrationDataVariables& v,
             float & sf) const=0;

  virtual CP::CorrectionCode getMCEfficiency( int flavour, const Analysis::CalibrationDataVariables& v,
              float & eff) const=0;

  virtual CP::CorrectionCode getInefficiencyScaleFactor( int flavour, const Analysis::CalibrationDataVariables& v,
             float & sf) const=0;


  // utility methods
  virtual const std::map<CP::SystematicVariation, std::vector<std::string> > listSystematics() const = 0;
  //
  virtual std::string getTaggerName() const = 0;
  //
  virtual std::string getOperatingPoint() const = 0;
  //
  virtual std::string getJetAuthor() const = 0;

  // select an efficiency map for use in MC/MC and inefficiency scale factors, based on user specified selection of efficiency maps
  virtual bool setMapIndex(const std::string& flavour, unsigned int index) = 0;
  // select an efficiency map for use in MC/MC and inefficiency scale factors, based on configuration file that contains DSID to effmap specification
  virtual bool setMapIndex(unsigned int dsid) = 0;

  // this merely passes on the request to the underlying CDI object (listSystematics() cannot be used here, as corresponding CP::SystematicVariation objects may not exist)
  virtual std::map<std::string, std::vector<std::string> > listScaleFactorSystematics(bool named = false) const = 0;

  virtual CP::CorrectionCode getEigenRecompositionCoefficientMap(const std::string &label, std::map<std::string, std::map<std::string, float>> & coefficientMap) = 0;
};

#endif // CPIBTAGGINGEFFICIENCYTOOL_H
