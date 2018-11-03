/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "FlavorTagDiscriminants/customGetter.h"

namespace {
  // this function is not at all optimized, but then it doesn't have
  // to be since it should only be called in the initialization stage.
  std::function<double(const xAOD::Jet&)> customGetter(const std::string& name)
  {
    if (name == "pt") {
      return [](const xAOD::Jet& j) {return j.pt();};
    }
    if (name == "abs_eta") {
      return [](const xAOD::Jet& j) {return std::abs(j.eta());};
    }
    throw std::logic_error("no match for custom getter " + name);
  }
}

namespace FlavorTagDiscriminants {

  // as long as we're giving lwtnn pair<name, double> objects, we
  // can't use the raw getter functions above (which only return a
  // double). Instead we'll wrap those functions in another function,
  // which returns the pair we wanted.
  std::function<std::pair<std::string, double>(const xAOD::Jet&)>
  customGetterAndName(const std::string& name) {
    auto getter = customGetter(name);
    return [name, getter](const xAOD::Jet& j) {
             return std::make_pair(name, getter(j));
           };
  }
}
