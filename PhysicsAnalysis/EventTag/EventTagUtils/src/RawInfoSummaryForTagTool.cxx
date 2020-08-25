/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
Name    : RawInfoSummaryForTagTool.cxx
Package : offline/PhysicsAnalysis/EventTag/EventTagUtils
Author  : Ketevi A. Assamagan
Created : January 2006
Purpose : create a DataQualityTag - The Detector Status Words and the Good
          Run tags are encode here

*****************************************************************************/

#include "GaudiKernel/MsgStream.h"
#include "Gaudi/Property.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "StoreGate/StoreGateSvc.h"

#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"
#include "TagEvent/RawInfoSummaryForTag.h"
#include "TagEvent/RawInfoSummaryForTagNames.h"
#include "EventTagUtils/RawInfoSummaryForTagTool.h"

#include <sstream>

/** the constructor */
RawInfoSummaryForTagTool::RawInfoSummaryForTagTool (const std::string& type, const 
    std::string& name, const IInterface* parent) : 
    AthAlgTool( type, name, parent ) {

  /** AOD Object Name */
  declareProperty("RawInfoSummaryKey",    m_dataObjectName = "");
  
  declareInterface<RawInfoSummaryForTagTool>( this );
}

/** initialization - called once at the begginning */
StatusCode  RawInfoSummaryForTagTool::initialize() {

  ATH_MSG_DEBUG("in initialize()");

  StatusCode sc = service("StoreGateSvc", m_storeGate);
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve pointer to StoreGateSvc");
    return sc;
  }

  return StatusCode::SUCCESS;
}

/** build the attribute list - called in initialize */
/*
StatusCode RawInfoSummaryForTagTool::attributeSpecification(
           std::map<std::string,AthenaAttributeType>& attrMap) {

  MsgStream mLog(msgSvc(), name());
  mLog << MSG::DEBUG << "in attributeSpecification()" << endmsg;

  // specifiy the Data Quality - Detector Status attributes 
  // Attention: The group and unit names are the same for all the Detector. 
  for (unsigned int i = 0; i <= Raw::NAttr; ++i) {
     attrMap.push_back(std::make_pair(RawSummaryAttributeSpecs[i].name(),
                                      RawSummaryAttributeSpecs[i].attributeType()
                      )              );
  }

  return StatusCode::SUCCESS;
}
*/
StatusCode RawInfoSummaryForTagTool::attributeSpecification(
           std::vector<AthenaAttributeSpecification>& attrMap) {

  ATH_MSG_DEBUG("in attributeSpecification()");

  /** specifiy the Data Quality - Detector Status attributes */
  /** Attention: The group and unit names are the same for all the Detector. */
  for (unsigned int i = 0; i < Raw::NAttr; ++i) {
     attrMap.push_back(RawSummaryAttributeSpecs[i]);
  }

  return StatusCode::SUCCESS;
}

/** execute - called on every event */
StatusCode RawInfoSummaryForTagTool::execute(TagFragmentCollection& rawfragment) {

  ATH_MSG_DEBUG("in execute()");

  RawInfoSummaryForTag rawinfoObj;
  const RawInfoSummaryForTag* rawinfo=0;
  StatusCode sc = m_storeGate->retrieve( rawinfo );
  if (sc.isFailure()) {
    ATH_MSG_WARNING("Cannot get raw summary info from storegate");
    rawinfo=&rawinfoObj;
  }
  //
  // FLOATS
  // Cell Energy
  rawfragment.insert(RawSummaryAttributeSpecs[0].name(),rawinfo->getCellEnergySum());
  rawfragment.insert(RawSummaryAttributeSpecs[1].name(),rawinfo->getCellEnergySumEMB());
  rawfragment.insert(RawSummaryAttributeSpecs[2].name(),rawinfo->getCellEnergySumEMEC());
  rawfragment.insert(RawSummaryAttributeSpecs[3].name(),rawinfo->getCellEnergySumHEC());
  rawfragment.insert(RawSummaryAttributeSpecs[4].name(),rawinfo->getCellEnergySumFCAL());
  rawfragment.insert(RawSummaryAttributeSpecs[5].name(),rawinfo->getCellEnergySumTile());
  rawfragment.insert(RawSummaryAttributeSpecs[6].name(),rawinfo->getClusterEnergySum());
  // TopoClusters
  rawfragment.insert(RawSummaryAttributeSpecs[7].name(),rawinfo->getTopoClEt1());
  rawfragment.insert(RawSummaryAttributeSpecs[8].name(),rawinfo->getTopoClEta1());
  rawfragment.insert(RawSummaryAttributeSpecs[9].name(),rawinfo->getTopoClPhi1());
  // Cell Met
  rawfragment.insert(RawSummaryAttributeSpecs[10].name(),rawinfo->getCellMET());
  rawfragment.insert(RawSummaryAttributeSpecs[11].name(),rawinfo->getCellMETPhi());
  rawfragment.insert(RawSummaryAttributeSpecs[12].name(),rawinfo->getMBTStimeDiff());
  rawfragment.insert(RawSummaryAttributeSpecs[13].name(),rawinfo->getLArECtimeDiff());
  rawfragment.insert(RawSummaryAttributeSpecs[14].name(),rawinfo->getMBTStimeAvg());
  rawfragment.insert(RawSummaryAttributeSpecs[15].name(),rawinfo->getLArECtimeAvg());

  // Track counts and muon
  rawfragment.insert(RawSummaryAttributeSpecs[16].name(),rawinfo->getNsctSPs());
  rawfragment.insert(RawSummaryAttributeSpecs[17].name(),rawinfo->getNpixSPs());
  rawfragment.insert(RawSummaryAttributeSpecs[18].name(),rawinfo->getNtrtDCs());
  rawfragment.insert(RawSummaryAttributeSpecs[19].name(),rawinfo->getNtrtHtDCs());
  rawfragment.insert(RawSummaryAttributeSpecs[20].name(),rawinfo->getBCMHit());

  //putting back MBTS and LAr
  rawfragment.insert(RawSummaryAttributeSpecs[21].name(),rawinfo->getMBTSword());


  return StatusCode::SUCCESS;
}

/** finialize - called once at the end */
StatusCode  RawInfoSummaryForTagTool::finalize() {
  ATH_MSG_DEBUG("in finalize()");

  return StatusCode::SUCCESS;
}
