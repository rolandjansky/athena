/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef __ASGELECTRONCHARGEIDSELECTORTOOL__
#define __ASGELECTRONCHARGEIDSELECTORTOOL__
// Atlas includes
#include "AsgTools/AsgTool.h"
#include "MVAUtils/BDT.h"
#include "ElectronPhotonSelectorTools/IAsgElectronLikelihoodTool.h"
#include "ElectronPhotonSelectorTools/AsgElectronLikelihoodTool.h"
#include <unordered_map>

class AsgElectronChargeIDSelectorTool : public asg::AsgTool, 
					virtual public IAsgElectronLikelihoodTool
{
  ASG_TOOL_CLASS2(AsgElectronChargeIDSelectorTool, IAsgElectronLikelihoodTool, IAsgSelectionTool)

public:
  /** Standard constructor */
  AsgElectronChargeIDSelectorTool( const std::string myname);


  /** Standard destructor */
  virtual ~AsgElectronChargeIDSelectorTool();
public:
  /** Gaudi Service Interface method implementations */
  virtual StatusCode initialize();

  /** Gaudi Service Interface method implementations */
  virtual StatusCode finalize();

  // Main methods for IAsgSelectorTool interface
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
    // return m_rootTool->getTAccept();
    return m_acceptDummy;
  }


  /** Method to get the plain TResult */
  inline virtual const Root::TResult& getTResult( ) const
  {
    // return m_rootTool->getTResult();
    return m_resultDummy;
  }

  /// Get the name of the current operating point
  inline virtual std::string getOperatingPointName( ) const
  // std::string AsgElectronChargeIDSelectorTool::getOperatingPointName() const
  {    return m_WorkingPoint;  };
  
  // Private methods
private:
  /// Get the number of primary vertices
  unsigned int getNPrimVertices() const;

  //BDT instances for different ID operating points (Tight, Medium, Loose) and the vector corresponds to n-fold
  const unsigned m_nfold  =1;
  const unsigned m_bdt_index=0;
  std::vector<MVAUtils::BDT*> m_v_bdts;

  TString m_pid_name;
  float m_cutOnBDT;

  int m_cutPosition_bdt;
  int m_resultPosition_bdt;

  /** Working Point */
  std::string m_WorkingPoint;

  /** A dummy return TAccept object */
  Root::TAccept m_acceptDummy;

  /** A dummy return TResult object */
  Root::TResult m_resultDummy;

  /** A BDT to return TResult/TAccept object */
  mutable Root::TAccept m_acceptBDT;
  mutable Root::TResult m_resultBDT;

  /// Whether to use the PV (not available for trigger)
  bool m_usePVCont;

  /// defualt nPV (when not using PVCont)
  unsigned int m_nPVdefault;

  /// The primary vertex container name
  std::string m_primVtxContName;

  /// The input ROOT file name that holds the PDFs
  std::string m_trainingFile;

  /// Flag for calo only LH
  //bool m_caloOnly;

  // BDT input variables
  mutable std::unordered_map<std::string,float> m_map_inputs;
  mutable std::vector<std::string> m_inputVars;

  bool SetVariableRefs(std::vector<std::string> inputs, std::vector<float*> &inputPointers);

}; // End: class definition





#endif

