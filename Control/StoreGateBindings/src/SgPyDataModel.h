// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATEBINDINGS_SGPYDATAMODEL_H
#define STOREGATEBINDINGS_SGPYDATAMODEL_H 1

#include "Python.h"

#include "StoreGateBindingsDict.h"
#include "SgPyMsg.h"

// Framework includes
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/DataObject.h"
#include "AthenaKernel/IClassIDSvc.h"
#include "AthenaKernel/IDictLoaderSvc.h"
#include "AthenaKernel/DataBucketBase.h"
#include "AthenaKernel/BaseInfo.h"

extern CLID PyCLID;

// ROOT includes
#include "TClass.h"
#include "RootUtils/TClassEditRootUtils.h"
#include "TClassRef.h"
#include "TROOT.h"
#include "TMethod.h"
#include "TMethodCall.h"

// PyROOT includes
#include "AthenaPyRoot.h"

#include <unordered_map>

// fwd declares
namespace SG { struct PyProxyMgr; }
namespace SG { struct PyProxyDict; }

namespace {
  /** @brief replace all occurences of 'from' with 'to' in 's'
   */
  std::string::size_type 
  cxx_replace(std::string& s,
              const std::string& from,
              const std::string& to)
  {
    std::string::size_type cnt(std::string::npos);

    if(from != to && !from.empty()) {
      std::string::size_type pos1(0);
      std::string::size_type pos2(0);
      const std::string::size_type from_len(from.size());
      const std::string::size_type to_len(to.size());
      cnt = 0;

      while((pos1 = s.find(from, pos2)) != std::string::npos) {
        s.replace(pos1, from_len, to);
        pos2 = pos1 + to_len;
        ++cnt;
      }
    }
    return cnt;
  }
}

namespace SG {
  
  /**
   * @brief Concrete @c DataBucket that holds the object via a @c void*
   *        and uses the Root dictionary to do conversions.
   *
   * A concrete @c DataBucket instance holds a pointer to an arbitrary
   * object, and is able to further convert it to pointers to other
   * types related by inheritance.  This variant is used for PyAthena,
   * where don't have the type available at compile time and thus cannot
   * use templates.  However, we know that we do have the Root dictionary
   * available for the types, so we can use that information for the
   * conversions.
   */
  class PyDataBucket : public DataBucketBase
  {
  public:
    /**
     * @brief Constructor.
     * @param pyObj The (pyroot) object proxy to hold.
     * @param clid The class ID of the wrapped object.
     */
    PyDataBucket( PyObject* obj,
                  CLID clid );


    /**
     * @brief Destructor.
     */
    virtual ~PyDataBucket() override { Py_DECREF( m_pyObj ); }
    /**
     * @brief Return the held object.
     */
    virtual void* object() override
    { 
      return m_clid != PyCLID
        ? ObjectProxy_ASVOIDPTR(m_pyObj)
        : m_pyObj; 
    }


    using DataBucketBase::cast;


    /**
     * @brief Return the contents of the @c DataBucket,
     *        converted to type given by @a clid.  Note that only
     *        derived->base conversions are allowed here.
     * @param clid The class ID to which to convert.
     * @param irt To be called if we make a new instance.
     * @param isConst True if the object being converted is regarded as const.
     */
    virtual void* cast (CLID clid, IRegisterTransient* itr = 0,
                        bool isConst = true) override;


    /**
     * @brief Return the contents of the @c DataBucket,
     *        converted to type given by @c std::type_info.  Note that only
     *        derived->base conversions are allowed here.
     * @param tinfo The @c std::type_info of the type to which to convert.
     * @param irt To be called if we make a new instance.
     * @param isConst True if the object being converted is regarded as const.
     */
    virtual void* cast (const std::type_info& tinfo,
                        IRegisterTransient* itr = 0,
                        bool isConst = true) override;

    /**
     * @brief Retrieve reference to class definition structure
     */
    virtual const CLID& clID() const override { return m_clid; }

    /**
     * @brief Return the @c type_info for the stored object.
     */
    virtual const std::type_info& tinfo() const override
    {
      return m_bib->typeinfo();
    }

    /**
     * @brief Give up ownership of the  @c DataBucket contents.
     */
    virtual void relinquish() override
    {
      Py_DECREF( m_pyObj );
    }

    /**
     * If the held object derives from @c ILockable, call @c lock() on it.
     */
    virtual void lock() override;

  private:
    /// Pointer to the held pyroot object (or 'regular' PyObject)
    PyObject* m_pyObj;

    /// The class ID of the wrapped object.
    CLID m_clid;

    /// pointer to the @c SG::BaseInfoBase structure holding the converter
    /// functions for objects held by StoreGate
    const SG::BaseInfoBase* m_bib;

    /// Pointer to the @ IClassIDSvc to be able to cast objects based on
    /// the @a clid.
    static IClassIDSvc* s_clidSvc;

    /// Access (and initialize) the pointer to the @c IClassIDSvc
    static IClassIDSvc* clidSvc();
  };

  /**
   * @brief A helper class to manage accesses to PyProxies
   */
  struct PyProxyMgr
  {
    IDictLoaderSvc * m_dictSvc;
    IClassIDSvc* m_clidSvc;
    /// a dictionary of "typedef'ed typename" -> "typename"
    PyObject* m_aliases;

    /// a dictionary of 'typename' -> CLID (and reverse CLID->'typename')
    PyObject* m_clids;

    typedef std::unordered_map<StoreGateSvc*,SG::PyProxyDict*> PyProxyMap_t;
    PyProxyMap_t m_proxyMap;
    
    /// a dictionary of CLID -> reflex typename
    typedef std::unordered_map<CLID, std::string> ClidMap_t;
    ClidMap_t m_clidMap;

    static
    PyProxyMgr& instance()
    { static PyProxyMgr mgr; return mgr; }

    PyProxyMgr();
    ~PyProxyMgr();

    SG::PyProxyDict* pyproxy( StoreGateSvc* sg );

    /// returns a borrowed reference
    inline
    PyObject* pyclid(PyObject* tp)
    {
      PyObject* clid = PyDict_GetItem(m_clids, tp);
      if ( NULL == clid ) {
        const CLID id = this->clid(tp);
        if ( id == CLID_NULL ) {
          return NULL;
        }
        clid = PyLong_FromLong(id);
        PyDict_SetItem(m_clids, tp, clid);
        // add reverse look-up entry too
        PyDict_SetItem(m_clids, clid, tp);
        //Py_INCREF(clid);
      }
      return clid;
    }

    /// returns a borrowed reference
    inline
    PyObject* pytp(PyObject* clid)
    {
      PyObject* tp = PyDict_GetItem(m_clids, clid);
      if ( NULL == tp ) {
        std::string cpp_tp;
        if (!m_clidSvc->getTypeNameOfID(PyLong_AsUnsignedLong(clid), 
                                        cpp_tp).isSuccess()) {
          return NULL;
        }
        PyObject* alias = PyDict_GetItemString(m_aliases, cpp_tp.c_str());
        if ( alias ) {
          tp = alias;
        } else {
#if PY_VERSION_HEX < 0x03000000
          tp = PyString_FromString(cpp_tp.c_str());
#else
          tp = PyUnicode_FromString(cpp_tp.c_str());
#endif
        }
        PyDict_SetItem(m_clids, clid, tp);
        // reverse look-up
        PyDict_SetItem(m_clids, tp, clid);
        if (!alias) {
          Py_DECREF(tp);
        }
      }
      return tp;
    }

    /// returns a borrowed reference
    inline 
    PyObject* pytp(CLID clid)
    {
      PyObject* pyclid = PyLong_FromLong(clid);
      PyObject* o = this->pytp(pyclid);
      Py_DECREF(pyclid);
      return o;
    }

    inline
    CLID clid(PyObject* tp)
    {
      CLID id = CLID_NULL;
      // FIXME: get rid of this massaging when/if ROOT finally
      // standardize on keeping the std:: namespace...
      std::string tpstr = RootUtils::PyGetString(tp).first;
      std::string tn = TClassEdit::ShortType(tpstr.c_str(),
                                             TClassEdit::kDropAllDefault);
      m_clidSvc->getIDOfTypeName(tn, id).ignore();
      if ( id == CLID_NULL ) {
        // try an alias...
        PyObject* alias = PyDict_GetItemString(m_aliases, tn.c_str());
        if ( alias ){
          std::string aliasstr = RootUtils::PyGetString(alias).first;
          m_clidSvc->getIDOfTypeName(aliasstr, id).ignore();
        }
      }
      if (id == CLID_NULL) {
        // then try a type-id name...
        return this->clid_from_tid (tp);
      }
      return id;
    }

    inline
    CLID clid_from_tid (PyObject* tp)
    {
      CLID id = CLID_NULL;
      // FIXME: get rid of this massaging when/if ROOT finally
      // standardize on keeping the std:: namespace...
      std::string tpstr = RootUtils::PyGetString(tp).first;
      std::string tn = TClassEdit::ShortType(tpstr.c_str(),
                                             TClassEdit::kDropAllDefault);
      m_clidSvc->getIDOfTypeInfoName(tn, id).ignore();
      if ( id == CLID_NULL ) {
        // try an alias...
        PyObject* alias = PyDict_GetItemString(m_aliases, tn.c_str());
        if ( alias ){
          std::string aliasstr = RootUtils::PyGetString(alias).first;
          m_clidSvc->getIDOfTypeInfoName(aliasstr,
                                         id).ignore();
        }
      }
      return id;
    }

    /** ensure everything has been loaded for the clid id (classid,
     *  reflex dict, baseinfobase,...)
     *  @return the reflex typename associated with that clid
     */
    inline
    const char* load_type(CLID id)
    {
      typedef ClidMap_t::iterator Itr_t;
      Itr_t i = m_clidMap.find(id);
      if (i != m_clidMap.end()) {
        return i->second.c_str();
      }
      // Reflex now returns std::basic_string<char> instead of std::string
      std::string tname = m_dictSvc->load_type(id).Name(Reflex::SCOPED);
      // handle '> >'. order MATTERS!
      ::cxx_replace(tname, "basic_string<char> >", "string>");
      ::cxx_replace(tname, "basic_string<char>",   "string");
      m_clidMap[id] = tname;

      // Try to make sure the BIB is initialized.
      std::string bibname = "SG::BaseInfo<" + tname + ">";
      TClass* bibcl = gROOT->GetClass (bibname.c_str());
      if (bibcl) {
        TMethod* m = bibcl->GetMethodAny ("baseinfo");
        if (m) {
          TMethodCall call (m);
          call.Execute();
        }
      }

      return m_clidMap[id].c_str();
    }

  private:
    /// import the dictionary of aliases from a well known location
    PyObject* importDictAliases()
    {
      const std::string moduleName = "AthenaPython.Bindings";
      PyObject* module = PyImport_ImportModule
        ( const_cast<char*>(moduleName.c_str()) );
      if ( !module || !PyModule_Check(module) ) {
        std::cerr << "SG::PyProxyDict WARNING: could not import module '"
                  << moduleName << "' !\n";
        Py_XDECREF(module);
        m_aliases = PyDict_New();
        return m_aliases;
      }

      m_aliases = PyDict_GetItemString(PyModule_GetDict(module),
                                       (char*)"_clid_typename_aliases");
      // borrowed ref. so we increment it
      Py_XINCREF(m_aliases);
      // don't need this one anymore
      Py_DECREF(module);
      
      if ( !m_aliases ) {
        std::cerr << "SG::PyProxyDict WARNING: could not retrieve the "
                  << "dictionary of aliases from '"
                  << moduleName << "' !\n";
        Py_XDECREF(m_aliases);
        m_aliases = PyDict_New();
      }
      return m_aliases;
    }
    
  }; //> class PyProxyMgr

  /** @brief a python front-end to the @c IProxyDict interface
   *  PyProxyDict encapsulates getting python objects from
   *  the raw C++ ones ones gets from the C++ store.
   */
  struct PyProxyDict
  {
    StoreGateSvc* m_sgSvc;

    PyProxyDict( StoreGateSvc* sgSvc ) : 
      m_sgSvc( sgSvc )
    {}

    ~PyProxyDict() 
    {}

    inline
    PyObject* proxy( PyObject* pyclid, PyObject* pykey )
    {
      _SGPY_MSG("PyProxyDict::proxy(...)...");

      PyObject* pyproxy = NULL;

      unsigned int id_tmp = 0;
      if (!PyArg_Parse( pyclid, "I", &id_tmp )) {
        return nullptr;
      }
          
      CLID id = id_tmp;
      std::string skey;
      if (pykey != Py_None) {
        skey = RootUtils::PyGetString (pykey).first;
      }
      SG::DataProxy* proxy = skey.empty()
        ? m_sgSvc->proxy(id)
        : m_sgSvc->proxy(id, skey);
      _SGPY_MSG("PyProxyDict::proxy(" 
                << (proxy ? proxy->clID() : id)   << ", "
                << (proxy ? proxy->name() : skey) << ")...");
      pyproxy = TPython::ObjectProxy_FromVoidPtr((void*)proxy, 
                                                 "SG::DataProxy");

      _SGPY_MSG("PyProxyDict::proxy(...)... [done]");
      return pyproxy;
    }

    inline
    PyObject* newPyDataObject( const char* klass, void* addr=0 )
    {
      PyObject* obj = NULL;
      if ( !(obj = TPython::ObjectProxy_FromVoidPtr((void*)addr, klass)) ) {
        throw PyROOT::TPyException();
      }
      return obj;
    }
  }; //> struct PyProxyDict



  // -------- inlines ----------------------
  inline
  PyProxyDict* 
  PyProxyMgr::pyproxy(StoreGateSvc* sg)
  {
    SG::PyProxyDict* p = 0;
    PyProxyMap_t::iterator i = m_proxyMap.find(sg);
    if ( i == m_proxyMap.end() ) {
      m_proxyMap[sg] = p = new SG::PyProxyDict(sg);
    } else {
      p = i->second;
    }
    return p;
  }
  

} //> end namespace SG

#endif // !STOREGATEBINDINGS_SGPYDATAMODEL_H

