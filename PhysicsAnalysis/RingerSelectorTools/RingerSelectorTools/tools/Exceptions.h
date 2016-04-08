/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Exceptions.h 667905 2015-05-18 19:07:55Z wsfreund $
#ifndef RINGERSELECTORTOOLS_TOOLS_EXCEPTIONS_H
#define RINGERSELECTORTOOLS_TOOLS_EXCEPTIONS_H

// STL includes:
#include <stdexcept>
#include <string>

#ifndef INCLUDE_HEADER_ONLY
#define INCLUDE_HEADER_ONLY // Avoid circular includes
#endif

// Local includes:
#include "RingerSelectorTools/RingerSelectorToolsDefs.h"
#include "RingerSelectorTools/procedures/Types.h"
#undef INCLUDE_HEADER_ONLY

namespace Ringer {

namespace Exceptions {

/**
 * Exception telling user to implement type on this file
 **/
class no_such_type : public std::runtime_error {
  public:

    no_such_type(const char* scope, std::string type = "",
        std::string file = ""):
      std::runtime_error(std::string("Could not recognize ") +
          scope + " :" + type + ". If it should be "
        "available, add it to file " + file){;}

    no_such_type(preProcEnum_t p,
        std::string file = ""):
      std::runtime_error(std::string("Could not recognize "
            "PreProcessorTypes(") + std::to_string(int(p)) +
            "). Make sure to add it to file " + file + 
            " and also add it to getType and toStr functions."){;}

    no_such_type(discrEnum_t d,
        std::string file = ""):
      std::runtime_error(std::string("Could not recognize "
            "DiscriminatorTypes(") + std::to_string(int(d)) +
            "). Make sure to add it to file " + file + " and also " 
            "add it to getType and toStr functions."){;}

    no_such_type(thresEnum_t t,
        std::string file = ""):
      std::runtime_error(std::string("Could not recognize "
            "ThresholdTypes(") + std::to_string(int(t)) +
            "). Make sure to add it to file " + file + 
            " and also add it to getType and toStr functions."){;}

    no_such_type(SegmentationType e, std::string file = ""):
      std::runtime_error(std::string("Could not recognize "
            "EtaDependency(") + std::to_string(int(e)) +
            "). Make sure to add it to file " + file + 
            " and also add it to getType and toStr functions."){;}

    no_such_type(EtaDependency e, std::string file = ""):
      std::runtime_error(std::string("Could not recognize "
            "EtaDependency(") + std::to_string(int(e)) +
            "). Make sure to add it to file " + file + 
            " and also add it to getType and toStr functions."){;}

    no_such_type(EtDependency e, std::string file = ""):
      std::runtime_error(std::string("Could not recognize "
            "EtDependency(") + std::to_string(int(e)) +
            "). Make sure to add it to file " + file + 
            " and also add it to getType and toStr functions."){;}

    no_such_type(Requirement e, std::string file = ""):
      std::runtime_error(std::string("Could not recognize "
            "Requirement(") + std::to_string(int(e)) +
            "). Make sure to add it to file " + file + 
            " and also add it to getType and toStr functions."){;}
};

} // namespace Exceptions
} // namespace Ringer

#endif // RINGERSELECTORTOOLS_TOOLS_EXCEPTIONS_H
