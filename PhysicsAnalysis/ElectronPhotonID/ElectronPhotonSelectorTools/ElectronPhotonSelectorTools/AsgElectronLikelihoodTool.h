/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef __ASGELECTRONLIKELIHOODTOOL__
#define __ASGELECTRONLIKELIHOODTOOL__


// Atlas includes
#include "AsgTools/AsgTool.h"
#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"
#include "xAODEgamma/ElectronFwd.h"
#include "PATCore/TAccept.h"            // for TAccept
#include "PATCore/TResult.h"            // for TResult

namespace Root{
  class TElectronLikelihoodTool;
}


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
  virtual const Root::TAccept& getTAccept( ) const;

  /** Method to get the plain TResult */
  virtual const Root::TResult& getTResult( ) const;

  virtual std::string getOperatingPointName( ) const;

  // Private methods
private:
  /// Get the number of primary vertices
  unsigned int getNPrimVertices() const;

  /// Get the FCal ET for centrality determination (for HI collisions)
  double getFcalEt() const;

  /// Get the name of the current operating point



  // Private member variables
private:
  /** Working Point */
  std::string m_WorkingPoint;

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

  /// Whether or not to use the CaloSums container in HI events
  bool m_useCaloSumsCont;

  /// defualt FCal ET (when not using CaloSums container, in HI events)
  double m_fcalEtDefault;

  /// The CaloSums container name, in HI events
  std::string m_CaloSumsContName;


  /// The input ROOT file name that holds the PDFs
  std::string m_pdfFileName;

  /// Flag for calo only LH
  bool m_caloOnly;



}; // End: class definition





#endif

