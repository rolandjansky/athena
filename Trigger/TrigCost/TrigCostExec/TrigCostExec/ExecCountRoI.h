/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_EXECCOUNTROI_H
#define ANP_EXECCOUNTROI_H

/**********************************************************************************
 * @Package: TrigCostExec
 * @Class  : ExecCountRoI
 * @Author : Doug Schaefer
 *
 * @Brief  : 
 *
 * Calculates requests per ROI
 *  - find the requests by chain, alg and ros
 *  - plot separately cached and retreived ROB requests
 *
 **********************************************************************************/

// Local
#include "TrigCostExec/ExecBase.h"

namespace Anp
{
  class ExecCountRoI: public virtual ExecClient {
  public:

    ExecCountRoI();
    virtual ~ExecCountRoI();

    void SetReg(const Registry &reg);
    
    void SetDir(TDirectory *dir);
    
    void ProcessAlg(const Exec::AlgCall &alg);
    
    void FinishEvent(const Exec::Event &event);
    
    const Exec::Count& GetCount() const;

    void ClearCount();

  private:

    Exec::Count& GetAlgCount(uint32_t alg_id);
    Exec::Count& GetDetCount(uint32_t det_id);
    Exec::Count& GetROSCount(uint32_t ros_id);
    Exec::Count& GetRoiCount(uint32_t roi_id);

    TH1* GetTH1(const std::string &key, const std::string &name = "") const;

  private:

    TDirectory                *fDir;    

    bool                       fDebug;
    bool                       fDoAlg;
    bool                       fDoDet;
    bool                       fDoROS;
    bool                       fDoChn;
    bool                       fMakeHist;
    
    uint32_t                   fAlgId;
    uint32_t                   fDetId;
    uint32_t                   fROSId;
    uint32_t                   fChnId;
    
    bool                       fSelectAlg;
    bool                       fSelectDet;
    bool                       fSelectROS;
    bool                       fSelectChn;

    double                     fTimeout;
    std::string                fHistKey;

    Exec::Count                fCountRoiEvt;
    Exec::Count                fCountRoiTot;
    Exec::Count                fCountTotal;
  };
}

#endif
