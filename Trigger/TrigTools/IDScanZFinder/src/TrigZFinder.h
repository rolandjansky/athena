// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
//
// filename: TrigZFinder.h
// 
// author: Nikos Konstantinidis <n.konstantinidis@ucl.ac.uk>
//         
//		 
// Description: AlgTool that finds z of primary vertex
// 
// -------------------------------
// ATLAS Collaboration
////////////////////////////////////////////////////////////////////////////////


#ifndef   IDSCANZFINDER_TRIGZFINDER_H
#define   IDSCANZFINDER_TRIGZFINDER_H

#include <string>
#include <vector>

#include "GaudiKernel/ToolHandle.h"

#include "TrigInDetToolInterfaces/ITrigZFinder.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
#include "TrigZFinderInternal.h"
#include "TrigInDetToolInterfaces/ITrigL2LayerNumberTool.h"

class TrigVertex;
class TrigSiSpacePointBase;
class IRoiDescriptor;


class TrigZFinder: public TrigZFinderInternal, public AthAlgTool, virtual public ITrigZFinder
{
 public:
 
  TrigZFinder( const std::string&, const std::string&, const IInterface* );
  virtual ~TrigZFinder(){};
  
  virtual StatusCode initialize() override;

  virtual
  TrigVertexCollection* findZ( const std::vector<TrigSiSpacePointBase>& spVec, const IRoiDescriptor& roi) const override;

  /// no private data members - all inherited from the TrigZFinderInternal class
  /// NB: BE CAREFUL !!! Nothing set with a job option should EVER be changed 
  ///     inside the ZFinderInternal code  

  /// actually, need the TrigL2LayerNumberTool so we can configure with the 
  /// correct number of layers when including the IBL etc
  ToolHandle<ITrigL2LayerNumberTool> m_numberingTool;  

};
 

#endif

