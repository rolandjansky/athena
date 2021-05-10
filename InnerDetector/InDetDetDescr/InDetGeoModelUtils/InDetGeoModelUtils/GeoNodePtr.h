/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef _GeoNodePtr_H_
#define _GeoNodePtr_H_

#include <iostream>

// Smart pointer for reference counted GeoModel objects
template <class T>
class GeoNodePtr {
   static T *ref(T *obj)   { if (obj) obj->ref();   return obj;}
   static T *unref(T *obj) { if (obj) obj->unref(); return obj;}
public:
   using T_Ptr=T*;
   using T_ConstPtr=const T * ;

   GeoNodePtr() = default;
   GeoNodePtr(T *obj)
      : m_obj(ref(obj))
   { }

   ~GeoNodePtr()
   { unref(m_obj); }

   // copy
   GeoNodePtr(const GeoNodePtr<T> &node_ptr)
      : m_obj( ref( node_ptr->m_obj() ) )
      { }

   GeoNodePtr<T> &operator=(const GeoNodePtr<T> &node_ptr) {
      if (node_ptr->m_obj != m_obj) {
         unref(m_obj);
         m_obj=ref(node_ptr->m_obj);
      }
      return *this;
   }
   GeoNodePtr<T> &operator=(T *obj) {
      if (obj != m_obj) {
         unref(m_obj);
         m_obj=ref(obj);
      }
      return *this;
   }

   // move
   GeoNodePtr(GeoNodePtr<T> &&node_ptr) : m_obj(node_ptr->m_obj) { node_ptr->m_obj=nullptr; }
   GeoNodePtr<T> &operator=(GeoNodePtr<T> &&node_ptr) {
      m_obj           = node_ptr->m_obj;
      node_ptr->m_obj = nullptr;
      return *this;
   }

   operator bool()  const      { return m_obj != nullptr; }
   operator T_Ptr()            { return m_obj; }
   T_ConstPtr get() const      { return m_obj; }

   T *operator->()             { assert(m_obj); return m_obj; }
   const T *operator->() const { assert(m_obj); return m_obj; }

private:
   T *m_obj = nullptr;
};

#endif
