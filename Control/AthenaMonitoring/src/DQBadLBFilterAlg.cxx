/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DQBadLBFilterAlg.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/AthenaAttributeListSpecification.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CoralBase/AttributeListException.h"

DQBadLBFilterAlg::DQBadLBFilterAlg(const std::string& name, ISvcLocator* pSvcLocator)
: AthAlgorithm( name, pSvcLocator )
, m_ignoreRecoverable(true)
, m_readKey("/GLOBAL/DETSTATUS/DEFECTS")
, m_writeKey("")
, m_condSvc("CondSvc",name)
{
  declareProperty("ignoreRecoverable", m_ignoreRecoverable);
  declareProperty("defectListStr", m_listofdefects_str);
  declareProperty("defectList", m_listofdefects);
  declareProperty("ReadKey", m_readKey);
  declareProperty("WriteKey", m_writeKey);
}
        
DQBadLBFilterAlg::~DQBadLBFilterAlg () {}
        
StatusCode DQBadLBFilterAlg::initialize()
{

  ATH_MSG_DEBUG("size of defects list in Bad DB " << m_listofdefects_str.size()<< " : " << m_listofdefects.size()); 

  ATH_MSG_VERBOSE("ATLAS Ready initialize");
  // define attributes to upload
  m_attribListSpec = new coral::AttributeListSpecification;
  m_attribListSpec->extend("Accept", "bool");

  // CondSvc
  ATH_CHECK( m_condSvc.retrieve() );
  // Handles
  ATH_CHECK( m_readKey.initialize() );
  ATH_CHECK( m_writeKey.initialize() );
  // Register write handle
  if (m_condSvc->regHandle(this, m_writeKey).isFailure()) {
    ATH_MSG_ERROR("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode DQBadLBFilterAlg::execute() {
  // from pixel example code ...
  // Write Cond Handle
  SG::WriteCondHandle<AthenaAttributeList> writeHandle{m_writeKey};
  // Do we have a valid Write Cond Handle for current time?
  if(writeHandle.isValid()) {
    // in theory this should never be called in MT
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid.");
    return StatusCode::SUCCESS; 
  }

  ATH_MSG_DEBUG( "Creating AthenaAttributeList object."  );
  std::unique_ptr<AthenaAttributeList> attribListW = std::make_unique<AthenaAttributeList>(*m_attribListSpec);
  (*attribListW)["Accept"].setValue(true);

  SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey};
  const CondAttrListCollection* attrListCol{*readHandle};
  if (attrListCol == nullptr) {
    ATH_MSG_ERROR("Unable to retrieve defect information");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("Col range " <<  " iov min " << attrListCol->minRange().start().event() << " iov max " << attrListCol->minRange().stop().event());

  std::vector<int>::const_iterator defect_iter = m_listofdefects.begin();
  
  for ( ; defect_iter != m_listofdefects.end();++defect_iter){
    if (attrListCol->chanAttrListPair(*defect_iter) != attrListCol->end()){
      const CondAttrListCollection::AttributeList* attrList = 
	&(attrListCol->attributeList( (*defect_iter) ));
      ATH_MSG_DEBUG("FOUND CHANNEL " << (*defect_iter) << " is: " << (*attrList)["present"].data<bool>());
      ATH_MSG_DEBUG("current channel : " << (*defect_iter) << " Name : " << attrListCol->chanName(*defect_iter) << " : iovrange " << attrListCol->iovRange(*defect_iter).start().event() << " : " <<  attrListCol->iovRange(*defect_iter).stop().event());
      ATH_MSG_DEBUG( "Check address " << attrList << " : " << *attrList);
      if ((*attrList)["present"].data<bool>() != 0){
	if ((*attrList)["recoverable"].data<bool>() == 1 && m_ignoreRecoverable){
	  continue;
	}
	(*attribListW)["Accept"].setValue(false);
      }
    }
  }    

  // Define validity of the output cond object and record it
  EventIDRange rangeW;
  if(!readHandle.range(rangeW)) {
    ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandle.key());
    return StatusCode::FAILURE;
  }
 
  if(writeHandle.record(rangeW,attribListW.release()).isFailure()) {
    ATH_MSG_ERROR("Could not record DQ AthenaAttributeList object with " 
		  << writeHandle.key() 
		  << " with EventRange " << rangeW
		  << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

  return StatusCode::SUCCESS;
}
