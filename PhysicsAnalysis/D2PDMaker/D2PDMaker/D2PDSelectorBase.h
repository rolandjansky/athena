/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef D2PDMAKER_D2PDSELECTORBASE_H
#define D2PDMAKER_D2PDSELECTORBASE_H

//=============================================================================
/**
   @package: D2PDMaker
   
   @class D2PDSelectorBase
   
   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>
   
   @date June 2009
   
   @brief Base class of all selection algorithms.
   
   Class to select good objects from an input container (which can either
   be anything inheriting from INavigable4Momentum or a container of 
   ElementLinks poining to an INavigable4MomentumCollection)
   and write out the good ones as a new container of
   ElementLinks pointing to the good INavigable4Momentum objects
   in the original input container.
   This is a templated base class. The first argument in the template T 
   stands for the object type you want to analyze, e.g., Analysis::Electron 
   and the second argument CT stands for the container/collection where 
   these objects are stored, e.g., ElectronContainer.
*/
//=============================================================================


// STL includes
#include <string>
#include <vector>
#include <map>
#include <cfloat>
#include <climits>
#include <math.h>
#include <typeinfo>
#include <iostream>
#include <utility>

// BOOST includes
//#include "boost/format.hpp"

// Gaudi includes
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ListItem.h"

// EventKernel includes
#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/INavigable4MomentumCollection.h"
#include "NavFourMom/INav4MomLink.h"
#include "NavFourMom/INav4MomLinkContainer.h"
#include "EventKernel/IParticle.h"
#include "NavFourMom/IParticleContainer.h"
#include "NavFourMom/INav4MomAssocs.h"

// Athena includes
#include "AthenaBaseComps/AthFilterAlgorithm.h"

// The AthenaBarCode, used to check if two particles are identical
#include "Navigation/IAthenaBarCode.h"

// The needed ElementLink stuff
#include "AthContainers/DataVector.h"
#include "AthLinks/ElementLink.h" 
#include "CLIDSvc/CLASS_DEF.h"

// Include official tools
#include "AnalysisUtils/ISelector.h"
#include "AnalysisUtils/KinematicSelector.h"
#include "AnalysisUtils/KinematicSelectorCore.h"
#include "AssociationKernel/IAssociationTool.h"

#include "PATCore/IAthSelectorTool.h"
#include "PATCore/TAccept.h"

// Include the filter tool
#include "D2PDMaker/FilterTool.h"

//#include "tauEvent/TauJet.h"



template < class T, class CT >
class D2PDSelectorBase : public AthFilterAlgorithm
{
public:

  /** Default constructor */
  D2PDSelectorBase( const std::string& name, ISvcLocator* pSvcLocator );

  /** Default destructor */
  virtual ~D2PDSelectorBase() {};
  

public:
  /** Standard Gaudi Service Interface method implementations:
      initialize the job. */
  virtual StatusCode initialize();

  /** Standard Gaudi Service Interface method implementations:
      execute this in every event. */
  virtual StatusCode execute();

  /** Standard Gaudi Service Interface method implementations:
      finalize after event loop is over. */
  virtual StatusCode finalize();


public:
  /** In case a derived class needs to initialize something more, 
      use this method. */
  virtual StatusCode initializeMore();

  /** In case one needs to do more in the event loop, use this method.
      This method will be executed ONCE PER EVENT! */
  virtual StatusCode executeMore();

  /** In case one needs to do more in the object loop, use this method.
      This method will be executed ONCE PER OBJECT!
      The first argument is the current object (particle) at hand.
      The second argument is the bool return to tell if the object 
      passed the selection criteria. */
  virtual StatusCode processObject( const T* object,
                                    bool &isPassed );

  /** In case one needs to do more after the event loop, use this method. */
  virtual StatusCode finalizeMore();



  // The derived classes can use these protected variables  
protected:
  /** Get a handle on the kinematic selector tool */
  ToolHandle<KinematicSelector> m_kinSelectorTool;


  /** Get a handle on the filter tool */
  ToolHandle<FilterTool> m_filterTool;


  /** Get a handle on the ISelectorCore tools; an array of tools */
  ToolHandleArray<ISelector> m_selectionTools;


  /** Get a handle on the PATCore tools; an array of tools */
  ToolHandleArray<IAthSelectorTool> m_selectorTools;


  /** Get a handle on the AssociationTools; an array of tools */
  ToolHandleArray<IAssociationTool> m_associationTools;


protected:
  /** Printing the configuration if true*/
  bool m_printSetup;

  /** Printing the summary if true*/
  bool m_printSummary;

  /** Input collection/container name  */
  std::string  m_inCollKey;

  /** Output collection name of the Object Container */
  std::string  m_outCollKey;
  
  /** Output collection name of the INav4MomLinkContainer */
  std::string  m_outLinkCollKey;


protected:
  /** Gets set to true if the input collection is a vector of ElementLinks */
  bool m_inputIsLinkColl;


  /** Calculates the associations (matches), even if the input collection is a link collection.
      This is needed because when the input is a link collection, the original object container
      may not have the associations calculated for each object. One thus gets an inhomogeneous
      container. This should only be used if the user really knows what s/he is doing! */
  bool m_forceExecuteAssociation;

  /** Create the vector of INav4MomAssocs maps that will hold the results of matching */
  std::vector< INav4MomAssocs* > m_vecINav4MomAssocs;

  /** List of the association names to be stored in the output file (INav4MomAssocs) */
  std::vector< std::string > m_outAssoCollKeys;

  /** List of the minimum number of associations an object has to have to pass this selection */
  std::vector< int > m_assoMinCutList;

  /** List of the maximum number of associations an object has to have to pass this selection */
  std::vector< int > m_assoMaxCutList;


private:
  //------------------------------------------------------
  // Call-back methods
  //------------------------------------------------------

  /// Call-back method to configure the cut properties (min/max): pt
  void setupPt( Property& prop );

  /// Call-back method to configure the cut properties (min/max): p
  void setupP( Property& prop );

  /// Call-back method to configure the cut properties (min/max): et
  void setupEt( Property& prop );

  /// Call-back method to configure the cut properties (min/max): e
  void setupE( Property& prop );

  /// Call-back method to configure the cut properties (min/max): eta
  void setupEta( Property& prop );

  /// Call-back method to configure the cut properties (min/max): |eta|
  void setupAbsEta( Property& prop );

  /// Call-back method to configure the cut properties (min/max): eta veto ranges
  void setupEtaVeto( Property& prop );

  /// Call-back method to configure the cut properties (min/max): phi
  void setupPhi( Property& prop );

  /// Call-back method to configure the cut properties (min/max): phi veto ranges
  void setupPhiVeto( Property& prop );

  /// Call-back method to configure the cut properties (min/max): mass
  void setupMass( Property& prop );




protected:

  /// Print the internal cutflow to the log file at finalize
  bool m_printInternalCutflow;

  /// The internal list of how many objects passed the cuts for a given event
  std::vector< unsigned int > m_objCutFlow;

  /// The internal list of cuts and how many events passed the cuts
  std::vector< std::pair< std::string, std::pair< unsigned int, unsigned int > > > m_eventCutFlow;


  //------------------------------------------------------
  // Selections
  //------------------------------------------------------
  /** minimum number of electrons passing the selection */
  unsigned int m_minNumberPassed;

  /** maximum number of electrons passing the selection */
  unsigned int m_maxNumberPassed;

private:
  /// do transverse momentum cut
  bool m_doPt;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxPt;

  /** minimum transverse momentum */
  DoubleProperty m_ptMin;

  /** maximum transverse momentum */
  DoubleProperty m_ptMax;


  /// do momentum cut
  bool m_doP;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxP;

  /** minimum momentum */
  DoubleProperty m_pMin;

  /** maximum momentum */
  DoubleProperty m_pMax;


  /// do transverse energy cut
  bool m_doEt;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxEt;

  /** minimum transverse energy */
  DoubleProperty m_etMin;

  /** maximum transverse energy */
  DoubleProperty m_etMax;


  /// do energy cut
  bool m_doE;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxE;

  /** minimum energy */
  DoubleProperty m_eMin;

  /** maximum energy */
  DoubleProperty m_eMax;


  /// do eta cut
  bool m_doEta;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxEta;

  /** minimum eta */
  DoubleProperty m_etaMin;

  /** maximum eta */
  DoubleProperty m_etaMax;


  /// do |eta| cut
  bool m_doAbsEta;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxAbsEta;

  /** minimum |eta| */
  DoubleProperty m_absEtaMin;

  /** maximum |eta| */
  DoubleProperty m_absEtaMax;


  /// do eta veto ranges cut
  bool m_doEtaVeto;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxEtaVeto;

  /** Eta ranges in which the particle should NOT be */
  StringProperty m_etaVetoRanges;


  /// do phi cut
  bool m_doPhi;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxPhi;

  /** minimum phi */
  DoubleProperty m_phiMin;

  /** maximum phi */
  DoubleProperty m_phiMax;


  /// do phi veto ranges cut
  bool m_doPhiVeto;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxPhiVeto;

  /** Phi ranges in which the particle should NOT be */
  StringProperty m_phiVetoRanges;


  /// do mass cut
  bool m_doMass;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxMass;

  /** minimum mass */
  DoubleProperty m_massMin;

  /** maximum mass */
  DoubleProperty m_massMax;


  /// do ISelectorCore tools cuts
  bool m_doSelectionTools;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxSelectionTools;

  /// do IAthSelectorTool cuts
  bool m_doSelectorTools;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxSelectorTools;

  /// do association tools cuts
  bool m_doAssociationTools;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxAssociationTools;

  /// Index of where in the internal cutflow the final count is bookkept
  unsigned int m_idxFinal;




private:

  /** Flag to dump the content of StoreGate after each event */
  bool m_dumpStoreGate;

  /** Internal event counter */
  unsigned long m_nEventsProcessed;

  /** Internal object counter */
  unsigned long m_nObjectsProcessed;

  /** Internal object passedcounter */
  unsigned long m_nObjectsPassed;

  /// Number of digits to print for the internal cutflow
  unsigned int m_nDigits;


}; // End class D2PDSelectorBase




///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////
template < class T, class CT >
inline void D2PDSelectorBase< T, CT >::setupPt( Property& /*prop*/ )
{
  m_doPt = true;
  return;
}

template < class T, class CT >
inline void D2PDSelectorBase< T, CT >::setupP( Property& /*prop*/ )
{
  m_doP = true;
  return;
}

template < class T, class CT >
inline void D2PDSelectorBase< T, CT >::setupEt( Property& /*prop*/ )
{
  m_doEt = true;
  return;
}

template < class T, class CT >
inline void D2PDSelectorBase< T, CT >::setupE( Property& /*prop*/ )
{
  m_doE = true;
  return;
}

template < class T, class CT >
inline void D2PDSelectorBase< T, CT >::setupEta( Property& /*prop*/ )
{
  m_doEta = true;
  return;
}

template < class T, class CT >
inline void D2PDSelectorBase< T, CT >::setupAbsEta( Property& /*prop*/ )
{
  m_doAbsEta = true;
  return;
}

template < class T, class CT >
inline void D2PDSelectorBase< T, CT >::setupEtaVeto( Property& /*prop*/ )
{
  m_doEtaVeto = true;
  return;
}

template < class T, class CT >
inline void D2PDSelectorBase< T, CT >::setupPhi( Property& /*prop*/ )
{
  m_doPhi = true;
  return;
}

template < class T, class CT >
inline void D2PDSelectorBase< T, CT >::setupPhiVeto( Property& /*prop*/ )
{
  m_doPhiVeto = true;
  return;
}

template < class T, class CT >
inline void D2PDSelectorBase< T, CT >::setupMass( Property& /*prop*/ )
{
  m_doMass = true;
  return;
}





//=============================================================================
// Define the implementation of the methods here in the header file.
// This is done since we are dealing with a templated base class!
//=============================================================================

#include "D2PDMaker/D2PDSelectorBase.icc"


#endif
