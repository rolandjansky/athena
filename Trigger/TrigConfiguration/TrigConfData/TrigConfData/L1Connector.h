/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFDATA_L1CONNECTOR_H
#define TRIGCONFDATA_L1CONNECTOR_H

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
   class TriggerLine {
   public:
      TriggerLine(const std::string & name, unsigned int startbit, unsigned int nbits, unsigned int fpga=0, unsigned int clock=0) :
         m_name(name), m_startbit(startbit), m_nbits(nbits), m_fpga(fpga), m_clock(clock)
      {}
      const std::string & name() const { return m_name; }
      unsigned int startbit() const { return  m_startbit; }
      unsigned int endbit() const { return  m_startbit + m_nbits - 1; }
      unsigned int nbits() const { return m_nbits; }
      unsigned int fpga() const { return m_fpga; }
      unsigned int clock() const { return m_clock; }
   private:
      std::string m_name{""};      // the name of the threshold whose multiplicity is transmitted
      unsigned int m_startbit{0};  // the location on the cable - first bit
      unsigned int m_nbits{0};     // the location on the cable - number of bits used to encode the multiplicity
      unsigned int m_fpga{0};      // for electrical signals from L1Topo boards only: the fpga the signal is coming from
      unsigned int m_clock{0};     // for electrical signals from L1Topo boards only: the clock of the signal
   };

   /** @brief L1 connectors configuration */
   class L1Connector final : public DataStructure {
   public:

      enum class ConnectorType { ELECTRICAL, OPTICAL, CTPIN };

      /** Constructor */
      L1Connector();

      L1Connector(const L1Connector &) = delete;
      L1Connector& operator=(const L1Connector&) = delete;
      L1Connector(L1Connector&&) = delete;

      /** Constructor initialized with configuration data 
       * @param data The data containing the L1 menu 
       */
      L1Connector(const std::string & connName, const ptree & data);

      /** Destructor */
      virtual ~L1Connector();

      virtual std::string className() const;

      /** Accessor to the number of trigger lines */
      std::size_t size() const;

      /** Accessor to the connector type */
      ConnectorType type() const;

      /** names of all trigger lines */
      std::vector<std::string> triggerLineNames() const;

      /** Accessor to the triggerlines on the connector
       * 
       * For electrical connectors from the L1Topo boards a triggerline vector holds up to 16 signals, which come from 
       * the same fpga and are transmitted at the same clock flank. So in this case the fpga and clock have to be specified.
       * For all other connectors the default value 0 has to be used for fpga and clock
       *
       * @param fpga - the L1Topo fpga (0 or 1)
       * @param clock - the clock of the signal group (0 or 1)
       */
      const std::vector<TrigConf::TriggerLine> & triggerLines(unsigned int fpga = 0, unsigned int clock = 0) const;

      bool hasLine( const std::string & lineName ) const;

      const TrigConf::TriggerLine & triggerLine( const std::string & lineName ) const;

      bool isLegacy() const { return m_isLegacy; }

   private:

      /** Update the internal members */
      virtual void update();

      ConnectorType m_type;
      std::vector<TrigConf::TriggerLine> m_triggerLines[2][2];
      std::map<std::string, TrigConf::TriggerLine*> m_lineByName;
      std::size_t m_maxFpga{1};
      std::size_t m_maxClock{1};

      bool m_isLegacy;
   };

}

#endif
