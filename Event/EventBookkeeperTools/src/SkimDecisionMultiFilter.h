///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// SkimDecisionMultiFilter.h 
// Header file for class SkimDecisionMultiFilter
// Author: Karsten Koeneke <karsten.koeneke@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef EVENTBOOKKEEPERTOOLS_SKIMDECISIONMULTIFILTER_H
#define EVENTBOOKKEEPERTOOLS_SKIMDECISIONMULTIFILTER_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthFilterAlgorithm.h"

// Forward declarations
class SkimDecisionCollection;



class SkimDecisionMultiFilter
  : public ::AthFilterAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  SkimDecisionMultiFilter( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~SkimDecisionMultiFilter(); 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 
private:

  /// Private method to determine the accept decission
  bool skimDecisionNameIsAccepted( const SkimDecisionCollection* skimDecCont,
                                   std::string& skimDecName,
                                   std::vector<int>& skimDecIdxList,
                                   unsigned int idx );


  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
private: 

  /// Default constructor: 
  //SkimDecisionMultiFilter();

  /// SkimDecisionCollection name
  std::string m_skimDecContName;

  /// The list of SkimDecision names to accept (logical OR)
  std::vector< std::string > m_accSkimDecNameList;

  /// The list of SkimDecision names to require (logical AND)
  std::vector< std::string > m_reqSkimDecNameList;

  /// The list of SkimDecision names to veto (logical NOT)
  std::vector< std::string > m_vetoSkimDecNameList;


  /// List of the indices where the correct SkimDecision is in the container for the accept (logical OR) method
  std::vector<int> m_accIdxList;

  /// List of the indices where the correct SkimDecision is in the container for the require (logical AND) method
  std::vector<int> m_reqIdxList;

  /// List of the indices where the correct SkimDecision is in the container for the veto (logical NOT) method
  std::vector<int> m_vetoIdxList;


}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 


#endif //> !EVENTBOOKKEEPERTOOLS_SKIMDECISIONMULTIFILTER_H
