/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGSERVICES_TRIGRDBMANAGER_H
#define TRIGSERVICES_TRIGRDBMANAGER_H

// Athena/Gaudi includes
#include "GaudiKernel/ServiceHandle.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

// CORAL includes
#include "CoralKernel/Context.h"
#include "RelationalAccess/IConnectionService.h"

/**
 * Helper to manage database connections
 *
 * This class is a "friend" of IRDBAccessSvc and allowed to manage
 * database connections.
 */
class TrigRDBManager {
public:
  /**
   * Close database connections
   *
   * @param msg          reference to MsgStream
   */
  static StatusCode closeDBConnections(MsgStream& msg)
  {
    // Cleanup of dangling database connections from RDBAccessSvc
    ServiceHandle<IRDBAccessSvc> rdbAccessSvc("RDBAccessSvc", "TrigRDBManager");
    if (rdbAccessSvc->shutdown("*Everything*")) {
      msg << MSG::INFO << "Cleaning up RDBAccessSvc connections" << endmsg;
    }
    else {
      msg << MSG::ERROR << "Cleaning up RDBAccessSvc connections failed" << endmsg;
      return StatusCode::FAILURE;
    }

    // Instantiate connection service
    coral::Context& context = coral::Context::instance();
    // Load CORAL connection service
    coral::IHandle<coral::IConnectionService> connSvcH = context.query<coral::IConnectionService>();
    if (connSvcH.isValid()) {
      msg << MSG::INFO << "Cleaning up idle CORAL connections" << endmsg;
      connSvcH->purgeConnectionPool();
    }
    return StatusCode::SUCCESS;
  }
};

#endif
