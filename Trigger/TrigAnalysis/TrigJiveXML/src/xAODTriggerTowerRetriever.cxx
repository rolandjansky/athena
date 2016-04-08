/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigJiveXML/xAODTriggerTowerRetriever.h"

#include <string>

#include "CLHEP/Units/SystemOfUnits.h"

#include "xAODTrigL1Calo/TriggerTowerContainer.h"

namespace JiveXML {

  //--------------------------------------------------------------------------

  xAODTriggerTowerRetriever::xAODTriggerTowerRetriever(const std::string& type, const std::string& name, const IInterface* parent):
    AthAlgTool(type, name, parent), m_typeName("LVL1TriggerTower")
  {
    declareInterface<IDataRetriever>(this);
    declareProperty("StoreGateKey", m_sgKey = "xAODTriggerTowers",  "Storegate key for Level-1 TriggerTowers");
  }

  //--------------------------------------------------------------------------

  StatusCode xAODTriggerTowerRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {

    //Retrieve trigger towers
    const DataVector<xAOD::TriggerTower>* TTVector;
        
    if ( evtStore()->retrieve(TTVector,m_sgKey).isFailure() ) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<  "No Level-1 Trigger Towers found in SG at "
          << m_sgKey << endreq;
      return StatusCode::SUCCESS;
    } 
    
    //Get number of trigger towers
    int numTrigTowers = TTVector->size();
    int count = 0;
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Retrieving xAOD Level-1 Trigger Towers with size " << numTrigTowers << endreq;

    DataVect phi; phi.reserve(numTrigTowers);
    DataVect eta; eta.reserve(numTrigTowers);
    DataVect emEnergyVec; emEnergyVec.reserve(numTrigTowers);
    DataVect hadEnergyVec; hadEnergyVec.reserve(numTrigTowers);

    DataVect sumEnergyVec; sumEnergyVec.reserve(numTrigTowers);
    DataVect numADCVec; numADCVec.reserve(numTrigTowers);
    DataVect emBCIDVec; emBCIDVec.reserve(numTrigTowers);
    DataVect hadBCIDVec; hadBCIDVec.reserve(numTrigTowers);
    DataVect isEMSaturated; isEMSaturated.reserve(numTrigTowers);
    DataVect isHadSaturated; isHadSaturated.reserve(numTrigTowers);
    DataVect emADCVec; emADCVec.reserve(numTrigTowers);
    DataVect hadADCVec; hadADCVec.reserve(numTrigTowers);

    // Loop over all trigger towers and fill vectors
    DataVector<xAOD::TriggerTower>::const_iterator trigTowerIter = TTVector->begin();
    for ( trigTowerIter  = TTVector->begin(); trigTowerIter < TTVector->end(); ++trigTowerIter ){
      
      // skip TTs with no energy
      if ( (*trigTowerIter)->cpET() == 0 ) continue;

      int sumEnergy = 0;
      int emEnergy = 0;
      int hadEnergy = 0;

      // from Sasha Mazurov (L1Calo):
      // references: energies: Trigger/TrigT1/TrigT1CaloMonitoring/PPrMon.cxx#L728
      //      adc: Trigger/TrigT1/TrigT1CaloMonitoring/trunk/src/PPrMon.cxx#L796
	// "We have separated elecromagnetic and hadronic towers, so instead of
	// one trigger tower we have two trigger towers where :
	// triggerTower->layer() == 0 => Electromagnetic layer (= emEnergy for old objects)
	// triggerTower->layer() == 1 => Hadronic layer (= hadEnergy for old objects)
	// triggerTower->cpET() and triggerTower->jepET() energies from LUT
	// triggerTower->adc()[triggerTower->adcPeak()] - peak energy in FADC"

      const int layer = (*trigTowerIter)->layer();
      const int cpET = (*trigTowerIter)->cpET();
      const int peakEnergy = (*trigTowerIter)->adc()[(*trigTowerIter)->adcPeak()];
      const int jepET = (*trigTowerIter)->jepET(); // currently not in output. JetElements ?

      if ( layer == 0 ){
	// emEnergy = emEnergy+cpET;
        emEnergy = peakEnergy;
      }else{
	// hadEnergy = hadEnergy+cpET;
        hadEnergy = peakEnergy;
      }

      if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) <<" L1 TT No " << count++ 
	<< " at layer: " << layer << ", with" 
        << " eta:"<<(*trigTowerIter)->eta() << ", phi:"<<(*trigTowerIter)->phi() 
        << ", em energy:"<< emEnergy <<", had energy:"<< hadEnergy 
        << ", cpET:" << cpET <<", jepET:"<< jepET << endreq;

      //Store eta and phi
      eta.push_back(DataType( (*trigTowerIter)->eta() ));
      phi.push_back(DataType( (*trigTowerIter)->phi() ));

      sumEnergy = emEnergy+hadEnergy; // probably too simple ? Saturation !

      emEnergyVec.push_back( emEnergy );
      hadEnergyVec.push_back( hadEnergy );
      sumEnergyVec.push_back( sumEnergy );

      //Store saturation flag
      isEMSaturated.push_back(DataType(  ((*trigTowerIter)->isCpSaturated())? 1 : 0 ));
      isHadSaturated.push_back(DataType( ((*trigTowerIter)->isJepSaturated())? 1 : 0 ));

      emADCVec.push_back( 0. ); // placeholders
      hadADCVec.push_back( 0. );
      numADCVec.push_back( 0. );
      emBCIDVec.push_back( 0. );
      hadBCIDVec.push_back( 0. );

    } // end of TT loop
   
    DataMap myDataMap;
    myDataMap["phi"] = phi;
    myDataMap["eta"] = eta;
    myDataMap["emEnergy"] = emEnergyVec;
    myDataMap["hadEnergy"] = hadEnergyVec;
    myDataMap["sumEnergy"] = sumEnergyVec;

    myDataMap["emADC"] = emADCVec;
    myDataMap["hadADC"] = hadADCVec;
    myDataMap["numADC"] = numADCVec;
    myDataMap["emBCID"] = emBCIDVec;
    myDataMap["hadBCID"] = hadBCIDVec;
    myDataMap["isEMSaturated"] = isEMSaturated;
    myDataMap["isHadSaturated"] = isHadSaturated;

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << ": "<< phi.size()
					    << " from: " << m_sgKey << endreq;

    //forward data to formating tool
    return FormatTool->AddToEvent(dataTypeName(), m_sgKey, &myDataMap);
  }
}
