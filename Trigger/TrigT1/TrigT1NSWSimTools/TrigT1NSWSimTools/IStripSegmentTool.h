/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISTRIPSEGMENTTOOL_H
#define ISTRIPSEGMENTTOOL_H


#include "GaudiKernel/IAlgTool.h"
#include "TrigT1NSWSimTools/TriggerTypes.h"
#include "TrigT1NSWSimTools/StripClusterData.h"
#include <vector>


class Identfier;

namespace NSWL1 {
  /**
   *
   *   @short interface for the Strip Cluster Tools
   *
   * This class implements the public interface of the Strip Segmentfinding tool Tools.
   * It Current unclear what it will output to input the StripTrigger simulation.
   *
   *
   *
   */

  class IStripSegmentTool: public virtual IAlgTool {

  public:
    virtual ~IStripSegmentTool() {}

    virtual StatusCode find_segments(std::vector< std::unique_ptr<StripClusterData> >& clusters) = 0;
  /*
    virtual std::vector<int> *seg_wedge1_size()=0;                     
    virtual std::vector<int> *seg_wedge2_size()=0;                       
    virtual std::vector<float> *seg_theta()=0;
    virtual std::vector<float> *seg_dtheta()=0;                   
    virtual std::vector<float> *seg_eta()=0;                         
    virtual std::vector<float> *seg_phi()=0;
    virtual std::vector<int> *seg_bandId()=0;
    virtual std::vector<float> *seg_global_r()=0;
    virtual std::vector<float> *seg_global_x()=0;
    virtual std::vector<float> *seg_global_y()=0;
    virtual std::vector<float> *seg_global_z()=0;
    virtual std::vector<float> *seg_dir_r()=0;
    virtual std::vector<float> *seg_dir_y()=0;
    virtual std::vector<float> *seg_dir_z()=0;
    */
    
    static const InterfaceID& interfaceID() {
        static const InterfaceID IID_IStripSegmentTool("NSWL1::IStripSegmentTool", 1 ,0);
        return IID_IStripSegmentTool;
    }

  };

}

#endif
