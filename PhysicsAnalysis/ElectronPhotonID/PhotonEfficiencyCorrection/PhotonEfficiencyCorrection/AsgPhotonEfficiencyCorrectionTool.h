/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef __ASGPHOTONEFFICIENCYCORRECTIONTOOL__
#define __ASGPHOTONEFFICIENCYCORRECTIONTOOL__

/**
   @class AthPhotonEfficiencyCorrectionTool
   @brief Calculate the egamma scale factors in Athena

   @author Rob Roy Fletcher, Karsten Koeneke, Michael Pitt, Giovanni Marchiori
   @date   August 2014
*/

// STL includes
#include <vector>
#include <string>
#include <fstream>      // std::ifstream

// Utility includes
#include "boost/unordered_map.hpp"
#include "boost/algorithm/string.hpp" // this one to replace std::string names

// Include the return object and the underlying ROOT tool
#include "PATCore/TResult.h"

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
  virtual StatusCode initialize();

  /// Gaudi Service Interface method implementations
  virtual StatusCode finalize();


  // Main methods from IUserDataCalcTool
public:
  /// The main calculate method: the actual correction factors are determined here
  const Root::TResult& calculate( const xAOD::IParticle* part ) const;
  const Root::TResult& calculate( const xAOD::Egamma* egam ) const;
  const Root::TResult& calculate( const xAOD::Egamma& egam ) const{  
 		    return calculate(&egam);} // pass the Egamma obj by reference

  ///Add some method for now as a first step to move the tool to then new interface 
  virtual CP::CorrectionCode getEfficiencyScaleFactor(const xAOD::Egamma& inputObject, double& efficiencyScaleFactor) const;
  virtual CP::CorrectionCode getEfficiencyScaleFactorError(const xAOD::Egamma& inputObject, double& efficiencyScaleFactorError) const;
  virtual CP::CorrectionCode applyEfficiencyScaleFactor(xAOD::Egamma& inputObject) const;

  ///The methods below should notify the user of what is actually in the list , without him having to go in the wiki

  /// returns: whether this tool is affected by the given systematic
  virtual bool isAffectedBySystematic( const CP::SystematicVariation& systematic ) const;
  
  /// returns: the list of all systematics this tool can be affected by
  virtual CP::SystematicSet affectingSystematics() const ;
  
  /// returns: the list of all systematics this tool recommends to use
  virtual CP::SystematicSet recommendedSystematics() const ;
  
  /// returns: the currently applied systematics
  const CP::SystematicSet& appliedSystematics() const {
    assert (m_appliedSystematics != nullptr);
    return *m_appliedSystematics;
  }
  
  /// Configure this tool for the given systematics
  virtual CP::SystematicCode applySystematicVariation ( const CP::SystematicSet& systConfig );

  CP::SystematicCode registerSystematics();

  // Private member variables
private:
  /// Pointer to the underlying ROOT based tool
  Root::TPhotonEfficiencyCorrectionTool* m_rootTool_unc;
  Root::TPhotonEfficiencyCorrectionTool* m_rootTool_con;
  /// additional pointers for ISO SF using RadZ decays
  Root::TPhotonEfficiencyCorrectionTool* m_rootTool_uncRadZ;
  Root::TPhotonEfficiencyCorrectionTool* m_rootTool_conRadZ;
  
  /// A dummy return TResult object
  Root::TResult m_resultDummy;

  /// Systematics filter map
  boost::unordered_map<CP::SystematicSet, CP::SystematicSet> m_systFilter;
  
  /// Currently applied systematics
  CP::SystematicSet* m_appliedSystematics = nullptr;
  
  // The prefix for the systematic name
  std::string m_sysSubstring;
  std::string m_sysSubstringRadZ;
  
  // Get the correction filename from the map
  std::string getFileName(std::string isoWP, bool isConv, std::string sufix);
  
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
  
  /// map filename
  std::string m_mapFile;  
  
  /// photonPT threshold for different isolation menus
  float m_Threshold_lowPT;
  float m_Threshold_highPT;
  bool  m_UseRadZ_mediumPT;



}; // End: class definition


#endif

