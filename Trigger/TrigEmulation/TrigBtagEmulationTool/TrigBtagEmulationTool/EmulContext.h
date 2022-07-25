/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EMUL_CONTEXT_H
#define EMUL_CONTEXT_H

#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <typeinfo>

namespace Trig {
  
class EmulContext {
 private:
  class IHolder {
  public:
    virtual ~IHolder() = default;
    virtual const std::type_info &type() const = 0;
  };

  template <typename T,
            typename Enable =
    std::enable_if_t<std::is_nothrow_move_constructible<T>::value>>
  class HolderT
    : public IHolder {
  public:
  HolderT(std::unique_ptr<T>&& value);

  const std::type_info &type() const;
  const T* value() const;

  private:
  std::unique_ptr<T> m_value;
  };

  // =========== //

 public:
  EmulContext() = default;
  EmulContext(const EmulContext&) = delete;
  EmulContext& operator=(const EmulContext&) = delete;
  ~EmulContext() = default;
  
  template <typename T>
    void store(const std::string &name, std::unique_ptr<T> &&object);
  
  template <typename T> 
    const T *get(const std::string &name) const;
  
 private:
  std::unordered_map<std::string, std::unique_ptr<IHolder>> m_store;
};

/* ============================================================== */
 
 template <typename T, typename Enable>
   EmulContext::HolderT<T, Enable>::HolderT(std::unique_ptr<T>&& value)
   : m_value( std::move(value) )
  {}

 template <typename T, typename Enable>
   const std::type_info& EmulContext::HolderT<T, Enable>::type() const {
   return typeid(T);
 }
 
 template <typename T, typename Enable>
   const T* EmulContext::HolderT<T, Enable>::value() const {
   return m_value.get();
 }

 // =========== // 

 template <typename T>
   inline void EmulContext::store(const std::string &name,
				  std::unique_ptr<T> &&object) 
   {
     if (name.empty())
       throw std::invalid_argument("Object can not have an empty name in order to be stored in memory");
     
     if (m_store.find(name) != m_store.end())
       throw std::runtime_error("Object with name `" + name +
				"` already stored in memory");

     using stored_object_t =  std::unique_ptr<T>;
     m_store.emplace( name,
     		      std::make_unique< HolderT<T> >( std::forward<stored_object_t>( object ) )
     		      );
 }

template <typename T>
  inline const T* EmulContext::get(const std::string &name) const 
  {
    auto itr = m_store.find(name);
    if ( itr == m_store.end() )
      throw std::invalid_argument("Object with name `" + name +
				  "` is not in memory. Cannot be retrieved: " + name);
    
    const IHolder *holder = (*itr).second.get();
    if (typeid(T) != holder->type())
      throw std::invalid_argument("Type mismatch for object '" + name + "'");
    
    return reinterpret_cast<const HolderT<T> *>(holder)->value();
  } 
 
} // namespace

#endif
