
// /***************************************************************************
//                           JEMJetSim.cxx  -  description
//                              -------------------
//     begin                : Wed Mar 12 2014
//     copyright            : (C) 2014 by Alan Watson
//     email                : Alan.Watson@CERN.CH
//  ***************************************************************************/
//
//
//================================================
// JEMJetSim class Implementation
// ================================================
//
//
//

// Utilities

// This algorithm includes
#include "TrigT1CaloSim/JEMJetSim.h"
#include "TrigT1CaloUtils/JetEnergyModuleKey.h"

#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1CaloUtils/CoordToHardware.h"
#include "TrigConfL1Data/L1DataDef.h"

#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
#include "xAODTrigL1Calo/JetElementContainer.h"
#include "TrigT1CaloEvent/JEMTobRoI_ClassDEF.h"
#include "TrigT1CaloEvent/JetCMXData_ClassDEF.h"
#include "TrigT1CaloEvent/JetInput.h"

#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/TriggerThresholdValue.h"
#include "TrigConfL1Data/ClusterThresholdValue.h"



namespace LVL1{

using namespace TrigConf;

//--------------------------------
// Constructors and destructors
//--------------------------------

JEMJetSim::JEMJetSim
  ( const std::string& name, ISvcLocator* pSvcLocator )
    : AthAlgorithm( name, pSvcLocator ), 
      m_allTOBs(0),
      m_JetCMXData(0),
      m_configSvc("TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name),
      m_JetTool("LVL1::L1JEMJetTools/L1JEMJetTools")
{
    m_JEMTobRoILocation      = TrigT1CaloDefs::JEMTobRoILocation;
    m_JetCMXDataLocation     = TrigT1CaloDefs::JetCMXDataLocation;
    m_JetElementLocation     = TrigT1CaloDefs::JetElementLocation;

    // This is how you declare the paramembers to Gaudi so that
    // they can be over-written via the job options file
    
    declareProperty( "JEMTOBRoILocation",       m_JEMTobRoILocation );
    declareProperty( "JetCMXDataLocation",       m_JetCMXDataLocation );
    declareProperty( "LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");
    declareProperty( "JetElementLocation", m_JetElementLocation ) ;

}


//---------------------------------
// initialise()
//---------------------------------

StatusCode JEMJetSim::initialize()
{
  ATH_CHECK(  m_configSvc.retrieve() );
  ATH_CHECK( m_JetTool.retrieve() );
  return StatusCode::SUCCESS ;
}


//-------------------------------------------------
// Optional debug of menu at start of run
//-------------------------------------------------

StatusCode JEMJetSim::beginRun()
{
  if (msgLvl(MSG::DEBUG)) printTriggerMenu();
  return StatusCode::SUCCESS ;
}



//---------------------------------
// finalise()
//---------------------------------

StatusCode JEMJetSim::finalize()
{
   ATH_MSG_INFO("Finalizing" );
   return StatusCode::SUCCESS ;
}


//----------------------------------------------
// execute() method called once per event
//----------------------------------------------
//


StatusCode JEMJetSim::execute( )
{
    
  /* 
    Retrieve collection of JetElements 
    Fill a std::map of JetInputs from them
    Loop over crates/modules
      For each crate, loop over towers and find TOBs 
                      form backplane data objects
                      form daq objects    
  */

  //make a message logging stream

  ATH_MSG_DEBUG ( "starting JEMJetSim" ); 

  // Create containers for this event
  m_JetCMXData   = new DataVector<JetCMXData>;  //Container of backplane data objects
  m_allTOBs      = new DataVector<JEMTobRoI>;  // Container to hold all TOB RoIs in event

  // Retrieve the JetElementContainer
  if (evtStore()->contains<xAOD::JetElementContainer>(m_JetElementLocation)) {
    const DataVector<xAOD::JetElement>* storedJEs;
    StatusCode sc = evtStore()->retrieve(storedJEs,m_JetElementLocation);
    if ( sc==StatusCode::SUCCESS ) {
       // Check size of JetElementCollection - zero would indicate a problem
      if (storedJEs->size() == 0)
         ATH_MSG_WARNING("Empty JetElementContainer - looks like a problem" );
	
      // Form JetInputs and put them in a map
      std::map<int, JetInput*>* inputMap = new std::map<int, JetInput*> ;
      m_JetTool->mapJetInputs(storedJEs, inputMap);
	 
      // Loop over crates and modules
      for (int iCrate = 0; iCrate < 2; ++iCrate) {
	for (int iModule = 0; iModule < 16; ++iModule) {
	    
	   // For each module, find TOBs and backplane data
	   std::vector<unsigned int> jetCMXData;
	   m_JetTool->findJEMResults(inputMap,iCrate,iModule,m_allTOBs,jetCMXData);
	   // Push backplane data into output DataVectors
	   JetCMXData* bpData = new JetCMXData(iCrate,iModule,jetCMXData);
	   m_JetCMXData -> push_back(bpData);
	   
	} // loop over modules
      } // loop over crates
  
      /// Clean up JetInputs
      for (std::map<int, JetInput*>::iterator it = inputMap->begin(); it != inputMap->end(); ++it) {
        delete (*it).second;
      }
      delete inputMap;
  
    } // found TriggerTowers
    
    else ATH_MSG_WARNING("Error retrieving JetElements" );
  }
  else ATH_MSG_WARNING("No JetElementContainer at " << m_JetElementLocation );
  
       
  // Store module readout and backplane results in the TES
  storeBackplaneTOBs();
  storeModuleRoIs();
  
  // Clean up at end of event
  m_JetCMXData  = 0;
  m_allTOBs    = 0;

  return StatusCode::SUCCESS ;
}

/** place backplane data objects (CPM -> CMX) in StoreGate */
void LVL1::JEMJetSim::storeBackplaneTOBs() {

  // Store backplane data objects
  StatusCode sc = evtStore()->overwrite(m_JetCMXData, m_JetCMXDataLocation, true);

  if (sc.isSuccess()) {
    ATH_MSG_VERBOSE ( "Stored " << m_JetCMXData->size()
                      << " JetCMXData at " << m_JetCMXDataLocation );
  }
  else {
     ATH_MSG_ERROR("failed to write JetCMXData to  "
         << m_JetCMXDataLocation );
  } 
    
  return;

} //end storeBackplaneTOBs


/** place final ROI objects in the TES. */
void LVL1::JEMJetSim::storeModuleRoIs() {

  StatusCode sc = evtStore()->overwrite(m_allTOBs, m_JEMTobRoILocation, true);

  if (sc.isSuccess()) {
    ATH_MSG_VERBOSE ( "Stored " << m_allTOBs->size()
                      << " Jet TOBs at " << m_JEMTobRoILocation );
  }
  else {
     ATH_MSG_ERROR("failed to write JEMTobRoIs to  "
         << m_JEMTobRoILocation );
  } 

  return;

} //end storeModuleRoIs


} // end of LVL1 namespace bracket


/** print trigger configuration, for debugging purposes */
void LVL1::JEMJetSim::printTriggerMenu(){
  /** This is all going to need updating for the new menu structure.
      Comment out in the meanwhile 
  
  L1DataDef def;

  unsigned int numThresh=0;  
  std::vector<TriggerThreshold*> thresholds = m_configSvc->ctpConfig()->menu().thresholdVector();
  std::vector<TriggerThreshold*>::const_iterator it;
  for (it = thresholds.begin(); it != thresholds.end(); ++it) {
    if ( (*it)->type() == def.jetType() ||
         (*it)->type() == def.fjetType()  ||
         (*it)->type() == def.jfType()  ||
         (*it)->type() == def.jbType()) {
      ATH_MSG_DEBUG("TriggerThreshold " << (*it)->id() << " has name " << (*it)->name() << endmsg
          << "  threshold number " << (*it)->thresholdNumber() << endmsg
          << "  number of values = " << (*it)->numberofValues() );
      for (std::vector<TrigConf::TriggerThresholdValue*>::const_iterator tv = (*it)->thresholdValueVector().begin();
           tv != (*it)->thresholdValueVector().end(); ++tv) {
        TrigConf::JetThresholdValue* jtv;
        jtv = dynamic_cast<JetThresholdValue*> (*tv);
        if (!jtv) {
          ATH_MSG_ERROR("Threshold type name is Jet, but is not a JetThreshold object!" );
          continue;
        }
        ATH_MSG_DEBUG("JetThresholdValue: " << endmsg
            << "  Type = " << (*it)->type() << endmsg
            << "  Threshold value = " << jtv->thresholdValueCount() << endmsg
            << "  Cluster size = " << jtv->window() << endmsg
            << "  EtaMin = " << jtv->etamin() << ", EtaMax = " << jtv->etamax() );
        
      } // end of loop over threshold values
      numThresh++;
    } //  is type == jet or forward jet
  } // end of loop over thresholds

  return;
  
  */
    
}


