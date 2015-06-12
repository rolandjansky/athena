///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ILumiCalcSvc.h 
// Header file for class ILumiCalcSvc
// Author: R.Hawkings<richard.hawkings@cern.ch>
//         B.Radics<radbal@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef TESTATHENAKERNEL_ILUMIBLOCKSVC_H 
#define TESTATHENAKERNEL_ILUMIBLOCKSVC_H 1

/**
 * @class ILumiCalcSvc
 * @brief This class defines a protocol to process lumiblock 
 * @brief iov information, given a list of lumiblocks
 */

// for size_t
#include <cstddef>

// STL includes
#include <vector>
#include <string>
#include <map>
#include "TString.h"

// FrameWork includes
#include "GaudiKernel/INamedInterface.h"
#include "GaudiKernel/Property.h"

class TTree;

typedef std::pair< TString, std::list<TString> > tvtPair;

class ILumiCalcSvc
: virtual public ::INamedInterface
{ 

public:

  virtual ~ILumiCalcSvc();

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

private:



public:

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  // Default calculation of ILumi based on "LumiBlocks" and
  // "IncompleteLumiBlocks" collections in MetaData store
  virtual StatusCode calcLumi() = 0;
  // Get a pointer the the TTree table of the calculation
  // NOTE: only call this AFTER calcLumi or calcLumiFromLBC 
  virtual TTree * getLumiTree() = 0;
  // Print some details to output screen
  virtual void print() = 0;

  /// register trigger-lbc combination 
  virtual bool registerLBCollection(const TString& tname, const TString& regexpr, const std::list<TString>& trigpar) = 0 ;

  
  static const InterfaceID& interfaceID();
  
}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline 
const InterfaceID& 
ILumiCalcSvc::interfaceID() 
{ 
  static const InterfaceID IID_ILumiCalcSvc("ILumiCalcSvc", 1, 0);
  return IID_ILumiCalcSvc; 
}


#endif //> !ATHENAKERNEL_ILUMICALCSVC_H

