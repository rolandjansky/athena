/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigExL2muCalibTest/L2muCalibTest.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "StoreGate/DataHandle.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "AthenaInterprocess/Incidents.h"

#include "TrigSteeringEvent/HLTResult.h"
#include "TrigHLTResultByteStream/HLTResultByteStreamTool.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "TrigSteeringEvent/Enums.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"

// static const AlgFactory<L2muCalibTest>    Factory;
// const IAlgFactory& L2muCalibTestFactory = Factory;

/////////////////////////////////////////////////////////////////////////////

L2muCalibTest::L2muCalibTest(const std::string& name, ISvcLocator* pSvcLocator) :
   AthAlgorithm(name, pSvcLocator),
   m_jobOptionsSvc(0),
   m_allowOksConfig(true),
   m_dumpCatalogue(false), 
   m_muonCalBufferName("NONE"),
   m_muonCalBufferSize(0),
   m_navigationForEF("HLT::Navigation/Navigation", this),
   m_nav(0)
{
   // Declare the properties
   declareProperty("AllowOksConfig",                 m_allowOksConfig);
   declareProperty("DumpPropertyCatalogue",          m_dumpCatalogue);
   declareProperty("MuonCalBufferName",              m_muonCalBufferName);
   declareProperty("MuonCalBufferSize",              m_muonCalBufferSize);
   declareProperty("ReadBackDataScoutingCollection", m_readBackDataScoutingCollection=0);
   declareProperty("KeyDataScoutingROB",             m_keyDataScoutingROB="DataScouting_03");
   declareProperty("KeyDataScoutingContainer",       m_keyDataScoutingContainer="HLT_xAOD__TrigCompositeContainer_MuonCalibrationStream");
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode L2muCalibTest::initialize(){

  // Part 1: Get the messaging service, print where you are
  MsgStream log(msgSvc(), name());
  ATH_MSG_INFO("initialize()");

  StatusCode sc;

  SmartIF<IService> tmp_msgSvc(msgSvc());
  if(tmp_msgSvc.isValid()) {
    ATH_MSG_INFO(" Algorithm = " << name() << " is connected to Message Service = "
        << tmp_msgSvc->name());
  }

  // Print out the property values after update from catalogue
  ATH_MSG_INFO(" ---------------------------------------------------------- ");
  ATH_MSG_INFO(" --- Properties before update from JobOptions Catalogue --- ");
  ATH_MSG_INFO(" ---------------------------------------------------------- ");
  ATH_MSG_INFO(" AllowOksConfig                 = " << m_allowOksConfig);
  ATH_MSG_INFO(" DumpPropertyCatalogue          = " << m_dumpCatalogue);
  ATH_MSG_INFO(" MuonCalBufferName              = " << m_muonCalBufferName);
  ATH_MSG_INFO(" MuonCalBufferSize              = " << m_muonCalBufferSize);
  ATH_MSG_INFO(" ReadBackDataScoutingCollection = " << m_readBackDataScoutingCollection);
  ATH_MSG_INFO(" KeyDataScoutingROB             = " << m_keyDataScoutingROB);
  ATH_MSG_INFO(" KeyDataScoutingContainer       = " << m_keyDataScoutingContainer);

  // Part 2: Get the JobOptionsSvc service, and update properties from catalogue
  if ( m_allowOksConfig ) {
    sc = service("JobOptionsSvc", m_jobOptionsSvc);
    if (sc.isFailure()) {
      ATH_MSG_ERROR("Could not find JobOptionsSvc");
      return sc;
    } else {
      IService* svc = dynamic_cast<IService*>(m_jobOptionsSvc);
      if(svc != 0 ) {
	ATH_MSG_INFO(" Algorithm = " << name() << " is connected to JobOptionsSvc Service = "
	    << svc->name());
      }

      if ( m_dumpCatalogue ) {
	// Dump of the catalogue
	ATH_MSG_INFO("=================================================");
	ATH_MSG_INFO("Dump of the property catalogue.... ");
	std::vector<std::string> clients = m_jobOptionsSvc->getClients();
	std::vector<std::string>::iterator cit;
	std::vector<const Gaudi::Details::PropertyBase*>::const_iterator pit;

	for( cit = clients.begin(); cit != clients.end(); cit++ ) {
	  ATH_MSG_INFO(" Properties of " <<  *cit << ": ";
	  const std::vector<const Gaudi::Details::PropertyBase*>* properties = m_jobOptionsSvc->getProperties(*cit);
	  for( pit = properties->begin(); pit != properties->end(); pit++ ) {
	    log << (*pit)->name();
	    if( (pit+1) != properties->end())  log << ", ";
	  }
	  log);
	}
	ATH_MSG_INFO("=================================================");
      }

      // Overwrite the properties with values from the catalogue using the non existing algorithm
      // 'MuonL2CalibrationConfig'
      sc = m_jobOptionsSvc->setMyProperties("MuonHltCalibrationConfig",this);
      if (sc.isFailure()) {
	ATH_MSG_ERROR("Could not set properties from JobOptionsSvc catalogue.");
	return sc;
      }

      // Find the Data Flow application name
      bool df_found = false;
      const std::vector<const Gaudi::Details::PropertyBase*>* dataFlowProps = m_jobOptionsSvc->getProperties("DataFlowConfig");
      if ( dataFlowProps ) { 
	ATH_MSG_INFO(" Properties available for 'DataFlowConfig': number = " << dataFlowProps->size());
	ATH_MSG_INFO(" --------------------------------------------------- ");
	for ( std::vector<const Gaudi::Details::PropertyBase*>::const_iterator cur = dataFlowProps->begin();
	      cur != dataFlowProps->end(); cur++) {
	  ATH_MSG_INFO((*cur)->name() << " = " << (*cur)->toString());
	  // the application name is found
	  if ( (*cur)->name() == "DF_ApplicationName" ) {
	    df_found = true;
	  }
	  // the enabled ROB list is found
	  if ( (*cur)->name() == "DF_Enabled_ROB_IDs" ) {
	    SimpleProperty< std::vector<uint32_t> > enabledROBs;
	    enabledROBs.setName("enabledROBIds");
	    if (enabledROBs.assign(**cur)) {
           ATH_MSG_INFO(" ---> got from OKS = " << enabledROBs.value().size() << " ROB ids.");
	    } else {
           ATH_MSG_WARNING(" ---> set property failed.");
	    }
	  }
	}
  
	if ( !df_found ) {
	  ATH_MSG_INFO(" Property for DF_ApplicationName not found.");
	}
      } else {
	ATH_MSG_INFO(" No Properties for 'DataFlowConfig' found.");
      } 
      ATH_MSG_INFO("=================================================");


      // release JobOptionsSvc
      unsigned long mjcounter = m_jobOptionsSvc->release();
      ATH_MSG_INFO(" --> Release JobOptionsSvc Service, Counter = " << mjcounter);
    }
  }

  // Print out the property values after update from catalogue
  ATH_MSG_INFO(" ---------------------------------------------------------- ");
  ATH_MSG_INFO(" --- Properties after update from JobOptions Catalogue  --- ");
  ATH_MSG_INFO(" ---------------------------------------------------------- ");
  ATH_MSG_INFO(" AllowOksConfig        = " << m_allowOksConfig);
  ATH_MSG_INFO(" DumpPropertyCatalogue = " << m_dumpCatalogue);
  ATH_MSG_INFO(" MuonCalBufferName     = " << m_muonCalBufferName);
  ATH_MSG_INFO(" MuonCalBufferSize     = " << m_muonCalBufferSize);
  ATH_MSG_INFO("=================================================");

  // Part 3: Locate the IncidentSvc and subscribe for UpdateAfterFork incident 
  ServiceHandle<IIncidentSvc> p_incidentSvc("IncidentSvc",name());
  sc =  p_incidentSvc.retrieve();
  if (!sc.isSuccess()) {
    ATH_MSG_ERROR("Could not find IncidentSvc");
    return sc;
  } else {
    long int pri=100;
    p_incidentSvc->addListener(this,AthenaInterprocess::UpdateAfterFork::type(),pri);
    p_incidentSvc.release().ignore();
  }

  // retrieve navigation
  if ( m_navigationForEF.retrieve().isFailure() ) {
    ATH_MSG_FATAL("failed to get navigation, can not retrieve navigation tool: " << m_navigationForEF);
    return  StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("successfully retrieved navigation tool: " << m_navigationForEF);
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode L2muCalibTest::execute() {

  // Get the messaging service, print where you are
  ATH_MSG_INFO("execute()");

  if (!m_readBackDataScoutingCollection) return StatusCode::SUCCESS;

  StatusCode sc;
  ATH_MSG_DEBUG(" StoreGate structure: Begin execute " << name() << "\n" << evtStore()->dump());

  // Get the Muon HLT DataScouting result
  const DataHandle<HLT::HLTResult> datascouting_03_result;

  sc = evtStore()->retrieve(datascouting_03_result,m_keyDataScoutingROB) ;
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR("Could not find HLTResult with key = " << m_keyDataScoutingROB  );
    return sc;
  } 

  ATH_MSG_DEBUG("HLTResult is level="<< datascouting_03_result->getHLTLevel()); 

  const std::vector<uint32_t>& navData = datascouting_03_result->getNavigationResult();
  if ( navData.size() != 0 ) { 
    ATH_MSG_DEBUG("navigation payload has size " << navData.size());
    m_nav = &*m_navigationForEF;
    m_nav->deserialize( navData );
  } else {
    ATH_MSG_WARNING("navigation payload has size 0" );
  }

  const DataHandle<xAOD::TrigCompositeContainer> muon_calib_obj;
  sc = evtStore()->retrieve(muon_calib_obj, m_keyDataScoutingContainer) ;
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING("Could not find muon calib obj with key = " << m_keyDataScoutingContainer);
  } else {
    const xAOD::TrigCompositeContainer* muon_comp_container = muon_calib_obj.cptr();   
    ATH_MSG_DEBUG("muon calib obj has size = " << muon_comp_container->size());
    const std::string muon_buffer_name("MuonCalibrationStream");
    for (const auto& ii : *muon_comp_container) {
      ATH_MSG_DEBUG("Composite object has name = " << ii->name());
      ATH_MSG_DEBUG("Composite object has detail " << muon_buffer_name << " = " << ii->hasDetail<std::vector<int> >(muon_buffer_name));
      std::vector<int> detail_values;
      bool got_data = ii->getDetail(muon_buffer_name,detail_values);
      ATH_MSG_DEBUG("Composite object has detail " <<  muon_buffer_name << " = " << got_data << " with size = " << detail_values.size() );
    }
  }
  ATH_MSG_DEBUG(" StoreGate structure: End execute " << name() << "\n" << evtStore()->dump());

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode L2muCalibTest::finalize() {

  // Part 1: Get the messaging service, print where you are
  MsgStream log(msgSvc(), name());
  ATH_MSG_INFO("finalize()");

  return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// handler for "UpdateAfterFork")
void L2muCalibTest::handle(const Incident& incident) {

  if (incident.type()==AthenaInterprocess::UpdateAfterFork::type()) {
    MsgStream log(msgSvc(), name());
    ATH_MSG_INFO("+-----------------------------------+");
    ATH_MSG_INFO("| handle for UpdateAfterFork called |");
    ATH_MSG_INFO("+-----------------------------------+");

    // get the worker id from the incident
    const AthenaInterprocess::UpdateAfterFork& updinc = dynamic_cast<const AthenaInterprocess::UpdateAfterFork&>(incident);
    ATH_MSG_INFO(" ---------------------------------------------------------- ");
    ATH_MSG_INFO(" --- Parameters from UpdateAfterFork incident           --- ");
    ATH_MSG_INFO(" ---------------------------------------------------------- ");
    ATH_MSG_INFO(" Worker ID             = " << updinc.workerID());
    ATH_MSG_INFO(" Worker Process ID     = " << updinc.workerProcessID());
    ATH_MSG_INFO("=================================================");

    // create an individual muon buffer name for every worker
    std::ostringstream ost;
    ost << "_" << std::setw(3) << std::setfill('0') << updinc.workerID(); 
    m_muonCalBufferName = m_muonCalBufferName.value() + ost.str();

    // Print out the property values after update from catalogue
    ATH_MSG_INFO(" ---------------------------------------------------------- ");
    ATH_MSG_INFO(" --- Updated properties after forking the workers       --- ");
    ATH_MSG_INFO(" ---------------------------------------------------------- ");
    ATH_MSG_INFO(" AllowOksConfig        = " << m_allowOksConfig);
    ATH_MSG_INFO(" DumpPropertyCatalogue = " << m_dumpCatalogue);
    ATH_MSG_INFO(" MuonCalBufferName     = " << m_muonCalBufferName);
    ATH_MSG_INFO(" MuonCalBufferSize     = " << m_muonCalBufferSize);
    ATH_MSG_INFO("=================================================");
  }
}
