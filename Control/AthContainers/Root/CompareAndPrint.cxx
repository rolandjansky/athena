/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CompareAndPrint.cxx 470529 2011-11-24 23:54:22Z ssnyder $
/**
 * @file  AthContainers/src/CompareAndPrint.cxx
 * @author scott snyder, Paolo Calafiura, etc
 * @date May 2005; rewritten from earlier version.
 * @brief helper class for remove_duplicates
 *        Out-of-line implementations.
 */

#include "AthContainers/tools/CompareAndPrint.h"
#include "AthContainers/tools/error.h"
#include <iostream>


namespace DataModel_detail {

/**
 * @brief Duplicate elements were found; print a warning.
 * @param ti @c type_info for the element.
 * @param f The element pointer.
 */
void CompareAndPrint::warn (const std::type_info& ti, const void* f) const
{
  std::cout << "DataVector ERROR: duplicated pointer found in a DataVector of " 
            << AthContainers_detail::typeinfoName(ti)
            << " owning its elements! "
            << std::hex << f << std::dec << std::endl;
}

} // namespace DataModel_detail
