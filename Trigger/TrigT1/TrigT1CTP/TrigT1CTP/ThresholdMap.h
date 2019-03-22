/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGT1CTP_THRESHOLDMAP_H
#define TRIGT1CTP_THRESHOLDMAP_H

// STL include(s):
#include <vector>
#include <map>

// local include(s):
#include "TrigT1CTP/MsgLogger.h"

// Forward declarations:
namespace TrigConf {
  class TriggerThreshold;
  class PIT;
	class TIP;
}

namespace LVL1CTP {

  // Forward declarations in LVL1CTP namespace:
  class CTPTriggerThreshold;

  /**
   *
   *   @short Class glueing TrigConf::TriggerThreshold and LVL1CTP::DecisionTriggerThreshold object together
   *
   *          This class automatically generates and manages CTPTriggerThreshold objects belonging
   *          to each and every TriggerThreshold object given to it in it's constructor. It is important
   *          therefore that the user of the class doesn't delete a CTPTriggerThreshold object
   *          returned by this class. (This can't be prevented, as the user has to modify the
   *          CTPTriggerThreshold objects himself.)
   *
   *     @see CTPTriggerThreshold
   *
   *  @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   *  @author Wolfgang Ehrenfeld <Wolfgang.Ehrenfeld@desy.de>
   *
   * @version \$Id: ThresholdMap.h,v 1.3 2007-06-15 16:15:59 efeld Exp $
   *
   */

  class ThresholdMap {

  public:
    //assign default pit mapping to threshold objects
    ThresholdMap( const std::vector< TrigConf::TriggerThreshold* >& threshold_vector);
    //assign correct pit mapping to threshold objects (for versions of run-I only)
    ThresholdMap( const std::vector< TrigConf::TriggerThreshold* >& threshold_vector, const std::vector< TrigConf::PIT* >& pit_vector);
		//assign correct tip mapping to threshold objects (for newer version direct inputs have to be dealt with, too)
    ThresholdMap( const std::vector< TrigConf::TriggerThreshold* >& threshold_vector, const std::vector< TrigConf::TIP* >& pit_vector );
    /// default destructor
    ~ThresholdMap();

    /// Function returning the CTPTriggerThreshold object belonging to the given TriggerThreshold object
    CTPTriggerThreshold* decision( const TrigConf::TriggerThreshold* threshold ) const;
    /// Function that clears the multiplicities in each DecisionTriggerThreshold object
    void clear();

  private:
    /// map storing the accosiation between TrigConf::TriggerThreshold and CTPTriggerThreshold
    std::map< const TrigConf::TriggerThreshold*, CTPTriggerThreshold* > m_map;
    /// message logger
    MsgLogger m_logger;

  }; // class ThresholdMap

} // namespace LVL1CTP

#endif // TRIGT1CTP_THRESHOLDMAP_H
