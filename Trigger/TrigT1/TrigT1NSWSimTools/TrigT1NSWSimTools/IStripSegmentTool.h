/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISTRIPSEGMENTTOOL_H
#define ISTRIPSEGMENTTOOL_H

//basic includes
#include "GaudiKernel/IAlgTool.h"

//local includes
#include "TrigT1NSWSimTools/StripClusterData.h"
#include <vector>
class Identfier;

// namespace for the NSW LVL1 related classes
namespace NSWL1 {
  /**
   *
   *   @short interface for the Strip Cluster Tools
   *
   * This class implements the public interface of the Strip Segmentfinding tool Tools.
   * It Current unclear what it will output to input the StripTrigger simulation.
   *
   *  @author Jacob Searcy <jsearcy@umich.edu>
   *
   *
   */


  class IStripSegmentTool: public virtual IAlgTool {

  public:
    virtual ~IStripSegmentTool() {}

    virtual StatusCode find_segments(std::vector< StripClusterData* >& clusters) = 0;

    static const InterfaceID& interfaceID() {
        static const InterfaceID IID_IStripSegmentTool("NSWL1::IStripSegmentTool", 1 ,0);
        return IID_IStripSegmentTool;
    }

  };  // end of IStripSegmentTools class

} // namespace NSWL1

#endif
