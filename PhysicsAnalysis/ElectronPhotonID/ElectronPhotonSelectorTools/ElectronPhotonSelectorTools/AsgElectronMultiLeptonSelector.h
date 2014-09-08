/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "ElectronPhotonSelectorTools/IAsgElectronMultiLeptonSelector.h"

// Include the return object and ROOT tool
#include "PATCore/TAccept.h"
#include "ElectronPhotonSelectorTools/TElectronMultiLeptonSelector.h"


class AsgElectronMultiLeptonSelector : virtual public asg::AsgTool,
	    virtual public IAsgElectronMultiLeptonSelector
{
  ASG_TOOL_CLASS2(AsgElectronMultiLeptonSelector, IAsgElectronMultiLeptonSelector, IAsgSelectionTool)

public: 
  /** Standard constructor */
  AsgElectronMultiLeptonSelector(std::string myname);


  /** Standard destructor */
  virtual ~AsgElectronMultiLeptonSelector();


public:
  /** Gaudi Service Interface method implementations */
  virtual StatusCode initialize();

  /** Gaudi Service Interface method implementations */
  virtual StatusCode finalize();

  // Main methods for IAsgSelectionTool interface

  /** The main accept method: the actual cuts are applied here */
  const Root::TAccept& accept( const xAOD::IParticle* part ) const;

  /** The main accept method: the actual cuts are applied here */
  const Root::TAccept& accept( const xAOD::Electron* eg ) const;

  /** Method to get the plain TAccept */
  inline virtual const Root::TAccept& getTAccept( ) const
  {
    return m_rootTool->getTAccept();
  }


  // Private member variables
private:
  /** Pointer to the underlying ROOT based tool */
  Root::TElectronMultiLeptonSelector* m_rootTool;
  
  /** A dummy return TAccept object */
  Root::TAccept m_acceptDummy;

}; // End: class definition



// Inline methods
inline const Root::TAccept& AsgElectronMultiLeptonSelector::accept(const xAOD::IParticle* part) const
{
  const xAOD::Electron* eg = dynamic_cast<const xAOD::Electron*>(part);
  if (eg)
    {
      return accept(eg);
    }
  else
    {
      ATH_MSG_ERROR ( " Could not cast to const egamma " );
      return m_acceptDummy;
    }
}


#endif

