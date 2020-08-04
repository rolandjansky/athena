/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "FakeBkgTools/BaseLinearFakeBkgTool.h"
#include "FakeBkgTools/FakeBkgInternals.h"
#include "FakeBkgTools/Database.h"
#include "PathResolver/PathResolver.h"

#ifndef FAKEBKGTOOLS_ATLAS_ENVIRONMENT
    #define declareProperty(n, p, h) ExtraPropertyManager<BaseLinearFakeBkgTool>::declareProperty(n, &BaseLinearFakeBkgTool::p, h)
#endif

#include "TTree.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"

using namespace FakeBkgTools;
using namespace CP;

namespace
{
    const char savedYieldsKey[] = "BaseLinearFakeBkgTool__yields"; 
    const char savedHistogramsKey[] = "BaseLinearFakeBkgTool__histograms";
}

BaseLinearFakeBkgTool::BaseLinearFakeBkgTool(const std::string toolname) : BaseFakeBkgTool(toolname), m_yields(1)
{
    /// Note: don't use the usual syntax declareProperty("", x = default, ""), it won't work for standalone compilation

    m_unlimitedSystematicVariations = true;
}

BaseLinearFakeBkgTool::~BaseLinearFakeBkgTool()
{
}

StatusCode BaseLinearFakeBkgTool::initialize()
{
    return BaseFakeBkgTool::initialize();
}

StatusCode BaseLinearFakeBkgTool::getEventWeight(float& weight, const std::string& selection, const std::string& process)
{
    FakeBkgTools::Weight w;
    auto sc = getEventWeightCommon(w, selection, process);
    weight = w.value(this);
    return sc;
}

StatusCode BaseLinearFakeBkgTool::getEventWeightCommon(FakeBkgTools::Weight& weight, const std::string& selection, const std::string& process)
{
    if(!m_initialized)
    {
         ATH_MSG_WARNING("the tool hasn't been initialized");
        return StatusCode::FAILURE;
    }
    weight.uncertainties.clear();
    bool success = true;
    auto fs = getCachedFinalState(m_particles.size(), selection, process, success);
    if(!success) return StatusCode::FAILURE;
    auto itr = m_cachedWeights.find(fs);
    if(itr != m_cachedWeights.end())
    {
        weight = itr->second;
        return StatusCode::SUCCESS;
    }
    auto sc = getEventWeightCustom(weight, fs);
    if(sc != StatusCode::SUCCESS) return sc;
    m_cachedWeights.emplace(fs, weight);
    return StatusCode::SUCCESS;
}

StatusCode BaseLinearFakeBkgTool::getTotalYield(float& yield, float& yieldStatErrorUp, float& yieldStatErrorDown)
{
    if(!m_initialized)
    {
         ATH_MSG_WARNING("the tool hasn't been initialized");
        return StatusCode::FAILURE;
    }
    if(m_progressFileName.length() && m_progressFileName != "none")
    {
        ATH_CHECK( mergeSubjobs() );
    }
    auto& total = m_yields.at(0);
    yield = total.value(this);
    yieldStatErrorUp = sqrt(total.stat2.up);
    yieldStatErrorDown = sqrt(total.stat2.down);
    for(auto& kv : m_histogramYieldsRange)
    {
        for(int i=0;i<kv.first->GetNcells();++i)
        {
            auto& yield = m_yields.at(kv.second.first + i);
            kv.first->SetBinContent(i, yield.value(this));
            kv.first->SetBinError(i, yield.stat());
        }
    }
    return StatusCode::SUCCESS;
}

StatusCode BaseLinearFakeBkgTool::incrementTotalYield()
{
    FakeBkgTools::Weight eventWeight;
    ATH_CHECK( getEventWeightCommon(eventWeight, m_selection, m_process) );
    /// note: we multiply by m_externalWeight which means that it is NOT included in m_eventWeight
    m_yields.at(0).add(eventWeight, m_externalWeight);
    for(auto& kv : m_values_1dhisto_map)
    {
        unsigned index = m_histogramYieldsRange.at(kv.first).first + kv.first->FindBin(*kv.second);
        m_yields.at(index).add(eventWeight, m_externalWeight);
    }
    for(auto& kv : m_values_2dhisto_map)
    {
        unsigned index = m_histogramYieldsRange.at(kv.first).first + kv.first->FindBin(*kv.second.first, *kv.second.second);
        m_yields.at(index).add(eventWeight, m_externalWeight);
    }
    for(auto& kv : m_values_3dhisto_map)
    {
      unsigned index = m_histogramYieldsRange.at(kv.first).first + kv.first->FindBin(*std::get<0>(kv.second), *std::get<1>(kv.second), *std::get<2>(kv.second));
        m_yields.at(index).add(eventWeight, m_externalWeight);
    }
    return StatusCode::SUCCESS;
}

StatusCode BaseLinearFakeBkgTool::register1DHistogram(TH1* h1, const float *val)
{
    auto sc = BaseFakeBkgTool::register1DHistogram(h1, val);
    if(sc != StatusCode::SUCCESS) return sc;
    return assignYieldRange(h1);
}

StatusCode BaseLinearFakeBkgTool::register2DHistogram(TH2* h2, const float *xval, const float *yval)
{
    auto sc = BaseFakeBkgTool::register2DHistogram(h2, xval, yval);
    if(sc != StatusCode::SUCCESS) return sc;
    return assignYieldRange(h2);
}

StatusCode BaseLinearFakeBkgTool::register3DHistogram(TH3* h3, const float *xval, const float *yval, const float *zval)
{
  auto sc = BaseFakeBkgTool::register3DHistogram(h3, xval, yval, zval);
    if(sc != StatusCode::SUCCESS) return sc;
    return assignYieldRange(h3);
}

StatusCode BaseLinearFakeBkgTool::assignYieldRange(TH1* h)
{
    const std::string histname = h->GetName();
    std::pair<uint32_t, uint32_t> range;
    for(auto itr=m_histogramYieldsRange.begin();itr!=m_histogramYieldsRange.end();++itr)
    {
        if(histname != itr->first->GetName()) continue;
        range = itr->second;
        m_histogramYieldsRange.erase(itr);
        m_histogramYieldsRange.emplace(h, range);
        return StatusCode::SUCCESS;
    }
    range.first = m_yields.size();
    m_yields.insert(m_yields.end(), std::max(1, h->GetNcells()), FakeBkgTools::Yield{});
    range.second = m_yields.size() - 1;
    m_histogramYieldsRange.emplace(h, range);
    return StatusCode::SUCCESS;
}

StatusCode BaseLinearFakeBkgTool::saveProgress(TDirectory* dir)
{
    TTree tree(::savedYieldsKey, "binned yields saved by BaseLinearFakeBkgTool");
    TObjArray histograms;
    
    ULong64_t histname = 0;
    UInt_t bin=0, n=0;
    float nominal, stat2Up, stat2Down;
    const std::size_t maxSize = m_database->numberOfStats() + m_database->numberOfSysts();
    std::unique_ptr<UShort_t[]> systUID(new UShort_t[maxSize]);
    std::unique_ptr<float[]> systUp(new float[maxSize]), systDown(new float[maxSize]);
    tree.Branch("Name", &histname, "Name/l");
    tree.Branch("Bin", &bin, "Bin/i");
    tree.Branch("nominal", &nominal, "nominal/F");
    tree.Branch("stat2Up", &stat2Up, "stat2Up/F");
    tree.Branch("stat2Down", &stat2Down, "stat2Down/F");
    tree.Branch("N", &n, "N/i");
    tree.Branch("systUID", systUID.get(), "systUID[N]/s");    
    tree.Branch("systUp", systUp.get(), "systUp[N]/F");
    tree.Branch("systDown", systDown.get(), "systDown[N]/F");
    
    auto fillTree = [&](const auto& yield)
    {
        nominal = yield.nominal;
        stat2Up = yield.stat2.up;
        stat2Down = yield.stat2.down;
        n = yield.uncertainties.size();
        auto itr = yield.uncertainties.begin();
        for(uint32_t j=0;j<n;++j)
        {
            systUID[j] = itr->first;
            systUp[j] = itr->second.up;
            systDown[j] = itr->second.down;
            ++itr;
        }
        tree.Fill();
    };
    
    fillTree(m_yields.at(0));

    std::hash<std::string> hasher;
    for(auto& kv : m_histogramYieldsRange)
    {
        histograms.AddLast(kv.first);
        histname = hasher(kv.first->GetName());
        for(uint32_t i=kv.second.first;i<=kv.second.second;++i)
        {
            bin = i - kv.second.first;
            fillTree(m_yields.at(i));
        }
    }
    
    dir->cd();
    tree.Write();
    dir->mkdir(::savedHistogramsKey);
    dir->cd(::savedHistogramsKey);
    histograms.Write();
    dir->cd("..");    
    return StatusCode::SUCCESS;
}

StatusCode BaseLinearFakeBkgTool::mergeSubjobs()
{
    std::string filename = PathResolverFindDataFile(m_progressFileName);

    auto file = std::unique_ptr<TFile>(TFile::Open(filename.c_str(), "READ"));
    if(!file)
    {
        ATH_MSG_ERROR("Unable to open the file specified for the \"ProgressFileName\" property: " << m_progressFileName);
        return StatusCode::FAILURE;
    }
    std::string path = ::savedYieldsKey;
    if(m_progressFileDirectory.length())
    {
       path = m_progressFileDirectory + "/" + path;
    }
    auto tree = static_cast<TTree*>(file->Get(path.c_str()));
    if(!tree)
    {
        ATH_MSG_ERROR("Unable to find the tree \"" << path << "\" in the input file " << m_progressFileName);
        return StatusCode::FAILURE;
    }
    
    /// To do: check that the registered histograms are compatible with the saved ones
    /// for that, will just need to list the histograms in the directory and call register1/2DHistogram(), where the checks are made.
    
    std::hash<std::string> hasher;
    std::map<std::size_t, decltype(m_histogramYieldsRange.cbegin())> dictionary;
    for(auto itr=m_histogramYieldsRange.cbegin();itr!=m_histogramYieldsRange.cend();++itr)
    {
        dictionary[hasher(itr->first->GetName())] = itr;
    }
    
    ULong64_t histname = 0;
    UInt_t bin=0, n=0;
    float nominal, stat2Up, stat2Down;
    const std::size_t maxSize = m_database->numberOfStats() + m_database->numberOfSysts();
    std::unique_ptr<UShort_t[]> systUID(new UShort_t[maxSize]);
    std::unique_ptr<float[]> systUp(new float[maxSize]), systDown(new float[maxSize]);
    tree->SetBranchStatus("*", kTRUE);
    tree->SetBranchAddress("Name", &histname);
    tree->SetBranchAddress("Bin", &bin);
    tree->SetBranchAddress("nominal", &nominal);
    tree->SetBranchAddress("stat2Up", &stat2Up);
    tree->SetBranchAddress("stat2Down", &stat2Down);
    tree->SetBranchAddress("N", &n);
    TBranch* branch = tree->FindBranch("N");
    tree->SetBranchAddress("systUID", systUID.get());    
    tree->SetBranchAddress("systUp", systUp.get());
    tree->SetBranchAddress("systDown", systDown.get());
    
    std::set<std::size_t> filledHistograms;
    const long entries = tree->GetEntries();
    for(long entry=0;entry<entries;++entry)
    {
        branch->GetEntry(entry);
        if(n > maxSize)
        {
            ATH_MSG_ERROR("the tool configuration seems to have changed (number of systematics)!");
            return StatusCode::FAILURE;
        }
        tree->GetEntry(entry);
        
        long index;
        if(histname != 0)
        {
            auto itr = dictionary.find(histname);
            if(itr == dictionary.end()) continue; /// histogram not registered? don't load the data
            filledHistograms.emplace(histname);
            const TH1* h = itr->second->first;
            const auto& range = itr->second->second;
            if(bin > (range.second - range.first))
            {
                ATH_MSG_ERROR("inconsistent binnings found for histogram " << h->GetName());
                return StatusCode::FAILURE;
            }
            index = range.first + bin;
        }
        else
        {
            index = 0;
        }
        
        FakeBkgTools::Yield yield;
        yield.nominal = nominal;
        yield.stat2.up = stat2Up;
        yield.stat2.down = stat2Down;
        for(unsigned j=0;j<n;++j)
        {
            FakeBkgTools::Uncertainty u;
            u.up = systUp[j];
            u.down= systDown[j];
            auto emplaced = yield.uncertainties.emplace(systUID[j], u);
            if(!emplaced.second) emplaced.first->second += u;
        }
        m_yields.at(index).add(yield);
    }
    
    if(filledHistograms.size() != m_histogramYieldsRange.size())
    {
        ATH_MSG_ERROR("some of the registered histogram were not found in the input file " << m_progressFileName);
        return StatusCode::FAILURE;
    }
    
    m_progressFileName.clear();
    return StatusCode::SUCCESS;
}
