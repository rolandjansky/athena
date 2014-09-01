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

  /// This internal method will realize if a user sets the 'InputContainer' property
  void setupInputContainer( Property& /*prop*/ );

  /// This internal method will realize if a user sets the 'OutputContainer' property
  void setupOutputContainer( Property& /*prop*/ );

  /// This internal method will realize if a user sets the 'OutputLinkContainer' property
  void setupOutputLinkContainer( Property& /*prop*/ );

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

  /// Input container name
  StringProperty m_inCollKey;

  /// This boolean is true if the user sets the 'InputContainer' property
  bool m_setInCollKey;


  /// Output collection name (deep copies of the original ones)
  StringProperty m_outCollKey;

  /// This boolean is true if the user sets the 'OutputContainer' property
  bool m_setOutCollKey;


  /// Output link collection name (ElementLinks to selected IParticles)
  StringProperty m_outLinkCollKey;

  /// This boolean is true if the user sets the 'OutputLinkContainer' property
  bool m_setOutLinkCollKey;


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

/// This internal method will realize if a user sets the 'InputContainer' property
inline void CutAlg::setupInputContainer( Property& /*prop*/ ) {
  m_setInCollKey = true;
  return;
}

/// This internal method will realize if a user sets the 'OutputContainer' property
inline void CutAlg::setupOutputContainer( Property& /*prop*/ ) {
  m_setOutCollKey = true;
  return;
}

/// This internal method will realize if a user sets the 'OutputLinkContainer' property
inline void CutAlg::setupOutputLinkContainer( Property& /*prop*/ ) {
  m_setOutLinkCollKey = true;
  return;
}

/// This internal method will realize if a user sets the 'Cut' property
inline void CutAlg::setupCut( Property& /*prop*/ ) {
  m_setCut = true;
  return;
}




#endif //> !EVENTUTILS_CUTALG_H
