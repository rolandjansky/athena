/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BASE_FAKE_BKG_TOOL_H
#define BASE_FAKE_BKG_TOOL_H

#include "AsgAnalysisInterfaces/IFakeBkgTool.h"
#include "AsgAnalysisInterfaces/IFakeBkgSystDescriptor.h"
#include "AsgTools/AsgTool.h"

#include <string>
#include <map>
#include <unordered_map>
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
  struct Efficiency;
  enum class Client;
}

namespace CP
{

  class ISelectionReadAccessor;

/* 
 * Base class of concrete tools implementations, providing common helper methods
 */
class BaseFakeBkgTool : virtual public CP::IFakeBkgTool, public CP::IFakeBkgSystDescriptor, public asg::AsgTool
#ifndef FAKEBKGTOOLS_ATLAS_ENVIRONMENT
  , public FakeBkgTools::PropertyManager<BaseFakeBkgTool>
#endif
{
  public:

    BaseFakeBkgTool(const std::string& toolname);
    virtual ~BaseFakeBkgTool();
  
    virtual StatusCode initialize() override;

    virtual StatusCode addEvent(const xAOD::IParticleContainer& particles, float extraWeight = 1.f) override final;
  
    virtual StatusCode register1DHistogram(TH1* h1, const float *val) override;
    virtual StatusCode register2DHistogram(TH2* h2, const float *xval, const float *yval) override;
    virtual StatusCode register3DHistogram(TH3* h3, const float *xval, const float *yval, const float *zval) override;
  
    virtual bool isAffectedBySystematic(const CP::SystematicVariation& systematic) const override;
    virtual CP::SystematicSet affectingSystematics() const override;
    virtual CP::SystematicSet recommendedSystematics() const override;
    virtual StatusCode applySystematicVariation(const CP::SystematicSet& systConfig) override;
  
    /// These functions are slow, don't use them in the tools implementations
    virtual CP::SystematicSet affectingSystematicsFor(const std::string& nuisanceParameter) const override;
    virtual std::string getUncertaintyDescription(const CP::SystematicVariation& systematic) const override final;
    virtual void printUncertaintyDescription(const CP::SystematicVariation& systematic) const override final;
    virtual bool isSystematicUncertainty(const CP::SystematicVariation& systematic) const override final;
    virtual bool isStatisticalUncertainty(const CP::SystematicVariation& systematic) const override final;
    virtual bool affectsElectrons(const CP::SystematicVariation& systematic) const override final;
    virtual bool affectsMuons(const CP::SystematicVariation& systematic) const override final;
    virtual bool affectsTaus(const CP::SystematicVariation& systematic) const override final;
    virtual bool affectsRealEfficiencies(const CP::SystematicVariation& systematic) const override final;
    virtual bool affectsFakeEfficiencies(const CP::SystematicVariation& systematic) const override final;
    virtual bool affectsFakeFactors(const CP::SystematicVariation& systematic) const override final;
    
    virtual const IFakeBkgSystDescriptor& getSystDescriptor() const override {return *this; }
  
  protected:

    bool m_initialized = false; //!
    bool m_needEventInfo = true; //!
    bool m_convertWhenMissing = false;

    /// This indicates which type of efficiencies/fake factor need to be filled
    virtual FakeBkgTools::Client clientForDB() = 0;

    std::vector<FakeBkgTools::ParticleData> m_particles; //!
  
    virtual StatusCode addEventCustom() = 0;
    // overridden by derived classes (compute the event weights and update sum of weights/histograms for AM+FF, increment event counters+average efficiencies for LM)
  
  #ifndef __CLING__
    std::unique_ptr<FakeBkgTools::Database> m_database; //!
  #else
	std::unique_ptr<int> m_database; //!
  #endif
    float m_externalWeight; //! comes from Event passed to addEvent()
  
    std::hash<std::string> m_hasher; //!
  #ifndef __CLING__
    std::set<FakeBkgTools::FinalState> m_cachedFinalStates; //!
  #else
	std::set<int> m_cachedFinalStates; //!
  #endif
    FakeBkgTools::FinalState getCachedFinalState(uint8_t nparticles, const std::string& strPID, const std::string& strProc, bool& success);
  
    std::string getListOfEfficienciesAffectedBy(uint16_t uid) const;   
  
    // maps to store relationships between histograms and variables
    std::map<TH1*, const float*> m_values_1dhisto_map; //!
    std::map<TH2*, std::pair<const float*, const float*> >  m_values_2dhisto_map; //!
    std::map<TH3*, std::tuple<const float*, const float*, const float*> >  m_values_3dhisto_map; //!

    StatusCode CheckHistogramCompatibility(const TH1* lhs, const TH1* rhs);

    /// \brief 'selection' settings used to compute the total yield / fill histograms
    std::string m_selection = defaultSelection();

    /// \brief 'process' settings used to compute the total yield / fill histograms
    std::string m_process = defaultProcess();
    
    /// \brief property EnergyUnit
    /// user can choose between MeV or GeV to indicate the unit of the pT parametrization of efficiencies in the input files
    std::string m_energyUnit;
  
    bool m_useDB = true; //!
  
    /// \brief property InputFiles
    std::vector<std::string> m_inputFiles;

    /// \brief property TightDecoration
    std::string m_tightDecoNameAndType;

    /// this can't be a unique_ptr as this can cause issues with the dictionary in some particular circumstances
    CP::ISelectionReadAccessor* m_tightAccessor = nullptr; //!

    /// \brief property ProgressFileName
    std::string m_progressFileName;

    /// \brief property ProgressFileDirectory
    std::string m_progressFileDirectory;
    
    /// \brief List of systematic variations (UID + sigma) to be considered
    /// This is implemented as a basic linked list, since unless in very rare cases it should only contain a single entry. 
    struct UncertaintyList
    {
        uint16_t UID;
        float sigma;
        UncertaintyList(uint16_t first_UID, float first_sigma) : UID(first_UID), sigma(first_sigma) {}
        void extraVariation(uint16_t extra_UID, float extra_sigma)
        { 
            if(m_next) m_next->extraVariation(extra_UID, extra_sigma);
            else m_next.reset(new UncertaintyList(extra_UID, extra_sigma));
        }
        const UncertaintyList* next() const { return m_next.get(); }
    protected:
        std::unique_ptr<UncertaintyList> m_next;
    };

    /// \brief Pointer to a value of the 'm_systSetDict' map
    /// it must be invalidated each time the map is updated
    /// in principle only applySystematicVariation() needs to do that. 
    UncertaintyList* m_selectedUncertainties = nullptr;

    /// \brief List of uncertainties in internal format, associated with a particular SystematicSet
    /// the m_selectedUncertainties pointer must be invalidated each time this map is updated
    /// in principle only applySystematicVariation() needs to do that. 
    std::unordered_map<CP::SystematicSet, UncertaintyList> m_systSetDict; //!

    /// \brief used to prevent multiple calls to applySystematicVariation() when unsupported
    /// set to true in a particular tool's constructor to disable the possibility of calling applySystematicVariation() more than once / after the first call to addEvent()
    bool m_unlimitedSystematicVariations = true; //!

    /// \brief when m_unlimitedSystematicVariations=false, keeps track of prior calls to applySystematicVariation() / addEvent()
    bool m_lockedSystematicVariations = false; //!

  private:

    /// \brief load the config file(s) storing efficiencies
    bool importEfficiencies(bool resetDB = false);

    std::pair<uint16_t, float> identifyCpSystematicVariation(const CP::SystematicVariation& systematic) const;
    
    friend struct FakeBkgTools::Efficiency;
};

}

#endif
