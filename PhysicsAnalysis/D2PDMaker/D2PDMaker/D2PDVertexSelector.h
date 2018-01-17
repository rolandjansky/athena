/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef D2PDMAKER_D2PDVERTEXSELECTOR_H
#define D2PDMAKER_D2PDVERTEXSELECTOR_H

//============================================================================
// Name: D2PDVertexSelector.h
//
/**
   @class D2PDVertexSelector

   @author Dennis Petschull <dennis.petschull@cernSPAMNOT.ch>

   @date May 2010

   @brief 

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
#include "AthContainers/DataVector.h"
#include "AthLinks/ElementLink.h"
#include "AthLinks/ElementLinkVector.h"
#include "AthenaKernel/CLASS_DEF.h"

// INav4Mom includes
#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/INavigable4MomentumCollection.h"
#include "NavFourMom/INav4MomLink.h"

// Vertex includes
#include "VxVertex/VxContainer.h"       
#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxTrackAtVertex.h"
//#include "VxVertex/RecVertex.h"

// Include official tools
#include "AnalysisUtils/KinematicSelector.h"
#include "AnalysisUtils/KinematicSelectorCore.h"


//class IIsolationTool;
class IAnalysisTools;
namespace Trk
  {
    class VxCandidate;
    class VxTrackAtVertex;
  }

class D2PDVertexSelector : public AthFilterAlgorithm
{

public:

  /** Default constructor */
  D2PDVertexSelector(const std::string& name, ISvcLocator* pSvcLocator);

  /** Default destructor */
  virtual ~D2PDVertexSelector() ;
  

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


private:
  /** Printing the configuration if true*/
  bool m_printSetup;

  /** Printing the summary if true*/
  bool m_printSummary;


  /** Input collection name  */
  std::string  m_inCollKey;




private:
  //------------------------------------------------------
  // Selections
  //------------------------------------------------------
  
  /** minimum number of tracks at vertex */
  unsigned int m_minNumberVxTracks;

  /** maximum number of tracks at vertex */
  unsigned int m_maxNumberVxTracks;


private:

  /** Flag to dump the content of StoreGate after each event */
  bool m_dumpStoreGate;

  /** Internal event counter */
  unsigned long m_nEventsProcessed;

  /** Pointer to VertexCandidate */
  const Trk::VxCandidate* m_vxcand;

  /** Pointer to vector of tracks at vertex */
  const std::vector<Trk::VxTrackAtVertex*>* m_trackAtVertex;

  /** Flag to decide if cuts were passed */
  bool m_Passed;

  /** Number of tracks at vertex */
  unsigned int m_Ntrk;


  /** VertexType list for accept (logical OR combination) */
  std::vector<int> m_acceptVertexTypeList;

  /** VertexType list for require (logical AND combination) */
  std::vector<int> m_requireVertexTypeList;

  /** VertexType list for veto (logical NOT combination) */
  std::vector<int> m_vetoVertexTypeList;

        
  /** minimum global x-poition of the vertex */
  double m_minVertexX;

  /** maximum global x-poition of the vertex */
  double m_maxVertexX;
  
   /** minimum global y-poition of the vertex */
  double m_minVertexY;

  /** maximum global y-poition of the vertex */
  double m_maxVertexY;
          
  /** minimum global z-poition of the vertex */
  double m_minVertexZ;

  /** maximum global z-poition of the vertex */
  double m_maxVertexZ;


  /** Maximum number of tracks if cuts are passed */
  unsigned int m_NtrkPassed;

  /** Maximum number of tracks if cuts are passed */
  unsigned int m_NtrkNotPassed;

}; // End class D2PDVertexSelector


#endif
