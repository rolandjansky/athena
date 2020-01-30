/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#ifndef _TOPCONFIGURATION_JSONUTILS_H_
#define _TOPCONFIGURATION_JSONUTILS_H_

#include "AsgTools/Property.h"

#include <boost/optional.hpp>

#include <cmath>
#include <ios>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

class Property;

namespace top {
/** @brief Convert boolean to JSON format. */
  template<typename T, typename std::enable_if<
             std::is_same<typename std::remove_cv<T>::type, bool>::value
             >::type* = nullptr>
  std::string json_dump(T value) {
    return(value ? "true" : "false");
  }

/** @brief Convert number to JSON format. */
  template<typename T, typename std::enable_if<
             std::is_integral<T>::value&& !std::is_same<typename std::remove_cv<T>::type, bool>::value
             >::type* = nullptr>
  std::string json_dump(T value) {
    std::stringstream out;
    out << value;
    return out.str();
  }

/** @brief Convert number to JSON format. */
  template<typename T, typename std::enable_if<
             std::is_floating_point<T>::value
             >::type* = nullptr>
  std::string json_dump(T value) {
    if (std::isfinite(value)) {
      std::stringstream out;
      out << value;
      return out.str();
    }
    return(std::isinf(value) ? (std::signbit(value) ? "-Infinity" : "Infinity") : "NaN");
  }

/** @brief Escape string for use in JSON format. */
  std::string json_dump(std::string const& value);


/**
 * @brief Convert values of Property instances to JSON.
 */
  class PropertyValueJsonDumper {
  public:
    PropertyValueJsonDumper();
    ~PropertyValueJsonDumper();

    /**
     * @brief Convert the value of a property to JSON.
     *
     * @return The JSON representation of the value, or boost::none if no conversion is available.
     */
    boost::optional<std::string> operator () (Property* prop) const;

    class ISpecializedDumper {
    public:
      virtual ~ISpecializedDumper();
      virtual std::string operator () (Property* prop) = 0;
    };
  protected:
    std::vector<std::pair<Property::Type, std::unique_ptr<ISpecializedDumper> > > m_dispatch;
  };
}


#endif
