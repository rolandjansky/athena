
// /***************************************************************************
//                           JetCMX.cxx  -  description
//                              -------------------
//     begin                : Mon Jul 28 2014
//     copyright            : (C) 2014 by Alan Watson
//     email                : Alan.Watson@CERN.CH
//  ***************************************************************************/
//
//
//================================================
// JetCMX class Implementation
// ================================================
//
//
//

// Utilities

// This algorithm includes
#include "TrigT1CaloSim/JetCMX.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1CaloUtils/CoordToHardware.h"
#include "TrigConfL1Data/L1DataDef.h"

#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
#include "TrigT1CaloEvent/CMXJetHits_ClassDEF.h"
#include "TrigT1CaloEvent/CMXJetTob_ClassDEF.h"
#include "TrigT1CaloEvent/JetTopoTOB.h"
#include "TrigT1CaloEvent/JetCMXTopoData_ClassDEF.h"
#include "TrigT1CaloEvent/JetCMXData.h"

#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/TriggerThresholdValue.h"
#include "TrigConfL1Data/JetThresholdValue.h"



namespace LVL1{

using namespace TrigConf;

//--------------------------------
// Constructors and destructors
//--------------------------------

JetCMX::JetCMX
  ( const std::string& name, ISvcLocator* pSvcLocator )
    : Algorithm( name, pSvcLocator ), 
      m_storeGate("StoreGateSvc", name), 
      m_configSvc("TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name)
{
    m_CMXJetHitLocation     = TrigT1CaloDefs::CMXJetHitsLocation;
    m_CMXJetTobLocation     = TrigT1CaloDefs::CMXJetTobLocation;
    m_JetCMXDataLocation    = TrigT1CaloDefs::JetCMXDataLocation;
    m_TopoOutputLocation    = TrigT1CaloDefs::JetTopoTobLocation;
    m_CTPOutputLocation     = TrigT1CaloDefs::JetCTPLocation;

    // This is how you declare the paramembers to Gaudi so that
    // they can be over-written via the job options file
    
    declareProperty( "EventStore",m_storeGate,"StoreGate Service");
    declareProperty( "CMXJetHitLocation",       m_CMXJetHitLocation );
    declareProperty( "CMXJetTobLocation",       m_CMXJetTobLocation );
    declareProperty( "JetCMXDataLocation",      m_JetCMXDataLocation );
    declareProperty( "CTPOutputLocation",       m_CTPOutputLocation );
    declareProperty( "TopoOutputLocation",      m_TopoOutputLocation );
    declareProperty( "LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");

}

// Destructor
JetCMX::~JetCMX() {
  MsgStream log( messageService(), name() ) ;
  log << MSG::INFO << "Destructor called" << endreq;
}


//---------------------------------
// initialise()
//---------------------------------

StatusCode JetCMX::initialize()
{

  // We must here instantiate items which can only be made after
  // any job options have been set
  MsgStream log( messageService(), name() ) ;
  int outputLevel = msgSvc()->outputLevel( name() );

     //
    // Connect to the LVL1ConfigSvc for the trigger configuration:
    //
  StatusCode sc = m_configSvc.retrieve();
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "Couldn't connect to " << m_configSvc.typeAndName() 
        << endreq;
    return sc;
  } else if (outputLevel <= MSG::DEBUG) {
    log << MSG::DEBUG << "Connected to " << m_configSvc.typeAndName() 
        << endreq;
  }

  // Now connect to the StoreGateSvc

  sc = m_storeGate.retrieve();
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "Couldn't connect to " << m_storeGate.typeAndName() 
        << endreq;
    return sc;
  } else if (outputLevel <= MSG::DEBUG) {
    log << MSG::DEBUG << "Connected to " << m_storeGate.typeAndName() 
        << endreq;
  }
    
  return StatusCode::SUCCESS ;
}


//-------------------------------------------------
// Optional debug of menu at start of run
//-------------------------------------------------

StatusCode JetCMX::beginRun()
{

  int outputLevel = msgSvc()->outputLevel( name() );
  if (outputLevel <= MSG::DEBUG) printTriggerMenu();

  return StatusCode::SUCCESS ;
}



//---------------------------------
// finalise()
//---------------------------------

StatusCode JetCMX::finalize()
{
   MsgStream log( messageService(), name() ) ;
   log << MSG::INFO << "Finalizing" << endreq;
   return StatusCode::SUCCESS ;
}


//----------------------------------------------
// execute() method called once per event
//----------------------------------------------
//


StatusCode JetCMX::execute( )
{
    
  /*
    The idea is that this thing gets the inputs, formats them as required, then
    uses a port of the online code. Then formats outputs and sends them to the
    required destinations.
    
    If that doesn't work, we'll put the processing in here as well.
  */

  //make a message logging stream

  MsgStream log( messageService(), name() );
  int outputLevel = msgSvc()->outputLevel( name() );
  if (outputLevel <= MSG::DEBUG) log << MSG::DEBUG << "starting JetCMX" << endreq;
  

  /** Create containers for BS simulation */
  DataVector<CMXJetTob>*  CMXTobs = new DataVector<CMXJetTob>;
  DataVector<CMXJetHits>* CMXHits = new DataVector<CMXJetHits>;

  /** Initialise pointer */
  m_jetCTP = 0;

  /** Create and initialise arrays for storing hit results */
  std::vector< std::vector<int> > crateHits;
  std::vector<int> Hits;
  
  Hits.resize(25);
  
  crateHits.resize(2);
  for (int crate = 0; crate < 2; ++crate) {
    crateHits[crate].resize(25);
  }
  
  
  // Create objects to store TOBs for L1Topo
  DataVector<JetCMXTopoData>* topoData = new DataVector<JetCMXTopoData>;
  for (int crate = 0; crate < 2; ++crate) {
    JetCMXTopoData* link = new JetCMXTopoData(crate);
    topoData->push_back(link);
  }
  

  /** Get Trigger Thresholds */
  std::vector< TrigConf::TriggerThreshold* > thresholds;
  std::vector< TrigConf::TriggerThreshold* > allThresholds = m_configSvc->ctpConfig()->menu().thresholdVector();
  for ( std::vector< TrigConf::TriggerThreshold* >::const_iterator it = allThresholds.begin();
        it != allThresholds.end(); ++it ) {
    if ( ( *it )->type() == L1DataDef::jetType() ) 
      thresholds.push_back( *it );
  }
  float jepScale = m_configSvc->thresholdConfig()->caloInfo().globalJetScale();


  /** Retrieve the JetCMXData (backplane data packages) */
  const t_jemDataContainer* bpData;
  if (m_storeGate->contains<t_jemDataContainer>(m_JetCMXDataLocation)) {
    StatusCode sc = m_storeGate->retrieve(bpData, m_JetCMXDataLocation);  
    if ( sc==StatusCode::SUCCESS ) {
	
      // Analyse module results
      t_jemDataContainer::const_iterator it = bpData->begin();
      for ( ; it != bpData->end(); ++it) {
        int crate = (*it)->crate();
        std::vector<unsigned int> tobWords = (*it)->TopoTOBs();
    
        // Store data for L1Topo
        bool overflow = (*it)->overflow();
        if (overflow) (*topoData)[crate]->setOverflow(true);
	
        for (std::vector<unsigned int>::const_iterator word = tobWords.begin();
	     word != tobWords.end(); ++word) {
	    
	   // Push back to Topo link
	   (*topoData)[crate]->addTOB( (*word) );
	   
	   // Decode TOB word 
	   JetTopoTOB tob( crate, (*word) );
	   int ieta = tob.ieta();
	   int iphi = tob.iphi();
           if (iphi < 0) iphi += 64;
	   int etLarge = tob.etLarge();
	   int etSmall = tob.etSmall();
	   
	   // Now check against trigger thresholds
	   for ( std::vector< TriggerThreshold* >::const_iterator itTh = thresholds.begin();
             itTh != thresholds.end(); ++itTh ) {
	     // Right type?
	     if ( (*itTh)->type() != L1DataDef::jetType() ) continue;
             // Does TOB satisfy this threshold?
             TriggerThresholdValue* ttv = (*itTh)->triggerThresholdValue( ieta, iphi );
             JetThresholdValue* jtv = dynamic_cast< JetThresholdValue* >( ttv );
	     if (jtv) {
                int etCut              = jtv->ptcut()*jepScale;
                std::string windowSize = jtv->windowSizeAsString();
                              
		if ( (windowSize == "LARGE" && etLarge > etCut) ||
		     (windowSize == "SMALL" && etSmall > etCut) ) {
                    int num = ( *itTh )->thresholdNumber();
		    if (num < 25) {
		        if ( num < 10 && crateHits[crate][num] < 7 )       crateHits[crate][num]++;
			else if ( num >= 10 && crateHits[crate][num] < 3 ) crateHits[crate][num]++;
		        if ( num < 10 && Hits[num] < 7 )                   Hits[num]++;
		        else if ( num >= 10 && Hits[num] < 3 )             Hits[num]++;
		    }
		    else log << MSG::WARNING << "Invalid threshold number " << num << endreq;
                } // passes cuts
		    
             } // JetThresholdValue pointer valid
           } // Loop over thresholds
		 
	} // Loop over TOBs
	    
      } // Loop over module results
      
      // Form CTP data objects
      unsigned int cableWord0 = 0;
      for (int i = 0; i < 10; ++i) cableWord0 |= ( Hits[i]<<(3*i) );

      unsigned int cableWord1 = 0;
      for (int i = 10; i < 25; ++i) cableWord1 |= ( Hits[i]<<(2*(i-10)) );

      m_jetCTP = new JetCTP( cableWord0, cableWord1 );
      
      // Form and store JetCMXHits
      std::vector<int> error0;  // Dummies - there will be no actual errors simulated
      std::vector<int> error1;
      
      // Now form hits words from module results and insert into objects
      std::vector<unsigned int> cratehits0;
      std::vector<unsigned int> cratehits1;
      const int peak = 0;
      const int system_crate = 1;

      // Crate sums (local and remote)  
      for (int crate = 0; crate < 2; ++crate) {
	
	  cratehits0.assign(1,0);
	  cratehits1.assign(1,0);
	  for (int i = 0; i < 5; ++i) {
	    cratehits0[0] |= ( crateHits[crate][i]<<(3*i) );
	    cratehits1[0] |= ( crateHits[crate][i+5]<<(3*i) );
	  }
	  CMXJetHits* crateCMXHits0 = new CMXJetHits(crate, LVL1::CMXJetHits::LOCAL_MAIN,
					            cratehits0, cratehits1, error0, error1, peak);
	  CMXHits->push_back(crateCMXHits0);
	  if (crate != system_crate) {
	    CMXJetHits* remoteCMXHits0 = new CMXJetHits(system_crate, LVL1::CMXJetHits::REMOTE_MAIN,
					                cratehits0, cratehits1, error0, error1, peak);
	    CMXHits->push_back(remoteCMXHits0);
	  }
	  
	  cratehits0.assign(1,0);
	  cratehits1.assign(1,0);
	  for (int i = 0; i < 8; ++i) cratehits0[0] |= ( crateHits[crate][i+10]<<(2*i) );
	  for (int i = 0; i < 7; ++i) cratehits1[0] |= ( crateHits[crate][i+18]<<(2*i) );
	  CMXJetHits* crateCMXHits1 = new CMXJetHits(crate, LVL1::CMXJetHits::LOCAL_FORWARD,
					             cratehits0, cratehits1, error0, error1, peak);
	  CMXHits->push_back(crateCMXHits1);
	  if (crate != system_crate) {
	    CMXJetHits* remoteCMXHits1 = new CMXJetHits(system_crate, LVL1::CMXJetHits::REMOTE_FORWARD,
					                cratehits0, cratehits1, error0, error1, peak);
	    CMXHits->push_back(remoteCMXHits1);
	  }
      } // loop over crates
      
      // global sums
      cratehits0.assign(1,0);
      cratehits1.assign(1,0);
      for (int i = 0; i < 5; ++i) {
	cratehits0[0] |= ( Hits[i]<<(3*i) );
	cratehits1[0] |= ( Hits[i+5]<<(3*i) );
      }
      CMXJetHits* totalCMXHits0 = new CMXJetHits(system_crate, LVL1::CMXJetHits::TOTAL_MAIN,
					         cratehits0, cratehits1, error0, error1, peak);
      CMXHits->push_back(totalCMXHits0);

      cratehits0.assign(1,0);
      cratehits1.assign(1,0);
      for (int i = 0; i < 8; ++i) cratehits0[0] |= ( Hits[i+10]<<(2*i) );
      for (int i = 0; i < 7; ++i) cratehits1[0] |= ( Hits[i+18]<<(2*i) );
      CMXJetHits* totalCMXHits1 = new CMXJetHits(system_crate, LVL1::CMXJetHits::TOTAL_FORWARD,
					         cratehits0, cratehits1, error0, error1, peak);
      CMXHits->push_back(totalCMXHits1);
      
    } // Successfully read input data
 
  } // Input collection exists in StoreGate

  
  // Store output for BS simulation
  StatusCode sc = m_storeGate->overwrite(CMXTobs, m_CMXJetTobLocation,true,false,false);
  if (sc != StatusCode::SUCCESS) log << MSG::WARNING << "Problem writeing CMXTobs to StoreGate" << endreq;

  sc = m_storeGate->overwrite(CMXHits, m_CMXJetHitLocation,true,false,false);
  if (sc != StatusCode::SUCCESS) log << MSG::WARNING << "Problem writeing CMXHits to StoreGate" << endreq;

  // Store Topo results
  sc = m_storeGate->overwrite(topoData, m_TopoOutputLocation,true,false,false);
  if (sc != StatusCode::SUCCESS) log << MSG::WARNING << "Problem writeing CPCMXTopoData object to StoreGate" << endreq;
 
  // Store CTP results
  if (m_jetCTP == 0) {
    m_jetCTP = new JetCTP(0,0);
    log << MSG::WARNING << "No JetCTP found. Creating empty object" << endreq;
  }
  sc = m_storeGate->overwrite(m_jetCTP, m_CTPOutputLocation,true,false,false);
  if (sc != StatusCode::SUCCESS) log << MSG::WARNING << "Problem writeing JetCTP object to StoreGate" << endreq;

  return StatusCode::SUCCESS ;
}



/** print trigger configuration, for debugging purposes */
void LVL1::JetCMX::printTriggerMenu(){
  MsgStream log( messageService(), name() );
  
  /** This is all going to need updating for the new menu structure.
      Comment out in the meanwhile 
  
  L1DataDef def;

  std::vector<TrigConf::TriggerThreshold*> thresholds = m_configSvc->ctpConfig()->menu().thresholdVector();
  std::vector<TrigConf::TriggerThreshold*>::const_iterator it;
  for (it = thresholds.begin(); it != thresholds.end(); ++it) {
    if ( (*it)->type() == def.emType() || (*it)->type() == def.tauType() ) {
      log << MSG::DEBUG << "TriggerThreshold " << (*it)->id() << " has name " << (*it)->name() << endreq
          << "  threshold number " << (*it)->thresholdNumber() << endreq
          << "  number of values = " << (*it)->numberofValues() << endreq;
      for (std::vector<TriggerThresholdValue*>::const_iterator tv = (*it)->thresholdValueVector().begin();
           tv != (*it)->thresholdValueVector().end(); ++tv) {
        ClusterThresholdValue* ctv;
        ctv = dynamic_cast<ClusterThresholdValue*> (*tv);
	if (!ctv) {
          log << MSG::ERROR << "Threshold type name is EM/Tau, but is not a ClusterThreshold object!" << endreq;
          continue;
        }
        log << MSG::DEBUG << "ClusterThresholdValue: " << endreq
            << "  Threshold value = " << ctv->thresholdValueCount() << endreq
            << "  EM isolation = " << ctv->emIsolationCount() << endreq
            << "  Had isolation = " << ctv->hadIsolationCount() << endreq
            << "  Had veto = " << ctv->hadVetoCount() << endreq
            << "  EtaMin = " << ctv->etamin() << ", EtaMax = " << ctv->etamax() << endreq;
        
      } // end of loop over threshold values
    } //  is type == em or tau?
  } // end of loop over thresholds
  
  */   
}

} // end of namespace bracket

