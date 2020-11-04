/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file StoreGate/src/ReadCondHandle.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2020
 * @brief Out-of-line implementations for ReadCondHandle.
 */

#include "StoreGate/ReadCondHandle.h"


namespace SG {


/**
 * @brief Report a conditions container lookup failure.
 * @param cc The conditions container.
 * @param eid The time for which to search.
 * @param The key corresponding to the conditions container.
 */
void ReadCondHandleNotFound (const CondContBase& cc,
                             const EventIDBase& eid,
                             const std::string& key)
{
  std::ostringstream ost;
  cc.list(ost);
  MsgStream msg(Athena::getMessageSvc(), "ReadCondHandle");
  msg << MSG::ERROR 
      << "ReadCondHandle::retrieve() could not find EventTime " 
      << eid  << " for key " << key << "\n"
      << ost.str()
      << endmsg;
}


} // namespace SG
