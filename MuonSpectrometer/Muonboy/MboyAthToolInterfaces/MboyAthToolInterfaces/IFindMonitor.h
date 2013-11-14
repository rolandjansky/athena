/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IFindMonitor_H
#define IFindMonitor_H

#include <vector>
#include "GaudiKernel/IAlgTool.h" 

static const InterfaceID IID_IIFindMonitor("Muon::IFindMonitor", 1, 0);

namespace Muon {

  class MuPatCandidateBase;
  class MuPatSegment;
  class MuPatTrack;  

  /**
   @class IFindMonitor

  @author samusog@cern.ch
  */

class IFindMonitor:virtual public IAlgTool {

public:
 static const InterfaceID& interfaceID();

 virtual void Reset() = 0 ;
 virtual void PrintSegConfig ( Muon::MuPatCandidateBase& candidate, const std::vector<Muon::MuPatSegment*>& MuPatSegmentFree              ) = 0 ;
 virtual bool AlreadySuccessfullyTried ( Muon::MuPatCandidateBase& candidate, const std::vector<Muon::MuPatSegment*>& MuPatSegmentFree) = 0 ;
 virtual void RegisterIfSuccessful     ( Muon::MuPatCandidateBase& candidate, const std::vector<Muon::MuPatSegment*>& MuPatSegmentFree, const std::vector<Muon::MuPatTrack*>* pVectorpMuPatTrack ) = 0 ;

};

inline const InterfaceID& IFindMonitor::interfaceID() { return IID_IIFindMonitor; }

}

#endif

