/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file RegistrationStream.cxx
 *
 * @brief Algorithm functioning like an OutputStream which registers a
 * reference to an event, i.e. a POOL Token, in an event
 * collection. Selection criteria may be applied to accept or reject
 * events.
 *
 * $Id: RegistrationStream.cxx,v 1.73 2009-04-28 19:04:06 cranshaw Exp $
 *
 */

//<<<<<< INCLUDES                                                       >>>>>>

#include "RegistrationStream.h"

#include "AthenaKernel/IRegistrationStreamTool.h"
#include "StoreGate/StoreGateSvc.h"
#include "SGTools/DataProxy.h"
#include "SGTools/SGIFolder.h"
#include "PersistentDataModel/DataHeader.h"
#include "StoreGate/DataHandle.h"

// Framework include files
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IIoComponentMgr.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IAddressCreator.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IIncidentSvc.h"

#include "DBDataModel/CollectionMetadata.h"

#include <cassert>
//  #include <iostream>
#include <string>
#include <vector>
#include <map>
#include <typeinfo>

// Standard Constructor
RegistrationStream::RegistrationStream(const std::string& name, 
				 ISvcLocator* pSvcLocator)
    :
    FilteredAlgorithm(name, pSvcLocator), 
    m_2BRegistered(std::string("SG::Folder/")+name+std::string("_TopFolder"),
                  this),
    m_writeInputDH(false),
    m_regTool("RegistrationStreamLCGTool/RegistrationStreamLCGTool",this)
{
  /// FIXME: should revamp the m_xyzSvc / m_xyzName configuration to
  /// FIXME: use all the power of Configurables and ServiceHandles
  /// FIXME: but it require too many client changes, now...
    assert(pSvcLocator);
    declareProperty("ItemList",         m_itemList, 
                    "DataHeader#<Name> and AthenaAttributeList#<Name>" );
    declareProperty("CollectionType",   m_collectionType = "ExplicitROOT", 
                    "ExplicitROOT, ExplicitRAL, ExplicitMySQL, ExplicitMySQLlt"); 
    declareProperty("Connection",       m_connection,
                    "CORAL type URL or alias"); 
    declareProperty("OutputCollection", m_outputCollection="",
                    "Name of collection. NOTE: will append .root for ExplicitROOT");
    declareProperty("CollectionOpenMode", m_openMode="CREATE_AND_OVERWRITE",
                    "READ, CREATE, CREATE_AND_OVERWRITE");
    declareProperty("WriteInputDataHeader", m_writeInputDH,
                    "Bool to tell it to point to the input DataHeader");
    declareProperty("WriteAllProv",     m_writeAllProvFlag=true,
                    "Bool to turn on writing ref's for previous processing stages, i.e. provenance");
    declareProperty("IncludeProvStages",m_provIncludeList,
                    "Explicit list of prov stages to include, e.g. StreamESD. Unnecessary if WriteAllProv.");
    declareProperty("ExcludeProvStages",m_provExcludeList,
                    "Explicit list of prov stages to exclude, e.g. Stream1. Overrides IncludeProvStages");
    declareProperty("StoreName",        m_storeName="StoreGateSvc", "EXPERTS");
    declareProperty("Tool", m_regTool);

    m_itemList.declareUpdateHandler(&RegistrationStream::itemListHandler, this);
    m_provIncludeList.declareUpdateHandler(&RegistrationStream::ProvIncludeHandler, this);
    m_provExcludeList.declareUpdateHandler(&RegistrationStream::ProvExcludeHandler, this);
}

// Standard Destructor
RegistrationStream::~RegistrationStream()   {
}

// initialize data writer
StatusCode 
RegistrationStream::initialize() 
{

    StatusCode status(StatusCode::FAILURE);
    StatusCode baseStatus = this->FilteredAlgorithm::initialize();

    ATH_MSG_DEBUG ("In initialize ");

    // set up the SG service:
    evtStore().setName (m_storeName);
    StatusCode sc = evtStore().retrieve();
    if ( !sc.isSuccess() ) {
      ATH_MSG_ERROR ("Could not locate default store ("<<m_storeName<<")");
      return sc;
    }
    else {
      ATH_MSG_DEBUG ("Found " << m_storeName << " store.");
    }
    assert( evtStore() );

    // get Tool
    status = m_regTool.retrieve();
    if (!status.isSuccess()) {
      ATH_MSG_ERROR (" Can't get IRegistrationStreamTool");
      return(StatusCode::FAILURE);
    } 

    if (!status.isSuccess()) {
      ATH_MSG_ERROR (" Tool initialization failed");
      return(StatusCode::FAILURE);
    } 
    else {
      ATH_MSG_DEBUG (" Tool initialized");
    }

    ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", this->name());
    status = incSvc.retrieve();
    if (!status.isSuccess()) {
       ATH_MSG_ERROR("Cannot get the IncidentSvc");
       return(status);
    } else {
       ATH_MSG_DEBUG("Retrieved IncidentSvc");
    }
    incSvc->addListener(this, "MetaDataStop", 30);
    ATH_MSG_DEBUG("Added MetaDataStop listener");

    // Register this algorithm for 'I/O' events
    ServiceHandle<IIoComponentMgr> iomgr("IoComponentMgr", name());
    status = iomgr.retrieve();
    if (!status.isSuccess()) {
       ATH_MSG_FATAL("Cannot get the IoComponentMgr");
       return(status);
    }
    status = iomgr->io_register(this);
    if (!status.isSuccess()) {
       ATH_MSG_FATAL("Could not register myself with the IoComponentMgr");
       return(status);
    }
    if (!iomgr->io_register(this, IIoComponentMgr::IoMode::WRITE, m_outputCollection).isSuccess()) {
      ATH_MSG_FATAL("Could not register [" << m_outputCollection << "] for output !");
      return StatusCode::FAILURE;
    }

    // Tell the tool which metadata to pick up
    m_regTool->setCollMetadataKeys(getCollMetadataKeys());

    ATH_MSG_DEBUG ("End initialize ");
  
    if (status == StatusCode::FAILURE || baseStatus == StatusCode::FAILURE) return StatusCode::FAILURE;
    return status;
}


// terminate data writer
StatusCode 
RegistrationStream::finalize() 
{
  ATH_MSG_DEBUG ("In finalize");
  unsigned long count = m_regTool->release();
  while (count > 1) {
     ATH_MSG_DEBUG ("Tool has ref count " << count << " after release");
     count = m_regTool->release();
  }
  m_regTool.release();
  return StatusCode::SUCCESS;
}

void RegistrationStream::handle(const Incident& inc) 
{
  // Now commit the results
  ATH_MSG_DEBUG("handle() incident type: " << inc.type());
  if (inc.type() == "MetaDataStop") {
    StatusCode sc = m_regTool->commit();
    if (!sc.isSuccess()) ATH_MSG_INFO("Unable to commit");

  }
}


// Work entry point
StatusCode 
RegistrationStream::execute() 
{
    // Here we add to a collection a set of refs built from the
    // DataHeaders and the tag as an AttributeList. The DataHeaders
    // and AthenaAttributeList are all specified in the itemList.

    ATH_MSG_DEBUG ("RegistrationStream execute");

    StatusCode sc(StatusCode::SUCCESS);
 
    // Check whether this passes the selection algorithms
    if ( isEventAccepted() ) {

	// Get tokens specified by item list
	std::vector<std::pair<std::string, std::string> > refs;

        // Check whether any references are available to store
        //
        if (getRefs(refs) == StatusCode::FAILURE) {
	  ATH_MSG_WARNING
	    ("No refs found: RegStream selected to write out ref to event "
	     << endmsg
	     << "   but DataHeader can NOT be found in StoreGate. "
	     << endmsg
	     << "   Most likely the event was NOT selected to be written by "
	     << "an output stream ");
            return sc;
        }
        
        //sc = m_regTool->fill(refs,inputRefs,refnames,this->getAttListKey());
        sc = m_regTool->fill(refs,this->getAttListKey());
    
    } // end of isEventAccepted clause
    else { ATH_MSG_DEBUG ("Event rejected by filtering");}
    
    return sc;
}

void
RegistrationStream::itemListHandler( Property& /* theProp */ )
{
  //assuming concrete SG::Folder also has an itemList property
  IProperty *pAsIProp(0);
  if ((m_2BRegistered.retrieve()).isFailure() ||
      0 == (pAsIProp=dynamic_cast<IProperty*>(&*m_2BRegistered)) ||
      (pAsIProp->setProperty(m_itemList)).isFailure())
    throw GaudiException("Could not set itemList property", name(),
                        StatusCode::FAILURE);
}

void
RegistrationStream::ProvIncludeHandler( Property& )
{
  std::vector<std::string> list = m_provIncludeList.value();
  std::vector<std::string>::const_iterator it = list.begin();
  while (it != list.end() ) {
    ATH_MSG_DEBUG ("Including " << *it);
    if (it->empty() != true) m_provInclude.insert(*it);
    ++it;
  }
}

void
RegistrationStream::ProvExcludeHandler( Property& )
{
  std::vector<std::string> list = m_provExcludeList.value();
  std::vector<std::string>::const_iterator it = list.begin();
  while (it != list.end() ) {
    ATH_MSG_DEBUG ("Excluding " << *it);
    if (it->empty() != true) m_provExclude.insert(*it);
    ++it;
  }
}

/// Get refs for an item list
StatusCode RegistrationStream::getRefs(std::vector< std::pair<std::string,std::string> >& refs)
{
    //
    // Find refs and inputRefs:
    //   refs      - contains the primary reference
    //   inputRefs - contains the secondary references
    //
    // The current logic is:
    //	
    //   m_writeInputDH is either true/false meaning that we either
    //   want to write the output DataHeader(s) or the input
    //   DataHeader(s)
    //
    //   m_writeInputDH - false (default)
    //     look for the "first" output DataHeader and add its token to refs
    //   
    //   m_writeInputDH - true
    //     find the first/primary input DataHeader and add its token to refs
    //
    //   In either case, we add to inputRefs the token for the first
    //   DataHeader found within the input DataHeader's list for back
    //   navigation
    //
    //   DataHeader to be found is specified in input ItemList which
    //   is a Folder (m_2BRegistered) in this method.
    //
    //   Current logic is to take the first DataHeader matching one
    //   specified in the input ItemList. Keys may either be explicit or '*'. 
    //
    //   As soon as requested ref is found return
       
    // Collect all objects that need to be persistified:
    SG::IFolder::const_iterator i(m_2BRegistered->begin()); 
    SG::IFolder::const_iterator iEnd(m_2BRegistered->end()); 
    for (; i != iEnd; ++i) {

	// Only allow DataHeaders for the moment to be inserted as refs
	if (ClassID_traits<DataHeader>::ID() != i->id()) {
	  ATH_MSG_DEBUG ("getRefs: Not DataHeader, skipping ("<< i->id()
			 << ",\"" << i->key() << "\") ");
	  continue;
	}
	else {
	  ATH_MSG_DEBUG (" get ref for ("<< i->id() << ",\""
			 << i->key() << "\") ");
	}

	if (i->isFolder()) {
	  ATH_MSG_ERROR ("Cannot currently treat folders in item list.");
	  return(StatusCode::FAILURE);
	}

	// rkey is i->key() except for i->key()="*"
	std::string rkey = i->key(); 

	// Retrieve DataHeader to check if it is input
	const DataHeader* hdr=0;
	if (i->key() == "*") {
            //ATH_MSG_DEBUG("xxx" << evtStore()->dump());
	    // For wildcard key, must go and fetch all DataHeaders  
	    const DataHandle<DataHeader> beg; 
	    const DataHandle<DataHeader> ending; 
	    StatusCode status = evtStore()->retrieve(beg,ending);
	    if (status.isFailure() || beg==ending) {
	      ATH_MSG_WARNING ("No DataHeaders present in StoreGate");
	      //return  StatusCode::FAILURE;
	    }
	    else {
		// go to next if wrong type
		for (; beg != ending; ++beg) {
		    if (m_writeInputDH == beg->isInput()) {
			hdr = beg;
			rkey = beg.key();
			ATH_MSG_DEBUG ("Retrieved DataHeader with key "
				       << rkey 
				       << " for requested key=*");
			break;
		    }
		}
	    }
	}
	else {	  // i.e. key is not "*"
	    StatusCode status = StatusCode::FAILURE;
            if  (evtStore()->contains(ClassID_traits<DataHeader>::ID(),rkey)) {
	        status = evtStore()->retrieve(hdr, rkey);
	    }
	    if (status.isFailure()) {
	      ATH_MSG_DEBUG ("Could not retrieve DataHeader with key "
			     << rkey 
			     << " Will try next in item list.");
	      continue;
	    }
	    else {
		// go to next if wrong type
		if (hdr && m_writeInputDH == hdr->isInput()) {
		  ATH_MSG_DEBUG ("Retrieved DataHeader with key " << rkey);
		}
		else {
		  ATH_MSG_DEBUG ("Could not retrieve DataHeader with key "
				 << rkey 
				 << " Will try next in item list.");
		  continue;
		}
	    }
	}
	
	// Check that dataheader ptr is valid before going further
        if (hdr==0) {
	  ATH_MSG_WARNING ("Did not find requested DataHeader in Storegate");
	  continue;
	}
        
        // Now look for the processtag/name of the DataHeader
        std::string ptag("Unknown");
	std::string ref("");
        // if hdr has process tag, take that as name
        if (hdr->begin()==hdr->end()) {
           if (hdr->getProcessTag().size()>0) ptag = hdr->getProcessTag();
           const Token* token = hdr->begin()->getToken();
	   ref = token != 0 ? token->toString() : "";
        }
        // or take the key of the DataHeaderElement for the DataHeader
        else if (hdr->begin()!=hdr->end()) {
           std::vector<DataHeaderElement>::const_iterator dheit = hdr->begin();
           while (dheit != hdr->end() ) {
              if (
                   dheit->getPrimaryClassID()==CLID(222376821) &&
                   dheit->getKey().size()>0
                 ) { 
                      ptag = dheit->getKey();
                      const Token* token = dheit->getToken();
	              ref = token != 0 ? token->toString() : "";
                   }
              ++dheit;
           }
        }
        // or issue warning and take default
        else {
	  ATH_MSG_WARNING 
	    ("DataHeader with key " << rkey << " has empty processtag");
        }
	
        // Update ref token to handle fast merged files.
        SG::DataProxy* dhProxy = evtStore()->proxy(hdr);
        if (dhProxy != 0 && dhProxy->address() != 0) {
          ref  = dhProxy->address()->par()[0];
        }

        ATH_MSG_DEBUG("Pushing back ref " << ref << " for " << ptag);
	refs.push_back(std::make_pair(ptag,ref));

	// Get a single input header ref if requested
	if (hdr->sizeProvenance()) {
            // get pointers to begin and end
	    std::vector<DataHeaderElement>::const_iterator it = hdr->beginProvenance();
	    std::vector<DataHeaderElement>::const_iterator last = hdr->endProvenance();
            // grab refs for those
	    for (; it!=last; ++it) {
                const Token* token = (*it).getToken();
	        const std::string ref = token != 0 ? token->toString() : "";
	        ATH_MSG_DEBUG ("Found ref for input header " << ref);
                bool inselected = m_writeAllProvFlag.value() || 
                                  m_provInclude.find(it->getKey()) != m_provInclude.end();
                bool outselected = m_provExclude.find(it->getKey()) != m_provExclude.end();
                if ( inselected && !outselected )
	        {
                  ATH_MSG_DEBUG ("Adding ref for " << it->getKey());
                  refs.push_back(std::make_pair(it->getKey(),ref));
                }
	    }
	}
	// As soon as it finds a valid ref, exit
	if (refs.size()>0) return(StatusCode::SUCCESS);
    }
    // Should only get to this point if ref.size == 0 for full itemlist
    return(StatusCode::FAILURE);
}

//------------------------------------------------------------
// getAttListKey()
//
// This method is placed in the Stream rather than the Tool 
//   simply because it requires parsing the itemlist
//
//------------------------------------------------------------
std::string RegistrationStream::getAttListKey()
{
    std::string tagKey("notthere");

    // Collect all objects that need to be persistified:
    SG::IFolder::const_iterator i(m_2BRegistered->begin());
    SG::IFolder::const_iterator iEnd(m_2BRegistered->end());
    for (; i != iEnd; ++i) {

        // Select from item list only the List requested by Tool
        if (m_regTool->listID() != i->id()) {
	  ATH_MSG_DEBUG (" Not class requested by Tool, skipping ("<< i->id()
			 << ",\"" << i->key() << "\") ");
	  continue;
        }

        if (i->isFolder()) {
	  ATH_MSG_ERROR ("Cannot currently treat folders in item list.");
	  //return(StatusCode::FAILURE);
        }

        tagKey = i->key();
        ATH_MSG_DEBUG ("Setting tagKey to " << tagKey);

    }
    return tagKey;
}

//------------------------------------------------------------
// getCollMetadataKeys()
//
// This method is placed in the Stream rather than the Tool 
//   simply because it requires parsing the itemlist
//
//------------------------------------------------------------
std::vector<std::string> RegistrationStream::getCollMetadataKeys()
{
    std::vector<std::string> cmdKeys;

    // Collect all objects that need to be persistified:
    SG::IFolder::const_iterator i(m_2BRegistered->begin());
    SG::IFolder::const_iterator iEnd(m_2BRegistered->end());
    for (; i != iEnd; ++i) {

        // Select from item list only the List requested by Tool
        if (ClassID_traits<CollectionMetadataContainer>::ID() != i->id()) {
	  ATH_MSG_DEBUG (" Not class requested by Tool, skipping ("<< i->id()
			 << ",\"" << i->key() << "\") ");
	  continue;
        }

        if (i->isFolder()) {
	  ATH_MSG_ERROR ("Cannot currently treat folders in item list.");
	  //return(StatusCode::FAILURE);
        }

        cmdKeys.push_back(i->key());
        ATH_MSG_DEBUG ("Adding cmd key " << i->key());

    }
    return cmdKeys;
}

StatusCode RegistrationStream::io_reinit() 
{
   ATH_MSG_DEBUG("I/O reinitialization...");
   ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", this->name());
   if (!incSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get the IncidentSvc");
      return StatusCode::FAILURE;
   }
   incSvc->addListener(this, "MetaDataStop", 30);
   return StatusCode::SUCCESS;
}

StatusCode RegistrationStream::io_finalize() {
   ATH_MSG_INFO("I/O finalization...");
   const Incident metaDataStopIncident(name(), "MetaDataStop");
   this->handle(metaDataStopIncident);
   ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", this->name());
   if (!incSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get the IncidentSvc");
      return StatusCode::FAILURE;
   }
   incSvc->removeListener(this, "MetaDataStop");
   return StatusCode::SUCCESS;
}
