/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SharedObject.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUITLS_SHAREDOBJECT_H
#define TRKDETDESCRUITLS_SHAREDOBJECT_H

namespace Trk {

  /** @class SharedObject
  
    A class template to reference count 
    shared objects in the geometry.
    
    @author Andreas.Salzburger@cern.ch
    
    */
    
  template <class T> class SharedObject {
    public: 
     /** Default Constructor */
     SharedObject() :
      m_templatedT(0),
      m_ref(0)
       {} 
      
      /** Templated Constructor, 
          given bool = true means object gets never deleted */
      SharedObject(T* tobj, bool ndel=false) :
        m_templatedT(tobj),
        m_ref( (tobj && !ndel ) ? new int(1) : 0)
        {}
                
      /** Copy Constructor */
      SharedObject(const SharedObject& so) :
        m_templatedT(so.m_templatedT),
        m_ref(so.m_ref)
      { addRef(); }
     
      /** Destructor */
      ~SharedObject()
      {          
       remRef();
       if (m_ref && !(*m_ref) ){
         delete m_templatedT;
         delete m_ref;         
       }
      }
     
      /** Assignement Operator */
      SharedObject& operator=(const SharedObject& so)
      { 
       if ( this != &so){
         remRef();
         if (m_ref && !(*m_ref)){
          delete m_templatedT;
          delete m_ref;
          m_ref = 0;
         }
        m_templatedT = so.m_templatedT;
        m_ref        = so.m_templatedT ? so.m_ref : 0;
        addRef(); 
       }
       return *this;
      }
     
      T& operator*() const { return (*m_templatedT);}  
    
      T* operator->() const { return m_templatedT; }

      T& getRef() const { return (*m_templatedT); }
      
      T* getPtr() const { return m_templatedT; }

      /** count the references */
      int references() const { return (m_ref ? *m_ref : 0 ); }
      
      /** adding a reference */
      void addRef() { if (m_ref) (*m_ref)++; }
      
      /** removing a reference */
      void remRef() { if (m_ref) (*m_ref)--; }
           
    private:
    
      T*            m_templatedT;
      int*          m_ref;
      
  };

} // end of namespace


#endif // TRKDETDESCRUITLS_SHAREDOBJECT_H
