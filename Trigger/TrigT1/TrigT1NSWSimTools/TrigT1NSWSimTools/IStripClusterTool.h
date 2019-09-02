/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISTRIPCLUSTERTOOL_H
#define ISTRIPCLUSTERTOOL_H

//basic includes
#include "GaudiKernel/IAlgTool.h"
//local includes
#include "TrigT1NSWSimTools/TriggerTypes.h"
#include "TrigT1NSWSimTools/StripData.h"
#include "TrigT1NSWSimTools/StripClusterData.h"
#include <vector>

class Identfier;

// namespace for the NSW LVL1 related classes
namespace NSWL1 {
  class PadTrigger;

  /**
   *
   *   @short interface for the Strip Cluster Tools
   *
   * This class implements the public interface of the Strip clustering Tools.
   * It Current unclear what it will output to input the StripTrigger simulation.
   *
   *
   *
   */


  class IStripClusterTool: public virtual IAlgTool {

  public:
    virtual ~IStripClusterTool() {}

    virtual StatusCode cluster_strip_data( std::vector<std::unique_ptr<StripData>>& strips,std::vector<std::unique_ptr<StripClusterData> >& clusters) = 0;

    static const InterfaceID& interfaceID() {
        static const InterfaceID IID_IStripClusterTool("NSWL1::IStripClusterTool", 1 ,0);
        return IID_IStripClusterTool;
    }

  };  // end of IStripClusterTools class

} // namespace NSWL1

#endif
