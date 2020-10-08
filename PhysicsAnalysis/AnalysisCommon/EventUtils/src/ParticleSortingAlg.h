///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ParticleSortingAlg.h
// Header file for class ParticleSortingAlg
// Author: Karsten Koeneke <karsten.koeneke@cern.ch>
///////////////////////////////////////////////////////////////////
#ifndef EVENTUTILS_PARTICLESORTINGALG_H
#define EVENTUTILS_PARTICLESORTINGALG_H 1

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"


// forward declarations
class IJobOptionsSvc;
namespace DerivationFramework {
  class IAugmentationTool;
}




class ParticleSortingAlg
  : public ::AthAlgorithm
{

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
 public:

  // Copy constructor:

  /// Constructor with parameters:
  ParticleSortingAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor:
  virtual ~ParticleSortingAlg();

  /// Athena algorithm's initalize hook
  virtual StatusCode  initialize();

  /// Athena algorithm's execute hook
  virtual StatusCode  execute();

  /// Athena algorithm's finalize hook
  virtual StatusCode  finalize();


private:
  // The update handlers

  /// This internal method will realize if a user sets the 'InputContainer' property
  void setupInputContainer( Gaudi::Details::PropertyBase& /*prop*/ );

  /// This internal method will realize if a user sets the 'OutputContainer' property
  void setupOutputContainer( Gaudi::Details::PropertyBase& /*prop*/ );

  /// This internal method will realize if a user sets the 'SortVariable' property
  void setupSortVar( Gaudi::Details::PropertyBase& /*prop*/ );

  /// This internal method will realize if a user sets the 'SortDeceding' property
  void setupSortDescending( Gaudi::Details::PropertyBase& /*prop*/ );



  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
 private:
  /// The job options service (will be used to forward this algs properties to
  /// the private tool)
  ServiceHandle<IJobOptionsSvc> m_jos;

  /// The ToolHandle to the private ParticleSortingTool
  ToolHandle<DerivationFramework::IAugmentationTool> m_tool;

  /// Input container name
  StringProperty m_inCollKey;

  /// This boolean is true if the user sets the 'InputContainer' property
  bool m_setInCollKey;


  /// The name of the output container (with SG::VIEW_ELEMENTS) with the sorted copy of input objects
  StringProperty m_outCollKey;

  /// This boolean is true if the user sets the 'OutputContainer' property
  bool m_setOutCollKey;


  /// Define by what parameter to sort (default: 'pt')
  StringProperty m_sortVar;

  /// This boolean is true if the user sets the 'SortVariable' property
  bool m_setSortVar;


  /// Define if the container should be sorted in a descending order (default=true)
  BooleanProperty m_sortDescending;

  /// This boolean is true if the user sets the 'SortDescending' property
  bool m_setSortDescending;


  /// Internal event counter
  unsigned long m_nEventsProcessed;

};



///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

/// This internal method will realize if a user sets the 'InputContainer' property
inline void ParticleSortingAlg::setupInputContainer( Gaudi::Details::PropertyBase& /*prop*/ ) {
  m_setInCollKey = true;
  return;
}

/// This internal method will realize if a user sets the 'OutputContainer' property
inline void ParticleSortingAlg::setupOutputContainer( Gaudi::Details::PropertyBase& /*prop*/ ) {
  m_setOutCollKey = true;
  return;
}

/// This internal method will realize if a user sets the 'SortVariable' property
inline void ParticleSortingAlg::setupSortVar( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_setSortVar = true;
  return;
}

/// This internal method will realize if a user sets the 'SortDeceding' property
inline void ParticleSortingAlg::setupSortDescending( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_setSortDescending = true;
  return;
}


#endif //> !EVENTUTILS_PARTICLESORTINGALG_H
