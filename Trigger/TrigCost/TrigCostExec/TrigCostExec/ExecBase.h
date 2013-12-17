/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_EXECBASE_H
#define ANP_EXECBASE_H

/**********************************************************************************
 * @Package: TrigCostExec
 * @Class  : ExecBase
 * @Author : Rustem Ospanov
 *
 * @Brief  : 
 *
 * Base class for analysis of algorithm execution data
 *
 **********************************************************************************/

// C/C++
#include <map>
#include <string>

// Local
#include "TrigCostBase/Handle.h"
#include "TrigCostBase/HtmlABC.h"
#include "TrigCostBase/Registry.h"

// Local
#include "TrigCostExec/ExecData.h"

class TDirectory;
class TH1;
class TH2;

namespace Anp
{
  class ExecClient;

  namespace Exec {

    //
    // Struct for analysis algorithm call and ROB requests
    //
    struct Count {
      Count();

      void Clear();
      void AddCount(const Count &rhs);

      void AddCount(const AlgCall &alg);
      void AddCount(const Event &event);

      double GetMeanAlgTimePerCall() const;
      double GetMeanCPUTimePerCall() const;
      double GetMeanROBTimePerRetr()  const;

      double GetMeanAlgTimePerEvent() const;
      double GetMeanStrTimePerEvent() const;
      double GetMeanCPUTimePerEvent() const;
      double GetMeanROBTimePerEvent() const;           
      double GetMeanNumRoIsPerEvent() const;           
      
    private:

      void AddCount(const ROBCall &rob);

    public:

      // Properties:
      uint32_t alg_id;
      uint32_t det_id;
      uint32_t ros_id;
      uint32_t robin_id;
      uint32_t roi_id;
      uint32_t chn_id;
      uint32_t seq_id;
      
      bool select_det;
      bool select_ros;
      bool select_robin;
      bool select_roi;

      double   limit_slow;

      // Variables:
      uint32_t nevent;             // Number of processed events
      uint32_t nroi;               // Number of rois

      std::set<uint32_t>    ros_all,  ros_cache,      ros_retrv,   ros_other, rob_all, rob_retrv;
      std::set<uint32_t>    robin_all,robin_cache,    robin_retrv, robin_other;
      std::vector<uint32_t> ros_nrob, ros_nrob_retrv, robin_nrob,  robin_nrob_retrv;

      // Variables cleared per event:
      double   timer_alg;          // Total algorithm timer (includes ROB request time)
      double   timer_rob;          // Time for ROB request(s) only
      double   timer_evt;          // Alg Event Time for only one event
      double   timer_evt_t;        // steering Event Time for all events
     
      uint64_t alg_ncalls;
      uint64_t alg_ncache;
      uint64_t alg_nslow;

      uint64_t chn_nslow;
      uint64_t seq_nslow;

      uint64_t rob_nreq;
      uint64_t rob_nreq_cache;
      uint64_t rob_nreq_retrv;
      uint64_t rob_nreq_other;

      uint64_t rob_size;           // Size in words
      uint64_t rob_size_cache;     // Size in words
      uint64_t rob_size_retrv;     // Size in words
      uint64_t rob_size_other;     // Size in words

      uint64_t rob_nids;
      uint64_t rob_nids_cache;
      uint64_t rob_nids_retrv;
      uint64_t rob_nids_other;

      uint64_t ros_nreq;
      uint64_t ros_nreq_cache;
      uint64_t ros_nreq_retrv;
      uint64_t ros_n0ret;
      uint64_t ros_nreq_other;

      uint64_t robin_nreq;
      uint64_t robin_nreq_cache;
      uint64_t robin_nreq_retrv;
      uint64_t robin_n0ret;
      uint64_t robin_nreq_other;
    };

    double ConvertWordsToMB(unsigned nwords);
    double ConvertWordsToKB(unsigned nwords);
    double ConvertTimerToSec(const TrigMonTimer &timer);
    std::string ConvertHistToString(const TrigMonROBData &rob);
    std::string ConvertSeqToStatus(const TrigMonSeq &seq);

    //
    // Summary for single LB and single ExecClient instance
    //    
    struct SummaryLB {
      SummaryLB() :run(0), 
	lb(0), 
	scale_corr(0), 
	total_time(0),
	lb_time(0),
	L1Count(0),
	L2Count(0),
	lb_start("") {}
      
      unsigned run;
      unsigned lb;

      double   scale_corr;
      double   total_time;
      double   lb_time;

      unsigned L1Count;
      unsigned L2Count;

      std::string lb_start;
      
      Handle<ExecClient>  client;
    };

    //
    // Struct for summary information of entire processing
    //
    struct Summary {

      Summary() :nevent(0), scale_corr(0), wall_time(0.0), total_time(0), write_full(true), 
		 alg_conf(0), chn_conf(0), seq_conf(0), rob_conf(0), farm_node(0) {}

      std::string GetChnName(uint32_t chn_id) const;
      std::string GetSeqName(uint32_t seq_id) const;
      std::string GetAlgType(uint32_t alg_id) const;
      std::string GetAlgName(uint32_t alg_id) const;
      std::string GetDetName(uint32_t det_id) const;
      std::string GetROSName(uint32_t ros_id) const;
      std::string GetROBINName(uint32_t robin_id) const;
      std::string GetNodeName (uint32_t node_id) const;
      
      void ClearMenuConfig();

      uint64_t    nevent;
      double      scale_corr; // scale factor to convert counts to Hz
      double      wall_time;  // time to collect these events
      double      total_time; // total HLT execution time
      bool        write_full; // write detailed tables

      std::string html_base;
      
      AlgConfMap  *alg_conf;
      ChnConfMap  *chn_conf;
      SeqConfMap  *seq_conf;
      ROBConfMap  *rob_conf;
      FarmNodeMap *farm_node;

      std::vector<SummaryLB> sum_lbs;
    };

    //
    // Standard histogram structures
    //
    struct AlgHist {
      AlgHist();

      TH1 *hAlgTime;
      TH1 *hAlgTimeFrac;
      TH1 *hCpuTime;
      TH1 *hROBTime; 
      TH1 *hAlgTimeFirstCall;

      TH1 *hAlgNCalls;
      TH1 *hAlgNCache;      
    };

    struct ROBHist {
      ROBHist();

      TH1 *hROBNreq;       // Number of ROB data requests
      TH1 *hROBNreqCache;  // Number of ROB data requests - fully cached
      TH1 *hROBNreqRetrv;  // Number of ROB data requests - data retrieved

      TH1 *hROBSize;       // ROB request size - all
      TH1 *hROBSizeCache;  // ROB request size - cached 
      TH1 *hROBSizeRetrv;  // ROB request size - retrieved

      TH1 *hROBNids;       // # ROBs per data request - all
      TH1 *hROBNidsCache;  // # ROBs per data request - cached 
      TH1 *hROBNidsRetrv;  // # ROBs per data request - retrieved
    };

    struct ROSHist {
      ROSHist();

      TH1 *hROSNreq;       // Number of ROS requests
      TH1 *hROSNreqCache;  // Number of ROS requests - fully cached
      TH1 *hROSNreqRetrv;  // Number of ROS requests - data retrieved

      TH1 *hROSSize;       // ROS request size - all
      TH1 *hROSSizeCache;  // ROS request size - cached 
      TH1 *hROSSizeRetrv;  // ROS request size - retrieved

      TH1 *hROSNrobids;       // ROS request rate vs # ROBs in request - all
      TH1 *hROSNrobidsCache;  // ROS request rate vs # ROBs in request - cached 
      TH1 *hROSNrobidsRetrv;  // ROS request rate vs # ROBs in request - retrieved
    };

    struct FarmHist{
      FarmHist();
      
      TH1 *hUseVsLBSteer;     // % use per lumi block - steering estimate
      TH1 *hUseVsLBAlg;       // % use per lumi block - alg estimate
    };
  }

  //
  // Base class for processing cost data algorithm
  //
  class ExecClient: public Base {
  public:
    
  ExecClient() :fLevel(0), fPassLB(false), fIndexPage(false), fShortClient(false) {}
    virtual ~ExecClient() {}
    
    //
    // Interface functions
    //
    virtual void SetReg(const Registry &) {}
    
    virtual void SetDir(TDirectory *) {}
    
    virtual void ProcessAlg(const Exec::AlgCall &) = 0;
    
    virtual void FinishEvent(const Exec::Event &) = 0;
    
    virtual const Exec::Count& GetCount() const = 0;
    
    virtual void WriteExec(HtmlNode &, HtmlMenu &, const Exec::Summary &) {}

    virtual void ClearCount() {}

    //
    // Base class functions
    //
    const std::map<uint32_t, Exec::Count>& GetAlgCount() const { return fCountAlg; }
    const std::map<uint32_t, Exec::Count>& GetDetCount() const { return fCountDet; }
    const std::map<uint32_t, Exec::Count>& GetGrpCount() const { return fCountGrp; }
    const std::map<uint32_t, Exec::Count>& GetROSCount() const { return fCountROS; }
    const std::map<uint32_t, Exec::Count>& GetROBINCount() const { return fCountROBIN; }
    const std::map<uint32_t, Exec::Count>& GetChnCount() const { return fCountChn; }
    const std::map<uint32_t, Exec::Count>& GetSeqCount() const { return fCountSeq; }
    
    Exec::AlgHist GetAlgHistCalls() const { return hAlgCalls; }
    Exec::AlgHist GetAlgHistEvent() const { return hAlgEvent; }

    Exec::ROBHist GetROBHistCalls() const { return hROBCalls; }
    Exec::ROBHist GetROBHistEvent() const { return hROBEvent; }

    Exec::ROSHist GetROSHistCalls() const { return hROSCalls; }
    Exec::ROSHist GetROSHistEvent() const { return hROSEvent; }
    Exec::ROSHist GetROSHistROI()   const { return hROSRoi; }

    void SetLevel(unsigned level) { fLevel = level; }

    unsigned    GetLevel()    const { return fLevel; }
    std::string GetLevelStr() const;    

    void SetClientLink(const std::string &link) { fClientLink = link; }
    void SetClientName(const std::string &name) { fClientName = name; }
    void SetClientType(const std::string &type) { fClientType = type; }
    void SetClientPass(const bool        &pass) { fPassLB     = pass; }
    void SetClientIPag(const bool        &node) { fIndexPage  = node; }
    
    const std::string& GetClientLink() const { return fClientLink; }
    const std::string& GetClientName() const { return fClientName; }
    const std::string& GetClientType() const { return fClientType; }

    const bool       & GetClientPass() const { return fPassLB;     }
    const bool       & GetClientIPag() const { return fIndexPage;  }
    const bool       & IsShortClient() const { return fShortClient;}

  protected:
    
    typedef std::map<uint32_t, Exec::Count> CountMap;
    
    CountMap                   fCountAlg;
    CountMap                   fCountDet;
    CountMap                   fCountGrp;
    CountMap                   fCountROS;
    CountMap                   fCountROBIN;
    CountMap                   fCountRoi;
    CountMap                   fCountChn;
    CountMap                   fCountSeq;

    unsigned                   fLevel;

    bool                       fPassLB;
    bool                       fIndexPage;
    bool                       fShortClient;

    std::string                fClientLink;
    std::string                fClientName;
    std::string                fClientType;
    
    Exec::AlgHist              hAlgEvent;
    Exec::AlgHist              hAlgCalls;

    Exec::ROBHist              hROBEvent;
    Exec::ROBHist              hROBCalls;

    Exec::ROSHist              hROSEvent;
    Exec::ROSHist              hROSCalls;
    Exec::ROSHist              hROSRoi;
  }; 

  //
  // Sorting functions
  //
  struct SortExecCount {

    SortExecCount(const std::string &key = "") :fKey(key) {}

    bool operator()(const Exec::Count &lhs, const Exec::Count &rhs) const;
    
    public:

    std::string fKey;
  };

  struct SortExecClient {

    SortExecClient(const std::string &key = "") :fSort(key) {}

    bool operator()(const ExecClient *lhs, const ExecClient *rhs) const;
    
    private:

    SortExecCount fSort;
  };

  struct SortAlgCall {

    SortAlgCall(const std::string &key = "") :fKey(key) {}

    bool operator()(const Exec::AlgCall &lhs, const Exec::AlgCall &rhs) const;

    bool operator()(const Ptr<Exec::AlgCall> &lhs, const Ptr<Exec::AlgCall> &rhs) const;
    
    public:

    std::string fKey;
  };
}

#endif
