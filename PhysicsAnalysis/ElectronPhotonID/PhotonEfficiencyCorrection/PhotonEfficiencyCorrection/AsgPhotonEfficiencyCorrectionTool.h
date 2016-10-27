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

// Utility includes
#include "boost/unordered_map.hpp"

// Include the return object and the underlying ROOT tool
#include "PATCore/TResult.h"

//xAOD includes
#include "AsgTools/AsgTool.h"
#include "PATInterfaces/ISystematicsTool.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/CorrectionCode.h"
#include "PhotonEfficiencyCorrection/TPhotonEfficiencyCorrectionTool.h"
#include "PhotonEfficiencyCorrection/IAsgPhotonEfficiencyCorrectionTool.h"

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

  /// A dummy return TResult object
  Root::TResult m_resultDummy;

  /// Systematics filter map
  boost::unordered_map<CP::SystematicSet, CP::SystematicSet> m_systFilter;
  
  /// Currently applied systematics
  CP::SystematicSet* m_appliedSystematics = nullptr;
  
  // Properties

  /// The list of file names
  std::string m_corrFileNameConv;
  std::string m_corrFileNameUnconv;

  // The prefix for the systematic name
  std::string m_sysSubstring;
  
  /// The prefix string for the result
  std::string m_resultPrefix;

  /// The string for the result
  std::string m_resultName;

  /// Force the data type to a given value
  int m_dataTypeOverwrite;



}; // End: class definition


#endif

