/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConfigSvc_HLTJobOptionsSvc
#define TrigConfigSvc_HLTJobOptionsSvc

#include <string>
#include <vector>
#include <memory>

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrigConfigSvc/ConfigSvcBase.h"
#include "TrigConfigSvc/JobOptionsCatalogue.h"

/**
 *  $Date: 2009-03-05 18:07:40 $
 *
 *  @short Service providing the HLT trigger job properties
 *
 *  @author Joerg Stelzer
 *  @version $Revision: 1.19 $
 *
 */

class StatusCode;

namespace TrigConf {

   class TrigDBConnectionConfig;
   class JobOptionTable;
  
   class HLTJobOptionsSvc : public extends2<ConfigSvcBase, IProperty, IJobOptionsSvc>
   {
   public:
      
      // Constructor and destructor
      HLTJobOptionsSvc( const std::string& name, ISvcLocator* pSvcLocator );
      virtual ~HLTJobOptionsSvc();

      // Service initialization method
      virtual StatusCode sysInitialize();
      virtual StatusCode initialize();

      // Service finalization method
      virtual StatusCode finalize();

      // IJobOptionsSvc::setMyProperties
      virtual StatusCode setMyProperties( const std::string& client,
                                          IProperty* myInt );
    
      /// implementation of IJobOptionsSvc::addPropertyToCatalogue
      virtual StatusCode addPropertyToCatalogue( const std::string& client, 
                                                 const Property& property );
    
      /// implementation of IJobOptionsSvc::removePropertyFromCatalogue
      virtual StatusCode removePropertyFromCatalogue( const std::string& client, 
                                                      const std::string& name );
    

      // implementation of IJobOptionsSvc::getProperties
      virtual const std::vector<const Property*>* getProperties( const std::string& client) const; 

      // To avoid that the previous method shadowes the Service::getProperties()
      virtual const std::vector<Property*> & getProperties() const { return Service::getProperties(); }


     /// Get a property for a client
     virtual const Property* 
     getClientProperty( const std::string& client,
                        const std::string& name ) const;


      /// implementation of IJobOptionsSvc::getClients
      virtual std::vector<std::string> getClients() const;
    
      /** look for file 'File' into search path 'Path' 
       *  and read it to update existing JobOptionsCatalogue 
       *  @param  File file   name 
       *  @param  Path search path 
       *  @return status code 
       */
      virtual StatusCode readOptions( const std::string& file      , 
                                      const std::string& path = "" ) ;
    
    
      StatusCode queryInterface( const InterfaceID& riid, void** ppvIF );
    
      // Access functions described by IHLTJobOptionsSvc:
      StatusCode readDbWithMasterKey();

      // IProperty implementation (needed for initialisation)
      using Service::setProperty;
      using Service::getProperty;

      //     virtual StatusCode setProperty(const Property& p);
      //     virtual StatusCode setProperty(std::string& s) { return Service::setProperty(s); }
      //     virtual StatusCode setProperty(const std::string& n, const std::string& v) { return Service::setProperty(n,v); }

      //     virtual StatusCode getProperty(Property *p) const;
      //     virtual const Property& getProperty(const std::string& name) const { return Service::getProperty(name); }
      //     virtual StatusCode getProperty(const std::string& n, std::string& v) const { return Service::getProperty(n,v); }
    
      void PrintCatalogue(std::ostream & os) const;

   private:

      StatusCode FillCatalogue(const JobOptionTable & jot);

      StatusCode readEnv();

      // Properties:
      std::string m_dirSearchPath;
      std::string m_sourcePath;
      std::string m_sourceType;
      JobOptionsCatalogue m_catalogue;

   }; // class HLTJobOptionsSvc

   // little helper functions -- have been moved to TrigConfL1Data/HelperFunctions class

} // namespace TrigConf

#endif // TrigJobOption_HLTJobOptionsSvc_h
