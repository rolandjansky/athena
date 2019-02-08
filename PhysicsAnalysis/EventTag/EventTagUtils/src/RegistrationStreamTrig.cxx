/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EventTagUtils/RegistrationStreamTrig.h"

//#include "SGTools/DataProxy.h"

// Framework include files
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IAddressCreator.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/IConverter.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/FileIncident.h"

#include "AthenaKernel/IOVRange.h"

#include "TrigConfInterfaces/ITrigConfigSvc.h"

#include "DBDataModel/CollectionMetadata.h"

#include "AthenaPoolUtilities/TagMetadataKey.h"

#include "TPython.h"
#include "TTree.h"
#include "TSystem.h"


#include <cassert>
#include <string>
#include <vector>
#include <map>
#include <typeinfo>
#include <ctime>


// Standard Constructor
RegistrationStreamTrig::RegistrationStreamTrig(const std::string& name,
                                               ISvcLocator* pSvcLocator)
    :
    AthAlgorithm(name,pSvcLocator),
    m_pOutputStore("StoreGateSvc/MetaDataStore", name),
    m_pInputStore("StoreGateSvc/InputMetaDataStore", name),
    m_trigConfSvc("TrigConf::TrigConfigSvc/TrigConfigSvc", name ),
    m_run(0),
    m_lb(0)
{
    declareProperty("TrigConfigSvc", m_trigConfSvc);
    declareProperty("GetTriggerConf", m_gettriggerconf=true);
}

// initialize data writer
StatusCode 
RegistrationStreamTrig::initialize() 
{
   ATH_MSG_DEBUG(this->name() << ": In initialize ");

   StatusCode sc = m_pOutputStore.retrieve();
   if (0 == m_pOutputStore || sc.isFailure()) {
     ATH_MSG_ERROR("Could not find metadata store, MetaDataStore");
     return(sc);
   }

   sc = m_pInputStore.retrieve();
   if (0 == m_pInputStore || sc.isFailure()) {
      ATH_MSG_ERROR("Could not find InputMetaDataStore");
      return(sc);
   }

   if ( m_gettriggerconf ) {
     sc= m_trigConfSvc.retrieve();
     if (0 == m_trigConfSvc || sc.isFailure()) {
       ATH_MSG_ERROR("Could not find TrigConfigSvc");
       return(sc);
     }
   }
   // Set to be listener for end of event
   ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", this->name());
   sc = incSvc.retrieve();
   if (sc.isFailure()) {
      ATH_MSG_ERROR("Unable to get the IncidentSvc");
      return(sc);
   }

   incSvc->addListener(this, "TrigConf");  // trigger config

   /** Check the first incidentce */ 
   m_isFirstTrigIncidence = true;


   return AthAlgorithm::initialize();
}

void RegistrationStreamTrig::handle(const Incident& inc) {

  ATH_MSG_DEBUG(this->name() << ": In handle");

  const FileIncident *fi = dynamic_cast<const FileIncident*>( &inc );
  if (fi != 0) {
     ATH_MSG_DEBUG("File incident: " << fi->fileName() << " [GUID: " << fi->fileGuid() << "]");
     ATH_MSG_DEBUG("File incident type: " << inc.type() );
  } else {
     ATH_MSG_DEBUG("File undefined");
  }

  if ( m_gettriggerconf ) { 
    if (inc.type() == "TrigConf") {
      handleTrigConf( inc );
    }  else {
      ATH_MSG_DEBUG(inc.type() << " not handled");
    } 
  }
}

void RegistrationStreamTrig::handleTrigConf( const Incident&) {

  ATH_MSG_DEBUG(this->name() << ": In handleTrigConf");

  /** get the keys from trig. config svc */
  int smk    = m_trigConfSvc->masterKey();
  int l1psk  = m_trigConfSvc->lvl1PrescaleKey();
  int hltpsk = m_trigConfSvc->hltPrescaleKey();

  /* Insert each key/value to the map */
  if (! m_isFirstTrigIncidence) {
    addkeys("SMK", m_SMKStart,"Run");
    addkeys("L1PSK", m_L1PSKStart,"RunLB");
    addkeys("HLTPSK", m_HLTPSKStart,"RunLB");
  }
  m_LBStart = m_lb;
  m_SMKStart = smk;
  m_L1PSKStart = l1psk;
  m_HLTPSKStart = hltpsk;
  m_isFirstTrigIncidence = false;

}

void RegistrationStreamTrig::addkeys( std::string trigKey, int trigValue , std::string iovtype) 
{

  TagMetadataKey tkey;
  tkey.setKey(trigKey); 
  std::stringstream qualname;
  int start=m_LBStart+1, stop=m_lb;
  if (iovtype == "Run") { start = 0; stop = -1;}  // for Run just pick 0,inf
  IOVRange iov(IOVTime(m_run,start),IOVTime(m_run,stop));
  tkey.setIOV((std::string)iov);
  std::stringstream keyvalue;  keyvalue << trigValue ;
  m_lumitrig.insert(std::make_pair(tkey.toString(), keyvalue.str()));

}

StatusCode RegistrationStreamTrig::stop() {

  ATH_MSG_INFO(this->name() << " : stop method");

  /** Add the last "TrigConf" incidence to the m_lumitrig. If, there are N number of LB in a file
   ** and last "TrigConf" incedence happens at LB_(N-2),
   ** then the last entry LB_(N-2)-LB_N to be added.
   */
  if ( m_gettriggerconf ) {
    addkeys("SMK", m_SMKStart,"Run");
    addkeys("L1PSK", m_L1PSKStart,"RunLB");
    addkeys("HLTPSK", m_HLTPSKStart,"RunLB");
  } 

  ATH_MSG_INFO("Filled a metadata container of size " << m_lumitrig.size());

  /** Record lumi xml & Trigger keys to  metadata */
  CollectionMetadata* def = new CollectionMetadata(m_lumitrig);
  CollectionMetadataContainer* mcont = new CollectionMetadataContainer();
  mcont->push_back(def);
  StatusCode rc = m_pOutputStore->record(mcont,this->name());
  if (!rc.isSuccess()) {
    ATH_MSG_ERROR("Unable to store Default CollectionMetadataContainer");
  } else {
    ATH_MSG_INFO("Stored CollectionMetadataContainer in " << m_pOutputStore);
    ATH_MSG_INFO("Size: " << def->size());
    ATH_MSG_INFO("Contents: ");
    CollectionMetadata::const_iterator i = def->begin();
    for (CollectionMetadata::const_iterator j=i; j != def->end(); ++j) {
      ATH_MSG_INFO("    "<<j->first<<" "<<j->second);
    }
  }

  return StatusCode::SUCCESS;

}

// initialize data writer
StatusCode 
RegistrationStreamTrig::execute() 
{
  ATH_MSG_DEBUG(this->name() << ": In execute ");

  StatusCode sc = evtStore()->retrieve( m_eventInfo );
  if (sc.isFailure()) {
      ATH_MSG_ERROR("Cannot get event info.");
  } else {
    m_run  = m_eventInfo->runNumber();
    m_lb   = m_eventInfo->lumiBlock();
  }
  return StatusCode::SUCCESS;
}


// terminate data writer
StatusCode 
RegistrationStreamTrig::finalize() 
{
    ATH_MSG_DEBUG(this->name() << ": In finalize ");
    return AthAlgorithm::finalize();
}

