/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "UserHooksUtils.h"
#include "UserSetting.h"
#include "Pythia8_i/UserHooksFactory.h"
#include <iostream>

namespace Pythia8 { class EnhanceSplittings; }
Pythia8_UserHooks::UserHooksFactory::Creator<Pythia8::EnhanceSplittings> EnhanceSplittingsCreator("EnhanceSplittings");


namespace Pythia8 {
//  using namespace std;


  /// @brief Allow enhancement of rare splittings
  ///
  /// @todo Currently hard-coded to accept parameters separately: would be good if we could accept any pattern-matching commands.
  class EnhanceSplittings : public UserHooks {
  public:

    EnhanceSplittings()
      : m_enh_isr_Q2QA("EnhanceSplittings:isr:Q2QA", -1),
        m_enh_fsr_Q2QA("EnhanceSplittings:fsr:Q2QA", -1)
    {

      std::cout << "**********************************************************" << endl;
      std::cout << "*                                                        *" << endl;
      std::cout << "*       Enhancing shower emissions with UserHook!        *" << endl;
      std::cout << "*                                                        *" << endl;
      std::cout << "**********************************************************" << endl;

#ifdef PYTHIA8_304SERIES
      settingsPtr->readString("Enhancements:doEnhance = true");
      settingsPtr->readString("Enhancements:doEnhanceTrial = false");
#endif // PYTHIA8_304SERIES
    }

#ifndef PYTHIA8_304SERIES
    // Enhance real-emission rate. Thus no trial-emission enhancement.
    bool canEnhanceEmission() { return true;}
    bool canEnhanceTrial()    { return false;}
#endif // not PYTHIA8_304SERIES

    // Function to return the weight enhance factor.
    double enhanceFactor(std::string name) {
      /// @todo What we'd actually like to have...
      //const auto itr = m_enhfactors.find(name);
      //if (itr == m_enhfactors.end()) return 1.0;
      //return itr->second;
      if (name == "isr:Q2QA" && m_enh_isr_Q2QA(settingsPtr) > 0) return m_enh_isr_Q2QA(settingsPtr);
      if (name == "fsr:Q2QA" && m_enh_fsr_Q2QA(settingsPtr) > 0) return m_enh_fsr_Q2QA(settingsPtr);
      return 1.0;
    }

    // Function to return the vetoing probability.
    double vetoProbability(std::string name) {
      /// @todo What we'd actually like to have...
      //const auto itr = m_enhfactors.find(name);
      //return (itr == m_enhfactors.end()) ? 0.0 : 0.5;
      if (name == "isr:Q2QA" && m_enh_isr_Q2QA(settingsPtr) > 0) return 0.5;
      if (name == "fsr:Q2QA" && m_enh_fsr_Q2QA(settingsPtr) > 0) return 0.5;
      return 0.0;
    }


  private:

    Pythia8_UserHooks::UserSetting<double> m_enh_isr_Q2QA, m_enh_fsr_Q2QA;

    /// @todo What we'd actually like to have...
    // map<std::string, double> m_enhfactors;

  };


}
