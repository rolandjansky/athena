/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef D2PDMAKER_D2PDMISSINGETSELECTOR_H
#define D2PDMAKER_D2PDMISSINGETSELECTOR_H

//============================================================================
// Name: D2PDMissingEtSelector.h
//
/**
   @class D2PDMissingEtSelector

   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>

   @date April 2009

   @brief Select neutrinos and write out good ones as ElementLinks 
   and NeutrinoContainer.

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


class D2PDMissingEtSelector : public AthFilterAlgorithm
{

public:

  /** Default constructor */
  D2PDMissingEtSelector(const std::string& name, ISvcLocator* pSvcLocator);

  /** Default destructor */
  virtual ~D2PDMissingEtSelector() ;
  

public:
  /** Gaudi Service Interface method implementations - initialize */
  StatusCode initialize() ;

  /** Gaudi Service Interface method implementations - execute */
  StatusCode execute() ;

  /** Gaudi Service Interface method implementations -finalize */
  StatusCode finalize() ;


private:
  
  /** Get a handle on the kinematic selector tool */
  ToolHandle<KinematicSelector> m_kinSelectorTool;

  /** Get a handle on the filter tool */
  FilterTool* m_filterTool;



private:
  /** Printing the configuration if true*/
  bool m_printSetup;

  /** Printing the summary if true*/
  bool m_printSummary;


  /** Input collection name  */
  std::string  m_inCollKey;

  /** Output collection name of the Neutrinos */
  std::string  m_outCollKey;
  
  /** Output collection name of the ElementLinks */
  std::string  m_outLinkCollKey;




private:
  //------------------------------------------------------
  // Selections
  //------------------------------------------------------
  /** minimum number of neutrinos passing the selection */
  unsigned int m_minNumberPassed;

  /** maximum number of neutrinos passing the selection */
  unsigned int m_maxNumberPassed;


  /** minimum missing transverse energy */
  double m_missingEtMin;

  /** maximum missing transverse energy */
  double m_missingEtMax;


  /** minimum sum Et */
  double m_sumEtMin;

  /** maximum sum Et */
  double m_sumEtMax;


  /** minimum missing transverse energy significance */
  double m_missingEtSignifMin;

  /** maximum missing transverse energy significance */
  double m_missingEtSignifMax;


  /** minimum phi */
  double m_phiMin;

  /** maximum phi */
  double m_phiMax;

  /** Phi ranges in which the particle should NOT be */
  std::string  m_phiVetoRanges;


private:

  /** Flag to dump the content of StoreGate after each event */
  bool m_dumpStoreGate;

  /** Internal event counter */
  unsigned long m_nEventsProcessed;

}; // End class D2PDMissingEtSelector


#endif
