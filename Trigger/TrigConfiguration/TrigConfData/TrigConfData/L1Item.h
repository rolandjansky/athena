/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFDATA_L1ITEM_H
#define TRIGCONFDATA_L1ITEM_H

#include "TrigConfData/DataStructure.h"

namespace TrigConf {

   /** 
    * @brief L1 threshold configuration
    *
    * Provides access to the L1 item name and ID and the deadtime settings, 
    * trigger type, logic, and partition.
    */
   class L1Item final : virtual public DataStructure {
   public:

      /** Constructor */
      L1Item();

      /** Constructor initialized with configuration data 
       * @param data The data containing the L1 item configuration 
       */
      L1Item(const ptree & data);

      /** Destrutor */
      ~L1Item();

      /** Accessor to the item name */
      const std::string & name() const;

      /** Accessor to the CTP ID */
      unsigned int ctpId() const;

      /** Accessor to the item definition */
      const std::string & definition() const;

      /** Accessor to the complex deadtime
       *
       * This can be 0 or 1, but 1 has never been used so far
       */
      unsigned int complexDeadtime() const;

      /** Accessor to the item monitoring definition */
      const std::string & monitor() const;

      /** Accessor to the item partition */
      unsigned int partition() const;

      /** Accessor to the item triggerType */
      const std::string & triggerType() const;

      /** Accessor to the item logic
       *
       * The current description of the logic is rather complex and
       * difficult to parse and should be redesigned.
       */
      DataStructure logic() const;
      
   };

}

#endif
