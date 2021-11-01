#ifdef XAOD_STANDALONE
  #include "xAODRootAccess/Init.h"
  #include "xAODRootAccess/TEvent.h"
  #include "xAODRootAccess/TStore.h"
#endif
#include "CaloGeoHelpers/CaloSampling.h"
#include "AsgTools/AnaToolHandle.h"
#include "EgammaAnalysisInterfaces/IAsgElectronEfficiencyCorrectionTool.h"
#include "ElectronEfficiencyCorrection/AsgElectronEfficiencyCorrectionTool.h"
#include "AsgTools/MessageCheck.h"
#include "CreateDummyEl.h"

#include <map>
#include <tuple>
#include <bitset>
#include <sstream>
#include <type_traits>
#include <cstdlib>
#include <limits>
#include <utility>

#define MSGSOURCE "dumpNPs"

struct Domain
{
  float etamin, etamax, ptmin, ptmax;
  std::string str(bool abs_eta=false) const;
};

struct NP
{
  int index;
  bool uncorr;
  bool operator<(const NP& rhs) const { 
    return (uncorr!=rhs.uncorr)? (uncorr<rhs.uncorr): (index<rhs.index);
  }
  bool operator==(const NP& rhs) const { 
    return (index == rhs.index) && (uncorr == rhs.uncorr);
  }
};

struct Config
{
  using T = IAsgElectronEfficiencyCorrectionTool;
  asg::AnaToolHandle<T> tool{"AsgElectronEfficiencyCorrectionTool/tool"};
  std::vector<std::tuple<CP::SystematicSet, NP>> systematics{};
  std::vector<Domain> domains{};
  int run_number{0};
  bool analysis_mode{false};
  bool initialize(int argc, char* argv[]);
};

using map_t = std::map<NP, std::vector<int>>;

template<typename... Args> bool parse_csv_list(std::string val, Args&... args);
bool scanPhaseSpace(Config& config, map_t& affected_bins);
bool displayFindings(Config& config, map_t& affected_bins);
bool displayFindings_analysis(Config& cfg, map_t& affected_bins);
bool find_boundaries(const Config& cfg, std::vector<int>& affected_bins, Domain& bounds, bool& abs_eta, bool& holes);
int get_run_number(int year);

std::vector<float> eta_edges = { // eta<0 bins are auto-added in main()
  0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.15, 1.2, 1.3, 
  1.37, 1.52, 1.6, 1.65, 1.7, 1.8, 1.9, 2.0, 2.1, 2.2, 2.3, 2.37, 2.4, 2.47
};

std::vector<float> pt_edges = { // in MeV
    45e2, 7e3, 1e4, 15e3, 2e4, 25e3, 3e4, 35e3, 4e4, 45e3, 5e4, 6e4, 8e4, 15e4, 25e4, 5e5, 1e7
};

// number of equidistant scanned points within a bin:
const int subdiv_eta{5};
const int subdiv_pt{3};


int main(int argc, char* argv[])
{
  using namespace asg::msgUserCode;

  #ifdef XAOD_STANDALONE
    StatusCode::enableFailure();
    ANA_CHECK(xAOD::Init());
    xAOD::TEvent event{xAOD::TEvent::kClassAccess};
  #endif

  CP::CorrectionCode::enableFailure();

  // Add bins for eta < 0, if needed.
  for(float eta : eta_edges) {
    if(eta > 0) eta_edges.push_back(-eta); // back, so loop iterator not invalidated.
  }
  std::sort(eta_edges.begin(), eta_edges.end());
  std::sort(pt_edges.begin(), pt_edges.end());
  if(std::adjacent_find(eta_edges.begin(), eta_edges.end()) != eta_edges.end() ||
      std::adjacent_find(pt_edges.begin(), pt_edges.end()) != pt_edges.end()) {
    Error(MSGSOURCE, "Duplicated values in the specified bin edges.");
    std::abort();
  }

  // Setup tool using command-line arguments
  Config cfg;
  ANA_CHECK( cfg.initialize(argc, argv) );
  ANA_CHECK( cfg.tool.initialize() );

  // List nuisance parameters for systematics
  auto allsysts = cfg.tool->recommendedSystematics();
  try {
    std::set<std::string> recorded;
    for(auto& var : allsysts)
    {
      std::string name = var.basename();
      if(!recorded.emplace(name).second) continue;
      NP np;
      if(name.find("UncorrUncertainty") != std::string::npos) np.uncorr = true;
      else if(name.find("CorrUncertainty") != std::string::npos) np.uncorr = false;
      else throw std::invalid_argument("");
      np.index = std::stoul(name.substr(name.find("NP") + 2));
      cfg.systematics.emplace_back(CP::SystematicSet{var}, np);
    }
  } catch(std::invalid_argument&) {
    Error(MSGSOURCE, "The pattern of the SystematicVariation names seems to have changed.");
    std::abort();
  }
  std::sort(cfg.systematics.begin(), cfg.systematics.end(),
    [](auto& x, auto& y) { return std::get<NP>(x) < std::get<NP>(y); });

  if(cfg.analysis_mode) {
    for(int year : {2015, 2016, 2017, 2018}) {
       map_t affected_bins;
       cfg.run_number = get_run_number(year);
      ANA_CHECK( scanPhaseSpace(cfg, affected_bins) );
      Info(MSGSOURCE, "%s", "");
      Info(MSGSOURCE, "Relevant nuisance parameters for the year %i:", year);
      ANA_CHECK( displayFindings_analysis(cfg, affected_bins) );
    }
  } else {
    map_t affected_bins;
    ANA_CHECK( scanPhaseSpace(cfg, affected_bins) );
    ANA_CHECK( displayFindings(cfg, affected_bins) );
  }
 
  return 0;
}

bool Config::initialize(int argc, char* argv[])
{
  using namespace asg::msgUserCode;

  auto help = [&]() {
    Info(MSGSOURCE, "Usage (1): EgEfficiencyCorr_dumpNPs Year=<value> IdKey=<value> <property1=value1> ... <property N=value N>");
    Info(MSGSOURCE, "   ++ prints eta x pt region of influence of each nuisance parameter for the chosen year");
    Info(MSGSOURCE, "Usage (2): EgEfficiencyCorr_dumpNPs AnalysisEta=<list> AnalysisPt=<list> IdKey=<value> <property1=value1> ... <property N=value N>");
    Info(MSGSOURCE, "   ++ prints a short list of all NPs relevant in the selected eta x pt (in MeV) region");
    Info(MSGSOURCE, "   ++ e.g. EgEfficiencyCorr_dumpNPs AnalysisEta=-2.0..2.0,nocrack AnalysisPt=20e3..150e3");
    Info(MSGSOURCE, "   ++ one of the Analysis* arguments can be omitted, in which case the full range is scanned");
    Info(MSGSOURCE, "   ++ several intervals can be specified, separated by commas, e.g. AnalysisEta=0..0.6,0.8..1.37");
    Info(MSGSOURCE, "   ++ recognized aliases: 'nocrack' (crack veto), 'sym' (duplicates for eta<0), 'barrel', 'endcap'");
    Info(MSGSOURCE, "Common details:");
    Info(MSGSOURCE, "   ++ for other SFs, IdKey can be replaced by RecoKey/IsoKey/TriggerKey");
    Info(MSGSOURCE, "   ++ CorrectionFileNameList can also be used instead of keys");
    Info(MSGSOURCE, "   ++ optional arguments are other configurable properties of the AsgElectronEfficiencyCorrectionTool.");
    Info(MSGSOURCE, "   ++ e.g. EgEfficiencyCorr_dumpNPs Year=2015 IdKey=Tight CorrelationModel=SIMPLIFIED UncorrEtaBinsUser=0,1.37,2.47");
    std::abort();
  };

  if(argc < 2) help();

  std::set<std::string> string_properties{"MapFilePath", "RecoKey", 
    "IdKey", "IsoKey", "TriggerKey", "CorrelationModel"};
  std::set<std::string> int_properties{"ForceDataType"};
  std::set<std::string> vector_properties{"UncorrEtaBinsUser", "UncorrEtBinsUser"};
  
  // Set default values.
  const int mc_type{static_cast<int>(PATCore::ParticleDataType::Full)};
  ANA_CHECK( tool.setProperty("ForceDataType", mc_type) );
  ANA_CHECK( tool.setProperty("CorrelationModel", "FULL") );
  ANA_CHECK( tool.setProperty("UseRandomRunNumber", true) );

  run_number = 0;
  std::vector<char> eta_flags(eta_edges.size()-1, 0);
  std::vector<char> pt_flags(pt_edges.size()-1, 0);

  for(int i=1;i<argc;++i) {
    std::string key{argv[i]};
    std::size_t pos = key.find('=');
    if(pos == std::string::npos) help();
    std::string val = key.substr(pos + 1);
    key = key.substr(0, pos);
    if(!key.length() || !val.length()) help();
    if(key == "CorrectionFileNameList") {
      std::vector<std::string> files{val};
      ANA_CHECK( tool.setProperty(key, files) );
    } else if(key == "Year") {
      run_number = get_run_number(std::stoi(val));
    } else if(string_properties.count(key)) {
      ANA_CHECK( tool.setProperty(key, val) );
    } else if(int_properties.count(key)) {
      ANA_CHECK( tool.setProperty(key, std::stoi(val)) );
    } else if(vector_properties.count(key)) {
      std::vector<float> bins;
      ANA_CHECK( parse_csv_list(val, bins) );
      ANA_CHECK( tool.setProperty(key, bins) );
    } else if(key == "AnalysisEta") {
      ANA_CHECK( parse_csv_list(val, eta_flags, eta_edges) );
      analysis_mode = true;
    } else if(key == "AnalysisPt") {
      ANA_CHECK( parse_csv_list(val, pt_flags, pt_edges) );
      analysis_mode = true;
    } else {
      Error(MSGSOURCE, "Unrecognized property %s", key.c_str());
      help();
    }
  }

  if(analysis_mode) {
    if(run_number) {
      Warning(MSGSOURCE, "When Analysis* arguments are provided, Year is ignored.");
    }
  } else if(!run_number) {
    Error(MSGSOURCE, "Command-line arguments must include Year");
    help();
    std::abort();
  }

  // Enable negative-eta bins, when the 'sym' alias was specified:
  for(std::size_t i=0;i<eta_flags.size();++i) {
    if(eta_flags[i] != 2) continue;
    float z = 0.5 * (eta_edges[i] + eta_edges[i+1]);
    auto eeb{eta_edges.cbegin()}, eee{eta_edges.cend()};
    std::size_t j = std::upper_bound(eeb, eee, -z) - eee - 1;
    if(i == j) continue;
    if(j >= eta_edges.size()) return false;
    eta_flags[i] = eta_flags[j];
  }

  // By default, scan whole ranges (except crack if vetoed)
  auto efb{eta_flags.begin()}, efe{eta_flags.end()};
  if(!std::count(efb, efe, 1)) {
    std::replace(efb, efe, 0, 1);
  }
  auto pfb{pt_flags.begin()}, pfe{pt_flags.end()};
  if(!std::count(pfb, pfe, 1)) {
    std::fill(pfb, pfe, 1);
  }

  domains.clear();
  for(std::size_t i=0;i<eta_flags.size();++i) {
    if(eta_flags[i] < 1) continue;
    for(std::size_t j=0;j<pt_flags.size();++j) {
      if(pt_flags[j] < 1) continue;
      domains.emplace_back(Domain{
        eta_edges[i], eta_edges[i+1],
        pt_edges[j], pt_edges[j+1]
      });
    }
  }
  return true;
}

bool scanPhaseSpace(Config& cfg, map_t& affected_bins)
{
  using namespace asg::msgUserCode;
  
  static int display_count{-1};
  if(!++display_count) {
    Info(MSGSOURCE, "Scanning the eta/pt plane, this will take a bit of time...");
  }
 
  xAOD::TStore store;

  using RealTool = const AsgElectronEfficiencyCorrectionTool;
  auto asgtool = dynamic_cast<RealTool*>(&*cfg.tool);
  const int jmax{subdiv_eta + 1}, kmax{subdiv_pt + 1};
  const int dmax{static_cast<int>(cfg.domains.size())};
  for(int d=0;d<dmax;++d) {
    auto& dom = cfg.domains[d];
    std::bitset<1024> zero_uncorr{}, nonzero_uncorr{}, expected_uncorr{};
    std::bitset<256> zero_corr{}, nonzero_corr{};
    bool multi_np{false};
    for(std::size_t j=1;j<jmax;++j) {
      float eta = ((jmax-j)*dom.etamin + j*dom.etamax) / jmax;
      for(std::size_t k=1;k<kmax;++k) {
        float pt = ((kmax-k)*dom.ptmin + k*dom.ptmax) / kmax;
        ANA_CHECK( getElectrons({{pt, eta}}, cfg.run_number, store) );
        const xAOD::ElectronContainer* electrons{nullptr};
        ANA_CHECK( store.retrieve(electrons, "MyElectrons") );
        const xAOD::Electron& electron = *electrons->at(0);

        double sf_nom{0}, sf{0};
        ANA_CHECK( cfg.tool->applySystematicVariation({}) );
        ANA_CHECK( cfg.tool->getEfficiencyScaleFactor(electron, sf_nom) );
        int vi = asgtool->systUncorrVariationIndex(electron);
        expected_uncorr.set(vi);
        int n_nonzero_uncorr{0};
        for(auto& s : cfg.systematics) {
          auto& sys = std::get<CP::SystematicSet>(s);
          ANA_CHECK( cfg.tool->applySystematicVariation(sys) );
          ANA_CHECK( cfg.tool->getEfficiencyScaleFactor(electron, sf) );     
          float delta  = sf - sf_nom;
          NP np = std::get<NP>(s);
          if(delta != 0.) {
            if(np.uncorr) {
              nonzero_uncorr.set(np.index);
              ++n_nonzero_uncorr;
            }
            else nonzero_corr.set(np.index);
          } else {
            if(np.uncorr) zero_uncorr.set(np.index);
            else zero_corr.set(np.index);
          }
        }
        multi_np = multi_np || (n_nonzero_uncorr > 1);
        store.clear();
      }
    }
    if(multi_np) {
      std::string w{"several uncorrelated NPs seem to affect the bin at " + dom.str() + "."};
      Warning(MSGSOURCE, w.c_str());
    } else if(nonzero_uncorr.count() > 1) {
      std::string w{"the binning used for the scan is inadapted at " + 
        dom.str() + " (too coarse?)."};
      Warning(MSGSOURCE, w.c_str());
    }
    if((zero_uncorr & nonzero_uncorr).any() ||  
        (zero_corr & nonzero_corr).any()) {
      std::string w{"the binning used for the scan is inadapted at " + 
        dom.str() + " (wrong boundaries?)."};
      Warning(MSGSOURCE, w.c_str());
    }
    if((nonzero_uncorr!=expected_uncorr) && nonzero_uncorr.any()) {
      std::string snz, se;
      for(std::size_t i=0;i<nonzero_uncorr.size();++i) {
        std::string x{std::to_string(i) + ','};
        if(nonzero_uncorr[i] && !expected_uncorr[i]) snz += x;
        if(!nonzero_uncorr[i] && expected_uncorr[i]) se += x;
      }
      snz.pop_back();
      se.pop_back();
      std::string w{"systUncorrVariationIndex() at " + dom.str() + 
        " indicates different NP(s) than those found in the scan, (%s) vs (%s)."};
      Warning(MSGSOURCE, w.c_str(), se.c_str(), snz.c_str());
    }
    for(auto& s : cfg.systematics) {
      NP np = std::get<NP>(s);
      bool impact = (np.uncorr? nonzero_uncorr[np.index] : nonzero_corr[np.index]);
      if(impact) affected_bins[np].emplace_back(d);
    }
  }
  return true;
}

bool displayFindings(Config& cfg, map_t& affected_bins)
{
  using namespace asg::msgUserCode;

  for(auto& s : cfg.systematics) {
    NP np = std::get<NP>(s);
    auto& bins = affected_bins[np];
    std::string name = std::get<CP::SystematicSet>(s).begin()->basename();
    Domain bounds;
    bool holes{false}, abs_eta{false};
    if(!find_boundaries(cfg, bins, bounds, abs_eta, holes)) {
      Warning(MSGSOURCE, "%s seems to not affect any bin", name.c_str());
      continue;
    }
    std::string txt = name + " --> ";
    if(holes) txt += "subdomain of ";
    txt += bounds.str(abs_eta) + '.';
    if(!holes) {
      Info(MSGSOURCE, txt.c_str());
    } else {
      Warning(MSGSOURCE, txt.c_str());
    }
  }
  return true;
}

bool displayFindings_analysis(Config& cfg, map_t& affected_bins)
{
  using namespace asg::msgUserCode;

  std::vector<std::pair<std::string, int>> summary;
  NP prev{-888, false};
  for(auto& kv : affected_bins) {
    auto& bins = kv.second;
    if(!bins.size()) continue;
    NP np = kv.first;
    bool next = (prev.uncorr==np.uncorr && np.index==prev.index+1);
    prev = np;
    if(summary.size() && next) {
      std::get<int>(summary.back()) = np.index;
      continue;
    }
    auto& sys = std::get<CP::SystematicSet>(*std::find_if(
      cfg.systematics.cbegin(), cfg.systematics.cend(), 
      [=](auto& x){ return std::get<NP>(x) == np; }));
    summary.emplace_back(sys.begin()->basename(), -1);
  }

  for(auto& x: summary) {
    std::string s{"   ++ " + std::get<std::string>(x)};
    int i = std::get<int>(x);
    if(i >= 0) s += " to " + std::to_string(i);
    Info(MSGSOURCE, s.c_str());
  }
  return true;
}

bool find_boundaries(const Config& cfg, std::vector<int>& affected_bins, Domain& bounds, bool& abs_eta, bool& holes)
{
  if(!affected_bins.size()) return false;
  constexpr float inf{std::numeric_limits<float>::max()};
  constexpr float inf_{std::numeric_limits<float>::lowest()};
  Domain bAC[2] = {{inf, inf_, inf, inf_}, {inf, inf_, inf, inf_}};
  auto update = [&](int side, auto& dom, float etamin, float etamax) {
    auto& b = bAC[side];
    b.etamin = std::min(b.etamin, etamin);
    b.etamax = std::max(b.etamax, etamax);
    b.ptmin = std::min(b.ptmin, dom.ptmin);
    b.ptmax = std::max(b.ptmax, dom.ptmax);
  };

  for(int bin : affected_bins) {
    const Domain& dom = cfg.domains.at(bin);
    if(dom.etamax > 0) update(1, dom, std::max(0.f, dom.etamin), dom.etamax);
    if(dom.etamin < 0) update(0, dom, dom.etamin, std::min(0.f, dom.etamax));
  }

  holes = false;
  const int ac = 2*(bAC[1].ptmin!=inf) + (bAC[0].ptmin!=inf);
  if(ac == 3) {
    abs_eta = (bAC[0].ptmin == bAC[1].ptmin) && (bAC[0].ptmax == bAC[1].ptmax) &&
      (std::fabs(bAC[0].etamax + bAC[1].etamin) < 1e-4) && 
      (std::fabs(bAC[0].etamin + bAC[1].etamax) < 1e-4);
    if(abs_eta) {
      bounds = bAC[1];
    } else {
      holes = true;
      bounds.etamin = bAC[0].etamin;
      bounds.etamax = bAC[1].etamax;
      bounds.ptmin = std::min(bAC[0].ptmin, bAC[1].ptmin);
      bounds.ptmax = std::min(bAC[0].ptmax, bAC[1].ptmax);
    }
  } else if(ac) bounds = bAC[ac - 1];
  else return false;

  if(!holes) {
    // Search for overlapping domains not affected by this NP.
    const int imax = cfg.domains.size();
    auto f = [=](float x) { return abs_eta? std::fabs(x) : x; };
    for(int i=0;i<imax && !holes;++i) {
      if(std::count(affected_bins.cbegin(), affected_bins.cend(), i)) continue;
      auto& dom = cfg.domains[i];
      holes = (dom.ptmax > bounds.ptmin) && 
        (dom.ptmin < bounds.ptmax) && 
        (f(dom.etamax) > bounds.etamin) &&
        (f(dom.etamin) < bounds.etamax);
    }
  }
  return true;
}

std::string Domain::str(bool abs_eta) const
{
  std::stringstream s;
  const char* eta = (abs_eta? "|eta|" : "eta");
  s << etamin << " < " << eta << " < " << etamax << ", ";
  if(ptmax != pt_edges.back()) {
    s << 1e-3f * ptmin << " < pt < " 
      << 1e-3f * ptmax << " GeV";
  } else {
    s << "pt > " << 1e-3f * ptmin << " GeV";
  }
  return s.str();
}

bool parse_csv_token(float x, std::vector<float>& bins)
{
  bins.push_back(x);
  return true;
}

bool parse_csv_token(std::string s, std::vector<char>& flags, 
  const std::vector<float>& edges)
{
  const bool accept_aliases = (&edges == &eta_edges);
  if(flags.size() != edges.size()-1) return false;
  int alias{0};
  if(s == "nocrack") alias = 1;
  else if(s == "barrel") alias = 2;
  else if(s == "endcap") alias = 3;
  else if(s == "sym") alias = 4;
  if(!accept_aliases && alias>0) return false;
  if(alias>=1 && alias<=3) {
    for(std::size_t i=0;i<edges.size()-1;++i) {
      float eta = std::fabs(edges[i]);
      if(alias==1 && eta>=1.37 && eta<1.52) flags[i] = -1;
      if(alias==2 && eta<1.37) flags[i] = 1;
      if(alias==3 && eta>1.52) flags[i] = 1;
    }
    return true;
  } else if(alias == 4) {
    for(std::size_t i=0;i<edges.size();++i) {
      if(edges[i] < 0) flags[i] = 2;
    }
    return true;
  }

  auto pos = s.find("..");
  if(pos == std::string::npos) return false;
  std::stringstream ss{s.replace(pos, 2, 1, ' ')};
  float xmin, xmax;
  ss >> xmin >> xmax;
  if(ss.fail() || !(ss >> std::ws).eof()) return false;
  for(std::size_t i=0;i<edges.size()-1;++i) {
    if(xmin<edges[i+1] && xmax>edges[i] && flags[i]>=0) {
      flags[i] = 1;
    }
  }
  return true;
}

template<typename... Args>
bool parse_csv_list(std::string val, Args&... args)
{
  std::replace(val.begin(), val.end(), ',', ' ');
  std::stringstream ss{val};
  while(true) {
    std::conditional_t<(sizeof...(Args)<2), float, std::string> x;
    ss >> x;
    if(ss.fail()) break;
    if(!parse_csv_token(x, args...)) return false;
  }
  return ss.eof();
}

int get_run_number(int year)
{
  using namespace asg::msgUserCode;

  int random_runs[] = {280423, 302737, 332720, 354826};
  switch(year) {
  case 2015: case 2016: case 2017: case 2018: 
    return random_runs[year - 2015];
  default:
    Error(MSGSOURCE, "Invalid year specified, it should be between 2015 and 2018.");
    std::abort();
  }
}
