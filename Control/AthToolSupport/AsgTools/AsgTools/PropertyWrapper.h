/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef ASG_TOOLS__PROPERTY_WRAPPER_H
#define ASG_TOOLS__PROPERTY_WRAPPER_H


#ifdef XAOD_STANDALONE

#include <iosfwd>
#include <string>
#include <utility>

namespace Gaudi
{
  /// \brief this is the standalone version of the \ref
  /// Gaudi::Property wrapper
  ///
  /// This makes it simpler to declare properties by just wrapping the
  /// property in this template and then using the inline constructor
  /// for it in the class definition:
  /// ```
  /// Gaudi::Property<int> m_property {
  ///   this, "propertyName", startingValue, "property description"};
  /// ```

  template<typename T> class Property final
  {
  public:
    template<typename T2>
    Property (T2 *owner, const std::string& name, const T& value,
              const std::string& title = "");

  public:
    template<typename T2>
    Property (T2 *owner, const std::string& name, T&& value,
              const std::string& title = "");

  public:
    operator const T& () const noexcept;

  private:
    T m_value;
  };



  /// Template Methods
  /// ================

  template<typename T> template<typename T2>
  Property<T> ::
  Property (T2 *owner, const std::string& name, const T& value,
            const std::string& title)
    : m_value (value)
  {
    owner->declareProperty (name, m_value, title);
  }



  template<typename T> template<typename T2>
  Property<T> ::
  Property (T2 *owner, const std::string& name, T&& value,
            const std::string& title)
    : m_value (std::move (value))
  {
    owner->declareProperty (name, m_value, title);
  }



  template<typename T>
  Property<T> ::
  operator const T& () const noexcept
  {
    return m_value;
  }



  template<typename T>
  std::ostream& operator << (std::ostream& str, const Property<T>& property)
  {
    return str << property.operator const T&();
  }
}

#endif

#endif
