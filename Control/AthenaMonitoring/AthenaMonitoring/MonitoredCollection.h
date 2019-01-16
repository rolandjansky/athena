/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoring_MonitoredCollection_h
#define AthenaMonitoring_MonitoredCollection_h

#include <functional>
#include <type_traits>
#include <vector>

#include "AthenaMonitoring/IMonitoredVariable.h"

namespace Monitored {

  /**
   * Monitoring of (double-convertable) collections
   */

  // Forward declares
  template <class T> class ValuesCollection;
  template <class T> class ObjectsCollection;

  /**
   * Declare a monitored (double-convertable) collection
   *
   * Any iterable container with elements covertable to double can be monitored.
   *
   * @param name        Name of monitored quantity
   * @param collection  Collection to be monitored (e.g. STL container or array)
   *
   * \code
   *   std::vector<float> eta( {0.2, 0.1} );
   *   auto m = Monitored::Collection("Eta", eta);
   * \endcode
   */
  template <class T> ValuesCollection<T> Collection(std::string name, const T& collection) {
    return ValuesCollection<T>(std::move(name), collection);
  }

  /**
   * Declare a monitored collection of objects
   *
   * A converter function/accessor needs to be provided to extract the relevant quantity.
   *
   * @param name        Name of monitored quantity
   * @param collection  Collection to be monitored (e.g. STL container or array)
   * @param converterToDouble  Function taking an element and returning a double
   *
   * \code
   *   std::vector<Track> tracks;
   *   auto phi = Monitored::Collection( "Phi", tracks, []( const Track& t ) { return
   * t.phi(); } ); \endcode
   */
  template <class T> ObjectsCollection<T>
  Collection(std::string name, const T& collection,
             std::function<double(const typename ObjectsCollection<T>::const_value_type&)>
                 converterToDouble) {
    return ObjectsCollection<T>(std::move(name), collection, std::move(converterToDouble));
  }

  // TEMPORARY: for backwards compatibility
  namespace MonitoredCollection {

    template <class T> ValuesCollection<T> declare(std::string name, const T& collection) {
      return ValuesCollection<T>(std::move(name), collection);
    }

    template <class T> ObjectsCollection<T>
    declare(std::string name, const T& collection,
            std::function<double(const typename ObjectsCollection<T>::const_value_type&)>
                converterToDouble) {
      return ObjectsCollection<T>(std::move(name), collection, std::move(converterToDouble));
    }
  } // namespace MonitoredCollection

  namespace detail {
    /// Get element type for containers
    template <typename T> struct get_value_type { typedef typename T::value_type value_type; };
    /// Get element type for arrays
    template <typename T, size_t N> struct get_value_type<T[N]> { typedef T value_type; };

    template <typename T> struct make_pointer_const { typedef T type; };
    template <typename T> struct make_pointer_const<T*> { typedef const T* type; };
  } // namespace detail

  /**
   * Monitoring of collections
   *
   * This class is not supposed to be used by the end user.
   */
  template <class T> class ValuesCollection : public IMonitoredVariable {
  public:
    /// Type of the collection elements
    using value_type = typename detail::get_value_type<T>::value_type;

    static_assert(std::is_convertible<value_type, double>::value,
                  "Collection values must be convertable to double");

    /// @brief .     \if empty doc string required due to doxygen bug 787131 \endif
    friend ValuesCollection<T> Collection<T>(std::string name, const T& collection);
    // TEMPORARY: for backwards compatibility
    friend ValuesCollection<T> MonitoredCollection::declare<T>(std::string name,
                                                               const T& collection);

    ValuesCollection(ValuesCollection&&) = default;

    const std::vector<double> getVectorRepresentation() const override {
      return std::vector<double>(std::begin(m_collection), std::end(m_collection));
    }

  private:
    const T& m_collection;

    ValuesCollection(std::string name, const T& collection)
        : IMonitoredVariable(std::move(name)), m_collection(collection) {}
    ValuesCollection(ValuesCollection const&) = delete;
    ValuesCollection& operator=(ValuesCollection const&) = delete;
  };

  /**
   * Monitoring of object collections
   *
   * This class is not supposed to be used by the end user.
   */
  template <class T> class ObjectsCollection : public IMonitoredVariable {
  public:
    /// Type of the collection elements
    using value_type = typename detail::get_value_type<T>::value_type;
    using const_value_type = typename detail::make_pointer_const<value_type>::type;

    /// @brief .     \if empty doc string required due to doxygen bug 787131 \endif
    // With a non-template friend declaration, clang 4.0.1
    // fails to match the friend.
    template <class U> friend ObjectsCollection<U>
    Collection(std::string name, const U& collection,
               std::function<double(const typename ObjectsCollection<U>::const_value_type&)>
                   converterToDouble);

    // TEMPORARY: for backwards compatibility
    template <class U> friend ObjectsCollection<U> MonitoredCollection::declare(
        std::string name, const U& collection,
        std::function<double(const typename ObjectsCollection<U>::const_value_type&)>
            converterToDouble);

    ObjectsCollection(ObjectsCollection&&) = default;

    const std::vector<double> getVectorRepresentation() const override {
      // Could be replaced with std::size in C++17
      auto N = std::distance(std::begin(m_collection), std::end(m_collection));

      // Reserve space and fill vector
      std::vector<double> result;
      result.reserve(N);
      for (auto value : m_collection) result.push_back(m_converterToDouble(value));

      return result;
    }

  private:
    const T& m_collection;
    std::function<double(const const_value_type&)> m_converterToDouble;

    ObjectsCollection(std::string name, const T& collection,
                      std::function<double(const const_value_type&)> converterToDouble)
        : IMonitoredVariable(std::move(name)),
          m_collection(collection),
          m_converterToDouble(std::move(converterToDouble)) {}
    ObjectsCollection(ObjectsCollection const&) = delete;
    ObjectsCollection& operator=(ObjectsCollection const&) = delete;
  };

} // namespace Monitored

#endif /* AthenaMonitoring_MonitoredCollection_h */
