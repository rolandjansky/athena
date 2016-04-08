/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigJiveXML/TriggerInfoRetriever.h"

#include <string>

#include "CLHEP/Units/SystemOfUnits.h"

#include "AnalysisTriggerEvent/LVL1_ROI.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/TriggerInfo.h"

namespace JiveXML {

  //--------------------------------------------------------------------------

  TriggerInfoRetriever::TriggerInfoRetriever(const std::string& type, const std::string& name, const IInterface* parent):
    AthAlgTool(type, name, parent),
    m_typeName("TriggerInfo")
  {

    declareInterface<IDataRetriever>(this);
  }

  //--------------------------------------------------------------------------

  StatusCode TriggerInfoRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {

// retrieve TriggerInfo
    const EventInfo* eventInfo;
    const TriggerInfo* trigger_info;
    
    if ( evtStore()->retrieve(eventInfo).isFailure() ) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No EventInfo found in SG" << endreq;
      return StatusCode::SUCCESS;
    } 
    trigger_info = eventInfo->trigger_info();
    if (trigger_info == 0){
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No TriggerInfo in EventInfo" << endreq;
      return StatusCode::SUCCESS;
    }
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Retrieved EventInfo and TriggerInfo" << endreq;

    std::string triggerInfoStrL1="0";
    std::string triggerInfoStrL2="0";
    std::string triggerInfoStrEF="0";
    std::string triggerInfoStreamTag="0";

// Level-1 Missing-ET from LVL1_ROI: 
    DataVect energySumEtVec; 
    DataVect energyExVec;
    DataVect energyEyVec;
    DataVect energyEtMissVec;

// TriggerInfo from EventInfo: 
    DataVect trigInfoStatusVec;
    DataVect trigInfoExtL1IDVec;
    DataVect trigInfoLvl1TypeVec;
    DataVect trigInfoL1Vec;
    DataVect trigInfoL2Vec;
    DataVect trigInfoEFVec;
    DataVect trigInfoStreamTagVec;

// from: AtlasTest/DatabaseTest/AthenaPoolTest/AthenaPoolTestDataReader

    trigInfoStatusVec.push_back(DataType( trigger_info->statusElement() ) );
    trigInfoExtL1IDVec.push_back(DataType( trigger_info->extendedLevel1ID() ) );
    trigInfoLvl1TypeVec.push_back(DataType( trigger_info->level1TriggerType() ) );

/*
       if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trigger info: status " << trigger_info->statusElement()
            << " extLvl1ID " << trigger_info->extendedLevel1ID()
            << " lvl1Type  " << trigger_info->level1TriggerType()
            << " lvl1Info  ";
*/
        if ( trigger_info->level1TriggerInfo().size() > 0 ){
	    triggerInfoStrL1 = "-"; // item seperator
        }
        for (unsigned int i = 0; i < trigger_info->level1TriggerInfo().size(); ++i) {
//            if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << trigger_info->level1TriggerInfo()[i] << " ";
            triggerInfoStrL1 += DataType( trigger_info->level1TriggerInfo()[i]).toString() + "-";
        }

        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " lvl2Info " << endreq;    
        if ( trigger_info->level2TriggerInfo().size() > 0 ){
	    triggerInfoStrL2 = "-"; // item seperator
        }
        for (unsigned int i = 0; i < trigger_info->level2TriggerInfo().size(); ++i) {
//            if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << trigger_info->level2TriggerInfo()[i] << " ";
            triggerInfoStrL2 += DataType( trigger_info->level2TriggerInfo()[i] ).toString() + "-";
        }

        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "EventFilterInfo " << endreq;    
        if ( trigger_info->eventFilterInfo().size() > 0 ){
	    triggerInfoStrEF = "-"; // item seperator
        }
        for (unsigned int i = 0; i < trigger_info->eventFilterInfo().size(); ++i) {
//            log << MSG::DEBUG << trigger_info->eventFilterInfo()[i] << " " ;
            triggerInfoStrEF += DataType( trigger_info->eventFilterInfo()[i]).toString() + "-";
        }

        if ( trigger_info->streamTags().size() > 0 ){
	    triggerInfoStreamTag = "-"; // item seperator
        }
        for (unsigned int i = 0; i < trigger_info->streamTags().size(); ++i) {
           if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << " i " << i << " name " << trigger_info->streamTags()[i].name()
          << " type " << trigger_info->streamTags()[i].type()
          << " ObeysLumi " << trigger_info->streamTags()[i].obeysLumiblock();

            /////// fixed in 15.5.0, but leave in if this resurfaces  jpt 12Aug09 
            //// problem with stream names: non-char values in string:
            //// e.g. 'L1<nul><bel>' (visible in nedit)  
	    ////  https://savannah.cern.ch/bugs/?52139,  jpt 7Jul09
            //// PATCH: cut all suspiciously short streams names starting 
            //// with L1 to just L1
            //std::string myStreamTag = trigger_info->streamTags()[i].name();
	    //log << MSG::DEBUG << " myStreamTag was: " << myStreamTag << "+++" 
            //    << " length: " << myStreamTag.size() <<  endreq;
            //if (myStreamTag.substr(0,2) == "L1" && myStreamTag.size() < 7 ){ myStreamTag = "L1";}
	    //log << MSG::DEBUG << " myStreamTag is: " << myStreamTag << "+++" << endreq;
            //triggerInfoStreamTag += myStreamTag  + "_" +

            //// normal code without mask is this:
	    triggerInfoStreamTag += trigger_info->streamTags()[i].name() + "_" +
            trigger_info->streamTags()[i].type() + "_";
          if ( trigger_info->streamTags()[i].obeysLumiblock() ){ // is a boolean
             triggerInfoStreamTag += "ObeysLumi";
          }else{
             triggerInfoStreamTag += "notObeysLumi";
	  }
          if ( eventInfo->event_ID()->lumi_block() ){
	     triggerInfoStreamTag += "_lumiBlock";
	     triggerInfoStreamTag += DataType(eventInfo->event_ID()->lumi_block()).toString();
          }else{
	     triggerInfoStreamTag += "_lumiBlockUnknown";
          }
          triggerInfoStreamTag += "-";
	}

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "trigInfoL1: " << triggerInfoStrL1 << endreq;
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "trigInfoL2: " << triggerInfoStrL2 << endreq;
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "trigInfoEF: " << triggerInfoStrEF << endreq;
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "triggerInfoStreamTag: " << triggerInfoStreamTag << endreq;

    trigInfoL1Vec.push_back(DataType( triggerInfoStrL1 ));
    trigInfoL2Vec.push_back(DataType( triggerInfoStrL2 ));
    trigInfoEFVec.push_back(DataType( triggerInfoStrEF ));
    trigInfoStreamTagVec.push_back(DataType( triggerInfoStreamTag ));

    // Retrieve LVL1_ROI for trigger energies
    // assume that eventInfo->triggerInfo _has_ to be present,
    // but LVL1_ROI not present under all circumstances (ACR: muoncomm)
    // Don't exit upon failed SG retrieval as usual.

    const LVL1_ROI * roi;
    if ( evtStore()->retrieve(roi,"LVL1_ROI").isFailure() ) {
       if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No LVL1_ROI for Trigger-MissingET found in SG, setting placeholders." << endreq;
//       return StatusCode::SUCCESS; // not here.
       // placeholders:
        energySumEtVec.push_back(DataType( -1. ) ); // means n/a
        energyExVec.push_back(DataType( -1. ) );
        energyEyVec.push_back(DataType( -1. ) );
        energyEtMissVec.push_back(DataType( -1. ) );
    }else{
      if ( (roi->getEnergySumROIs()).size() != 0 ){ // catch empty container
        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "TriggerInfo: LVL1_ROI for EnergySum retrieved" << endreq;

        LVL1_ROI::energysums_type::const_iterator itES  = (roi->getEnergySumROIs()).begin();
        LVL1_ROI::energysums_type::const_iterator itESe = (roi->getEnergySumROIs()).end();
        for (; itES != itESe; ++itES){
	  float m_Et = itES->getEnergyT()/CLHEP::GeV;
	  float m_Ex = itES->getEnergyX()/CLHEP::GeV;
	  float m_Ey = itES->getEnergyY()/CLHEP::GeV;
	  energySumEtVec.push_back(DataType( m_Et ) );
	  energyExVec.push_back(DataType( m_Ex ) );
	  energyEyVec.push_back(DataType( m_Ey ) );
	  float m_EtMiss = static_cast<long>(sqrt(static_cast<double>(m_Ex*m_Ex + m_Ey*m_Ey)));
	  energyEtMissVec.push_back(DataType( m_EtMiss ) );

	  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "EnergySum from LVL1_ROI: X:" << m_Ex << ", Y:"
	      << m_Ey << ", T:" << m_Et << ", ETMiss:" << m_EtMiss << endreq;
        }
      }else{ // empty container
        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "TriggerInfo: LVL1_ROI for EnergySum empty !" << endreq;

	  energySumEtVec.push_back(DataType( "-1" ) ); // means n/a
	  energyExVec.push_back(DataType( "-1" ) );
	  energyEyVec.push_back(DataType( "-1" ) );
	  energyEtMissVec.push_back(DataType( "-1" ) );
      }	  
    } // LVL1_ROI available ?

    DataMap myDataMap;
    myDataMap["energySumEt"] = energySumEtVec;
    myDataMap["energyEx"] = energyExVec;
    myDataMap["energyEy"] = energyEyVec;
    myDataMap["energyEtMiss"] = energyEtMissVec;
    myDataMap["trigInfoStatus"] = trigInfoStatusVec;
    myDataMap["trigInfoExtL1ID"] = trigInfoExtL1IDVec;
    myDataMap["trigInfoLvl1Type"] = trigInfoLvl1TypeVec;
    myDataMap["trigInfoL1"] = trigInfoL1Vec;
    myDataMap["trigInfoL2"] = trigInfoL2Vec;
    myDataMap["trigInfoEF"] = trigInfoEFVec;
    myDataMap["trigInfoStreamTag"] = trigInfoStreamTagVec;

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << ": "<< trigInfoStatusVec.size() << endreq;

    //forward data to formating tool
    std::string emptyStr=""; // eventInfo has no SGKey
    return FormatTool->AddToEvent(dataTypeName(), emptyStr, &myDataMap);
  }
}
