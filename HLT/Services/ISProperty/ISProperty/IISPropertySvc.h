// this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISPROPERTY_IISPROPERTYSVC_H 
#define ISPROPERTY_IISPROPERTYSVC_H 

#include "GaudiKernel/IService.h"
#include "ISProperty/ISProperty.h"

/**
 *  @class IISPropertySvc
 *  @brief Interface for IS property service 
 *
 *  @author Sebastian Boeser
 * 
 */
 
class IISPropertySvc : virtual public IService {

  public:

    /** Return the interface identifier **/
    static const InterfaceID& interfaceID();
      
    /** Publish a set of properties to IS under a given name */
    virtual StatusCode publish( const std::string name, const std::string type_name, const PropertyList props, double updateTime ) = 0 ;

    /** Unpublish a set of properties with a given name */
    virtual StatusCode unpublish( const std::string name ) = 0 ;

};

inline const InterfaceID& IISPropertySvc::interfaceID() { 
  static InterfaceID s_iIsPropertySvcID("IISPropertySvc", 1 , 0); 
  return s_iIsPropertySvcID; 
}

#endif  

