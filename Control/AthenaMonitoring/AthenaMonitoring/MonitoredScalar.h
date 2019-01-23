/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoring_MonitoredScalar_h
#define AthenaMonitoring_MonitoredScalar_h

#include <vector>

#include "AthenaMonitoring/IMonitoredVariable.h"

namespace Monitored {

  namespace impl {
    /**
     * Declare a monitored scalar
     *
     * Monitoring for any double-convertable scalar
     *
     * @param name         Name of monitored quantity
     * @param defaultValue default value assigned to the monitored scalar
     *
     * \code
     *   auto phi = Monitored::Scalar("phi", 4.2);       // deduced double
     *   auto eta = Monitored::Scalar<float>("eta", 0);  // explicit float
     * \endcode
     */
    template <class T>
    class Scalar : public IMonitoredVariable {
    public:
      static_assert(std::is_convertible<T, double>::value, "Value must be convertable to double");

      Scalar(std::string name, const T& defaultValue = {})
          : IMonitoredVariable(std::move(name)), m_value(defaultValue) {}
      Scalar(Scalar&&) = default;
      Scalar(Scalar const&) = delete;

      Scalar& operator=(Scalar const&) = delete;
      T operator=(T value) {
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

      const std::vector<double> getVectorRepresentation() const override {
        return {double(m_value)};
      }

    private:
      T m_value;
    };
  } // namespace impl

  // TEMPORARY: for backwards compatibility
  namespace MonitoredScalar {
    template <typename T>
    [[deprecated("Use Monitored::Scalar instead (see MR athena!20403)")]]
    static impl::Scalar<T> declare(std::string name, const T& defaultValue = {}) {
      return impl::Scalar<T>(name, defaultValue);
    }
  } // namespace MonitoredTimer

#if __cplusplus >= 201700
  using impl::Scalar;
#else
  template <typename T>
  static impl::Scalar<T> Scalar(std::string name, const T& defaultValue = {}) {
    return impl::Scalar<T>(name, defaultValue);
  }
#endif

} // namespace Monitored

#endif /* AthenaMonitoring_MonitoredScalar_h */
