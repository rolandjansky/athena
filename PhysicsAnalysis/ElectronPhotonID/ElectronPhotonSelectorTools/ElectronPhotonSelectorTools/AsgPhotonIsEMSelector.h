/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "EgammaAnalysisInterfaces/IAsgPhotonIsEMSelector.h"
#include "xAODEgamma/PhotonFwd.h"
#include "xAODEgamma/EgammaFwd.h"
#include "xAODEgamma/ElectronFwd.h"
#include "xAODTracking/VertexFwd.h"

class EventContext;

namespace Root{
  class TPhotonIsEMSelector;
}

class AsgPhotonIsEMSelector : public asg::AsgTool,
			      virtual public IAsgPhotonIsEMSelector

{
  ASG_TOOL_CLASS3(AsgPhotonIsEMSelector, IAsgPhotonIsEMSelector,
		  IAsgEGammaIsEMSelector, CP::ISelectionTool)

  public:

  /** @brief Default constructor*/
  AsgPhotonIsEMSelector(const std::string& myname);

  /** @brief Destructor*/
  ~AsgPhotonIsEMSelector();
	
  /** @brief AlgTool initialize method*/
  virtual StatusCode initialize();

  /** Method to get the plain AcceptInfo.
      This is needed so that one can already get the AcceptInfo 
      and query what cuts are defined before the first object 
      is passed to the tool. */
  virtual const asg::AcceptInfo& getAcceptInfo() const;

  /** Accept with generic interface */
  virtual asg::AcceptData accept( const xAOD::IParticle* part ) const ;
  virtual asg::AcceptData accept( const EventContext& ctx, const xAOD::IParticle* part ) const ;

  /** Accept with Egamma objects */
  virtual asg::AcceptData accept( const EventContext& ctx, const xAOD::Egamma* part) const ;

  /** The main accept method: the actual cuts are applied here */
  virtual asg::AcceptData accept( const EventContext& ctx, const xAOD::Photon* part ) const ;

  /** The main accept method: the actual cuts are applied here */
  virtual asg::AcceptData accept( const EventContext& ctx, const xAOD::Electron* part ) const ;

   /** Method to get the operating point */
  virtual std::string getOperatingPointName( ) const;

  /** The basic isem */
  virtual StatusCode execute(const EventContext& ctx, const xAOD::Egamma* eg, unsigned int& isEM) const;

private:

  /** Working Point */
  std::string m_WorkingPoint;

  /** Config File */
  std::string m_configFile;

  /** Pointer to the underlying ROOT based tool */
  Root::TPhotonIsEMSelector* m_rootTool;

  /** @brief use f3core or f3 (default: use f3)*/
  bool m_useF3core;

  /// Flag for calo only cut-base 
  bool m_caloOnly; 
  float m_trigEtTh;

}; // End: class definition

#endif
