/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef D2PDMAKER_D2PDTRUTHPARTICLESELECTOR_H
#define D2PDMAKER_D2PDTRUTHPARTICLESELECTOR_H

//============================================================================
// Name: D2PDTruthParticleSelector.h
//
/**
   @class D2PDTruthParticleSelector

   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>

   @date April 2009

   @brief Select TruthParticles and write out good ones as ElementLinks

   Class to select good TruthParticles from an input container (which should be
   a TruthParticleContainer) and write out the good ones as a new container of
   ElementLinks pointing to the good TruthParticles in the original input container.
*/
//=============================================================================

// Include the header of the base class
#include "D2PDMaker/D2PDSelectorBase.h"

// TruthParticle includes
#include "GeneratorObjects/McEventCollection.h"
#include "McParticleEvent/TruthParticle.h"
#include "McParticleEvent/TruthParticleContainer.h"
#include "AtlasHepMC/GenParticle.h"




class D2PDTruthParticleSelector : public D2PDSelectorBase< TruthParticle, TruthParticleContainer >
{
public:

  /** Default constructor */
  D2PDTruthParticleSelector(const std::string& name, ISvcLocator* pSvcLocator);

  /** Default destructor */
  virtual ~D2PDTruthParticleSelector() ;
  

private:
  /** This method is executed once at the end of the 
      standard athena initialize() method. */
  StatusCode initializeMore();

  /** This method is executed ONCE PER EVENT, right before
      the loop over the electrons begins. */
  //StatusCode executeMore();

  /** This method is executed ONCE FOR EVERY PROCESSED TRUTHPARTICLE. */
  StatusCode processObject( const TruthParticle* part,
                            bool &isPassed );

  /** This method is executed once at the end of the 
      standard athena finalize() method. */
  StatusCode finalizeMore();

  
  /** Get the mother of a TruthParticle */
  int getMotherPDGID( const TruthParticle* part,
                      const TruthParticle* &moth,
                      int counter=0,
                      const TruthParticle* previousPart=NULL );

  /** Get the mother of a TruthParticle */
  bool getDaughters( HepMC::ConstGenParticlePtr genPart, 
                     std::vector<HepMC::ConstGenParticlePtr > &daughters );


private:

  //------------------------------------------------------
  // Selections
  //------------------------------------------------------

  /// do PDG_ID
  bool m_doPDGIDList;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxPDGIDList;

  /** The list of allowed PDG_IDs */
  std::vector<int> m_pdgIDList;

  /** Allow selecting also the anti particle (default: true) */
  bool m_allowAntiPart;


  /// do PDG_ID for origin particle
  bool m_doOriginPDGIDList;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxOriginPDGIDList;

  /** The list of allowed PDG_IDs of the mother or grand-mother or grand-grand... */
  std::vector<int> m_originPdgIDList;

  /** Allow selecting also the anti particle (default: true) 
      of the mother or grand-mother or grand-grand... */
  bool m_allowOriginAntiPart;

  /** The maximum number of generations where the origin search
      can find the required particles */
  int m_maxNumGenerations;


  /// do PDG_ID for daughter particles
  bool m_doDaughterPDGIDList;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxDaughterPDGIDList;

  /** The list of allowed PDG_IDs of all daughters. If one of the daughters has a 
      different PDG_ID, it will fail */
  std::vector<int> m_daughterPdgIDList;

  /** Allow selecting also the anti particle (default: true)
      for all the daughters */
  bool m_allowDaughterAntiPart;


  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxRemoveDocLine;

  /** remove the documentation lines, i.e., status code == 3 */
  bool m_removeDocLine;


  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxKeepOnlyGenerator;

  /** Only generator particles are kept */
  bool m_keepOnlyGenerator;


  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxKeepOnlyStable;

  /** Only stable particles (at generator level) are kept */
  bool m_keepOnlyStable;


  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxRemoveSelfDecay;

  /** Remove a particle that is decaying into itself */
  bool m_removeSelfDecay;


  /// do accept StatusCode
  bool m_doAcceptStatusCodeList;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxAcceptStatusCodeList;

  /** Accept only particles with these status codes */
  std::vector<int> m_acceptStatusCodeList;


  /// do reject StatusCode
  bool m_doRejectStatusCodeList;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxRejectStatusCodeList;

  /** Reject only particles with these status codes */
  std::vector<int> m_rejectStatusCodeList;


}; // End class D2PDTruthParticleSelector


#endif
