/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFDATA_L1THRESHOLD_H
#define TRIGCONFDATA_L1THRESHOLD_H

#include "TrigConfData/DataStructure.h"
#include <vector>

namespace TrigConf {

   /** 
    * @brief L1 threshold configuration
    *
    * Provides access to the L1Topo algorithm name and type and the selection parameters,
    * input and output collections. Can hold sorting and decision algorithms
    */
   class L1Threshold final : public DataStructure {
   public:

      /** Constructor */
      L1Threshold();

      /** Constructor initialized with configuration data 
       * @param data The data containing the L1 threshold 
       */
      L1Threshold(const ptree & data);

      /** Destructor */
      ~L1Threshold();

      /** Accessor to the threshold name */
      const std::string & name() const;

      /** Accessor to the threshold type */
      const std::string & type() const;

      /** Accessor to the input board type */
      const std::string & input() const;

      /** Accessor to the mapping number
       *
       * The mapping is unique within a type
       */
      unsigned int mapping() const;

      /** Accessor to the threshold ID */
      unsigned int id() const;

      /** Accessor to the list of threshold values */
      std::vector<DataStructure> thresholdValues() const;
      
      /** Accessor to the cabling information
       *
       * If no cabling information is available then bool(cable()) is false
       */
      DataStructure cable() const;
      
   };

}

#endif
