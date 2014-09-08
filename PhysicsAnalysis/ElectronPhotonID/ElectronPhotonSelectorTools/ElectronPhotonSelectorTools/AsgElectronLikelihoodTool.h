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

#include "xAODEgamma/Electron.h"

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


  // Main methods for IAsgCalculatorTool interface
public:
  /** The main result method: the actual likelihood is calculated here */
  const Root::TResult& calculate( const xAOD::IParticle* part ) const;

  /** The main result method: the actual likelihood is calculated here */
  const Root::TResult& calculate( const xAOD::Electron* eg ) const;


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



// Inline methods
inline const Root::TAccept& AsgElectronLikelihoodTool::accept(const xAOD::IParticle* part) const
{
  const xAOD::Electron* eg = dynamic_cast<const xAOD::Electron*>(part);
  if (eg)
    {
      return accept(eg);
    }
  else
    {
      ATH_MSG_ERROR ( " Could not cast to const electron " );
      return m_acceptDummy;
    }
}

// Inline methods
inline const Root::TResult& AsgElectronLikelihoodTool::calculate(const xAOD::IParticle* part) const
{
  const xAOD::Electron* eg = dynamic_cast<const xAOD::Electron*>(part);
  if (eg)
    {
      return calculate(eg);
    }
  else
    {
      ATH_MSG_ERROR ( " Could not cast to const egamma " );
      return m_resultDummy;
    }
}


#endif

