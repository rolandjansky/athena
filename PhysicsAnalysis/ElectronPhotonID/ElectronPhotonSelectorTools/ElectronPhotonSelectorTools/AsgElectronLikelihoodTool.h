/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef __ASGELECTRONLIKELIHOODTOOL__
#define __ASGELECTRONLIKELIHOODTOOL__

/**
   @class AsgElectronLikelihoodTool
   @brief Tool to select objects

   @author Karsten Koeneke
   @date   October 2012

   09-APR-2014, convert to ASGTool (Jovan Mitrevski)

*/

// Atlas includes
#include "AsgTools/AsgTool.h"
#include "ElectronPhotonSelectorTools/TElectronLikelihoodTool.h"
#include "ElectronPhotonSelectorTools/IAsgElectronLikelihoodTool.h"

#include "xAODEgamma/ElectronFwd.h"

class AsgElectronLikelihoodTool : virtual public asg::AsgTool, 
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
  /** The main accept method: the actual cuts are applied here */
  const Root::TAccept& accept( const xAOD::IParticle* part ) const;

  /** The main accept method: the actual cuts are applied here */
  const Root::TAccept& accept( const xAOD::Electron* eg ) const;

  /** Accept method for EFCaloLH in the trigger; do full LH if !CaloCutsOnly **/
  const Root::TAccept& accept( const xAOD::Egamma* eg,
                               bool CaloCutsOnly=true) const;

  /** Accept using reference **/
  virtual const Root::TAccept& accept( const xAOD::Electron& part ) const {
    return accept (&part);
  }

  /** Accept using reference **/
  virtual const Root::TAccept& accept( const xAOD::IParticle& part ) const {
    return accept (&part);
  }

  /** Accept using reference **/
  virtual const Root::TAccept& accept( const xAOD::Egamma& part, 
                                       bool CaloCutsOnly=true) const {
    return accept (&part, CaloCutsOnly);
  }

  // Main methods for IAsgCalculatorTool interface
public:
  /** The main result method: the actual likelihood is calculated here */
  const Root::TResult& calculate( const xAOD::IParticle* part ) const;

  /** The main result method: the actual likelihood is calculated here */
  const Root::TResult& calculate( const xAOD::Electron* eg ) const;

  /** Calculate method for EFCaloLH in the trigger; do full LH if !CaloCutsOnly **/
  const Root::TResult& calculate( const xAOD::Egamma* eg,
                                  bool CaloCutsOnly=true) const;

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


  // Private methods
private:
  /// Get the number of primary vertices
  unsigned int getNPrimVertices() const;


  /// Get the name of the current operating point
  std::string getOperatingPointName( const LikeEnum::Menu operating_point ) const;


  // Private member variables
private:
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


  // /// Say if we should re-calculate the likelihood every time (otherwise tried to be taken from UserData)
  // bool m_forceCalcLH;

  /// The cut value to be used
  unsigned int m_operatingPoint;

}; // End: class definition





#endif

