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
#include "xAODEgamma/PhotonFwd.h"
#include "xAODEgamma/EgammaFwd.h"
#include "xAODEgamma/ElectronFwd.h"
#include "xAODTracking/VertexFwd.h"

// Include the return object and the underlying ROOT tool
#include "PATCore/TAccept.h"

namespace Root{
  class TPhotonIsEMSelector;
}

class AsgPhotonIsEMSelector : public asg::AsgTool,
			      virtual public IAsgPhotonIsEMSelector

{
  ASG_TOOL_CLASS3(AsgPhotonIsEMSelector, IAsgPhotonIsEMSelector,
		  IAsgEGammaIsEMSelector,IAsgSelectionTool)

  public:

  /** @brief Default constructor*/
  AsgPhotonIsEMSelector(std::string myname);

  /** @brief Destructor*/
  ~AsgPhotonIsEMSelector();
	
  /** @brief AlgTool initialize method*/
  StatusCode initialize();
  /** @brief AlgTool finalize method*/
  StatusCode finalize();

  /** Accept with generic interface */
  virtual const Root::TAccept& accept( const xAOD::IParticle* part ) const ;

  /** Accept with generic interface */
  virtual const Root::TAccept& accept( const xAOD::IParticle& part ) const {
    return accept(&part);
  }
  
  /** Accept with Egamma objects */
  virtual const Root::TAccept& accept( const xAOD::Egamma* part) const ;

  /** Accept with Egamma objects */
  virtual const Root::TAccept& accept( const xAOD::Egamma& part) const {
    return accept(&part);
  }

  /** The main accept method: the actual cuts are applied here */
  virtual const Root::TAccept& accept( const xAOD::Photon* part ) const ;

  /** The main accept method: the actual cuts are applied here */
  virtual const Root::TAccept& accept( const xAOD::Photon& part ) const {
    return accept(&part);
  }

  /** The main accept method: the actual cuts are applied here */
  virtual const Root::TAccept& accept( const xAOD::Electron* part ) const ;

  /** The main accept method: the actual cuts are applied here */
  virtual const Root::TAccept& accept( const xAOD::Electron& part ) const{
    return accept(&part);
  }

  /** The value of the isem **/
  virtual unsigned int IsemValue() const ;

   /** Method to get the operating point */
  virtual std::string getOperatingPointName( ) const;

  /** The basic isem */
  virtual StatusCode execute(const xAOD::Egamma* eg) const;

  /** Method to get the plain TAccept */
  virtual const Root::TAccept& getTAccept( ) const;


private:

  /** Working Point */
  std::string m_WorkingPoint;

  /** Config File */
  std::string m_configFile;

  /** Pointer to the underlying ROOT based tool */
  Root::TPhotonIsEMSelector* m_rootTool;

  /** @brief use f3core or f3 (default: use f3)*/
  bool m_useF3core;

  /** A dummy return TAccept object */
  Root::TAccept m_acceptDummy;

  /// Flag for calo only cut-base 
  bool m_caloOnly; 
  float m_trigEtTh;

}; // End: class definition

#endif
