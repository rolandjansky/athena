/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigJiveXML/LVL1ResultRetriever.h"

//#include <string>
#include "TrigSteeringEvent/Chain.h"
//#include "TrigSteeringEvent/TrigRoiDescriptor.h"

//#include "TrigDecisionTool/ChainGroup.h"
//#include "TrigDecisionTool/FeatureContainer.h"
//#include "TrigDecisionTool/Feature.h"

/// Migrated to new TrigDecisionTool described here
///   https://twiki.cern.ch/twiki/bin/view/Atlas/TrigDecisionTool15
namespace JiveXML {


  /**
   * Gaudi default constructor
   */
  LVL1ResultRetriever::LVL1ResultRetriever(const std::string& type, const std::string& name, const IInterface* parent):
    AthAlgTool(type, name, parent),
    m_typeName("LVL1Result"),m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool")
  {

    declareInterface<IDataRetriever>(this);
    declareProperty("TrigDecisionTool", m_trigDec, "Tool to access TrigDecision");
  }

  /**
   * Initialize the tool
   * - get handle to TrigDecisionTool
   * - define chain groups
   **/
  StatusCode LVL1ResultRetriever::initialize() {

    //be verbose
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "initialize()" << endreq;

    //Try to retrieve the trig decision tool
    if ( !m_trigDec.retrieve() ) {
      if (msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "Could not retrieve TrigDecisionTool!" << endreq;
      return StatusCode::FAILURE;
    }

    // We define the chain groups relying on the name convention (i.e. all L1
    // items start their name from "L1_", etc) In principle we would not have to do
    // so as the TrigDecisionTool jobOptions defines these as public chain
    // groups. This way, we are independant of jobOptions
    m_all   = m_trigDec->getChainGroup(".*");
    m_allL1 = m_trigDec->getChainGroup("L1_.*");
    m_allL2 = m_trigDec->getChainGroup("L2_.*");
    m_allEF = m_trigDec->getChainGroup("EF_.*");
    m_allHLT = m_trigDec->getChainGroup("HLT_.*");

    return StatusCode::SUCCESS;
  }

  /**
   * Get a long strong with all the item lists and prescales that are passed by
   * the given chain group
   **/
  StatusCode LVL1ResultRetriever::getItemLists(const Trig::ChainGroup* m_chains, 
      std::string& m_itemList, std::string& m_prescaleList) {

    std::string sig_name;

    //Get a list of L1 items
    std::vector<std::string> chainList = m_chains->getListOfTriggers();
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Number of items in chain is " << chainList.size() << endreq;

    for (std::vector<std::string>::iterator itemItr = chainList.begin();
         itemItr != chainList.end(); ++itemItr) {
      
      // Make sure the item is not empty
      // (can this actually happen ?!?
      if ( (*itemItr).empty() ) continue;
    
      // too many triggers in data15, write only non-prescaled
      if ( m_trigDec->getPrescale(*itemItr) != 1. ){  continue; }

      std::string myItem = (*itemItr);
      //Output debug info
      if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "  * item : name=" << myItem 
	  << "; result = " << (m_trigDec->isPassed((*itemItr)) ? "passed" : "failed") 
	  << "; prescale = " <<  m_trigDec->getPrescale((*itemItr))  << endreq ;
      // replace HLT with EF (as AtlantisJava doesn't know 'HLT'):
      if ( myItem.find("HLT",0) != std::string::npos){ 
	myItem.replace(0,4,"EF_");
        if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << (*itemItr) << " renamed into: " << myItem
   	  << endreq ;
      }

      // prescale: see TWiki page TrigDecisionTool15

      //Only add passed items
      if ( m_trigDec->isPassed((*itemItr)) ) { 

        //Add item to list
        m_itemList += "-" + myItem; 

        // prescale factor
        m_prescaleList += "-" + DataType(  m_trigDec->getPrescale(*itemItr) ).toString(); 
      }
    }

    //Mark empty item lists
    if ( m_itemList.empty() ){ m_itemList = "empty"; }
    if ( m_prescaleList.empty() ){ m_prescaleList = "empty"; }

    //print debug information
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " itemList: " << m_itemList << endreq;
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " prescaleList: " << m_prescaleList << endreq;

    return StatusCode::SUCCESS;
  }

  /**
   * Retrieve all trigger info
   * - item lists for L1, L2 and EF, data15: HLT instead of L2,EF
   * - prescale lists for L1, L2 and EF, data15: HLT instead of L2,EF
   * - passed flags for L1, L2 and EF, data15: HLT instead of L2,EF
   **/
  StatusCode LVL1ResultRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {

    //be verbose
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "retrieve()" << endreq;

    //Get the item and prescale lists for all levels
    std::string itemListL1="";
    std::string prescaleListL1="";
    std::string itemListL2="";
    std::string prescaleListL2="";
    std::string itemListEF="";
    std::string prescaleListEF="";
    std::string itemListHLT="";
    std::string prescaleListHLT="";

    //Get L1
    getItemLists( m_allL1, itemListL1, prescaleListL1 ).ignore();
    //Summarize L1 result
    int flagL1Passed = m_allL1->isPassed();
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Decision : Level-1 " << ((flagL1Passed)? "passed":"failed") << endreq;

    //Get L2
    getItemLists( m_allL2, itemListL2, prescaleListL2 ).ignore();
    //Summarize L2 result
    int flagL2Passed = m_allL2->isPassed();
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Decision : Level-2 " << ((flagL2Passed)? "passed":"failed") << endreq;

    //Get EF
    getItemLists( m_allEF, itemListEF, prescaleListEF ).ignore();
    //Summarize EF result
    int flagEFPassed = m_allEF->isPassed();
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Decision : EventFilter " << ((flagEFPassed)? "passed":"failed") << endreq;

    //Get HLT
    getItemLists( m_allHLT, itemListHLT, prescaleListHLT ).ignore();
    //Summarize HLT result
    int flagHLTPassed = m_allHLT->isPassed();
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Decision : HLT " << ((flagHLTPassed)? "passed":"failed") << endreq;

    //Do not write trigger info if we failed to obtain any of it
    if ((itemListL1=="empty") && (itemListL2=="empty") && (itemListEF=="empty") && (itemListHLT=="empty") ){
      if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "All item lists empty, will not write out any data" << endreq;
      return StatusCode::SUCCESS;
    }
    
    //Store results in data list
    DataVect itemListL1Vec;        itemListL1Vec.push_back( DataType( itemListL1 )); 
    DataVect prescaleListL1Vec;    prescaleListL1Vec.push_back( DataType( prescaleListL1 )); 
    DataVect itemListL2Vec;        itemListL2Vec.push_back( DataType( itemListL2 ));
    DataVect prescaleListL2Vec;    prescaleListL2Vec.push_back( DataType( prescaleListL2 ));
    DataVect itemListEFVec;        itemListEFVec.push_back( DataType( itemListEF ));
    DataVect prescaleListEFVec;    prescaleListEFVec.push_back( DataType( prescaleListEF ));
    DataVect itemListHLTVec;       itemListHLTVec.push_back( DataType( itemListHLT )); 
    DataVect prescaleListHLTVec;   prescaleListHLTVec.push_back( DataType( prescaleListHLT ));
    DataVect passedTrigger;        passedTrigger.push_back(DataType( flagHLTPassed )); //this is just a duplicate
    DataVect passedL1;             passedL1.push_back(DataType( flagL1Passed )); 
    DataVect passedL2;             passedL2.push_back(DataType( flagL2Passed ));
    DataVect passedEF;             passedEF.push_back(DataType( flagHLTPassed )); // temporary.
    DataVect passedHLT;            passedHLT.push_back(DataType( flagHLTPassed ));
    // placeholders only ! For backwards compatibility.
    // Trigger energies moved to TriggerInfoRetriever
    DataVect energySumEt; energySumEt.push_back(DataType( -1. ) );
    DataVect energyEx; energyEx.push_back(DataType( -1. ) );
    DataVect energyEy; energyEy.push_back(DataType( -1. ) );
    DataVect energyEtMiss; energyEtMiss.push_back(DataType( -1. ) );

    //Finally create data map and write out results
    DataMap dataMap;
    dataMap["ctpItemList"] = itemListL1Vec;
    dataMap["prescaleListL1"] = prescaleListL1Vec;
    dataMap["itemListL2"] = itemListL2Vec;
    dataMap["prescaleListL2"] = prescaleListL2Vec;
    dataMap["itemListEF"] = itemListHLTVec; // temporary. AtlantisJava doesn't know 'HLT' yet. jpt 23Jun15
    dataMap["prescaleListEF"] = prescaleListHLTVec;
    dataMap["passedTrigger"] = passedTrigger;
    dataMap["passedL1"] = passedL1;
    dataMap["passedL2"] = passedHLT; // temporary. AtlantisJava doesn't know 'HLT' yet. jpt 23Jun15
    dataMap["passedEF"] = passedHLT; // temporary. AtlantisJava doesn't know 'HLT' yet. jpt 23Jun15
    dataMap["energySumEt"] = energySumEt;
    dataMap["energyEx"] = energyEx;
    dataMap["energyEy"] = energyEy;
    dataMap["energyEtMiss"] = energyEtMiss;
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << ": "<< itemListL1Vec.size() 
                                            << endreq; 
    //forward data to formating tool
    return FormatTool->AddToEvent(dataTypeName(), "TrigDecision", &dataMap);
  }
}
