/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: SGDataVectorGetterTool.cxx 790090 2016-12-16 05:31:29Z ssnyder $
/**
 * @file D3PDMakerCoreComps/src/SGDataVectorGetterTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Getter tool to retrieve DataVector/List collections from StoreGate.
 */


#include "SGDataVectorGetterTool.h"
#include "AthContainers/tools/DVLInfo.h"
#include "TROOT.h"
#include "TMethodCall.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
SGDataVectorGetterTool::SGDataVectorGetterTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : SGGetterImpl (name, this->evtStore()),
      CollectionGetterToolImpl (type, name, parent),
      m_athenaPoolCnvSvc ("AthenaPoolCnvSvc", name),
      m_info (0),
      m_it (0)
{
  SGGETTERIMPL_PROPS;
}


/**
 * @brief Standard Gaudi @c initialize method.
 */
StatusCode SGDataVectorGetterTool::initialize()
{
  CHECK( m_athenaPoolCnvSvc.retrieve() );

  // Initialize the code getting the collection from SG.
  CHECK( SGGetterImpl::initializeImpl() );

  // Find the DataVector/DataList information for this collection.
  m_info = getInfo (typeinfo());

  // We couldn't find it.
  // Maybe we haven't yet loaded a library that has a complete
  // instantiation of the DataVector class.  Try loading the pool
  // converter, and see if that'll do it.
  if (!m_info) {
    m_athenaPoolCnvSvc->converter (clid());
    m_info = getInfo (typeinfo());
  }

  // Maybe it didn't.  (Once we make the info method virtual, then loading
  // the converter should always do it.  But now, maybe not.)
  // As a last attempt, try to load a dictionary for the class.
  if (!m_info) {
    TClass* cls = gROOT->GetClass (m_typename.c_str());
    cls->GetBaseClass ("SG::AuxElement");
    m_info = getInfo (typeinfo());
  }

  // Ok, one more try...
  if (!m_info) {
    TClass* cls = gROOT->GetClass (m_typename.c_str());
    TMethodCall meth (cls, "initHelper", "");
    if (meth.IsValid())
      meth.Execute();
    m_info = getInfo (typeinfo());
  }
  if (!m_info) {
    TClass* cls = gROOT->GetClass (m_typename.c_str());
    TMethodCall meth (cls, "dvlinfo", "");
    if (meth.IsValid())
      meth.Execute();
    m_info = getInfo (typeinfo());
  }

  if (!m_info) {
    REPORT_MESSAGE (MSG::ERROR)
      << "Can't find DataVector/DataList info for class " << m_typename;
    return StatusCode::FAILURE;
  }

  // We need to convert from the type we get from SG to the DV/DL base class.
  CHECK( m_converter.init (typeinfo(), m_info->tinfo()) );

  return CollectionGetterToolImpl::initialize();
}


/**
 * @brief Standard Gaudi @c finalize method.
 */
StatusCode SGDataVectorGetterTool::finalize()
{
  delete m_it;
  m_it = 0;
  return StatusCode::SUCCESS;
}


/**
 * @brief Return the element type of the collection.
 *
 * I.e., @c nextUntyped returns a pointer to this type.
 */
const std::type_info& SGDataVectorGetterTool::elementTypeinfo() const
{
  return m_info->elt_tinfo();
}


/**
 * @brief Reset the iteration to the start of the collection.
 * @param allowMissing If true, then we should not generate errors
 *        if the requested object is missing.
 *
 * Return failure if the container cannot be retrieved.
 */
StatusCode SGDataVectorGetterTool::reset (bool allowMissing /*= false*/)
{
  delete m_it;
  const void* p0 = getUntyped (allowMissing);
  if (allowMissing && p0 == 0) {
    m_it = 0;
    return StatusCode::SUCCESS;
  }
  const void* p = m_converter.convertUntyped (p0);
  if (p) {
    SG::AuxVectorBase* auxbase =
      m_info->base (const_cast<void*> (p));
    if (auxbase && auxbase->trackIndices() && !auxbase->getConstStore()) {
      // Try to retrieve a corresponding aux store.
      const SG::IConstAuxStore* store =
        evtStore()->tryConstRetrieve<SG::IConstAuxStore> (m_resolver.key() + "Aux.");
      if (store)
        auxbase->setStore (store);
    }
    m_it = m_info->iterator (p);
    return StatusCode::SUCCESS;
  }
  else {
    m_it = 0;
    return StatusCode::FAILURE;
  }
}


/**
 * @brief Return a pointer to the next element in the collection.
 *
 * Return 0 when the collection has been exhausted.
 */
const void* SGDataVectorGetterTool::nextUntyped()
{
  if (m_it)
    return m_it->next();
  return 0;
}


/**
 * @brief Return an estimate of the number of elements in the iteration.
 * @param allowMissing If true, then we should not generate errors
 *        if the requested object is missing.
 *
 * This can be used to pre-allocate memory.
 * (It's possible that this isn't known in advance of
 * iterating over the entire collection, for example
 * if a selection is being applied, so this is only a hint.)
 */
size_t SGDataVectorGetterTool::sizeHint (bool allowMissing /*= false*/)
{
  const void* p = m_converter.convertUntyped (getUntyped (allowMissing));
  if (!p) return 0;
  return m_info->size (const_cast<void*> (p));
}


/**
 * @brief Retrieve DataVector/List information for this collection.
 * @param ti Type that we get from SG.
 */
const DataModel_detail::DVLInfoBase*
SGDataVectorGetterTool::getInfo (const std::type_info& ti)
{
  // If we have info for this class directly, use that.
  DataModel_detail::DVLInfoBase* info =
    DataModel_detail::DVLInfoBase::find (ti);
  if (info) return info;

  // We gotta check the base classes.  Fail if we don't have
  // base info.
  const SG::BaseInfoBase* bib = SG::BaseInfoBase::find (ti);
  if (!bib) return 0;

  // Pick the most-derived class among the possible bases.
  std::vector<const std::type_info*> bases = bib->get_ti_bases();
  for (size_t i = 0; i < bases.size(); i++) {
    DataModel_detail::DVLInfoBase* this_info =
      DataModel_detail::DVLInfoBase::find (*bases[i]);
    if (this_info) {
      if (info) {
        bib = SG::BaseInfoBase::find (this_info->tinfo());
        if (bib && bib->is_base (info->tinfo()))
          info = this_info;
      }
      else
        info = this_info;
    }
  }
  return info;
}


} // namespace D3PD
