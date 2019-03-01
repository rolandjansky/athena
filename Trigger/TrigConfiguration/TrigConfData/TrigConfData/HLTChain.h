/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFDATA_HLTCHAIN_H
#define TRIGCONFDATA_HLTCHAIN_H

#include "TrigConfData/DataStructure.h"

namespace TrigConf {

   /** 
    * @brief HLT chain configuration
    *
    * Provides access to the name and counter of the HLT chain, to 
    * the seeding L1 item and to the connected output streams and chain groups.
    */
   class HLTChain final : virtual public DataStructure {
   public:

      /** Constructor */
      HLTChain();

      /** Constructor initialized with configuration data 
       * @param data The data containing the HLT chain configuration 
       */      
      HLTChain(const boost::property_tree::ptree & data);

      /** Destructor */
      ~HLTChain();

      /** Accessor to the chain name */      
      const std::string & name() const;

      /** Accessor to the chain counter
       *
       * The chain counter is unique with the menu
       */
      unsigned int counter() const;

      /** Accessor to the seeding L1 item */
      const std::string & l1item() const;

      /** Accessor to the connected output streams */
      std::vector<DataStructure> streams() const;

      /** Accessor to the groups this chain belongs to */
      std::vector<std::string> groups() const;
   };

}

#endif
