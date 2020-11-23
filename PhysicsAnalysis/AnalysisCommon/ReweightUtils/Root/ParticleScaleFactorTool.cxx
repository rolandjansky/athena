/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ReweightUtils includes
#include "ReweightUtils/ParticleScaleFactorTool.h"

#include "PathResolver/PathResolver.h"

#include "PATInterfaces/SystematicRegistry.h"

#include "TH2D.h"
#include "TKey.h"
#include "TClass.h"
#include "TFile.h"

ParticleScaleFactorTool::ParticleScaleFactorTool( const std::string& name ) : asg::AsgTool( name ){

  declareProperty( "File", m_configFile = "" , "User should specify config file to read");

}

ParticleScaleFactorTool::~ParticleScaleFactorTool() {
  
  //delete all histograms
  for(auto& a : m_hists) {
    for(auto& b : a.second.hists) {
      delete b.second;
    }
  }
  
}

StatusCode ParticleScaleFactorTool::initialize() {
  
  
  std::string file = PathResolverFindCalibFile(m_configFile);
  if(file=="") return StatusCode::FAILURE;
  
  TFile* f = TFile::Open(file.c_str());
  
  if(!f) {
    ATH_MSG_ERROR("Could not open file " << file);
    return StatusCode::FAILURE;
  }
  
  m_affectingSysts.insert(CP::SystematicVariation("")); //add the nominal syst
  
  //loop over the keys in the file, we are looking for TDirectory (systematics) and TH1 (factors)
  std::unique_ptr<TIterator> itr(f->GetListOfKeys()->MakeIterator());
  TKey* key = 0;
  while( (key = static_cast<TKey*>(itr->Next())) ) {
    TClass* cl = TClass::GetClass(key->GetClassName());
    if(!cl || !cl->InheritsFrom(TDirectory::Class())) continue;
    ATH_MSG_DEBUG("Detected systematic " << key->GetName());
    m_affectingSysts.insert(CP::SystematicVariation(key->GetName()));
    if(!CP::SystematicVariation(key->GetName()).parameter()) {
      ATH_MSG_ERROR("Systematic must be of form: <string>__Xup  OR  <string>__Xdown  ... where X is a number");
      return StatusCode::FAILURE;
    }
  }
   
   itr->Reset();
   while( (key = static_cast<TKey*>(itr->Next())) ) {
    TClass* cl = TClass::GetClass(key->GetClassName());
    if(!cl || !cl->InheritsFrom("TH1")) continue;
   
    
    ATH_MSG_DEBUG("Detected scale factor " << key->GetName());
    
    //loop over systematics
    for(auto& syst : m_affectingSysts) {
      TString h = (syst==CP::SystematicVariation("")) ? std::string(key->GetName()) : syst.name()+ "/" + std::string(key->GetName());
      TH1* hist = static_cast<TH1*>(f->Get(h));
      if(!hist) {
        if(!(syst==CP::SystematicVariation(""))) {
          ATH_MSG_VERBOSE("No " << syst.name() << " variation of " << key->GetName());
          continue; //fine, just no systematic variation
        } else {
          ATH_MSG_ERROR("Failed at retrieving " << key->GetName());
          return StatusCode::FAILURE;
        }
      }
      //detect particle type from title
      xAOD::Type::ObjectType type = xAOD::Type::Other;
      if(strcmp(hist->GetTitle(),"Electron")==0) type = xAOD::Type::Electron;
      else if(strcmp(hist->GetTitle(),"Muon")==0) type = xAOD::Type::Muon;
      else if(strcmp(hist->GetTitle(),"Jet")==0) type = xAOD::Type::Jet;
      else if(strcmp(hist->GetTitle(),"Photon")==0) type = xAOD::Type::Photon;
      else if(strcmp(hist->GetTitle(),"Tau")==0) type = xAOD::Type::Tau;
      else {
         ATH_MSG_ERROR("Unknown particle type: " << hist->GetTitle());
         return StatusCode::FAILURE;
      }
      
      //check all axis are labelled
      if(strlen(hist->GetXaxis()->GetTitle())==0 || 
        (hist->GetDimension()>1 && strlen(hist->GetYaxis()->GetTitle())==0) || 
        (hist->GetDimension()>2 && strlen(hist->GetZaxis()->GetTitle())==0) ) {
          ATH_MSG_ERROR("All axis of histogram " << hist->GetName() << " need to be labelled");
          return StatusCode::FAILURE;
      }
      //see if nominal already present, if it is we have to 'extend' the existing histogram
      TH1*& existHist = m_hists[type].hists[syst.name()];
      if(existHist) {
        ATH_MSG_DEBUG("Combining " << hist->GetTitle() << ": f(" <<  
            existHist->GetXaxis()->GetTitle() << "," << hist->GetXaxis()->GetTitle() << 
            ") = 2*f(" << existHist->GetXaxis()->GetTitle() << ")*f(" << 
            hist->GetXaxis()->GetTitle() << ")/[f(" << existHist->GetXaxis()->GetTitle() << 
            ")+f(" << hist->GetXaxis()->GetTitle() << ")]");
        
        TH1* newHist = 0;
        if(existHist->GetDimension()==1 && hist->GetDimension()==1) {
          std::vector<double> binEdges1, binEdges2;
          for(int i=1;i<=existHist->GetNbinsX()+1;i++) binEdges1.push_back(existHist->GetBinLowEdge(i));
          for(int i=1;i<=hist->GetNbinsX()+1;i++) binEdges2.push_back(hist->GetBinLowEdge(i));
          newHist = new TH2D("myHist","myHist",existHist->GetNbinsX(),&binEdges1[0],hist->GetNbinsX(),&binEdges2[0]);
          newHist->SetDirectory(0);
          newHist->GetXaxis()->SetTitle(existHist->GetTitle());
          newHist->GetYaxis()->SetTitle(hist->GetTitle());
          for(int i=0;i<=existHist->GetNbinsX()+1;i++) {
            for(int j=0;j<=hist->GetNbinsX()+1;j++) {
              double a = existHist->GetBinContent(i); double b = hist->GetBinContent(j);
              if(a && b) newHist->SetBinContent(newHist->GetBin(i,j), 2.0 * (a*b)/(a+b) );
              //combine error ... 
              if(!(syst==CP::SystematicVariation(""))) {
                double da = existHist->GetBinError(i);double db = hist->GetBinError(j);
                if(a && b) newHist->SetBinError(newHist->GetBin(i,j), 2.0 * (da*b*b + db*a*a)/((a+b)*(a+b)) );
              }
            }
          }
          delete existHist;
          existHist = newHist;
        } else {
          ATH_MSG_ERROR("Unsupported scale factor reparamertization :-(");
          return StatusCode::FAILURE; 
        }
        
      } else {
        existHist = static_cast<TH1*>(hist->Clone("myHist")); existHist->SetDirectory(0);
        //no systematic if nominal hist ... 
        //if(syst==CP::SystematicVariation("")) for(int i=0;i<(hist->GetNbinsX()+2)*(hist->GetNbinsY()+2)*(hist->GetNbinsZ()+2);i++) hist->SetBinError(i,0);
      }
      
      if(syst==CP::SystematicVariation("")) {
        for(int i=0;i<hist->GetDimension();i++) {
          std::string axisTitle = ( i==0 ) ? hist->GetXaxis()->GetTitle() : ( (i==1) ? hist->GetYaxis()->GetTitle() : hist->GetZaxis()->GetTitle() );
          axisTitle.erase(remove_if(axisTitle.begin(), axisTitle.end(), isspace),axisTitle.end());
          std::function<double(const xAOD::IParticle&)> bfunc;
          ATH_MSG_DEBUG("  Parameter " << axisTitle);
          if(axisTitle=="pt" || axisTitle=="pt/MeV" || axisTitle=="pt[MeV]") {
            bfunc = part_pt;
          } else if(axisTitle == "pt/GeV" || axisTitle=="pt[GeV]") {
            bfunc = part_pt_gev;
          } else if(axisTitle=="eta") {
            bfunc = part_eta;
          } else {
            bfunc = std::bind(part_decor, std::placeholders::_1, axisTitle);
          }
          m_hists[type].axisFuncs.push_back(bfunc);
        }
      }
    }
    
    
    
    
  }
  
  //remove the nominal syst to finish up
  CP::SystematicSet tmpSet(m_affectingSysts);
  m_affectingSysts.clear();
  for(auto& s : tmpSet) {
    if(s==CP::SystematicVariation("")) continue;
    m_affectingSysts.insert(s);
    //m_affectingSysts.insert(CP::SystematicVariation(s.basename(),-1)); //add the negative fluctuation
  }
  
  
  f->Close();
  delete f;
  f = 0;
  
  if(CP::SystematicRegistry::getInstance().registerSystematics(*this) != StatusCode::SUCCESS ) return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
}

double ParticleScaleFactorTool::evaluate( const xAOD::IParticle* particle ) const {
   //check we have a hist for this particle type 
   auto histItr = m_hists.find(particle->type());
   if(histItr==m_hists.end()) {
     ATH_MSG_ERROR("No scale factor available for particle type: " << particle->type());
     throw std::runtime_error(TString::Format("%s : No scale factor available for particle type %d",name().c_str(),int(particle->type())).Data());
   }
  
  
   std::pair<CP::SystematicVariation,TH1*> res = histItr->second.getHist(m_currentSyst);
  
   const TH1* hist = res.second;
  
   
   int bin = 0;
   switch(hist->GetDimension()) {
     case 1: bin = hist->FindFixBin(histItr->second.axisFuncs[0](*particle)); break;
     case 2: bin = hist->FindFixBin(histItr->second.axisFuncs[0](*particle),histItr->second.axisFuncs[1](*particle)); break;
     case 3: bin = hist->FindFixBin(histItr->second.axisFuncs[0](*particle),histItr->second.axisFuncs[1](*particle),histItr->second.axisFuncs[2](*particle)); break;
   }

   if(!res.first.parameter()) return hist->GetBinContent(bin); //must have been nominal;

   double nom = histItr->second.getHist(CP::SystematicVariation("")).second->GetBinContent(bin);
   //got here so get nominal hist and do difference ...
   return nom + (m_currentSyst.parameter()/res.first.parameter()) * (hist->GetBinContent(bin) - nom);


   
   
   
}

const std::pair<CP::SystematicVariation,TH1*> ParticleScaleFactorTool::Hists::getHist(const CP::SystematicVariation& set) const { 
    //find hist that matches basename and take the one that has the closest parameter value
    std::pair<CP::SystematicVariation,TH1*> result(CP::SystematicVariation(""),0);
    for(auto& s : hists) {
      if(s.first.basename()!=set.basename()) continue;
      if(!result.second) {result = s;}
      else {
        if(set.parameter()*s.first.parameter()>0) result = s;
      }
    }
    if(result.second) return result;
    //return nominal
    auto it = hists.find(CP::SystematicVariation("")); 
    result = *it; return result;
    //return it->second;
}



StatusCode ParticleScaleFactorTool::applySystematicVariation( const CP::SystematicSet& systConfig ) { 
    for(auto& syst : systConfig) {
      for(auto& s : m_affectingSysts) {
        if(s.basename()==syst.basename()) {
          m_currentSyst = syst; m_isNominal=false;
          return StatusCode::SUCCESS;
        }
      }
    }
    m_currentSyst = CP::SystematicVariation("");
    m_isNominal = true;
    return StatusCode::SUCCESS;
}



