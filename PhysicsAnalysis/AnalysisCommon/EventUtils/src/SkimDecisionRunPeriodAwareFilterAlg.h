///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// SkimDecisionRunPeriodAwareFilterAlg.h 
// Header file for class SkimDecisionRunPeriodAwareFilterAlg
// Author: Karsten Koeneke
/////////////////////////////////////////////////////////////////// 
#ifndef EVENTUTILS_SKIMDECISIONRUNPERIODAWAREFILTERALG_H
#define EVENTUTILS_SKIMDECISIONRUNPERIODAWAREFILTERALG_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthFilterAlgorithm.h"

// Forward declarations
class SkimDecisionCollection;



class SkimDecisionRunPeriodAwareFilterAlg
  : public ::AthFilterAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  SkimDecisionRunPeriodAwareFilterAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~SkimDecisionRunPeriodAwareFilterAlg(); 

  // Assignment operator: 
  //SkimDecisionRunPeriodAwareFilterAlg &operator=(const SkimDecisionRunPeriodAwareFilterAlg &alg); 

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
                                   std::vector< std::vector<int> >& skimDecIdxList,
                                   unsigned int idxA,
                                   unsigned int idxB );



  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  //SkimDecisionRunPeriodAwareFilterAlg();

  /// Containers

  /// SkimDecisionCollection name
  std::string m_skimDecContName;


  /// The list of SkimDecision names to accept 
  std::vector< std::string > m_skimDecNameList;


  /// The list of SkimDecision names to accept for each run period (can be multiple in each run period)
  std::vector< std::vector< std::string > > m_skimDecNameListList;


  /// The list of run numbers where a block begins (for data)
  std::vector< unsigned int > m_dataBeginRunNumList;

  /// The list of run numbers where a block ends (for data)
  std::vector< unsigned int > m_dataEndRunNumList;


  /// The list of run numbers where a block begins (for MC)
  std::vector< unsigned int > m_mcBeginRunNumList;

  /// The list of run numbers where a block ends (for MC)
  std::vector< unsigned int > m_mcEndRunNumList;

  /// The list of relate integrated luminosities (fractional) for each MC run-number block
  std::vector< double > m_mcRelLumiList;


  /// List of the indices where the correct SkimDecision is in the container for the accept (logical OR) method
  //std::vector<int> m_skimDecIdxList;
  std::vector< std::vector< int > > m_skimDecIdxListList;


}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 


#endif //> !EVENTUTILS_SKIMDECISIONRUNPERIODAWAREFILTERALG_H
