// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// InDetTrackCut.h
// Declarations of objects to perform cuts for InDetTrackSelectionTool

#ifndef INDETTRACKSELECTIONTOOL_INDETTRACKCUT_H
#define INDETTRACKSELECTIONTOOL_INDETTRACKCUT_H

#include "InDetTrackSelectionTool/InDetTrackAccessor.h"

#include <map>

namespace InDet {

  class InDetTrackSelectionTool;

  // ---------------- TrackCut ----------------

  class TrackCut : public asg::AsgMessaging {
    // abstract interface class for all cuts.
  public:
    TrackCut(InDetTrackSelectionTool*);
    virtual ~TrackCut() = 0;
    // initialize should find the accessor(s) or create them if they don't exist
    virtual StatusCode initialize();
    virtual bool result() const = 0; // whether the cut passes
  protected:
    // a function that gets the accessor labelled by "name", adding it if it needs to.
    template <class AccessorType>
    StatusCode getAccessor(const std::string& name, std::shared_ptr<AccessorType>& accessor);
  private:
    // a pointer to the tool's accessors so we can modify it in initialize()
    std::unordered_map< std::string, std::shared_ptr<TrackAccessor> >* m_trackAccessors;
    // a pointer to the tool so that we can create the accessors when need be, as they inherit from AsgMessaging and need a pointer to the tool
    InDetTrackSelectionTool* m_selectionTool;
  }; // class TrackCut

  // ---------------- MaxSummaryValueCut ----------------
  class MaxSummaryValueCut : public virtual TrackCut {
  public:
    MaxSummaryValueCut(InDetTrackSelectionTool*);
    void setMaxValue(Int_t max) {m_maxValue = max;}
    void addSummaryType(xAOD::SummaryType type) {m_summaryTypes.push_back(type);}
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    Int_t m_maxValue;
    // will check the sum of all its accessors and compare to the max allowed value
    std::vector< xAOD::SummaryType > m_summaryTypes;
    std::vector< std::shared_ptr<SummaryAccessor> > m_summaryAccessors;
  }; // class MaxSummaryValueCut

  // ---------------- MinSummaryValueCut ----------------

  class MinSummaryValueCut : public virtual TrackCut {
  public:
    MinSummaryValueCut(InDetTrackSelectionTool*);
    void setMinValue(Int_t min) {m_minValue = min;}
    void addSummaryType(xAOD::SummaryType type) {m_summaryTypes.push_back(type);}
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    Int_t m_minValue;
    std::vector< xAOD::SummaryType > m_summaryTypes;
    // will check the sum of all its accessors and compare to the max allowed value
    std::vector< std::shared_ptr<SummaryAccessor> > m_summaryAccessors;
  }; // class MinSummaryValueCut

  // ---------------- MaxSummaryValueRatioCut ----------------
  class MaxSummaryValueRatioCut : public virtual TrackCut {
  public:
    MaxSummaryValueRatioCut(InDetTrackSelectionTool*);
    void setMaxValue(Int_t max) {m_maxValue = max;}
    void addSummaryTypeNumerator(xAOD::SummaryType type) {m_summaryTypesNum.push_back(type);}
    void addSummaryTypeDenominator(xAOD::SummaryType type) {m_summaryTypesDen.push_back(type);}
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    Double_t m_maxValue;
    // will check the sum of all its accessors and compare to the max allowed value
    std::vector< xAOD::SummaryType > m_summaryTypesNum;
    std::vector< xAOD::SummaryType > m_summaryTypesDen;
    std::vector< std::shared_ptr<SummaryAccessor> > m_summaryAccessorsNum;
    std::vector< std::shared_ptr<SummaryAccessor> > m_summaryAccessorsDen;
  }; // class MaxSummaryValueCut

  // // ---------------- MaxTrtHitCut ----------------
  // class MaxTrtHitCut : public virtual MaxSummaryValueCut {
  // public: 
  //   MaxTrtHitCut(InDetTrackSelectionTool*);
  //   void setMaxEtaAcceptance(Double_t eta) {m_maxTrtEtaAcceptance = eta;}
  //   virtual StatusCode initialize();
  //   virtual bool result() const;
  // private:
  //   Double_t m_maxTrtEtaAcceptance;
  //   std::shared_ptr<EtaAccessor> m_etaAccessor;
  // }; // class MaxTrtHitCut

  // ---------------- MinTrtHitCut ----------------
  class MinTrtHitCut : public virtual MinSummaryValueCut {
  public: 
    MinTrtHitCut(InDetTrackSelectionTool*);
    void setMaxEtaAcceptance(Double_t eta) {m_maxTrtEtaAcceptance = eta;}
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    Double_t m_maxTrtEtaAcceptance;
    std::shared_ptr<EtaAccessor> m_etaAccessor;
  }; // class MinTrtHitCut

  // ---------------- MaxTrtHitRatioCut ----------------
  class MaxTrtHitRatioCut : public virtual MaxSummaryValueRatioCut {
  public: 
    MaxTrtHitRatioCut(InDetTrackSelectionTool*);
    void setMaxEtaAcceptance(Double_t eta) {m_maxTrtEtaAcceptance = eta;}
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    Double_t m_maxTrtEtaAcceptance;
    std::shared_ptr<EtaAccessor> m_etaAccessor;
  }; // class MaxTrtHitRatioCut

  // ---------------- D0Cut ----------------
  class D0Cut : public virtual TrackCut {
  public:
    D0Cut(InDetTrackSelectionTool*);
    void setMaxValue(Double_t max) {m_maxValue = max;}
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    Double_t m_maxValue;
    std::shared_ptr<ParamAccessor> m_paramAccessor;
  }; // class D0Cut

  // ---------------- D0SigmaCut ----------------
  class D0SigmaCut : public virtual TrackCut {
  public:
    D0SigmaCut(InDetTrackSelectionTool*);
    void setMaxValue(Double_t max) {m_maxValue = max*max;} // cut is on variance
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    Double_t m_maxValue;
    std::shared_ptr<ParamCovAccessor> m_paramCovAccessor;
  }; // class D0SigmaCut

  // ---------------- D0SignificanceCut ----------------
  class D0SignificanceCut : public virtual TrackCut {
  public:
    D0SignificanceCut(InDetTrackSelectionTool*);
    void setMaxValue(Double_t max) {m_maxValue = max*max;}
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    Double_t m_maxValue;
    std::shared_ptr<ParamAccessor> m_paramAccessor;
    std::shared_ptr<ParamCovAccessor> m_paramCovAccessor;
  }; // class D0SignificanceCut


  // ---------------- Z0Cut ----------------
  class Z0Cut : public virtual TrackCut {
  public:
    Z0Cut(InDetTrackSelectionTool*);
    void setMaxValue(Double_t max) {m_maxValue = max;}
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    Double_t m_maxValue;
    std::shared_ptr<ParamAccessor> m_paramAccessor;
  }; // class Z0Cut

  // ---------------- Z0SigmaCut ----------------
  class Z0SigmaCut : public virtual TrackCut {
  public:
    Z0SigmaCut(InDetTrackSelectionTool*);
    void setMaxValue(Double_t max) {m_maxValue = max*max;} // cut is on variance
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    Double_t m_maxValue;
    std::shared_ptr<ParamCovAccessor> m_paramCovAccessor;
  }; // class Z0SigmaCut

  // ---------------- Z0SignificanceCut ----------------
  class Z0SignificanceCut : public virtual TrackCut {
  public:
    Z0SignificanceCut(InDetTrackSelectionTool*);
    void setMaxValue(Double_t max) {m_maxValue = max*max;}
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    Double_t m_maxValue;
    std::shared_ptr<ParamAccessor> m_paramAccessor;
    std::shared_ptr<ParamCovAccessor> m_paramCovAccessor;
  }; // class Z0SignificanceCut


  // ---------------- Z0SinThetaCut ----------------
  class Z0SinThetaCut : public virtual TrackCut {
  public:
    Z0SinThetaCut(InDetTrackSelectionTool*);
    void setMaxValue(Double_t max) {m_maxValue = max;}
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    Double_t m_maxValue;
    std::shared_ptr<ParamAccessor> m_Z0Accessor;
    std::shared_ptr<ParamAccessor> m_ThetaAccessor;
  }; // class Z0SinThetaCut

  // ---------------- Z0SinThetaSigmaCut ----------------
  class Z0SinThetaSigmaCut : public virtual TrackCut {
  public:
    Z0SinThetaSigmaCut(InDetTrackSelectionTool*);
    void setMaxValue(Double_t max) {m_maxValue = max*max;} // cut is on variance
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    Double_t m_maxValue;
    std::shared_ptr<ParamAccessor> m_Z0Accessor;
    std::shared_ptr<ParamCovAccessor> m_Z0VarAccessor;
    std::shared_ptr<ParamAccessor> m_ThetaAccessor;
    std::shared_ptr<ParamCovAccessor> m_ThetaVarAccessor;
    std::shared_ptr<ParamCovAccessor> m_CovAccessor;
  }; // class Z0SinThetaSigmaCut

  // ---------------- Z0SinThetaSignificanceCut ----------------
  class Z0SinThetaSignificanceCut : public virtual TrackCut {
  public:
    Z0SinThetaSignificanceCut(InDetTrackSelectionTool*);
    void setMaxValue(Double_t max) {m_maxValue = max*max;} // cut is on variance
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    Double_t m_maxValue;
    std::shared_ptr<ParamAccessor> m_Z0Accessor;
    std::shared_ptr<ParamCovAccessor> m_Z0VarAccessor;
    std::shared_ptr<ParamAccessor> m_ThetaAccessor;
    std::shared_ptr<ParamCovAccessor> m_ThetaVarAccessor;
    std::shared_ptr<ParamCovAccessor> m_CovAccessor;
  }; // class Z0SinThetaSignificanceCut

  // ---------------- PtCut ----------------
  class PtCut : public virtual TrackCut {
  public:
    PtCut(InDetTrackSelectionTool*);
    void setMinValue(Double_t min) {m_minValue = min;}
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    Double_t m_minValue;
    std::shared_ptr<PtAccessor> m_accessor;
  };

  // ---------------- EtaCut ----------------
  class EtaCut : public virtual TrackCut {
  public:
    EtaCut(InDetTrackSelectionTool*);
    void setMaxValue(Double_t max) {m_maxValue = max;}
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    Double_t m_maxValue;
    std::shared_ptr<EtaAccessor> m_accessor;
  };

  // ---------------- PCut ----------------
  class PCut : public virtual TrackCut {
  public:
    PCut(InDetTrackSelectionTool*);
    void setMinValue(Double_t min) {m_minValue = min;}
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    Double_t m_minValue;
    std::shared_ptr<PAccessor> m_accessor;
  };

  // ---------------- EtaDependentChiSqCut ----------------
  class EtaDependentChiSqCut : public virtual TrackCut {
  public:
    EtaDependentChiSqCut(InDetTrackSelectionTool*);
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    std::shared_ptr<EtaAccessor> m_etaAccessor;
    std::shared_ptr<FitQualityAccessor> m_fitAccessor;
  };

  // ---------------- MaxOneSharedModule ----------------

  class MaxOneSharedModule : public virtual TrackCut {
  public:
    MaxOneSharedModule(InDetTrackSelectionTool*);
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    std::shared_ptr<SummaryAccessor> m_pixAccessor;
    std::shared_ptr<SummaryAccessor> m_sctAccessor;
  };

  // ---------------- MinNSiHitsIfSiSharedHits ----------------
  class MinNSiHitsIfSiSharedHits : public virtual TrackCut {
  public:
    MinNSiHitsIfSiSharedHits(InDetTrackSelectionTool*);
    void setMinSiHits(Int_t min) {m_minSiHits = min;}
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    Int_t m_minSiHits;
    std::shared_ptr<SummaryAccessor> m_pixHitsAccessor;
    std::shared_ptr<SummaryAccessor> m_sctHitsAccessor;
    std::shared_ptr<SummaryAccessor> m_pixSharedHitsAccessor;
    std::shared_ptr<SummaryAccessor> m_sctSharedHitsAccessor;
  };

  // ---------------- MinNSiHitsAboveEta ----------------
  class MinNSiHitsAboveEta : public virtual TrackCut {
  public:
    MinNSiHitsAboveEta(InDetTrackSelectionTool*);
    void setEtaCutoff(Double_t eta) {m_etaCutoff = eta;}
    void setMinSiHits(Double_t hits) {m_minSiHits = hits;}
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    Double_t m_etaCutoff;
    Int_t m_minSiHits;
    std::shared_ptr<SummaryAccessor> m_pixAccessor;
    std::shared_ptr<SummaryAccessor> m_sctAccessor;
    std::shared_ptr<EtaAccessor> m_etaAccessor;
  };

  // ---------------- MaxChiSq ----------------
  class MaxChiSq : public virtual TrackCut {
  public:
    MaxChiSq(InDetTrackSelectionTool*);
    void setMaxValue(Double_t max) {m_maxValue = max;}
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    Double_t m_maxValue;
    std::shared_ptr<FitQualityAccessor> m_accessor;
  };

  // ---------------- MaxChiSqPerNdf ----------------
  class MaxChiSqPerNdf : public virtual TrackCut {
  public:
    MaxChiSqPerNdf(InDetTrackSelectionTool*);
    void setMaxValue(Double_t max) {m_maxValue = max;}
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    Double_t m_maxValue;
    std::shared_ptr<FitQualityAccessor> m_accessor;
  };

  // ---------------- MinProb ----------------
  class MinProb : public virtual TrackCut {
  public:
    MinProb(InDetTrackSelectionTool*);
    void setMinValue(Double_t min) {m_minValue = min;}
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    Double_t m_minValue;
    std::shared_ptr<FitQualityAccessor> m_accessor;
  };


  // // ---------------- PtDependentSctHitsCut ----------------
  // class PtDependentSctHitsCut : public virtual TrackCut;


} // namespace InDet

#endif // INDETTRACKSELECTIONTOOL_INDETTRACKCUT_H
