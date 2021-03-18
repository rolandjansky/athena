/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFDATA_L1CTP_H
#define TRIGCONFDATA_L1CTP_H

#include "TrigConfData/DataStructure.h"

#include <map>

namespace TrigConf {

   /** @brief a TriggerLine entry describes the location of a threshold multiplicity on a cable (connector)
    *
    * for electrical connections from L1Topo boards it also knows
    * which fpga they come from and which clock signal they have (those signals run on doubled clock)
    */

   /** @brief L1 board configuration */
   class L1CTP final : public DataStructure {
   public:

      L1CTP() = default;
      /** Constructor initialized with configuration data 
       * @param data The data containing the L1 CTP configuration 
       */
      L1CTP(const std::string & name, const ptree & data);

      L1CTP(const L1CTP &) = delete;
      L1CTP& operator=(const L1CTP&) = delete;
      L1CTP(L1CTP&&) = delete;

      ~L1CTP() = default;

      /** @brief name of ctpin connector
       * @param slot CTPIN board 7..9 
       * @param conn CTPIN connector 0..3 on each board
       */
      const std::string & ctpin(size_t slot, size_t conn) const;

      const std::string & electrical(size_t conn) const;

      const std::string & optical(size_t conn) const;

      const std::map<std::string, std::pair<size_t,std::string>> ctpMon() const { return m_ctpmon; }

      /** Clearing the configuration data */
      virtual void clear() override;

   private:

      /** Update the internal data after modification of the data object */
      virtual void update() override { load(); };
      void load();

      std::string m_ctpin[3][4];
      std::string m_electrical[3];
      std::string m_optical[12];

      std::map<std::string, std::pair<size_t,std::string>> m_ctpmon;
   };
}

#endif
