/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFDATA_L1TOPOOUTPUT_H
#define TRIGCONFDATA_L1TOPOOUTPUT_H

#include "TrigConfData/DataStructure.h"

namespace TrigConf {

   /** 
    * @brief L1Topo output line configuration
    *
    * Provides access to the name of the trigger line, to the connected algorithm
    *  and the lines the signals are being sent on.
    */
   class L1TopoOutput final : virtual public DataStructure {
   public:
      /** Constructor */
      L1TopoOutput();

      /** Constructor initialized with configuration data 
       * @param data The data containing the L1Topo output line 
       */      
      L1TopoOutput(const boost::property_tree::ptree & data);

      /** Destructor */
      ~L1TopoOutput();

      /** Accessor to the algorithm name */
      const std::string & algName() const;

      /** Accessor to the algorithm name */
      unsigned int algId() const;

      /** Accessor to the clock the signals are sent on
       *
       * The clock is 0 or 1. In case of multiple output signals from 
       * the same algorithm, they are all on the same clock
       */
      unsigned int clock() const;

      /** Accessor to the cable bit of the first signal
       *
       * In case of multiple signals, they are consecutive, starting at this bit. 
       */
      unsigned int firstBit() const;

      /** Accessor to the FPGA number 
       *
       * The FPGA number can be 0 or 1
       */
      unsigned int fpga() const;

      /** Accessor to the board number
       *
       * The board number can be 0 or 1
       */
      unsigned int board() const;

      /** Accessor to the algorithm trigger lines
       *
       * Comma-separated list of line names
       */
      const std::string & triggerLines() const;

   };

}

#endif
