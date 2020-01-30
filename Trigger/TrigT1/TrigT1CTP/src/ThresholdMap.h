/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CTP_THRESHOLDMAP_H
#define TRIGT1CTP_THRESHOLDMAP_H

#include "./CTPTriggerThreshold.h"

#include "TrigConfData/L1Menu.h"

#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/PIT.h"
#include "TrigConfL1Data/TIP.h"

#include <vector>
#include <map>

namespace LVL1CTP {

   /**
    *   @short Access to TrigConf::CTPTriggerThreshold by name
    */
   
   class ThresholdMap {
   public:
      ThresholdMap() = delete;
      
      // construct map from L1Menu (Run 3)
      ThresholdMap( const TrigConf::L1Menu * l1menu );

      // construct map from thresholds using default PIT mapping (Run 1)
      ThresholdMap( const std::vector< TrigConf::TriggerThreshold* >& threshold_vector);
      
      // construct map from thresholds using given PIT mapping (Run 1)
      ThresholdMap( const std::vector< TrigConf::TriggerThreshold* >& threshold_vector, const std::vector< TrigConf::PIT* >& pit_vector);
      
      // construct map from thresholds using default TIP mapping (Run 2)
      ThresholdMap( const std::vector< TrigConf::TriggerThreshold* >& threshold_vector, const std::vector< TrigConf::TIP* >& pit_vector );

      /// default destructor
      ~ThresholdMap();
      
      // Function returning the threshold names
      std::vector<std::string> getThresholdNames() const;

      // Function returning the CTPTriggerThreshold object belonging to the given TriggerThreshold object
      const CTPTriggerThreshold & getCTPThreshold( const std::string & thrName ) const;

   private:
      
      /// map storing the accosiation between threshold name and CTPTriggerThreshold
      std::map< std::string, const CTPTriggerThreshold* > m_mapByName;

   };

}

#endif
