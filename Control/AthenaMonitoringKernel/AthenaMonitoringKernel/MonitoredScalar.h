/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoringKernel_MonitoredScalar_h
#define AthenaMonitoringKernel_MonitoredScalar_h


#include <vector>

#include "AthenaMonitoringKernel/IMonitoredVariable.h"


namespace Monitored {

  /**
   * Declare a monitored scalar
   *
   * Monitoring for any double-convertible scalar
   *
   * @param name            Name of monitored quantity
   * @param defaultValue    Default value assigned to the monitored scalar
   * @param valueTransform  Optional transformation applied to value
   *
   * Examples:
   * - Monitored quantities have "functionality" similar to a number
   *   @snippet Control/AthenaMonitoringKernel/test/GenericMonFilling_test.cxx assignWorked
   *   @snippet Control/AthenaMonitoringKernel/test/GenericMonFilling_test.cxx operatorsWorked_comp
   *   @snippet Control/AthenaMonitoringKernel/test/GenericMonFilling_test.cxx operatorsWorked_plus_eq
   *   @snippet Control/AthenaMonitoringKernel/test/GenericMonFilling_test.cxx operatorsWorked_plus_plus
   *   @snippet Control/AthenaMonitoringKernel/test/GenericMonFilling_test.cxx operatorsWorked_minus_minus
   *   @snippet Control/AthenaMonitoringKernel/test/GenericMonFilling_test.cxx operatorsWorked_mul_eq
   *
   * - Simplest use of the Scalar monitored variable for histograms filling
   *   @snippet Control/AthenaMonitoringKernel/test/GenericMonFilling_test.cxx fillFromScalarWorked
   * - The fact that the group object lifetime determines moment of filling can be used to reuse the same variables to perform several fill operations
   *  @snippet Control/AthenaMonitoringKernel/test/GenericMonFilling_test.cxx fillFromScalarIndependentScopesWorked 
   * - The automatic filling at the end of the scope can be disabled:
   * @snippet Control/AthenaMonitoringKernel/test/GenericMonFilling_test.cxx fillExplicitlyWorked_noop
   * - The explicit filling then can be used. Actually explicit call of the GenericMonitoringTool::fill() causes the same effect:
   * @snippet Control/AthenaMonitoringKernel/test/GenericMonFilling_test.cxx fillExplicitlyWorked_fill
   * See corresponding histogram definitions:
   * @snippet Control/AthenaMonitoringKernel/share/GenericMon.txt 1DHistograms
   * - Histograms can be also filled conditionally i.e. with one variable playing the role of monitored quantity, and other, a mask 
   * @snippet Control/AthenaMonitoringKernel/test/GenericMonFilling_test.cxx fillWithCutMaskWorked
   * See corresponding histogram definitions:
   * @snippet Control/AthenaMonitoringKernel/share/GenericMon.txt Cut
   * 
   * - Monitored quantities can be used to fill 2D histograms.
   * No code change is needed for it however the monitored variables need to be grouped correctly
   *  @snippet Control/AthenaMonitoringKernel/test/GenericMonFilling_test.cxx fill2DWorked_correct
   * @snippet Control/AthenaMonitoringKernel/test/GenericMonFilling_test.cxx fill2DWorked_one_var
   * See corresponding histogram definitions:
   * @snippet Control/AthenaMonitoringKernel/share/GenericMon.txt 2DHistograms
   *
   * - It is possible to monitor objects by providing accessor functions/methods.
   * @snippet Control/AthenaMonitoringKernel/test/GenericMonFilling_test.cxx classes
   * @snippet Control/AthenaMonitoringKernel/test/GenericMonFilling_test.cxx fillFromNonTrivialSourcesWorked_conversion
   * @snippet Control/AthenaMonitoringKernel/test/GenericMonFilling_test.cxx fillFromNonTrivialSourcesWorked_lambda
   *
   * - Strings can be used to fill histograms
   * @snippet Control/AthenaMonitoringKernel/test/GenericMonFilling_test.cxx stringFillingWorked
   * @snippet Control/AthenaMonitoringKernel/test/GenericMonFilling_test.cxx string2DFillingWorked
   * Labelled histograms are defined as follows:
   * @snippet Control/AthenaMonitoringKernel/share/GenericMon.txt LabeledHistograms   
   *
   * @see Monitored::Collection and @see Monitored::Timer
   */
  template <class T> class Scalar : public IMonitoredVariable {
  public:
    static_assert(std::is_convertible<T, double>::value or std::is_constructible<std::string, T>::value, "Conversion of scalar template type to double or string is impossible");

    Scalar(std::string name, const T& defaultValue = {}) :
        IMonitoredVariable(std::move(name)),
        m_value(defaultValue),
        m_valueTransform()
    {}

    Scalar(std::string name, const T& defaultValue, std::function<double(const T&)> valueTransform) :
        IMonitoredVariable(std::move(name)),
        m_value(defaultValue),
        m_valueTransform(valueTransform)
    {}

    Scalar(std::string name, std::function<T()> generator) :
        IMonitoredVariable(std::move(name)),
	m_value(0),
	m_valueTransform(),
	m_valueGenerator( generator )
    {}

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
      return { convertToString( m_value ) };
    }

    virtual bool hasStringRepresentation() const override {
      return std::is_constructible<std::string, T>::value;
    }

    virtual size_t size() const override {
      return 1;
    }
    
  private:
    T m_value;
    std::function<double(const T&)> m_valueTransform;
    std::function<T()> m_valueGenerator;

    template< typename U, typename = typename std::enable_if< std::is_constructible<std::string, U>::value >::type >
    std::string convertToString( const U& value ) const {
      return  std::string{value};
    }

    template< typename U, typename = typename std::enable_if< ! std::is_constructible<std::string, U>::value >::type, typename = void >
    std::string convertToString( const U& ) const {
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
