/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConfXMLLoader
#define TrigConfXMLLoader

#include "TrigConfStorage/ILoader.h"
#include "TrigConfStorage/XMLStorageMgr.h"
#include "boost/property_tree/ptree.hpp"
#include "TrigConfBase/TrigConfMessaging.h"

namespace TrigConf {

   //class XMLStorageMgr;

   typedef boost::property_tree::ptree ptree;
   typedef boost::property_tree::ptree::value_type value_type;

   /**@brief Base class for loaders of XML based configurations*/
   class XMLLoader : public TrigConfMessaging, 
                     virtual public ILoader
   {
   public:

      /**@brief constructor
       * @param sm reference to storage manager
       * @param data is a ptree
       */
      explicit XMLLoader( XMLStorageMgr& sm, const value_type* data );

      explicit XMLLoader( const std::string& name, XMLStorageMgr& sm, const value_type* data );

      /**@brief destructor*/
      virtual ~XMLLoader(){};

      /**@brief sets the internal pointer to the DOM (XML) node*/
      void setPtree(const value_type* data) { m_data = data; }

      virtual void setLevel(MSGTC::Level lvl);
      MSGTC::Level outputLevel() const { return msg().level(); }

      virtual int verbose() const { return m_verbose; }
      virtual void setVerbose(int v) { m_verbose=v; }

   protected:

      bool is_a(const std::string& elementtag) const;

      const std::string & key() const;
      const ptree & pt() const;

   private:
      int m_verbose { 1 };

   public:
      typedef enum  {ALL, CTP, CTPOnl, HLT, COOLL1, COOLHLT, L1Simu} ENV;
      static        ENV getEnv()    { return m_env; }
      static void   setEnv(ENV env) { m_env = env;  }

   protected:
      XMLStorageMgr&                   m_storageMgr; ///< reference to the storage manager
      const value_type*                      m_data;       ///< pointer to the XML node

   private:
      static ENV m_env;
   };

}

#endif

