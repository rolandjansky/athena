///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// xAODTEvent.h 
// Header file for class xAODTEvent
// Author: Johannes Elmsheuser, Will Buttinger 
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENAROOTCOMPS_XAODTEVENT_H
#define ATHENAROOTCOMPS_XAODTEVENT_H 1

//This class exists purely to gain public access to the getInputObject method of
//xAOD::TEvent. We need this because we cannot use the templated retrieve method


#include "xAODRootAccess/TEvent.h"

namespace xAOD {

class xAODTEvent : public xAOD::TEvent
{ 
 public: 
  using TEvent::TEvent;
  using TEvent::getInputObject;
  /*inline virtual const void* getInputObject( const std::string& key,
			const std::type_info& ti ) {
      return TEvent::getInputObject(key, ti);
  }*/
}; 

} //> end namespace xAOD

#endif //> !ATHENAROOTCOMPS_XAODTEVENT_H
