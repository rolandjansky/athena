
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////// -*- C++ -*- /////////////////////////////
// EventQualityFilterAlg.h
// Header file for class EventQualityFilterAlg
// Author: Karsten Koeneke <karsten.koeneke@cern.ch>
// Description: Algorithm to filter out events with a bad Event errors
///////////////////////////////////////////////////////////////////
#ifndef EVENTUTILS_EVENTQUALITYFILTERALG_H
#define EVENTUTILS_EVENTQUALITYFILTERALG_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthFilterAlgorithm.h"



class EventQualityFilterAlg
  : public ::AthFilterAlgorithm
{

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
 public:


  /// Constructor with parameters:
  EventQualityFilterAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor:
  virtual ~EventQualityFilterAlg();

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

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
 private:

  /// Flag to turn on/off checking of LAr calorimeter error flag
  BooleanProperty m_useLArError;

  /// Flag to turn on/off checking of tile calorimter error flag
  BooleanProperty m_useTileError;

  /// Flag to turn on/off checking of SCT error flag
  BooleanProperty m_useSCTError;

  /// Flag to turn on/off checking of core error flag
  BooleanProperty m_useCoreError;

  // /// Flag to turn on/off checking of tile trip information
  // BooleanProperty m_useTileTripReader;


};

// I/O operators
//////////////////////

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////


#endif //> !EVENTUTILS_EVENTQUALITYFILTERALG_H
