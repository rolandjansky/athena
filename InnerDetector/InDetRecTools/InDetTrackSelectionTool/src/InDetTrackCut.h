// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// InDetTrackCut.h
// Declarations of objects to perform cuts for InDetTrackSelectionTool

#ifndef INDETTRACKSELECTIONTOOL_INDETTRACKCUT_H
#define INDETTRACKSELECTIONTOOL_INDETTRACKCUT_H

#include "InDetTrackAccessor.h"

#include <map>
#include <array>

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
    operator bool() const {return this->result();}
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

  // ---------------- OrCut ----------------
  // a cut that passes if any of its N cuts do.
  template <size_t N>
  class OrCut : public TrackCut {
  public:
    OrCut(InDetTrackSelectionTool*);
    virtual StatusCode initialize();
    virtual bool result() const;
    void setCut(size_t i, std::unique_ptr<TrackCut>&& cut) {m_cuts.at(i).swap(cut);}
    //    std::unique_ptr<TrackCut>& operator[](size_t i) {return m_cuts.at(i);}
    std::unique_ptr<TrackCut>& Cut(size_t i) {return m_cuts.at(i);}
  private:
    std::array< std::unique_ptr<TrackCut> , N > m_cuts;
  }; // class OrCut

  // ---------------- MaxSummaryValueCut ----------------
  class MaxSummaryValueCut : public virtual TrackCut {
  public:
    MaxSummaryValueCut(InDetTrackSelectionTool*, Int_t max=0);
    MaxSummaryValueCut(InDetTrackSelectionTool*, Int_t max, const std::vector<xAOD::SummaryType>&);
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
    MinSummaryValueCut(InDetTrackSelectionTool*, Int_t min=0);
    MinSummaryValueCut(InDetTrackSelectionTool*, Int_t min, const std::vector<xAOD::SummaryType>&);
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

  // ---------------- FuncSummaryValueCut ----------------
  template <size_t N>
  class FuncSummaryValueCut : public virtual TrackCut {
  public:
    FuncSummaryValueCut(InDetTrackSelectionTool*, const std::array<xAOD::SummaryType,N>&&);
    void setFunction(std::function<bool(const std::array<uint8_t,N>&)> func) {m_func = func;}
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    std::function<bool(const std::array<uint8_t,N>&)> m_func;
    std::array< xAOD::SummaryType, N> m_summaryTypes;
    std::array< std::shared_ptr<SummaryAccessor>,N > m_summaryAccessors;
    mutable std::array<uint8_t,N> m_results; // member variable to store the results
  }; // class FuncSummaryValueCut
  

  // ---------------- MaxSummaryValueRatioCut ----------------
  class MaxSummaryValueRatioCut : public virtual TrackCut {
  public:
    MaxSummaryValueRatioCut(InDetTrackSelectionTool*, Double_t max);
    void setMaxValue(Double_t max) {m_maxValue = max;}
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

  // ---------------- MinTrtHitCut ----------------
  class MinTrtHitCut : public virtual MinSummaryValueCut {
  public: 
    MinTrtHitCut(InDetTrackSelectionTool*, Int_t min=0, Double_t etaLow=0, Double_t etaHigh=0);
    void setMaxEtaAcceptance(Double_t eta) {m_maxTrtEtaAcceptance = eta;}
    void setMaxEtaForCut(Double_t eta) {m_maxEtaForCut = eta;}
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    Double_t m_maxTrtEtaAcceptance;
    Double_t m_maxEtaForCut;
    std::shared_ptr< FuncAccessor<double, &xAOD::TrackParticle::eta> > m_etaAccessor;
  }; // class MinTrtHitCut

  // ---------------- MaxTrtHitRatioCut ----------------
  class MaxTrtHitRatioCut : public virtual MaxSummaryValueRatioCut {
  public: 
    MaxTrtHitRatioCut(InDetTrackSelectionTool*, Double_t ratio=0, Double_t etaLow=0, Double_t etaHigh=0);
    void setMaxEtaAcceptance(Double_t eta) {m_maxTrtEtaAcceptance = eta;}
    void setMaxEtaForCut(Double_t eta) {m_maxEtaForCut = eta;}
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    Double_t m_maxTrtEtaAcceptance;
    Double_t m_maxEtaForCut;
    std::shared_ptr< FuncAccessor<double, &xAOD::TrackParticle::eta> > m_etaAccessor;
  }; // class MaxTrtHitRatioCut

  // ---------------- D0Cut ----------------
  class D0Cut : public virtual TrackCut {
  public:
    D0Cut(InDetTrackSelectionTool*, Double_t max=0);
    void setMaxValue(Double_t max) {m_maxValue = max;}
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    Double_t m_maxValue;
    std::shared_ptr< ParamAccessor<0> > m_paramAccessor;
  }; // class D0Cut

  // ---------------- D0SigmaCut ----------------
  class D0SigmaCut : public virtual TrackCut {
  public:
    D0SigmaCut(InDetTrackSelectionTool*, Double_t max=0);
    void setMaxValue(Double_t max) {m_maxValueSq = max*max;} // cut is on variance
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    Double_t m_maxValueSq;
    std::shared_ptr< ParamCovAccessor<0,0> > m_paramCovAccessor;
  }; // class D0SigmaCut

  // ---------------- D0SignificanceCut ----------------
  class D0SignificanceCut : public virtual TrackCut {
  public:
    D0SignificanceCut(InDetTrackSelectionTool*, Double_t max=0);
    void setMaxValue(Double_t max) {m_maxValueSq = max*max;}
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    Double_t m_maxValueSq;
    std::shared_ptr< ParamAccessor<0> > m_paramAccessor;
    std::shared_ptr< ParamCovAccessor<0,0> > m_paramCovAccessor;
  }; // class D0SignificanceCut


  // ---------------- Z0Cut ----------------
  class Z0Cut : public virtual TrackCut {
  public:
    Z0Cut(InDetTrackSelectionTool*, Double_t max=0);
    void setMaxValue(Double_t max) {m_maxValue = max;}
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    Double_t m_maxValue;
    std::shared_ptr< ParamAccessor<1> > m_paramAccessor;
  }; // class Z0Cut

  // ---------------- Z0SigmaCut ----------------
  class Z0SigmaCut : public virtual TrackCut {
  public:
    Z0SigmaCut(InDetTrackSelectionTool*, Double_t max=0);
    void setMaxValue(Double_t max) {m_maxValueSq = max*max;} // cut is on variance so we will save the squared value
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    Double_t m_maxValueSq;
    std::shared_ptr< ParamCovAccessor<1,1> > m_paramCovAccessor;
  }; // class Z0SigmaCut

  // ---------------- Z0SignificanceCut ----------------
  class Z0SignificanceCut : public virtual TrackCut {
  public:
    Z0SignificanceCut(InDetTrackSelectionTool*, Double_t max=0);
    void setMaxValue(Double_t max) {m_maxValueSq = max*max;}
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    Double_t m_maxValueSq;
    std::shared_ptr< ParamAccessor<1> > m_paramAccessor;
    std::shared_ptr< ParamCovAccessor<1,1> > m_paramCovAccessor;
  }; // class Z0SignificanceCut


  // ---------------- Z0SinThetaCut ----------------
  class Z0SinThetaCut : public virtual TrackCut {
  public:
    Z0SinThetaCut(InDetTrackSelectionTool*, Double_t max=0);
    void setMaxValue(Double_t max) {m_maxValue = max;}
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    Double_t m_maxValue;
    std::shared_ptr< ParamAccessor<1> > m_Z0Accessor;
    std::shared_ptr< ParamAccessor<3> > m_ThetaAccessor;
  }; // class Z0SinThetaCut

  // ---------------- Z0SinThetaSigmaCut ----------------
  class Z0SinThetaSigmaCut : public virtual TrackCut {
  public:
    Z0SinThetaSigmaCut(InDetTrackSelectionTool*, Double_t maxSigma=0);
    void setMaxValue(Double_t max) {m_maxValueSq = max*max;} // cut is on variance
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    Double_t m_maxValueSq;
    std::shared_ptr< ParamAccessor<1> > m_Z0Accessor;
    std::shared_ptr< ParamCovAccessor<1,1> > m_Z0VarAccessor;
    std::shared_ptr< ParamAccessor<3> > m_ThetaAccessor;
    std::shared_ptr< ParamCovAccessor<3,3> > m_ThetaVarAccessor;
    std::shared_ptr< ParamCovAccessor<1,3> > m_CovAccessor;
  }; // class Z0SinThetaSigmaCut

  // ---------------- Z0SinThetaSignificanceCut ----------------
  class Z0SinThetaSignificanceCut : public virtual TrackCut {
  public:
    Z0SinThetaSignificanceCut(InDetTrackSelectionTool*, Double_t maxSig=0);
    void setMaxValue(Double_t max) {m_maxValueSq = max*max;} // cut is on variance
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    Double_t m_maxValueSq;
    std::shared_ptr< ParamAccessor<1> > m_Z0Accessor;
    std::shared_ptr< ParamCovAccessor<1,1> > m_Z0VarAccessor;
    std::shared_ptr< ParamAccessor<3> > m_ThetaAccessor;
    std::shared_ptr< ParamCovAccessor<3,3> > m_ThetaVarAccessor;
    std::shared_ptr< ParamCovAccessor<1,3> > m_CovAccessor;
  }; // class Z0SinThetaSignificanceCut

  // ---------------- MinCut ----------------
  template <typename T, T (xAOD::TrackParticle::*Func)() const>
  class MinCut : public virtual TrackCut {
  public:
    MinCut(InDetTrackSelectionTool*, T min, std::string accName);
    void setMinValue(T min) {m_minValue = min;}
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    T m_minValue;
    std::string m_accName;
    std::shared_ptr< FuncAccessor<T, Func> > m_accessor;
  };

  // ---------------- MaxCut ----------------
  template <typename T, T (xAOD::TrackParticle::*Func)() const>
  class MaxCut : public virtual TrackCut {
  public:
    MaxCut(InDetTrackSelectionTool*, T max, std::string accName);
    void setMaxValue(T max) {m_maxValue = max;}
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    T m_maxValue;
    std::string m_accName;
    std::shared_ptr< FuncAccessor<T, Func> > m_accessor;
  };

  // ---------------- MinAbsCut ----------------
  template <typename T, T (xAOD::TrackParticle::*Func)() const>
  class MinAbsCut : public virtual TrackCut {
  public:
    MinAbsCut(InDetTrackSelectionTool*, T min, std::string accName);
    void setMinValue(T min) {m_minValue = min;}
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    T m_minValue;
    std::string m_accName;
    std::shared_ptr< FuncAccessor<T, Func> > m_accessor;
  };

  // ---------------- MaxAbsCut ----------------
  template <typename T, T (xAOD::TrackParticle::*Func)() const>
  class MaxAbsCut : public virtual TrackCut {
  public:
    MaxAbsCut(InDetTrackSelectionTool*, T max, std::string accName);
    void setMaxValue(T max) {m_maxValue = max;}
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    T m_maxValue;
    std::string m_accName;
    std::shared_ptr< FuncAccessor<T, Func> > m_accessor;
  };

  // ---------------- EtaDependentChiSqCut ----------------
  class EtaDependentChiSqCut : public virtual TrackCut {
  public:
    EtaDependentChiSqCut(InDetTrackSelectionTool*);
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    std::shared_ptr< FuncAccessor<double, &xAOD::TrackParticle::eta> > m_etaAccessor;
    std::shared_ptr<FitQualityAccessor> m_fitAccessor;
  };

  // ---------------- MinNSiHitsAboveEta ----------------
  class MinNSiHitsAboveEta : public virtual TrackCut {
  public:
    MinNSiHitsAboveEta(InDetTrackSelectionTool*, Int_t hits=0, Double_t eta=0);
    void setEtaCutoff(Double_t eta) {m_etaCutoff = eta;}
    void setMinSiHits(Int_t hits) {m_minSiHits = hits;}
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    Double_t m_etaCutoff;
    Int_t m_minSiHits;
    std::shared_ptr<SummaryAccessor> m_pixAccessor;
    std::shared_ptr<SummaryAccessor> m_pixDeadAccessor;
    std::shared_ptr<SummaryAccessor> m_sctAccessor;
    std::shared_ptr<SummaryAccessor> m_sctDeadAccessor;
    std::shared_ptr< FuncAccessor<double, &xAOD::TrackParticle::eta> > m_etaAccessor;
  };

  // ---------------- MaxChiSq ----------------
  class MaxChiSq : public virtual TrackCut {
  public:
    MaxChiSq(InDetTrackSelectionTool*, Double_t max=0);
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
    MaxChiSqPerNdf(InDetTrackSelectionTool*, Double_t max=0);
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
    MinProb(InDetTrackSelectionTool*, Double_t min=0);
    void setMinValue(Double_t min) {m_minValue = min;}
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    Double_t m_minValue;
    std::shared_ptr<FitQualityAccessor> m_accessor;
  };

  // ---------------- MinEProbHT ----------------
  // unfortunately not yet a summary value in Trk::Tracks
  //   so we can't call this a summary value cut
  class MinEProbHTCut : public virtual TrackCut {
  public:
    MinEProbHTCut(InDetTrackSelectionTool*, Double_t min=0);
    void setMin(Double_t min) {m_minValue = min;}
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    Double_t m_minValue;
    std::shared_ptr<eProbabilityHTAccessor> m_accessor;
  }; // class MinEProbHT
  
 // ---------------- EtaDependentSiliconHitsCut ----------------
  class EtaDependentSiliconHitsCut : public virtual TrackCut {
  public:
    EtaDependentSiliconHitsCut(InDetTrackSelectionTool*, std::vector<Double_t> eta, std::vector<Int_t> hits);
    void setEta(std::vector<Double_t> eta) {m_etaCutoffs = eta;}
    void setHits(std::vector<Int_t> hits) {m_siHitCuts = hits;}
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    std::vector<Double_t> m_etaCutoffs;
    std::vector<Int_t> m_siHitCuts;
    std::shared_ptr< FuncAccessor< double, &xAOD::TrackParticle::eta> > m_etaAccessor;
    std::shared_ptr<SummaryAccessor> m_sctAccessor;
    std::shared_ptr<SummaryAccessor> m_pixAccessor;
    std::shared_ptr<SummaryAccessor> m_sctDeadAccessor;
    std::shared_ptr<SummaryAccessor> m_pixDeadAccessor;
  } ;
  // ---------------- EtaDependentPtCut ----------------                                                                                                                                                 
  class EtaDependentPtCut : public virtual TrackCut {
  public:
    EtaDependentPtCut(InDetTrackSelectionTool*, std::vector<Double_t> eta, std::vector<Double_t> pt);
    void setEta(std::vector<Double_t> eta) {m_etaCutoffs = eta;}
    void setPt(std::vector<Double_t> pt) {m_ptCuts = pt;}
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    std::vector<Double_t> m_etaCutoffs;
    std::vector<Double_t> m_ptCuts;
    std::shared_ptr< FuncAccessor< double, &xAOD::TrackParticle::eta> > m_etaAccessor;
    std::shared_ptr< FuncAccessor< double, &xAOD::TrackParticle::pt> > m_ptAccessor;
  } ;



  // ---------------- PtDependentSctHitsCut ----------------
  // this cut could in principle be constructed out of others,
  //   but it might not be so clean.
  // maybe as a FuncSummaryValueCut if the lambda function capture of the vectors works in a performant manner
  class PtDependentSctHitsCut : public virtual TrackCut {
  public:
    PtDependentSctHitsCut(InDetTrackSelectionTool*, std::vector<Double_t> pt, std::vector<Int_t> sct);
    void setPt(std::vector<Double_t> pt) {m_ptCutoffs = pt;}
    void setSct(std::vector<Int_t> sct) {m_sctHitCuts = sct;}
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    std::vector<Double_t> m_ptCutoffs;
    std::vector<Int_t> m_sctHitCuts;
    std::shared_ptr< FuncAccessor< double, &xAOD::TrackParticle::pt> > m_ptAccessor;
    std::shared_ptr<SummaryAccessor> m_sctAccessor;
    std::shared_ptr<SummaryAccessor> m_sctDeadAccessor;
  };

#ifndef XAOD_ANALYSIS
  // ---------------- MinSiHitsModTopBottomCut ----------------
  class MinSiHitsModTopBottomCut : public virtual TrackCut {
  public:
    MinSiHitsModTopBottomCut(InDetTrackSelectionTool*, Int_t top=0, Int_t bottom=0);
    void setMinTop(uint8_t min) {m_minTop = min;}
    void setMinBottom(uint8_t min) {m_minBottom = min;}
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    uint8_t m_minTop;
    uint8_t m_minBottom;
    std::shared_ptr<SiHitsTopBottomAccessor> m_accessor;
  };
#endif


} // namespace InDet


// -------------------------------------------------------
// ---------------- template definitions -----------------
// -------------------------------------------------------

template <class AccessorType>
StatusCode InDet::TrackCut::getAccessor(const std::string& accessorName,
					std::shared_ptr<AccessorType>& accessor)
{
  // this function looks for the accessor indicated by "name" in the tool's
  // track accessors and assigns "accessor" to it if it exists, creating
  // the accessor if it does not.
  std::unordered_map< std::string, std::shared_ptr<TrackAccessor> >::const_iterator it =
    std::find_if(m_trackAccessors->begin(), m_trackAccessors->end(),
		 [&] (std::pair< std::string, std::shared_ptr<TrackAccessor> > acc)
		 {return acc.first == accessorName;} );
  if (it == m_trackAccessors->end()) {
    // if we can't find the accessor, add one to the tool
    if (!m_selectionTool) {
      ATH_MSG_ERROR( "Must initialize TrackCut with pointer to the selection tool before adding accessors." );
      return StatusCode::FAILURE;
    }
    accessor = std::make_shared<AccessorType>(m_selectionTool);
    (*m_trackAccessors)[accessorName] = accessor;
    ATH_MSG_DEBUG( "Adding accessor " << accessorName );
  } else {
    accessor = std::dynamic_pointer_cast<AccessorType>(it->second);
    if (accessor==nullptr) {
      ATH_MSG_ERROR( "Logic error: could not cast accessor to type " << typeid(AccessorType).name() );
      return StatusCode::FAILURE;
    }
  }

  if (!accessor) {
    // this could happen if the accessor indicated by "name" is not of type "AccessorType"
    ATH_MSG_ERROR( "Could not instantiate "<< accessorName <<
		   " accessor. Pointer to accessor is to type " <<
		   typeid(AccessorType).name() << ". Is this correct?" );
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}



// ---------------- MinCut ----------------
template <typename T, T (xAOD::TrackParticle::*Func)() const>
InDet::MinCut<T,Func>::MinCut(InDet::InDetTrackSelectionTool* tool, T min, std::string accName)
  : InDet::TrackCut(tool)
  , m_minValue(min)
  , m_accName(accName)
  , m_accessor(nullptr)
{
}

template <typename T, T (xAOD::TrackParticle::*Func)() const>
StatusCode InDet::MinCut<T,Func>::initialize()
{
  ATH_CHECK( TrackCut::initialize() );
  if (m_accName.empty()) {
    ATH_MSG_ERROR( "Accessor name string is empty!" );
    return StatusCode::FAILURE;
  }
  ATH_CHECK( getAccessor(m_accName, m_accessor) );
  return StatusCode::SUCCESS;
}

template <typename T, T (xAOD::TrackParticle::*Func)() const>
bool InDet::MinCut<T,Func>::result() const
{
  if (!m_accessor) {
    ATH_MSG_WARNING( m_accName << " accessor not valid. Track will not pass." );
    return false;
  }
  return m_accessor->getValue() >= m_minValue;
}

// ---------------- MaxCut ----------------
template <typename T, T (xAOD::TrackParticle::*Func)() const>
InDet::MaxCut<T,Func>::MaxCut(InDet::InDetTrackSelectionTool* tool, T max, std::string accName)
  : InDet::TrackCut(tool)
  , m_maxValue(max)
  , m_accName(accName)
  , m_accessor(nullptr)
{
}

template <typename T, T (xAOD::TrackParticle::*Func)() const>
StatusCode InDet::MaxCut<T,Func>::initialize()
{
  ATH_CHECK( TrackCut::initialize() );
  if (m_accName.empty()) {
    ATH_MSG_ERROR( "Accessor name string is empty!" );
    return StatusCode::FAILURE;
  }
  ATH_CHECK( getAccessor(m_accName, m_accessor) );
  return StatusCode::SUCCESS;
}

template <typename T, T (xAOD::TrackParticle::*Func)() const>
bool InDet::MaxCut<T,Func>::result() const
{
  if (!m_accessor) {
    ATH_MSG_WARNING( m_accName << " accessor not valid. Track will not pass." );
    return false;
  }
  return m_accessor->getValue() <= m_maxValue;
}


// ---------------- MinAbsCut ----------------
template <typename T, T (xAOD::TrackParticle::*Func)() const>
InDet::MinAbsCut<T,Func>::MinAbsCut(InDet::InDetTrackSelectionTool* tool, T min, std::string accName)
  : InDet::TrackCut(tool)
  , m_minValue(min)
  , m_accName(accName)
  , m_accessor(nullptr)
{
}

template <typename T, T (xAOD::TrackParticle::*Func)() const>
StatusCode InDet::MinAbsCut<T,Func>::initialize()
{
  ATH_CHECK( TrackCut::initialize() );
  if (m_accName.empty()) {
    ATH_MSG_ERROR( "Accessor name string is empty!" );
    return StatusCode::FAILURE;
  }
  ATH_CHECK( getAccessor(m_accName, m_accessor) );
  return StatusCode::SUCCESS;
}

template <typename T, T (xAOD::TrackParticle::*Func)() const>
bool InDet::MinAbsCut<T,Func>::result() const
{
  if (!m_accessor) {
    ATH_MSG_WARNING( m_accName << " accessor not valid. Track will not pass." );
    return false;
  }
  return std::fabs(m_accessor)->getValue() >= m_minValue;
}

// ---------------- MaxAbsCut ----------------
template <typename T, T (xAOD::TrackParticle::*Func)() const>
InDet::MaxAbsCut<T,Func>::MaxAbsCut(InDet::InDetTrackSelectionTool* tool, T max, std::string accName)
  : InDet::TrackCut(tool)
  , m_maxValue(max)
  , m_accName(accName)
  , m_accessor(nullptr)
{
}

template <typename T, T (xAOD::TrackParticle::*Func)() const>
StatusCode InDet::MaxAbsCut<T,Func>::initialize()
{
  ATH_CHECK( TrackCut::initialize() );
  if (m_accName.empty()) {
    ATH_MSG_ERROR( "Accessor name string is empty!" );
    return StatusCode::FAILURE;
  }
  ATH_CHECK( getAccessor(m_accName, m_accessor) );
  return StatusCode::SUCCESS;
}

template <typename T, T (xAOD::TrackParticle::*Func)() const>
bool InDet::MaxAbsCut<T,Func>::result() const
{
  if (!m_accessor) {
    ATH_MSG_WARNING( m_accName << " accessor not valid. Track will not pass." );
    return false;
  }
  return std::fabs(m_accessor->getValue()) <= m_maxValue;
}


#endif // INDETTRACKSELECTIONTOOL_INDETTRACKCUT_H
