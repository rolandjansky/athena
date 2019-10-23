/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include "TopEventSelectionTools/JetNGhostSelector.h"

namespace top {
  JetNGhostSelector::JetNGhostSelector(const std::string& params)
    : SignValueSelector("", parseType(params)) {
    switch (m_type) {
    case c: m_name = "JET_N_GHOST c" + SignValueSelector::name();
      break;

    case C: m_name = "JET_N_GHOST C" + SignValueSelector::name();
      break;

    case b: m_name = "JET_N_GHOST b" + SignValueSelector::name();
      break;

    case B: m_name = "JET_N_GHOST B" + SignValueSelector::name();
      break;

    case t: m_name = "JET_N_GHOST t" + SignValueSelector::name();
      break;

    case W: m_name = "JET_N_GHOST W" + SignValueSelector::name();
      break;

    case Z: m_name = "JET_N_GHOST Z" + SignValueSelector::name();
      break;

    case H: m_name = "JET_N_GHOST H" + SignValueSelector::name();
      break;

    case tau: m_name = "JET_N_GHOST tau" + SignValueSelector::name();
      break;

    case Invalid: m_name = "JET_N_GHOST !!! INVALID !!";
      break;
    }
    checkMultiplicityIsInteger();
  }

  std::string JetNGhostSelector::parseType(const std::string& params) {
    // Initialise the type variable
    m_type = Invalid;
    // Skip leading whitespace
    std::size_t pos {
      0
    };
    while (pos < params.length() - 1 && std::isspace(params[pos])) {
      ++pos;
    }

    // list of allowed chars
    static const std::vector<char> allowedChar = {
      'c', 'C', 'b', 'B', 't', 'W', 'Z', 'H'
    };

    // check if the char is allowed
    if (std::find(allowedChar.begin(), allowedChar.end(), params[pos]) == allowedChar.end()) {
      throw std::invalid_argument {
              "Cannot parse the type from the parameter string"
      };
    }

    // process the char
    if (pos + 1 < params.length() && std::isspace(params[ pos + 1 ])) {
      m_type = static_cast<Type>(params[ pos ]);
      return params.substr(pos + 2);
    } else if (params.compare(pos, 4, "tau ") == 0) {
      m_type = tau;
      return params.substr(pos + 4);
    } else {
      throw std::invalid_argument {
              "Cannot parse the type from the parameter string for the common configuration"
      };
    }
  }

  bool JetNGhostSelector::applyParticleLevel(const top::ParticleLevelEvent& plEvent) const {
    // If any of the required collections is a nullptr (i.e. has not been
    // loaded) return false.
    if (!plEvent.m_jets) {
      return false;
    }

    auto func = [&](const xAOD::Jet* truJetPtr) -> bool {
                  switch (m_type) {
                  case c: {
                    return truJetPtr->auxdata<int>("GhostCQuarksFinalCount");
                  }

                  case C: {
                    return truJetPtr->auxdata<int>("GhostCHadronsFinalCount");
                  }

                  case b: {
                    return truJetPtr->auxdata<int>("GhostBQuarksFinalCount");
                  }

                  case B: {
                    return truJetPtr->auxdata<int>("GhostBHadronsFinalCount");
                  }

                  case t: {
                    return truJetPtr->auxdata<int>("GhostTQuarksFinalCount");
                  }

                  case W: {
                    return truJetPtr->auxdata<int>("GhostWBosonsCount");
                  }

                  case Z: {
                    return truJetPtr->auxdata<int>("GhostZBosonsCount");
                  }

                  case H: {
                    return truJetPtr->auxdata<int>("GhostHBosonsCount");
                  }

                  case tau: {
                    return truJetPtr->auxdata<int>("GhostTausFinalCount");
                  }

                  default: /* No-Op */ return 0;
                  }
                };
    auto count = std::count_if(plEvent.m_jets->begin(), plEvent.m_jets->end(), func);
    return checkInt(count, value());
  }

  std::string JetNGhostSelector::name() const {
    return m_name;
  }
}
