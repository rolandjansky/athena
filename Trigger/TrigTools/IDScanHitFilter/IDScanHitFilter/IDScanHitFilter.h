// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
//
// filename: IDScanHitFilter.h
// 
// author: Nikos Konstantinidis
//         nk@hep.ucl.ac.uk
//		 
// 
// Description: - main pattern recognition algorithm of IdScan 
// 
// date: 28/07/2003
//
// migrated to an AlgTool 28/01/2007 
// -------------------------------
// ATLAS Collaboration
////////////////////////////////////////////////////////////////////////////////


#ifndef __IDSCAN_HITFILTER_H__
#define __IDSCAN_HITFILTER_H__

#include <vector>
#include <list>
#include <string>

#include "AthenaBaseComps/AthAlgTool.h"

#include "IDScanHitFilter/iHitFilter.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetToolInterfaces/ITrigHitFilter.h"

#include "GaudiKernel/ToolHandle.h"
#include "TrigInDetToolInterfaces/ITrigL2LayerNumberTool.h"

class TrigSiSpacePoint;
class IRoiDescriptor;
class Group;
class IdScanSpacePoint;

////////////////////////////////////////////////////////////////////////////////
////    IDScanHitFilter
////////////////////////////////////////////////////////////////////////////////

class IDScanHitFilter: public iHitFilter, public AthAlgTool, virtual public ITrigHitFilter
{
 public:
  
  IDScanHitFilter( const std::string&, const std::string&, const IInterface* );
  virtual ~IDScanHitFilter(){ };

  virtual StatusCode initialize();
  virtual StatusCode finalize  ();
  
  void findTracks( std::vector<const TrigSiSpacePoint* >, 
		   TrigInDetTrackCollection&, 
		   const IRoiDescriptor*, double zvertex,
		   double shiftx, double shifty,
		   const int missing_layers);

 private:

  void makeTracks( std::list<Group>&, TrigInDetTrackCollection&, double zvertex, double shiftx, double shifty);

  /// actually, need the TrigL2LayerNumberTool so we can configure with the 
  /// correct number of layers when including the IBL etc
  ToolHandle<ITrigL2LayerNumberTool> m_numberingTool;  

};

#endif
