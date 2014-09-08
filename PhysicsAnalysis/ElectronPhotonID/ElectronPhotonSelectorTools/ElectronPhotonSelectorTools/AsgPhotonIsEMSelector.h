/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef __ASGPHOTONISEMSELECTOR__
#define __ASGPHOTONISEMSELECTOR__

/**
   @class AsgElectronIsEMSelector
   @brief Tool to select photons

   @author Jovan Mitrevski (UCSC) Karsten Koeneke (CERN)
   @date   Dec 2011 - Fab 2012

   Based on egammaPhotonCutIDTool, by F. Derue.

   11-MAR-2014, convert to ASGTool
*/

// Atlas includes
#include "AsgTools/AsgTool.h"

// Include the interfaces
#include "ElectronPhotonSelectorTools/IAsgPhotonIsEMSelector.h"

#include "xAODTracking/VertexFwd.h"

// Include the return object and the underlying ROOT tool
#include "PATCore/TAccept.h"
#include "ElectronPhotonSelectorTools/TPhotonIsEMSelector.h"

class AsgPhotonIsEMSelector : virtual public asg::AsgTool,
                              virtual public IAsgPhotonIsEMSelector
{
  ASG_TOOL_CLASS2(AsgPhotonIsEMSelector, IAsgPhotonIsEMSelector, IAsgSelectionTool)

  public:

  /** @brief Default constructor*/
  AsgPhotonIsEMSelector(std::string myname);

  /** @brief Destructor*/
  ~AsgPhotonIsEMSelector();
	
  /** @brief AlgTool initialize method*/
  StatusCode initialize();
  /** @brief AlgTool finalize method*/
  StatusCode finalize();

  /** The main accept method: the actual cuts are applied here */
  const Root::TAccept& accept( const xAOD::IParticle* part ) const;

  /** The main accept method: the actual cuts are applied here */
  const Root::TAccept& accept( const xAOD::Photon* part ) const;

  unsigned int IsemValue() const {return m_rootTool->isEM(); };

  // what isEM should be filled
  egammaPID::PID PIDName() const {return m_rootTool->isEMPIDName(); };

  /** The basic isem */
  virtual StatusCode execute(const xAOD::Photon*) const;

  /** The isem potentially for the trigger */
  virtual StatusCode execute(const xAOD::Photon*, double trigEtTh) const;

  /** Method to get the plain TAccept */
  virtual const Root::TAccept& getTAccept( ) const
  {
    return m_rootTool->getTAccept();
  }


private:

  double getConversion1OverP(const xAOD::Vertex*) const; 
  
  /** Pointer to the underlying ROOT based tool */
  Root::TPhotonIsEMSelector* m_rootTool;

  /** @brief use f3core or f3 (default: use f3)*/
  bool m_useF3core;

  /** A dummy return TAccept object */
  Root::TAccept m_acceptDummy;

}; // End: class definition

#endif
