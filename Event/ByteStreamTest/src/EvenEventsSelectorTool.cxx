/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file EvenEventsSelectorTool.cxx
 *  @brief This file contains the implementation for the EvenEventsSelectorTool class.
 *  $Id: EvenEventsSelectorTool.cxx,v 1.5 2009-03-17 09:44:46 cranshaw Exp $
 **/

#include "EvenEventsSelectorTool.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

//___________________________________________________________________________
EvenEventsSelectorTool::EvenEventsSelectorTool(const std::string& type, const std::string& name, const IInterface* parent) : 
	AthAlgTool(type, name, parent) {
   declareInterface<IAthenaSelectorTool>(this);
}
//___________________________________________________________________________
EvenEventsSelectorTool::~EvenEventsSelectorTool() {
}

//__________________________________________________________________________
StatusCode EvenEventsSelectorTool::postNext() {
   ATH_MSG_INFO("EvenEventsSelectorTool::postNext");
   StatusCode retc = StatusCode::SUCCESS;
   const DataHandle<AthenaAttributeList> attrList;
   ATH_MSG_INFO(evtStore()->dump());
   StatusCode sc = evtStore()->retrieve(attrList,"EventInfoAtts");
   if (sc.isSuccess()) {
      ATH_MSG_INFO("Found attribute list");
      // dump spec
      coral::AttributeList::const_iterator it = attrList->begin();
      while (it != attrList->end()) {
        //ATH_MSG_INFO("Spec element " << it->specification().name());
        std::ostringstream value;
        it->toOutputStream(value);
        ATH_MSG_INFO("Spec element " << value.str());
        ++it;
      }
      // take only even events
      if ((*attrList)["EventNumber"].data<long>()%2==0) {
        retc = StatusCode::RECOVERABLE;
        ATH_MSG_INFO("Rejecting event");
      }
   }
   else {
      ATH_MSG_INFO("Could not find attribute list");
   }
   return retc;
}
//__________________________________________________________________________
StatusCode EvenEventsSelectorTool::preNext() {
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
