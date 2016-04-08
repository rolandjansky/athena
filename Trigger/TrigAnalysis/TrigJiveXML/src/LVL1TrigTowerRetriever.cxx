/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigJiveXML/LVL1TrigTowerRetriever.h"

#include <string>

#include "CLHEP/Units/SystemOfUnits.h"

#include "TrigT1CaloEvent/TriggerTower.h"

namespace JiveXML {

  /** 
   * Gaudi default constructor
   */
  LVL1TrigTowerRetriever::LVL1TrigTowerRetriever(const std::string& type, const std::string& name, const IInterface* parent):
    AthAlgTool(type, name, parent),
    m_typeName("LVL1TriggerTower")
  {

    declareInterface<IDataRetriever>(this);

    declareProperty("StoreGateKey", m_sgKey = "TriggerTowers",  "Storegate key for Level-1 TriggerTowers");
    declareProperty("SumEnergyThreshold", m_sumEnergyThr = 0, "TriggerTower only written out if sumEnergy above this");
  }

  /**
   * Retrieve trigger tower information
   * - loop over towers
   * - sum ADC energies if requested
   * - fill arrays and write out
   */
  StatusCode LVL1TrigTowerRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {

    //Retrieve trigger towers
    const DataVector<LVL1::TriggerTower>* TTVector;
        
    if ( evtStore()->retrieve(TTVector,m_sgKey).isFailure() ) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<  "No Level-1 Trigger Towers found in SG at "
          << m_sgKey << endreq;
      return StatusCode::SUCCESS;
    } 
    
    //Get number of trigger towers
    int numTrigTowers = TTVector->size();
    int count = 0;
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Retrieving Level-1 Trigger Towers with size " << numTrigTowers << endreq;

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
    DataVector<LVL1::TriggerTower>::const_iterator trigTowerIter = TTVector->begin();
    for ( trigTowerIter  = TTVector->begin(); trigTowerIter < TTVector->end(); ++trigTowerIter ){
      
      // skip TTs with no energy
      if ( (*trigTowerIter)->hadEnergy() == 0 && (*trigTowerIter)->emEnergy() == 0 ) continue;

      if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) <<" Lvl1 TriggerTower No " << count++ << " with" 
        << " eta:"<<(*trigTowerIter)->eta() << ", phi:"<<(*trigTowerIter)->phi()
        << ", EM energy:"<<(*trigTowerIter)->emEnergy()<<", Had Energy:"<<(*trigTowerIter)->hadEnergy() <<endreq;

      //Be verbose
      //if (msgLvl(MSG::DEBUG)){
      //   msg(MSG::DEBUG) << " Em ADC size:" << (*trigTowerIter)->emADC().size() << endreq; 
      //  msg(MSG::DEBUG) << " Had ADC size:" << (*trigTowerIter)->hadADC().size() << endreq; 
      //}

      int sumEnergy = 0;
      int emEnergy = 0;
      int hadEnergy = 0;

      emEnergy  = (*trigTowerIter)->emEnergy();
      hadEnergy = (*trigTowerIter)->hadEnergy();
      // propagate saturated towers
      if ((emEnergy != 255) && (hadEnergy != 255)){ // are in GeV
         sumEnergy = (*trigTowerIter)->emEnergy()+(*trigTowerIter)->hadEnergy();
      } else {
         sumEnergy = 255;
      }     

      // finally write out only TrigTowers with sumEnergy > sumEnergyThr (default: 0)
      if ( sumEnergy > m_sumEnergyThr ){

        //Store energies
        emEnergyVec.push_back( emEnergy );
        hadEnergyVec.push_back( hadEnergy );
        sumEnergyVec.push_back( sumEnergy );
        
        //Store eta and phi
        eta.push_back(DataType((*trigTowerIter)->eta()));
        phi.push_back(DataType((*trigTowerIter)->phi()));

        //Store the bunch crossing ID
        emBCIDVec.push_back(DataType((*trigTowerIter)->emBCID()));
        hadBCIDVec.push_back(DataType((*trigTowerIter)->hadBCID()));

        //Store saturation flag
        isEMSaturated.push_back(DataType(  ((*trigTowerIter)->isEMSaturated())? 1 : 0 ));
        isHadSaturated.push_back(DataType( ((*trigTowerIter)->isHadSaturated())? 1 : 0 ));

        //Make sure number of hadronic and em ADC values match. 
        //Dummy data inserted if they don't.  
        if ((*trigTowerIter)->emADC().size() !=  (*trigTowerIter)->hadADC().size()){
          if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Number of EmADC and HadADC values do not match" 
						      << ",em:"<< (*trigTowerIter)->emADC().size()  
                                                      << ",had:" << (*trigTowerIter)->hadADC().size() << endreq;
	  numADCVec.push_back(DataType( 1 ));
          emADCVec.push_back(DataType( 0 ));
          hadADCVec.push_back(DataType( 0 )); 
          continue ; // next tower or exit
        }

        // We have checked that em and had have same size
        numADCVec.push_back(DataType( (*trigTowerIter)->emADC().size() )); 

        // loop again over ADCs to write values out:                  
        std::vector<int>::const_iterator emADCItr = ((*trigTowerIter)->emADC()).begin();
        for (; emADCItr != ((*trigTowerIter)->emADC()).end(); ++emADCItr)
          emADCVec.push_back(DataType( *emADCItr ));

        std::vector<int>::const_iterator hadADCItr = ((*trigTowerIter)->hadADC()).begin();
        for (; hadADCItr != ((*trigTowerIter)->hadADC()).end(); ++hadADCItr)
          hadADCVec.push_back(DataType( *hadADCItr ));
      } // end sumEnergy > sumEnergyThr
    }//end trigger tower iterator loop

    //Calculate multiple string (we have checked that EM and Had match)
    std::string multiple = " multiple=\"0.\""; 
    if (emEnergyVec.size() != 0){ // avoid division by zero !
       multiple = " multiple=\""+DataType(((float)emADCVec.size())/((float)emEnergyVec.size())).toString()+"\"";
    }

    DataMap myDataMap;
    myDataMap["phi"] = phi;
    myDataMap["eta"] = eta;
    myDataMap["emEnergy"] = emEnergyVec;
    myDataMap["hadEnergy"] = hadEnergyVec;
    myDataMap["sumEnergy"] = sumEnergyVec;
    myDataMap["emADC"+multiple] = emADCVec;
    myDataMap["hadADC"+multiple] = hadADCVec;
    myDataMap["numADC"] = numADCVec;
    myDataMap["emBCID"] = emBCIDVec;
    myDataMap["hadBCID"] = hadBCIDVec;
    myDataMap["isEMSaturated"] = isEMSaturated;
    myDataMap["isHadSaturated"] = isHadSaturated;

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << ": "<< phi.size() << endreq;

    //forward data to formating tool
    return FormatTool->AddToEvent(dataTypeName(), m_sgKey, &myDataMap);
  }
}
