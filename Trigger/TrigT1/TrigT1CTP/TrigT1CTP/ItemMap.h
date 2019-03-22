/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGT1CTP_ITEMMAP_H
#define TRIGT1CTP_ITEMMAP_H

// STL include(s):
#include <vector>
#include <map>

// local include(s):
#include "TrigT1CTP/MsgLogger.h"
#include "TrigT1CTP/ISpecialTrigger.h"
#include "TrigConfL1Data/Menu.h"

// Forward include(s):
namespace CLHEP {
  class HepRandomEngine;
}
namespace TrigConf {
  class TriggerItem;
  class PrescaleSet;
}

namespace LVL1CTP {

  // forward includes in namespace LVL1CTP
  class CTPTriggerItem;
  class ThresholdMap;

  //! helper struct for sorting the keys in the ItemMap
  struct ItemMapCmp {
    bool operator()(const TrigConf::TriggerItem* t1, const TrigConf::TriggerItem* t2) const {
      return t1->ctpId() < t2->ctpId();
    }
  };

  /**
   *
   *   @short Map associating TrigConf::TriggerItem objects to CTPTriggerItem objects
   *
   *          This object can be used to create and maintain CTPTriggerItem objects for each
   *          of the TrigConf::TriggerItem ones.
   *
   *     @see TrigConf::TriggerItem
   *     @see CTPTriggerItem
   *
   *  @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   *  @author Wolfgang Ehrenfeld <Wolfgang.Ehrenfeld@desy.de>
   *
   * @version \$Id: ItemMap.h,v 1.6 2008-07-08 21:35:10 efeld Exp $
   *
   */

  class ItemMap {

  public:
    //! constructor setting list of trigger items and associated prescales (random prescale offset if randEngine is given)
    ItemMap( const TrigConf::ItemContainer& item_vector, const TrigConf::PrescaleSet& prescales,
	     CLHEP::HepRandomEngine* randEng = 0 );
    //! default destructor
    ~ItemMap();

    /// Get pointer to associated CTPTriggerItem object
    CTPTriggerItem* getItem( const TrigConf::TriggerItem* item ) const;

    //! update prescales (either by one or randomly if randEngine is given)
    void updatePrescaleCounters( const ThresholdMap* decisionMap, const InternalTriggerMap* internalTrigger, CLHEP::HepRandomEngine* randEngine = 0 );

  private:

    /// map between TrigConf::TriggerItem and CTPTriggerItem
    std::map< const TrigConf::TriggerItem*, CTPTriggerItem*, ItemMapCmp > m_map;
    /// message logger
    MsgLogger m_logger;

  }; // class ItemMap

} // namespace LVL1CTP
#endif // TRIGT1CTP_ITEMMAP_H
