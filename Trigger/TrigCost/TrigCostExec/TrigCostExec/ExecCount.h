/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_EXECCOUNT_H
#define ANP_EXECCOUNT_H

/**********************************************************************************
 * @Package: TrigCostExec
 * @Class  : ExecCount
 * @Author : Rustem Ospanov
 *
 * @Brief  : 
 *
 * Timing plots for algorithms
 *  - cpu and ROB request timing per call and per event
 *  - plot separately cached and retreived ROB requests
 *  - additional plots
 *
 **********************************************************************************/

// Local
#include "TrigCostExec/ExecBase.h"

namespace Anp
{
  class ExecCount: public virtual ExecClient {
  public:

    ExecCount();
    virtual ~ExecCount();

    void SetReg(const Registry &reg);
    
    void SetDir(TDirectory *dir);
    
    void ProcessAlg(const Exec::AlgCall &alg);
    
    void FinishEvent(const Exec::Event &event);
    
    const Exec::Count& GetCount() const;

    void ClearCount();

  private:

    Exec::Count& GetAlgCount(uint32_t alg_id);
    Exec::Count& GetGrpCount(uint32_t det_id);
    Exec::Count& GetDetCount(uint32_t det_id);
    Exec::Count& GetROSCount(uint32_t ros_id);
    Exec::Count& GetRoiCount(uint32_t roi_id);
    Exec::Count& GetChnCount(uint32_t chn_id);
    Exec::Count& GetSeqCount(uint32_t seq_id);
    Exec::Count& GetROBINCount(uint32_t robin_id);

    typedef std::vector<Exec::Count*> VecCount;

    TH1* GetTH1(const std::string &key,
		const std::string &xaxis = "",
		const std::string &yaxis = "") const;

  private:

    TDirectory                *fDir;    

    bool                       fDebug;
    bool                       fDoAlg;
    bool                       fDoDet;
    bool                       fDoROS;
    bool                       fDoROBIN;
    bool                       fDoRoi;
    bool                       fDoChn;
    bool                       fDoSeq;
    bool                       fMakeHist;
    bool                       fMakeAlgHist;
    
    uint32_t                   fAlgId;
    uint32_t                   fDetId;
    uint32_t                   fROSId;
    uint32_t                   fROBINId;
    uint32_t                   fChnId;
    uint32_t                   fSeqId;
    
    bool                       fSelectAlg;
    bool                       fSelectDet;
    bool                       fSelectROS;
    bool                       fSelectROBIN;
    bool                       fSelectRoi;
    bool                       fSelectChn;
    bool                       fSelectSeq;

    double                     fTimeout;
    std::string                fHistKey;

    Exec::Count                fCountRoiEvt;
    Exec::Count                fCountRoiTot;
    Exec::Count                fCountCalls;
    Exec::Count                fCountEvent;
    Exec::Count                fCountTotal;

    VecCount                   fCountChnVec;
    VecCount                   fCountSeqVec;
    VecCount                   fCountDetVec;
  };
}

#endif
