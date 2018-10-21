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

  // Most b-tagging variables have "default" flags defined.
  struct DefaultRegex
  {
    std::regex regex;
    std::string default_flag;
  };

  // All the code here should just be a thin wrapper on lwtnn plus
  // some stuff to pull variables out of the EDM
  struct DL2Config
  {
    std::string nn_file_path;
    std::vector<InputRegex> input_regexes;
    std::vector<DefaultRegex> defatul_regexes;
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
    std::pair<std::string, double> operator()(const xAOD::Jet& jet) const {
      const xAOD::BTagging* btag = jet.btagging();
      if (!btag) throw std::logic_error("can't find btagging object");
      return {m_name, m_default_flag(*btag) ? NAN : m_getter(*btag)};
    }
  };

  // now we define some additional helper functions
  //
  // Matcher
  template <typename T>
  class Matcher
  {
  private:
    std::vector<std::pair<std::regex, T> > m_regexes;
  public:
    Matcher(const std::vector<std::pair<std::regex, T> >& regexes):
      m_regexes(regexes)
      {}
    T operator()(const std::string& var_name) const {
      for (const auto& pair: m_regexes) {
        if (std::regex_match(var_name, pair.first)) {
          return pair.second;
        }
      }
      throw std::logic_error("no match found for " + var_name);
    }
  };
  //
  // Type finder
  class TypeFinder
  {
  public:
    TypeFinder(std::vector<InputRegex>);
    EDMType operator()(const std::string&) const;
  private:
    std::vector<InputRegex> m_regexes;
  };
  // Default finder
  class DefaultFinder
  {
  public:
    DefaultFinder(std::vector<DefaultRegex>);
    std::string operator()(const std::string&) const;
  private:
    std::vector<DefaultRegex> m_regexes;
  };
  //
  // Filler function
  std::function<std::pair<std::string, double>(const xAOD::Jet&)>
  get_filler(std::string name, EDMType, std::string default_flag);

}
#endif
