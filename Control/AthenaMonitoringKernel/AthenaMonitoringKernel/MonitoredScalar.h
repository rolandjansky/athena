/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoringKernel_MonitoredScalar_h
#define AthenaMonitoringKernel_MonitoredScalar_h


#include <vector>

#include "AthenaMonitoringKernel/IMonitoredVariable.h"


namespace Monitored {

  /**
   * Declare a monitored scalar variable.
   *
   * A monitored Scalar behaves similar to a regular builtin type:
   *
   * @tparam T  Type of scalar (convertable to double or string)
   *
   * ###Examples:
   *   @snippet Control/AthenaMonitoringKernel/test/GenericMonFilling_test.cxx assign
   *   @snippet Control/AthenaMonitoringKernel/test/GenericMonFilling_test.cxx operators_comp
   *   @snippet Control/AthenaMonitoringKernel/test/GenericMonFilling_test.cxx operators_examples
   *
   * In case of std::string an alphanumeric histogram fill will be performed:
   *   @snippet Control/AthenaMonitoringKernel/test/GenericMonFilling_test.cxx stringFilling
   *
   * @see Monitored::Collection
   * @see Monitored::Timer
   */
  template <class T> class Scalar : public IMonitoredVariable {
  public:
    static_assert(std::is_convertible<T, double>::value or std::is_constructible<std::string, T>::value, "Conversion of scalar template type to double or string is impossible");

    /** @name Client API */
    ///@{

    /**
     * %Scalar with optional default value.
     *
     * @param name            Name of monitored quantity
     * @param defaultValue    Optional default value
     *
     * #### Example
     * @snippet Control/AthenaMonitoringKernel/test/GenericMonFilling_test.cxx fillFromScalar
     */
    Scalar(std::string name, const T& defaultValue = {}) :
        IMonitoredVariable(std::move(name)),
        m_value(defaultValue)
    {}

    /**
     * %Scalar with default value and optional transformation applied before filling.
     *
     * @param name            Name of monitored quantity
     * @param defaultValue    Default value assigned to the monitored scalar
     * @param valueTransform  Optional transformation applied to value before filling
     *
     * #### Example
     * @snippet Control/AthenaMonitoringKernel/test/GenericMonFilling_test.cxx fillFromScalarTrf
     */
    Scalar(std::string name, const T& defaultValue, std::function<double(const T&)> valueTransform) :
        IMonitoredVariable(std::move(name)),
        m_value(defaultValue),
        m_valueTransform(valueTransform)
    {}

    /**
     * %Scalar with generator function to retrieve the value.
     *
     * @param name            Name of monitored quantity
     * @param generator       Function returning the monitored value
     *
     * #### Example
     * @snippet Control/AthenaMonitoringKernel/test/GenericMonFilling_test.cxx fillFromNonTrivialSources_lambda
     */
    Scalar(std::string name, std::function<T()> generator) :
      IMonitoredVariable(std::move(name)),
      m_valueGenerator( generator )
    {}

    ///@}

    Scalar(Scalar&&) = default;
    Scalar(Scalar const&) = delete;

    Scalar& operator=(Scalar const&) = delete;
    T operator=(T value)
    {
      m_value = value;
      return value;
    }

    operator T() const { return m_value; }
    operator T&() { return m_value; }

    // Needed to work around an apparent bug in clang 4.0.1.
    // Without these declarations, clang rejects `--SCALAR'
    // (but ++SCALAR, SCALAR++, and SCALAR-- are all accepted!).
    T operator--() { return --m_value; }
    T operator--(int) { return m_value--; }

    virtual double get(size_t) const override {
      if constexpr (std::is_convertible_v<double, T>) {
        return (m_valueGenerator ? static_cast<double>(m_valueGenerator()) :
                m_valueTransform ? m_valueTransform(m_value) : static_cast<double>(m_value));
      }
      else return 0;
    }

    virtual std::string getString(size_t) const override {
      if constexpr (std::is_constructible_v<std::string, T>) {
        return (m_valueGenerator ? m_valueGenerator() : m_value);
      }
      else return {};
    }

    virtual bool hasStringRepresentation() const override {
      return std::is_constructible<std::string, T>::value;
    }

    virtual size_t size() const override {
      return 1;
    }
    
  private:
    T m_value{};
    std::function<double(const T&)> m_valueTransform;
    std::function<T()> m_valueGenerator;
  };

} // namespace Monitored

#endif /* AthenaMonitoringKernel_MonitoredScalar_h */
