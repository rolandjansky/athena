/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifdef XAOD_STANDALONE
    #include "xAODRootAccess/Init.h"
    #include "xAODRootAccess/TEvent.h"
    #include "xAODRootAccess/TStore.h"
#else
    #include "AthAnalysisBaseComps/AthAnalysisHelper.h"
    #include "POOLRootAccess/TEvent.h"
#endif

#include "AsgTools/AnaToolHandle.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEventInfo/EventAuxInfo.h"
#include "xAODEgamma/Electron.h"
#include "xAODMuon/Muon.h"
#include "AthContainers/AuxElement.h"
#include "AsgTools/AsgMessaging.h"
#include "AsgAnalysisInterfaces/IFakeBkgTool.h"
#include "AsgAnalysisInterfaces/ILinearFakeBkgTool.h"

#include "TError.h"
#include "TFile.h"
#include "TH1F.h"
#include "TSystem.h"

#include <fstream>
#include <map>
#include <memory>

bool successful(bool arg) { return arg; }
bool successful(int arg);
bool successful(const StatusCode& arg) { return arg == StatusCode::SUCCESS; }
bool successful(const CP::SystematicCode& arg) { return arg == CP::SystematicCode::Ok; }

#define FBT_CHECK(x) \
    while(true) { \
        if(successful(x)) break; \
        Error("fbtTestBasics", "%s", ("failure encounted on l." + std::to_string(__LINE__)).c_str()); \
        return false; \
    }

struct Result
{
    float value = 0, statUp = 0, statDown = 0;
    std::map<CP::SystematicVariation, float> variations;
    bool operator==(const Result&) const;
    void Print() const;
};
    
bool allTests();
bool minimalTest(const std::string& type, Result& result);
bool parallelJob(const std::string& type, const std::string& saveAs);
bool readFromROOT();
bool readFromXML();

template<class Interface = CP::IFakeBkgTool> bool setup(asg::AnaToolHandle<Interface>& tool, const std::string& type);
template<class Interface = CP::IFakeBkgTool> bool eventLoop(asg::AnaToolHandle<Interface>& tool, Result& result);
bool addEventWeight(asg::AnaToolHandle<CP::ILinearFakeBkgTool>& tool, Result& result);
bool addEventWeight(asg::AnaToolHandle<CP::IFakeBkgTool>& tool, Result& result);
template<class Interface = CP::IFakeBkgTool> bool fillResult(asg::AnaToolHandle<Interface>& tool, Result& result);

#ifdef XAOD_STANDALONE
  std::unique_ptr<xAOD::TStore> store;
#else
  StoreGateSvc* store = nullptr;
#endif

std::vector<std::string> config;
std::string selection = ">=1T";    
std::string process = ">=1F[T]";
bool readCPVariations = true;
int eventOffset = 0;
int nEvents = 48;
std::string progressFile = "";

bool verbose = false;

int main(int argc, char* argv[])
{
    for(int i=1;i<argc;++i)
    {
        std::string option = argv[i];
        if(option=="-v" || option=="--verbose") verbose = true;
    }
    
  #ifdef XAOD_STANDALONE
    xAOD::Init("fbtTestBasics").ignore();
    xAOD::TEvent event(xAOD::TEvent::kClassAccess);
    store = std::make_unique<xAOD::TStore>();
    StatusCode::enableFailure();
  #else
    IAppMgrUI* app = POOL::Init();
    POOL::TEvent event(POOL::TEvent::kClassAccess);
	FBT_CHECK( event.evtStore().retrieve() );
	store = event.evtStore().get();
  #endif
    
    int returnCode = allTests() ? 0 : 1;
    
  #ifndef XAOD_STANDALONE
    app->finalize();
  #endif
    return returnCode;
}

bool allTests()
{
    
  Result result0, result1, result2, result3, result4, result5;
    
    if(verbose) std::cout <<"\nWill do minimal test with CP::ApplyFakeFactor\n";
    FBT_CHECK( readFromROOT() );
    FBT_CHECK( minimalTest("CP::ApplyFakeFactor", result0) );
    
    if(verbose) std::cout <<"\nWill do minimal test with CP::AsymptMatrixTool\n";
    FBT_CHECK( minimalTest("CP::AsymptMatrixTool", result1) );
    
    if(verbose) std::cout <<"\nWill do minimal test with CP::LhoodMM_tools\n";
    FBT_CHECK( minimalTest("CP::LhoodMM_tools", result2) );
    
    if(verbose) std::cout <<"\nWill test loading efficiencies from XML\n";
    FBT_CHECK( readFromXML() );
    FBT_CHECK( minimalTest("CP::AsymptMatrixTool", result3) );
    FBT_CHECK( readFromROOT() );
    FBT_CHECK( result1 == result2 );
    
    if(verbose) std::cout <<"\nWill test getEventWeight()\n";
    {
        asg::AnaToolHandle<CP::ILinearFakeBkgTool> tool;
        FBT_CHECK( setup(tool, "CP::AsymptMatrixTool") );
        FBT_CHECK( eventLoop(tool, result4) );
        if(verbose) result3.Print();
    }
    FBT_CHECK( result1 == result3 );
    
    if(verbose) std::cout <<"\nWill test parallel jobs and merging\n";
    eventOffset = 0;
    nEvents = 16;
    FBT_CHECK( parallelJob("CP::LhoodMM_tools", "/tmp/fbt_job1.root") );
    eventOffset = 16;
    FBT_CHECK( parallelJob("CP::LhoodMM_tools", "/tmp/fbt_job2.root") );
    eventOffset = 32;
    FBT_CHECK( parallelJob("CP::LhoodMM_tools", "/tmp/fbt_job3.root") );
    eventOffset = 0;
    FBT_CHECK( gSystem->Exec("hadd -f /tmp/fbt_merged.root /tmp/fbt_job*.root") == 0 );
    {
        asg::AnaToolHandle<CP::IFakeBkgTool> tool;
        progressFile = "/tmp/fbt_merged.root";
        FBT_CHECK( setup(tool, "CP::LhoodMM_tools") );
        progressFile.clear();
        FBT_CHECK( fillResult(tool, result4) );
    }
    FBT_CHECK(result2 == result4 );
    
    return true;
}

bool minimalTest(const std::string& type, Result& result)
{
    asg::AnaToolHandle<CP::IFakeBkgTool> tool;
    FBT_CHECK( setup(tool, type) );
    FBT_CHECK( eventLoop(tool, result) );
    FBT_CHECK( fillResult(tool, result) );
    return true;
}

bool parallelJob(const std::string& type, const std::string& saveAs)
{
    asg::AnaToolHandle<CP::IFakeBkgTool> tool;
    FBT_CHECK( setup(tool, type) );
    Result result;
    FBT_CHECK( eventLoop(tool, result) );
    std::unique_ptr<TFile> f(TFile::Open(saveAs.c_str(), "RECREATE"));
    FBT_CHECK( !!f );
    FBT_CHECK( tool->saveProgress(f->mkdir("fakes")) );
    f->Close();
    if(verbose)
    {
        float value, up, down;
        FBT_CHECK( tool->getTotalYield(value, up, down) );
        std::cout << "Parallel job stored intermediate yield " << value << " +" << up << " -" << down << std::endl;
    }
    return true;
}

static long instance = 0; /// mostly useful for athena, which will otherwise re-use the previous tool

template<class Interface>
bool setup(asg::AnaToolHandle<Interface>& tool, const std::string& type)
{
    tool = asg::AnaToolHandle<Interface>(type + "/FBT" + std::to_string(++instance));
    FBT_CHECK( tool.setProperty("InputFiles", config) );
    FBT_CHECK( tool.setProperty("EnergyUnit", "GeV") );
    FBT_CHECK( tool.setProperty("ConvertWhenMissing", true) );
    FBT_CHECK( tool.setProperty("Selection", selection) );
    FBT_CHECK( tool.setProperty("Process", process) );
    if(progressFile.size())
    {
        FBT_CHECK( tool.setProperty("ProgressFileName", progressFile) );
        FBT_CHECK( tool.setProperty("ProgressFileDirectory", "fakes") );
    }
    FBT_CHECK( tool.initialize() );
    return true;
    
}

template<class Interface>
bool eventLoop(asg::AnaToolHandle<Interface>& tool, Result& result)
{
    auto eventInfo = std::make_unique<xAOD::EventInfo>();
    auto eventAuxInfo = std::make_unique<xAOD::EventAuxInfo>();
    eventInfo->setStore(eventAuxInfo.get());
    eventInfo->auxdata<int>("flag") = 1;
    FBT_CHECK( store->record(std::move(eventInfo), "EventInfo") );
    FBT_CHECK( store->record(std::move(eventAuxInfo), "EventInfoAux.") );
 
    xAOD::IParticleContainer particles(SG::VIEW_ELEMENTS);
    auto e = std::make_unique<xAOD::Electron>();
    e->makePrivateStore();
    e->setCharge(1);
    particles.push_back(static_cast<xAOD::IParticle*>(&*e));
    for(int i=eventOffset;i<nEvents+eventOffset;++i)
    {
        e->setP4((1 + (i%3))*1e4, 0., 0. ,0.511);
        e->auxdecor<char>("Tight") = (i%4)? 0 : 1;
        FBT_CHECK( tool->addEvent(particles) );
        FBT_CHECK( addEventWeight(tool, result) );
    }
	
  #ifdef XAOD_STANDALONE
    store->clear();
  #else
    store->clearStore(true);
  #endif
    return true;
}

bool addEventWeight(asg::AnaToolHandle<CP::ILinearFakeBkgTool>& tool, Result& result)
{
    FBT_CHECK( tool->applySystematicVariation({}) );
    float y;
    FBT_CHECK( tool->getEventWeight(y, selection, process) );
    result.value += y;
    result.statUp = sqrt(pow(result.statUp, 2) + y*y);
    result.statDown = sqrt(pow(result.statDown, 2) + y*y);
    for(auto& sysvar : tool->affectingSystematics())
    {
        FBT_CHECK( tool->applySystematicVariation({sysvar}) );
        FBT_CHECK( tool->getEventWeight(y, selection, process) );
        result.variations[sysvar] += y;
    }
    return true;
}

bool addEventWeight(asg::AnaToolHandle<CP::IFakeBkgTool>&, Result&)
{
    return true;
}

template<class Interface>
bool fillResult(asg::AnaToolHandle<Interface>& tool, Result& result)
{
    result.variations.clear();
    FBT_CHECK( tool->getTotalYield(result.value, result.statUp, result.statDown) );
    if(readCPVariations)
    {
        for(auto& sysvar : tool->affectingSystematics())
        {
            FBT_CHECK( tool->applySystematicVariation({sysvar}) );
            float unused;
            FBT_CHECK( tool->getTotalYield(result.variations[sysvar], unused, unused) );
        }
    }
    if(verbose) result.Print();
    return true;
}

bool Result::operator==(const Result& rhs) const
{
    auto compare = [&](auto x1, auto x2) -> bool
    {
        bool closeEnough = std::fabs(x1 - x2) <  std::max(1e-5, 1e-3 * std::min(std::fabs(x1), std::fabs(x2)));;
        if(!closeEnough)
        {
            std::cout << "ERROR: found different values for the equality test: " << x1 << " vs " << x2 << std::endl;
        }
        return closeEnough; 
    };
    FBT_CHECK( compare(value, rhs.value) ); 
    FBT_CHECK( compare(statUp, rhs.statUp) ); 
    FBT_CHECK( compare(statDown, rhs.statDown) );
    FBT_CHECK( variations.size() == rhs.variations.size() );
    for(auto& kv : variations)
    {
        auto itr = rhs.variations.find(kv.first);
        FBT_CHECK( itr != rhs.variations.end() );
        FBT_CHECK( compare(kv.second, itr->second) );
    }
    return true;
}

void Result::Print() const
{
    std::cout << "Result: total yield = " << value << " +" << statUp << " -" << statDown << std::endl;
    for(auto& kv : variations)
    {
        std::cout << "   variation " << kv.first.name() << " = " << kv.second << std::endl;
    }
}

bool readFromROOT()
{
    config.clear();
    config.emplace_back("/tmp/fbt_efficiencies.root");
    TH1D hElFake("FakeEfficiency_el_pt","FakeEfficiency", 1, 10., 100.);
    hElFake.SetBinContent(1, 0.05);
    hElFake.SetBinError(1, 0.01);
    TH1D hMuFake("FakeEfficiency_mu_pt","FakeEfficiency", 1, 10., 100.);
    hMuFake.SetBinContent(1, 0.15);
    hMuFake.SetBinError(1, 0.032);
    TH1D hElReal("RealEfficiency_el_pt","RealEfficiency", 1, 10., 100.);
    hElReal.SetBinContent(1, 0.90);
    hElReal.SetBinError(1, 0.01);
    TH1D hMuReal("RealEfficiency_mu_pt","RealEfficiency", 1, 10., 100.);
    hMuReal.SetBinContent(1, 0.95);
    hMuReal.SetBinError(1, 0.01);
    std::unique_ptr<TFile> f(TFile::Open(config.back().c_str(), "RECREATE"));
    FBT_CHECK( !!f );
    f->cd();
    hElFake.Write();
    hElReal.Write();
    hMuFake.Write();
    hMuReal.Write();
    f->Close();
    return true;
}

bool readFromXML()
{
    config.clear();
    config.emplace_back("/tmp/fbt_efficiencies.xml");
    std::ofstream out(config.back().c_str(), std::ios_base::out);
    FBT_CHECK( out.is_open() );
    /// note: the declarations must be placed in the "good" order,
    /// otherwise the definitions of the SystematicVariations will be different than those obtained with readFromROOT()
    out << "<efficiencies>\n";
    out << "<param type=\"int\" level=\"event\"> flag </param>\n";
    out << "<electron type=\"fake-efficiency\" input=\"central-value\" stat=\"per-bin\" >\n\t<bin flag=\"1\">\n 0.05 +- 0.01 (stat) </bin>\n</electron>\n";
    out << "<electron type=\"real-efficiency\" input=\"central-value\" stat=\"global\" >\n\t<bin flag=\"1\">\n 0.90 +- 0.01 (stat) </bin>\n</electron>\n";
    out << "<muon type=\"fake-efficiency\" input=\"central-value\" stat=\"per-bin\" >\n\t<bin flag=\"1\">\n 0.15 +- 0.032 (stat) </bin>\n</muon>\n";
    out << "<muon type=\"real-efficiency\" input=\"central-value\" stat=\"global\" >\n\t<bin flag=\"1\">\n 0.95 +- 0.01 (stat) </bin>\n</muon>\n";
    out << "</efficiencies>\n";
    out.close();
    return true;
}
