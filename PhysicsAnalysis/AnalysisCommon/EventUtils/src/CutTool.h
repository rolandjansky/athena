///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// CutTool.h
// Header file for class CutTool
// Author: Karsten Koeneke <karsten.koeneke@cern.ch>
///////////////////////////////////////////////////////////////////
#ifndef EVENTUTILS_CUTTOOL_H
#define EVENTUTILS_CUTTOOL_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/ISkimmingTool.h"
#include "ExpressionEvaluation/ExpressionParserUserWithTrigSupport.h"

class CutTool
  : public ExpressionParserUserWithTrigSupport<::AthAlgTool>,
    virtual public ::DerivationFramework::ISkimmingTool
{

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
 public:

  // Copy constructor:

  /// Constructor with parameters:
  CutTool( const std::string& type,
           const std::string& name,
           const IInterface* parent );

  /// Destructor:
  virtual ~CutTool();

  /// Athena algtool's initialize
  virtual StatusCode  initialize() override;

  /// Athena algtool's finalize
  virtual StatusCode  finalize() override;


  /// Implement the method from the ISkimmingTool interface
  virtual bool eventPassesFilter() const final override;

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
 private:

  /// The cut string
  StringProperty m_cut;


  /// Internal event counter
  //mutable unsigned long m_nEventsProcessed;


};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////


#endif //> !EVENTUTILS_CUTTOOL_H
