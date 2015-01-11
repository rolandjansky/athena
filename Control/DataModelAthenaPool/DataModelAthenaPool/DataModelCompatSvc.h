// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DataModelCompatSvc.h,v 1.5 2008-08-20 05:14:42 yyao Exp $

/**
 * @file  DataModelAthenaPool/DataModelCompatSvc.h
 * @author scott snyder
 * @date Nov 2005
 * @brief Provides backwards compatibility for reading @c DataVector
 *        (and other) classes.
 *
 * When setting up @c DataVector inheritance using @c DATAVECTOR_BASE,
 * a side effect is that the persistent form of the data may change.
 * Including this service will fix up the root metadata to allow
 * reading old data.  In fact, any sort of @c DataVector<A> to
 * @c DataVector<B> conversions will be allowed, provided that
 * @c A* can be converted to @c B*.
 *
 * Note: we currently don't notice when the input file changes.
 * This may cause problems if you're mixing files of different versions.
 *
 * This service also sets up other needed conversions.
 */

#ifndef DATAMODELCOMPATSVC_H
#define DATAMODELCOMPATSVC_H

#include "RootUtils/ILogger.h"
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IIncidentListener.h"


/**
 * @brief Service for @c DataVector backwards compatibility.
 */
class DataModelCompatSvc
  : public AthService,
    public RootUtils::ILogger,
    virtual public IIncidentListener
{
public:
  /**
   * @brief Constructor.
   * @param name The service name.
   * @param svc The service locator.
   */
  DataModelCompatSvc(const std::string& name, ISvcLocator* svc);


  /**
   * @brief Service initialization; called at the beginning of the job.
   */
  virtual StatusCode initialize();


  /**
   * @brief Handle incidents.
   * @param name The fired incident.
   */
  virtual void handle (const Incident& inc);


  /**
   * @brief Log a debugging message.
   * @param msg The message to log.
   */
  virtual void debug (const char* msg);


  /**
   * @brief Log an error message.
   * @param msg The message to log.
   */
  virtual void error (const char* msg);


private:
  /// True if we've already scanned the types.
  bool m_initialized;
};

#endif // not DATAMODELCOMPATSVC_H
