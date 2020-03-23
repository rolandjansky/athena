/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ILumiCalcSvc.h 
// Header file for class ILumiCalcSvc
// Author: R.Hawkings<richard.hawkings@cern.ch>
//         B.Radics<radbal@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef LUMIBLOCKCOMPS_ILUMIBLOCKSVC_H
#define LUMIBLOCKCOMPS_ILUMIBLOCKSVC_H

/**
 * @class ILumiCalcSvc
 * @brief This class defines a protocol to process lumiblock 
 * @brief iov information, given a list of lumiblocks
 */

#include <string>
#include <list>
#include <utility>

#include "TString.h"

#include "GaudiKernel/INamedInterface.h"

class TTree;

typedef std::pair< TString, std::list<TString> > tvtPair;

class ILumiCalcSvc
: virtual public ::INamedInterface
{ 

public:
  DeclareInterfaceID(ILumiCalcSvc, 1, 0);

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

};

#endif
