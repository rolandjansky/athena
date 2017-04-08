/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <map>
#include <mutex>
#include <TH1.h>
#include <TH2.h>
#include <TProfile.h>
#include <TProfile2D.h>

#include "AthenaMonitoring/GenericMonitoringTool.h"
#include "AthenaMonitoring/HistogramDef.h"

using namespace std;

const InterfaceID& GenericMonitoringTool::interfaceID() {
	static InterfaceID GenericMonitoringTool_ID("GenericMonitoringTool", 1, 0);

	return GenericMonitoringTool_ID;
}

GenericMonitoringTool::GenericMonitoringTool(const std::string & type, const std::string & name, const IInterface* parent)
  : AthAlgTool(type, name, parent), m_histSvc("THistSvc", name) { 
  declareProperty("Histograms", m_histograms, "Definitions of histograms");
  declareInterface<GenericMonitoringTool>(this);
}

GenericMonitoringTool::~GenericMonitoringTool() { }

StatusCode GenericMonitoringTool::initialize() {
  // ATH_CHECK(setProperties());
  ATH_CHECK(m_histSvc.retrieve());

  // const INamedInterface* parentAlg = dynamic_cast<const INamedInterface*>(parent());

  // if (parentAlg == nullptr) {
  //   ATH_MSG_ERROR("Cannot retrieve INamedInterface of parent algorithm");
  //   return StatusCode::FAILURE;
  // }
  
  string parentAlgName = "TODO"; // parentAlg->name();

  m_histogramCategory["EXPERT"]   = new MonGroup(this, parentAlgName, expert);
  m_histogramCategory["SHIFT"]    = new MonGroup(this, parentAlgName, shift);
  m_histogramCategory["DEBUG"]    = new MonGroup(this, parentAlgName, debug);
  m_histogramCategory["RUNSTAT"]  = new MonGroup(this, parentAlgName, runstat);
  m_histogramCategory["EXPRESS"]  = new MonGroup(this, parentAlgName, express);

  for (const string& item : m_histograms) {
    HistogramDef def = HistogramDef::parse(item);

    if ( ! def.ok || createFiller(def).isFailure() ) {
      ATH_MSG_DEBUG("Demand to monitor variable(s): " << def.name  << " can't be satisfied");
    }
  }

  if ( m_fillers.empty() ) {
    ATH_MSG_ERROR("No variables to be monitored, detach this tool, it will save time");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

vector<HistogramFiller*> GenericMonitoringTool::getHistogramsFillers(vector<reference_wrapper<Monitored::IMonitoredVariable>> monitoredVariables) {
  vector<HistogramFiller*> result;

  for (auto filler : m_fillers) {
    auto fillerVariables = filler->histogramVariablesNames();
    vector<reference_wrapper<Monitored::IMonitoredVariable>> variables;

    for (auto fillerVariable : fillerVariables) {
      for (auto monValue : monitoredVariables) {
        if (fillerVariable.compare(monValue.get().name()) == 0) {
          variables.push_back(monValue);
          break;
        }
      }
    }

    if (fillerVariables.size() != variables.size()) {
      ATH_MSG_DEBUG("Filler has different variables than monitoredVariables");
      continue;
    }

    HistogramFiller* fillerCopy = filler->clone();
    fillerCopy->setMonitoredVariables(variables);
    result.push_back(fillerCopy);
  }

  return result;
}

string GenericMonitoringTool::level2string(Level l) {
  string result = "/UNSPECIFIED_NONE_OF_EXPERT_DEBUG_SHIFT_EXPRESS_RUNSUM/";

  switch (l){
  case debug:
    result = "/DEBUG/";
    break;
  case expert:
    result = "/EXPERT/";
    break;
  case shift:
    result = "/SHIFT/";
    break;
  case express:
    result = "/EXPRESS/";
    break;
  case runsum:
    result = "/RUNSUM/";
    break;
  }

  return result;
}

void GenericMonitoringTool::setOpts(TH1* hist, const std::string& opt) {
  // try to apply an option
  if ( opt.find("kCanRebin") != std::string::npos ) {
     hist->SetCanExtend(TH1::kAllAxes);
  } else {
     hist->SetCanExtend(TH1::kNoAxis);  
  }
  // try to apply option to make Sumw2 in histogram
  if ( opt.find("Sumw2") != std::string::npos ) {
    hist->Sumw2();
  }
}

void GenericMonitoringTool::setLabels(TH1* hist, const std::vector<std::string>& labels) {
  if (labels.empty()){
    return;
  }
  for ( int i = 0; i < std::min( (int)labels.size(), (int)hist->GetNbinsX() ); ++i ) {
    int bin = i+1;
    hist->GetXaxis()->SetBinLabel(bin, labels[i].c_str());
    ATH_MSG_DEBUG("setting label X" <<  labels[i] << " for bin " << bin);
  }

  for ( int i = (int)hist->GetNbinsX(); i < std::min( (int)labels.size(), (int)hist->GetNbinsX()+(int)hist->GetNbinsY() ); ++i ) {
    int bin = i+1-(int)hist->GetNbinsX();
    hist->GetYaxis()->SetBinLabel(bin, labels[i].c_str());
    ATH_MSG_DEBUG("setting label Y" <<  labels[i] << " for bin " << bin);
  }
}

/**
 * Create and register histogram
 * 
 * If histogram already exists under that name, re-use it
 * H      : actual type of histogram to be created
 * HBASE  : histogram base class (TH[1,2,3])
 * Args   : remaining arguments to TH constructor (except name, title)
 */
template<class H, class HBASE, typename... Types> 
HBASE* GenericMonitoringTool::create(const HistogramDef& def, Types&&... hargs) {
  // Check if histogram exists already
  HBASE* histo = m_histogramCategory[def.path]->template getHist<HBASE>(def.alias);

  if (histo) {
    ATH_MSG_DEBUG("Histogram " << def.alias << " already exists. Re-using it.");
    return histo;
  }

  // Create the histogram and register it
  H* h = new H(def.alias.c_str(), def.title.c_str(), std::forward<Types>(hargs)...);
  StatusCode sc = m_histogramCategory[def.path]->regHist(static_cast<TH1*>(h));

  if (sc.isFailure()) {
    delete h;
    h = nullptr;
  }
  
  return h;
}

template<class H> 
TH1* GenericMonitoringTool::create1D( TH1*& histo, const HistogramDef& def ) {
  histo = create<H,TH1>(def, def.xbins, def.xmin, def.xmax);
  return histo;
}

template<class H> 
TH1* GenericMonitoringTool::createProfile( TProfile*& histo, const HistogramDef& def ) {
  TH1* h = create<H,TH1>(def, def.xbins, def.xmin, def.xmax, def.ymin, def.ymax);
  histo = dynamic_cast<TProfile*>(h);
  return h;
}

template<class H> 
TH1* GenericMonitoringTool::create2D( TH2*& histo, const HistogramDef& def ) {
  histo = create<H,TH2>(def, def.xbins, def.xmin, def.xmax, def.ybins, def.ymin, def.ymax);
  return histo;
}

template<class H> 
TH1* GenericMonitoringTool::create2DProfile( TProfile2D*& histo, const HistogramDef& def ) {
  TH1* h = create<H,TH2>(def, def.xbins, def.xmin, def.xmax, 
                         def.ybins, def.ymin, def.ymax, def.zmin, def.zmax);
  histo = dynamic_cast<TProfile2D*>(h);
  return histo;
}

StatusCode GenericMonitoringTool::createFiller(const HistogramDef& def) {
  TH1* histo(0);
  TH1* histo1D(0);
  TProfile* histoProfile(0);
  TH2* histo2D(0);
  TProfile2D* histo2DProfile(0);

  // create 1D
  if (def.type == "TH1F") 
    histo = create1D<TH1F>(histo1D, def);

  else if (def.type == "TH1D") 
    histo = create1D<TH1D>(histo1D, def);
  
  else if (def.type == "TH1I") 
    histo = create1D<TH1I>(histo1D, def);

  else if (def.type == "TProfile")
    histo = createProfile<TProfile>(histoProfile, def);
  
  //create 2D
  else if (def.type == "TH2F")
    histo = create2D<TH2F>(histo2D, def);

  else if (def.type == "TH2D")
    histo = create2D<TH2D>(histo2D, def);

  else if (def.type == "TH2I") 
    histo = create2D<TH2I>(histo2D, def);
  
  else if (def.type == "TProfile2D")
    histo = create2DProfile<TProfile2D>(histo2DProfile, def);
  
  if (histo == 0 ) {
    ATH_MSG_WARNING("Can not create yet histogram of type: " << def.type);
    ATH_MSG_WARNING("Try one of: TH1[F,D,I], TH2[F,D,I], TProfile, TProfile2D");
    return StatusCode::FAILURE;
  }

  setLabels(histo, def.labels);
  setOpts(histo, def.opt);

  std::string m_parentName = "TODO";

  if ( histo1D && !histoProfile){
    if ( def.opt.find("kCumulative") != std::string::npos ) {
      ATH_MSG_DEBUG("Variable: " << def.name[0] << " from parent algorithm: "
                    << m_parentName << " will be histogrammed in Cummulative histogram");

      m_fillers.push_back(new CumulativeHistogramFiller1D(histo1D, def));
    } else if (def.opt.find("kVecUO") != std::string::npos) {
      ATH_MSG_DEBUG("Variable: " << def.name << " from parent algorithm: "
                    << m_parentName << " will be added to histogram");
      // if (g && (unsigned(histo1D->GetNbinsX()+2) != g->size())) {
      //   ATH_MSG_WARNING("Variable: " << def.name << " from parent algorithm: " << m_parentName 
      //                   << " has different dimension: " << g->size() 
      //                   << " than histogram: " << histo1D->GetNbinsX()
      //                   << " booked for it and kVecUO options is requested (variable has to accomodate Under/Overflows too)");
      // }

      m_fillers.push_back(new VecHistogramFiller1DWithOverflows(histo1D, def));
    } else if (def.opt.find("kVec") != std::string::npos) {
      ATH_MSG_DEBUG("Variable: " << def.name << " from parent algorithm: "
                    << m_parentName << " will be added to histogram");
      // if (g && (unsigned(histo1D->GetNbinsX()) != varIt->second->size())) {
      //   ATH_MSG_WARNING("Variable: " << def.name << " from parent algorithm: " << m_parentName 
      //                   << " has different dimension: " << g->size() 
      //                   << " than histogram: " << histo1D->GetNbinsX()
      //                   << " booked for it and kVec options is requested");
      // }

      m_fillers.push_back(new VecHistogramFiller1D(histo1D, def));
    } else {
      ATH_MSG_DEBUG("Variable: " << def.name << " from parent algorithm: " << m_parentName
                    <<  " will be histogrammed");

      m_fillers.push_back(new HistogramFiller1D(histo1D, def));
    }
  } else if ( histo2DProfile ) {
      ATH_MSG_DEBUG("Variables: " << def.name[0] << "," << def.name[1] << "," << def.name[2]
                    << " from parent algorithm: " << m_parentName
                    << " will be histogrammed in 2D Profile histogram");

      m_fillers.push_back(new HistogramFiller2DProfile(histo2DProfile, def));
  } else if ( histo2D || histoProfile ){
      ATH_MSG_DEBUG("Variables: " << def.name[0] << "," << def.name[1]
                    << " from parent algorithm: " << m_parentName
                    << " will be histogrammed in 2D histogram");

      if (histo2D) {
        m_fillers.push_back(new HistogramFiller2D(histo2D, def));
      } else {       
        m_fillers.push_back(new HistogramFillerProfile(histoProfile, def));
      }
  }
  
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////////////
// code form monitoring group
/////////////////////////////////////////////////////////////////////////
GenericMonitoringTool::MonGroup::MonGroup(GenericMonitoringTool* tool, const std::string& algo, Level l) 
  : m_tool(tool), m_algo(algo), m_level(l) {}

StatusCode GenericMonitoringTool::MonGroup::regHist(TH1* h) {
  return m_tool->m_histSvc->regHist(m_tool->level2string(m_level)+m_algo+"/"+h->GetName(), h);
}

StatusCode GenericMonitoringTool::MonGroup::deregHist(TH1* h) {
  return m_tool->m_histSvc->deReg(h);
}
