/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_COSTUTIL_H
#define TRIG_COSTUTIL_H

//
// Common functions for cost tools and algorithms
//

#include "GaudiKernel/MsgStream.h"
#include "TrigTimeAlgs/TrigTimer.h"

class TrigMonEvent;
class TrigMonConfig;

//---------------------------------------------------------------------------------------
namespace Trig 
{

  void Print(const TrigMonEvent &event, 
	     const TrigMonConfig &config,
	     MsgStream &log, MSG::Level level, int verb = 6);

  void Print(const TrigMonConfig &config,
	     std::ostream &os);

  std::string getUniqueString();

  std::string MergeEvent(TrigMonEvent &event, 
			 const TrigMonEvent &other, 
			 int level_other,
			 const std::string &option = "");
}

#endif