// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef ONLINESURVEY_H
#define ONLINESURVEY_H

#include <string>
#include <vector>

//#include "GaudiKernel/IHistogramSvc.h"

class LVL1emu;
class MdtPatRec;
class MuonFex;
class MuFeature;
class ITHistSvc;

class TH1D;
class TH2D;


class OnlineSurvey {

public:
    OnlineSurvey(ITHistSvc* histo);
    OnlineSurvey(ITHistSvc* histo,std::string name);
    OnlineSurvey();
    ~OnlineSurvey(void);

public:
    //void update(LVL1emu& LVL1EMU,
    //            MdtPatRec& MDTPATREC, 
    //            MuonFex& FEX);
    void update(MdtPatRec& MDTPATREC, 
                MuonFex& FEX);

    float innHits(void) const;
    float midHits(void) const;
    float outHits(void) const;
    const std::vector<float>& residuals(void) const;
    float eff(void) const;
    float saginv(void) const;
    float pos(void) const;
    float sagitta(void) const;
    float aPt(void) const;
    const std::vector<float>& resInn(void) const;
    const std::vector<float>& resMid(void) const;
    const std::vector<float>& resOut(void) const;

    
private:
    void initVariables();
    void voidHistograms();
    void bookHistograms(ITHistSvc *, std::string name);

    TH1D* m_InnerHitMul;
    TH1D* m_MiddleHitMul;
    TH1D* m_OuterHitMul;
    
    TH1D* m_FitPChi2;
    TH1D* m_FitRS;
    TH1D* m_FitRSlowPChi2;
    TH1D* m_FitRSmidPChi2;
    TH1D* m_FitRShigPChi2;
    TH1D* m_Sagitta;
    TH2D* m_SagVSPtLarge;
    TH2D* m_SagVSPtSmall;
    TH1D* m_Efficiency;
    
    float m_innHits;
    float m_midHits;
    float m_outHits;
    
    std::vector<float> m_res_inner;
    std::vector<float> m_res_outer;
    std::vector<float> m_res_middle;
    
    std::vector<float> m_residuals;
    float m_eff;
    float m_saginv;
    float m_pos;
    float m_sagitta;
    float m_aPt;
};

inline float OnlineSurvey::innHits() const {return m_innHits;}
inline float OnlineSurvey::midHits() const {return m_midHits;}
inline float OnlineSurvey::outHits() const {return m_outHits;}
inline const std::vector<float>& OnlineSurvey::residuals() const {return m_residuals;}
inline float OnlineSurvey::eff() const {return m_eff;}
inline float OnlineSurvey::saginv() const {return m_saginv;}
inline float OnlineSurvey::pos() const {return m_pos;}
inline float OnlineSurvey::aPt() const {return m_aPt;}
inline const std::vector<float>& OnlineSurvey::resInn() const {return m_res_inner;}
inline const std::vector<float>& OnlineSurvey::resMid() const {return m_res_middle;}
inline const std::vector<float>& OnlineSurvey::resOut() const {return m_res_outer;}
inline float OnlineSurvey::sagitta() const {return m_sagitta;}

#endif // ONLINESURVEY_H
