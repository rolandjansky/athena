/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFDATA_CONSTITER_H
#define TRIGCONFDATA_CONSTITER_H

/**
 * @file TrigConfData/ConstIter.h
 * @author J. Stelzer
 * @date Feb 2019
 * @brief Forward iterator to traverse the main components of the trigger configuration
 */

#include <functional>

namespace TrigConf {

   /**
    * @brief Forward iterator over an iterable of type @c V returning an object of type @c T
    *
    * By its nature this is a generator, returning values of type @c T, computed on the fly
    *     <https://en.wikipedia.org/wiki/Generator_(computer_programming)>
    *
    * Objects of type @c T must be creatable from @c V::value_type, either by having 
    * a constuctor T(V::valuetype) or by supplying an explicit conversion function to
    * the constructor of the iterator
    *
    * This is being used by HLTMenu, @c L1Menu, and L1TopoMenu
    */
   template<typename V, typename T>
   class ConstIter final {
   public:
      /**
       * @brief Constructor
       * @param buf Reference to the underlying iterable data container
       * @param offset Position of the current element at construction time
       * @param f Function to create an object of type @c T from type @c V::value_type, the default calls a contstructor T(V::value_type)
       *
       * Iterators pointing to the front and back of the container, assuming that T can be created from the elements of V are created like this
       *@code
       ConstIter<V, T> begin( v );
       ConstIter<V, T> end( v, v.size() );       
       @endcode
       *
       * An example for iterating over chains of an HLTMenu
       *@code
       ptree & chains = hltmenu.data().get_child("chains");
       ConstIter<ptree, TrigConf::HLTChain> begin( v, 0,  [](auto x){return HLTChain(x.second));
       ConstIter<ptree, TrigConf::HLTChain> end( chains, chains.size(), [](auto x){return HLTChain(x.second) );   
       *@endcode
       */
      ConstIter( const V & buf, size_t offset = 0, 
                 std::function<T(const typename V::value_type &)> f = [](auto x)->T{return {x};}) :
         m_buf(buf),
         m_offset(offset),
         m_data(),
         m_f(f),
         m_bufIt(buf.begin())
      {}

      /** Destructor */
      ~ConstIter()
      {}

      /** @brief Comparison operator */
      bool operator==(const ConstIter &i) {
         return &i.m_buf == &m_buf && i.m_offset == m_offset;
      }

      /** Not-equal operator */
      bool operator!=(const ConstIter &i) {
         return !(*this == i);
      }

      /** Pre-increment operator */
      ConstIter & operator++() {
         ++m_offset;
         ++m_bufIt;
         return *this;
      }

      /** Dereference operator
       * Creates object of type @c T from the current object in the container on the fly using the creation function @c m_f
       */
      const T & operator*() const { 
         m_data = m_f(*m_bufIt);
         return m_data;
      }

   private:
      const V & m_buf; //!< Const reference to the underlying data container
      std::size_t m_offset; //!< Current position of the iterator 
      mutable T m_data; //!< Holder of the transformed data
      std::function<T(const typename V::value_type &)> m_f; //!< Function to turn a single datum from the container into the output type 
      typename V::const_iterator m_bufIt; //!< Iterator over the container

   };

}

/**
 * @brief Iterator traits for the above iterator
 * 
 * note that the @c std::iterator will be deprecated in C++17
 */
namespace std {
    template<typename V, typename T>
    class iterator_traits<TrigConf::ConstIter<V,T> >
    {
    public:
        using difference_type = std::ptrdiff_t;
        using size_type = std::size_t;
        using value_type = const T;
        using pointer = const T*;
        using reference = const T&;
        using iterator_category = std::random_access_iterator_tag;
    };
}

#endif
