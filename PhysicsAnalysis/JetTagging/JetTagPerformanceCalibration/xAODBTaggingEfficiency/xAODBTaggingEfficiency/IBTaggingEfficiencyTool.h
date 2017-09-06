// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IBTaggingEfficiencyTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef CPIBTAGGINGEFFICIENCYTOOL_H
#define CPIBTAGGINGEFFICIENCYTOOL_H

#include "AsgTools/IAsgTool.h"
#include "PATInterfaces/CorrectionCode.h"
#include "PATInterfaces/SystematicCode.h"
#include "PATInterfaces/SystematicSet.h"

#include "xAODJet/Jet.h"

#include <vector>
#include <string>
#include <set>

#include "CalibrationDataInterface/CalibrationDataInterfaceROOT.h"

class IBTaggingEfficiencyTool : virtual public asg::IAsgTool {

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

  // utility methods
  virtual const std::map<CP::SystematicVariation, std::vector<std::string> > listSystematics() const = 0;
  // 
  virtual std::string getTaggerName() const = 0;
  //
  virtual std::string getOperatingPoint() const = 0;
  //
  virtual std::string getJetAuthor() const = 0;
  
  virtual CP::SystematicCode applySystematicVariation(const CP::SystematicSet & set) = 0;
  // 
  virtual CP::SystematicSet affectingSystematics() const = 0;
  //
  virtual CP::SystematicSet recommendedSystematics() const = 0;
  //
  virtual bool isAffectedBySystematic(const CP::SystematicVariation & systematic) const = 0;
  
  virtual bool setMapIndex(const std::string& flavour, unsigned int index) = 0;
  // virtual bool setMapIndex(const std::string& flavour, const std::string & type) = 0;

  // this merely passes on the request to the underlying CDI object (listSystematics() cannot be used here, as corresponding CP::SystematicVariation objects may not exist)
  virtual std::map<std::string, std::vector<std::string> > listScaleFactorSystematics(bool named = false) const = 0;
};

#endif // CPIBTAGGINGEFFICIENCYTOOL_H
