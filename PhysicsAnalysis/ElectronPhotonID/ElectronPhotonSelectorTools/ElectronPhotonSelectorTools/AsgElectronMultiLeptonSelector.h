/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef __ASGELECTRONMULTILEPTONSELECTOR__
#define __ASGELECTRONMULTILEPTONSELECTOR__

/**
   @class AsgElectronMultiLeptonSelector
   @brief Tool to select electrons

   @author Karsten Koeneke
   @date   October 2012

   12-MAR-2014, convert to ASGTool (Jovan Mitrevski)

*/

// Atlas includes
#include "AsgTools/AsgTool.h"
#include "EgammaAnalysisInterfaces/IAsgElectronMultiLeptonSelector.h"

namespace Root{
  class TElectronMultiLeptonSelector;
}


class AsgElectronMultiLeptonSelector : public asg::AsgTool,
	    virtual public IAsgElectronMultiLeptonSelector
{
  ASG_TOOL_CLASS2(AsgElectronMultiLeptonSelector, IAsgElectronMultiLeptonSelector,
		  CP::ISelectionTool)

public: 
  /** Standard constructor */
  AsgElectronMultiLeptonSelector(const std::string& myname);


  /** Standard destructor */
  virtual ~AsgElectronMultiLeptonSelector();


public:
  /** Gaudi Service Interface method implementations */
  virtual StatusCode initialize();

  /** Gaudi Service Interface method implementations */
  virtual StatusCode finalize();

  // Main methods for IAsgSelectionTool interface

  /** Method to get the plain AcceptInfo.
      This is needed so that one can already get the AcceptInfo 
      and query what cuts are defined before the first object 
      is passed to the tool. */
  const asg::AcceptInfo& getAcceptInfo() const;

  /** The main accept method: the actual cuts are applied here */
  asg::AcceptData accept( const xAOD::IParticle* part ) const;

  /** The main accept method: the actual cuts are applied here */
  asg::AcceptData accept( const xAOD::Electron* eg ) const;

  /** Method to get the operating point */
  virtual std::string getOperatingPointName( ) const;


  // Private member variables
private:
  /** Pointer to the underlying ROOT based tool */
  Root::TElectronMultiLeptonSelector* m_rootTool;
  
}; // End: class definition






#endif

