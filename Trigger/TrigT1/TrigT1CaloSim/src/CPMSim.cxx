/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// /***************************************************************************
//                           CPMSim.cxx  -  description
//                              -------------------
//     begin                : Wed Mar 05 2014
//     email                : Alan.Watson@CERN.CH
//  ***************************************************************************/
//
//
//================================================
// CPMSim class Implementation
// ================================================
//
//
//

// Utilities

// This algorithm includes
#include "TrigT1CaloSim/CPMSim.h"
#include "TrigT1CaloUtils/CoordToHardware.h"
#include "TrigConfL1Data/L1DataDef.h"

#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
#include "xAODTrigL1Calo/CPMTowerContainer.h"
#include "TrigT1CaloEvent/CPMTobRoI_ClassDEF.h"
#include "TrigT1CaloEvent/CPMCMXData_ClassDEF.h"

#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/TriggerThresholdValue.h"
#include "TrigConfL1Data/ClusterThresholdValue.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"



namespace LVL1{

using namespace TrigConf;

//--------------------------------
// Constructors and destructors
//--------------------------------

CPMSim::CPMSim
  ( const std::string& name, ISvcLocator* pSvcLocator )
    : AthReentrantAlgorithm( name, pSvcLocator ), 
      //m_emTauSlinkLocation(TrigT1CaloDefs::EmTauSlinkLocation),
      m_configSvc("TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name),
      m_CPMTool("LVL1::L1CPMTools/L1CPMTools")
{
    declareProperty( "LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");
    //declareProperty( "EmTauSlinkLocation", m_emTauSlinkLocation );
}


//---------------------------------
// initialise()
//---------------------------------

StatusCode CPMSim::initialize()
{
  ATH_CHECK( m_configSvc.retrieve() );
  ATH_CHECK( m_CPMTool.retrieve() );
  ATH_CHECK( m_CPMTowerLocation.initialize() );
  ATH_CHECK( m_CPMCMXDataLocation.initialize() );
  ATH_CHECK( m_CPMTobRoILocation.initialize() );

#if 0
  m_emTauSlinkKeys.clear();
  m_emTauSlinkKeys.push_back (m_emTauSlinkLocation + "0");
  m_emTauSlinkKeys.push_back (m_emTauSlinkLocation + "1");
  m_emTauSlinkKeys.push_back (m_emTauSlinkLocation + "2");
  m_emTauSlinkKeys.push_back (m_emTauSlinkLocation + "3");
  ATH_CHECK( m_emTauSlinkKeys.initialize() );
#endif
  return StatusCode::SUCCESS ;
}


//----------------------------------------------
// execute() method called once per event
//----------------------------------------------
//


StatusCode CPMSim::execute(const EventContext& ctx) const
{
    
  /* Retrieve collection of CPMTowers
     Form TOBs from them
     Form backplane data from TOBs
     Store TOBs for BS simulation
     Form and store RoI SLink data for RoIB
  */

  ATH_MSG_DEBUG ( "starting CPMSim" ); 

  // Create containers for this event
  auto CMXData    = std::make_unique<DataVector<CPMCMXData> >();  //Container of backplane data objects 
  auto allTOBs    = std::make_unique<DataVector<CPMTobRoI> >();  // Container to hold all TOB RoIs in event

  SG::ReadHandle<xAOD::CPMTowerContainer> storedCPMTs
    (m_CPMTowerLocation, ctx);

  if (storedCPMTs->size() == 0)
    ATH_MSG_WARNING("Empty CPMTowerContainer - looks like a problem" );
	
  // Map the CPMTs
  xAOD::CPMTowerMap_t towerMap;
  m_CPMTool->mapTowers(storedCPMTs.cptr(), &towerMap);
	 
  // Loop over crates and modules
  for (int iCrate = 0; iCrate < 4; ++iCrate) {
    for (int iModule = 1; iModule <= 14; ++iModule) {
	    
      // For each module, find TOBs and backplane data
      std::vector<unsigned int> emCMXData;
      std::vector<unsigned int> tauCMXData;
      m_CPMTool->findCPMResults(&towerMap,iCrate,iModule,allTOBs.get(),emCMXData,tauCMXData);
      // Push backplane data into output DataVectors
      CMXData -> push_back(std::make_unique<CPMCMXData>(iCrate,iModule,0,emCMXData));
      CMXData -> push_back(std::make_unique<CPMCMXData>(iCrate,iModule,1,tauCMXData));

    } // loop over modules
  } // loop over crates
      
     
  // Store RoIB output, module readout and backplane results in the TES
  ATH_CHECK( SG::makeHandle(m_CPMCMXDataLocation, ctx).record (std::move (CMXData)) );
  //ATH_CHECK( storeSlinkObjects (*allTOBs, ctx) );
  ATH_CHECK( SG::makeHandle(m_CPMTobRoILocation, ctx).record (std::move (allTOBs)) );
  
  return StatusCode::SUCCESS ;
}


#if 0
/** Simulate SLink streams to RoIB */
StatusCode CPMSim::storeSlinkObjects (const DataVector<CPMTobRoI>& allTOBs,
                                      const EventContext& ctx) const
{
  ATH_MSG_DEBUG("storeSlinkObjects" );

   /** there are 4 CP RoI RODs which have a Slink cable connected to the RoIB.
    This array holds pointers to 4 DataVectors containing the Slink words  */
  std::unique_ptr<DataVector<LVL1CTP::SlinkWord> > CPRoIROD[TrigT1CaloDefs::numOfCPRoIRODs];

  /// Create words and add headers
  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfCPRoIRODs;i++){
    CPRoIROD[i] = std::make_unique<DataVector<LVL1CTP::SlinkWord> >();
    addHeader(*CPRoIROD[i],0x72,i, ctx);//set modID to crate #
  }
  
  /// Push TOBs to ROD vectors
  /// This version makes no attempt to order them.
  for (const CPMTobRoI* tob : allTOBs) {
    int crate = tob->crate();
    CPRoIROD[crate]->push_back( getWord(tob->roiWord()) );
  }
  
  /// Add tails
  const unsigned int headerWords=8+1;//8 in Slink head + control word
  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfCPRoIRODs;i++){
    addTail(*CPRoIROD[i], (CPRoIROD[i]->size()-headerWords) );
  }
  
  /** TEMPORARY: dump contents to log */
  /*
  for (unsigned int slink=0; slink<(TrigT1CaloDefs::numOfCPRoIRODs);slink++){
    ATH_MSG_INFO("Slink cable  "<<slink
        <<" has "<<(CPRoIROD[slink]->size())<<" words");
    for (DataVector<LVL1CTP::SlinkWord>::iterator i=CPRoIROD[slink]->begin();
                                      i!=CPRoIROD[slink]->end();++i){
      ATH_MSG_INFO<<"Word :"<<hex<<(*i)->word()<<dec);
    }
  }
  */

  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfCPRoIRODs;++i){
    ATH_CHECK( SG::makeHandle (m_emTauSlinkKeys[i], ctx).record (std::move (CPRoIROD[i])) );
  }
  return StatusCode::SUCCESS;
}

/** adds slink header as defined in DAQ-98-129*/
void CPMSim::addHeader (DataVector<LVL1CTP::SlinkWord>& slink,
                        unsigned int subDetID,
                        unsigned int moduleId,
                        const EventContext& ctx) const
{
    slink.push_back(getWord(0xb0f00000));      // beginning of fragment word (control word)
    slink.push_back(getWord(0xee1234ee));      // start of header marker
    slink.push_back(getWord(8));               // header length
    slink.push_back(getWord(4));               // format version number
    unsigned int moduleType=0x00;  // ROD
    slink.push_back(getWord( (moduleType<<16)+(subDetID<<8)+moduleId ));  // 1st 8b but source id: CP system
    slink.push_back(getWord(ctx.eventID().event_number()));       // 8b ECR_count (set to 0) | 24b L1 id = evt number
    slink.push_back( getWord(1 ));                 // 8b BCID
    slink.push_back( getWord(1 ));                 // 8b L1 trig type
    slink.push_back( getWord(0 ));                 // detector evt type
}

/** add Slink tail */
void CPMSim::addTail (DataVector<LVL1CTP::SlinkWord>& slink,
                      unsigned int numberOfDataWords) const
{
  slink.push_back(getWord(0 ));                           // write status word 1
  slink.push_back(getWord(0 ));                           // write status word 2
//^^^ not simulating any errors.

  slink.push_back(getWord(2 ));                           // status block length
  slink.push_back(getWord(numberOfDataWords));            // calculated data block length
  slink.push_back(getWord(1 ));                           // status block position
// ^^^ indicates that data precedes status block
  slink.push_back(getWord(0xe0f00000 ));                  // end of fragment word
}


/** creates a new SlinkWord object with the passed word, and returns a pointer.*/
std::unique_ptr<LVL1CTP::SlinkWord>
CPMSim::getWord(unsigned int tword) const 
{
  return std::make_unique<LVL1CTP::SlinkWord>(tword);
}
#endif


} // end of LVL1 namespace bracket


/** print trigger configuration, for debugging purposes */
void LVL1::CPMSim::printTriggerMenu() const {
  
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


