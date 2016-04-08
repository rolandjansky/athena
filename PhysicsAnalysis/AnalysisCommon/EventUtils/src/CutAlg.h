///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthFilterAlgorithm.h"


// forward declarations
class IJobOptionsSvc;
namespace DerivationFramework {
  class ISkimmingTool;
}



class CutAlg
  : public ::AthFilterAlgorithm
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


private:
  // The update handlers

  /// This internal method will realize if a user sets the 'Cut' property
  void setupCut( Property& /*prop*/ );


  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
 private:
  /// The job options service (will be used to forward this algs properties to
  /// the private tool)
  ServiceHandle<IJobOptionsSvc> m_jos;

  /// The ToolHandle to the SkimmingTool
  ToolHandle<DerivationFramework::ISkimmingTool> m_skimTool;


  /// The cut string
  StringProperty m_cut;

  /// This boolean is true if the user sets the 'Cut' property
  bool m_setCut;



  /// Internal event counter
  unsigned long m_nEventsProcessed;


};

// I/O operators
//////////////////////

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

/// This internal method will realize if a user sets the 'Cut' property
inline void CutAlg::setupCut( Property& /*prop*/ ) {
  m_setCut = true;
  return;
}




#endif //> !EVENTUTILS_CUTALG_H
