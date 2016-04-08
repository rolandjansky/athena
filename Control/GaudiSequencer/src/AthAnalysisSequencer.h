///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthAnalysisSequencer.h 
// Header file for class AthAnalysisSequencer
// Author: C. Burgard <cburgard@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef GAUDISEQUENCER_ATHANALYSISSEQUENCER_H
#define GAUDISEQUENCER_ATHANALYSISSEQUENCER_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthSequencer.h"

//#include "GaudiKernel/ToolHandle.h"
//#include "GaudiKernel/IJobOptionsSvc.h"

class IAthHistogramTool;

/**
 ** ClassName: AthAnalysisSequencer
 **
 ** Description: An AthAnalysisSequencer is a special version of the
 ** AthSequencer that provides additional convenience functionality
 ** for histogramming and cutflow making. However, all the work
 ** is actually done in the python configurable here:
 ** Control/AthenaCommon/python/AlgSequence.py
 ** We keep this empty C++ shell currently only for being able to do:
 ** myAnaSeq = CfgMgr.AthAnalysisSequencer("MyAnalysisSeq")
 **/
class AthAnalysisSequencer
  : public ::AthSequencer
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  AthAnalysisSequencer( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~AthAnalysisSequencer(); 

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  //AthAnalysisSequencer();

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 


#endif //> !GAUDISEQUENCER_ATHANALYSISSEQUENCER_H
