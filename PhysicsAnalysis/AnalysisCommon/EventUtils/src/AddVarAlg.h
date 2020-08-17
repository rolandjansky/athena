///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AddVarAlg.h
// Header file for class AddVarAlg
// Author: Karsten Koeneke <karsten.koeneke@cern.ch>
///////////////////////////////////////////////////////////////////
#ifndef EVENTUTILS_ADDVARALG_H
#define EVENTUTILS_ADDVARALG_H 1

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthFilterAlgorithm.h"


// forward declarations
class IJobOptionsSvc;
namespace DerivationFramework {
  class IAugmentationTool;
}



class AddVarAlg
  : public ::AthAlgorithm
{

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
 public:

  // Copy constructor:

  /// Constructor with parameters:
  AddVarAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor:
  virtual ~AddVarAlg();

  /// Athena algorithm's initalize hook
  virtual StatusCode  initialize();

  /// Athena algorithm's execute hook
  virtual StatusCode  execute();

  /// Athena algorithm's finalize hook
  virtual StatusCode  finalize();


private:
  // The update handlers

  /// This internal method will realize if a user sets the 'AddVarTo' property
  void setupInputContainer( Gaudi::Details::PropertyBase& /*prop*/ );

  /// This internal method will realize if a user sets the 'VarName' property
  void setupVarName( Gaudi::Details::PropertyBase& /*prop*/ );

  /// This internal method will realize if a user sets the 'VarType' property
  void setupVarType( Gaudi::Details::PropertyBase& /*prop*/ );

  /// This internal method will realize if a user sets the 'Selection' property
  void setupSelection( Gaudi::Details::PropertyBase& /*prop*/ );



  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
 private:
  /// The job options service (will be used to forward this algs properties to
  /// the private tool)
  ServiceHandle<IJobOptionsSvc> m_jos;

  /// The ToolHandle to the augmentation tool
  ToolHandle<DerivationFramework::IAugmentationTool> m_tool;


  /// Input container name
  StringProperty m_inCollKey;

  /// This boolean is true if the user sets the 'InputContainer' property
  bool m_setInCollKey;


  /// The name of the new variable
  StringProperty m_varName;

  /// This boolean is true if the user sets the 'OutputContainer' property
  bool m_setVarName;


  /// The type of the new variable (allowed values are: 'bool', 'int', 'float')
  StringProperty m_varType;

  /// This boolean is true if the user sets the 'OutputLinkContainer' property
  bool m_setVarType;


  /// The string that describes how the new variable should be calculated
  StringProperty m_selection;

  /// This boolean is true if the user sets the 'Cut' property
  bool m_setSelection;



  /// Internal event counter
  unsigned long m_nEventsProcessed;


};

// I/O operators
//////////////////////

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

/// This internal method will realize if a user sets the 'AddVarTo' property
inline void AddVarAlg::setupInputContainer( Gaudi::Details::PropertyBase& /*prop*/ ) {
  m_setInCollKey = true;
  return;
}

/// This internal method will realize if a user sets the 'VarName' property
inline void AddVarAlg::setupVarName( Gaudi::Details::PropertyBase& /*prop*/ ) {
  m_setVarName = true;
  return;
}

/// This internal method will realize if a user sets the 'VarType' property
inline void AddVarAlg::setupVarType( Gaudi::Details::PropertyBase& /*prop*/ ) {
  m_setVarType = true;
  return;
}

/// This internal method will realize if a user sets the 'Selection' property
inline void AddVarAlg::setupSelection( Gaudi::Details::PropertyBase& /*prop*/ ) {
  m_setSelection = true;
  return;
}


#endif //> !EVENTUTILS_ADDVARALG_H
