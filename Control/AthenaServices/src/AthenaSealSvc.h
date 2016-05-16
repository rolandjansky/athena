// this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENASERVICES_ATHENASEALSVC_H
#define ATHENASERVICES_ATHENASEALSVC_H 1

/**
 * @file AthenaSealSvc.h
 *
 * @brief AthenaSealSvc is a Gaudi service for loading and checking
 * reflex dictionaries
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: AthenaSealSvc.h,v 1.2 2008-12-10 13:37:27 dquarrie Exp $
 */


#include "AthenaBaseComps/AthService.h"
#include "AthenaKernel/IAthenaSealSvc.h"
#include "AthenaKernel/IDictLoaderSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include <vector>
#include <set>
#include <string>

#include "RVersion.h"
#if ROOT_VERSION_CODE < ROOT_VERSION(5,99,0)
#define ROOT6_MIGRATION_HAVE_REFLEX
namespace Reflex {
   class Type;
   class Member;
}
typedef Reflex::Base          ReflexBase;
typedef Reflex::Member        ReflexMember;
typedef Reflex::PropertyList  ReflexPropertyList;
typedef Reflex::Scope         ReflexScope;
typedef Reflex::Type          ReflexType;
typedef Reflex::Type_Iterator ReflexType_Iterator;
#else /* ROOT6 */
#include "DataModelRoot/RootType.h"
typedef RootBase         ReflexBase;
typedef RootDataMember   ReflexMember;
typedef RootPropertyList ReflexPropertyList;
typedef RootScope        ReflexScope;
typedef RootType         ReflexType;
#endif


template <class TYPE> class SvcFactory;

class MsgStream;
class IChronoStatSvc;

/**
 **  @class AthenaSealSvc
 **  
 **  @brief AthenaSealSvc is a Gaudi service for loading and checking
 **  Reflex dictionaries
 **  
 **  This Athena service check "completeness" of class definitions in
 **  Reflex dictionaries.
 **  
 **  The "completeness" check for classes makes sure that the types of all
 **  attributes of a class have their corresponding types defined.  The
 **  checks are automatically done for the classes being written or read
 **  in. The checking is controled by a jobOption  flag, which by default
 **  is True:
 **  
 **  <pre>
 **  # Check the dictionary in memory for completeness (set to True of False)
 **  AthenaSealSvc.CheckDictionary = True;
 **  </pre>
 **  
 **  A list of class names to be ignored during the completeness check may
 **  be added to AthenaSealSvcIgnore_joboptions.py.
 ** 
 **/

class AthenaSealSvc 
  : virtual public IAthenaSealSvc,
            public AthService
{
  

public:

    AthenaSealSvc (const std::string& name, ISvcLocator* pSvcLocator);
    ~AthenaSealSvc();
  
    /// Gaudi Service interface method implementatios:
    StatusCode initialize();
    StatusCode finalize();
    StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );

    /** Check dictionary for a class 
      - dictionary library containing class will automatically be loaded if needed
      @param typeName - type/name of class to check
    */
    virtual StatusCode checkClass(const std::string& typeName) const;


    /** Dynamically load a dictionary filler
	@param dictName - name of dict filler lib, e.g. MyClassesDict
    */
    virtual StatusCode loadDictFiller(const std::string& dictName) const;


  
private:

    // initialise seal components
    void initSeal();   // MN: missing ??

    // check dictionary
    void checkDictionary() const;
    bool missingTypes(const ReflexType& type) const;
    bool ignoreName(const ReflexType& type) const;
    bool ignoreName(const std::string& name) const;

    StatusCode                  loadClasses() const;
    const ReflexType loadClass(const std::string& name ) const;
    const ReflexType loadClassAndDependents(const std::string& name ) const;

    void show_member (const ReflexMember& m) const;
    void show_type   (const ReflexType& t) const;
    bool member_is_ok(const std::string& className, 
		      const std::string& memberName) const;
    void find_extra_types (const ReflexType& t,
			   std::vector<ReflexType>& newTypes) const;
    bool incorrect_guid (const ReflexType& t) const;
    bool missing_member_types (const ReflexType& t) const;

    /// Helper method to setup the default dicts list
    void setDefaultDictNames();

    /// Helper method to setup the default ignore-name list
    void setDefaultIgnoreNames();

    /// Helper method to setup the default ignore-dict list
    void setDefaultIgnoreDicts();

    AthenaSealSvc(const AthenaSealSvc& rhs);
    AthenaSealSvc& operator=(const AthenaSealSvc& rhs);
  
    StringArrayProperty	          m_dictionaryNames;
    StringArrayProperty	          m_ignoreNames;
    StringArrayProperty	          m_ignoreDicts;
    BooleanProperty 	          m_checkDictAtInit;
    BooleanProperty 	          m_checkDictionary;
    BooleanProperty 	          m_loadAllDicts;
    mutable std::set<std::string> m_checkedClasses;
    typedef ServiceHandle<IChronoStatSvc> IChronoStatSvc_t;
    IChronoStatSvc_t              m_chronoStatSvc;
    typedef ServiceHandle<IDictLoaderSvc> IDictLoaderSvc_t;
    /** @brief a handle to the autoloader of reflex dictionaries
     */
    IDictLoaderSvc_t              m_dictLoaderSvc;
    /// bool to activate the chrono stats, depending on the 
    /// m_skipFirstChronoCommit data member
    bool                          m_doChronoStat;
    mutable std::string           m_currentClassName;
};

#endif // !ATHENASERVICES_ATHENASEALSVC_H




