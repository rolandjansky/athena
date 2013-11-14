/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IRefineMonitor_H
#define IRefineMonitor_H

#include <vector>
#include "GaudiKernel/IAlgTool.h" 

static const InterfaceID IID_IIRefineMonitor("Muon::IRefineMonitor", 1, 0);

namespace Muon {

  class MuPatCandidateBase;
  class MuPatSegment;
  class MuPatTrack;  

  /**
   @class IRefineMonitor

  @author samusog@cern.ch
  */

class IRefineMonitor:virtual public IAlgTool {

public:
 static const InterfaceID& interfaceID();

 virtual void Reset() = 0 ;
 virtual void PrintSegConfig ( Muon::MuPatTrack& MuPatTrackToBeRefined ) = 0 ;
 virtual bool AlreadySuccessfullyTried (  Muon::MuPatTrack& MuPatTrackToBeRefined ) = 0 ;
 virtual void RegisterIfSuccessful     (  Muon::MuPatTrack& MuPatTrackToBeRefined , const Muon::MuPatTrack* pMuPatTrack ) = 0 ;

};

inline const InterfaceID& IRefineMonitor::interfaceID() { return IID_IIRefineMonitor; }

}

#endif

