/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaMonitoring/DQBadLBFilterTool.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CoralBase/AttributeListException.h"

DQBadLBFilterTool::DQBadLBFilterTool(const std::string& type,const std::string& name,const IInterface* parent)
: AthAlgTool( type, name, parent )
, m_fallbackValue(true)
, m_ignoreRecoverable(true)
, m_alwaysReturnTrue(false)
, m_valueCache(true)
//, m_listofdefects(0)
//, m_listofdefects_str(0)
{

  declareInterface<IDQFilterTool>(this);
  declareProperty("fallbackValue", m_fallbackValue);
  declareProperty("ignoreRecoverable", m_ignoreRecoverable);
  declareProperty("alwaysReturnTrue", m_alwaysReturnTrue);
  declareProperty("defectListStr", m_listofdefects_str);
  declareProperty("defectList", m_listofdefects);

}
        
DQBadLBFilterTool::~DQBadLBFilterTool () {}
        
StatusCode DQBadLBFilterTool::initialize()
{

  ATH_MSG_DEBUG("size of defects list in Bad DB " << m_listofdefects_str.size()<< " : " << m_listofdefects.size()); 
  ATH_MSG_DEBUG("test other params fall and def " << m_fallbackValue << " : " << m_alwaysReturnTrue );

  ATH_MSG_VERBOSE("ATLAS Ready initialize");
  // don't register callback if we always return true anyway
  if (m_alwaysReturnTrue) return StatusCode::SUCCESS;

  const DataHandle<CondAttrListCollection> attrList;
//  const DataHandle<AthenaAttributeList> attrList;
  StatusCode sc = detStore()->regFcn(&DQBadLBFilterTool::callback, 
  				     this, attrList,
  				     "/GLOBAL/DETSTATUS/DEFECTS");
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Unable to register Bad LB filter tool callback");
    return StatusCode::FAILURE;
  }
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Unable to set up ready value cache");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode DQBadLBFilterTool::updateCache() {

  const CondAttrListCollection* attrListCol(0);
  StatusCode sc = detStore()->retrieve(attrListCol, "/GLOBAL/DETSTATUS/DEFECTS");
  if (sc.isFailure()) {
    ATH_MSG_WARNING("Unable to retrieve defect information; falling back to" << m_fallbackValue);
    m_valueCache = m_fallbackValue;
    return StatusCode::SUCCESS;
  }

  m_valueCache = m_fallbackValue;
  if (attrListCol != 0) {

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
                m_valueCache =  0;
                return StatusCode::SUCCESS;
              }
          }
      }    
    
  }
  return StatusCode::SUCCESS;
}
        
bool DQBadLBFilterTool::accept() const {
  if (m_alwaysReturnTrue) {
    return true;
  } else {
    ATH_MSG_VERBOSE("Bad LB accept called, value " << m_valueCache);
    return m_valueCache;
  }
}

StatusCode DQBadLBFilterTool::callback( IOVSVC_CALLBACK_ARGS ) {
  ATH_MSG_VERBOSE("Bad LB callback called");
  return updateCache();
}
