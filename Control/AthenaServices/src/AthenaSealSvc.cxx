/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/********************************************************************

NAME:     AthenaSealSvc.cxx
PACKAGE:  AthenaSealSvc

AUTHORS:  Christian Arnault
CREATED:  May. 13, 2003

UPDATES FOR ROOT6: Wim Lavrijsen (WLavrijsen@lbl.gov)

PURPOSE:  Verify dictionary completeness by attempting to load all
          referenced classes
********************************************************************/

#include "AthenaSealSvc.h"

#ifdef ROOT6_MIGRATION_HAVE_REFLEX
#include "Reflex/Base.h"
#include "Reflex/Member.h"
#include "Reflex/Scope.h"
#include "Reflex/Type.h"
#endif

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/System.h"

#include <errno.h>
#include <set>
#include <algorithm>

/////////////////////////////////////////////////////////////////////////////
//
//  Job control cards:
//
//  "Dictionaries"    - list of the dictionaries to load
//
/////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
static inline ReflexType mig56_ReflexTypeByName(const std::string& name)
{
#ifdef ROOT6_MIGRATION_HAVE_REFLEX
    return ReflexType::ByName(name);
#else
    return ReflexType::ByName(name, kFALSE /* load */, kFALSE /* quiet */ );
#endif
}

static inline ReflexScope mig56_ReflexScopeByName(const std::string& name)
{
#ifdef ROOT6_MIGRATION_HAVE_REFLEX
    return ReflexScope::ByName(name);
#else
    return ReflexScope::ByName(name, kFALSE /* load */, kFALSE /* quiet */ );
#endif
}


//----------------------------------------------------------------------------  
AthenaSealSvc::AthenaSealSvc(const std::string& name, ISvcLocator* pSvcLocator) 
        :
        AthService(name, pSvcLocator),
        m_checkDictAtInit(false),
        m_checkDictionary(true),
        m_loadAllDicts(false),
        m_chronoStatSvc( "ChronoStatSvc", name ),
        m_dictLoaderSvc( "AthDictLoaderSvc", name ),
        m_doChronoStat (true)
{
    declareProperty ("DictNames",       m_dictionaryNames);
    declareProperty ("IgnoreNames",     m_ignoreNames);
    declareProperty ("CheckDictionary", m_checkDictionary);
    declareProperty ("CheckDictAtInit", m_checkDictAtInit);
    declareProperty ("LoadAllDicts",    m_loadAllDicts);
    declareProperty ("IgnoreDicts",     m_ignoreDicts);
    declareProperty ("UseChronoStag",   m_doChronoStat);

    // configure sensible defaults
    setDefaultDictNames();
    setDefaultIgnoreNames();
    setDefaultIgnoreDicts();
}

//----------------------------------------------------------------------------  
AthenaSealSvc::~AthenaSealSvc()
{}

//----------------------------------------------------------------------------  
StatusCode AthenaSealSvc::queryInterface ( const InterfaceID& riid, void** ppvInterface )
{
    if ( IAthenaSealSvc::interfaceID() == riid )
	{
	    *ppvInterface = (IAthenaSealSvc*)this;
	}
    else
	{
	    // Interface is not directly available: try out a base class
	    return AthService::queryInterface(riid, ppvInterface);
	}
    addRef();
    return StatusCode::SUCCESS;
}


//----------------------------------------------------------------------------  
StatusCode AthenaSealSvc::initialize()
{
/////////////////////////////////////////////////////////////////////////////
//
//  AthenaSealSvc::initialize()
//  Initiliaze the streams defined by the job control cards as follows:
//
//  No  data-card name                   Action
//  --  ---------------     --------------------------------------------
//   1. "Dictionaries"      - load the specified dictionaries
//  --  ---------------     ----------------------------------------------------
//
/////////////////////////////////////////////////////////////////////////////

    // get ChronoStatSvc
    StatusCode status = m_chronoStatSvc.retrieve();
    if (!status.isSuccess()) {
        ATH_MSG_ERROR ("Can not get ChronoStatSvc.");
        return(status);
    }

    if (!m_dictLoaderSvc.retrieve().isSuccess()) {
        ATH_MSG_ERROR ("could not retrieve athena dict. loader service !");
        return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG
        ("begin initialize() - loading dictionary fillers: size " 
         << m_dictionaryNames.value().size());

    if (m_dictionaryNames.value().size()) {

	if (m_doChronoStat) {
	    m_chronoStatSvc->chronoStart("AthenaSealSvc::dictLoading");
	}

	for (unsigned int i = 0; i < m_dictionaryNames.value().size(); i++) {
	    std::string name = m_dictionaryNames.value()[i];

	    // Try to load: we ignore the return code for the moment
	    status = loadDictFiller(name); 
	    if (status.isFailure()) {
		return status;
	    }
	}

	if (m_doChronoStat) {
	    m_chronoStatSvc->chronoStop("AthenaSealSvc::dictLoading");
	}
    }


    // Print out names to ignore
    for (unsigned int i = 0; i < m_ignoreNames.value().size(); i++) {
	std::string name = m_ignoreNames.value()[i];
	ATH_MSG_VERBOSE ("Ignore name: " << name);
    }


    // Load all classes available
    if(m_loadAllDicts) {
	status = loadClasses();
	if (status.isFailure()) {
            ATH_MSG_ERROR ("Unable to load classes");
	    return status;
	}
    }
    
    // Check the dictionary completeness
    if(m_checkDictAtInit) {
	try {
	    checkDictionary();
	}
	
	catch (std::exception& e) {
            ATH_MSG_FATAL ("*** Reflex exception caught: " << e.what());
            return StatusCode::FAILURE;
	}
    }

    ATH_MSG_VERBOSE ("end initialize()");

    return status;
}


//----------------------------------------------------------------------------  
StatusCode
AthenaSealSvc::loadClasses() const
{
    ATH_MSG_VERBOSE ("loadClasses: Looking for class capabilities");

    // Iterate over the reflex types

#ifdef ROOT6_MIGRATION_HAVE_REFLEX
    ReflexType_Iterator itr  = ReflexType::Type_Begin();
    ReflexType_Iterator last = ReflexType::Type_End();

    // Printout the list of classes/ one per module
    ATH_MSG_DEBUG
        ("Loading ALL dict libs - number is: " << ReflexType::TypeSize());
    for (; itr != last; ++itr) {
        if (!itr->IsClass() || !itr->IsStruct()) {
            continue;
        }
#if ROOT_VERSION_CODE < ROOT_VERSION(5,19,0)
        const std::string n = itr->Name(ROOT::Reflex::SCOPED);
#else
        const std::string n = itr->Name(Reflex::SCOPED);
#endif
        if ( !m_dictLoaderSvc->load_type (n)) {
            ATH_MSG_VERBOSE ("could not load reflex-dict for [" << n << "] !");
        }
        ATH_MSG_VERBOSE ("Called IDictLoaderSvc to load dictionary for: " << n);
    }
#else /* ROOT6 */
    // Don't loadClasses() ... at issue is that with no longer any distinction
    // between Reflex-based classes (i.e. ATLAS classes) and ROOT ones, we'd be
    // pulling all of ROOT and its derived uses.

    // Further, the use of this function seems to have been to resolve conflicts
    // by loading Reflex-based classes early. This is no longer an issue.

    // Final point: this function is only called when LoadAllDicts == True, which
    // is not the currently case (per LXR). Since this could change, a warning is
    // warrented:
    ATH_MSG_WARNING ("loadClasses() is a no-op under ROOT6 (LoadAllDicts ignored)");
#endif
    return (StatusCode::SUCCESS);
}


//----------------------------------------------------------------------------  
void 
AthenaSealSvc::show_member (const ReflexMember& m) const
{
    // none of this code will print unless the current level is VERBOSE, so just
    // return and be done with it if not
    if (!msgLvl(MSG::VERBOSE))
        return;

    const ReflexType t1 = m.DeclaringType ();
    const ReflexType t2 = m.TypeOf ();
    unsigned int width = 20;

    if (t1) {
        msg(MSG::VERBOSE) << "    " << m.Name();
	unsigned int s = m.Name().size();
	if (s < width) {
	    std::string tab(width - s,' ');
	    msg(MSG::VERBOSE) << tab;
	}
    }
    else msg(MSG::VERBOSE) << "  warning - declaring type unknown for " << m.Name();

    msg(MSG::VERBOSE) << " -- offset: ";
    if (m.Offset() < 100) {
        msg(MSG::VERBOSE) << ' '; 
	if (m.Offset() < 10) msg(MSG::VERBOSE) << ' ';
    }

    msg(MSG::VERBOSE) << m.Offset(); 

    if (t2) {
        msg(MSG::VERBOSE) << " -- type: " << t2.Name();
	if (m.IsTransient()) {
            msg(MSG::VERBOSE) << "  [declared transient]";
	}
	if (m.IsStatic()) {
            msg(MSG::VERBOSE) << "  [declared static]";
	}
    }
    else {
	if (m.IsTransient()) {
            msg(MSG::VERBOSE) << " -- type: " << "[unknown - declared transient]";
	}
	else if (m.IsStatic()) {
            msg(MSG::VERBOSE) << " -- type: " << "[unknown - declared static]";
	}
	else {
            msg(MSG::VERBOSE) << " -- type: " << "[unknown]";
	}
    }
    msg(MSG::VERBOSE) << endreq;
}

//----------------------------------------------------------------------------  
void 
AthenaSealSvc::show_type (const ReflexType& t) const
{
    // none of this code nor show_member() will print unless the current level
    // is VERBOSE, so just return and be done with it if not
    if (!msgLvl(MSG::VERBOSE))
        return;

    // show info of members
    msg(MSG::VERBOSE) << "Members of class " << t.Name() << ":" << endreq;
    for (size_t i = 0; i < t.DataMemberSize(); ++i) {
	ReflexMember m = t.DataMemberAt(i);
	if (m) {
            show_member (m);
	}
    }

}

//----------------------------------------------------------------------------  
bool
AthenaSealSvc::member_is_ok(const std::string& typeName,
			    const std::string& memberName) const
{
    if (typeName == "basic_string<char>" &&
	memberName == "_M_dataplus") {
	return (true);
    }
    if (typeName.find("map<") != std::string::npos &&
	memberName == "_M_t") {
	return (true);
    }
    if (typeName.find("hash_") != std::string::npos &&
	memberName == "_M_ht") {
	return (true);
    }
    if (typeName.find("set<") != std::string::npos &&
	memberName == "_M_t") {
	return (true);
    }
    if (typeName.find("std::_Vector_alloc_base<DataLinkVector") != std::string::npos &&
	memberName.find("_M_") != std::string::npos) {
	return (true);
    }
    if (typeName == "DetailedTrackTruthCollection" &&
	memberName == "m_trackCollection") {
	return (true);
    }
#if ROOT_VERSION_CODE > ROOT_VERSION(6,0,0)
    if( typeName.find("DataVector<") == 0 && memberName == "m_pCont") {
       msg(MSG::VERBOSE) << "**** Ignoring  " << typeName << "." << memberName << endreq;
       return (true);
    }
#endif
    
    return (false);
}

//----------------------------------------------------------------------------  
static bool 
isnum(const std::string& str) 
{
    for (unsigned int i = 0; i < str.size(); i++) {
        if ((str[i] < '0') || (str[i] > '9'))
            return false;
    }
    return true;
}

//----------------------------------------------------------------------------  
void
AthenaSealSvc::find_extra_types (const ReflexType& t,
				 std::vector<ReflexType>& newTypes) const
{
    // Add the type of each field

    // check that types of the fields are defined
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Looking for members of type " << t.Name() << ":"
                                                << "  newTypes size " << newTypes.size() << endreq;
    for (size_t i = 0; i < t.DataMemberSize(); ++i) {
	ReflexMember m = t.DataMemberAt(i);
	if (m) {
	    ReflexType t2 = m.TypeOf ();
	    // If type is pointer, get the underlying type
	    if (t2.IsPointer()) t2 = t2.ToType();
	    
	    // Skip transient fields - this and downstream classes may
	    // not yet be defined
	    if (m.IsTransient()) {
	        if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) 
                                              << " Skipping check for transient member " << m.Name()
                                              << " of type " << t2.Name()
                                              << endreq;
		continue;
	    }
            // Also skip static members.
	    if (m.IsStatic()) {
	        if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) 
                                              << " Skipping check for static member " << m.Name()
                                              << " of type " << t2.Name()
                                              << endreq;
		continue;
	    }
	    if (t2) {
		newTypes.push_back(t2);
                if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << " found class " << t2.Name() << endreq;
	    }
	}
    }

    // Add in types for each superclass
    if (t.IsClass()) {
	if(t.BaseSize()) {
            if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "    Loop over superclass names " << endreq;
	}
	for (size_t i = 0; i < t.BaseSize(); ++i) {
	    ReflexBase b = t.BaseAt(i);
	    if (b) {
		std::string bname = b.Name();
		// Remove stl container base classes
		if (bname.find("_base<") == std::string::npos) {
		    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "      found name: " << bname;
		    ReflexType tb = b.ToType();
		    if (tb) {
                        if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << ". Type IS defined in dictionary. ";
                        newTypes.push_back(tb);
		    }
		    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << endreq;
		}
	    }
	}
    }

    // Add in types for templates
    if (t.IsTemplateInstance()) {
        if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << " Loop over template args of " << t.Name()<< " "
                                                    << endreq;
	for (size_t i = 0; i < t.TemplateArgumentSize(); ++i) {
	    ReflexType ta = t.TemplateArgumentAt(i);
            
            // If type not found, try to dynamically load it
            std::string taname = ta.Name(7);
            if (!ta) {
                std::string name = ta.Name(7);
                // skip single digit strings
                if (name.size() == 1 && isdigit(name[0])) {
                    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Skip template arg: " << name << " is digit " << endreq;
                    continue;
                }
                if ('*' == name[name.size()-1]) name = name.substr(0, name.size()-1);
                if ("const " == name.substr(0, 6)) name = name.substr(6, name.size());
		int ntypesBefore = ReflexType::TypeSize();
                if (m_doChronoStat) {
                    m_chronoStatSvc->chronoStart("AthenaSealSvc::autoLoading");
                }
                ta = m_dictLoaderSvc->load_type (name);
                if (m_doChronoStat) {
                    m_chronoStatSvc->chronoStop("AthenaSealSvc::autoLoading");
                }
                if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Trying to 'Autoload' dictionary for class "
                                                            << name
                                                            << " ntypes before load " << ntypesBefore
                                                            << " ntypes loaded " << ReflexType::TypeSize() - ntypesBefore
                                                            << " top level class " <<     m_currentClassName << endreq;
            }


	    // If type is pointer, get the underlying type
	    if (ta.IsPointer()) ta = ta.ToType();
	    
	    if (ta) {
		newTypes.push_back(ta);
                if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << " name " << ta.Name()<< " " << endreq;
	    }
	    else {
	        if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << " type not found " << taname
                                                            << " isnum: " << isnum(taname) << endreq;
		if (!(ignoreName(t) || ignoreName(ta) || isnum(taname) )) {
                    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) 
                                                  << "****> Missing type for template argument " << taname
                                                  << " of template " << t.Name()
                                                  << endreq;
                    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) 
                                                  << "****> For the moment, we DO NOT return FAILURE - so expect abort if type is really missing!! "
                                                  << endreq;
		}
	    }
	    if (t.IsPointer()) {
                if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " type is pointer:  " << t.Name() << endreq;
	    }

            // Check for pointers and add in simple class
// 		msg(MSG::VERBOSE) << "Checking for pointer class: "
// 		    <<((*it2)) << endreq;
// 		    std::string nm = (*it2);
// 		    nm = nm.substr(0, nm.size() - 1);
// 		    msg(MSG::VERBOSE) << " try class " << nm << " " << endreq;
	}
    }

    return;
}

//----------------------------------------------------------------------------  
bool 
AthenaSealSvc::incorrect_guid (const ReflexType& t) const
{
    bool result = false;

    // Check that guid is ok

    // If class has GUID, try to look it up in this way
    ReflexPropertyList plist = t.Properties();
    if (plist.HasProperty("ClassID")) {
	const std::string clid = plist.PropertyAsString("ClassID");
	if (clid.size()) {
	    for (unsigned int i = 0; i < clid.size(); ++i) {
		if ('a' <= clid[i] && clid[i] <= 'z') {
		    msg(MSG::ERROR)
                        << "----->  Found lower case letter in class guid: pos, value - "
                        << i << ", " << clid[i] << endreq
                        << "----->  For class, guid " 
                        << t.Name() << ", " << clid << endreq
                        << "----->  The guid MUST be all upper case. "
                        << endreq;
		    result = true;
		}
	    }
	}
	else {
            ATH_MSG_VERBOSE ("Class " << t.Name() << " has an empty CLID. ");
	}
    }
    else {
        ATH_MSG_VERBOSE ("Class " << t.Name() << " does NOT have CLID. ");
    }
    return result;
}


//----------------------------------------------------------------------------  
bool 
AthenaSealSvc::missing_member_types (const ReflexType& t) const
{
    bool result  = false;
    int nmembers = 0;

    // check that types of the fields are defined
    if (t.DataMemberSize()) {
        ATH_MSG_DEBUG ("Checking members of type " << t.Name() << " for "
                       << t.DataMemberSize() << " members:");
    }

    bool first = true;
    for (size_t i = 0; i < t.DataMemberSize(); ++i) {
	ReflexMember m = t.DataMemberAt(i);
	if (m) {
	    ReflexType t1 = m.DeclaringType ();
	    ReflexType t2 = m.TypeOf ();
	    bool transient = m.IsTransient();
            bool is_static = m.IsStatic();
#ifdef ROOT6_MIGRATION_HAVE_REFLEX
            // enum class members don't exist in Reflex
            bool is_enum   = false;
#else
            bool is_enum   = m.IsEnum();
            //MN: protect against anonymous enums
            if( t2.Name().find("(anonymous)") != std::string::npos )
               is_enum = true;
#endif            

	    msg(MSG::VERBOSE) << "Checking member: " << m.Name() << " of type " << t1.Name(7) << "::" << t2.Name(7) << endreq;
#ifndef ROOT6_MIGRATION_HAVE_REFLEX
	    msg(MSG::VERBOSE) << "Trans:" << transient << " Static:" << is_static << " Enum:" << is_enum << " Const:" << m.IsConstant() << endreq;
#endif            
	    if (!t2 && !member_is_ok(t1.Name(), m.Name())
                && !transient && !is_static && !is_enum ) {
                // Missing type, try to load it
                //  We ignore transient types and some special members
		std::string name = t2.Name(7);
                if ('*' == name[name.size()-1]) name = name.substr(0, name.size()-1);
                if ("const " == name.substr(0, 6)) name = name.substr(6, name.size());
                ATH_MSG_VERBOSE (' ');
		int ntypesBefore = ReflexType::TypeSize();
                if (m_doChronoStat) {
                    m_chronoStatSvc->chronoStart("AthenaSealSvc::autoLoading");
                }
		t2 = m_dictLoaderSvc->load_type (name);
                if (m_doChronoStat) {
                    m_chronoStatSvc->chronoStop("AthenaSealSvc::autoLoading");
                }
                ATH_MSG_VERBOSE ( "Trying to 'Autoload' dictionary for class "
                                  << name
                                  << " ntypes before load " << ntypesBefore
                                  << " ntypes loaded " 
                                  << ReflexType::TypeSize() - ntypesBefore
                                  << " top level class " <<     m_currentClassName);
            }

	    if (t2) {
		nmembers++;
	    }
	    else {
		// 0 => possible error:
		// Know exceptions:
		if (member_is_ok(t1.Name(), m.Name()) || transient || is_static || is_enum ) {
		    if (first) {
			first = false;
			ATH_MSG_VERBOSE (' ');
		    }
		    if (transient) {
                        ATH_MSG_VERBOSE ("****> IGNORING missing type for transient member "
                                         << "  " << m.Name());
                    }
		    else if (is_static) {
                        ATH_MSG_VERBOSE ("****> IGNORING missing type for static member "
                                         << "  " << m.Name());
                    }
		    else {
                        ATH_MSG_VERBOSE ("****> IGNORING missing type for " << t1.Name()
                                         << "  " << m.Name());
                    }
		}
		else {
		    if (first) {
			first = false;
			ATH_MSG_VERBOSE (' ');
		    }
		    ATH_MSG_VERBOSE ("****> Missing type "
                                     << m.TypeOf().Name(7)
                                     << " for " << t1.Name()
                                     << "::" << m.Name());
		    result = true;
		}
	    }
	}
    }
    if (!result) {
	if (t.DataMemberSize()) 
            ATH_MSG_VERBOSE (" ok  - isComplete " << t.IsComplete());
    }

    // print out class info
    if(nmembers && msgLvl(MSG::VERBOSE)) show_type(t);
    return (result);
}



//----------------------------------------------------------------------------  
StatusCode AthenaSealSvc::checkClass(const std::string& typeName) const
{
   //msg().setLevel(MSG::VERBOSE);
   
    if(!m_checkDictionary) {
        ATH_MSG_VERBOSE 
            ("checkClass - AthenaSealSvc.CheckDictionary is set to false. "
             "No checks done for class:  " << typeName);
	return (StatusCode::SUCCESS);
    }

    if (m_doChronoStat) {
        m_chronoStatSvc->chronoStart("AthenaSealSvc::checkClass");
    }

    ATH_MSG_DEBUG 
        ("checkClass - Number of types on entry " << ReflexType::TypeSize());

    ReflexType t = mig56_ReflexTypeByName(typeName);

    // Set current class name
    m_currentClassName = typeName;
    if (!t) {
	int ntypesBefore = ReflexType::TypeSize();
        if (m_doChronoStat) {
            m_chronoStatSvc->chronoStart("AthenaSealSvc::autoLoading");
        }
        if (!m_dictLoaderSvc->load_type (typeName)) {
            ATH_MSG_DEBUG ("problem loading type [" << typeName << "] !");
	}
        if (m_doChronoStat) {
            m_chronoStatSvc->chronoStop("AthenaSealSvc::autoLoading");
        }
        t = mig56_ReflexTypeByName(typeName);
        ATH_MSG_VERBOSE 
            ("Loaded dictionary for class " << typeName 
             << " ntypes before load " << ntypesBefore
             << " ntypes loaded " << ReflexType::TypeSize() - ntypesBefore);
    }


    // typeName comes from typeinfo and may not be the same as the
    // name of the class in the reflection dictionary, e.g. for
    // maps. Skip checks if class not found.
    if (!t) {
        ATH_MSG_WARNING
            ("checkClass - could NOT find reflection class for type "
             << typeName << " => NO CHECKS DONE ");

	ReflexScope s = mig56_ReflexScopeByName(typeName);
	if (s) {
            ATH_MSG_DEBUG ("checkClass - found scope " << s.Name());
	}
	else {
            ATH_MSG_DEBUG ("checkClass - unable to find scope ");
	}

	return (StatusCode::SUCCESS);
    }

    ATH_MSG_DEBUG ("checkClass - found type " << t.Name());

    // We require class c to have a GUID - assume that it it a data
    // object. (Otherwise, add and argument to the method call)
    ReflexPropertyList plist = t.Properties();
    const std::string clid = plist.PropertyAsString("ClassID");
    ATH_MSG_DEBUG ("checkClass - found ClassID " << clid);
    if (!clid.size()) {
        ATH_MSG_ERROR
            ("----->  MISSING GUID (id in selection file) for class " << t.Name()
             << endreq
             << "----->  This is needed for ALL DataObjects ");
        return (StatusCode::FAILURE);
    }

    std::vector<ReflexType> toCheck;
    std::vector<ReflexType> toCheckAll;
    std::vector<ReflexType>::const_iterator it;

    std::map<std::string, ReflexType> checked;
    std::string cn = t.Name();
    checked[cn] = t;
    if(cn.find('*') == std::string::npos)
       m_checkedClasses.insert(cn);

    // Print names for verbose
//      ATH_MSG_VERBOSE ("Found classes");
//      std::vector <const reflect::Class*> myvec = reflect::Class::forNames ();
//      for (it = myvec.begin(); it != myvec.end (); ++it) {
//  	const reflect::Class* c = *it;
//  	if (c) ATH_MSG_VERBOSE (c->fullName());
//      }


    bool continueCheck = true;

    do {
       if( !ignoreName(t.Name()) ) {
          if(missingTypes(t)) return (StatusCode::FAILURE);

          ATH_MSG_VERBOSE ("checkClass - check for class " << t.Name() << " ok");
          // Collect the extra types for this class - fields, base
          // classes, template args, etc.
          find_extra_types (t, toCheck);

          // Select the new classes to be checked
          for (it = toCheck.begin(); it != toCheck.end(); ++it) {
             if (checked.find((*it).Name()) == checked.end()) {
		toCheckAll.push_back((*it));
		cn = (*it).Name();
		checked[cn] = (*it);
		if(cn.find('*') == std::string::npos)
                   m_checkedClasses.insert(cn);
//		ATH_MSG_VERBOSE ("checkClass - added class " << (*it)->fullName());
             }
          }
          toCheck.clear();
       }
       
	if (toCheckAll.size() > 0) {
	    // Select the next one to check and save in set
	    t = toCheckAll[0];
	    //checked.insert(c);
	    //checked[c->fullName()] = c;
	}
	else {
	    continueCheck = false;
	}
	if (toCheckAll.size() > 1) {
	    // Remove the first element
	    toCheck = toCheckAll;
	    toCheckAll.clear();
	    it = toCheck.begin();
	    ++it;
	    for (; it != toCheck.end(); ++it )toCheckAll.push_back((*it));
	    // Clear vec for collecting new classes to check
	    toCheck.clear();
	}
	else {
	    toCheckAll.clear();
	}


    } while (continueCheck);

    ATH_MSG_DEBUG
        ("checkClass - NO MISSING FIELDS!!!"
         << endreq
         << "checkClass - Number of types on exit " << ReflexType::TypeSize());

//     ATH_MSG_VERBOSE ("Classes checked: ");

//     std::map<std::string, const reflect::Class*>::const_iterator it1   = checked.begin();
//     std::map<std::string, const reflect::Class*>::const_iterator last1 = checked.end();
//     for (; it1 != last1; ++it1) {
// 	ATH_MSG_VERBOSE ((*it1).first);
//    }

    if (m_doChronoStat) {
        m_chronoStatSvc->chronoStop("AthenaSealSvc::checkClass");
    }

    return (StatusCode::SUCCESS);

}


//----------------------------------------------------------------------------  
StatusCode 
AthenaSealSvc::loadDictFiller(const std::string& dictName) const
{
    unsigned int typesBefore = ReflexType::TypeSize();
    
    System::ImageHandle h;
    long rc = System::loadDynamicLib (dictName, &h);

    ATH_MSG_DEBUG
        ("loadDictFiller - Ntypes before " << typesBefore 
         << " types added "  << (ReflexType::TypeSize() - typesBefore)
         << " dictionary filler name:" << dictName);

    if (1 == rc) {
        ATH_MSG_VERBOSE (" Loaded dict filler " << dictName);
    }
    else {
        ATH_MSG_ERROR 
            (" Unable to load " << dictName << "  rc=" << rc
             << " h=" << h << " errno=" << errno);
        return (StatusCode::FAILURE);
    }
    return (StatusCode::SUCCESS);
}

//----------------------------------------------------------------------------  
void AthenaSealSvc::checkDictionary() const
{
    ATH_MSG_DEBUG
        ("Checking the completeness of the dictionary for all classes");
    bool isMissingTypes = false;

    ATH_MSG_VERBOSE ("Number of types: " << ReflexType::TypeSize());

    for (size_t i = 0; i < ReflexType::TypeSize(); ++i) {
	ReflexType t = ReflexType::TypeAt(i);
	if(t && t.IsClass()) {
	    ATH_MSG_DEBUG ("Checking type " << t.Name() << " number: " << i);
//	    if (missingTypes(t)) isMissingTypes = true;

	    // Build up full type name
	    std::string name = t.Name();
	    bool finished = false;
	    ReflexScope s =  t.DeclaringScope();
	    do {
		if (s) {
// 		    ATH_MSG_DEBUG ("checkDictionary - found scope " << s.Name());
		    if (s.IsTopScope()) {
			finished = true;
		    }
		    else {
			name = s.Name() + "::" + name;
			s = s.DeclaringScope();
		    }
		}
		else {
		    finished = true;
		}
	    } while (!finished);
	    
	    ReflexType t1 = mig56_ReflexTypeByName(name);
	    if(t1) {
                ATH_MSG_DEBUG ("Found type: " << t1.Name());
	    }
	    ATH_MSG_DEBUG ("checkDictionary - full scope name " << name);

	    //if (checkClass(t.Name()) != StatusCode::SUCCESS) isMissingTypes = true;
	    if (checkClass(name) != StatusCode::SUCCESS) isMissingTypes = true;

	}
	//ATH_MSG_VERBOSE ("Checking type number: " << i);
    }

    if (isMissingTypes) {

        ATH_MSG_DEBUG ("----->  Missing fields. Please check output and complete selection file!!");

    }
    else {
        ATH_MSG_DEBUG ("----->  NO Missing fields!!");
    }
}

//----------------------------------------------------------------------------  
bool AthenaSealSvc::ignoreName(const ReflexType& t) const
{
//     if (t) {
    ATH_MSG_VERBOSE ("Check if class should be ignored: " << t.Name());
//     }
//     else {
// 	return (false);
//     }
    // Now try and match
    for (unsigned int i = 0, iMax = m_ignoreNames.value().size(); i!=iMax; i++) {
	const std::string& name = m_ignoreNames.value()[i];
	if(t.Name().find(name) != std::string::npos) {
            ATH_MSG_VERBOSE
                ("Warning: Ignoring checks for class name: " << t.Name());
            return (true);
	}

    }
    ATH_MSG_VERBOSE ("Not ignored.");
    return (false);
}

//----------------------------------------------------------------------------  
bool AthenaSealSvc::ignoreName(const std::string& typeName) const
{
    if (typeName.size()) {
        ATH_MSG_VERBOSE ("Check if class should be ignored: " << typeName);
    }
    else {
	return (false);
    }
    // Now try and match
    for (unsigned int i = 0, iMax = m_ignoreNames.value().size(); i!=iMax; i++) {
	const std::string& ignoreName = m_ignoreNames.value()[i];
	if(typeName.find(ignoreName) != std::string::npos) {
	    ATH_MSG_VERBOSE 
                ("Warning: Ignoring checks for class name: " << typeName);
	    return (true);
	}

    }
    ATH_MSG_VERBOSE ("Not ignored.");
    return (false);
}


//----------------------------------------------------------------------------  
bool
AthenaSealSvc::missingTypes(const ReflexType& t) const
{
    bool isMissingTypes = false;

    // Check for missing class name - NOT NORMAL!!!
    if (t) {
	if (t.Name() == "")  { 
	    ATH_MSG_DEBUG ("missingTypes: empty Type name - NOT NORMAL ");
	    return (false);
	}
    }
    else {
	// Missing class object, return true
	return (true);
    }


    //ATH_MSG_DEBUG ("missingTypes: check type " << t.Name() << " isComplete  " << t.IsComplete());


    // Check if we should ignore this class
    if (ignoreName(t)) return (false);


    // Check c and collect additional classes to check
    if (missing_member_types (t)) isMissingTypes = true;


    // Now check base classes
    if (t.IsClass()) {
	if(t.BaseSize()) {
            ATH_MSG_VERBOSE ("    Loop over superclass names ");
	}
	for (size_t i = 0; i < t.BaseSize(); ++i) {
	    ReflexBase b = t.BaseAt(i);
	    if (b) {
		std::string bname = b.Name();
		// Remove stl container base classes
		if (bname.find("_base<") == std::string::npos)
                {
                    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "      found name: " << bname;
		    ReflexType tb = b.ToType();
		    if (tb) {
		        if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << ". Type IS defined in dictionary. ";
		    }
                    else {
                      if (!ignoreName(bname)) {
		        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) 
                                                  << endreq
                                                  << "    ****> Missing base class not defined in dictionary!! " 
                                                  << endreq
                                                  << "    ****>   Looking for base class " 
                                                  << bname << " of type " << t.Name()
                                                  << endreq;
			isMissingTypes = true;
                      }
		    }
		}
	    }
	    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << endreq;
	}
    }

    // Now check the guid
    if (incorrect_guid (t)) isMissingTypes = true;

    return (isMissingTypes);
}


//----------------------------------------------------------------------------  
StatusCode AthenaSealSvc::finalize()
{
    ATH_MSG_DEBUG ("finalize() in AthenaSealSvc");

    if (msgLvl(MSG::VERBOSE)) {
        ATH_MSG_VERBOSE ("Classes checked: ");

        std::set<std::string>::const_iterator it   = m_checkedClasses.begin();
        std::set<std::string>::const_iterator last = m_checkedClasses.end();
        for (; it != last; ++it) {
            ATH_MSG_VERBOSE ((*it));
        }
    }
    return StatusCode::SUCCESS;
}


void AthenaSealSvc::setDefaultDictNames()
{
    std::vector<std::string> dictNames;
    dictNames.reserve( 10 );

    // STL dictionaries 
#ifdef ROOT6_MIGRATION_HAVE_REFLEX
    dictNames.push_back( "STLRflx" );
#endif
//   dictNames.push_back( "STLAddRflx" );
//   dictNames.push_back( "AtlasSTLAddReflexDict" );

    m_dictionaryNames.set( dictNames );
    return;
}

void AthenaSealSvc::setDefaultIgnoreNames()
{
    std::vector<std::string> ignoreNames;
    ignoreNames.reserve( 100 );

    /// Gaudi
    ignoreNames.push_back( "Gaudi" );
    ignoreNames.push_back( "GaudiPython::Interface" );
    ignoreNames.push_back( "GaudiPython::Helper" );
    ignoreNames.push_back( "FactoryTable" );
    ignoreNames.push_back( "IInterface" );
    ignoreNames.push_back( "IFactory" );
    ignoreNames.push_back( "IAlgFactory" );
    ignoreNames.push_back( "ISvcFactory" );
    ignoreNames.push_back( "IToolFactory" );
    ignoreNames.push_back( "InterfaceID" );
    ignoreNames.push_back( "StatusCode" );
    ignoreNames.push_back( "IAppMgrUI" );
    ignoreNames.push_back( "IProperty" );
    ignoreNames.push_back( "Property" );
    ignoreNames.push_back( "std::vector<Property*>" );
    ignoreNames.push_back( "std::vector<const Property*>" );
    ignoreNames.push_back( "std::list<IAlgorithm*>" );
    ignoreNames.push_back( "std::list<IService*>" );
    ignoreNames.push_back( "std::list<const IFactory*>" );
    ignoreNames.push_back( "std::vector<IRegistry*>" );
    ignoreNames.push_back( "SimpleProperty" );
    ignoreNames.push_back( "SimplePropertyRef" );
    ignoreNames.push_back( "IService" );
    ignoreNames.push_back( "IAlgorithm" );
    ignoreNames.push_back( "ISvcManager" );
    ignoreNames.push_back( "IAlgManager" );
    ignoreNames.push_back( "IJobOptionsSvc" );
    ignoreNames.push_back( "ISvcLocator" );
    ignoreNames.push_back( "IEventProcessor" );
    ignoreNames.push_back( "IDataProviderSvc" );
    ignoreNames.push_back( "IDataManagerSvc" );
    ignoreNames.push_back( "IRegistry" );
    ignoreNames.push_back( "ContainedObject" );
    ignoreNames.push_back( "std::vector<const ContainedObject*>" );
    ignoreNames.push_back( "DataObject" );
    ignoreNames.push_back( "IHistogramSvc" );
    ignoreNames.push_back( "AIDA::I" );
    ignoreNames.push_back( "Algorithm" );
    ignoreNames.push_back( "Service" );
    ignoreNames.push_back( "GaudiPython::PyAlgorithm" );
    ignoreNames.push_back( "GaudiPython::PyAlgorithmWrap" );
    ignoreNames.push_back( "IParticlePropertySvc" );
    ignoreNames.push_back( "ParticleProperty" );
    ignoreNames.push_back( "StoreGateSvc" );
    ignoreNames.push_back( "IStoragePolicy" );

    /// POOL - StorageSvc/DbArray.h
    ignoreNames.push_back( "CharDbArray" );
    ignoreNames.push_back( "DoubleDbArray" );
    ignoreNames.push_back( "FloatDbArray" );
    ignoreNames.push_back( "IntDbArray" );
    ignoreNames.push_back( "LongDbArray" );
    ignoreNames.push_back( "ShortDbArray" );


    /// Dicts added by athena.py
    ignoreNames.push_back( "AthenaEventLoopMgr" );
    ignoreNames.push_back( "MinimalEventLoopMgr" );
    ignoreNames.push_back( "PyAthenaEventLoopMgr" );
    ignoreNames.push_back( "NTuple::Directory" );
    ignoreNames.push_back( "NTuple::File " );
    ignoreNames.push_back( "NTuple::Tuple" );
    ignoreNames.push_back( "INTuple" );
    ignoreNames.push_back( "NTuple::Tuple" );

    /// Ignore extra template args
    ignoreNames.push_back( "greater<int>" );
    ignoreNames.push_back( "allocator<" );

    // MN:  adding stuff for ROOT6
    ignoreNames.push_back( "string" );

    m_ignoreNames.set( ignoreNames );
    return;
}

void AthenaSealSvc::setDefaultIgnoreDicts()
{
    std::vector<std::string> ignoreDicts;
    ignoreDicts.reserve( 5 );

    ignoreDicts.push_back( "libSealCLHEPDict" );

    m_ignoreDicts.set( ignoreDicts );
    return;
}
