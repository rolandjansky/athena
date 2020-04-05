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

    std::vector<double> getVectorRepresentation() const override {
      return { convertToDouble( m_value, m_valueTransform, m_valueGenerator ) };
    }

    std::vector<std::string> getStringVectorRepresentation() const override{
      return { convertToString( m_value, m_valueGenerator ) };
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

    template< typename U, typename Generator,
              typename = typename std::enable_if< std::is_constructible<std::string, U>::value >::type >
    std::string convertToString( const U& value, Generator g ) const {
      return  ( g ? g() : value );
    }

    template< typename U, typename Generator,
              typename = typename std::enable_if< ! std::is_constructible<std::string, U>::value >::type, typename = void >
    std::string convertToString( const U&, Generator ) const {
      return "";
    }

    template< typename U, typename Transformer, typename Generator,  typename = typename std::enable_if< !std::is_convertible<double, U>::value >::type >
      double convertToDouble( const U&, Transformer, Generator  ) const {
      return 0;
    }

    template< typename U, typename Transformer, typename Generator, typename = typename std::enable_if< std::is_convertible<double, U>::value >::type, typename = void >
      double convertToDouble(const U& value, Transformer t, Generator g ) const {      
      return  ( g ? static_cast<double>(g()) : t ? t(value) : static_cast<double>(value) );
    }


  };





} // namespace Monitored

#endif /* AthenaMonitoringKernel_MonitoredScalar_h */
