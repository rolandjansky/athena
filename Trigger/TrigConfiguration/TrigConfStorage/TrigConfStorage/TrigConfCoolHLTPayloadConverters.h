/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConfStorage_TrigConfCoolHLTPayloadConverters
#define TrigConfStorage_TrigConfCoolHLTPayloadConverters

#include <string>
#include "CoolKernel/pointers.h"

namespace cool {
   class IRecord;
   class Record;
}

namespace coral {
   class AttributeList;
}

namespace TrigConf {

   class HLTChain;
   class HLTChainList;
   class HLTSequenceList;

   namespace TrigConfCoolHLTPayloadConverters {


      /**@brief build a COOL db record from a HLT chain
       *
       * @param ch the HLT trigger chain
       *
       * @returns a COOL db record
       */
      cool::Record createHltMenuPayload(cool::IFolderPtr, const TrigConf::HLTChain&, const std::string& concise );

      /**@brief build a COOL db record from a HLT chain
       *
       * @param ch the HLT trigger chain
       * @returns a COOL db record
       */
      cool::Record createHltChainGroupPayload(cool::IFolderPtr, const TrigConf::HLTChain & ch);


      /**@brief build a COOL db record from a set of HLT configuration keys
       *
       * @param masterKey the configuration super master key
       * @param hltPrescaleKey the HLT prescale key
       * @param configSource the source of configuration information (DB
       * connection string or XML file name)
       *
       * @returns a COOL db record
       */
      cool::Record createHltConfigKeysPayload( cool::IFolderPtr, unsigned int masterKey,
                                               unsigned int hltPrescaleKey, const std::string& configSource );


       /**@brief build a COOL db record from a HLT monitoring configuration key
        *
        * @param MCK the monitoring configuration key
        * @param configSource the source of configuration information (DB
        * connection string or XML file name)
        *
        * @returns a COOL db record
        */
       cool::Record createMonConfigKeyPayload(cool::IFolderPtr fld, uint MCK, std::string& info );     
      
      /**@brief build a COOL db record from a HLT prescale value
       *
       * @param prescale the prescale value
       * @returns a COOL db record
       *
       * Note that the prescale values are inserted in the same order as
       * the trigger items, the matching between them depends on their
       * position in the list (COOL channel ID)
       */
      cool::Record createHltPrescalesPayload(cool::IFolderPtr, float prescale,
                                             float passthrough, float rerunprescale);


      /**@brief build a COOL db record from a HLT prescale value
       *
       * @param prescale the prescale value
       * @returns a COOL db record
       *
       * Note that the prescale values are inserted in the same order as
       * the trigger items, the matching between them depends on their
       * position in the list (COOL channel ID)
       */
      cool::Record createHltPrescaleKeyPayload(cool::IFolderPtr, int psk);


      /** @brief build an HLT chain from a COOL db record */
      HLTChain * createHLTChain( const coral::AttributeList & al, TrigConf::HLTSequenceList* sequences=0 );
     

      /** @brief add the groups to and HLT chain from a AttributeList */
      void addGroupsToHltChain( const coral::AttributeList & al, const TrigConf::HLTChainList& chl);


      /** @brief read the HLT configuration keys from a coral::AttributeList */
      void readHltConfigKeys( const coral::AttributeList & al, unsigned int & masterConfigKey,
                              unsigned int & hltPrescaleKey, std::string & configSource );


      /** @brief build the HLT prescale value from a COOL db record */
      void readHltPrescale( const coral::AttributeList & al, float& ps, float& pt, float& rrps );


      /** @brief build the HLT prescale key from a COOL db record */
      int readHltPrescaleKey( const coral::AttributeList & al );
   }
}

#endif
