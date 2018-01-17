/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerUtils/src/VectorFillerToolBase.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2010
 * @brief Common code for wrapping filled variables up into a vector.
 */


#include "D3PDMakerUtils/VectorFillerToolBase.h"
#include "D3PDMakerUtils/TypeNameConversions.h"
#include "D3PDMakerInterfaces/IIteration.h"
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/System.h"
#include "TClass.h"
#include "TROOT.h"
#include "TDataType.h"
#include "TVirtualCollectionProxy.h"
#include "boost/foreach.hpp"
#include <cstring>
#include <cassert>


namespace D3PD {


/**
 * @brief Move the variable pointer to the next element.
 */
inline
void VectorFillerToolBase::Var::next()
{
  *m_addr += m_valdiff;
}


/**
 * @brief Move all variable pointers to the next element.
 */
inline
void VectorFillerToolBase::Vars::next_all ()
{
  BOOST_FOREACH (Var& v, *this) {
    v.next();
  }
}


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
VectorFillerToolBase::VectorFillerToolBase (const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent)
  : AthAlgTool (type, name, parent),
    m_metadata (),
    m_prefix (),
    m_blockFillers (this),
    m_tree (0),
    m_nrow (0),
    m_nobj (0),
    m_objIndex (0)
{
  declareProperty ("BlockFillers", m_blockFillers,
                   "List of contained block filler tools.");
  declareProperty ("Prefix",       m_prefix = "",
                   "Variable name prefix for the contained blocks.");
  declareProperty ("NrowName",     m_nrowName = "n",
                   "Name of the variable for the count of rows. "
                   "Omitted if empty.");
  declareProperty ("NobjName",     m_nobjName = "",
                   "Name of the variable for the count of objects. "
                   "Omitted if empty.");
  declareProperty ("ObjIndexName", m_objIndexName = "",
                   "Name of the variable for the object index. "
                   "Omitted if empty.");
}


/**
 * @brief Standard Gaudi @c initialize method.
 */
StatusCode
VectorFillerToolBase::initialize()
{
  CHECK( AthAlgTool::initialize() );
  CHECK( m_blockFillers.retrieve() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Standard Gaudi @c finalize method.
 */
StatusCode VectorFillerToolBase::finalize()
{
  // Don't clear the m_vars variable, because some memory will never be freed
  // in that case...
  return StatusCode::SUCCESS;
}


/**
 * @brief Declare tuple variables.
 *
 * This is called at the start of the first event.
 */
StatusCode VectorFillerToolBase::doBook()
{
  // Create the count/index variables.
   if (!m_nrowName.empty()) {
      CHECK( m_metadata.addVariable (m_prefix + m_nrowName, m_nrow,
                                     "Number of ntuple rows.") );
      CHECK( m_tree->addVariable (m_prefix + m_nrowName, m_nrow,
                                  "Number of ntuple rows.") );
   }
   if (!m_nobjName.empty()) {
      CHECK( m_metadata.addVariable (m_prefix + m_nobjName, m_nobj,
                                     "Number of objects.") );
      CHECK( m_tree->addVariable (m_prefix + m_nobjName, m_nobj,
                                  "Number of objects.") );
   }
   if (!m_objIndexName.empty())
      CHECK( IAddVariable::addVariable (m_objIndexName, m_objIndex,
                                        "Index of this object.") );

  // Book child variables.
  for (size_t i = 0; i < m_blockFillers.size(); i++)
    CHECK( m_blockFillers[i]->book() );

  return StatusCode::SUCCESS;
}


/**
 * @brief Configure the tool.
 * @param tree The parent D3PD tree.
 * @param it The iteration object we'll use.  (Used to get the elt type.)
 *
 * This is called during initialization.
 */
StatusCode
VectorFillerToolBase::doConfigure (IAddVariable* tree, IIteration& it)
{
  m_tree = tree;

  // Get the type of object we'll get from the iteration.
  const std::type_info& ti = it.elementTypeinfo();

  // Configure all child tools.
  for (size_t i = 0; i < m_blockFillers.size(); i++)
    CHECK( m_blockFillers[i]->configureD3PD (this, ti) );

  return StatusCode::SUCCESS;
}


/**
 * @brief Extend vectors if needed.
 * @param n Number of entries currently made in the vectors.
 * @param sz Current size of the vectors.
 */
void VectorFillerToolBase::maybeExtend (size_t& n, size_t& sz)
{
  if (n >= sz) {
    sz = std::max(static_cast<size_t>(16), 2*sz);
    m_vars.resize_all (sz, n);
  }
}


/**
 * @brief Make one loop over block filler tools.
 * @param obj Object being processed.
 * @param objndx The index of the object we're currently processing.
 * @param n Number of entries currently made in the vectors.
 * @param sz Current size of the vectors.
 * @param codes_in Vector of status returns for each tool
 *                 from the first @c fill call.
 * @param codes_out[out] Vector of status returns for each tool
 *                       from this @c fill call.
 *
 * This function may be called more than once for an object
 * if @c AGAIN was requested.
 *
 * We loop over all block filler tools.  For each, we call
 * @c fillUntyped with the @c again parameter set depending
 * on whether @c codes_in for this tool is @c AGAIN.
 * We collect the return codes from each tool in @c codes_out.
 */
StatusCode
VectorFillerToolBase::fillOnce (const void* obj,
                                size_t objndx,
                                size_t& n,
                                size_t& sz,
                                const std::vector<unsigned long>& codes_in,
                                std::vector<unsigned long>& codes_out)
{
  codes_out.resize (m_blockFillers.size());

  // Extend vectors if needed.
  maybeExtend (n, sz);

  // Loop over block fillers.
  bool all_empty = true;
  for (size_t j = 0; j < m_blockFillers.size(); j++) {
    StatusCode scode =
      m_blockFillers[j]->fillUntyped(obj,
                                     codes_in[j] == IBlockFillerTool::AGAIN);
    codes_out[j] = scode.getCode();
    if (codes_out[j] != IBlockFillerTool::EMPTY)
      all_empty = false;
    CHECK( IBlockFillerTool::againok (scode) );
  }

  // If all tools returned empty, then ignore this row.
  // But don't ignore it if there are no block fillers at all
  // (it may be that an associator tool will be filling variables).
  if (!all_empty || m_blockFillers.empty()) {
    if (m_objIndex)
      *m_objIndex = objndx;

    // Count rows and update variable pointers.
    ++n;
    m_vars.next_all();
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Fill objects from an @c IIteration.
 * @param it Iteration object.  Must have already been reset.
 *
 * This will loop over the objects in the iteration; for each object,
 * it will loop over the contained block filler tools.
 */ 
StatusCode
VectorFillerToolBase::doFill (IIteration& it, size_t sizeHint)
{
  // Set variable vectors to the expected size.
  m_vars.resize_all (sizeHint);
  size_t n = 0;
  size_t nobj = 0;
  size_t sz = sizeHint;

  std::vector<unsigned long> codes_in;
  std::vector<unsigned long> codes_out;

  // Loop over objects.
  // Note: @c nextUntyped may try to fill variables.  Make sure there's space.
  maybeExtend (n, sz);
  while (const void* obj = it.nextUntyped() ) {
    // AGAIN processing.
    codes_in.clear();
    codes_in.resize (m_blockFillers.size(), StatusCode(StatusCode::SUCCESS).getCode());
    CHECK( fillOnce (obj, nobj, n, sz, codes_in, codes_out) );
    codes_in = codes_out;
    while (std::find (codes_out.begin(), codes_out.end(),
                      (unsigned long)IBlockFillerTool::AGAIN)
           != codes_out.end())
    {
      CHECK( fillOnce (obj, nobj, n, sz, codes_in, codes_out) );
    }

    ++nobj;

    it.releaseElementUntyped (obj);
    maybeExtend (n, sz);
  }

  // Set vectors to the proper final size.
  if (sz != n)
    m_vars.resize_all (n);

  // Save the counts.
  if (m_nrow)
    *m_nrow = n;
  if (m_nobj)
    *m_nobj = nobj;

  return StatusCode::SUCCESS;
}


/**
 * @brief Add a variable to the tuple.
 * @param name The name of the variable.
 * @param type The type of the variable.
 * @param ptr Pointer to the type of the variable.
 *            The pointer need not be initialized;
 *            the D3PD software will set the pointer
 *            prior to calling @c fill().
 * @param docstring Documentation string for this variable.
 * @param defval Pointer to the default value to use for this variable.
 *               Null for no default (generally means to fill with zeros).
 *               Of the type given by @c ti.
 *               Only works for basic types.
 */
StatusCode
VectorFillerToolBase::addVariable (const std::string& name,
                                   const std::type_info& ti,
                                   void* & ptr,
                                   const std::string& docstring /*= ""*/,
                                   const void* defval /*= 0*/)
{
  if (!m_tree) {
    // Called from constructor --- just initialize pointer.
    ptr = 0;
    return StatusCode::SUCCESS;
  }

  // Default value handling.
  size_t defsize = 0;
  char* defcopied = 0;
  if (defval) {
    EDataType dt = TDataType::GetType (ti);
    switch (dt) {
    case kChar_t:
    case kUChar_t:
    case kShort_t:
    case kUShort_t:
    case kInt_t:
    case kUInt_t:
    case kLong_t:
    case kULong_t:
    case kFloat_t:
    case kDouble_t:
    case kDouble32_t:
    case kchar:
    case kBool_t:
    case kLong64_t:
    case kULong64_t:
    case kFloat16_t:
      // ok
      break;

    default:
      REPORT_MESSAGE (MSG::ERROR)
        << "Requested a default value for variable " << name
        << " of type " << System::typeinfoName (ti)
        << "; but default values are only supported for basic types.";
      return StatusCode::FAILURE;
    }

    TDataType* tdt = gROOT->GetType (TDataType::GetTypeName (dt));
    assert (tdt != 0);
    defsize = tdt->Size();
    defcopied = new char[defsize];
    std::memcpy (defcopied, defval, defsize);
  }

  m_vars.push_back (Var (name, ti, ptr, docstring, defcopied, defsize));
  CHECK( m_vars.back().init (&m_metadata, m_prefix) );
  CHECK( m_vars.back().init (m_tree, m_prefix) );

  return StatusCode::SUCCESS;
}


/**
 * @brief Add a variable to the tuple.
 * @param name The name of the variable.
 * @param type The type of the variable.
 * @param ptr Pointer to the type of the variable.
 *            The pointer need not be initialized;
 *            the D3PD software will set the pointer
 *            prior to calling @c fill().
 * @param dim Dimension for the variable.
 *            (Presently unimplemented!)
 * @param docstring Documentation string for this variable.
 * @param defval Pointer to the default value to use for this variable.
 *               Null for no default (generally means to fill with zeros).
 *               Of the type given by @c ti.
 *               Only works for basic types.
 */
StatusCode
VectorFillerToolBase::addDimensionedVariable (const std::string& /*name*/,
                                              const std::type_info& /*ti*/,
                                              void* & /*ptr*/,
                                              const std::string& /*dim*/,
                                              const std::string& /*docstring = ""*/,
                                              const void* /*defval = 0*/)
{
  REPORT_MESSAGE(MSG::ERROR) << "addDimensionedVariable not yet implemented.";
  return StatusCode::FAILURE;
}


/**
 * @brief Constructor.
 * @param name The variable name (without prefix).
 * @param ti The element type of the variable.
 * @param ptr The variable pointer provided by the block filler tool.
 * @param docstring Documentation string.
 * @param defval Pointer to the default value to use for this variable.
 *               Null for no default.
 *               Of the type given by @c ti.
 *               Only works for basic types.
 *               We take ownership of this.
 * @param defsize Size of the object pointed at by defval.
 */
VectorFillerToolBase::Var::Var (const std::string& name,
                                const std::type_info& ti,
                                void* & ptr,
                                const std::string& docstring,
                                char* defval,
                                size_t defsize)
  : m_name (name),
    m_ti (&ti),
    m_addr (reinterpret_cast<char**> (&ptr)),
    m_docstring (docstring),
    m_proxy (0),
    m_valdiff (0),
    m_contptr (0),
    m_default (defval),
    m_defsize (defsize)
{
}


/**
 * @brief Initialize the variable.
 * @param tree The parent tree.
 * @param prefix The variable name prefix.
 */
StatusCode VectorFillerToolBase::Var::init (IAddVariable* tree,
                                            const std::string& prefix)
{
  // Get the name of the element variable.
  std::string eltname = typeinfoToName (*m_ti);

  // Use vector<int> instead of vector<bool>.
  if (eltname == "bool")
    eltname = "int";

  // Construct the vector name.
  std::string vname = "std::vector<" + eltname;
  if (vname[vname.size()-1] == '>')
    vname += ' ';
  vname += '>';

  // Fetch the root dictionary for the vector and find the proxy.
  TClass* cls = TClass::GetClass (vname.c_str());
  if (!cls) {
    REPORT_MESSAGE(MSG::ERROR) << "Can't find TClass for type "
                               << vname
                               << " for variable "
                               << prefix + m_name;
    return StatusCode::FAILURE;
  }
  m_proxy = cls->GetCollectionProxy();

  if (!m_proxy) {
    REPORT_MESSAGE(MSG::ERROR) << "Can't find root collection proxy for type "
                               << vname
                               << " for variable "
                               << prefix + m_name;
    return StatusCode::FAILURE;
  }

  if (!m_proxy->GetCollectionClass() ||
      !m_proxy->GetCollectionClass()->GetTypeInfo())
  {
    REPORT_MESSAGE(MSG::ERROR) << "Dictionary error for collection class "
                               << vname
                               << " for variable "
                               << prefix + m_name;
    return StatusCode::FAILURE;
  }

  // Will be set at the first resize().
  m_valdiff = 0;

  // Create the vector.
  m_contptr = 0;
  return tree->addVariable (prefix + m_name,
                            *m_proxy->GetCollectionClass()->GetTypeInfo(),
                            m_contptr,
                            m_docstring);
}


/**
 * @brief Resize the vector.
 * @param sz The new vector size.
 * @param pos After the resize, move the variable pointer to this element.
 */
void VectorFillerToolBase::Var::resize (size_t sz, size_t pos)
{
  // Resize the vector.
  TVirtualCollectionProxy::TPushPop pushcont (m_proxy, m_contptr);
  size_t oldsz = m_proxy->Size();
  m_proxy->Allocate (sz, false);
  if (sz == 0) {
    *m_addr = 0;
    return;
  }

  char* begin = static_cast<char*> (m_proxy->At(0));

  // Calculate the element offset, if needed.
  // We do this by finding the difference between the first two
  // elements, so we can only do this if there are at least two elements
  // in the vector.  But if there are less than two elements, then
  // the offset doesn't matter anyway.
  if (m_valdiff == 0 && sz >= 2)
    m_valdiff = static_cast<char*> (m_proxy->At(1)) - begin;

  // Set the pointer properly.
  *m_addr = begin + pos * m_valdiff;

  // Fill in default value if requested.
  if (m_default && sz > oldsz) {
    if (sz == 1) {
      std::memcpy (begin, m_default, m_defsize);
    }
    else {
      assert (m_defsize <= m_valdiff);
      for (char* p = begin + oldsz * m_valdiff;
           p < begin + sz * m_valdiff;
           p += m_valdiff)
      {
        std::memcpy (p, m_default, m_defsize);
      }
    }
  }
}


/**
 * @brief Free allocated storage.
 *
 * Don't do this from the destructor since these guys get used in a vector.
 */
void VectorFillerToolBase::Var::free ()
{
  delete [] m_default;
}


/**
 * @brief Destructor.
 */
VectorFillerToolBase::Vars::~Vars()
{
  BOOST_FOREACH (Var& v, *this) {
    v.free();
  }
}


/**
 * @brief Resize all vector.
 * @param sz The new vector size.
 * @param pos After the resize, move the variable pointers to this element.
 */
void VectorFillerToolBase::Vars::resize_all (size_t sz, size_t pos /*= 0*/)
{
  BOOST_FOREACH (Var& v, *this) {
    v.resize (sz, pos);
  }
}


} // namespace D3PD
