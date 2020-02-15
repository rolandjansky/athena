/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFDATA_L1MENU_H
#define TRIGCONFDATA_L1MENU_H

#include "TrigConfData/ConstIter.h"
#include "TrigConfData/DataStructure.h"
#include "TrigConfData/L1Item.h"
#include "TrigConfData/L1Threshold.h"
#include "TrigConfData/L1Connector.h"

#include <vector>
#include <map>

namespace TrigConf {

   /** 
    * @brief L1 menu configuration
    *
    * Provides access to menu name and ctpVersion and to the L1 items and thresholds
    */
   class L1Menu final : public DataStructure {
   public:

      /** Constructor */
      L1Menu();
      L1Menu(const L1Menu&) = default;
      L1Menu(L1Menu&&) = default;

      /** Constructor initialized with configuration data 
       * @param data The data containing the L1 menu 
       */
      L1Menu(const ptree & data);

      /** Destructor */
      virtual ~L1Menu();

      /** Accessor to the menu name */
      std::string name() const;

      /** Accessor to the menu version */
      unsigned int version() const;

      /** Accessor to the version of the CTP format */
      unsigned int ctpVersion() const;

      /** Accessor to the number of L1 items */
      std::size_t size() const;

      /** Get item by name */
      L1Item item(const std::string & itemName) const;

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

      /** Access to list of all L1Thresholds
       * (requires vector copy)
       */
      std::vector<TrigConf::L1Threshold> thresholds() const;

      /** Access to list of L1Thresholds by type */
      const std::vector<TrigConf::L1Threshold> & thresholds(const std::string & typeName) const;

      /** Access to L1Threshold by name */
      const TrigConf::L1Threshold & threshold(const std::string & thresholdName) const;

      /** List of L1 thresholds types */
      std::vector<std::string> thresholdTypes() const;

      /** Access to connector by name */
      const TrigConf::L1Connector & connector(const std::string & connectorName) const;

      /** Connector names */
      std::vector<std::string> connectorNames() const;

      /** print overview of L1 Menu */
      void printMenu(bool full = false) const;

   private:

      /** Update the internal data after modification of the data object */
      virtual void update() override;

      std::string m_name{""};

      std::map<std::string, TrigConf::L1Connector> m_connectors{};

      std::map<std::string, std::vector<TrigConf::L1Threshold>> m_thresholdsByType{};
      std::map<std::string, TrigConf::L1Threshold*> m_thresholdsByName{};

   };

}

#ifndef TRIGCONF_STANDALONE

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( TrigConf::L1Menu , 26419484 , 1 )

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( TrigConf::L1Menu , 11747932 );

#endif

#endif
