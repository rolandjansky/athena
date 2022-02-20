/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGDECISIONTOOL_TYPELESSFEATURE_H
#define TRIGDECISIONTOOL_TYPELESSFEATURE_H
#include "TrigNavStructure/TriggerElement.h"

class TrigPassBits;
#include <fstream>

namespace Trig{
  class TypelessFeature;

  std::ostream & operator<<(std::ostream &, const Trig::TypelessFeature &);

  /**
   * @brief Feature class, holding information on Feature attached to navigation
   **/
  class TypelessFeature {
  public:
    TypelessFeature(const HLT::TriggerElement::FeatureAccessHelper accessor, const HLT::TriggerElement* te, const std::string& label) : m_accessor(accessor), m_te(te), m_label(label) {;}
    virtual ~TypelessFeature(){;}
    const HLT::TriggerElement::FeatureAccessHelper accessHelper() const {return m_accessor;}
    const HLT::TriggerElement* te() const {return m_te;}
    const std::string label() const {return m_label;}
  private:
    const HLT::TriggerElement::FeatureAccessHelper m_accessor;
    const HLT::TriggerElement*  m_te;
    const std::string m_label;

    friend std::ostream & operator<<(std::ostream &, const Trig::TypelessFeature &);
  };
}// end of Trig namespace
#endif
