///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// CutAlg.h
// Header file for class CutAlg
// Author: Karsten Koeneke <karsten.koeneke@cern.ch>
///////////////////////////////////////////////////////////////////
#ifndef EVENTUTILS_CUTALG_H
#define EVENTUTILS_CUTALG_H 1

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthFilterAlgorithm.h"

#include "ExpressionEvaluation/ExpressionParserUserWithTrigSupport.h"


class CutAlg
  : public ExpressionParserUserWithTrigSupport<::AthFilterAlgorithm>
{

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
 public:

  // Copy constructor:

  /// Constructor with parameters:
  CutAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor:
  virtual ~CutAlg();

  /// Athena algorithm's initalize hook
  virtual StatusCode  initialize();

  /// Athena algorithm's execute hook
  virtual StatusCode  execute();

  /// Athena algorithm's finalize hook
  virtual StatusCode  finalize();

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
 private:

  /// The cut string
  StringProperty m_cut;

  /// Internal event counter
  unsigned long m_nEventsProcessed;


};


#endif //> !EVENTUTILS_CUTALG_H
