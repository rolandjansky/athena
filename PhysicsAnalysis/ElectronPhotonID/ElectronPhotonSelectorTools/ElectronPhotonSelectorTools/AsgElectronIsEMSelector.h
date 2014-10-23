/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef __ASGELECTRONISEMSELECTOR__
#define __ASGELECTRONISEMSELECTOR__

/**
   @class AsgElectronIsEMSelector
   @brief Tool to select objects.

   @author Jovan Mitrevski (UCSC) Karsten Koeneke (CERN)
   @date   Dec 2011 - Fab 2012

   Based on egammaElectronCutIDTool, by F. Derue.

   11-MAR-2014, convert to ASGTool (Jovan Mitrevski)

*/

// Atlas includes
#include "AsgTools/AsgTool.h"

// Include the interfaces
#include "ElectronPhotonSelectorTools/IAsgElectronIsEMSelector.h"

// Include the return object and the underlying ROOT tool
#include "PATCore/TAccept.h"
#include "ElectronPhotonSelectorTools/TElectronIsEMSelector.h"
#include <string>

class AsgElectronIsEMSelector :  virtual public asg::AsgTool, 
                                 virtual public IAsgElectronIsEMSelector
{
  ASG_TOOL_CLASS2(AsgElectronIsEMSelector, IAsgElectronIsEMSelector, IAsgSelectionTool)

  public:
  /** Standard constructor */
  AsgElectronIsEMSelector(std::string myname);

  /** Standard destructor */
  virtual ~AsgElectronIsEMSelector();

  /** Gaudi Service Interface method implementations */
  virtual StatusCode initialize();

  /** Gaudi Service Interface method implementations */
  virtual StatusCode finalize();

  // Main methods for IAsgSelectionTool interface

  /** The main accept method: the actual cuts are applied here */
  const Root::TAccept& accept( const xAOD::IParticle* part ) const;

  /** The main accept method: the actual cuts are applied here */
  const Root::TAccept& accept( const xAOD::Electron* part ) const;

  /** Accept using reference **/
  virtual const Root::TAccept& accept( const xAOD::Electron& part ) const {
    return accept (&part);
  }

  /** Accept using reference **/
  virtual const Root::TAccept& accept( const xAOD::IParticle& part ) const {
    return accept (&part);
  }

  /** METHODS FOR THE TRIGGER **/
  /*This method is for the trigger */
  virtual const Root::TAccept& accept( const xAOD::Electron* part, 
				       double trigEtTh, 
				       bool CaloCutsOnly=true) const;

  /** This method is for the trigger, and implies CaloCutsOnly set to true */
  virtual const Root::TAccept& accept( const xAOD::Egamma* part, 
				       double trigEtTh) const;

  unsigned int IsemValue() const {return m_rootTool->isEM(); };

  // what isEM should be filled
  egammaPID::PID PIDName() const {return m_rootTool->isEMPIDName(); };

  /** The basic isem */
  StatusCode execute(const xAOD::Electron* eg) const;
  
  /** The isem potentially for the trigger */
  StatusCode execute(const xAOD::Electron* eg, 
		     double trigEtTh, 
		     bool CaloCutsOnly) const;

  /** The isem potentially for the trigger, implies CaloCutsOnly */
  StatusCode execute(const xAOD::Egamma* eg, 
		     double trigEtTh) const;

  /** Method to get the plain TAccept */
  virtual const Root::TAccept& getTAccept( ) const
  {
    return m_rootTool->getTAccept();
  }


  // Private member variables
private:

  unsigned int calocuts_electrons(const xAOD::Egamma* eg, 
				  float eta2, double et,
				  double trigEtTh,
				  unsigned int iflag) const;

  unsigned int TrackCut(const xAOD::Electron* eg, 
			float eta2, double et, double energy,
			unsigned int iflag) const;

  /** Config File */
  std::string m_configFile;

  /** Pointer to the underlying ROOT based tool */
  Root::TElectronIsEMSelector* m_rootTool;

  /** @brief use f3core or f3 (default: use f3)*/
  bool m_useF3core;

  /** A dummy return TAccept object */
  Root::TAccept m_acceptDummy;

}; // End: class definition


#endif

