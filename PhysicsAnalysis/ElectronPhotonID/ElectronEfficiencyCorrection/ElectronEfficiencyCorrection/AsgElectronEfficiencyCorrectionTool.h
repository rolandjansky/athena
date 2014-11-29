/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef __ASGELECTRONEFFICIENCYCORRECTIONTOOL__
#define __ASGELECTRONEFFICIENCYCORRECTIONTOOL__

/**
   @class AthElectronEfficiencyCorrectionTool
   @brief Calculate the egamma scale factors in Athena

   @author Rob Roy Fletcher, Karsten Koeneke
   @date   May 2014
*/

// STL includes
#include <vector>
#include <string>

// Include the return object and the underlying ROOT tool
#include "PATCore/TResult.h"

//xAOD includes
#include "AsgTools/AsgTool.h"
#include "PATInterfaces/SystematicsTool.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/CorrectionCode.h"
#include "ElectronEfficiencyCorrection/TElectronEfficiencyCorrectionTool.h"
#include "ElectronEfficiencyCorrection/IAsgElectronEfficiencyCorrectionTool.h"

#include "xAODEgamma/ElectronFwd.h"

class AsgElectronEfficiencyCorrectionTool
  : virtual public IAsgElectronEfficiencyCorrectionTool,
    public CP::SystematicsTool,
    public asg::AsgTool
{
  ASG_TOOL_CLASS(AsgElectronEfficiencyCorrectionTool, IAsgElectronEfficiencyCorrectionTool)

public:
  /// Standard constructor
  AsgElectronEfficiencyCorrectionTool ( const std::string myname );

  /// Standard destructor
  virtual ~AsgElectronEfficiencyCorrectionTool();


public:
  /// Gaudi Service Interface method implementations
  virtual StatusCode initialize();

  /// Gaudi Service Interface method implementations
  virtual StatusCode finalize();

  // Main methods from IUserDataCalcTool
public:
  /// The main calculate method: the actual correction factors are determined here
  const Root::TResult& calculate( const xAOD::IParticle* part ) const;
  const Root::TResult& calculate( const xAOD::Electron* egam ) const;
  const Root::TResult& calculate( const xAOD::Electron& egam ) const{ 
    return calculate(&egam); 
  }

  CP::CorrectionCode getEfficiencyScaleFactor(const xAOD::Electron& inputObject, double& efficiencyScaleFactor) const;

  CP::CorrectionCode applyEfficiencyScaleFactor(xAOD::Electron& inputObject) const;



  /// returns: whether this tool is affected by the given systematis
  virtual bool isAffectedBySystematic( const CP::SystematicVariation& systematic ) const ;
  
  /// returns: the list of all systematics this tool can be affected by
  virtual CP::SystematicSet affectingSystematics() const ;
  
  /// returns: the list of all systematics this tool recommends to use
  virtual CP::SystematicSet recommendedSystematics() const ;

  virtual CP::SystematicCode sysApplySystematicVariation ( const CP::SystematicSet& systConfig ) ;

  CP::SystematicCode registerSystematics();

  /// Method to get the plain TResult
  virtual const Root::TResult& getTResult() const{
    return m_rootTool->getTResult();
  }

  // Private member variables
private:
 
  /// Pointer to the underlying ROOT based tool
  Root::TElectronEfficiencyCorrectionTool* m_rootTool;

  /// A dummy return TResult object
  Root::TResult m_resultDummy;

  // Properties

  /// The list of file names
  std::vector< std::string > m_corrFileNameList;


  /// The prefix string for the result
  std::string m_resultPrefix;

  /// The string for the result
  std::string m_resultName;

  /// Force the data type to a given value
  int m_dataTypeOverwrite;


}; // End: class definition



#endif

