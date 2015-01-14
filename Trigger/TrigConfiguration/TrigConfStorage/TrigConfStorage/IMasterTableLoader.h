/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_IMasterTableLoader
#define TrigConf_IMasterTableLoader

#include "TrigConfStorage/ILoader.h"


namespace TXC{
   class L1TopoMenu;
}

namespace TrigConf {

   class ThresholdConfig;
   class CTPConfig;
   class Muctpi;
   class L1TopoConfig;

   /**
    * @brief Interface for loaders of the LVL1 trigger configuration
    */
   class IMasterTableLoader : virtual public ILoader {
   public:

      IMasterTableLoader() {};

      /**@brief destructor*/       
      virtual ~IMasterTableLoader(){};

      /**
       * @brief Load the LVL1 trigger thresholds from the configuration
       * source
       *
       * @param thrcfg reference to the ThresholdConfig object to be filled
       * 
       * @returns @c true if the loading was successful, @c false otherwise
       */
      virtual bool load(ThresholdConfig& thrcfg) = 0;

      /**
       * @brief Load the LVL1 CTP configuration (trigger menu) from the
       * configuration source (also for online configuration)
       *
       * @param ctpc reference to the CTPConfig object to be filled
       * 
       * @returns @c true if the loading was successful, @c false otherwise
       */
      virtual bool load(CTPConfig& ctpc) = 0;

      /**
       * @brief Load the LVL1 muon hardware configuration from the
       * configuration source
       *
       * @param muctpi reference to the Muctpi object to be filled
       * 
       * @returns @c true if the loading was successful, @c false otherwise
       */
      virtual bool load(Muctpi& muctpi) = 0;

      /**
       * @brief Load the LVL1 topo hardware configuration from the
       * configuration source
       *
       * @param l1topomenu reference to the L1Topo object to be filled
       * 
       * @returns @c true if the loading was successful, @c false otherwise
       */
      virtual bool load(TXC::L1TopoMenu& l1topo) = 0;

   };

}

#endif
