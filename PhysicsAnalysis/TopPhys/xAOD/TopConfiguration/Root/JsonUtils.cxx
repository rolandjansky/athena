/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#include "TopConfiguration/JsonUtils.h"

#include "AsgTools/TProperty.h"

#include <stdexcept>

namespace top {
  std::string json_dump(std::string const& value) {
    std::string out;
    out.reserve(value.size() + value.size() / 8 + 8);
    char const* hexalphabet = "0123456789abcdef";
    out.push_back('"');
    for (auto it = value.cbegin(); it != value.cend(); it++) {
      char c = *it;
      if (0x00 <= c && c < 0x20) {
        out.push_back('\\');
        switch (c) {
        case '\b': out.push_back('b');
          break;

        case '\f': out.push_back('f');
          break;

        case '\n': out.push_back('n');
          break;

        case '\r': out.push_back('r');
          break;

        case '\t': out.push_back('t');
          break;

        default:
          out.push_back('x');
          out.push_back(hexalphabet[c >> 4]);
          out.push_back(hexalphabet[c & 0xf]);
          break;
        }
      } else {
        if (c == '"' || c == '\\') out.push_back('\\');
        out.push_back(c);
      }
    }
    out.push_back('"');
    return out;
  }

  namespace {
/**
 * @brief Convert values of TProperty<T> instances to JSON.
 */
    template<typename T>
    class TPropertyValueJsonDumper: public PropertyValueJsonDumper::ISpecializedDumper {
    public:
      TPropertyValueJsonDumper() :
        m_convprop(createProperty(m_value)) {
      }

      virtual std::string operator () (Property* prop) override {
        if (m_convprop->setFrom(*prop) != 0) throw std::logic_error(
                  "TPropertyValueJsonDumper called with wrong type of argument");
        return json_dump(m_value);
      }

    private:
      T m_value;
      std::unique_ptr<Property> m_convprop;
    };
  }


  PropertyValueJsonDumper::PropertyValueJsonDumper() {
    m_dispatch.emplace_back(Property::BOOL, std::make_unique<TPropertyValueJsonDumper<bool> >());
    m_dispatch.emplace_back(Property::INT, std::make_unique<TPropertyValueJsonDumper<int> >());
    m_dispatch.emplace_back(Property::FLOAT, std::make_unique<TPropertyValueJsonDumper<float> >());
    m_dispatch.emplace_back(Property::DOUBLE, std::make_unique<TPropertyValueJsonDumper<double> >());
    m_dispatch.emplace_back(Property::STRING, std::make_unique<TPropertyValueJsonDumper<std::string> >());
  }

  PropertyValueJsonDumper::~PropertyValueJsonDumper() {
  }

  boost::optional<std::string> PropertyValueJsonDumper::operator () (Property* prop) const {
    Property::Type propType = prop->type();
    for (auto&& kv : m_dispatch) {
      if (kv.first == propType) return (*kv.second)(prop);
    }
    return boost::none;
  }

  PropertyValueJsonDumper::ISpecializedDumper::~ISpecializedDumper() {
  }
}
