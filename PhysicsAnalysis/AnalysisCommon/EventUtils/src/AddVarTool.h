///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AddVarTool.h
// Header file for class AddVarTool
// Author: Karsten Koeneke <karsten.koeneke@cern.ch>
///////////////////////////////////////////////////////////////////
#ifndef EVENTUTILS_ADDVARTOOL_H
#define EVENTUTILS_ADDVARTOOL_H 1

// STL includes
#include <vector>
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

// Forward declarations
namespace ExpressionParsing {
  class ExpressionParser;
}
namespace SG {
  class AuxElement;
}




class AddVarTool
  : virtual public ::DerivationFramework::IAugmentationTool,
            public ::AthAlgTool
{

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  // Copy constructor:

  /// Constructor with parameters:
  AddVarTool( const std::string& type,
              const std::string& name,
              const IInterface* parent );

  /// Destructor:
  virtual ~AddVarTool();

  /// Athena algtool's initialize
  virtual StatusCode  initialize() override;

  /// Athena algtool's finalize
  virtual StatusCode  finalize() override;


  /// Implement the method from the ISkimmingTool interface
  virtual StatusCode addBranches() const final override;



// Private methods
private:
  /// Helper methods
  StatusCode evaluateAuxElement( const SG::AuxElement* inAuxElement ) const;



  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
private:
  /// The trigger decision tool
  ToolHandle<Trig::TrigDecisionTool> m_trigDecisionTool;

  /// The expression parser
  ExpressionParsing::ExpressionParser *m_parser;


  /// Input container name
  StringProperty m_inCollKey;

  /// The name of the new variable
  StringProperty m_varName;

  /// The type of the new variable (allowed values are: 'bool', 'int', 'float')
  StringProperty m_varType;

  /// The string that describes how the new variable should be calculated
  StringProperty m_selection;


  /// Index to specify the variable type
  mutable int m_varTypeIndex;

  /// An internal integer to keep track of the type of the input container
  //mutable int m_inContIdx;

  /// Internal event counter
  mutable unsigned long m_nEventsProcessed;


};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////


#endif //> !EVENTUTILS_ADDVARTOOL_H
