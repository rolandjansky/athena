/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "FakeBkgTools/BaseFakeBkgTool.h"
#include "FakeBkgTools/FakeBkgInternals.h"
#include "FakeBkgTools/Database.h"
#include "xAODEgamma/Electron.h"
#include "xAODMuon/Muon.h"
#include "xAODTau/TauJet.h"
#include "SelectionHelpers/ISelectionReadAccessor.h"

#ifndef FAKEBKGTOOLS_ATLAS_ENVIRONMENT
    #define declareProperty(n, p, h) declareProperty(n, &BaseFakeBkgTool::p, h)
#endif

#include <TFile.h>
#include <TH2F.h>
#include <TH3.h>
#include <string>
#include <vector>
#include <regex>
#include <stdexcept>

using namespace FakeBkgTools;
using namespace CP;

BaseFakeBkgTool::BaseFakeBkgTool(const std::string& toolname) :  AsgTool(toolname),
  m_initialized(false), m_database(nullptr), m_energyUnit("MeV"), m_tightDecoNameAndType("Tight,as_char"), m_progressFileName("none"), m_progressFileDirectory("")
{
    /// Note: don't use the usual syntax declareProperty("", x = default, ""), it won't work for standalone compilation
    
    declareProperty("InputFiles", m_inputFiles, "list of XML/ROOT files storing the efficiencies [default = {}]. If left empty, the tool assumes that efficiencies are provided by the user as decorations (xAOD) or class members (standalone) of the lepton objects");
    
    m_selection = IFakeBkgTool::defaultSelection();
    declareProperty("Selection", m_selection, "Selection used to increment total yield / fill histograms [default = \"" + m_selection +"\"]");
    
    m_process = IFakeBkgTool::defaultProcess();
    declareProperty("Process", m_process, "Process used to increment total yield / fill histograms [default = \"" + m_process +"\"]");

    declareProperty("EnergyUnit", m_energyUnit, "MeV or GeV -- keep consistent between the input (IParticle decorations/members) and the configuration files! [default = \"MeV\"]");

    declareProperty("ConvertWhenMissing", m_convertWhenMissing, "set to 'true' to compute fake factors from fake efficiencies if only the latter are provided in the config files (and vice-versa) [default = false]");

    declareProperty("TightDecoration", m_tightDecoNameAndType, "Name (and type) of the decoration used to indicate whether leptons pass the 'tight' requirements or not [default = \"Tight,as_char\"]");

    declareProperty("ProgressFileName", m_progressFileName, "Name of ntuple file with results from a subset of the data");
    
    declareProperty("ProgressFileDirectory", m_progressFileDirectory, "complementary to ProgressFileName, specifies the directory inside the file where the results are stored [default = \"\"]");
}

BaseFakeBkgTool::~BaseFakeBkgTool()
{
    delete m_tightAccessor;
}

Client BaseFakeBkgTool::clientForDB()
{
    return Client::NONE;
}

StatusCode BaseFakeBkgTool::initialize()
{
    if(m_initialized)
    {
        ATH_MSG_WARNING("the tool has already been initialized successfully");
        return StatusCode::FAILURE;
    }
    
    m_useDB = m_inputFiles.size();
    
    bool useGeV;
    if(m_energyUnit == "MeV" || m_energyUnit == "MEV") useGeV = false;
    else if(m_energyUnit == "GeV" || m_energyUnit == "GEV" ) useGeV = true;
    else
    {
        ATH_MSG_ERROR("unrecognized energy unit \"" << m_energyUnit << '"');
        return StatusCode::FAILURE;
    }
    
    std::unique_ptr<CP::ISelectionReadAccessor> tightAccessor;
    ATH_CHECK(CP::makeSelectionReadAccessor(m_tightDecoNameAndType, tightAccessor));
    m_tightAccessor = tightAccessor.release();
    
    if(!m_useDB)
    {
        ATH_MSG_WARNING("no input file(s) provided with the reference efficiencies, will try reading the efficiencies directly from the IParticle");
    }
    
    if(m_useDB && clientForDB()!=Client::NONE)
    {
        auto db = new Database(clientForDB(), useGeV, m_convertWhenMissing);
        if(!db) return StatusCode::FAILURE;
        m_database = std::unique_ptr<Database>(db);
        if(!importEfficiencies(true)){
          ATH_MSG_ERROR("importEfficiencies failed!");
          return StatusCode::FAILURE;
        }
        m_needEventInfo = db->needEventInfo();
    }
    else m_needEventInfo = false;
    
    m_initialized = true;
    
    return StatusCode::SUCCESS;
}

bool BaseFakeBkgTool::importEfficiencies(bool resetDB)
{
    if(resetDB) m_database->reset();
    std::string filename;
    try
    {
        for(const auto& fn : m_inputFiles)
        {
            filename = fn;
            auto pos = filename.rfind(".xml");
            if(pos == filename.length()-4)
            {
                m_database->importXML(filename);
                continue;
            }
            pos = filename.rfind(".root");
            if(pos == filename.length()-5)
            {
                m_database->importDefaultROOT(fn);
                continue;
            }
            ATH_MSG_ERROR("File extension not supported for " << filename);
            return false;
        }
    }
    catch(const Database::XmlError& err) /// XML parsing error
    {
        unsigned line1 = m_database->getXmlLineNumber(err.location.ptr);
        unsigned line2 = m_database->getXmlLineNumber(err.location.endptr);
        ATH_MSG_INFO("Exception caught while reading file " << filename << ", details follow");
        std::string fullmsg = "(while parsing XML, line";
        if(line2 != line1) fullmsg += "s " + std::to_string(line1) + " - " + std::to_string(line2);
        else fullmsg += " " + std::to_string(line1);
        fullmsg +=  "): " +  err.reason;
        ATH_MSG_ERROR(fullmsg);
        return false;
    }
    catch(const Database::GenericError& err) /// ROOT histograms importing error, or unknown error
    {
        ATH_MSG_INFO("Exception caught while reading file " << filename << ", details follow");
        ATH_MSG_ERROR(err.reason);
        return false;
    }
    return m_database->ready();
}

StatusCode BaseFakeBkgTool::addEvent(const xAOD::IParticleContainer& iparticles, float mcWeight)
{
    if(!m_initialized)
    {
         ATH_MSG_WARNING("the tool hasn't been initialized");
        return StatusCode::FAILURE;
    }
    m_particles.clear();
    if(!m_unlimitedSystematicVariations) m_lockedSystematicVariations = true; /// prevent further calls to applySystematicVariation() if not allowed

    const xAOD::EventInfo* eventInfo = nullptr; /// only retrieved if some of the efficiency parameters are event-level!
    if(m_needEventInfo)
    {
      #ifdef FAKEBKGTOOLS_ATLAS_ENVIRONMENT
        ATH_CHECK( evtStore()->retrieve(eventInfo, "EventInfo") );
      #else
        eventInfo = &iparticles.eventInfo;
      #endif
    }
    
    for(const auto& pp : iparticles)
    {
        const auto& p = *pp; /// for standalone compilation, pp is already a reference and the * operator is redefined to a no-op so that this works
        m_particles.emplace_back();
        auto& d = m_particles.back();
        d.tight = m_tightAccessor->getBool(p);
        switch(p.type())
        {
            case xAOD::Type::Electron: d.charge = static_cast<const xAOD::Electron&>(p).charge(); break;
            case xAOD::Type::Muon: d.charge = static_cast<const xAOD::Muon&>(p).charge(); break;
            case xAOD::Type::Tau: d.charge = static_cast<const xAOD::TauJet&>(p).charge(); break;
            default:
                ATH_MSG_WARNING("unknown particle type, setting charge to 0");
                d.charge = 0;
        }
        if(m_useDB)
        {
            std::string error;
            if(!m_database->fillEfficiencies(d, p, *eventInfo, error))
            {
                ATH_MSG_ERROR("unable to retrieve efficiencies: " << error);
                return StatusCode::FAILURE;
            }
            ATH_MSG_DEBUG("particle has fake fact. = " << d.fake_factor.value(this) <<", fake eff. = " << d.fake_efficiency.value(this) <<", real eff. = " << d.real_efficiency.value(this));
        }
        else
        {
            d.real_efficiency.nominal = p.auxdataConst<float>("real_eff");
            d.fake_efficiency.nominal = p.auxdataConst<float>("fake_eff");
        }
    }
    if(m_particles.size() > maxParticles())
    {
        ATH_MSG_WARNING( "the input contains " << m_particles.size() << " particles but the maximum allowed is " << maxParticles()
            << "; the last " << (m_particles.size()-maxParticles()) << " will be ignored");
        m_particles.erase(m_particles.begin() + maxParticles(), m_particles.end());
    }
    m_externalWeight = mcWeight;
    ATH_MSG_DEBUG("calling addEventCustom() with #particles = " << m_particles.size());
    return addEventCustom();
}

FinalState BaseFakeBkgTool::getCachedFinalState(uint8_t nparticles, const std::string& strPID, const std::string& strProc, bool& success)
{
    success = true;
    size_t hFS = m_hasher(strProc) ^ m_hasher(strPID) ^ nparticles;
    auto itrFS = m_cachedFinalStates.find(FinalState(hFS));
    if(itrFS != m_cachedFinalStates.end()) return *itrFS;
    std::string error;
    FinalState fs(hFS, nparticles, strPID, strProc, error);
    if(error.length())
    {
        ATH_MSG_ERROR(error);
        success = false;
        return FinalState(0);
    }
    if(m_cachedFinalStates.size() < 1024) /// no longer fill cache if it becomes too large
    {
        m_cachedFinalStates.emplace(fs);
    }
    return fs;
}

StatusCode BaseFakeBkgTool::register1DHistogram(TH1* h1, const float *val) {

    if(!h1)
    {
        ATH_MSG_ERROR("invalid histogram pointer");
        return StatusCode::FAILURE;
    }
    const std::string name = h1->GetName();
    auto itr = m_values_1dhisto_map.begin();
    const auto enditr = m_values_1dhisto_map.end(); 
    for(;itr!=enditr;++itr)
    {
        if(itr->first == h1)
        {
            ATH_MSG_ERROR("the histogram \"" << name << "\" has already been registered");
            return StatusCode::FAILURE;
        }
        if(name == itr->first->GetName()) break;
    }
    if(itr == enditr)
    {
      m_values_1dhisto_map.emplace(h1, val);
    }
    else
    {
        ATH_CHECK( CheckHistogramCompatibility(h1, itr->first) );
    }
    return StatusCode::SUCCESS;
}

StatusCode BaseFakeBkgTool::register2DHistogram(TH2* h2, const float *xval, const float *yval) {

    if(!h2)
    {
        ATH_MSG_ERROR("invalid histogram pointer");
        return StatusCode::FAILURE;
    }
    const std::string name = h2->GetName();
    auto itr = m_values_2dhisto_map.begin();
    const auto enditr = m_values_2dhisto_map.end(); 
    for(;itr!=enditr;++itr)
    {
        if(itr->first == h2)
        {
            ATH_MSG_ERROR("the histogram \"" << name << "\" has already been registered");
            return StatusCode::FAILURE;
        }
        if(name == itr->first->GetName()) break;
    }
    if(itr == enditr)
    {
      m_values_2dhisto_map.emplace(h2, std::make_pair(xval, yval));
    }
    else
    {
        ATH_CHECK( CheckHistogramCompatibility(h2, itr->first) );
    }
    return StatusCode::SUCCESS;
}

				StatusCode BaseFakeBkgTool::register3DHistogram(TH3* h3, const float *xval, const float *yval, const float *zval) {

    if(!h3)
    {
        ATH_MSG_ERROR("invalid histogram pointer");
        return StatusCode::FAILURE;
    }
    const std::string name = h3->GetName();
    auto itr = m_values_3dhisto_map.begin();
    const auto enditr = m_values_3dhisto_map.end(); 
    for(;itr!=enditr;++itr)
    {
        if(itr->first == h3)
        {
            ATH_MSG_ERROR("the histogram \"" << name << "\" has already been registered");
            return StatusCode::FAILURE;
        }
        if(name == itr->first->GetName()) break;
    }
    if(itr == enditr)
    {
m_values_3dhisto_map.emplace(h3, std::make_tuple(xval, yval, zval));
    }
    else
    {
        ATH_CHECK( CheckHistogramCompatibility(h3, itr->first) );
    }
    return StatusCode::SUCCESS;
}

StatusCode BaseFakeBkgTool::CheckHistogramCompatibility(const TH1* lhs, const TH1* rhs)
{
    std::string error;
    if(std::string(lhs->GetName()) != rhs->GetName()) error = "names";
    else if(lhs->GetDimension() != rhs->GetDimension()) error = "dimensions";
    else if(lhs->GetNbinsX()!=rhs->GetNbinsX() || lhs->GetNbinsY()!=rhs->GetNbinsY() || lhs->GetNbinsZ()!=rhs->GetNbinsZ()) error = "number of bins";
    else
    {
        for(auto getAxis : std::initializer_list<const TAxis*(TH1::*)()const>{&TH1::GetXaxis, &TH1::GetYaxis, &TH1::GetZaxis})
        {
            auto lhsAxis=(lhs->*getAxis)(), rhsAxis=(rhs->*getAxis)();
            for(int i=0;i<=lhsAxis->GetNbins();++i)
            {
                auto x=lhsAxis->GetBinUpEdge(i), y=rhsAxis->GetBinUpEdge(i), width=lhsAxis->GetBinWidth(i?i:1);
                if(std::fabs(x-y) > 0.01*width) error = "bin edges";
            }
        }
    }
    if(error.length())
    {
        ATH_MSG_ERROR("the registered histogram \"" << lhs->GetName() << "\" is not compatible with the one saved in the in-progress ROOT file (mismatching " << error << "). Or, you tried registering two different histograms with the same name.");
        return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
}

std::string BaseFakeBkgTool::getListOfEfficienciesAffectedBy(unsigned short uid) const
{
    if(!m_initialized || !m_database)
    {
        ATH_MSG_ERROR("This function can be called only once the tool has been initialized");
        return "";
    }
    std::bitset<Database::N_EFFICIENCY_TYPES> affects;
    auto stat = m_database->findStat(uid);
    if(stat) affects = stat->affects;
    else
    {
        auto syst = m_database->findSyst(uid);
        if(syst) affects = syst->affects;
        else
        {
            ATH_MSG_ERROR("uncertainty with UID " << std::hex << uid << std::dec << " not found in database");
            return "";
        }
    }
    std::string info;
    if(affects[Database::ELECTRON_REAL_EFFICIENCY]) info += "electron real efficiencies, ";
    if(affects[Database::ELECTRON_FAKE_EFFICIENCY]) info += "electron fake efficiencies, ";
    if(affects[Database::ELECTRON_FAKE_FACTOR]) info += "electron fake factors, ";
    if(affects[Database::MUON_REAL_EFFICIENCY]) info += "muon real efficiencies, ";
    if(affects[Database::MUON_FAKE_EFFICIENCY]) info += "muon fake efficiencies, ";
    if(affects[Database::MUON_FAKE_FACTOR]) info += "muon fake factors, ";
    return info.substr(0, info.size()-2);
}

std::pair<uint16_t, float> BaseFakeBkgTool::identifyCpSystematicVariation(const CP::SystematicVariation& systematic) const
{
    if(!m_initialized || !m_database)
    {
        ATH_MSG_ERROR("This function can be called only once the tool has been initialized, since the number of systematic variations depends on the configuration...");
        throw std::logic_error("BaseFakeBkgTool::identifyCpSystematicVariation() called before initialization");
    }
    std::smatch smr;
    auto bn = systematic.basename();
    if(!std::regex_match(bn, smr, std::regex("FAKEBKG_(STAT|SYST)_VAR(\\d+)")))  return {{0}, 0.f};
    unsigned index = std::stol(smr[2].str());
    float sigma = systematic.parameter();
    if(smr[1].str() == "SYST") return {m_database->systIndexToUID(index), sigma};
    else return {m_database->statIndexToUID(index), sigma};
}

bool BaseFakeBkgTool::isAffectedBySystematic(const CP::SystematicVariation& systematic) const
{
    return identifyCpSystematicVariation(systematic).first;
}

CP::SystematicSet BaseFakeBkgTool::affectingSystematics() const
{
    if(!m_initialized || !m_database)
    {
        ATH_MSG_ERROR("This function can be called only once the tool has been initialized, since the number of systematic variations depends on the configuration...");
        throw std::logic_error("BaseFakeBkgTool::affectingSystematics() called before initialization");
    }
    CP::SystematicSet affecting;
    for(int step=0;step<2;++step)
    {
        std::string type = step? "STAT" : "SYST";
        const int imax = step? m_database->numberOfStats() : m_database->numberOfSysts();
        for(int i=0;i<imax;++i)
        {
            std::string name = "FAKEBKG_" + type + "_VAR" + std::to_string(i);
            affecting.insert(CP::SystematicVariation(name, 1.f));
            affecting.insert(CP::SystematicVariation(name, -1.f));
        }
    }
    return affecting;
}

CP::SystematicSet BaseFakeBkgTool::recommendedSystematics() const
{
    return affectingSystematics();
}

StatusCode BaseFakeBkgTool::applySystematicVariation(const CP::SystematicSet& systConfig)
{
    if(!m_initialized || !m_database)
    {
        ATH_MSG_ERROR("This function can be called only once the tool has been initialized");
        throw std::logic_error("BaseFakeBkgTool::applySystematicVariation() called before initialization");
    }
    if(m_lockedSystematicVariations)
    {
        ATH_MSG_ERROR("this particular method doesn't support multiple calls to applySystematicVariation(), or after the first call to addEvent(). Please use a separate instance of the tool for each variation.");
        throw std::logic_error("BaseFakeBkgTool::applySystematicVariation() called at a wrong time");
    }
    else if(!m_unlimitedSystematicVariations) m_lockedSystematicVariations = true; /// prevent further calls to applySystematicVariation() if not allowed
    if(!systConfig.size())
    {
        m_selectedUncertainties = nullptr;    
        return StatusCode::SUCCESS;
    }
    auto itr = m_systSetDict.find(systConfig);
    if(itr != m_systSetDict.end())
    {
        m_selectedUncertainties = &itr->second;
        return StatusCode::SUCCESS;
    }
    CP::SystematicSet mysys;
    auto sc = CP::SystematicSet::filterForAffectingSystematics(systConfig, affectingSystematics(), mysys);
    if(sc != StatusCode::SUCCESS) return sc;
    std::unique_ptr<UncertaintyList> uncertainties;
    for(auto& sysvar : mysys)
    {
        auto var = identifyCpSystematicVariation(sysvar);
        if(!var.first)
        {
            ATH_MSG_ERROR("The systematic variation " << sysvar.name() << " is not recognized, despite being present in affectingSystematics()...");
            return StatusCode::FAILURE;
        }
        if(!uncertainties) uncertainties.reset(new UncertaintyList(var.first, var.second));
        else uncertainties->extraVariation(var.first, var.second);
    }
    if(uncertainties)
    {
        auto emplaced = m_systSetDict.emplace(systConfig, std::move(*uncertainties));
        m_selectedUncertainties = &emplaced.first->second;
    }
    else m_selectedUncertainties = nullptr;
    return StatusCode::SUCCESS;
}

void BaseFakeBkgTool::printUncertaintyDescription(const CP::SystematicVariation& systematic) const
{
    auto info = getUncertaintyDescription(systematic);
    if(info.length()) ATH_MSG_INFO(info);
}

std::string BaseFakeBkgTool::getUncertaintyDescription(const CP::SystematicVariation& systematic) const
{
    if(!m_initialized || !m_database)
    {
        ATH_MSG_ERROR("This function can be called only once the tool has been initialized");
        return "";
    }
    
    auto UID = identifyCpSystematicVariation(systematic).first;
    if(!UID) 
    {
        ATH_MSG_WARNING("Systematic variation " + systematic.name() + " is not recognized by BaseFakeBkgTool");
        return "";
    }
    
    if(UID == 0)
    {
        return "Total statistical uncertainty in the event yield";
    }
    std::string info;
    auto stat = m_database->findStat(UID);
    if(stat)
    {
        info = "Statistical uncertainty affecting ";
        /// need to add more info (bin index, etc)
    }
    else
    {
        auto syst = m_database->findSyst(UID);
        if(syst)
        {
            info = "Systematic uncertainty \"" + syst->name + "\" affecting ";
        }
        else
        {
            ATH_MSG_ERROR("uncertainty with UID " << std::hex << UID << std::dec << " not found in database");
            return "";
        }
    }
    info += getListOfEfficienciesAffectedBy(UID);
    return info;
}

bool BaseFakeBkgTool::isSystematicUncertainty(const CP::SystematicVariation& systematic) const
{
    uint16_t UID = identifyCpSystematicVariation(systematic).first;
    if(UID) return Database::isSystUID(UID);
    return false;
}

bool BaseFakeBkgTool::isStatisticalUncertainty(const CP::SystematicVariation& systematic) const
{
    uint16_t UID = identifyCpSystematicVariation(systematic).first;
    if(UID) return Database::isStatUID(UID);
    return false;
}

bool BaseFakeBkgTool::affectsElectrons(const CP::SystematicVariation& systematic) const
{
    uint16_t UID = identifyCpSystematicVariation(systematic).first;
    if(UID) return getListOfEfficienciesAffectedBy(UID).find("electron") != std::string::npos;
    return false;
}

bool BaseFakeBkgTool::affectsMuons(const CP::SystematicVariation& systematic) const
{
    uint16_t UID = identifyCpSystematicVariation(systematic).first;
    if(UID) return getListOfEfficienciesAffectedBy(UID).find("muon") != std::string::npos;
    return false;
}

bool BaseFakeBkgTool::affectsTaus(const CP::SystematicVariation& systematic) const
{
    uint16_t UID = identifyCpSystematicVariation(systematic).first;
    if(UID) return getListOfEfficienciesAffectedBy(UID).find("tau") != std::string::npos;
    return false;
}

bool BaseFakeBkgTool::affectsRealEfficiencies(const CP::SystematicVariation& systematic) const
{
    uint16_t UID = identifyCpSystematicVariation(systematic).first;
    if(UID) return getListOfEfficienciesAffectedBy(UID).find("real eff") != std::string::npos;
    return false;
}

bool BaseFakeBkgTool::affectsFakeEfficiencies(const CP::SystematicVariation& systematic) const
{
    uint16_t UID = identifyCpSystematicVariation(systematic).first;
    if(UID) return getListOfEfficienciesAffectedBy(UID).find("fake eff") != std::string::npos;
    return false;
}

bool BaseFakeBkgTool::affectsFakeFactors(const CP::SystematicVariation& systematic) const
{
    uint16_t UID = identifyCpSystematicVariation(systematic).first;
    if(UID) return getListOfEfficienciesAffectedBy(UID).find("fake factor") != std::string::npos;
    return false;
}

CP::SystematicSet BaseFakeBkgTool::affectingSystematicsFor(const std::string& nuisanceParameter) const
{
    CP::SystematicSet affecting;
    if(!m_initialized || !m_database)
    {
        ATH_MSG_ERROR("This function can be called only once the tool has been initialized, since the number of systematic variations depends on the configuration...");
        return {};
    }
    bool up=true, down=true;
    auto pos1=nuisanceParameter.find("__up"), pos2=nuisanceParameter.find("__down");
    if(pos1!=std::string::npos && pos1+4==nuisanceParameter.length()) down = false;
    else if(pos2!=std::string::npos && pos2+6==nuisanceParameter.length()) up = false;
    std::string np = nuisanceParameter.substr(0, std::min(pos1, pos2));
    for(unsigned i=0;i<m_database->numberOfSysts();++i)
    {
        auto syst = m_database->findSyst(m_database->systIndexToUID(i));
        if(syst && syst->name==np)
        {
            std::string name = "FAKEBKG_SYST_VAR" + std::to_string(i);
            if(up) affecting.insert(CP::SystematicVariation(name, 1.f));
            if(down) affecting.insert(CP::SystematicVariation(name, -1.f));
        }
    }
    return affecting;
}
