/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// /***************************************************************************
//                           JetCMX.cxx  -  description
//                              -------------------
//     begin                : Mon Jul 28 2014
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
#include "JetCMX.h"
#include "TrigT1CaloUtils/CoordToHardware.h"
#include "TrigConfL1Data/L1DataDef.h"

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

JetCMX::JetCMX(const std::string& name, ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator) {}

//---------------------------------
// initialise()
//---------------------------------

StatusCode JetCMX::initialize()
{
  ATH_CHECK( m_configSvc.retrieve() );
  ATH_CHECK( m_JetCMXDataLocation.initialize() );
  ATH_CHECK( m_CMXJetHitsLocation.initialize() );
  ATH_CHECK( m_CMXJetTobLocation.initialize() );
  ATH_CHECK( m_TopoOutputLocation.initialize() );
  ATH_CHECK( m_CTPOutputLocation.initialize() );
  return StatusCode::SUCCESS ;
}

//----------------------------------------------
// execute() method called once per event
//----------------------------------------------

StatusCode JetCMX::execute(const EventContext& ctx) const
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
  SG::WriteHandle<CMXJetHitsCollection> CMXHits = SG::makeHandle(m_CMXJetHitsLocation, ctx);
  ATH_CHECK(CMXHits.record(std::make_unique<CMXJetHitsCollection>()));
  SG::WriteHandle<CMXJetTobCollection> CMXTobs = SG::makeHandle(m_CMXJetTobLocation, ctx);
  ATH_CHECK(CMXTobs.record(std::make_unique<CMXJetTobCollection>()));
  SG::WriteHandle<JetCMXTopoDataCollection> topoData = SG::makeHandle(m_TopoOutputLocation, ctx);
  ATH_CHECK(topoData.record(std::make_unique<JetCMXTopoDataCollection>()));


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
  for (int crate = 0; crate < 2; ++crate) {
    topoData->push_back(std::make_unique<JetCMXTopoData>(crate));
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
  SG::ReadHandle<JetCMXDataCollection> bpDataVec = SG::makeHandle(m_JetCMXDataLocation, ctx);
  if (bpDataVec.isValid()) {
      // Analyse module results
      for (const LVL1::JetCMXData* bpData : *bpDataVec) {
        int crate = bpData->crate();
        std::vector<unsigned int> tobWords = bpData->TopoTOBs();
    
        // Store data for L1Topo
        bool overflow = bpData->overflow();
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

      SG::WriteHandle<JetCTP> jetCTP = SG::makeHandle(m_CTPOutputLocation, ctx);
      ATH_CHECK(jetCTP.record(std::make_unique<JetCTP>(cableWord0, cableWord1)));
      
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
        CMXHits->push_back(std::make_unique<CMXJetHits>(crate, LVL1::CMXJetHits::LOCAL_MAIN,
                                                        cratehits0, cratehits1, error0, error1, peak));
	if (crate != system_crate) {
          CMXHits->push_back(std::make_unique<CMXJetHits>(system_crate, LVL1::CMXJetHits::REMOTE_MAIN,
                                                          cratehits0, cratehits1, error0, error1, peak));
	}
	  
	cratehits0.assign(1,0);
	cratehits1.assign(1,0);
	for (int i = 0; i < 8; ++i) cratehits0[0] |= ( crateHits[crate][i+10]<<(2*i) );
	for (int i = 0; i < 7; ++i) cratehits1[0] |= ( crateHits[crate][i+18]<<(2*i) );
        CMXHits->push_back(std::make_unique<CMXJetHits>(crate, LVL1::CMXJetHits::LOCAL_FORWARD,
                                                        cratehits0, cratehits1, error0, error1, peak));
        if (crate != system_crate) {
          CMXHits->push_back(std::make_unique<CMXJetHits>(system_crate, LVL1::CMXJetHits::REMOTE_FORWARD,
                                                          cratehits0, cratehits1, error0, error1, peak));
	}
      } // loop over crates
      
      // global sums
      cratehits0.assign(1,0);
      cratehits1.assign(1,0);
      for (int i = 0; i < 5; ++i) {
	cratehits0[0] |= ( Hits[i]<<(3*i) );
	cratehits1[0] |= ( Hits[i+5]<<(3*i) );
      }
      CMXHits->push_back(std::make_unique<CMXJetHits>(system_crate, LVL1::CMXJetHits::TOTAL_MAIN,
                                                      cratehits0, cratehits1, error0, error1, peak));

      cratehits0.assign(1,0);
      cratehits1.assign(1,0);
      for (int i = 0; i < 8; ++i) cratehits0[0] |= ( Hits[i+10]<<(2*i) );
      for (int i = 0; i < 7; ++i) cratehits1[0] |= ( Hits[i+18]<<(2*i) );
      CMXHits->push_back(std::make_unique<CMXJetHits>(system_crate, LVL1::CMXJetHits::TOTAL_FORWARD,
                                                      cratehits0, cratehits1, error0, error1, peak));
      
  } // Input collection exists in StoreGate
  else {
    ATH_MSG_WARNING("No JetCTP found. Creating empty object" );
    SG::WriteHandle<JetCTP> jetCTP = SG::makeHandle(m_CTPOutputLocation, ctx);
    ATH_CHECK(jetCTP.record(std::make_unique<JetCTP>(0, 0)));
  }

  return StatusCode::SUCCESS ;
}



/** print trigger configuration, for debugging purposes */
void LVL1::JetCMX::printTriggerMenu() const {
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

