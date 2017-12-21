
// /***************************************************************************
//                           CPCMX.cxx  -  description
//                              -------------------
//     begin                : Mon Jul 28 2014
//     copyright            : (C) 2014 by Alan Watson
//     email                : Alan.Watson@CERN.CH
//  ***************************************************************************/
//
//
//================================================
// CPCMX class Implementation
// ================================================
//
//
//

// Utilities

// This algorithm includes
#include "TrigT1CaloSim/CPCMX.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1CaloUtils/CoordToHardware.h"
#include "TrigConfL1Data/L1DataDef.h"

#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
#include "TrigT1CaloEvent/CMXCPHits_ClassDEF.h"
#include "TrigT1CaloEvent/CMXCPTob_ClassDEF.h"
#include "TrigT1CaloEvent/CPTopoTOB.h"
#include "TrigT1CaloEvent/CPCMXTopoData_ClassDEF.h"
#include "TrigT1CaloEvent/CPMCMXData.h"

#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/TriggerThresholdValue.h"
#include "TrigConfL1Data/ClusterThresholdValue.h"



namespace LVL1{

using namespace TrigConf;

const int CPCMX::s_SourceLocal;
const int CPCMX::s_SourceTotal;


//--------------------------------
// Constructors and destructors
//--------------------------------

CPCMX::CPCMX
  ( const std::string& name, ISvcLocator* pSvcLocator )
    : AthAlgorithm( name, pSvcLocator ), 
      m_configSvc("TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name),
      m_emTauCTP(0)
{
    m_CMXCPHitLocation      = TrigT1CaloDefs::CMXCPHitsLocation;
    m_CMXCPTobLocation      = TrigT1CaloDefs::CMXCPTobLocation;
    m_CPMCMXDataLocation    = TrigT1CaloDefs::CPMCMXDataLocation;
    m_TopoOutputLocation    = TrigT1CaloDefs::EmTauTopoTobLocation;
    m_CTPOutputLocation     = TrigT1CaloDefs::EmTauCTPLocation;

    // This is how you declare the paramembers to Gaudi so that
    // they can be over-written via the job options file
    
    declareProperty( "CMXCPHitLocation",        m_CMXCPHitLocation );
    declareProperty( "CMXCPTobLocation",        m_CMXCPTobLocation );
    declareProperty( "CPMCMXDataLocation",      m_CPMCMXDataLocation );
    declareProperty( "CTPOutputLocation",       m_CTPOutputLocation );
    declareProperty( "TopoOutputLocation",      m_TopoOutputLocation );
    declareProperty( "LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");

}


//---------------------------------
// initialise()
//---------------------------------

StatusCode CPCMX::initialize()
{
  ATH_CHECK( m_configSvc.retrieve() );
  return StatusCode::SUCCESS ;
}


//-------------------------------------------------
// Optional debug of menu at start of run
//-------------------------------------------------

StatusCode CPCMX::beginRun()
{
  if (msgLvl(MSG::DEBUG)) printTriggerMenu();
  return StatusCode::SUCCESS ;
}



//---------------------------------
// finalise()
//---------------------------------

StatusCode CPCMX::finalize()
{
   ATH_MSG_INFO("Finalizing" );
   return StatusCode::SUCCESS ;
}


//----------------------------------------------
// execute() method called once per event
//----------------------------------------------
//


StatusCode CPCMX::execute( )
{
    
  /*
    The idea is that this thing gets the inputs, formats them as required, then
    uses a port of the online code. Then formats outputs and sends them to the
    required destinations.
    
    If that doesn't work, we'll put the processing in here as well.
  */

  //make a message logging stream

  ATH_MSG_DEBUG ( "starting CPCMX" );
  
  /** Initialise pointer */
  m_emTauCTP = 0;

  /** Create containers for BS simulation */
  DataVector<CMXCPTob>*  CMXTobs = new DataVector<CMXCPTob>;
  DataVector<CMXCPHits>* CMXHits = new DataVector<CMXCPHits>;


  /** Create and initialise arrays for storing hit results */
  std::vector< std::string > triggerTypes;
  triggerTypes.push_back(L1DataDef::emType());
  triggerTypes.push_back(L1DataDef::tauType());
  
  std::vector< std::vector< std::vector<int> > > crateHits;
  std::vector< std::vector<int> > Hits;
  
  Hits.resize(2);
  Hits[0].resize(16);
  Hits[1].resize(16);
  
  crateHits.resize(4);
  for (int crate = 0; crate < 4; ++crate) {
    crateHits[crate].resize(2);
    for (int cmx = 0; cmx < 2; ++cmx) {
      crateHits[crate][cmx].resize(16);
    }
  }
  
  
  // Create objects to store TOBs for L1Topo
  DataVector<CPCMXTopoData>* topoData = new DataVector<CPCMXTopoData>;
  for (int crate = 0; crate < 4; ++crate) {
    for (int cmx = 0; cmx < 2; ++cmx) {
       CPCMXTopoData* link = new CPCMXTopoData(crate, cmx);
       topoData->push_back(link);
    }
  }

  // Overflow flags for EM and TAU RoIs (i.e. the 2 CMXes)
  bool cpmOverflow[2] = {false, false};

  /** Get EM and Tau Trigger Thresholds */
  std::vector< TrigConf::TriggerThreshold* > thresholds;
  std::vector< TrigConf::TriggerThreshold* > allThresholds = m_configSvc->ctpConfig()->menu().thresholdVector();
  for ( std::vector< TrigConf::TriggerThreshold* >::const_iterator it = allThresholds.begin();
        it != allThresholds.end(); ++it ) {
    if ( ( *it )->type() == L1DataDef::emType() || ( *it )->type() == L1DataDef::tauType() ) 
      thresholds.push_back( *it );
  }

  float cpScale = m_configSvc->thresholdConfig()->caloInfo().globalEmScale();


  /** Retrieve the CPCMXData (backplane data packages) */
  const t_cpmDataContainer* bpData;
  if (evtStore()->contains<t_cpmDataContainer>(m_CPMCMXDataLocation)) {
    StatusCode sc = evtStore()->retrieve(bpData, m_CPMCMXDataLocation);  
    if ( sc==StatusCode::SUCCESS ) {
	
      // Analyse module results
      t_cpmDataContainer::const_iterator it = bpData->begin();
      for ( ; it != bpData->end(); ++it) {
        int crate = (*it)->crate();
        int cmx   = (*it)->type();
        std::vector<unsigned int> tobWords = (*it)->TopoTOBs();
    
        // Store data for L1Topo
        int index = 2*crate + cmx;	
        bool overflow = (*it)->overflow();
        if (overflow) {
          (*topoData)[index]->setOverflow(true);
          cpmOverflow[cmx] = true;
        }
	
        for (std::vector<unsigned int>::const_iterator word = tobWords.begin();
             word != tobWords.end(); ++word) {
	    
          // Push back to Topo link
          (*topoData)[index]->addTOB( (*word) );
	   
          // Decode TOB word 
          CPTopoTOB tob( crate, cmx, (*word) );
          int ieta = tob.ieta() - 1;
          int iphi = tob.iphi();
          if (iphi < 0) iphi += 64;
          int et = tob.et();
          unsigned int isol = tob.isolation();
	   
          // Now check against trigger thresholds
          for ( std::vector< TriggerThreshold* >::const_iterator itTh = thresholds.begin();
                itTh != thresholds.end(); ++itTh ) {
            // Right type?
            if ( (*itTh)->type() != triggerTypes[cmx] ) continue;
            // Does TOB satisfy this threshold?
            TriggerThresholdValue* ttv = (*itTh)->triggerThresholdValue( ieta, iphi );
            ClusterThresholdValue* ctv = dynamic_cast< ClusterThresholdValue* >( ttv );
            if (ctv) {
              int etCut             = ctv->ptcut()*cpScale;
              unsigned int isolMask = ctv->isolationMask();
               
              bool isolationPassed = true;
              for (unsigned int bit = 0; bit < TrigT1CaloDefs::numOfIsolationBits; ++bit) 
                if ( (isolMask & (1<<bit)) && !(isol & (1<<bit)) ) isolationPassed = false;
               
              if ( et > etCut && isolationPassed ) {		
                int num = ( *itTh )->thresholdNumber();
                if (num < 16) {
                  if (crateHits[crate][cmx][num] < 7) crateHits[crate][cmx][num]++;
                  if (Hits[cmx][num] < 7)             Hits[cmx][num]++;
                }
                else ATH_MSG_WARNING("Invalid threshold number " << num );
              } // passes cuts
		    
            } // ClusterThresholdValue pointer valid
          } // Loop over thresholds
		 
        } // Loop over TOBs
	    
      } // Loop over module results

      // Overflow sets all trigger bits
      for (int cmx = 0; cmx < 2; ++cmx) {
        if (cpmOverflow[cmx]) {
          for (int i = 0; i < 16; ++i) Hits[cmx][i] = 7;
        }
      }
      
      // Form CTP data objects
      unsigned int cableWord0 = 0;
      unsigned int cableWord1 = 0;
      unsigned int cableWord2 = 0;
      unsigned int cableWord3 = 0;
      for (int i = 0; i < 8; ++i) {
        cableWord0 |= ( Hits[0][i]<<(3*i) );
        cableWord1 |= ( Hits[0][i+8]<<(3*i) );
        cableWord2 |= ( Hits[1][i]<<(3*i) );
        cableWord3 |= ( Hits[1][i+8]<<(3*i) );
      }

      m_emTauCTP = new EmTauCTP( cableWord0, cableWord1, cableWord2, cableWord3 );
      
      // Form and store CPCMXHits
      std::vector<int> error0;  // Dummies - there will be no actual errors simulated
      std::vector<int> error1;
      
      // Now form hits words from module results and insert into objects
      std::vector<unsigned int> cratehits0;
      std::vector<unsigned int> cratehits1;
      const int peak = 0;
      const int system_crate = 3;

      // Crate sums (local and remote)  
      for (int crate = 0; crate < 4; ++crate) {
        for (int cmx = 0; cmx < 2; ++cmx) {
          cratehits0.assign(1,0);
          cratehits1.assign(1,0);
          for (int i = 0; i < 8; ++i) {
            cratehits0[0] |= ( crateHits[crate][cmx][i]<<(3*i) );
            cratehits1[0] |= ( crateHits[crate][cmx][i+8]<<(3*i) );
          }
          CMXCPHits* crateCMXHits = new CMXCPHits(crate, cmx, LVL1::CMXCPHits::LOCAL,
      		                                  cratehits0, cratehits1, error0, error1, peak);
          CMXHits->push_back(crateCMXHits);
          if (crate != system_crate) {
            CMXCPHits* remoteCMXHits = new CMXCPHits(system_crate, cmx, crate,
	  	                                     cratehits0, cratehits1, error0, error1, peak);
            CMXHits->push_back(remoteCMXHits);
          }
        } // loop over CMXes
      } // loop over crates
      
      // global sums
      cratehits0.assign(1,cableWord0);
      cratehits1.assign(1,cableWord1);
      CMXCPHits* totalCMXHits0 = new CMXCPHits(system_crate, 0, LVL1::CMXCPHits::TOTAL,
					       cratehits0, cratehits1, error0, error1, peak);
      cratehits0.assign(1,cableWord2);
      cratehits1.assign(1,cableWord3);
      CMXCPHits* totalCMXHits1 = new CMXCPHits(system_crate, 1, LVL1::CMXCPHits::TOTAL,
					       cratehits0, cratehits1, error0, error1, peak);
      CMXHits->push_back(totalCMXHits0);
      CMXHits->push_back(totalCMXHits1);
      
    } // Successfully read input data
 
  } // Input collection exists in StoreGate


  // Store output for BS simulation
  StatusCode sc = evtStore()->overwrite(CMXTobs, m_CMXCPTobLocation, true);
  if (sc != StatusCode::SUCCESS) ATH_MSG_WARNING ( "Problem writeing CMXTobs to StoreGate" );

  sc = evtStore()->overwrite(CMXHits, m_CMXCPHitLocation, true);
  if (sc != StatusCode::SUCCESS) ATH_MSG_WARNING ( "Problem writeing CMXHits to StoreGate" );

  // Store Topo results
  sc = evtStore()->overwrite(topoData, m_TopoOutputLocation, true);
  if (sc != StatusCode::SUCCESS) ATH_MSG_WARNING ( "Problem writeing CPCMXTopoData object to StoreGate" );

  // Store CTP results
  if (m_emTauCTP == 0) {
    m_emTauCTP = new EmTauCTP(0,0,0,0);
    ATH_MSG_WARNING("No EmTauCTP found. Creating empty object" );
  }
  sc = evtStore()->overwrite(m_emTauCTP, m_CTPOutputLocation, true);
  if (sc != StatusCode::SUCCESS) ATH_MSG_WARNING ( "Problem writeing EmTauCTP object to StoreGate" );

  return StatusCode::SUCCESS ;
}



/** print trigger configuration, for debugging purposes */
void LVL1::CPCMX::printTriggerMenu(){
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

