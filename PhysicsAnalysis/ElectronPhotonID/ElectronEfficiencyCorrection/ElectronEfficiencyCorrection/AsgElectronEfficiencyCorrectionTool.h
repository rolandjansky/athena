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
#include "ElectronEfficiencyCorrection/TElectronEfficiencyCorrectionTool.h"
#include "ElectronEfficiencyCorrection/IAsgElectronEfficiencyCorrectionTool.h"

#include "xAODEgamma/Egamma.h"




class AsgElectronEfficiencyCorrectionTool
  : virtual public IAsgElectronEfficiencyCorrectionTool,
 // : virtual public IAthCalculatorTool,
 //   virtual public IUserDataCalcTool,
            public asg::AsgTool
{
  ASG_TOOL_CLASS2(AsgElectronEfficiencyCorrectionTool, IAsgElectronEfficiencyCorrectionTool, IAsgTool )

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
  const Root::TResult& calculate( const xAOD::Egamma* egam ) const;


  // Public methods
public:
  /// Method to get the underlying ROOT tool
 // inline Root::TElectronEfficiencyCorrectionTool* getRootTool() {return m_rootTool;};


  /// Method to get the plain TResult
  virtual const Root::TResult& getTResult() const
  {
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


inline const Root::TResult& AsgElectronEfficiencyCorrectionTool::calculate( const xAOD::IParticle *part ) const
{
  const xAOD::Egamma* egam = dynamic_cast<const xAOD::Egamma*>(part);
  if ( egam )
    {
      return calculate(egam);
    } 
  else
    {
      ATH_MSG_ERROR ( " Could not cast to const egamma pointer!" );
      return m_resultDummy;
    }
}

#endif

