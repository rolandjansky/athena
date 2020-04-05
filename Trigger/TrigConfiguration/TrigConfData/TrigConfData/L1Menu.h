/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFDATA_L1MENU_H
#define TRIGCONFDATA_L1MENU_H

#include "TrigConfData/ConstIter.h"
#include "TrigConfData/DataStructure.h"
#include "TrigConfData/L1Item.h"
#include "TrigConfData/L1Connector.h"
#include "TrigConfData/L1Board.h"
#include "TrigConfData/L1TopoAlgorithm.h"
#include "TrigConfData/L1Threshold.h"
#include "TrigConfData/L1ThrExtraInfo.h"

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

      /** Accessor to the menu version */
      unsigned int version() const;

      /** Accessor to the version of the CTP format */
      unsigned int ctpVersion() const;

      /** Accessor to the number of L1 items */
      std::size_t size() const;

      /** setter and getter for the supermasterkey */
      unsigned int smk() const;
      void setSMK(unsigned int psk);

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

      /** List of L1 thresholds types */
      std::vector<std::string> thresholdTypes() const;

      /** List of L1 thresholds names */
      std::vector<std::string> thresholdNames() const;

      /** Access to list of all L1Thresholds */
      std::vector<std::shared_ptr<TrigConf::L1Threshold>> thresholds() const;

      /** Access to list of L1Thresholds by type */
      const std::vector<std::shared_ptr<TrigConf::L1Threshold>> & thresholds(const std::string & typeName) const;

      /** Access to L1Threshold by name */
      const TrigConf::L1Threshold & threshold(const std::string & thresholdName) const;

      /** Access to extra info for threshold types */
      const L1ThrExtraInfo & thrExtraInfo() const;

      /** Access to topo algorithm by name */
      const TrigConf::L1TopoAlgorithm & algorithm(const std::string & algoName) const;

      /** Access to topo algorithm by output */
      const TrigConf::L1TopoAlgorithm & algorithmFromOutput(const std::string & outputName) const;


      /** Access to boards by name */
      const TrigConf::L1Board & board(const std::string & boardName) const;

      /** Board names */
      std::vector<std::string> boardNames() const;


      /** Access to connector by name */
      const TrigConf::L1Connector & connector(const std::string & connectorName) const;

      /** Connector names */
      std::vector<std::string> connectorNames() const;


      /** print overview of L1 Menu */
      void printMenu(bool full = false) const;

   private:

      /** Update the internal data after modification of the data object */
      virtual void update() override;

      /** the supermasterkey */
      unsigned int m_smk {0};

      std::map<std::string, TrigConf::L1Connector> m_connectors{};

      std::map<std::string, TrigConf::L1Board> m_boards{};

      std::map<std::string, std::vector<std::shared_ptr<TrigConf::L1Threshold>>> m_thresholdsByType{};
      std::map<std::string, std::shared_ptr<TrigConf::L1Threshold>> m_thresholdsByName{};

      TrigConf::L1ThrExtraInfo m_thrExtraInfo;


      std::map<std::string, std::vector<TrigConf::L1TopoAlgorithm>> m_algorithmsByType{};
      std::map<std::string, TrigConf::L1TopoAlgorithm*> m_algorithmsByName{};
      std::map<std::string, TrigConf::L1TopoAlgorithm*> m_algorithmsByOutput{};

   };

}

#ifndef TRIGCONF_STANDALONE

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( TrigConf::L1Menu , 26419484 , 1 )

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( TrigConf::L1Menu , 11747932 );

#endif

#endif
