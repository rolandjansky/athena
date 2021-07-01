/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file  src/DataModelCompatSvc.cxx
 * @author scott snyder
 * @date Nov 2005
 * @brief Provides backwards compatibility for reading @c DataVector
 *        (and other) classes.
 */


#include "DataModelAthenaPool/DataModelCompatSvc.h"
#include "DataModelAthenaPool/DataVectorConvert.h"
#include "DataModelAthenaPool/CLHEPConverters.h"
#include "DataModelAthenaPool/PackedContainerConverter.h"
#include "RootConversions/VectorConverters.h"
#include "RootConversions/TConvertingStreamerInfo.h"
#include "RootConversions/TConvertingBranchElement.h"
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"


//============================================================================
// Standard service methods.
//


/**
 * @brief Constructor.
 * @param name The service name.
 * @param svc The service locator.
 */
DataModelCompatSvc::DataModelCompatSvc (const std::string& name,
                                        ISvcLocator* svc)
  : AthService (name, svc),
    m_initialized (false)
{
  // no properties...
}


/**
 * @brief Service initialization; called at the beginning of the job.
 */
StatusCode DataModelCompatSvc::initialize()
{
  // Base class initialization.
  CHECK( AthService::initialize() );

  // We're going to want to scan all types, looking for @c DataVector
  // instantiations.  We can't do that, though, until the data file
  // has been open and we've set up the proxies.  So defer the real
  // work until there's a BeginProcessing/BeginEvent incident.
  IIncidentSvc* incsvc = 0;
  REPORT_MESSAGE(MSG::DEBUG) << "running";
  CHECK( service("IncidentSvc", incsvc) );
  incsvc->addListener (this, "BeginProcessing");
  incsvc->addListener (this, "BeginEvent");

  // This, however, we need to do before opening any files.
  TConvertingStreamerInfo::Initialize();
  TConvertingBranchElement::Initialize();
  DataModelAthenaPool::CLHEPConverters::initialize();
  RootConversions::VectorConverters::initialize();
  DataModelAthenaPool::installPackedContainerConverters();

  return StatusCode::SUCCESS;
}


/**
 * @brief Handle incidents.
 * @param name The fired incident.
 */
void DataModelCompatSvc::handle (const Incident& inc)
{
  if (inc.type() == "BeginProcessing" || inc.type() == "BeginEvent")
  {
    std::scoped_lock lock (m_mutex);
    if (!m_initialized) {
      // Do our initialization.
      REPORT_MESSAGE(MSG::DEBUG) << "handling incident " << inc.type();
      DataModelAthenaPool::DataVectorConvert::initialize (this);
      m_initialized = true;
    }
  }
}


/**
 * @brief Log a debugging message.
 * @param msg The message to log.
 */
void DataModelCompatSvc::debug (const char* msg)
{
  REPORT_MESSAGE(MSG::DEBUG) << msg;
}


/**
 * @brief Log an error message.
 * @param msg The message to log.
 */
void DataModelCompatSvc::error (const char* msg)
{
  REPORT_MESSAGE(MSG::ERROR) << msg;
}
