/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef D2PDMAKER_D2PDLEPTONNEUTRINOCOMBINER_H
#define D2PDMAKER_D2PDLEPTONNEUTRINOCOMBINER_H

//============================================================================
// Name: D2PDLeptonNeutrinoCombiner.h
//
/**
   @class D2PDLeptonNeutrinoCombiner

   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>

   @date April 2009

   @brief Select neutrinos and write out good ones as ElementLinks

   Class to select good neutrinos from an input container (which can either
   be an neutrino container or a container of ElementLinks poining to an
   neutrino container) and write out the good ones as a new container of
   ElementLinks pointing to the good neutrinos in the original input container.
*/
//=============================================================================


// STL includes
#include <string>
#include <vector>

// Gaudi includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ListItem.h"

// Athena includes
#include "AthenaBaseComps/AthFilterAlgorithm.h"

// DataModel
#include "DataModel/DataVector.h"
#include "DataModel/ElementLink.h"
#include "DataModel/ElementLinkVector.h"
#include "SGTools/CLASS_DEF.h"

// INav4Mom includes
#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/INavigable4MomentumCollection.h"
#include "NavFourMom/INav4MomLink.h"

// neutrino includes
#include "MissingETEvent/MissingET.h"
#include "ParticleEvent/Neutrino.h"
#include "ParticleEvent/NeutrinoContainer.h"

// Include official tools
#include "AnalysisUtils/KinematicSelector.h"
#include "AnalysisUtils/KinematicSelectorCore.h"

// Include the filter tool
#include "D2PDMaker/FilterTool.h"

//class IIsolationTool;
class IAnalysisTools;


class D2PDLeptonNeutrinoCombiner : public AthFilterAlgorithm
{

public:

  /** Default constructor */
  D2PDLeptonNeutrinoCombiner(const std::string& name, ISvcLocator* pSvcLocator);

  /** Default destructor */
  virtual ~D2PDLeptonNeutrinoCombiner() ;
  

public:
  /** Gaudi Service Interface method implementations */
  StatusCode initialize() ;

  /** Gaudi Service Interface method implementations */
  StatusCode execute() ;

  /** Gaudi Service Interface method implementations */
  StatusCode finalize() ;


private:

  /** Method for running the mass-constraint neutrino finding */
  StatusCode findNeutrinosWithMassConstraint( const INavigable4Momentum* lep, 
                                              const double etxMiss, 
                                              const double etyMiss,
                                              NeutrinoContainer* outContainer );

  /** Helper method to select the neutrinos that pass the selection */
  bool selectNeutrinos( Neutrino* neutrino ); 


  /** Printing the configuration if true*/
  bool m_printSetup;

  /** Printing the summary if true*/
  bool m_printSummary;


  /** Input collection name  */
  std::string  m_inCollKey;

  /** Output collection name of the Neutrinos */
  std::string  m_outNeuCollKey;
  
  /** Output collection name of the ElementLinks to Neutrinos */
  std::string  m_outNeuLinkCollKey;
  
  /** Output collection name of the CompositeParticles */
  std::string  m_outCollKey;
  
  

private:

  /** Get a handle on the kinematic selector tool */
  ToolHandle<KinematicSelector> m_kinSelectorTool;

  /** Get a handle on the kinematic selector tool for neutrinos */
  ToolHandle<KinematicSelector> m_neutrinoKinSelectorTool;

  /** Get a handle on the kinematic selector tool for missing et*/
  ToolHandle<KinematicSelector> m_missingEtKinSelectorTool;

  /** Get a handle on the filter tool */
  FilterTool* m_filterTool;


private:
  //------------------------------------------------------
  // Ways to determine the neutrino(s)
  //------------------------------------------------------
  /** mass for the mass constraint */
  double m_wMass;

  /** Input lepton (link) collection name  */
  std::string  m_inLepCollKey;

  /** deal with possible negative solutions of the mass constraint */
  bool m_dealWithNegSol;


private:
  //------------------------------------------------------
  // Output properties
  //------------------------------------------------------
  /** The PDG_ID of the CompositeParticle */
  int m_pdgId;
  

private:
  //------------------------------------------------------
  // Selections
  //------------------------------------------------------
  /** minimum number of neutrinos passing the selection */
  unsigned int m_minNumberPassed;

  /** maximum number of neutrinos passing the selection */
  unsigned int m_maxNumberPassed;


  // for missing et
  //-----------------

  /** minimum missing transverse energy */
  double m_missingEtMin;

  /** maximum missing transverse energy */
  double m_missingEtMax;


  /** minimum sum Et */
  double m_sumEtMin;

  /** maximum sum Et */
  double m_sumEtMax;


  /** minimum phi of missing et */
  double m_missingEtPhiMin;

  /** maximum phi of missing et */
  double m_missingEtPhiMax;

  /** Phi ranges in which the  missing et should NOT be */
  std::string  m_missingEtPhiVetoRanges;

  

  // for neutrinos
  //-----------------

  /** minimum transverse momentum of the neutrino */
  double m_neutrinoPtMin;

  /** maximum transverse momentum of the neutrino */
  double m_neutrinoPtMax;


  /** minimum momentum of the neutrino */
  double m_neutrinoPMin;

  /** maximum momentum of the neutrino */
  double m_neutrinoPMax;


  /** minimum transverse energy of the neutrino */
  double m_neutrinoEtMin;

  /** maximum transverse energy of the neutrino */
  double m_neutrinoEtMax;


  /** minimum energy of the neutrino */
  double m_neutrinoEMin;

  /** maximum energy of the neutrino */
  double m_neutrinoEMax;


  /** minimum eta of the neutrino */
  double m_neutrinoEtaMin;

  /** maximum eta of the neutrino */
  double m_neutrinoEtaMax;

  /** Eta ranges in which the the neutrino should NOT be */
  std::string  m_neutrinoEtaVetoRanges;


  /** minimum phi of the neutrino */
  double m_neutrinoPhiMin;

  /** maximum phi of the neutrino */
  double m_neutrinoPhiMax;

  /** Phi ranges in which the neutrino should NOT be */
  std::string  m_neutrinoPhiVetoRanges;




  // for the composite particles
  //-----------------

  /** minimum transverse momentum */
  double m_ptMin;

  /** maximum transverse momentum */
  double m_ptMax;


  /** minimum momentum */
  double m_pMin;

  /** maximum momentum */
  double m_pMax;


  /** minimum transverse energy */
  double m_etMin;

  /** maximum transverse energy */
  double m_etMax;


  /** minimum energy */
  double m_eMin;

  /** maximum energy */
  double m_eMax;


  /** minimum eta */
  double m_etaMin;

  /** maximum eta */
  double m_etaMax;

  /** Eta ranges in which the particle should NOT be */
  std::string  m_etaVetoRanges;


  /** minimum phi */
  double m_phiMin;

  /** maximum phi */
  double m_phiMax;

  /** Phi ranges in which the particle should NOT be */
  std::string  m_phiVetoRanges;


  /** minimum mass */
  double m_massMin;

  /** maximum mass */
  double m_massMax;


  /** minimum deltaEta */
  double m_deltaEtaMin;

  /** maximum deltaEta */
  double m_deltaEtaMax;

  /** minimum deltaPhi */
  double m_deltaPhiMin;

  /** maximum deltaPhi */
  double m_deltaPhiMax;

  /** minimum deltaR */
  double m_deltaRMin;

  /** maximum deltaR */
  double m_deltaRMax;

  /** minimum charge of the composite particle */
  int m_chargeMin;

  /** maximim charge of the composite particle */
  int m_chargeMax;

  /** allow charge conjugate */
  bool m_allowChargeConj;




private:

  /** Create a temporary neutrino object */
  Neutrino* m_tmpNeutrino;

  /** Flag to dump the content of StoreGate after each event */
  bool m_dumpStoreGate;

  /** Internal event counter */
  unsigned long m_nEventsProcessed;

}; // End class D2PDLeptonNeutrinoCombiner


#endif
