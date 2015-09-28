/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file QueryTag.cxx
 *  @brief This file contains the implementation for the QueryTag class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: QueryTag.cxx,v 1.5 2009-03-17 09:44:46 gemmeren Exp $
 **/

#include "QueryTag.h"

#include "AthenaPoolUtilities/AthenaAttributeList.h"

using namespace AthPoolEx;

//___________________________________________________________________________
QueryTag::QueryTag(const std::string& type, const std::string& name, const IInterface* parent) : 
	AthAlgTool(type, name, parent), m_attrListKey() {
   declareInterface<IAthenaSelectorTool>(this);
}
//___________________________________________________________________________
QueryTag::~QueryTag() {
}
//___________________________________________________________________________
StatusCode QueryTag::initialize() {
   ATH_MSG_INFO("in initialize()");
   const IService* parentSvc = dynamic_cast<const IService*>(this->parent());
   if (parentSvc != 0) {
      const IProperty* propertyServer = dynamic_cast<const IProperty*>(parentSvc);
      if (propertyServer != 0) {
         StringProperty attrKeyProperty("AttributeListKey", "");
         StatusCode status = propertyServer->getProperty(&attrKeyProperty);
         if (status.isSuccess()) {
            m_attrListKey = attrKeyProperty.value();
         }
      }
   }
   return StatusCode::SUCCESS;
}
//___________________________________________________________________________
StatusCode QueryTag::postInitialize() {
   return StatusCode::SUCCESS;
}
//__________________________________________________________________________
StatusCode QueryTag::preNext() {
   return StatusCode::SUCCESS;
}
//__________________________________________________________________________
StatusCode QueryTag::postNext() {
   if (!evtStore()->contains<AthenaAttributeList>(m_attrListKey)) {
      ATH_MSG_DEBUG("Can't get attributeList for preselection");
      return StatusCode::SUCCESS;
   }

   const DataHandle<AthenaAttributeList> attrList;
   StatusCode status = evtStore()->retrieve(attrList, m_attrListKey);
   if (status.isFailure()) {
      ATH_MSG_ERROR("Could not retrieve AthenaAttributeList");
      return StatusCode::FAILURE;
   }

   try {
      unsigned int eventNumber = (*attrList)["EventNumber"].data<unsigned int>();
      unsigned int runNumber = (*attrList)["RunNumber"].data<unsigned int>();
      unsigned int magicNumber = (*attrList)["MagicNumber"].data<unsigned int>();

      ATH_MSG_DEBUG("EventNumber = " << eventNumber);
      ATH_MSG_DEBUG("RunNumber = " << runNumber);
      ATH_MSG_DEBUG("MagicNumber = " << magicNumber);
      if (eventNumber < 10 && magicNumber > 17) {
         return StatusCode::RECOVERABLE;
      }
   } catch (...) {
      ATH_MSG_DEBUG("Can't apply attributeList preselection");
      return StatusCode::SUCCESS;
   }
   return StatusCode::SUCCESS;
}
//__________________________________________________________________________
StatusCode QueryTag::preFinalize() {
   return StatusCode::SUCCESS;
}
//__________________________________________________________________________
StatusCode QueryTag::finalize() {
   ATH_MSG_INFO("in finalize()");
   return StatusCode::SUCCESS;
}
//__________________________________________________________________________
