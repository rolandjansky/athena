/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef __ASGFORWARDELECTRONISEMSELECTOR__
#define __ASGFORWARDELECTRONISEMSELECTOR__

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
#include "EgammaAnalysisInterfaces/IAsgForwardElectronIsEMSelector.h"

#include "PATCore/TAccept.h"

#include <string>

namespace Root{
  class TForwardElectronIsEMSelector;
}

class AsgForwardElectronIsEMSelector : public asg::AsgTool, 
				       virtual public IAsgForwardElectronIsEMSelector
{

  ASG_TOOL_CLASS3(AsgForwardElectronIsEMSelector, IAsgForwardElectronIsEMSelector,
		  IAsgEGammaIsEMSelector,IAsgSelectionTool)

  public:
  /** Standard constructor */
  AsgForwardElectronIsEMSelector(std::string myname);

  /** Standard destructor */
  virtual ~AsgForwardElectronIsEMSelector();

  /** Gaudi Service Interface method implementations */
  virtual StatusCode initialize();

  /** Gaudi Service Interface method implementations */
  virtual StatusCode finalize();

  // Main methods for IAsgSelectionTool interface


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

  /** Accept with Photon objects */
  virtual const Root::TAccept& accept( const xAOD::Photon* part ) const ;

  /** Accept with Photon objects */
  virtual const Root::TAccept& accept( const xAOD::Photon& part ) const {
    return accept(&part);
  }

  /** Accept with Electron objects */
  virtual const Root::TAccept& accept( const xAOD::Electron* part ) const ;

  /** Accept with Electron objects */
  virtual const Root::TAccept& accept( const xAOD::Electron& part ) const{
    return accept(&part);
  }

  /** The value of the isem **/
  virtual unsigned int IsemValue() const ;

  /** Method to get the operating point */
  virtual std::string getOperatingPointName( ) const;

  //The main execute method
  StatusCode execute(const xAOD::Egamma* eg) const;

  /** Method to get the plain TAccept */
  virtual const Root::TAccept& getTAccept( ) const;


  // Private member variables
private:

  unsigned int getNPrimVertices() const;

  unsigned int calocuts_electrons(const xAOD::Egamma* eg,
				  float eta2, float nvtx,
				  unsigned int iflag) const;

  /** Working Point */
  std::string m_WorkingPoint;

  /** Config File */
  std::string m_configFile;

  /** Pointer to the underlying ROOT based tool */
  Root::TForwardElectronIsEMSelector* m_rootForwardTool;

  /** A dummy return TAccept object */
  Root::TAccept m_acceptDummy;

  bool m_usePVCont;

  // defualt nPV (when not using PVCont)
  unsigned int m_nPVdefault;

  // The primary vertex container name
  std::string m_primVtxContName;

}; // End: class definition


#endif

