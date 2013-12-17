/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_EXECLOAD_H
#define ANP_EXECLOAD_H

/**********************************************************************************
 * @Package: TrigCostExec
 * @Class  : Structures for storing algorithm execution data
 * @Author : Rustem Ospanov
 *
 * @Brief  : 
 *
 * Algorithm execution data
 *
 **********************************************************************************/

// C/C++
#include <string>

// Local
#include "TrigCostBase/Ptr.h"

// Trigger
#include "TrigMonitoringEvent/TrigMonConfig.h"
#include "TrigMonitoringEvent/TrigMonEvent.h"

namespace Anp
{
  namespace Exec 
  {
    struct ROBConf {
      ROBConf() :rob_id(0), det_id(0), ros_id(0), robin_id(0) {}
      
      uint32_t        rob_id;
      uint32_t        det_id;
      uint32_t        ros_id;
      uint32_t        robin_id;
      
      std::string     det_name;
      std::string     ros_name;
      std::string     robin_name;
    };

    typedef std::map<uint32_t, Anp::Ptr<TrigConfChain> >      ChnConfMap;
    typedef std::map<uint32_t, Anp::Ptr<TrigConfSeq> >        SeqConfMap;
    typedef std::map<uint32_t, Anp::Ptr<TrigConfAlg> >        AlgConfMap;
    typedef std::map<uint32_t, Anp::Ptr<Anp::Exec::ROBConf> > ROBConfMap;
    typedef std::map<uint32_t, std::string>                   FarmNodeMap;

    struct ROBData {
      ROBData() :det_id(0), ros_id(0), robin_id(0) {}
      
      uint32_t        det_id;
      uint32_t        ros_id;
      uint32_t        robin_id;

      Ptr<ROBConf>    rob_conf;
      TrigMonROBData  rob_data;
    };

    struct ROBCall {
      ROBCall() :rob_timer(0.0), rob_tbeg(0), rob_tend(0) {}
      
      double               rob_timer;
      unsigned             rob_tbeg;
      unsigned             rob_tend;
      std::vector<ROBData> rob_data;

      std::set<uint32_t>   det_ids;
      std::set<uint32_t>   ros_ids;
      std::set<uint32_t>   robin_ids;
    };

    struct ROILoad {
      ROILoad() : roi_valid(false), 
		  roi_id  (1000), 
		  roi_eta (0.0), 
		  roi_phi (0.0), 
		  roi_area(0.0), 
		  roi_th  (0), 
		  roi_type(TrigMonRoi::kNone) {}
      
      void SetRoi(const TrigMonRoi &roi);

      void SetRoi(const unsigned &roiId);

      void Reset();
      
      bool              roi_valid;
      unsigned          roi_id;
      float             roi_eta;
      float             roi_phi;
      float             roi_area;
      uint8_t           roi_th;

      TrigMonRoi::Type  roi_type;
      bool              isPassed;
    };

    struct AlgCall {
      AlgCall() {}

      void Print(std::ostream &os = std::cout) const;

      void FillTEs(std::vector<TrigMonTE> &TEs);

      std::string GetDetName  (uint32_t det_id) const;
      std::string GetROSName  (uint32_t ros_id) const;      
      std::string GetROBINName(uint32_t robin_id) const;      

      Ptr<TrigConfAlg>         alg_conf;
      Ptr<TrigConfSeq>         seq_conf;
      Ptr<TrigConfChain>       chn_conf;

      std::set<uint32_t>       robin_ids;
      std::set<uint32_t>       ros_ids;
      std::set<uint32_t>       det_ids;

      TrigMonAlg               alg_data;
      std::vector<ROILoad>     roi_data;
      std::vector<ROBCall>     rob_data;
    };
    
    struct Event {
      Event();

      void Reset();
      
      unsigned run;
      unsigned lb;
      unsigned event_id;
      unsigned host_id;
      unsigned host_sec;
      unsigned host_usec;
      unsigned bcid;
      unsigned sec;
      unsigned nsec;
      unsigned nRoi;

      std::string L1_time;
      std::string host;
      
      Ptr<TrigMonEvent> evt_data;

      double timer_alg;
      double timer_rob;
      float  timer_tcost;
      float  timer_steer;
    };

    //
    // Inlined functions
    //

    //-----------------------------------------------------------------------------------------
    inline void ROILoad::SetRoi(const TrigMonRoi &roi) {
      //
      // Set current ROI
      //
      const std::vector<uint16_t>&  keys = roi.getVarKey();
      const std::vector<float>&     vals = roi.getVarVal();
      
      bool valid_eta = false;
      bool valid_phi = false;

      if(keys.size() == vals.size()) {
	for(unsigned int i = 0; i < keys.size(); ++i) {
	  if(keys.at(i) == 0) { roi_eta  = vals.at(i); valid_eta = true; }
	  if(keys.at(i) == 1) { roi_phi  = vals.at(i); valid_phi = true; }
	  if(keys.at(i) == 2) { roi_area = vals.at(i); }
	}
      }

      // L1 RoI Threshold
      roi_th    = roi.getNL1th();
      roi_id    = roi.getRoiId();
      roi_valid = valid_eta && valid_phi;
      roi_type  = roi.getRoiType();
    }

    //-----------------------------------------------------------------------------------------
    inline void ROILoad::SetRoi(const unsigned &roiId) {
      //
      // Hack RoI for RoIs that are missing TrigMonRoI
      //
      roi_th    = 0;
      roi_phi   = 0.0;
      roi_eta   = 0.0;
      roi_id    = roiId;
      roi_valid = true;
      roi_type  = TrigMonRoi::kNone;
    }

    //-----------------------------------------------------------------------------------------
    inline void ROILoad::Reset() {
      roi_valid  = false;
      roi_id     = +1000;
      roi_eta    = -1000.0;
      roi_phi    = -1000.0;
    }

    //-----------------------------------------------------------------------------------------
    inline Event::Event() :
      run(0),
      lb(0),
      event_id(0),
      host_id(0),
      host_sec(0),
      host_usec(0),
      bcid(0),
      sec(0),
      nsec(0),
      nRoi(0),
      L1_time(""),
      timer_alg(0.0),
      timer_rob(0.0), 
      timer_tcost(0.0),
      timer_steer(0.0)
      {

      }
      
    //-----------------------------------------------------------------------------------------
    inline void Event::Reset() {
      run         = 0;
      lb          = 0;
      event_id    = 0;
      host_id     = 0;
      bcid        = 0;
      sec         = 0;
      nsec        = 0;
      nRoi        = 0;
      L1_time.clear();
      host.clear();
      evt_data.release();
      timer_alg   = 0.0;
      timer_rob   = 0.0; 
      timer_tcost = 0.0;
      timer_steer = 0.0;
    }
  }
}

#endif
