#ifndef DL2_H
#define DL2_H

// EDM includes
#include "xAODJet/Jet.h"

// STL includes
#include <string>
#include <vector>
#include <regex>
#include <functional>
#include <exception>

namespace FlavorTagDiscriminants {

  enum class EDMType {INT, FLOAT, DOUBLE};

  // these regexes are used to figure out what type the b-tagging
  // inputs are
  struct InputRegex
  {
    std::regex regex;
    EDMType type;
  };

  // All the code here should just be a thin wrapper on lwtnn plus
  // some stuff to pull variables out of the EDM
  struct DL2Config
  {
    std::string nnFilePath;
    std::vector<InputRegex> inputRegexes;
  };

  // internally we want a bunch of std::functions that return pairs to
  // populate the lwtnn input map. We define a functor here to deal
  // with the b-tagging cases.
  template <typename T>
  class BVarGetter
  {
  private:
    typedef SG::AuxElement AE;
    AE::ConstAccessor<T> m_getter;
    AE::ConstAccessor<char> m_default_flag;
    std::string m_name;
  public:
    BVarGetter(std::string name, std::string default_flag):
      m_getter(name),
      m_default_flag(default_flag),
      m_name(name)
      {
      }
    std::pair<std::string, double> operator()(const xAOD::Jet& jet) {
      const xAOD::BTagging* btag = jet.btagging();
      if (!btag) throw std::logic_error("can't find btagging object");
      return {m_name, m_default_flag(*btag) ? NAN : m_getter(*btag)};
    }
  };
}
#endif
