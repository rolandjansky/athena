/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoringKernel_MonitoredCollection_h
#define AthenaMonitoringKernel_MonitoredCollection_h

#include <functional>
#include <type_traits>
#include <vector>

#include "AthenaMonitoringKernel/IMonitoredVariable.h"

namespace Monitored {

  // Forward declares
  template <class T> class ValuesCollection;
  template <class T, class R> class ObjectsCollection;

  /**
   * Declare a monitored (double-convertible) collection.
   *
   * @tparam T          Type of collection
   * @param name        Name of monitored quantity
   * @param collection  Collection to be monitored (e.g. STL container or array)
   *
   * #### Examples:
   * Various types of collections can be monitored. STL containers:
   *   @snippet Control/AthenaMonitoringKernel/test/GenericMonFilling_test.cxx fillFromNonTrivialSources_collection
   * or more generally, any iterable container:
   *   @snippet Control/AthenaMonitoringKernel/test/GenericMonFilling_test.cxx fillFromNonTrivialSources_array
   *
   * @see Monitored::Scalar
   * @see Monitored::Group
   */
  template <class T> ValuesCollection<T> Collection(std::string name, const T& collection) {
    return ValuesCollection<T>(std::move(name), collection);
  }

  // Disallow temporaries
  template <class T> ValuesCollection<T> Collection(std::string name, T const&& collection) = delete;

  /**
   * Declare a monitored collection of objects via accessor function.
   *
   * @tparam T          Type of collection
   * @param name        Name of monitored quantity
   * @param collection  Collection to be monitored (e.g. STL container or array)
   * @param accessor    Function taking an element and returning a double
   *
   * #### Examples:
   * Collection of objects can be monitored provided via an accessor function:
   *   @snippet Control/AthenaMonitoringKernel/test/GenericMonFilling_test.cxx fillFromNonTrivialSources_obj_collection
   *
   * A collection of strings can be used to fill labelled histograms:
   *   @snippet Control/AthenaMonitoringKernel/test/GenericMonFilling_test.cxx string2DFilling
   * Use of accessor functions work the same way as for Monitored::Scalar:
   *   @snippet Control/AthenaMonitoringKernel/test/GenericMonFilling_test.cxx stringFromCollection
   *
   * @see Monitored::Scalar
   * @see Monitored::Group
   */
  template <class T>
  ObjectsCollection<T, double>
  Collection(std::string name, const T& collection,
             std::function<double(const typename ObjectsCollection<T, double>::const_value_type&)> accessor) {
    return ObjectsCollection<T, double>(std::move(name), collection, std::move(accessor));
  }

  template <class T>
  ObjectsCollection<T, double>
  Collection(std::string name, const T&& collection,
             std::function<double(const typename ObjectsCollection<T, double>::const_value_type&)> accessor) = delete;


  template <class T>
  ObjectsCollection<T, std::string>
  Collection(std::string name, const T& collection,
	     std::function<std::string(const typename ObjectsCollection<T, std::string>::const_value_type&)> accessor) {
    return ObjectsCollection<T, std::string>(std::move(name), collection, std::move(accessor));
  }

  template <class T>
  ObjectsCollection<T, std::string>
  Collection(std::string name, const T&& collection,
             std::function<std::string(const typename ObjectsCollection<T, std::string>::const_value_type&)> accessor) = delete;


  namespace detail {
    /// Get element type for containers
    template <typename T> struct get_value_type { typedef typename T::value_type value_type; };
    /// Get element type for arrays
    template <typename T, size_t N> struct get_value_type<T[N]> { typedef T value_type; };

    template <typename T> struct make_pointer_const { typedef T type; };
    template <typename T> struct make_pointer_const<T*> { typedef const T* type; };
  } // namespace detail


  /**
   * Internal class not to be used by end user.
   */
  template <class T> class ValuesCollection : public IMonitoredVariable {
  public:
    /// Type of the collection elements
    using value_type = typename detail::get_value_type<T>::value_type;

    static_assert(std::is_convertible<value_type, double>::value or std::is_constructible<std::string, value_type>::value, "Conversion of collection values to double or string is impossible");

    // @brief .     \if empty doc string required due to https://github.com/doxygen/doxygen/issues/6251 \endif
    friend ValuesCollection<T> Collection<T>(std::string name, const T& collection);
    friend ValuesCollection<T> Collection<T>(std::string name, const T&& collection);

    ValuesCollection(ValuesCollection&&) = default;

    virtual double get([[maybe_unused]] size_t i) const override {
      if constexpr (std::is_convertible_v<value_type, double>)
        return m_collection[i];
      else
        return -1;
    }

    virtual std::string getString([[maybe_unused]] size_t i) const override {
      if constexpr (std::is_constructible_v<std::string, value_type>)
        return m_collection[i];
      else
        return {};
    }

    virtual bool hasStringRepresentation() const override {
      return std::is_constructible<std::string, value_type>::value;
    }

    virtual size_t size() const override {
      return std::size(m_collection);
    }

  private:
    const T& m_collection;


    ValuesCollection(std::string vname, const T& collection)
      : IMonitoredVariable(std::move(vname)), m_collection{collection} {
    }

    ValuesCollection(ValuesCollection const&) = delete;
    ValuesCollection& operator=(ValuesCollection const&) = delete;
  };

  /**
   * Monitoring of object collections (internal)
   *
   * Template types are: T - collection, R - result type of the converter function (convertible to double or string)
   * This class is not supposed to be used by the end user.
   */
  template <class T, class R> class ObjectsCollection : public IMonitoredVariable {
  public:
    /// Type of the collection elements
    using value_type = typename detail::get_value_type<T>::value_type;
    using const_value_type = typename detail::make_pointer_const<value_type>::type;

    static_assert(std::is_convertible<R, double>::value or std::is_constructible<std::string, R>::value, "Conversion from type returned by the converter/accessor to double or string is impossible");

    // With a non-template friend declaration, clang 4.0.1
    // fails to match the friend.
    // @brief .     \if empty doc string required due to https://github.com/doxygen/doxygen/issues/6251 \endif
    template <class U> friend ObjectsCollection<U, double>
    Collection(std::string name, const U& collection,
               std::function<double(const typename ObjectsCollection<U, double>::const_value_type&)> accessor);

    template <class U> friend ObjectsCollection<U, std::string>
    Collection(std::string name, const U& collection,
               std::function<std::string(const typename ObjectsCollection<U, std::string>::const_value_type&)> converterToString);

    ObjectsCollection(ObjectsCollection&&) = default;

    virtual double get([[maybe_unused]] size_t i) const override {
      if constexpr (std::is_convertible_v<R, double>)
        return m_converterToR(m_collection[i]);
      else
        return -1;
    }

    virtual std::string getString([[maybe_unused]] size_t i) const override {
      if constexpr (std::is_constructible_v<std::string, R>)
        return m_converterToR(m_collection[i]);
      else
        return {};
    }

    virtual bool hasStringRepresentation() const override {
      return std::is_constructible<std::string, R>::value;
    };
    virtual size_t size() const override {
      return std::size(m_collection);
    }

  private:
    const T& m_collection;
    std::function<R(const const_value_type&)> m_converterToR;

    ObjectsCollection(std::string name, const T& collection,
                      std::function<R(const const_value_type&)> converterToR)
        : IMonitoredVariable(std::move(name)),
	  m_collection(std::move(collection)),
          m_converterToR(std::move(converterToR)) {}

    ObjectsCollection(ObjectsCollection const&) = delete;

    ObjectsCollection& operator=(ObjectsCollection const&) = delete;

  };


} // namespace Monitored

#endif /* AthenaMonitoringKernel_MonitoredCollection_h */
