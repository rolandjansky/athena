
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
    : AthAlgorithm( name, pSvcLocator ),
      m_CMXJetHitLocation(TrigT1CaloDefs::CMXJetHitsLocation),
      m_CMXJetTobLocation(TrigT1CaloDefs::CMXJetTobLocation),
      m_TopoOutputLocation(TrigT1CaloDefs::JetTopoTobLocation),
      m_CTPOutputLocation(TrigT1CaloDefs::JetCTPLocation),
      m_JetCMXDataLocation(TrigT1CaloDefs::JetCMXDataLocation),
      m_jetCTP(nullptr),
      m_configSvc("TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name)
{
    declareProperty( "CMXJetHitLocation",       m_CMXJetHitLocation );
    declareProperty( "CMXJetTobLocation",       m_CMXJetTobLocation );
    declareProperty( "JetCMXDataLocation",      m_JetCMXDataLocation );
    declareProperty( "CTPOutputLocation",       m_CTPOutputLocation );
    declareProperty( "TopoOutputLocation",      m_TopoOutputLocation );
    declareProperty( "LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");
}


//---------------------------------
// initialise()
//---------------------------------

StatusCode JetCMX::initialize()
{
  ATH_CHECK( m_configSvc.retrieve() );
  return StatusCode::SUCCESS ;
}


//-------------------------------------------------
// Optional debug of menu at start of run
//-------------------------------------------------

StatusCode JetCMX::beginRun()
{
  if (msgLvl(MSG::DEBUG)) printTriggerMenu();
  return StatusCode::SUCCESS ;
}



//---------------------------------
// finalise()
//---------------------------------

StatusCode JetCMX::finalize()
{
   ATH_MSG_INFO("Finalizing" );
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

  ATH_MSG_DEBUG ( "starting JetCMX" );
  

  /** Create containers for BS simulation */
  DataVector<CMXJetTob>*  CMXTobs = new DataVector<CMXJetTob>;
  DataVector<CMXJetHits>* CMXHits = new DataVector<CMXJetHits>;

  /** Initialise pointer */
  m_jetCTP = 0;

  /** Create and initialise arrays for storing hit results */
  std::vector< std::vector<int> > crateHits;
  std::vector<int> Hits;

  bool jetOverflow = false;
  
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
  if (evtStore()->contains<t_jemDataContainer>(m_JetCMXDataLocation)) {
    StatusCode sc = evtStore()->retrieve(bpData, m_JetCMXDataLocation);  
    if ( sc==StatusCode::SUCCESS ) {
	
      // Analyse module results
      t_jemDataContainer::const_iterator it = bpData->begin();
      for ( ; it != bpData->end(); ++it) {
        int crate = (*it)->crate();
        std::vector<unsigned int> tobWords = (*it)->TopoTOBs();
    
        // Store data for L1Topo
        bool overflow = (*it)->overflow();
        if (overflow) {
          (*topoData)[crate]->setOverflow(true);
          jetOverflow = true;
        }
	
        for (std::vector<unsigned int>::const_iterator word = tobWords.begin();
	           word != tobWords.end(); ++word) {
	    
	  // Push back to Topo link
	  (*topoData)[crate]->addTOB( (*word) );
	   
	  // Decode TOB word 
	  JetTopoTOB tob( crate, (*word) );
          int etaindex = tob.etaIndex();	   
	  int ieta = 2*(etaindex-15) + (etaindex > 15 ? 0 : -1);
	  if (etaindex < 2 || etaindex > 28) {
            if (etaindex == 0)       ieta = -40;
	    else if (etaindex == 1)  ieta = -30;
	    else if (etaindex == 29) ieta =  29;
	    else if (etaindex >= 30) ieta =  39;
	  }
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
		else ATH_MSG_WARNING("Invalid threshold number " << num );
              } // passes cuts
		    
            } // JetThresholdValue pointer valid
          } // Loop over thresholds
		 
	} // Loop over TOBs
	    
      } // Loop over module results
      
      // Form CTP data objects
      unsigned int cableWord0 = 0;
      unsigned int cableWord1 = 0;

      if (!jetOverflow) {
        for (int i = 0; i < 10; ++i) cableWord0 |= ( Hits[i]<<(3*i) );
        for (int i = 10; i < 25; ++i) cableWord1 |= ( Hits[i]<<(2*(i-10)) );
      }
      else {
        cableWord0 = 0x3fffffff;
        cableWord1 = 0x3fffffff;
      }

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
  StatusCode sc = evtStore()->overwrite(CMXTobs, m_CMXJetTobLocation, true);
  if (sc != StatusCode::SUCCESS) ATH_MSG_WARNING ( "Problem writing CMXTobs to StoreGate" );

  sc = evtStore()->overwrite(CMXHits, m_CMXJetHitLocation, true);
  if (sc != StatusCode::SUCCESS) ATH_MSG_WARNING ( "Problem writing CMXHits to StoreGate" );

  // Store Topo results
  sc = evtStore()->overwrite(topoData, m_TopoOutputLocation, true);
  if (sc != StatusCode::SUCCESS) ATH_MSG_WARNING ( "Problem writing CPCMXTopoData object to StoreGate" );
 
  // Store CTP results
  if (m_jetCTP == 0) {
    m_jetCTP = new JetCTP(0,0);
    ATH_MSG_WARNING("No JetCTP found. Creating empty object" );
  }
  sc = evtStore()->overwrite(m_jetCTP, m_CTPOutputLocation, true);
  if (sc != StatusCode::SUCCESS) ATH_MSG_WARNING ( "Problem writing JetCTP object to StoreGate" );

  return StatusCode::SUCCESS ;
}



/** print trigger configuration, for debugging purposes */
void LVL1::JetCMX::printTriggerMenu(){
  /** This is all going to need updating for the new menu structure.
      Comment out in the meanwhile 
  
  L1DataDef def;

  std::vector<TrigConf::TriggerThreshold*> thresholds = m_configSvc->ctpConfig()->menu().thresholdVector();
  std::vector<TrigConf::TriggerThreshold*>::const_iterator it;
  for (it = thresholds.begin(); it != thresholds.end(); ++it) {
    if ( (*it)->type() == def.emType() || (*it)->type() == def.tauType() ) {
      ATH_MSG_DEBUG("TriggerThreshold " << (*it)->id() << " has name " << (*it)->name() << endmsg
          << "  threshold number " << (*it)->thresholdNumber() << endmsg
          << "  number of values = " << (*it)->numberofValues() );
      for (std::vector<TriggerThresholdValue*>::const_iterator tv = (*it)->thresholdValueVector().begin();
           tv != (*it)->thresholdValueVector().end(); ++tv) {
        ClusterThresholdValue* ctv;
        ctv = dynamic_cast<ClusterThresholdValue*> (*tv);
	if (!ctv) {
          ATH_MSG_ERROR("Threshold type name is EM/Tau, but is not a ClusterThreshold object!" );
          continue;
        }
        ATH_MSG_DEBUG("ClusterThresholdValue: " << endmsg
            << "  Threshold value = " << ctv->thresholdValueCount() << endmsg
            << "  EM isolation = " << ctv->emIsolationCount() << endmsg
            << "  Had isolation = " << ctv->hadIsolationCount() << endmsg
            << "  Had veto = " << ctv->hadVetoCount() << endmsg
            << "  EtaMin = " << ctv->etamin() << ", EtaMax = " << ctv->etamax() );
        
      } // end of loop over threshold values
    } //  is type == em or tau?
  } // end of loop over thresholds
  
  */   
}

} // end of namespace bracket

