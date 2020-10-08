/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file RegistrationStream.cxx
 *
 * @brief Algorithm functioning like an OutputStream which registers a
 * reference to an event, i.e. a POOL Token, in an event
 * collection. Selection criteria may be applied to accept or reject
 * events.
 *
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
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IIoComponentMgr.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IAddressCreator.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/GaudiException.h"

#include "DBDataModel/CollectionMetadata.h"

#include <cassert>
#include <string>
#include <vector>
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

    ATH_CHECK( this->FilteredAlgorithm::initialize() );

    // set up the SG service:
    evtStore().setName (m_storeName);
    ATH_CHECK( evtStore().retrieve() );
    ATH_MSG_DEBUG ("Found " << m_storeName << " store.");

    // get Tool
    ATH_CHECK( m_regTool.retrieve() );

    // Register this algorithm for 'I/O' events
    ServiceHandle<IIoComponentMgr> iomgr("IoComponentMgr", name());
    ATH_CHECK( iomgr.retrieve() );
    ATH_CHECK( iomgr->io_register(this) );

    if (iomgr->io_register(this, IIoComponentMgr::IoMode::WRITE, m_outputCollection).isFailure()) {
      ATH_MSG_FATAL("Could not register [" << m_outputCollection << "] for output !");
      return StatusCode::FAILURE;
    }

    // Tell the tool which metadata to pick up
    m_regTool->setCollMetadataKeys(getCollMetadataKeys());

    return StatusCode::SUCCESS;
}


StatusCode
RegistrationStream::stop() 
{
  if (m_regTool->commit().isFailure()) ATH_MSG_INFO("Unable to commit");
  return StatusCode::SUCCESS;
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
        
        sc = m_regTool->fill(refs,this->getAttListKey());
    
    } // end of isEventAccepted clause
    else { ATH_MSG_DEBUG ("Event rejected by filtering");}
    
    return sc;
}

void
RegistrationStream::itemListHandler( Gaudi::Details::PropertyBase& /* theProp */ )
{
  //assuming concrete SG::Folder also has an itemList property
  IProperty *pAsIProp(nullptr);
  if ((m_2BRegistered.retrieve()).isFailure() ||
      nullptr == (pAsIProp=dynamic_cast<IProperty*>(&*m_2BRegistered)) ||
      (pAsIProp->setProperty(m_itemList)).isFailure())
    throw GaudiException("Could not set itemList property", name(),
                        StatusCode::FAILURE);
}

void
RegistrationStream::ProvIncludeHandler( Gaudi::Details::PropertyBase& )
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
RegistrationStream::ProvExcludeHandler( Gaudi::Details::PropertyBase& )
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
	const DataHeader* hdr=nullptr;
	if (i->key() == "*") {
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
	    if (hdr==nullptr) {
	        if (evtStore()->retrieve(hdr, "EventSelector").isFailure()) {
	          ATH_MSG_DEBUG ("Could not retrieve DataHeader with key EventSelector.");
	          continue;
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
        if (hdr==nullptr) {
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
	   ref = token != nullptr ? token->toString() : "";
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
	              ref = token != nullptr ? token->toString() : "";
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
        if (dhProxy != nullptr && dhProxy->address() != nullptr) {
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
	        const std::string ref = token != nullptr ? token->toString() : "";
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
