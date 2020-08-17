/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFDATA_L1BOARD_H
#define TRIGCONFDATA_L1BOARD_H

#include "TrigConfData/ConstIter.h"
#include "TrigConfData/DataStructure.h"

#include <map>
#include <vector>

namespace TrigConf {

   /** @brief a TriggerLine entry describes the location of a threshold multiplicity on a cable (connector)
    *
    * for electrical connections from L1Topo boards it also knows
    * which fpga they come from and which clock signal they have (those signals run on doubled clock)
    */

   /** @brief L1 board configuration */
   class L1Board final : public DataStructure {
   public:

      enum class BoardType { CTPIN, TOPO, MUCTPI };

      /** Constructor */
      L1Board();

      L1Board(const L1Board &) = delete;
      L1Board& operator=(const L1Board&) = delete;
      L1Board(L1Board&&) = delete;

      /** Constructor initialized with configuration data 
       * @param data The data containing the L1 menu 
       */
      L1Board(const std::string & boardName, const ptree & data);

      /** Destructor */
      virtual ~L1Board();

      virtual std::string className() const;

      /** Accessor to the number of connectors */
      std::size_t size() const;

      std::string type() const;

      BoardType boardType() const;

      /** Accessor to the connector type */
      bool legacy() const;

      /** Accessor to connector names */
      const std::vector<std::string> & connectorNames() const;

   private:

      /** Update the internal members */
      virtual void update();

      BoardType m_boardType;
      bool m_legacy;
      std::vector<std::string> m_connectorNames;     
   };

}

#endif
