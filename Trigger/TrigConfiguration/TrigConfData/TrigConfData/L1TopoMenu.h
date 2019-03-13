/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFDATA_L1TOPOMENU_H
#define TRIGCONFDATA_L1TOPOMENU_H

#include "TrigConfData/ConstIter.h"
#include "TrigConfData/DataStructure.h"
#include "TrigConfData/L1TopoOutput.h"
#include "TrigConfData/L1TopoAlgorithm.h"

#include <vector>

namespace TrigConf {

   /** 
    * @brief L1Topo menu configuration
    *
    * Provides access to menu name and to the L1Topo putput lines
    */
   class L1TopoMenu final : virtual public DataStructure {
   public:

      /** Constructor */
      L1TopoMenu();

      /** Constructor initialized with configuration data 
       * @param data The data containing the L1Topo menu 
       */
      L1TopoMenu(const ptree & data);

      /** Destructor */
      ~L1TopoMenu();

      /** Accessor to the menu name */
      std::string name() const;

      /** Accessor to the menu version */
      unsigned int version() const;

      /** Access to the number of L1TopoOutput lines */
      std::size_t size() const;

      /** Iterator over the L1Topo output lines */
      using const_iterator = ConstIter<ptree, L1TopoOutput>;

      /** Begin of the L1Topo output lines list
       *
       * Needed for range-based loops
       */
      const_iterator begin() const;

      /** End of the L1Topo output lines list
       *
       * Needed for range-based loops
       */
      const_iterator end() const;

      /** List of sorting algorithms */
      std::vector<TrigConf::L1TopoAlgorithm> sortingAlgorithms() const;

      /** List of decision algorithms */
      std::vector<TrigConf::L1TopoAlgorithm> decisionAlgorithms() const;

   };

}

#endif
