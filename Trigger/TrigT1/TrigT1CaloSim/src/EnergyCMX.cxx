/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// /***************************************************************************
//                           EnergyCMX.cxx  -  description
//                              -------------------
 //     begin                : 04 04 2014
 //     email                : Alan.Watson@cern.ch
//  ***************************************************************************/
//
//================================================
// EnergyCMX class Implementation
// ================================================
//
//
//
//

// Utilities
#include <cmath>


// This algorithm includes
#include "EnergyCMX.h"

#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/CTPConfig.h"

using namespace TrigConf;

namespace LVL1 {


//--------------------------------
// Constructors and destructors
//--------------------------------

EnergyCMX::EnergyCMX (const std::string& name, ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm( name, pSvcLocator ) {}


//---------------------------------
// initialise()
//---------------------------------

StatusCode EnergyCMX::initialize()
{
  ATH_CHECK( m_configSvc.retrieve() );
  ATH_CHECK( m_EtTool.retrieve() );
  ATH_CHECK( m_energyCMXDataLocation.initialize() );
  ATH_CHECK( m_energyCTPLocation.initialize() );
  ATH_CHECK( m_energyTopoLocation.initialize() );
  ATH_CHECK( m_cmxEtsumsLocation.initialize() );
  ATH_CHECK( m_cmxRoILocation.initialize() );
  return StatusCode::SUCCESS ;
}


//-------------------------------------------------
// Optional debug of menu at start of run
//-------------------------------------------------

StatusCode EnergyCMX::start()
{
  setupTriggerMenuFromCTP();

  return StatusCode::SUCCESS ;
}


//----------------------------------------------
// execute() method called once per event
//----------------------------------------------
//


StatusCode EnergyCMX::execute(const EventContext& ctx) const
{
  ATH_MSG_DEBUG( "Executing" );
   
  // form module sums
  SG::ReadHandle<EnergyCMXDataCollection> jemContainer = SG::makeHandle(m_energyCMXDataLocation, ctx);
      
  // form crate sums (full eta range)
  DataVector<CrateEnergy>* cratesFull  = new DataVector<CrateEnergy>;
  m_EtTool->crateSums(jemContainer.cptr(), cratesFull);
  // system summation and threshold tests
  const SystemEnergy resultsFull = m_EtTool->systemSums(cratesFull);
  
  /** Find restructed eta range.
   *  This will use the min/max values for the first valid threshold in the range 9-16 to define the ranges
   */
  uint32_t maskXE =  0;
  uint32_t maskTE =  0;
  bool maskXESet = false;
  bool maskTESet = false;
  const float moduleEta[8] = {-4.,-2.,-1.2,-0.4,0.4,1.2,2.,4.};
  
  L1DataDef def;
  std::vector<TriggerThreshold*> thresholds = m_configSvc->ctpConfig()->menu().thresholdVector();
  std::vector<TriggerThreshold*>::const_iterator it;
  
  for (it = thresholds.begin(); it != thresholds.end(); ++it) {
    if ( ( (*it)->type() == def.xeType() || (*it)->type() == def.teType()) && (*it)->thresholdNumber() > 7 ) {
      std::vector<TriggerThresholdValue*> ttvs = (*it)->thresholdValueVector();
      std::vector<TriggerThresholdValue*>::const_iterator itv;
      // Make sure only set masks from the first valid threshold in the range (for each type)       
      if (maskXE > 0) maskXESet = true;
      if (maskTE > 0) maskTESet = true;
      for (itv = ttvs.begin(); itv != ttvs.end(); ++itv) {
        // Bits are set false by default, so ignore thresholds that are just doing that
        if ((*itv)->thresholdValueCount() >= 0x7fff) continue;
        // Set bits true if module centre between etaMin and etaMax
        if ( (*it)->type() == def.xeType()  && !maskXESet ) {
          for (unsigned int bin = 0; bin < 8; ++bin) {
            if (moduleEta[bin] > (*itv)->etamin()*0.1 && moduleEta[bin] < (*itv)->etamax()*0.1)
                maskXE |= (1<<bin);
          }
        }
        else if ( (*it)->type() == def.teType()  && !maskTESet ) {
          for (unsigned int bin = 0; bin < 8; ++bin) {
            if (moduleEta[bin] > (*itv)->etamin()*0.1 && moduleEta[bin] < (*itv)->etamax()*0.1)
                maskTE |= (1<<bin);
          }
        }
      }  // loop over TTV
    } // Is this XE or TE threshold?
  }  // Loop over thresholds
  
  // form crate sums (restricted eta range). Explicitly set restricted eta flag regardless of eta range
  DataVector<CrateEnergy>* cratesTrunc  = new DataVector<CrateEnergy>;
  m_EtTool->crateSums(jemContainer.cptr(), cratesTrunc, maskXE, maskTE, true);
  // system summation and threshold tests
  const SystemEnergy resultsTrunc = m_EtTool->systemSums(cratesTrunc);
  
  // CTP Data
  ATH_CHECK(saveCTPObjects(resultsFull, resultsTrunc, ctx));
  
  // RoI output
  ATH_CHECK(saveRoIs(resultsFull, resultsTrunc, ctx));
  
  // Module readout simulation
  SG::WriteHandle<CMXEtSumsCollection> CMXSums = SG::makeHandle(m_cmxEtsumsLocation, ctx);
  ATH_CHECK(CMXSums.record(std::make_unique<CMXEtSumsCollection>()));

  std::vector<unsigned int> exVec;
  std::vector<unsigned int> eyVec;
  std::vector<unsigned int> etVec;
  std::vector<int> exErr;
  std::vector<int> eyErr;
  std::vector<int> etErr;
  exErr.assign(1,0);
  eyErr.assign(1,0);
  etErr.assign(1,0);
  const int peak = 0;
  const int system_crate = 1;
  
  // Full eta range
  DataVector<CrateEnergy>::const_iterator itCrate = cratesFull->begin();
  for ( ; itCrate != cratesFull->end(); ++itCrate) {
    exVec.clear();
    exVec.push_back((*itCrate)->exTC());
    eyVec.clear();
    eyVec.push_back((*itCrate)->eyTC());
    etVec.clear();
    etVec.push_back((*itCrate)->et());
    int crate = (*itCrate)->crate();
    CMXEtSums* crateEtSumFull = new CMXEtSums(crate, LVL1::CMXEtSums::LOCAL_STANDARD,
					      exVec, eyVec, etVec, exErr, eyErr, etErr, peak);
    CMXSums->push_back(crateEtSumFull);
    
    if (crate != system_crate) {
      CMXEtSums* remoteEtSumFull = new CMXEtSums(system_crate, LVL1::CMXEtSums::REMOTE_STANDARD,
					         exVec, eyVec, etVec, exErr, eyErr, etErr, peak);
      CMXSums->push_back(remoteEtSumFull); 
    }
       
  }
  
  exVec.clear();
  exVec.push_back(resultsFull.exTC());
  eyVec.clear();
  eyVec.push_back(resultsFull.eyTC());
  etVec.clear();
  etVec.push_back(resultsFull.et());
  CMXEtSums* systemEtSumFull = new CMXEtSums(system_crate, LVL1::CMXEtSums::TOTAL_STANDARD,
					     exVec, eyVec, etVec, exErr, eyErr, etErr, peak);
  CMXSums->push_back(systemEtSumFull); 
  
  // Restricted eta range
  DataVector<CrateEnergy>::const_iterator itTrunc = cratesTrunc->begin();
  for ( ; itTrunc != cratesTrunc->end(); ++itTrunc) {
    exVec.clear();
    exVec.push_back((*itTrunc)->exTC());
    eyVec.clear();
    eyVec.push_back((*itTrunc)->eyTC());
    etVec.clear();
    etVec.push_back((*itTrunc)->et());
    int crate = (*itTrunc)->crate();
    CMXEtSums* crateEtSumTrunc = new CMXEtSums(crate, LVL1::CMXEtSums::LOCAL_RESTRICTED,
					      exVec, eyVec, etVec, exErr, eyErr, etErr, peak);
    CMXSums->push_back(crateEtSumTrunc);
    
    if (crate != system_crate) {
      CMXEtSums* remoteEtSumTrunc = new CMXEtSums(system_crate, LVL1::CMXEtSums::REMOTE_RESTRICTED,
					         exVec, eyVec, etVec, exErr, eyErr, etErr, peak);
      CMXSums->push_back(remoteEtSumTrunc); 
    }
       
  }
  
  exVec.clear();
  exVec.push_back(resultsTrunc.exTC());
  eyVec.clear();
  eyVec.push_back(resultsTrunc.eyTC());
  etVec.clear();
  etVec.push_back(resultsTrunc.et());
  CMXEtSums* systemEtSumTrunc = new CMXEtSums(system_crate, LVL1::CMXEtSums::TOTAL_RESTRICTED,
					      exVec, eyVec, etVec, exErr, eyErr, etErr, peak);

  CMXSums->push_back(systemEtSumTrunc); 

  // Topo data
  SG::WriteHandle<EnergyTopoData> topoData = SG::makeHandle(m_energyTopoLocation, ctx);
  ATH_CHECK(topoData.record(std::make_unique<EnergyTopoData>()));
  
  topoData->addEx(resultsFull.exTC(), resultsFull.exOverflow(), LVL1::EnergyTopoData::Normal);
  topoData->addEy(resultsFull.eyTC(), resultsFull.eyOverflow(), LVL1::EnergyTopoData::Normal);
  topoData->addEt(resultsFull.et(),   resultsFull.etOverflow(), LVL1::EnergyTopoData::Normal);
  
  topoData->addEx(resultsTrunc.exTC(), resultsTrunc.exOverflow(), LVL1::EnergyTopoData::Restricted);
  topoData->addEy(resultsTrunc.eyTC(), resultsTrunc.eyOverflow(), LVL1::EnergyTopoData::Restricted);
  topoData->addEt(resultsTrunc.et(),   resultsTrunc.etOverflow(), LVL1::EnergyTopoData::Restricted);

  // tidy up at end of event
  delete cratesFull;
  delete cratesTrunc;
 
  return StatusCode::SUCCESS ;
}

} // end of LVL1 namespace bracket


/** retrieves the Calo config put into detectorstore by TrigT1CTP and set up trigger menu */
void LVL1::EnergyCMX::setupTriggerMenuFromCTP(){
  ATH_MSG_DEBUG("Loading Trigger Menu");
  
  std::vector<TriggerThreshold*> thresholds = m_configSvc->ctpConfig()->menu().thresholdVector();
  std::vector<TriggerThreshold*>::const_iterator it;
  for (it = thresholds.begin(); it != thresholds.end(); ++it) {

    /// Debug printout of menu
    if ( (*it)->type() == m_def.xeType() || (*it)->type() == m_def.teType() || (*it)->type() == m_def.xsType() ) {
     TriggerThresholdValue* tv = (*it)->triggerThresholdValue(0,0); // ET trigger thresholds can only have one global value
     ATH_MSG_DEBUG("TriggerThreshold " << (*it)->name() << " has:" << endmsg
         << "--------------------------" << endmsg
         << " Number   = " << (*it)->thresholdNumber() << endmsg
         << " Type     = " << (*it)->type() << endmsg
         << " Value    = " << (*tv).thresholdValueCount() );
	         
    } //  is type == energySum trigger
  } // end of loop over thresholds
  
  /// Should check whether this is still necessary
  m_def.setNewJEP3Cabling();

  return;
}

/** form CMXRoI & save in SG */
StatusCode LVL1::EnergyCMX::saveRoIs(const SystemEnergy& resultsFull,
                                     const SystemEnergy& resultsTrunc,
                                     const EventContext& ctx) const {
  ATH_MSG_DEBUG("saveRoIs");

  // copy values into roi words
  unsigned int roiWord0 = resultsFull.roiWord0();
  unsigned int roiWord2 = resultsFull.roiWord1();
  unsigned int roiWord4 = resultsFull.roiWord2();
  
  // Truncated eta range
  unsigned int roiWord1 = resultsTrunc.roiWord0();
  unsigned int roiWord3 = resultsTrunc.roiWord1();
  unsigned int roiWord5 = resultsTrunc.roiWord2();
 
  // DAQ readout object. 
  SG::WriteHandle<CMXRoI> daqRoI = SG::makeHandle(m_cmxRoILocation, ctx);
  ATH_CHECK(daqRoI.record(std::make_unique<CMXRoI>()));

  // Add data to RoI object. The object will perform format checks on inputs
  bool added = daqRoI->setRoiWord(roiWord0);
  if (!added) ATH_MSG_WARNING("Failed to add RoI Word 0: " << MSG::hex << roiWord0 << MSG::dec);
  added = daqRoI->setRoiWord(roiWord1);
  if (!added) ATH_MSG_WARNING("Failed to add RoI Word 1: " << MSG::hex << roiWord1 << MSG::dec);
  added = daqRoI->setRoiWord(roiWord2);
  if (!added) ATH_MSG_WARNING( "Failed to add RoI Word 2: " << MSG::hex << roiWord2 << MSG::dec );
  added = daqRoI->setRoiWord(roiWord3);
  if (!added) ATH_MSG_WARNING( "Failed to add RoI Word 3: " << MSG::hex << roiWord3 << MSG::dec );
  added = daqRoI->setRoiWord(roiWord4);
  if (!added) ATH_MSG_WARNING( "Failed to add RoI Word 4: " << MSG::hex << roiWord4 << MSG::dec );
  added = daqRoI->setRoiWord(roiWord5);
  if (!added) ATH_MSG_WARNING( "Failed to add RoI Word 5: " << MSG::hex << roiWord5 << MSG::dec );

  return StatusCode::SUCCESS;
}

/** form CTP word from ETmiss and ETsum hits */
unsigned int LVL1::EnergyCMX::ctpWord(unsigned int metSigPassed,
                                      unsigned int etMissPassed,
                                      unsigned int etSumPassed) const {

  return ( (metSigPassed<<(m_def.max_TE_Threshold_Number()+m_def.max_XE_Threshold_Number())) +
	   (etMissPassed<<m_def.max_TE_Threshold_Number()) + etSumPassed );
}

/** form CTP objects and store them in SG. */
StatusCode LVL1::EnergyCMX::saveCTPObjects(const SystemEnergy& resultsFull,
                                           const SystemEnergy& resultsTrunc,
                                           const EventContext& ctx) const {
  ATH_MSG_DEBUG("saveCTPObjects");

  // get bit words of thresholds passed
  unsigned int etSumHitsFull   = resultsFull.etSumHits();
  unsigned int etMissHitsFull  = resultsFull.etMissHits();
  unsigned int metSigHitsFull  = resultsFull.metSigHits();
  unsigned int etSumHitsTrunc  = resultsTrunc.etSumHits();
  unsigned int etMissHitsTrunc = resultsTrunc.etMissHits();

  // form CTP words
  unsigned int word0 = ctpWord(metSigHitsFull, etMissHitsFull, etSumHitsFull);
  unsigned int word1 = ctpWord(0, etMissHitsTrunc, etSumHitsTrunc);
  // Comment out parity generation for now - should be in bit 32 of a 34 bit word
  //Parity genParity;
  //temp += (genParity.odd(temp)<<31);

  // form CTP object
  SG::WriteHandle<EnergyCTP> energyCTP = SG::makeHandle(m_energyCTPLocation, ctx);
  ATH_CHECK(energyCTP.record(std::make_unique<EnergyCTP>(word0, word1)));
  ATH_MSG_DEBUG( "Stored energy CTP object with words "<< std::hex
    << (energyCTP->cableWord0()) << ", " <<  (energyCTP->cableWord1())<< std::dec);

  return StatusCode::SUCCESS;
}
