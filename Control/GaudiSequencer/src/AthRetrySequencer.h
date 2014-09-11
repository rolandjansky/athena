///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthRetrySequencer.h 
// Header file for class AthRetrySequencer
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef GAUDISEQUENCER_ATHRETRYSEQUENCER_H
#define GAUDISEQUENCER_ATHRETRYSEQUENCER_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthSequencer.h"


/** @class AthRetrySequencer
 *  @c AthRetrySequencer runs a sequence of (sub)algorithms.
 *     if the sequence ends successfully but its 'filter-passed' flag is set
 *     to false, the retry-sequencer will re-run the same sequence anew,
 *     until the flag is set to true or the maximum number of retries is
 *     reached (in which case, the retry-sequencer will return 
 *     StatusCode::FAILURE)
 */
class AthRetrySequencer
  : public ::AthSequencer
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  AthRetrySequencer( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~AthRetrySequencer(); 

  // Assignment operator: 
  //AthRetrySequencer &operator=(const AthRetrySequencer &alg); 

  // Athena algorithm's Hooks
  virtual StatusCode  execute();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  AthRetrySequencer();

  /// Maximum number of loop iterations
  int m_maxRetries;
}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 


#endif //> !GAUDISEQUENCER_ATHRETRYSEQUENCER_H
