/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFDATA_L1MENU_H
#define TRIGCONFDATA_L1MENU_H

#include "TrigConfData/ConstIter.h"
#include "TrigConfData/DataStructure.h"
#include "TrigConfData/L1Item.h"
#include "TrigConfData/L1Threshold.h"

#include <vector>

namespace TrigConf {

   /** 
    * @brief L1 menu configuration
    *
    * Provides access to menu name and ctpVersion and to the L1 items and thresholds
    */
   class L1Menu final : virtual public DataStructure {
   public:

      /** Constructor */
      L1Menu();

      /** Constructor initialized with configuration data 
       * @param data The data containing the L1 menu 
       */
      L1Menu(const ptree & data);

      /** Destructor */
      ~L1Menu();

      /** Accessor to the menu name */
      std::string name() const;

      /** Accessor to the menu version */
      unsigned int version() const;

      /** Accessor to the version of the CTP format */
      unsigned int ctpVersion() const;

      /** Accessor to the number of L1 items */
      std::size_t size() const;

      /** Iterator over the L1 items */
      using const_iterator = ConstIter<ptree, L1Item>;

      /** Begin of the L1 items list
       *
       * Needed for range-based loops
       */
      const_iterator begin() const;

      /** End of the L1 items list
       *
       * Needed for range-based loops
       */
      const_iterator end() const;

      /** List of L1 thresholds */
      std::vector<TrigConf::L1Threshold> thresholds() const;

   };

}

#endif
