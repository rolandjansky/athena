/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "POOLCore/DbPrint.h"
#include "POOLCore/SystemTools.h"

using namespace pool;

// Global variable storing APR output level
// Can be set through the environment POOL_OUTMSG_LEVEL
// or through API
DbPrintLvl::MsgLevel DbPrintLvl::outputLvl = SystemTools::GetOutputLvl();
