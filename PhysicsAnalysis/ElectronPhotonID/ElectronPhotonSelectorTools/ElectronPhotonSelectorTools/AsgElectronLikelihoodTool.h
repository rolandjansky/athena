/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef __ASGELECTRONLIKELIHOODTOOL__
#define __ASGELECTRONLIKELIHOODTOOL__


// Atlas includes
#include "AsgTools/AsgTool.h"
#include "ElectronPhotonSelectorTools/TElectronLikelihoodTool.h"
#include "ElectronPhotonSelectorTools/IAsgElectronLikelihoodTool.h"

#include "xAODEgamma/ElectronFwd.h"

class AsgElectronLikelihoodTool : public asg::AsgTool, 
				  virtual public IAsgElectronLikelihoodTool
{
  ASG_TOOL_CLASS2(AsgElectronLikelihoodTool, IAsgElectronLikelihoodTool, IAsgSelectionTool)

public:
  /** Standard constructor */
  AsgElectronLikelihoodTool( const std::string myname);


  /** Standard destructor */
  virtual ~AsgElectronLikelihoodTool();
public:
  /** Gaudi Service Interface method implementations */
  virtual StatusCode initialize();

  /** Gaudi Service Interface method implementations */
  virtual StatusCode finalize();

  // Main methods for IAsgSelectorTool interface
public:
  /** The main accept method: using the generic interface */
  const Root::TAccept& accept( const xAOD::IParticle* part ) const;

  /** The main accept method: the actual cuts are applied here */
  const Root::TAccept& accept( const xAOD::Electron* eg ) const {
    return accept (eg, -99); // mu = -99 as input will force accept to grab the pileup variable from the xAOD object
  }

  /** The main accept method: the actual cuts are applied here */
  const Root::TAccept& accept( const xAOD::Egamma* eg ) const {
    return accept (eg, -99); // mu = -99 as input will force accept to grab the pileup variable from the xAOD object
  }

  /** The main accept method: in case mu not in EventInfo online */
  const Root::TAccept& accept( const xAOD::Electron* eg, double mu ) const;

  /** The main accept method: in case mu not in EventInfo online */
  const Root::TAccept& accept( const xAOD::Egamma* eg, double mu ) const;
  
  /** Accept using reference **/
  virtual const Root::TAccept& accept( const xAOD::IParticle& part ) const {
    return accept (&part);
  }

  /** Accept using reference **/
  virtual const Root::TAccept& accept( const xAOD::Electron& part ) const {
    return accept (&part, -99); // mu = -99 as input will force accept to grab the pileup variable from the xAOD object
  }

  /** Accept using reference **/
  virtual const Root::TAccept& accept( const xAOD::Egamma& part ) const {
    return accept (&part, -99); // mu = -99 as input will force accept to grab the pileup variable from the xAOD object
  }

  /** Accept using reference; in case mu not in EventInfo online **/
  virtual const Root::TAccept& accept( const xAOD::Electron& part, double mu ) const {
    return accept (&part, mu);
  }

  /** Accept using reference; in case mu not in EventInfo online **/
  virtual const Root::TAccept& accept( const xAOD::Egamma& part, double mu ) const {
    return accept (&part, mu);
  }


  // Main methods for IAsgCalculatorTool interface
public:
  /** The main result method: the actual likelihood is calculated here */
  const Root::TResult& calculate( const xAOD::IParticle* part ) const;

  /** The main result method: the actual likelihood is calculated here */
  const Root::TResult& calculate( const xAOD::Electron* eg ) const {
    return calculate (eg, -99); // mu = -99 as input will force accept to grab the pileup variable from the xAOD object
  }

  /** The main result method: the actual likelihood is calculated here */
  const Root::TResult& calculate( const xAOD::Egamma* eg ) const {
    return calculate (eg, -99); // mu = -99 as input will force accept to grab the pileup variable from the xAOD object
  }

  /** The main result method: the actual likelihood is calculated here */
  const Root::TResult& calculate( const xAOD::Electron* eg, double mu ) const;

  /** The main result method: the actual likelihood is calculated here */
  const Root::TResult& calculate( const xAOD::Egamma* eg, double mu ) const; 

  /** Method to get the plain TAccept */
  inline virtual const Root::TAccept& getTAccept( ) const
  {
    return m_rootTool->getTAccept();
  }


  /** Method to get the plain TResult */
  inline virtual const Root::TResult& getTResult( ) const
  {
    return m_rootTool->getTResult();
  }

  virtual std::string getOperatingPointName( ) const;

  // Private methods
private:
  /// Get the number of primary vertices
  unsigned int getNPrimVertices() const;


  /// Get the name of the current operating point



  // Private member variables
private:
  // The input config file.
  std::string m_configFile;

  /** Pointer to the underlying ROOT based tool */
  Root::TElectronLikelihoodTool* m_rootTool;
  
  /** A dummy return TAccept object */
  Root::TAccept m_acceptDummy;

  /** A dummy return TResult object */
  Root::TResult m_resultDummy;


  /// Whether to use the PV (not available for trigger)
  bool m_usePVCont;

  /// defualt nPV (when not using PVCont)
  unsigned int m_nPVdefault;

  /// The primary vertex container name
  std::string m_primVtxContName;

  /// The input ROOT file name that holds the PDFs
  std::string m_pdfFileName;

  /// Flag for calo only LH
  bool m_caloOnly;

  /// Flag for b-layer protection
  bool m_cutNextToInnerMost;

  // /// Say if we should re-calculate the likelihood every time (otherwise tried to be taken from UserData)
  // bool m_forceCalcLH;

  /// The cut value to be used
  unsigned int m_operatingPoint;

}; // End: class definition





#endif

