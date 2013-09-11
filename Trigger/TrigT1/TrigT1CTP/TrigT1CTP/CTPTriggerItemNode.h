/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGT1CTP_CTPTRIGGERITEMNODE_H
#define TRIGT1CTP_CTPTRIGGERITEMNODE_H

#include "TrigT1CTP/ISpecialTrigger.h"

namespace TrigConf {
  class TriggerItemNode;
}

namespace LVL1CTP {

  class ThresholdMap;

  /**
   *   @short Class making decision about a whole trigger item
   *
   *          The decision about a single trigger item can be made with one recursive
   *          function, but for future flexibility it seemed wise to create a class around
   *          that function...
   *
   *  @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   *  @author Wolfgang Ehrenfeld <Wolfgang.Ehrenfeld@desy.de>
   *
   * @version \$Id: CTPTriggerItemNode.h,v 1.4 2007-06-15 16:15:59 efeld Exp $
   *
   */
  
  class CTPTriggerItemNode {

  public:
    /// Make decision about trigger item
    static bool evaluate( const TrigConf::TriggerItemNode* node, const ThresholdMap* map, const InternalTriggerMap* internalTrigger );

  }; // class CTPTriggerItemNode

} // namespace LVL1CTP

#endif // TRIGT1CTP_CTPTRIGGERITEMNODE_H
