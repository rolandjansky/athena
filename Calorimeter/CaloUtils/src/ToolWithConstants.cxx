/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CaloUtils/src/ToolWithConstants.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2020
 * @brief Hold constants for a tool.  Out-of-line implementations.
 */


#include "CaloUtils/ToolWithConstants.h"


namespace CaloUtils {


/**
 * @brief Constructor
 * @param toolName Name of the tool.
 * @param prefix Constant prefix in COOL.
 * @param DBHandle Handle key for the conditions object.
 */
ToolWithConstantsImpl::ToolWithConstantsImpl
  (const std::string& toolName,
   const std::string& prefix,
   SG::ReadCondHandleKey<CaloRec::ToolConstants>& DBHandle)
  : m_toolName (toolName),
    m_prefix (prefix),
    m_DBHandle (DBHandle)
{
}


/**
 * @brief Initialize.
 */
StatusCode ToolWithConstantsImpl::initialize()
{
  ATH_CHECK( m_DBHandle.initialize (SG::AllowEmpty) );
  return StatusCode::SUCCESS;
}


/**
 * @brief Create a @c Context object.
 *
 * This can then be passed to @c Constant::operator().
 */
ToolWithConstantsImpl::Context
ToolWithConstantsImpl::context (const EventContext& ctx) const
{
  const CaloRec::ToolConstants* constants = nullptr;
  if (!m_DBHandle.empty()) {
    SG::ReadCondHandle<CaloRec::ToolConstants> consts (m_DBHandle, ctx);
    constants = consts.retrieve();
  }
  return Context (constants, ctx);
}


/**
 * @brief Add a new constant to the tool.
 * @param c Constant to add.
 */
void ToolWithConstantsImpl::addConstant (IToolConstant* c)
{
  m_declaredConstants.push_back (c);
}


/**
 * @brief Dump method (for debugging)
 * @param stream Ostream to which to write.
 * @param name Name to go in output
 * @param ctx Event context.
 */
void ToolWithConstantsImpl::writeConstants (std::ostream& stream,
                                            const std::string& name,
                                            const EventContext& ctx) const
{
  stream << "Member: m_constants" << std::endl;
  m_constants.writeConstants (stream,name);
  if (!m_DBHandle.empty()) {
    SG::ReadCondHandle<CaloRec::ToolConstants> consts (m_DBHandle, ctx);
    stream << "Member: m_DBconstants" << std::endl;
    consts->writeConstants (stream,name);
  }
}


/**
 * @brief Merge our constants into @c out with the proper prefix.
 * @param typeName Name of the type of the tool.
 * @param toolVersion Version of the tool.
 * @param myctx Internal Context structure.
 * @param isDummy isDummy constant from the tool
 * @param[out] out Object to receive our constants.
 */
StatusCode
ToolWithConstantsImpl::mergeConstants (const std::string& typeName,
                                       int toolVersion,
                                       Context& myctx,
                                       bool isDummy,
                                       CaloRec::ToolConstants& out) const
{
  if (out.clsname().empty()) {
    out.clsname (typeName);
    out.version (toolVersion);
  }
  else if (out.clsname() != typeName) {
    REPORT_ERROR_WITH_CONTEXT(StatusCode::FAILURE, m_toolName)
      << "Trying to write to ToolConstants for tools of differing types: "
      << out.clsname() << " and " << typeName;
    return StatusCode::FAILURE;
  }
  else if (out.version() != toolVersion) {
    REPORT_ERROR_WITH_CONTEXT(StatusCode::FAILURE, m_toolName)
      << "Trying to write to ToolConstants for differing versions of tool "
      << out.clsname() << ": " << out.version() << " and " << toolVersion;
    return StatusCode::FAILURE;
  }

  for (IToolConstant* c : m_declaredConstants) {
    std::string cname = c->name();
    std::string prefname = this->m_prefix + cname;
    const CaloRec::Arrayrep* rep = nullptr;
    if (m_constants.hasrep (cname)) {
      rep = &m_constants.getrep (m_toolName, cname);
    }
    if (!c->setFromJO() &&
        myctx.m_constants &&
        myctx.m_constants->hasrep (prefname))
    {
      rep = &myctx.m_constants->getrep (m_toolName, prefname);
    }
    if (!rep) {
      throw ExcConstantNotSet (m_toolName, cname);
    }

    if (!isDummy || cname == "isDummy")
      out.setrep (this->m_prefix + cname, *rep);
  }
  return StatusCode::SUCCESS;
}


} // namespace CaloUtils
