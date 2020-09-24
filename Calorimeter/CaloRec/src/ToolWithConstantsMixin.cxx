// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file  ToolWithConstantsMixin.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date March, 2006, updated from June, 2004
 * @brief A mixin class to hold a set of constants for an Algorithm,
 *        Tool, or Service.
 */


#include "CaloRec/ToolWithConstantsMixin.h"
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/Service.h"
#include "AthenaKernel/getMessageSvc.h"

#include "CoralBase/AttributeListException.h"
#include "CoralBase/Blob.h"
//#include "CoolKernel/StorageType.h"

#include "CaloRec/Blob2ToolConstants.h"
#include <stdexcept>


namespace CaloRec {


/**
 * @brief Constructor.
 *
 * The only reason we need this is that SG stupidly requires that the
 * DataHandle be const.
 */
ToolWithConstantsMixin::ToolWithConstantsMixin()
  : m_detStore(nullptr),
    m_use_callback(true),
    m_order(0),
    m_isdummy(false),
    m_coolInlineTool (nullptr),
    m_toolSvc (nullptr)
{
}


/**
 * @brief Remainder of constructor code, that needs the complete
 *        class to be initialized.
 * @param toolcls Name of derived C++ class.
 *                This is used only for @c mergeConstants; further,
 *                it can be safely defaulted for the @c AlgTool case.
 *
 * This needs to be called from the derived class's constructor.
 */
void
ToolWithConstantsMixin::finish_ctor (const std::string& toolcls /*= ""*/)
{
  m_use_callback = true;
  m_toolcls = toolcls;

  if (AlgTool* thistool = dynamic_cast<AlgTool*> (this)) {
    thistool->declareProperty ("detStoreKey", m_detStoreKey);
    thistool->declareProperty ("prefix",      m_prefix);
    thistool->declareProperty ("useCallback", m_use_callback);
    thistool->declareProperty ("COOLFolder",  m_folderName);
    if (m_toolcls.empty())
      m_toolcls = thistool->type();
  }
  else if (Gaudi::Algorithm* thisalg = dynamic_cast<Gaudi::Algorithm*> (this)) {
    thisalg->declareProperty ("detStoreKey", m_detStoreKey);
    thisalg->declareProperty ("prefix",      m_prefix);
    thisalg->declareProperty ("useCallback", m_use_callback);
    thisalg->declareProperty ("COOLFolder",  m_folderName);
  }
  else if (Service* thissvc = dynamic_cast<Service*> (this)) {
    thissvc->declareProperty ("detStoreKey", m_detStoreKey);
    thissvc->declareProperty ("prefix",      m_prefix);
    thissvc->declareProperty ("useCallback", m_use_callback);
    thissvc->declareProperty ("COOLFolder",  m_folderName);
  }
  else
    std::abort();

  declareConstant ("order",       m_order = 0,        true);
  declareConstant ("isDummy",     m_isdummy = false,  true);
}


/**
 * @brief Destructor.
 */
ToolWithConstantsMixin::~ToolWithConstantsMixin()
{
  // Delete the Propinfo_* structures that we've allocated.
  for (unsigned int i=0; i < m_infos.size(); i++)
    delete m_infos[i];
}


/**
 * @brief Initialize method.
 *
 * Here, we check that all declared constants actually received values.
 *
 * The class using this must override @c initialize and call first
 * the standard @c AlgTool::initialize followed by
 * @c ToolWithConstantsMixin::initialize.
 */
StatusCode ToolWithConstantsMixin::initialize()
{  
  ISvcLocator* svcLoc = Gaudi::svcLocator( );
  if (!svcLoc) {
    REPORT_MESSAGE(MSG::ERROR) << "Could not get svcLocator!";
    return StatusCode::FAILURE;
  }

  CHECK(svcLoc->service("ToolSvc",m_toolSvc));
  CHECK(svcLoc->service("DetectorStore", m_detStore));

  REPORT_MESSAGE(MSG::DEBUG) << "Source of constants: DetStoreKey: '" << m_detStoreKey << "' FolderName: '" << m_folderName << "'";

  // The SG key should be set via job options.
  // Don't try to read from pool if there isn't one set.
  StatusCode sc = StatusCode::SUCCESS;
  
    
  if (!m_detStoreKey.empty() && !m_folderName.empty() && m_detStore->contains<CondAttrListCollection>(m_folderName)) {
    //First case: Read constants from COOL inline
    REPORT_MESSAGE(MSG::DEBUG) << "Reading constants from COOL-inline storage. Folder " << m_folderName 
			       << " channel " << m_detStoreKey;

    // ...register callback.
    if (m_use_callback) {
      CHECK(m_toolSvc->retrieveTool("Blob2ToolConstants",m_coolInlineTool));
      CHECK(m_coolInlineTool->addFolder(m_folderName,m_detStoreKey));
      CHECK(m_detStore->regFcn(&Blob2ToolConstants::fillObjects,
			       m_coolInlineTool,
			       &CaloRec::ToolWithConstantsMixin::processConstantsFromDB,
			       dynamic_cast<CaloRec::ToolWithConstantsMixin*>(this),
			       true));
    }
    else {
      CHECK (m_detStore->bind (m_DBconstants,m_detStoreKey));
      int i=0;
      std::list<std::string> l;
      CHECK(processConstantsFromDB (i, l) );
    }
  }
  else if (!m_detStoreKey.empty() && m_detStore->contains<ToolConstants>(m_detStoreKey)) {
     REPORT_MESSAGE(MSG::DEBUG) << "Reading constants from POOL storage. SG Key " << m_detStoreKey;
    //Second case: Read from POOL (relayed via COOL or CondProxyProviderSvc) 
     // ...register callback.
     if (m_use_callback) {
       CHECK(m_detStore->regFcn(
				&CaloRec::ToolWithConstantsMixin::processConstantsFromDB,
				dynamic_cast<CaloRec::ToolWithConstantsMixin*>(this),
				m_DBconstants,m_detStoreKey));
     }
     else {
       CHECK (m_detStore->bind (m_DBconstants, m_detStoreKey));
       int i=0;
       std::list<std::string> l;
       CHECK( processConstantsFromDB (i, l) );
     }
  }
  else {
    REPORT_MESSAGE(MSG::INFO) << "No database info found. Loading all from jobOpts";
    // Check to be sure that all constants have been initialized.
    if (!m_isdummy) {
      for (unsigned int i=0; i < m_infos.size(); i++) {
        if (!m_infos[i]->m_set && !m_infos[i]->m_default) {
          REPORT_ERROR(StatusCode::FAILURE)
            << "Constant named " << m_infos[i]->m_name
            << " did not get initialized. ";
          sc = StatusCode::FAILURE;
        }
      }
      if (sc.isFailure())
        REPORT_ERROR(StatusCode::FAILURE)
          << "One or more constants were not set by jobO "
          << "and could not be loaded from the DetStore!";  
    }
  }
  return sc;
}   

     
/**
 * @brief IOVDB callback function
 * @param Macro defined in AthenaKernel/IOVSvcDefs.h
 *
 * This function is called when ToolConstants are loaded from pool.
 * It checks which constants have been already set by the PropertySvc,
 * other constants are set by this function. 
 */
StatusCode
ToolWithConstantsMixin::processConstantsFromDB (IOVSVC_CALLBACK_ARGS_K(keys))
{
  MsgStream log(Athena::getMessageSvc(), name());
  log << MSG::DEBUG
      << "In Callback function ToolWithConstants::processConstantsFromDB" 
      << endmsg;

  if (!m_folderName.empty()) {
    //COOL-inline case
    if (std::find(keys.begin(),keys.end(),m_folderName)==keys.end()) {
       log << MSG::DEBUG
	   << "The cool folder we care about (" << m_folderName<< ") is not in the list of keys. Do nothing." << endmsg;
       return StatusCode::SUCCESS;
    }
    else
       log << MSG::DEBUG
	   << "Found cool folder " << m_folderName << " in the list of keys." << endmsg;
  }

  // Check validity
  //if (!m_DBconstants.isValid()) {
  if (m_detStore->retrieve(m_DBconstants, m_detStoreKey).isFailure()) {
    log << MSG::ERROR << "DataHandle of ToolConstants object invalid!"
        << endmsg;
    return StatusCode::FAILURE;
  }
  
  return fillFromDB(m_DBconstants);
}

StatusCode
ToolWithConstantsMixin::fillFromDB(const ToolConstants* constants) {
  MsgStream log(Athena::getMessageSvc(), name());
  // Check what's already set and what's missing
  for (unsigned int i=0;i<m_infos.size();i++) {
    if (m_infos[i]->m_set) {
      log << MSG::DEBUG << "Property '" << m_infos[i]->m_name
          << "' set by jobOptions." << endmsg;
    } 
    else if (constants->hasrep (m_prefix + m_infos[i]->m_name)) {
      try {
        const Arrayrep& rep =
          constants->getrep(name(),               //context (tool name)
			    m_prefix + m_infos[i]->m_name); // Prop name 

        // remember that this constant is set.
        m_infos[i]->m_setDB = true;
        CHECK(m_infos[i]->setFromDB(rep, *this));

        if (m_infos[i]->m_setDB)
          log << MSG::DEBUG << "Property '" << m_infos[i]->m_name
              << "' updated from database." << endmsg;
        else
          log << MSG::DEBUG << "Property '" << m_infos[i]->m_name
              << "' loaded from database."<< endmsg;
      }
      catch (GaudiException& e)
      {
      }
    } // end else
  } // end for           

  // Now check if all constants were set (either be jobO or DB)
  StatusCode sc = StatusCode::SUCCESS;
  if (!m_isdummy) {
    for (unsigned int i=0; i < m_infos.size(); i++) {
      if (!m_infos[i]->m_set && !m_infos[i]->m_setDB && !m_infos[i]->m_default)
      {
        REPORT_ERROR(StatusCode::FAILURE)
          << "Constant named " << m_infos[i]->m_name
          << " did not get initialized from jobOptions "
          << "and could not be loaded from DB.";
        sc = StatusCode::FAILURE;
      }
    }

    // Check version.
    if (constants->version() > version()) {
      REPORT_ERROR(StatusCode::FAILURE)
        << "Constants for tool " << name() << " require code version "
        << m_DBconstants->version() << " but only version "
        << version() << " is available.";
      sc = StatusCode::FAILURE;
    }
  }

  return sc;
}         


/**
 * @brief Method to set a property value.
 * @param propname The name of the property to set.
 * @param value The value to which to set it.
 * @return Gaudi status code.
 *
 * The property service calls this method when it actually makes a property
 * setting.  We override it in order to hook our own code into the process.
 * If this property corresponds to one of our declared
 * constants, we install it in the @c ToolConstants structure.
 * If needed, we also complete initialization of the declared
 * variable here.
 *
 * The class using this must override this method, and call first
 * @c AlgTool::setProperty
 * followed by @c ToolWithConstantsMixin::setProperty.
 * You may need to use a using directive to prevent warnings
 * (like above, but using the other base class).
 *
 * This version is used by Gaudi up to at least v16.
 */
StatusCode
ToolWithConstantsMixin::setProperty (const std::string& propname,
                                     const std::string& value)
{
  // Don't do anything if this was defaulted.
  // We'll either read it from pool later or give an error.
  if (value.empty() || value == "\"\"")
    return StatusCode::SUCCESS;

  // Loop through the declared constants, and see if there's one that
  // matches the input name.  It's ok if there's no match.
  for (unsigned int i=0; i < m_infos.size(); i++) {
    if (m_infos[i]->m_name == propname) {
      // Found a match.  Remember that we've seen this.
      m_infos[i]->m_set = true;

      // Install this name in the @c ToolConstants structure.
      try {
        const Arrayrep& rep = m_constants.newrep (name(),
                                                  propname,
                                                  value);

        // Complete any other (type-dependent) needed initialization.
        CHECK( m_infos[i]->set (rep, *this) );
      }
      catch (const std::runtime_error& e) {
        REPORT_MESSAGE (MSG::ERROR) << "Error reading from job options: "
                                    << e.what();
        return StatusCode::FAILURE;
      }
        
      break;
    }
  }

  // Done.
  return StatusCode::SUCCESS;
}


/**
 * @brief Method to set a property value.
 * @param p The property name/value to set.
 * @return Gaudi status code.
 *
 * The property service calls this method when it actually makes a property
 * setting.  We override it in order to hook our own code into the process.
 * If this property corresponds to one of our declared
 * constants, we install it in the @c ToolConstants structure.
 * If needed, we also complete initialization of the declared
 * variable here.
 *
 * The class using this must override this method, and call first
 * @c AlgTool::setProperty
 * followed by @c ToolWithConstantsMixin::setProperty.
 * You may need to use a using directive to prevent warnings
 * (like above, but using the other base class).
 *
 * This version is used by Gaudi as of v19.
 */
StatusCode
ToolWithConstantsMixin::setProperty (const Gaudi::Details::PropertyBase& p)
{
  return CaloRec::ToolWithConstantsMixin::setProperty (p.name(), p.toString());
}



/**
 * @brief Add a new constant to our store.
 * @param pi The @c Propinfo instance describing this constant.
 */
void ToolWithConstantsMixin::add_constant (Propinfo_Base* pi)
{
  if (!pi)
    return;

  // Make sure that a name isn't duplicated.
  for (unsigned int i=0; i < m_infos.size(); i++)
    if (m_infos[i]->m_name == pi->m_name) {
      REPORT_ERROR(StatusCode::FAILURE)
        << "Duplicate constant: " << pi->m_name;
      delete pi;
      return;
    }

  // Record this declaration in @c m_infos.
  // @c makeinfo is one of an overloaded family of functions that
  // creates the @c Propinfo_* structure for the type of @a c.
  m_infos.push_back (pi);
}


/**
 * @brief Constructor.
 * @param name The name of the constant.
 */
ToolWithConstantsMixin::Propinfo_Base::Propinfo_Base (const std::string& name,
                                                      bool deflt)
  : m_name (name),
    m_set (false),
    m_setDB (false),
    m_default (deflt)
{
}


/**
 * @brief Destructor.
 *
 * Just so that this class gets a vtable.
 */
ToolWithConstantsMixin::Propinfo_Base::~Propinfo_Base ()
{
}


/**
 * @brief Finish up initialization of this constant.
 * @param rep The @c Arrayrep for this constant.
 * @param tool The @c ToolWithConstants instance, for error reporting.
 *
 * This is called from @c setProperty after the property manager
 * has finished its work, and after we've installed the constant
 * in the @c ToolConstants structure.  We get the
 * @c Arrayrep from the @c ToolConstants for this constant.
 *
 * The action needed here depends on the constant type.
 * This default implementation doesn't do anything.
 */
StatusCode
ToolWithConstantsMixin::Propinfo_Base::set (const Arrayrep& /*rep*/,
                                            ToolWithConstantsMixin& /*tool*/)
{
  return StatusCode::SUCCESS;
}


/**
 * @brief Set a constant from the Database.
 * @param rep The @c Arrayrep for this constant.
 * @param tool The @c ToolWithConstants instance, for error reporting.
 *
 * Called explicitly by the callback function.
 *
 * The action needed here depends on the constant type.
 * This default implementation doesn't do anything.
 */
StatusCode
ToolWithConstantsMixin::Propinfo_Base::setFromDB (const Arrayrep& /*rep*/,
						  ToolWithConstantsMixin& /*tool*/)
{
  return StatusCode::SUCCESS;
}


/**
 * @brief Merge our constants into @c out with the proper prefix.
 * @param[out] out Object to receive our constants.
 */
StatusCode
ToolWithConstantsMixin::mergeConstants (ToolConstants& out) const
{
  if (out.clsname().empty()) {
    out.clsname (m_toolcls);
    out.version (version());
  }
  else if (out.clsname() != m_toolcls) {
    REPORT_ERROR(StatusCode::FAILURE)
      << "Trying to write to ToolConstants for tools of differing types: "
      << out.clsname() << " and " << m_toolcls;
    return StatusCode::FAILURE;
  }
  else if (out.version() != version()) {
    REPORT_ERROR(StatusCode::FAILURE)
      << "Trying to write to ToolConstants for differing versions of tool "
      << out.clsname() << ": " << out.version() << " and " << version();
    return StatusCode::FAILURE;
  }

  for (unsigned int i=0; i < m_infos.size(); i++) {
    // Decide which source block to use.
    const ToolConstants& source = (m_infos[i]->m_setDB ?
                                   *m_DBconstants :
                                   m_constants);
    if (!m_isdummy || m_infos[i]->m_name == "isDummy")
      out.setrep (m_prefix + m_infos[i]->m_name,
                  source.getrep (name(), m_infos[i]->m_name));
  }
  return StatusCode::SUCCESS;
}


/**
 * @brief Return the version number for this tool.
 *
 * A saved set of constants includes both the C++ class name and
 * a version number.  The idea is that the version number can
 * be bumped whenever there's a backwards-incompatible change;
 * this gives some protection against trying to use an old version
 * of a tool with an incompatible newer set of constants.
 *
 * If you want a tool to have a version number, override this method.
 * Otherwise, it will default to a version number of 0.
 */
int ToolWithConstantsMixin::version() const
{
  return 0;
}


void ToolWithConstantsMixin::writeConstants(std::ostream& stream, const std::string& name) const {
  stream << "Member: m_constants" << std::endl;
  m_constants.writeConstants(stream,name);
  if (m_DBconstants) {
    stream << "Member: m_DBconstants" << std::endl;
    m_DBconstants->writeConstants(stream,name);
  }
  //stream << "Member m_COOLconstants" << std::endl;
  //m_COOLconstants->writeConstants(stream,name);
  return;
}

} // namespace CaloRec

