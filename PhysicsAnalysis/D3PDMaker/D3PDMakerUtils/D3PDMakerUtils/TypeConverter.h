// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerUtils/TypeConverter.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Helper to convert pointers.
 */


#ifndef D3PDMAKERUTILS_TYPECONVERTER_H
#define D3PDMAKERUTILS_TYPECONVERTER_H


#include "GaudiKernel/StatusCode.h"
#include <typeinfo>
#include <cassert>
#include <vector>
namespace SG {
  class BaseInfoBase;
}


namespace D3PD {


/**
 * @brief This is helper for converting between pointers of different types,
 *        given dynamically by @c std::type_info.
 *
 * The converter must first be initialized by calling @c init() with the
 * source and destination types.  If the conversion is allowable,
 * then this will return @c SUCCESS, and @c isValid() will return @c true.
 * Pointers can them be converted by calling @c convert() or
 * @c convertUntyped().
 *
 * The remaining methods retrieve the data that was passed to @c init().
 */
class TypeConverter
{
public:
  /**
   * @brief Default constructor.
   * 
   * This will create the object in an invalid state.
   */
  TypeConverter();


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
  StatusCode init (const std::type_info& src_ti,
                   const std::type_info& dst_ti);


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
  StatusCode init (const std::type_info& src_ti,
                   const std::vector<const std::type_info*>& dst_tis,
                   size_t& which);


  /**
   * @brief Alternate form of @c init, passing the destination
   *        type as a template argument.
   * @param src_ti The source type for the conversion.
   *
   * The types should be for the pointer value types.
   * I.e, if we want to convert from T* to U*,
   * pass typeid(T) as @c src_ti and @c U as the template argument.
   *
   * Returns @c FAILURE if the conversion is not allowable.
   * Either derived -> base or base -> derived conversions are allowed.
   */
  template <typename T>
  StatusCode init (const std::type_info& src_ti);


  /**
   * @brief Convert pointer.
   * @param The pointer to convert, as a pointer to the source type.
   * @return The converted pointer, as a pointer to the destination type.
   *
   * Will return 0 if the converter isn't properly initialized, or if
   * the conversion fails (for a base -> derived conversion).
   */
  const void* convertUntyped (const void* p) const;


  /**
   * @brief Convert pointer.
   * @param The pointer to convert, as a pointer to the source type.
   * @return The converted pointer, as a @c T*.
   *
   * @c T must be the same as the previously configured destination type.
   *
   * Will return 0 if the converter isn't properly initialized, or if
   * the conversion fails (for a base -> derived conversion).
   */
  template <class T>
  const T* convert (const void* p) const;


  /**
   * @brief Test to see if this converter has been properly initialized.
   */
  bool isValid() const;


  /// Return the configured source type.
  const std::type_info& srcTypeinfo() const;


  /// Return the configured destination type.
  const std::type_info& dstTypeinfo() const;

  /// Return the name of the source type, or an empty string.
  std::string srcName() const;

  /// Return the name of the destination type, or an empty string.
  std::string dstName() const;


private:
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
  StatusCode tryInit (const std::type_info& src_ti,
                      const std::type_info& dst_ti);


  /// The source type.
  const std::type_info* m_srcTypeinfo;

  /// The destination type.
  const std::type_info* m_dstTypeinfo;

  /// BaseInfo for the source type.
  const SG::BaseInfoBase* m_srcBIB;

  /// BaseInfo for the destination type.
  const SG::BaseInfoBase* m_dstBIB;

  enum Strategy {
    /// No valid conversion has been configured.
    INVALID,

    /// Source and destination types are identical.
    IDENTICAL,

    /// Destination is a base of source --- use source BaseInfo.
    SRC_BASE,

    /// Source is a base of destination --- use destination BaseInfo.
    DST_BASE
  };

  /// Strategy to use for the conversion.
  Strategy m_strategy;
};


} // namespace D3PD


#include "D3PDMakerUtils/TypeConverter.icc"


#endif // not D3PDUTILS_TYPECONVERTER_H
