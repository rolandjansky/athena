// $Id$
/**
 * @file AthenaKernel/src/CondCont.cpp
 * @author Vakho, Charles, Scott
 * @date Apr, 2018
 * @brief Hold mappings of ranges to condition objects.
 */


#include "AthenaKernel/CondCont.h"


/// Default name of the global conditions cleaner service.
std::string CondContBase::s_cleanerSvcName = "Athena::ConditionsCleanerSvc";


/**
 * @brief Name of the category.
 */
const char* CondContBase::Category::name() const
{
  return "CondCont";
}


/**
 * @brief Description for code within this category.
 */
std::string CondContBase::Category::message (code_t code) const
{
  if (code == static_cast<code_t> (CondContStatusCode::DUPLICATE)) {
    return "DUPLICATE";
  }
  return StatusCode::Category::message (code);
}


/**
 * @brief Is code considered success?
 */
bool CondContBase::Category::isSuccess (code_t code) const
{
  return code == static_cast<code_t>( CondContStatusCode::DUPLICATE ) ||
    code == static_cast<code_t>( CondContStatusCode::SUCCESS );
}


/// Helper to test whether a code is DUPLICATE.
bool CondContBase::Category::isDuplicate (code_t code)
{
  return code == static_cast<code_t> (CondContStatusCode::DUPLICATE);
}


/// Helper to test whether a code is DUPLICATE.
bool CondContBase::Category::isDuplicate (StatusCode code)
{
  return isDuplicate (code.getCode());
}


STATUSCODE_ENUM_IMPL (CondContStatusCode, CondContBase::Category)


/**
 * @brief Internal constructor.
 * @param rcusvc RCU service instance.
 * @param CLID of the most-derived @c CondCont.
 * @param id CLID+key for this object.
 * @param proxy @c DataProxy for this object.
 */
CondContBase::CondContBase (Athena::IRCUSvc& /*rcusvc*/,
                            CLID clid,
                            const DataObjID& /*id*/,
                            SG::DataProxy* /*proxy*/)
  : m_clid (clid),
    m_cleanerSvc (s_cleanerSvcName, "CondContBase")
{
  if (!m_cleanerSvc.retrieve().isSuccess()) {
    std::abort();
  }
}


/**
 * @brief Tell the cleaner that a new object was added to the container.
 */
StatusCode CondContBase::inserted (const EventContext& ctx)
{
  return m_cleanerSvc->condObjAdded (ctx, *this);
}


/**
 * @brief Allow overriding the name of the global conditions cleaner
 *        service (for testing purposes).
 * @param name The name of the global conditions cleaner service.
 */
void CondContBase::setCleanerSvcName (const std::string& name)
{
  s_cleanerSvcName = name;
}

