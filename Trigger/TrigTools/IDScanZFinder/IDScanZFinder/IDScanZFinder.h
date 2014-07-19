// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
//
// filename: IDScanZFinder.h
// 
// author: Nikos Konstantinidis <n.konstantinidis@ucl.ac.uk>
//         
//		 
// Description: AlgTool that finds z of primary vertex
// 
// -------------------------------
// ATLAS Collaboration
////////////////////////////////////////////////////////////////////////////////


#ifndef   IDSCANZFINDER_H__
#define   IDSCANZFINDER_H__

#include <string>
#include <vector>

#include "GaudiKernel/ToolHandle.h"

#include "TrigInDetToolInterfaces/ITrigZFinder.h"
#include "GaudiKernel/AlgTool.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
#include "IDScanZFinder/IDScanZFinderInternal.h"
// for some reason it cannot find the header with the usual cmt path resolution
#include "TrigInDetToolInterfaces/ITrigL2LayerNumberTool.h"

class TrigVertex;
class TrigSiSpacePoint;
class IRoiDescriptor;


class IDScanZFinder: public IDScanZFinderInternal<TrigSiSpacePoint>, public AlgTool, virtual public ITrigZFinder
{
 public:
 
  IDScanZFinder( const std::string&, const std::string&, const IInterface* );
  virtual ~IDScanZFinder(){};
  
  virtual StatusCode initialize();
  virtual StatusCode finalize  ();

  TrigVertexCollection* findZ( const std::vector<TrigSiSpacePoint *>& spVec, const IRoiDescriptor& roi);

  /// no private data members - all inherited from the IDScanZFinderInternal class
  /// NB: BE CAREFUL !!! Nothing set with a job option should EVER be changed 
  ///     inside the ZFinderInternal code  

  /// actually, need the TrigL2LayerNumberTool so we can configure with the 
  /// correct number of layers when including the IBL etc
  ToolHandle<ITrigL2LayerNumberTool> m_numberingTool;  

};
 

#endif

