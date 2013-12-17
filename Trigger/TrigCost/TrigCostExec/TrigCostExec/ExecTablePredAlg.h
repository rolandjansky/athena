/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_EXECTABLEPREDALG_H
#define ANP_EXECTABLEPREDALG_H

/**********************************************************************************
 * @Package: TrigCostExec
 * @Class  : ExecTablePredAlg
 * @Author : Doug Schaefer
 *
 * @Brief  : 
 *
 * Timing plots for ROBs and ROSes
 *  - scale ROS rates to new luminosity rates using enhanced bias data
 *  - cpu and ROB request timing per call and per event
 *  - plot separately cached and retreived ROB requests
 *  - additional plots
 *
 **********************************************************************************/

// C/C++
#include <set>
#include <map>
#include <string>
#include <vector>

// Trigger
#include "TrigMonitoringEvent/TrigMonConfig.h"
#include "TrigMonitoringEvent/TrigMonEvent.h"

// Local
#include "TrigCostBase/Handle.h"
#include "TrigCostBase/Registry.h"
#include "TrigCostBase/HtmlNode.h"
#include "TrigCostBase/LumiABC.h"
#include "TrigCostBase/ScaleABC.h"

// Local
#include "TrigCostExec/ExecBase.h"

class TCanvas;

namespace Anp
{
  class ExecTablePredAlg: public virtual ExecClient {
  public:

    ExecTablePredAlg();
    virtual ~ExecTablePredAlg();

  public:
    struct pCount {
      pCount();

      void Clear();
      void AddCount(const Anp::Exec::Count &rhs, const double &scale);
      void AddCount(const Anp::Exec::Event &rhs, const double &scale);

      double GetMeanROBTimePerRetr()  const;
      double GetMeanROBTimePerEvent() const;           

    public:

      // Properties:
      uint32_t alg_id;
      uint32_t det_id;
      uint32_t ros_id;
      uint32_t roi_id;
      uint32_t chn_id;
      
      std::string ros_name;

      bool select_det;
      bool select_ros;
      bool select_roi;

      double   limit_slow;

      // Variables:
      uint32_t      nevent;             // Number of processed events
      long double   nevent_pred;        // Number of processed events predicted
      long double   nevent_pred_err;    // Number of processed events predicted squared for error procedure
      uint32_t      nroi;               // Number of rois

      // Variables cleared per event:
      long double timer_alg;            // Total algorithm timer (includes ROB request time)
      long double timer_rob;            // Time for ROB request(s) only
     
      long double alg_ncalls;
      long double alg_ncache;
      long double alg_nslow;

      long double rob_nreq;
      long double rob_nreq_cache;
      long double rob_nreq_retrv;
      long double rob_nreq_other;

      long double rob_size;           // Size in words
      long double rob_size_cache;     // Size in words
      long double rob_size_retrv;     // Size in words
      long double rob_size_other;     // Size in words

      long double rob_nids;
      long double rob_nids_cache;
      long double rob_nids_retrv;
      long double rob_nids_other;

      long double ros_nreq;
      long double ros_nreq_cache;
      long double ros_nreq_retrv;
      long double ros_nreq_other;

      long double ros_nreq_err;
      long double ros_nreq_retrv_err;
      long double ros_size_retrv_err;
    };
    
  private:

    typedef std::map<uint32_t, Handle<ExecClient> > ALGMap;
    typedef std::map<uint32_t, Exec::Count>         CountMap;
    typedef std::vector<ExecClient *>               ClientVec;
    typedef std::vector<pCount>                     CountVec;

  public:
    struct rCount{
      rCount();

      pCount alg;
      CountVec roses;
    };

  private:
    typedef std::vector<rCount>                     rCountVec;
    typedef std::map<uint32_t,rCount>               ALGCountMap;

    void SetReg(const Registry &reg);
    
    void SetDir(TDirectory *dir);
    
    void ProcessAlg(const Exec::AlgCall &alg);
    
    void FinishEvent(const Exec::Event &event);
    
    const Exec::Count& GetCount() const;
    
    void WriteExec (HtmlNode &anode, HtmlMenu &,         const Exec::Summary &sum_init);
    void WriteTable(HtmlNode &anode,const pCount &count, const std::string& ros_link, const Exec::Summary &sum);
    void WriteTableAlg(HtmlNode &anode,const pCount &count, const std::string& ros_link, const Exec::Summary &sum,const long double &tot_alg_time);

  private:
    
    ExecClient* GetALGClient(const uint32_t alg_id, const Exec::AlgCall &alg);
    rCount*     GetALGCount( const uint32_t alg_id);
    pCount*     GetROSCount( const unsigned alg_id, CountVec& algs);

    TH1* MakeHist(const std::string &hname, const std::string &yaxis, 
		  unsigned min_lb, unsigned max_lb, TDirectory *dir);
    
    std::ostream& log() const;

  private:

    TDirectory                *fDir;
    TCanvas                   *fCanvas;
    Registry                   fReg;

    // Properties:
    bool                       fDebug;
    bool                       fFillNode;
    bool                       fMakeHist;
    bool                       fPlots;
    double                     fTimeout;

    std::string                fDirName;
    std::string                fHistOpt;
    
    // Variables:
    Exec::Count                fCount;

    Handle<ScaleABC>           fScaleBG;        // BG scaling algorithm

    ALGMap                     fALG;
    ALGCountMap                fALGCount;
    ClientVec                  fActiveClients;

  };

  //
  // Sorting functions
  //
  struct SortpCount {

    SortpCount(const std::string &key = "") :fKey(key) {}

    bool operator()(const ExecTablePredAlg::pCount &lhs, const ExecTablePredAlg::pCount &rhs) const;
    
    public:

    std::string fKey;
  };

  struct SortrCount {

    SortrCount(const std::string &key = "") :fKey(key) {}

    bool operator()(const ExecTablePredAlg::rCount &lhs, const ExecTablePredAlg::rCount &rhs) const;
    
    public:

    std::string fKey;
  };


}

#endif
