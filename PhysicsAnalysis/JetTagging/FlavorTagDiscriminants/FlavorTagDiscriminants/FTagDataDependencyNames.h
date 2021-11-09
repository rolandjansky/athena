#ifndef FTAG_DATA_DEPENDENCY_NAMES_H
#define FTAG_DATA_DEPENDENCY_NAMES_H

#include <set>
#include <string>

namespace FlavorTagDiscriminants {
  struct FTagDataDependencyNames {
    std::set<std::string> trackInputs;
    std::set<std::string> bTagInputs;
    std::set<std::string> bTagOutputs;
  };
}

#endif
