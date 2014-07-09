/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerUtils/src/TypeConverter.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Helper to convert pointers.
 */


#include "D3PDMakerUtils/TypeConverter.h"
#include "SGTools/BaseInfo.h"
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/System.h"
#include <sstream>


namespace D3PD {


/**
 * @brief Default constructor.
 * 
 * This will create the object in an invalid state.
 */
TypeConverter::TypeConverter()
  : m_srcTypeinfo (0),
    m_dstTypeinfo (0),
    m_srcBIB (0),
    m_dstBIB (0),
    m_strategy (INVALID)
{
}


/**
 * @brief Initialize the converter.
 * @param src_ti The source type for the conversion.
 * @param dst_ti The destination type for the conversion.
 *
 * The types should be for the pointer value types.
 * I.e, if we want to convert from T* to U*,
 * pass typeid(T) and typeid(U) as @c src_ti and @c dst_ti, respectively.
 *
 * Returns @c FAILURE if the conversion is not allowable.
 * Either derived -> base or base -> derived conversions are allowed.
 */
StatusCode TypeConverter::init (const std::type_info& src_ti,
                                const std::type_info& dst_ti)
{
  if (tryInit (src_ti, dst_ti).isFailure()) {
    // Not a valid conversion.
    REPORT_MESSAGE (MSG::ERROR)
      << "Incompatible conversion: from " << srcName() << " to " << dstName();
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


/**
 * @brief Initialize the converter.
 * @param src_ti The source type for the conversion.
 * @param dst_tis List of possible destination types for the conversion.
 * @param[out] which Index of accepted conversion.
 *
 * The types should be for the pointer value types.
 * I.e, if we want to convert from T* to U*,
 * pass typeid(T) and typeid(U) as @c src_ti and @c dst_ti, respectively.
 *
 * All possible destination types are tried; the first one
 * that succeeds is accepted, and WHICH is set to the index
 * of that type.
 * Either derived -> base or base -> derived conversions are allowed.
 *
 * Returns @c FAILURE if no candidate conversions are allowable.
 */
StatusCode
TypeConverter::init (const std::type_info& src_ti,
                     const std::vector<const std::type_info*>& dst_tis,
                     size_t& which)
{
  size_t sz = dst_tis.size();
  for (which = 0; which < sz; which++) {
    if (tryInit (src_ti, *dst_tis[which]).isSuccess())
      return StatusCode::SUCCESS;
  }

  // Produce an error message listing the possible destination types.
  std::ostringstream dst_names;
  for (which = 0; which < sz; which++) {
    if (which > 0)
      dst_names << " ";
    dst_names << System::typeinfoName (*dst_tis[which]);
  }
  
  REPORT_MESSAGE (MSG::ERROR)
    << "Incompatible conversion: from " << srcName() << " to one of ["
    << dst_names.str() << "]";
  return StatusCode::FAILURE;
}



/**
 * @brief Convert pointer.
 * @param The pointer to convert, as a pointer to the source type.
 * @return The converted pointer, as a pointer to the destination type.
 *
 * Will return 0 if the converter isn't properly initialized, or if
 * the conversion fails (for a base -> derived conversion).
 */
const void* TypeConverter::convertUntyped (const void* p) const
{
  switch (m_strategy) {
  case IDENTICAL:
    return p;

  case SRC_BASE:
    return m_srcBIB->cast (const_cast<void*> (p), *m_dstTypeinfo);

  case DST_BASE:
    return m_dstBIB->castTo (const_cast<void*> (p), *m_srcTypeinfo);

  default:
    std::abort();
  }
}


/**
 * @brief Return the name of the source type, or an empty string.
 */
std::string TypeConverter::srcName() const
{
  if (!m_srcTypeinfo) return "";
  return System::typeinfoName (*m_srcTypeinfo);
}


/**
 * @brief Return the name of the destination type, or an empty string.
 */
std::string TypeConverter::dstName() const
{
  if (!m_dstTypeinfo) return "";
  return System::typeinfoName (*m_dstTypeinfo);
}


/**
 * @brief Initialize the converter.
 * @param src_ti The source type for the conversion.
 * @param dst_ti The destination type for the conversion.
 *
 * The types should be for the pointer value types.
 * I.e, if we want to convert from T* to U*,
 * pass typeid(T) and typeid(U) as @c src_ti and @c dst_ti, respectively.
 *
 * Returns @c FAILURE if the conversion is not allowable.
 * Either derived -> base or base -> derived conversions are allowed.
 *
 * Helper: this doesn't print error messages.
 */
StatusCode TypeConverter::tryInit (const std::type_info& src_ti,
                                   const std::type_info& dst_ti)
{
  // Find the source BaseInfo.
  if (!m_srcBIB || m_srcBIB->typeinfo() != src_ti) {
    m_srcTypeinfo = &src_ti;
    m_srcBIB = SG::BaseInfoBase::find (src_ti);
  }

  // Find the destination BaseInfo.
  m_dstTypeinfo = &dst_ti;
  m_dstBIB = SG::BaseInfoBase::find (dst_ti);

  // Test for source and destination types identical.
  if (m_srcTypeinfo == m_dstTypeinfo) {
    m_strategy = IDENTICAL;
    return StatusCode::SUCCESS;
  }

  if (m_srcBIB && m_srcBIB->is_base (*m_dstTypeinfo)) {
    // Destination is a base of source --- use source BaseInfo.
    m_strategy = SRC_BASE;
    return StatusCode::SUCCESS;
  }

  if (m_dstBIB && m_dstBIB->is_base (*m_srcTypeinfo)) {
    // Source is a base of destination --- use destination BaseInfo.
    m_strategy = DST_BASE;
    return StatusCode::SUCCESS;
  }

  return StatusCode::FAILURE;
}


} // namespace D3PD
