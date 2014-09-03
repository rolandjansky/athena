/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_IGMASVC_H
#define ATHENAKERNEL_IGMASVC_H

// Include Files
#ifndef GAUDIKERNEL_IINTERFACE_H
 #include "GaudiKernel/IInterface.h"
#endif
#ifndef KERNEL_STATUSCODES_H
 #include "GaudiKernel/StatusCode.h"
#endif
#ifndef _CPP_STRING
 #include <string>
#endif

/** @class IGMASvc IGMASvc.h AthenaKernel/IGMASvc.h

 * @brief The interface implemented by the GMASvc service.

 * @author David Quarrie
*/
class IGMASvc : virtual public IInterface {
public:
  
  virtual StatusCode report( unsigned int level, const char* eventName, const char* eventData=0 ) = 0;
  virtual StatusCode report( unsigned int level, const std::string& eventName ) = 0;
  virtual StatusCode report( unsigned int level, const std::string& eventData, const std::string& eventData ) = 0;

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { 
    static const InterfaceID _iid("IGMASvc", 1, 0);
    return _iid;
  }
};

#endif // ATHENAKERNEL_IGMASVC_H



