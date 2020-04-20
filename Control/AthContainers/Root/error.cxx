/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AthContainers/error.cxx
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>, scott snyder <snyder@bnl.gov>
 * @date Sep, 2013
 * @brief Helper for emitting error messages.
 */


#include "AthContainers/tools/error.h"
#ifdef __GXX_ABI_VERSION
# include <cstdlib>
# include <cxxabi.h>
#endif
#include <iomanip>
#include <iostream>
#include <sstream>


#ifdef XAOD_STANDALONE


namespace AthContainers_detail {


/// Code mimicking Athena's message formatting, in standalone builds
void reportErrorStandalone (const std::string& context,
                            const std::string& file, int line,
                            const std::string& msg) {

  // Make sure that the message's source/location is not longer than a
  // pre-set maximum value.
  std::string source (context);
  static constexpr std::size_t CONTEXT_WIDTH = 18;
  if (source.size() > CONTEXT_WIDTH) {
    source = source.substr(0, CONTEXT_WIDTH - 3);
    source += "...";
  }

  // Print the message to stdout/std::cout.
  std::ostringstream output;
  output << std::setiosflags(std::ios::left)
         << std::setw(CONTEXT_WIDTH) << source << "  ERROR "
         << file << ":" << line << ": "
         << msg;
  std::cout << output.str() << std::endl;
  return;
}


/**
 * This function does pretty much the same thing as Gaudi's
 * <code>System::typeinfoName</code>. The only reason for having it
 * is to make the compilation of the package easier outside of
 * Athena.
 *
 * @param ti The type identifier of the class we're interested in
 * @returns The string name of the type
 */
std::string typeinfoName (const std::type_info& ti)
{
  // In case the "translation" is not successful, the fallback is to
  // keep the original, mangled name:
  std::string result = ti.name();

#ifdef __GXX_ABI_VERSION
  // Try to demangle the name:
  int status;
  char* typeName = abi::__cxa_demangle( ti.name(), 0, 0, &status );
  if (status == 0) {
    // Take the demangled name, and free up the memory allocated in
    // the process:
    result = typeName;
    ::free( typeName );
  }
#endif

  return result;
}


} // namespace AthContainers_detail


#endif // XAOD_STANDALONE
