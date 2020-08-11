/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef PHOTONEFFICIENCYCORRECTION_ASGPHOTONEFFICIENCYCORRECTIONTOOL
#define PHOTONEFFICIENCYCORRECTION_ASGPHOTONEFFICIENCYCORRECTIONTOOL

/**
   @class AthPhotonEfficiencyCorrectionTool
   @brief Calculate the egamma scale factors in Athena

   @author Michael Pitt <michael.pitt@cern.ch>, Giovanni Marchiori
   @date   February 2018
*/

// STL includes
#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>

// Utility includes
#include "boost/algorithm/string.hpp" // this one to replace std::string names


//xAOD includes
#include "AsgTools/AsgTool.h"
#include "PATInterfaces/ISystematicsTool.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/CorrectionCode.h"
#include "PhotonEfficiencyCorrection/TPhotonEfficiencyCorrectionTool.h"
#include "EgammaAnalysisInterfaces/IAsgPhotonEfficiencyCorrectionTool.h"

#include "xAODEgamma/Egamma.h"

class AsgPhotonEfficiencyCorrectionTool
  : virtual public IAsgPhotonEfficiencyCorrectionTool,
    virtual public CP::ISystematicsTool,
            public asg::AsgTool
{
  ASG_TOOL_CLASS2(AsgPhotonEfficiencyCorrectionTool, IAsgPhotonEfficiencyCorrectionTool, CP::ISystematicsTool )

public:
  /// Standard constructor
  AsgPhotonEfficiencyCorrectionTool ( const std::string myname );

  /// Standard destructor
  virtual ~AsgPhotonEfficiencyCorrectionTool();


public:
  /// Gaudi Service Interface method implementations
  virtual StatusCode initialize() override;



public:
  typedef Root::TPhotonEfficiencyCorrectionTool::Result Result;
  /// The main calculate method: the actual correction factors are determined here
  const Result calculate( const xAOD::IParticle* part ) const;
  const Result calculate( const xAOD::Egamma* egam ) const;
  const Result calculate( const xAOD::Egamma& egam ) const{  
 		    return calculate(&egam);
  } // pass the Egamma obj by reference

  ///Add some method for now as a first step to move the tool to then new interface 
  virtual CP::CorrectionCode getEfficiencyScaleFactor(const xAOD::Egamma& inputObject, double& efficiencyScaleFactor) const override;
  virtual CP::CorrectionCode getEfficiencyScaleFactorError(const xAOD::Egamma& inputObject, double& efficiencyScaleFactorError) const override;
  virtual CP::CorrectionCode applyEfficiencyScaleFactor(xAOD::Egamma& inputObject) const override;

  ///The methods below should notify the user of what is actually in the list , without him having to go in the wiki

  /// returns: whether this tool is affected by the given systematic
  virtual bool isAffectedBySystematic( const CP::SystematicVariation& systematic ) const override;
  
  /// returns: the list of all systematics this tool can be affected by
  virtual CP::SystematicSet affectingSystematics() const override;
  
  /// returns: the list of all systematics this tool recommends to use
  virtual CP::SystematicSet recommendedSystematics() const override;
  
  /// returns: the currently applied systematics
  const CP::SystematicSet& appliedSystematics() const {
    assert (m_appliedSystematics != nullptr);
    return *m_appliedSystematics;
  }
  
  /// Configure this tool for the given systematics
  virtual CP::SystematicCode applySystematicVariation ( const CP::SystematicSet& systConfig ) override;

  CP::SystematicCode registerSystematics();

  // Private member variables
private:
  /// Pointer to the underlying ROOT based tool
  Root::TPhotonEfficiencyCorrectionTool* m_rootTool_unc;
  Root::TPhotonEfficiencyCorrectionTool* m_rootTool_con;
  
  /// Systematics filter map
  std::unordered_map<CP::SystematicSet, CP::SystematicSet> m_systFilter;
  
  /// Currently applied systematics
  CP::SystematicSet* m_appliedSystematics = nullptr;
  
  // The prefix for the systematic name
  std::string m_sysSubstring;
  
  // Get the correction filename from the map
  std::string getFileName(std::string isoWP, std::string trigWP, bool isConv);
  
  // Set prefix of the corresponding calibration filenames:
  std::string m_file_prefix_ID="offline.Tight";
  std::string m_file_prefix_ISO="Isolation.isolFixedCut";
  std::string m_file_prefix_Trig="HLT";
  
  // Properties
  
  /// The list of input file names
  std::string m_corrFileNameConv;
  std::string m_corrFileNameUnconv;
 
  /// The prefix string for the result
  std::string m_resultPrefix;

  /// The string for the result
  std::string m_resultName;

  /// Force the data type to a given value
  int m_dataTypeOverwrite;
  
  /// Isolation working point
  std::string m_isoWP;
  
  /// Trigger name for trigger SF
  std::string m_trigger;
  
  /// map filename
  std::string m_mapFile;  
  
  //use RandomRun Number
  bool m_useRandomRunNumber;
  int m_defaultRandomRunNumber;
 

}; // End: class definition


#endif

