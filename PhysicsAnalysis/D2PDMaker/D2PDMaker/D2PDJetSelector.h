/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef D2PDMAKER_D2PDJETSELECTOR_H
#define D2PDMAKER_D2PDJETSELECTOR_H

//============================================================================
// Name: D2PDJetSelector.h
//
/**
   @class D2PDJetSelector

   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>

   @date April 2009

   @brief Select jets and write out good ones as ElementLinks

   Class to select good jets from an input container (which can either
   be an jet container or a container of ElementLinks poining to an
   jet container) and write out the good ones as a new container of
   ElementLinks pointing to the good jets in the original input container.
*/
//=============================================================================

// Include the header of the base class
#include "D2PDMaker/D2PDSelectorBase.h"

// STL includes
#include <string>
#include <vector>

// Gaudi includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

// jet includes
#include "JetEvent/JetCollection.h"
#include "JetEvent/Jet.h"

// Include official tools
#include "AnalysisUtils/KinematicSelector.h"

// Forward declarations
class JetVariablesTool;

// Class definition
class D2PDJetSelector : public D2PDSelectorBase< Jet, JetCollection >
{
public:

  /** Default constructor */
  D2PDJetSelector(const std::string& name, ISvcLocator* pSvcLocator);

  /** Default destructor */
  virtual ~D2PDJetSelector() ;
  

private:
  /** This method is executed once at the end of the 
      standard athena initialize() method. */
  StatusCode initializeMore();

  /** This method is executed ONCE PER EVENT, right before
      the loop over the electrons begins. */
  //StatusCode executeMore();

  /** This method is executed ONCE FOR EVERY PROCESSED JET. */
  StatusCode processObject( const Jet* jet,
                            bool &isPassed );

  /** This method is executed once at the end of the 
      standard athena finalize() method. */
  StatusCode finalizeMore();


private:

  /** Get a handle on the kinematic selector tool for jets with an alternate SignalState */
  ToolHandle<KinematicSelector> m_signalStateKinSelectorTool;

  ToolHandle<JetVariablesTool> m_jetVariablesTool;



  //------------------------------------------------------
  // Call-back methods
  //------------------------------------------------------

  /// Call-back method to configure the cut properties (min/max): 
  void setupJetVertexFraction( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): 
  void setupFlavorTag( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): 
  void setupEMFrac( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): 
  void setupPSFrac( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): 
  void setupN90( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): 
  void setupSamplingMax( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): 
  void setupFCor( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): 
  void setupScaledJESEt( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): 
  void setupScaledJESE( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): 
  void setupScaledJESPt( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): 
  void setupScaledJESP( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): 
  void setupScaledJESMass( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): 
  void setupSignalStateEt( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): 
  void setupSignalStateE( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): 
  void setupSignalStatePt( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): 
  void setupSignalStateP( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): 
  void setupSignalStateEta( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): 
  void setupSignalStateAbsEta( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): 
  void setupSignalStateEtaVeto( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): 
  void setupSignalStatePhi( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): 
  void setupSignalStatePhiVeto( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): 
  void setupSignalStateMass( Gaudi::Details::PropertyBase& prop );



  //------------------------------------------------------
  // Selections
  //------------------------------------------------------

  /// do  cut
  bool m_doJetShapeList;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxJetShapeList;

  /** list of jet shapes */
  std::vector< std::string > m_jetShapeList;

  /** list of minimum cut values for the jet shapes list */
  std::vector<double> m_jetShapeMinList;

  /** list of maximum cut values for the jet shapes list */
  std::vector<double> m_jetShapeMaxList;



  /// do  cut
  bool m_doJVF;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxJVF;

  /** minimum |JetVertexFraction| */
  DoubleProperty m_absJVFMin;

  /** maximum |JetVertexFraction| */
  DoubleProperty m_absJVFMax;

  /** name of the JetVertexFraction jet moment to be used */
  std::string m_JVFName;



  /// do  cut
  bool m_doFlavorTag;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxFlavorTag;

  /** minimum flavor tag weight */
  DoubleProperty m_flavorTagWeightMin;

  /** maximum flavor tag weight */
  DoubleProperty m_flavorTagWeightMax;

  /** name of a specific flavor tagger to be used */
  std::string m_flavorTaggerName;


  /// do  cut
  bool m_doEMFrac;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxEMFrac;

  /** minumum jet EM fraction */
  DoubleProperty m_emFractionMin;

  /** maxumum jet EM fraction */
  DoubleProperty m_emFractionMax;


  /// do  cut
  bool m_doPSFrac;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxPSFrac;

  /** minumum jet Presampler fraction */
  DoubleProperty m_psFractionMin;

  /** maxumum jet Presampler fraction */
  DoubleProperty m_psFractionMax;


  /// do  cut
  bool m_doN90;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxN90;

  /** minumum jet n90 cut */
  DoubleProperty m_n90Min;

  /** maxumum jet n90 cut */
  DoubleProperty m_n90Max;

  /** n90 moment name */
  std::string m_n90Name;


  /// do  cut
  bool m_doSamplingMax;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxSamplingMax;

  /** list of integer values that should be vetoed for the SamplingMax jet moment/shape */
  std::vector<int> m_vetoSamplingMaxList;

  /** SamplingMax moment name */
  std::string m_samplingMaxName;


  /// do  cut
  bool m_doFCor;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxFCor;

  /** jetFCor=BAD_CELLS_CORR_E/E_jet(emscale) minimum cut value */
  DoubleProperty m_fCorMin;

  /** jetFCor=BAD_CELLS_CORR_E/E_jet(emscale) maximum cut value */
  DoubleProperty m_fCorMax;





  //-----------------------
  // jet quantities at a scaled jet energy scale
  //-----------------------

  /** Name of the jet moment that stores the jet energy scale factor */
  std::string m_jetScaleMoment;


  /// do  cut
  bool m_doScaledJESEt;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxScaledJESEt;

  /** jet minimum transverse energy at a scaled jet energy scale */
  DoubleProperty m_scaledJESEtMin;

  /** jet maximum transverse energy at a scaled jet energy scale */
  DoubleProperty m_scaledJESEtMax;


  /// do  cut
  bool m_doScaledJESE;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxScaledJESE;

  /** jet minimum energy at a scaled jet energy scale */
  DoubleProperty m_scaledJESEMin;

  /** jet maximum energy at a scaled jet energy scale */
  DoubleProperty m_scaledJESEMax;


  /// do  cut
  bool m_doScaledJESPt;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxScaledJESPt;

  /** jet minimum transverse momentum at a scaled jet energy scale */
  DoubleProperty m_scaledJESPtMin;

  /** jet maximum transverse momentum at a scaled jet energy scale */
  DoubleProperty m_scaledJESPtMax;


  /// do  cut
  bool m_doScaledJESP;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxScaledJESP;

  /** jet minimum momentum at a scaled jet energy scale */
  DoubleProperty m_scaledJESPMin;

  /** jet maximum momentum at a scaled jet energy scale */
  DoubleProperty m_scaledJESPMax;


  /// do  cut
  bool m_doScaledJESMass;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxScaledJESMass;

  /** jet minimum mass at a scaled jet energy scale */
  DoubleProperty m_scaledJESMassMin;

  /** jet maximum mass at a scaled jet energy scale */
  DoubleProperty m_scaledJESMassMax;



  //-----------------------
  // jet quantities at a different signal state
  //-----------------------

  /** jet signal state. This is defined as an enum in http://alxr.usatlas.bnl.gov/lxr/source/atlas/Event/EventKernel/EventKernel/ISignalState.h?v=head#019 */
  int m_signalState;


  /// do  cut
  bool m_doSignalStateEt;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxSignalStateEt;

  /** jet minimum transverse energy at a different signal state */
  DoubleProperty m_signalStateEtMin;

  /** jet maximum transverse energy at a different signal state */
  DoubleProperty m_signalStateEtMax;


  /// do  cut
  bool m_doSignalStateE;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxSignalStateE;

  /** jet minimum energy at a different signal state */
  DoubleProperty m_signalStateEMin;

  /** jet maximum energy at a different signal state */
  DoubleProperty m_signalStateEMax;


  /// do  cut
  bool m_doSignalStatePt;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxSignalStatePt;

  /** jet minimum transverse momentum at a different signal state */
  DoubleProperty m_signalStatePtMin;

  /** jet maximum transverse momentum at a different signal state */
  DoubleProperty m_signalStatePtMax;


  /// do  cut
  bool m_doSignalStateP;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxSignalStateP;

  /** jet minimum momentum at a different signal state */
  DoubleProperty m_signalStatePMin;

  /** jet maximum momentum at a different signal state */
  DoubleProperty m_signalStatePMax;


  /// do  cut
  bool m_doSignalStateEta;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxSignalStateEta;

  /** jet minimum eta at a different signal state */
  DoubleProperty m_signalStateEtaMin;

  /** jet maximum eta at a different signal state */
  DoubleProperty m_signalStateEtaMax;


  /// do  cut
  bool m_doSignalStateAbsEta;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxSignalStateAbsEta;

  /** jet minimum |eta| at a different signal state */
  DoubleProperty m_signalStateAbsEtaMin;

  /** jet maximum |eta| at a different signal state */
  DoubleProperty m_signalStateAbsEtaMax;


  /// do  cut
  bool m_doSignalStateEtaVeto;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxSignalStateEtaVeto;

  /** jet eta ranges in which the particle should NOT be (at a different signal state) */
  StringProperty m_signalStateEtaVetoRanges;

  
  /// do  cut
  bool m_doSignalStatePhi;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxSignalStatePhi;

  /** jet minimum phi at a different signal state */
  DoubleProperty m_signalStatePhiMin;

  /** jet maximum phi at a different signal state */
  DoubleProperty m_signalStatePhiMax;


  /// do  cut
  bool m_doSignalStatePhiVeto;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxSignalStatePhiVeto;

  /** jet phi ranges in which the particle should NOT be (at a different signal state) */
  StringProperty m_signalStatePhiVetoRanges;
  

  /// do  cut
  bool m_doSignalStateMass;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxSignalStateMass;

  /** jet minimum mass at a different signal state */
  DoubleProperty m_signalStateMassMin;

  /** jet maximum mass at a different signal state */
  DoubleProperty m_signalStateMassMax;


  int m_nJetsProcessed;
  int m_nJetsPassed;


}; // End class D2PDJetSelector



///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////
inline void D2PDJetSelector::setupJetVertexFraction( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doJVF = true;
  return;
}

inline void D2PDJetSelector::setupFlavorTag( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doFlavorTag = true;
  return;
}

inline void D2PDJetSelector::setupEMFrac( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doEMFrac = true;
  return;
}

inline void D2PDJetSelector::setupPSFrac( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doPSFrac = true;
  return;
}

inline void D2PDJetSelector::setupN90( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doN90 = true;
  return;
}

inline void D2PDJetSelector::setupSamplingMax( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doSamplingMax = true;
  return;
}

inline void D2PDJetSelector::setupFCor( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doFCor = true;
  return;
}

inline void D2PDJetSelector::setupScaledJESEt( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doScaledJESEt = true;
  return;
}

inline void D2PDJetSelector::setupScaledJESE( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doScaledJESE = true;
  return;
}

inline void D2PDJetSelector::setupScaledJESPt( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doScaledJESPt = true;
  return;
}

inline void D2PDJetSelector::setupScaledJESP( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doScaledJESP = true;
  return;
}

inline void D2PDJetSelector::setupScaledJESMass( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doScaledJESMass = true;
  return;
}

inline void D2PDJetSelector::setupSignalStateEt( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doSignalStateEt = true;
  return;
}

inline void D2PDJetSelector::setupSignalStateE( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doSignalStateE = true;
  return;
}

inline void D2PDJetSelector::setupSignalStatePt( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doSignalStatePt = true;
  return;
}

inline void D2PDJetSelector::setupSignalStateP( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doSignalStateP = true;
  return;
}

inline void D2PDJetSelector::setupSignalStateEta( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doSignalStateEta = true;
  return;
}

inline void D2PDJetSelector::setupSignalStateAbsEta( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doSignalStateAbsEta = true;
  return;
}

inline void D2PDJetSelector::setupSignalStateEtaVeto( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doSignalStateEtaVeto = true;
  return;
}

inline void D2PDJetSelector::setupSignalStatePhi( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doSignalStatePhi = true;
  return;
}

inline void D2PDJetSelector::setupSignalStatePhiVeto( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doSignalStatePhiVeto = true;
  return;
}

inline void D2PDJetSelector::setupSignalStateMass( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doSignalStateMass = true;
  return;
}


#endif
