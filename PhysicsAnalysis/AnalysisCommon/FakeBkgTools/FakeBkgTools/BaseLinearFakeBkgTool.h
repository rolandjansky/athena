/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BASE_LINEAR_FAKE_BKG_TOOL_H
#define BASE_LINEAR_FAKE_BKG_TOOL_H

#include "FakeBkgTools/BaseFakeBkgTool.h"
#include "AsgAnalysisInterfaces/ILinearFakeBkgTool.h"

#ifdef FAKEBKGTOOLS_ATLAS_ENVIRONMENT
  #include "AsgTools/AsgTool.h"
  #include "xAODBase/IParticle.h"
#endif

#include <string>
#include <map>
#include <set>
#include <vector>
#include <memory>
#include <functional>

namespace FakeBkgTools
{
  /// These are defined in FakeBkgInternals.h
  struct ParticleData;
  class FinalState;
  class Database;
  struct Weight;
  struct Yield;
  enum class Client;
}

namespace CP
{

/* 
 * Base class of concrete tools implementations, providing common helper methods
 */
class BaseLinearFakeBkgTool : public BaseFakeBkgTool, virtual public ILinearFakeBkgTool
#ifndef FAKEBKGTOOLS_ATLAS_ENVIRONMENT 
  , public FakeBkgTools::ExtraPropertyManager<BaseLinearFakeBkgTool, BaseFakeBkgTool>
#endif
{
  public:

    BaseLinearFakeBkgTool(const std::string toolname);
    virtual ~BaseLinearFakeBkgTool();

    virtual StatusCode initialize() override;
  
    virtual StatusCode getEventWeight(float& weight, const std::string& selection, const std::string& process) override final;

    virtual StatusCode getTotalYield(float& yield, float& statErrorUp, float& statErrorDown) override final;
    
    virtual StatusCode register1DHistogram(TH1* h1, const float *val) override;
    virtual StatusCode register2DHistogram(TH2* h2, const float *xval, const float *yval) override;
    virtual StatusCode register3DHistogram(TH3* h3, const float *xval, const float *yval, const float *zval) override;

    StatusCode assignYieldRange(TH1* h);
  
    virtual StatusCode saveProgress(TDirectory* dir) override;
  
  protected:

  #ifndef __CLING__
    /// \brief accumulated yield for all events (and histogram bins with uncertainties)
    std::vector<FakeBkgTools::Yield> m_yields; //!
    
    /// \brief cached weight+uncertainties for a single event
    /// Each tool derived from this base class MUST clear the map in addEventCustom()!
    std::map<FakeBkgTools::FinalState, FakeBkgTools::Weight> m_cachedWeights;
  #else
    std::vector<int> m_yields; //!
    std::map<int,int> m_cachedWeights; //!
  #endif
  
    std::map<TH1*, std::pair<uint32_t, uint32_t>> m_histogramYieldsRange; //!

    /// be sure to only call this once per event! (typically at the end of addEvent())
    StatusCode incrementTotalYield();

    virtual StatusCode getEventWeightCustom(FakeBkgTools::Weight& weight, const FakeBkgTools::FinalState& fs) = 0;
  
  private:
  
    StatusCode getEventWeightCommon(FakeBkgTools::Weight& weight, const std::string& selection, const std::string& process);
    
    StatusCode mergeSubjobs();

};

}

#endif
