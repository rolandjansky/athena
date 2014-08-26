/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef D2PDMAKER_D2PDPARTICLECOMBINER_H
#define D2PDMAKER_D2PDPARTICLECOMBINER_H

//============================================================================
// Name: D2PDParticleCombiner.h
//
/**
   @class D2PDParticleCombiner

   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>

   @date April 2009

   @brief Combine particles to composite particles

   Class to combine two or more particles from a given list of
   input containers. All combinations will be tried and only the
   ones passing the used selections will be written to UserData.
*/
//=============================================================================


// STL includes
#include <string>
#include <vector>

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

// Athena includes
#include "AthenaBaseComps/AthFilterAlgorithm.h"

// EventKernel includes
#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/INavigable4MomentumCollection.h"
#include "NavFourMom/INav4MomLinkContainer.h"
#include "NavFourMom/INav4MomAssocs.h"

// CompositeParticle
#include "ParticleEvent/CompositeParticleContainer.h"
#include "ParticleEvent/CompositeParticle.h"

// Include the UserDataSvc
#include "AthenaKernel/IUserDataSvc.h"

// Include the filter tool
#include "D2PDMaker/FilterTool.h"
#include "AnalysisUtils/KinematicSelector.h"

// Include official tools
#include "AnalysisUtils/ISelector.h"
#include "AnalysisUtils/KinematicSelector.h"
#include "AnalysisUtils/KinematicSelectorCore.h"
#include "PATCore/IUserDataCalcTool.h"
#include "AssociationKernel/IAssociationTool.h"

// MC Truth 
#include "GeneratorObjects/McEventCollection.h"


class D2PDParticleCombiner : public AthFilterAlgorithm 
{
public:

  /** Default constructor */
  D2PDParticleCombiner(const std::string& name, ISvcLocator* pSvcLocator);

  /** Default destructor */
  virtual ~D2PDParticleCombiner() ;
  

public:
  /** Gaudi Service Interface method implementations - initialize */
  StatusCode initialize() ;

  /** Gaudi Service Interface method implementations - execute */
  StatusCode execute() ;

  /** Gaudi Service Interface method implementations - finalize */
  StatusCode finalize() ;


private:

  /** Build the composite candidates */
  StatusCode buildComposite( CompositeParticleContainer* outContainer,
                             INav4MomLinkContainer& anINav4MomLinkList ) ;

  /** Check if the composite particle at hand was already found before */
  bool compositeParticleAlreadyFound( CompositeParticleContainer* compContainer, 
                                      CompositeParticle* compPart );

  /** Check that two particles are not the same or, if they are 
      composite particles, that they don't share the same constitutents */
  bool shareSameConstituents( const INavigable4Momentum* part1,
                              const INavigable4Momentum* part2 );

  /** Check that two particles are not the same or, if they are 
      composite particles, that they don't share the same constitutents */
  bool shareSameConstituents( const INavigable4Momentum* part1,
                              const CompositeParticle* compPart2 );

  /** Check that two particles are not the same or, if they are 
      composite particles, that they don't share the same constitutents */
  bool shareSameConstituents( const CompositeParticle* compPart1,
                              const CompositeParticle* compPart2 );

  /** Do the MC Truth selections */
  bool mcTruthSelections( const CompositeParticle* compPart );


private:
  //------------------------------------------------------
  // Tools and Containers
  //------------------------------------------------------
  /** Printing the configuration if true*/
  bool m_printSetup;

  /** Printing the summary if true*/
  bool m_printSummary;


  /** List of input collection names */
  std::vector< std::string >  m_inCollKeyList;
  
  /** Input collection name one */
  std::string  m_inCollKey1;
  
  /** Input collection name two */
  std::string  m_inCollKey2;

  /** Input collection name two */
  std::string  m_inCollKey3;

  /** Output collection name (the composite particles) */
  std::string  m_outCollKey;


  /** MCEventCollection name */
  std::string  m_mcEventCollKey;

  /** MCEventCollection */
  const McEventCollection* m_mcEventColl;


  /** Get a handle on the UserDataSvc */
  ServiceHandle<IUserDataSvc> m_userDataSvc;


  /** Get a handle on the kinematic selector tool */
  ToolHandle<KinematicSelector> m_kinSelectorTool;


  /** Get a handle on the filter tool */
  FilterTool* m_filterTool;


  /** Get a handle on the ISelectorCore tools; an array of tools */
  ToolHandleArray<ISelector> m_selectionTools;


  /** Get a handle on the AssociationTools; an array of tools */
  ToolHandleArray<IAssociationTool> m_associationTools;


  /** Get a handle on the UserDataCalcTools; an array of tools */
  ToolHandleArray<IUserDataCalcTool> m_userDataCalcTools;


private:
  /** Create the vector of INav4MomAssocs maps that will hold the results of matching */
  std::vector< INav4MomAssocs* > m_vecINav4MomAssocs;

  /** List of the association names to be stored in the output file (INav4MomAssocs) */
  std::vector< std::string > m_outAssoCollKeys;


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
  /** minimum number of composite particles passing the selection */
  unsigned int m_minNumberPassed;

  /** maximum number of electrons passing the selection */
  unsigned int m_maxNumberPassed;

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

  /** minimum |eta| */
  double m_absEtaMin;

  /** maximum |eta| */
  double m_absEtaMax;

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


  /** Require that the truth mother has the same PDG_ID defined with compositeParticlePDG_ID */
  bool m_mcTruthRequireSameMotherPdgID;

  /** Require that the truth mother has the identical MC Truth barcode */
  bool m_mcTruthRequireSameMotherBarcode;


  /** Internal event counter */
  unsigned long m_nEventsProcessed;

  /** Is this a two-body decay that we want to build? */
  bool m_isTwoBodyDecay;

  /** Flag to dump the content of StoreGate after each event */
  bool m_dumpStoreGate;



private:
  //------------------------------------------------------
  // Internal variables
  //------------------------------------------------------
  /** Create a vector that will hold all the INav4MomLinks for one CompositeParticle */
  INav4MomLinkContainer m_anINav4MomLinkList;


  /** Create a vector of LinkContainers and a map of their names */
  std::vector< INav4MomLinkContainer* > m_inputLinkContainerList;

  /** create a vector that will say if the INav4MomLinkContainer needs to be deleted manually */
  std::vector< bool > m_inputLinkContainerListDelete;

  /** A map to store the name associated to each input link container object */
  std::map< INav4MomLinkContainer*, std::string > m_inputLinkContainerNames;

  /** A map to store the input container that are already used.
   *  use a map to determine if a given input container
   *  has already been seen, and simply point to the
   *  link container corresponding to  that already-used
   *  input container   */
  std::map< INavigable4MomentumCollection*, INav4MomLinkContainer* > m_alreadyUsedInputContainers;


  /** Vector of the labels/keys of all input containers (all converted to link containers) */
  std::vector<std::string> m_containerLabels;

  /** A map to store the size of each input container */
  std::map<std::string, int> m_containerMaxN;

  /** A map to store the container that are already used */
  std::map< INav4MomLinkContainer*, std::string > m_alreadyUsedContainers;




}; // End class D2PDParticleCombiner


#endif
