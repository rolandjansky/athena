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

      // class name
      virtual std::string className() const override {
         return "L1Menu";
      }

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

      /** Access to topo algorithm names
       * @param category must be one of "TOPO", "MUTOPO", "MULTTOPO", or "R2TOPO"
       *
       * TOPO     ... new topo algorithms,
       * R2TOPO   ... Run-2 topo algorithms
       * MUTOPO   ... MUCTPI topo algorithms
       * MULTTOPO ... multiplicity algorithms 
       */
      std::vector<std::string> topoAlgorithmNames(const std::string & category) const;

      /** Access to topo algoritm output names */
      std::vector<std::string> topoAlgorithmOutputNames(const std::string & category) const;

      /** Access to topo algorithm by name */
      const TrigConf::L1TopoAlgorithm & algorithm(const std::string & algoName, const std::string & category) const;

      /** Access to topo algorithm by name of triggerline as given in topo connector specification
       * @param triggerlineName string name of the triggerline 
       * @return reference to the algorithm that produces this output
       *
       * The name of the triggerline starts with "TOPO_", "MUTOPO_", or "R2TOPO_", except in the case of multiplicity output lines which are just the threshold name
       */
      const TrigConf::L1TopoAlgorithm & algorithmFromTriggerline(const std::string & triggerlineName) const;

      /** Access to topo algorithm by name of the output as given in the algorithm definition
       * @param fullOutputName string name of the output prefixed with the category, e.g. ("HT150-J20s5.ETA31","TOPO") or ("0INVM9-EM7ab-EMab","R2TOPO")
       * @return reference to the algorithm that produces this output
       */
      const TrigConf::L1TopoAlgorithm & algorithmFromOutput(const std::string & bareOutputName, const std::string & category) const;

      /** Access to boards by name */
      const TrigConf::L1Board & board(const std::string & boardName) const;

      /** Board names */
      std::vector<std::string> boardNames() const;


      /** Access to connector by name */
      const TrigConf::L1Connector & connector(const std::string & connectorName) const;

      /** Connector names */
      std::vector<std::string> connectorNames() const;

      /** Name of connector from name of threshold or triggerline */
      const std::string & connectorNameFromThreshold(const std::string & thresholdName) const;

      /** print overview of L1 Menu */
      void printMenu(bool full = false) const;

   private:

      /** Update the internal data after modification of the data object */
      virtual void update() override;

      /** the supermasterkey */
      unsigned int m_smk {0};

      /** connector by name */ 
      std::map<std::string, TrigConf::L1Connector> m_connectors{};

      /** connector name by threshold name */ 
      std::map<std::string, std::string> m_threshold2ConnectorName{};

      /** board by name */
      std::map<std::string, TrigConf::L1Board> m_boards{};

      /** threshold maps */
      std::map<std::string, std::vector<std::shared_ptr<TrigConf::L1Threshold>>> m_thresholdsByType{};
      std::map<std::string, std::shared_ptr<TrigConf::L1Threshold>> m_thresholdsByName{};

      TrigConf::L1ThrExtraInfo m_thrExtraInfo;

      /** algorithm maps */
      std::map<std::string, std::vector<TrigConf::L1TopoAlgorithm>> m_algorithmsByCategory{}; // primary set of vectors of algos, one per category (TOPO, R2TOPO, MUTOPO, MULTTOPO)
      std::map<std::string, std::map<std::string, TrigConf::L1TopoAlgorithm*>> m_algorithmsByName{}; // map from category and algorithm name to algorithm 
      std::map<std::string, std::map<std::string, TrigConf::L1TopoAlgorithm*>> m_algorithmsByOutput{}; // map from category and output name to algorithm

   };

}

#ifndef TRIGCONF_STANDALONE

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( TrigConf::L1Menu , 26419484 , 1 )

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( TrigConf::L1Menu , 11747932 );

#endif

#endif
