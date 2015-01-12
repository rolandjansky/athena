/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerUtils/src/SGGetterImpl.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Common object getter code for retrieving from StoreGate.
 */


#include "D3PDMakerUtils/SGGetterImpl.h"
#include "D3PDMakerUtils/TypeNameConversions.h"
#include "SGTools/DataProxy.h"
#include "AthenaKernel/errorcheck.h"
#include "AthContainersInterfaces/IConstAuxStore.h"

namespace D3PD {


/**
 * @brief Constructor.
 * @param name Name of this tool.
 * @param sg The event storage service.
 */
SGGetterImpl::SGGetterImpl (const std::string& name,
                            ServiceHandle<StoreGateSvc>& sg)
  : m_clidsvc ("ClassIDSvc", name),
    m_resolver (name, sg, m_sgkey),
    m_clid (CLID_NULL),
    m_sg (sg)
{
}


/**
 * @brief Return the type of object retrieved by this tool.
 */
const std::type_info& SGGetterImpl::typeinfo() const
{
  const std::type_info* ti = 0;
  if (clidToTypeinfo (m_clid, m_typename, ti, this->name()).isSuccess())
    return *ti;

  REPORT_MESSAGE (MSG::ERROR) << "Can't find typeinfo for CLID "
                              << m_clid << " (type " << m_typename << ").";
  return typeid (void);
}


/**
 * @brief Return the class ID being read by this tool.
 */
CLID SGGetterImpl::clid() const
{
  return m_clid;
}


/**
 * @brief Return the target object.
 * @param allowMissing If true, then we should not generate errors
 *        if the requested object is missing.
 *
 * Should be of the type given by @c typeinfo.
 * Return 0 on failure.
 */
const void* SGGetterImpl::getUntyped (bool allowMissing /*= false*/)
{
  std::string key = m_resolver.key();
  SG::DataProxy* proxy = m_sg->proxy (m_clid, key);
  if (!proxy) {
    if (!allowMissing) {
      REPORT_MESSAGE (MSG::FATAL) << "Can't find object in event store for "
                                  << m_typename << "(" << m_clid << ")/" 
                                  << key;
      REPORT_MESSAGE (MSG::INFO) << m_sg->dump();
    }
    return 0;
  }

  void* ptr = SG::DataProxy_cast (proxy, m_clid);
  if (!ptr) {
    if (!allowMissing) {
      REPORT_MESSAGE (MSG::FATAL)
        << "Null object retrieved for " << m_typename << "(" << m_clid << ")/" 
        << key;
    }
    return 0;
  }

  // Try to retrieve a corresponding aux store as well.
  m_sg->tryConstRetrieve<SG::IConstAuxStore> (key + "Aux.");

  return ptr;
}


/**
 * @brief Initialize this mixin class.
 *
 * Call this from @c initialize.
 */
StatusCode SGGetterImpl::initializeImpl ()
{
  CHECK( m_clidsvc.retrieve() );

  CHECK( nameToCLID (m_typename, m_clid, this->name(), m_clidsvc) );

  CHECK( m_resolver.initialize (m_clid, m_typename) );

  return StatusCode::SUCCESS;
}


} // namespace D3PD
