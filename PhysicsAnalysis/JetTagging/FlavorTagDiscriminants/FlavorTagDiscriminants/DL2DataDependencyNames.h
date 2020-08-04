#ifndef DL2_DATA_DEPENDENCY_NAMES_H
#define DL2_DATA_DEPENDENCY_NAMES_H

#include <set>
#include <string>

namespace FlavorTagDiscriminants {
  struct DL2DataDependencyNames {
    std::set<std::string> trackInputs;
    std::set<std::string> bTagInputs;
    std::set<std::string> bTagOutputs;
  };
}

#endif
