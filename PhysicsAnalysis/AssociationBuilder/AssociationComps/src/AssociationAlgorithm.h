/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef ASSOCIATIONCOMPS_ASSOCIATIONALGORITHM_H
#define ASSOCIATIONCOMPS_ASSOCIATIONALGORITHM_H

//============================================================================
// Name: AssociationAlgorithm.h
//
/**
   @class AssociationAlgorithm

   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>

   @date January 2010

   @brief Algorithm to match one object container to another using 
   a list of tools provided by job options

*/
//=============================================================================


// STL includes
#include <string>
#include <vector>
#include <utility>

// Gaudi includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

// Athena includes
#include "AthenaBaseComps/AthAlgorithm.h"

// INav4Mom includes
#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/INav4MomAssocs.h"

// Include the association base tool
#include "AssociationKernel/IAssociationTool.h"



class AssociationAlgorithm : public AthAlgorithm
{

public:

  /** Default constructor */
  AssociationAlgorithm(const std::string& name, ISvcLocator* pSvcLocator);

  /** Default destructor */
  virtual ~AssociationAlgorithm() ;
  

public:
  /** Gaudi Service Interface method implementations */
  StatusCode initialize() ;

  /** Gaudi Service Interface method implementations */
  StatusCode execute() ;

  /** Gaudi Service Interface method implementations */
  StatusCode finalize() ;



private:
  /** Input collection name, used to match from these objects  */
  std::string  m_matchFromCollKey;

  /** Output match map collection name list  */
  std::vector< std::string >  m_outCollKeys;
  

  /** Get a handle on the AssociationTools; an array of tools */
  ToolHandleArray<IAssociationTool> m_assoTools;


private:
  //------------------------------------------------------
  // Options
  //------------------------------------------------------

  /** minimum number of neutrinos passing the selection */
  unsigned int m_minNumberPassed;

  /** maximum number of neutrinos passing the selection */
  unsigned int m_maxNumberPassed;



private:
  //------------------------------------------------------
  // Internally used variables
  //------------------------------------------------------
  /** Flag to dump the content of StoreGate after each event */
  bool m_dumpStoreGate;

  /** Internal event counter */
  unsigned long m_nEventsProcessed;

  /** Create the vector of INav4MomAssocs maps that will hold the results of matching */
  std::vector< INav4MomAssocs* > m_vecINav4MomAssocs;


}; // End class AssociationAlgorithm


#endif
