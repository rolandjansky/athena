/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BeamSpotID.h"


std::ostream & operator<<(std::ostream & os,const BeamSpot::ID & rhs){
  os << "Run:Id = [" << rhs.run() <<" , " << rhs.id() << "]  "
     << "LumiRange = [ " << rhs.lumiStart() << ", " << rhs.lumiEnd() << ")  " 
     << "EventRange = [ " << rhs.firstEvent() << ", " << rhs.lastEvent() << "]  "
     << "TimeStamp  = [ " << rhs.firstTimeStamp() << ", " << rhs.lastTimeStamp() << "] (Offset "
     << rhs.timeStampOffset() << ") , BCID = " << rhs.bcid() <<" m_beamSpotTool = "<<rhs.getBSTool();
  return os;
}
