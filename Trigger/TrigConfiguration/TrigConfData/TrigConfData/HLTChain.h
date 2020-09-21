/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
   class Chain final : public DataStructure {
   public:

      /** Constructor */
      Chain();

      /** Constructor initialized with configuration data 
       * @param data The data containing the HLT chain configuration 
       */      
      Chain(const boost::property_tree::ptree & data);
      Chain(const std::string & name, const boost::property_tree::ptree & data);

      /** Destructor */
      virtual ~Chain();

      virtual std::string className() const override;

      /** Accessor to the chain counter
       *
       * The chain counter is unique with the menu
       */
      unsigned int counter() const;

      /** Accessor to the chain name hash
       *
       * The hash is unique within the menu and identifies the chain online
       */
      unsigned int namehash() const;

      /** Accessor to the seeding L1 item */
      const std::string & l1item() const;

      /** Accessor to the l1 thresholds */
      std::vector<std::string> l1thresholds() const;

      /** Accessor to the connected output streams */
      std::vector<std::string> streams() const;

      /** Accessor to the groups this chain belongs to */
      std::vector<std::string> groups() const;

      /** Accessor to the sequencers this chain belongs to */
      std::vector<std::string> sequencers() const;

   private:
      void update() override;
      void load();
   };

}

#endif
