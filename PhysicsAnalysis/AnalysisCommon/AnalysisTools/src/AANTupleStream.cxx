/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AnalysisTools/AANTupleStream.h"
#include "AnalysisTools/AANTupleParams.h"

#include "AnalysisUtils/AANTTreeMap.h"

#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IAddressCreator.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/ServiceHandle.h"

#include "GaudiKernel/IIoComponentMgr.h"
#include "StoreGate/StoreGateSvc.h"
#include "SGTools/DataProxy.h"
#include "PersistentDataModel/DataHeader.h"
#include "StoreGate/DataHandle.h"

#include "RootCollection/AttributeListLayout.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/AttributeListSpecification.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "xAODEventInfo/EventInfo.h"

#include "TFile.h"
#include "TTree.h"
#include "TText.h"
#include "TBranch.h"
#include "TInterpreter.h"

#include <map>
#include <string>
#include <algorithm>

using namespace AANTupleParams;

// Standard Constructor
AANTupleStream::AANTupleStream(const std::string& name, ISvcLocator* pSvcLocator) 
  : AthLegacySequence(name, pSvcLocator),
    m_persSvc      ("EventPersistencySvc", name),
    m_attribSpec(0),
    m_schemaDone(false),
    m_tree(0),
    m_tokenBranch(0),
    m_tHistSvc     ("THistSvc", name)
{
  declareProperty("OutputName",           m_fileName="AANTuple.root");
  declareProperty("ExtraRefNames",        m_extraRefNames);
  declareProperty("WriteInputDataHeader", m_writeInputDH=false);
  declareProperty("ExistDataHeader",      m_existDH=true);
  declareProperty("StreamName",           m_streamName=c_streamName);
  declareProperty("LateSchemaWriting",    m_lateSchema=false);
  declareProperty("TreeName",             m_treeName=c_treeName);
  declareProperty("Macro",                m_macro="");
  declareProperty("Members",              m_membersNames);
  declareProperty("FilterAlgs",           m_acceptNames);

  m_tokenCString[0] = '\0';
}


// Standard Destructor
AANTupleStream::~AANTupleStream()  
{
}


// initialize data writer
StatusCode AANTupleStream::initialize() 
{
  ATH_MSG_DEBUG ("In initialize ");
  // register with the I/O component manager
  {
    ServiceHandle<IIoComponentMgr> iomgr ("IoComponentMgr", this->name());
    if ( !iomgr.retrieve().isSuccess() ) {
      ATH_MSG_ERROR ("Could not retrieve IIoComponentMgr/IoComponentMgr !");
      return StatusCode::FAILURE;
    }
    if ( !iomgr->io_register (this).isSuccess() ) {
      ATH_MSG_ERROR ("Could not register with the I/O component mgr !");
      return StatusCode::FAILURE;
    }
    if ( !iomgr->io_register (this,
			      IIoComponentMgr::IoMode::WRITE,
			      m_fileName).isSuccess() ) {
      ATH_MSG_ERROR ("Could not register [" << this->name() << "]["
		     << m_fileName << "] with the I/O component manager !");
      return StatusCode::FAILURE;
    }
  }

  // StoreGateSvc
  StatusCode sc = evtStore().retrieve();
  if ( !sc.isSuccess() )
    {
      ATH_MSG_ERROR ("Could not locate default store");
      return sc;
    }

  // set up the persistency service:
  sc = m_persSvc.retrieve();
  if ( !sc.isSuccess() )
    {
      ATH_MSG_ERROR ("Could not locate persistency service");
      return sc;
    }

  // instantiate AttrListSpec and add attributes for Run# and Event# 
  m_attribSpec = new coral::AttributeListSpecification;
  m_attribSpec->extend( name_RunNumber,   "unsigned int" );
  m_attribSpec->extend( name_EventNumber, "unsigned int" );

  // Add on specification for extra refs
  std::vector<std::string> extraRefs = m_extraRefNames.value();
  for (unsigned int i = 0; i < extraRefs.size(); ++i)
    {
      // Append _ref to name of attribute
      std::string name = extraRefs[i] + "_ref";
      m_attribSpec->extend(name, "string");
    }

  if (!m_lateSchema) {
    sc = initSchema();
    if (sc.isFailure())
      return sc;
  }

  // initialize sub-algos
  sc = initialize_subAlgos();
  if ( sc.isFailure() )
    {
      ATH_MSG_ERROR ("Could not initialize sub-algos");
      return sc;
    }      

  // get filters
  sc = getFilters();
  if ( sc.isFailure() )
    {
      ATH_MSG_ERROR ("Could not get filters");
      return sc;
    }      

  ATH_MSG_DEBUG ("End initialize ");

  return AthLegacySequence::initialize();
}


// initialize output collection
StatusCode AANTupleStream::initCollection() 
{
  // write schema
  writeAttributeListSpecification();

  // setup TTree
  setupTree();

  // setup Token
  m_tokenBranch = m_tree->GetBranch(c_tokenBranchName);
  m_tokenBranch->SetAddress(m_tokenCString);

  return StatusCode::SUCCESS;
}


// terminate data writer
StatusCode AANTupleStream::finalize() 
{
  ATH_MSG_INFO ("finalize");

  // exec macro
  if (m_macro != "")
    {
      // save current dir
      std::string curDir = gDirectory->GetPath();
      // go to the root dir of output file
      gDirectory->cd((m_fileName+":/").c_str());
      // exec
      // G__exec_tempfile(m_macro.c_str());
      // MN: that should be the equivalent new call:
      gInterpreter->ExecuteMacro(m_macro.c_str());
      // back to the current dir
      gDirectory->cd(curDir.c_str());
    }

  return AthLegacySequence::finalize();
}


// Work entry point
StatusCode AANTupleStream::execute() 
{
  StatusCode sc;

  if (!m_schemaDone) {
    sc = initSchema();
    if (sc.isFailure())
      return sc;
  }

  // execute sub-algos
  sc = execute_subAlgos();
  if ( sc.isFailure() )
    {
      ATH_MSG_ERROR ("Could not execute sub-algos");
      return sc;
    }      

  std::map<std::string,std::string> inputRefs;
  std::string ref;

  // Get tokens specified by item list
  if (m_existDH)
    {
      // get token
      const DataHandle<DataHeader> beg; 
      const DataHandle<DataHeader> ending; 
      StatusCode status = evtStore()->retrieve(beg,ending);
      if (status.isFailure() || beg==ending)
	{
	  ATH_MSG_WARNING ("No DataHeaders present in StoreGate");
	  return  StatusCode::FAILURE;
	}

      // look for header
      for (; beg != ending; ++beg)
	if (m_writeInputDH == beg->isInput()) break;

      if (beg==ending)
	{
	  ATH_MSG_WARNING ("No appropriate DataHeader present in StoreGate");
	  return  StatusCode::FAILURE;
	}

      // get the ref
      while (getRef(ClassID_traits<DataHeader>::ID(), beg.key(), ref) == StatusCode::FAILURE)
	{
	  ++beg;
	  if (beg==ending)
	    {
	      ATH_MSG_ERROR ("No refs found");
	      return StatusCode::FAILURE;
	    }
	}

      ATH_MSG_DEBUG ("Found token: Key=" << beg.key() << ", Ref=" << ref);

      // Get a single input header ref if requested
      const DataHeader* hdr=0;
      hdr = beg;
      if (hdr->sizeProvenance())
	{
	  std::vector<DataHeaderElement>::const_iterator it =
	    (const_cast<DataHeader*>(hdr)->beginProvenance());
	  std::vector<DataHeaderElement>::const_iterator itE = 
	    (const_cast<DataHeader*>(hdr)->endProvenance());
	  // grab refs for those
	  for (; it!=itE; ++it)
	    {
	      std::string tmpRef = (*it).getToken() != NULL
		? (*it).getToken()->toString()
		: "";
	      inputRefs.insert(std::make_pair(it->getKey(),tmpRef));
	      ATH_MSG_DEBUG ("Found ref for input header: Key=" << it->getKey()
			     << ", Ref=" << tmpRef);
	    }
	}
    }
  
  // create Attrlist, add attributes and if extra ref is requested, append to the end

  ATH_MSG_DEBUG ("create AttributeList");
  // AthenaAttributeList  newAttr(*m_attribSpec);
  coral::AttributeList newAttr(*m_attribSpec);

  unsigned int runNumber   = 0;
  unsigned int eventNumber = 0;

  // retrieve event info
  const EventInfo* eventInfo;
  eventInfo = evtStore()->tryConstRetrieve<EventInfo>();
  if ( !eventInfo )
    {
      // Try to get the xAOD::EventInfo
      const xAOD::EventInfo* eventInfoX{nullptr};
      sc = evtStore()->retrieve(eventInfoX);
      if (sc.isFailure())
        {
          ATH_MSG_ERROR ("Cannot get event info.");
          return sc;
        }
      else
        {
          runNumber   = eventInfoX->runNumber();
          eventNumber = eventInfoX->eventNumber();
        }
    }
  else
    {
      runNumber   = eventInfo->event_ID()->run_number();
      eventNumber = eventInfo->event_ID()->event_number();
    }

  // add global event tag data
  newAttr[ name_RunNumber   ].setValue( runNumber );
  newAttr[ name_EventNumber ].setValue( eventNumber );

  // add back nav ref's
  ATH_MSG_DEBUG ("Append extra ref");
  std::vector<std::string> extraRefs = m_extraRefNames.value();
  std::map<std::string,std::string>::iterator itRef  = inputRefs.begin();
  std::map<std::string,std::string>::iterator itRefE = inputRefs.end();
  for (; itRef != itRefE; ++itRef)
    {
      std::string name = itRef->first + "_ref";
      if (extraRefs.end() != std::find(extraRefs.begin(),extraRefs.end(),itRef->first))
	{
	  newAttr[name].setValue(std::string(itRef->second));
	  ATH_MSG_DEBUG (" Added extra ref to new attribute list - name: " 
			 << name << " " << itRef->second);
	}
    }

  // write if filters passed
  if (isEventAccepted())
    {
      ATH_MSG_DEBUG ("write attributes to collection");
  
      // write token plus attributes
      try
	{
	  writeTokenAttrList(ref, newAttr);
	} 
      catch (std::exception &e)
	{
	  ATH_MSG_ERROR 
	    ("Caught exception from collection add of Pool attributes. "
	     "Message: " << e.what());
	  return StatusCode::FAILURE;
	}
    }
  
  ATH_MSG_DEBUG ("done successfully");

  return StatusCode::SUCCESS;
}

  
void AANTupleStream::writeAttributeListSpecification()
{
  // go to the root dir of output file
  TDirectory::TContext save;
  gDirectory->cd((m_fileName+":/").c_str());

  AttributeListLayout all;
  // store schema in the schema object
  for( coral::AttributeListSpecification::const_iterator iter = m_attribSpec->begin();
       iter != m_attribSpec->end(); ++iter) {
     all.m_layout.push_back( make_pair( iter->name(), iter->typeName() ) );
  }
  // write schema object
  all.Write(c_attributeListLayoutName);
}


void AANTupleStream::setupTree()
{
  static std::map<std::string,char> typeDict;
  if (!typeDict.size())
    {
      typeDict["double"] = 'D';   
      typeDict["long double"] = 'D';    // only 64 bit doubles are supported 
      typeDict["float"] = 'F';
      typeDict["int"] = 'i';
      typeDict["unsigned int"] = 'I';
      typeDict["long"] = 'i';           // support for 64 bit integers introduced with ROOT 4.00/08 
      typeDict["unsigned long"] = 'I';  // support for 64 bit integers introduced with ROOT 4.00/08 
      typeDict["short"] = 's';
      typeDict["unsigned short"] = 'S';
      typeDict["char"] = 'b';           // AttributeLists's "get_root_type_char(...)" returned 'C' ??? 
      typeDict["unsigned char"] = 'B';  // AttributeLists's "get_root_type_char(...)" returned 's' ???
      typeDict["bool"] = 'B';           // AttributeLists's "get_root_type_char(...)" returned 'i' ???
      typeDict["string"] = 'C';
      typeDict["Token"] = 'C';
    }

  // go to the root dir of output file
  TDirectory::TContext save;
  gDirectory->cd((m_fileName+":/").c_str());
  
  // instantiate TTree
  m_tree = new TTree(m_treeName.c_str(), m_treeName.c_str());

  // add branches for attributes
  for(coral::AttributeListSpecification::const_iterator iter=m_attribSpec->begin(); iter!=m_attribSpec->end(); ++iter)
    {
      std::string type = "/?";
      type[1] = typeDict[iter->typeName()];
      std::string leaflist = iter->name() + type;
      m_tree->Branch(iter->name().c_str(),0,leaflist.c_str());
    }

  // add Token branch
  std::string leaflist = c_tokenBranchName;
  leaflist += "/C";
  m_tree->Branch(c_tokenBranchName,0,leaflist.c_str());

  // set TTree
  AANTTreeMap::setTree(m_streamName,m_tree);
}


// Get the string form of a ref for a given clid and key
StatusCode AANTupleStream::getRef(CLID id, const std::string& key, std::string& ref)
{
  // Must access the SG proxy to get the IOpaqueAddress
  SG::DataProxy* itemProxy = 0;
  ATH_MSG_DEBUG ("Retrieving proxy for " << id << " " << key);
  itemProxy = evtStore()->proxy(id, key);
  if (!itemProxy)
    {
      ATH_MSG_ERROR ("Could not get proxy ");
      return(StatusCode::FAILURE);
    }
  else
    {
      ATH_MSG_DEBUG (" found proxy for ("<< id << ",\"" << key << "\") ");
    }
  
  if (!itemProxy)
    {
      ATH_MSG_ERROR ("Empty proxy ");
      return(StatusCode::FAILURE);
    }
  
  // Get IOpaqueAddress from the proxy
  IOpaqueAddress* addr = itemProxy->address();
  if (!addr)
    {
      ATH_MSG_DEBUG ("Could not get address for clid "
		     << itemProxy->clID() << " " << itemProxy->name());
      return( StatusCode::FAILURE);
    }

  // Convert IOpaqueAddress to string via the persistency
  // service. And then remove header to extract the token.
  std::string saddr;

  StatusCode sc = m_persSvc->convertAddress(addr, saddr);   
  if (sc.isFailure()) 
    {
      ATH_MSG_ERROR 
	("Could not get string from IOpaqueAddress for clid " << id
	 << " " << key
	 << " is BAD_STORAGE_TYPE: "<< (sc == IConversionSvc::Status::BAD_STORAGE_TYPE));
      return( StatusCode::FAILURE);
    }
		    

  // We assume here some knowledge of the form of the AthenaPool
  // string address
  std::string hdr;
  sc = splitAddress(saddr, " /> ", hdr, ref);
  if (sc.isFailure()) 
    {
      ATH_MSG_ERROR ("Could not get split off token. string address " << saddr);
      return( StatusCode::FAILURE);
    }
  
  ATH_MSG_DEBUG ("String address " << ref);

  return StatusCode::SUCCESS;
}


// Extract the address header and data from an IOpaqueAddress in
// string form
StatusCode AANTupleStream::splitAddress( const std::string& address,
					 const std::string& match,
					 std::string&       address_header,
					 std::string&       address_data ) const
{
  ATH_MSG_DEBUG ("splitAddress " << endmsg
		 << "   address :  " << address
		 << "   match :  " << match);
  
  std::string::size_type p1;

  p1 = address.find( match );
  if (p1 != std::string::npos) 
    {
      address_header = address.substr( 0, p1+match.size());
      
      address_data = address.substr( p1+match.size() );
    } 
  else 
    {
      return StatusCode::FAILURE;
    }

  ATH_MSG_DEBUG ("   address_header :  " << address_header
		 << endmsg
		 << "   address_data   :  " << address_data);

  return StatusCode::SUCCESS;
}


// write Token and Attributes to the output collection
bool AANTupleStream::writeTokenAttrList( const std::string& token, const coral::AttributeList& attributeList )
{
  // set addresses for attributes
  coral::AttributeList::const_iterator attIter = attributeList.begin();
  const TObjArray* branches = m_tree->GetListOfBranches();
  std::vector<std::string> tokenStringDump;
  // loop over all attributes
  for (unsigned int i=0; i<attributeList.size(); ++i)
    {
      TBranch* branch = (TBranch*)branches->UncheckedAt(i);
      // string attributes
      if (attIter->specification().type() == typeid(std::string))
	{
	  const std::string& str = attIter->data<std::string>();
	  if( str.length()+1 >= s_maxLengthOfStrings )
	    {
	      throw std::runtime_error("String is too long : RootCollection::pool2Root<std::string>"); 
	    }
	  branch->SetAddress( (void*)str.c_str() );
	}
      // Token
      else if(attIter->specification().typeName() == "Token")
	{
	  tokenStringDump.push_back( attIter->data<std::string>() );
	  branch->SetAddress( (void*) tokenStringDump.back().c_str() );
	}
      // else
      else
	{
	  branch->SetAddress( (void*)attIter->addressOfData() );
	}
      ++attIter;  
    }

  // set address for Token
  m_tokenBranch->SetAddress((void*)token.c_str());

  // fill TTree
  m_tree->Fill();

  // releases Token
  m_tokenBranch->SetAddress(m_tokenCString); 

  return true;
}


// initialize sub-algos
StatusCode AANTupleStream::initialize_subAlgos()
{
  StatusCode sc = StatusCode::SUCCESS;
  
  Algorithm* algo;
  std::vector<std::string>::const_iterator it;
  std::vector<std::string>::const_iterator itend = m_membersNames.end( );
  for (it = m_membersNames.begin(); it != itend; ++it)
    {
      // Parse the name for a syntax of the form:
      //
      // <type>/<name>
      //
      // Where <name> is the algorithm instance name, and <type> is the
      // algorithm class type (being a subclass of Algorithm).
      ListItem foo(*it);
      std::string theType = foo.type();
      std::string theName = foo.name();

      // create sub-algorithm
      ATH_MSG_INFO (" -> creating sub-algorithm " << (*it));
      sc =  createSubAlgorithm( theType,theName, algo );
      if (sc.isFailure())
	{
	  ATH_MSG_FATAL (" ERROR creating sub-alg." << (*it));
	  return StatusCode::FAILURE;
	}

      // force subAlgorithm to set his properties now (reading the jobOptions
      sc = algo->setProperties();
      if (sc.isFailure())
	{
	  ATH_MSG_FATAL (" ERROR setting properties for this sub-algorithm.");
	  return StatusCode::FAILURE;
	}
    }

  return sc;
}


// execute sub-algos
StatusCode AANTupleStream::execute_subAlgos()
{
  StatusCode sc = StatusCode::SUCCESS;
  
  ATH_MSG_DEBUG ("in execute_subAlgos() ...");

  const EventContext& ctx = Gaudi::Hive::currentContext();
  
  // -- run subalgorithms
  for ( unsigned int i=0; i < m_membersNames.size(); ++i )
    {
      ATH_MSG_DEBUG (" -> calling sub-algorithm " << m_membersNames[i]);
      // skip disabled algo
      if (! (*(this->subAlgorithms()))[i]->isEnabled()) continue ;
      
      sc = (*(this->subAlgorithms()))[i]->execute(ctx);
      if ( sc.isFailure())
	{
	  ATH_MSG_ERROR
	    (" ERROR executing sub-algorithm:" << m_membersNames[i]);
	  return StatusCode::FAILURE;
	}
    }
  return sc;
}


// get filters
StatusCode AANTupleStream::getFilters()
{
  ATH_MSG_DEBUG ("in getFilters()");
  
  // no filter
  if (m_acceptNames.empty())
    return StatusCode::SUCCESS;



  // old way to get ApplicationMgr crash with gaudi 21 (dynamic_cast fails)
  //  IAlgManager* theAlgMgr;
//   IInterface* ptmp;
//   StatusCode sc = serviceLocator()->getService("ApplicationMgr",
//                                                IAlgManager::interfaceID(),
//                                                ptmp);

//   if (sc.isFailure())
//     {
//       ATH_MSG_FATAL ("Can't locate ApplicationMgr!!!");
//       return sc;
//     }
//   theAlgMgr = dynamic_cast<IAlgManager*> (ptmp);

  // new safe way to get ApplicationMgr (gaudi 21 and backward compatible)
  ServiceHandle<IAlgManager> theAlgMgr("ApplicationMgr", this->name());
  if (theAlgMgr==0){
    ATH_MSG_FATAL ("Can't locate    ApplicationMgr!!!");
    return StatusCode::FAILURE;
  }
  

  // loop over all alg names
  std::vector<std::string>::const_iterator it;
  std::vector<std::string>::const_iterator itend = m_acceptNames.end();
  for (it = m_acceptNames.begin(); it != itend; ++it)
    {
      IAlgorithm* ialg;
      // get Alg
      StatusCode sc = theAlgMgr->getAlgorithm(*it, ialg);

      if (sc.isFailure() )
	{
	  ATH_MSG_ERROR ("Can't get Filter Alg : " << *it);
	  return StatusCode::FAILURE;
	}
      Algorithm *theAlg = dynamic_cast<Algorithm*>(ialg);
      if (theAlg==0 )
	{
	  ATH_MSG_ERROR ("Can't cast Filter Alg : " << *it);
	  return StatusCode::FAILURE;
	}
      ATH_MSG_DEBUG (" -> getting Filter Alg success " << *it);
      // push back
      m_acceptAlgs.push_back(theAlg);
    }
  return StatusCode::SUCCESS;
}


// Loop over all Algorithms in the accept list to see
// whether any have been executed and have their filter
// passed flag set. Any match causes the event to be
// provisionally accepted.
bool AANTupleStream::isEventAccepted() const
{
  // no filter
  if (m_acceptAlgs.empty())
    return true;

  const EventContext& ctx = Gaudi::Hive::currentContext();
  
  // loop over all algs
  std::vector<Algorithm*>::const_iterator it;
  std::vector<Algorithm*>::const_iterator itend = m_acceptAlgs.end();
  for (it = m_acceptAlgs.begin(); it != itend; ++it)
    {
      const Algorithm* theAlgorithm = (*it);
      if ( theAlgorithm->execState(ctx).state() == AlgExecState::State::Done &&
             ( ! theAlgorithm->execState(ctx).filterPassed() ) )
	return false;
    }

  return true;
}

StatusCode
AANTupleStream::initSchema()
{
  m_schemaDone = true;

  StatusCode sc = m_tHistSvc.retrieve();
  if (sc.isFailure())
  {
    ATH_MSG_ERROR ("Unable to retrieve pointer to THistSvc");
    return sc;
  }

  // initialize output collection
  sc = initCollection();
  if ( sc.isFailure() )
  {
    ATH_MSG_ERROR ("Could not init collection");
    return sc;
  }

  // register TTree to THistSvc
  sc = m_tHistSvc->regTree("/"+m_streamName+"/"+m_treeName, m_tree);
  if ( sc.isFailure() )
  {
    ATH_MSG_ERROR ("Could not register TTree");
    return sc;
  }

  /// dump
  m_tree->Print();

  return sc;
}

StatusCode
AANTupleStream::io_reinit()
{
  ServiceHandle<IIoComponentMgr> iomgr ("IoComponentMgr", this->name());
  if ( !iomgr.retrieve().isSuccess() ) {
    ATH_MSG_ERROR ("Could not retrieve IIoComponentMgr/IoComponentMgr !");
    return StatusCode::FAILURE;
  }

  ServiceHandle<IJobOptionsSvc> josvc ("JobOptionsSvc", this->name());
  if ( !josvc.retrieve().isSuccess() ) {
    ATH_MSG_ERROR ("Could not retrieve IJobOptionsSvc/JobOptionsSvc !");
    return StatusCode::FAILURE;
  }

  // check the I/O manager knows about me
  if ( !iomgr->io_hasitem (this) ) {
    ATH_MSG_ERROR ("I/O component mgr does not know about ourselves !");
    return StatusCode::FAILURE;
  }

  // check there is actually something to do...
  if ( !iomgr->io_contains (this, m_fileName) ) {
    ATH_MSG_DEBUG 
      ("I/O component mgr does not have any entry about ["<< m_fileName<<"]");
    return StatusCode::SUCCESS;
  }

  if ( !iomgr->io_retrieve (this, m_fileName).isSuccess() ) {
    ATH_MSG_ERROR ("Could not retrieve new name for [" << m_fileName << "] !");
    return StatusCode::FAILURE;
  }
  
  // recreate the proper property value...
  std::vector<std::string> outvec(1);
  outvec[0] = "AANT DATAFILE='" + m_fileName + "' OPT='RECREATE'";
  StringArrayProperty prop ("Output", outvec);
  if ( !josvc->addPropertyToCatalogue ("THistSvc", prop).isSuccess() ) {
    ATH_MSG_ERROR ("Could not update THistSvc.Output property with new name !");
  }

  // handle schema...
  if (!m_schemaDone) {
    if ( !initSchema().isSuccess() ) {
      ATH_MSG_ERROR ("Could not initialize schema !");
    }
  }

  return StatusCode::SUCCESS;
}
