/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <xAODPrimitives/IsolationType.h>
#include <IsolationSelection/IsolationSelectionTool.h>
#include <IsolationSelection/IsolationConditionHist.h>
#include <IsolationSelection/IsolationConditionFormula.h>
#include "PathResolver/PathResolver.h"
#include <TROOT.h>
#include <TFile.h>
#include <TObjString.h>
#include <TH3.h>

namespace CP
{
  IsolationSelectionTool::IsolationSelectionTool( const std::string& name ) :
    asg::AsgTool( name ),  
    /// input file 
    m_calibFileName("IsolationSelection/v1/DC14_IsolationCutValues_TruthBased_Zee_Zmumu.root"),
    m_calibFile(nullptr),
    
    /// TAccept's
    m_photonAccept("IsolationSelectionToolPhotonTAccept"),
    m_electronAccept("IsolationSelectionToolElectronTAccept"),
    m_muonAccept("IsolationSelectionToolMuonTAccept"),
    m_objAccept("IsolationSelectionToolObjTAccept"),
    m_iparWPs(0),
    m_iparAccept(0)
  {
    declareProperty("CalibFileName", m_calibFileName = "IsolationSelection/v1/MC15_Z_Jpsi_cutMap.root", "The config file to use");
    declareProperty("MuonWP", m_muWPname = "Undefined" , "Working point for muon");
    declareProperty("ElectronWP", m_elWPname = "Undefined" , "Working point for electron");
    declareProperty("PhotonWP", m_phWPname = "Undefined" , "Working point for photon");
    declareProperty("MuonKey", m_muWPKey = "/Muons/DFCommonGoodMuon/mu_cutValues_", "path of the cut map for muon");
    declareProperty("ElectronKey", m_elWPKey = "/ElectronPhoton/LHTight/el_cutValues_", "path of the cut map for electron");
    declareProperty("PhotonKey", m_phWPKey = "/ElectronPhoton/LHTight/el_cutValues_", "path of the cut map for photon");
  }

  IsolationSelectionTool::~IsolationSelectionTool(){
    /// need a fix? Some wp might be create by external code..
    for (auto c: m_muWPs) if(c){delete c; c=nullptr;}
    for (auto c: m_elWPs) if(c){delete c; c=nullptr;}
    for (auto c: m_phWPs) if(c){delete c; c=nullptr;}
  }

  StatusCode IsolationSelectionTool::initialize() 
  {
    /// Greet the user:
    ATH_MSG_INFO( "Initialising..." );
    std::string filename = PathResolverFindCalibFile(m_calibFileName);

    ATH_MSG_INFO("Reading input file "<< m_calibFileName << " from " << filename);
    m_calibFile = std::shared_ptr<TFile>(new TFile(filename.c_str() , "READ")); 
    
    TObjString* versionInfo = (TObjString*)m_calibFile->Get("VersionInfo");
    if(versionInfo) ATH_MSG_INFO( "VersionInfo:" << versionInfo->String());
    else ATH_MSG_WARNING( "VersionInfo of input file (" << filename << ") is missing.");

    /// setup working points
    if(m_phWPname != "Undefined") ATH_CHECK(addPhotonWP(m_phWPname));
    if(m_elWPname != "Undefined") ATH_CHECK(addElectronWP(m_elWPname));
    if(m_muWPname != "Undefined") ATH_CHECK(addMuonWP(m_muWPname));
 
    /// Return gracefully:
    return StatusCode::SUCCESS;
  }

  StatusCode IsolationSelectionTool::setIParticleCutsFrom(xAOD::Type::ObjectType ObjType){
    if(ObjType==xAOD::Type::Electron) {m_iparAccept = &m_electronAccept; m_iparWPs = &m_elWPs;}
    else if(ObjType==xAOD::Type::Muon){m_iparAccept = &m_muonAccept; m_iparWPs = &m_muWPs;}
    else if(ObjType==xAOD::Type::Photon){m_iparAccept = &m_photonAccept; m_iparWPs = &m_phWPs;}
    else {return StatusCode::FAILURE;}

    return StatusCode::SUCCESS;
  }

  StatusCode IsolationSelectionTool::finalize()
  {
    /// Return gracefully:
    return StatusCode::SUCCESS;
  }  

  void IsolationSelectionTool::addCutToWP(IsolationWP* wp, std::string key, const xAOD::Iso::IsolationType t, const std::string expression)
  {
    std::string varname(xAOD::Iso::toString(t));
    key+=varname;
    
    // Stupid ROOT, this is silly, why do I need to do this????? why????
    // Create a unique directory in memory to hold the histograms:
    TDirectory* tempDir = getTemporaryDirectory();
    tempDir->cd();    
    
    std::shared_ptr<TH3F> histogram((TH3F*)m_calibFile->Get(key.c_str())->Clone());
    wp->addCut(new IsolationConditionHist(varname, t, expression, histogram));
  }

  StatusCode IsolationSelectionTool::addMuonWP(std::string muWPname)
  {
    auto wp = new IsolationWP(muWPname);
//     if(muWPname == "VeryLooseTrackOnly"){
    if(muWPname == "LooseTrackOnly"){
      addCutToWP(wp, m_muWPKey, xAOD::Iso::ptvarcone30, "99");
//     }else if(muWPname == "VeryLoose"){
    }else if(muWPname == "Loose"){
      addCutToWP(wp, m_muWPKey, xAOD::Iso::ptvarcone30, "99");
      addCutToWP(wp, m_muWPKey, xAOD::Iso::topoetcone20, "99");
//     }else if(muWPname == "Loose"){
    }else if(muWPname == "Tight"){
      addCutToWP(wp, m_muWPKey, xAOD::Iso::ptvarcone30, "99");
      addCutToWP(wp, m_muWPKey, xAOD::Iso::topoetcone20, "96");
//     }else if(muWPname == "Tight"){
//       addCutToWP(wp, m_muWPKey, xAOD::Iso::ptvarcone30, "94");
//       addCutToWP(wp, m_muWPKey, xAOD::Iso::topoetcone20, "89");
    }else if(muWPname == "Gradient"){
      addCutToWP(wp, m_muWPKey, xAOD::Iso::ptvarcone30, "0.1143*x+92.14");
      addCutToWP(wp, m_muWPKey, xAOD::Iso::topoetcone20, "0.1143*x+92.14");
    }else if(muWPname == "GradientLoose"){
      addCutToWP(wp, m_muWPKey, xAOD::Iso::ptvarcone30, "0.057*x+95.57");
      addCutToWP(wp, m_muWPKey, xAOD::Iso::topoetcone20, "0.057*x+95.57");
    }else{
      ATH_MSG_ERROR("Unknown muon isolaiton WP: " << muWPname);
      delete wp;
      return StatusCode::FAILURE;
    }

    m_muWPs.push_back(wp);
    m_muonAccept.addCut(wp->name(), wp->name());

    return StatusCode::SUCCESS; 
  }

  
  StatusCode IsolationSelectionTool::addPhotonWP(std::string phWPname)
  {   
    auto wp = new IsolationWP(phWPname);
    if(phWPname == "Cone40CaloOnly"){
      wp->addCut(new IsolationConditionFormula("topoetcone40", xAOD::Iso::topoetcone40, "0.024*x+2900"));
    }else if(phWPname == "Cone40"){
      wp->addCut(new IsolationConditionFormula("Cone40_calo", xAOD::Iso::topoetcone40, "0.028*x+3400"));
      wp->addCut(new IsolationConditionFormula("Cone40_track", xAOD::Iso::ptcone40, "0.12*x"));
    }else if(phWPname == "Cone20"){
      wp->addCut(new IsolationConditionFormula("Cone20_calo", xAOD::Iso::topoetcone20, "0.015*x+1500"));
      wp->addCut(new IsolationConditionFormula("Cone20_track", xAOD::Iso::ptcone20, "0.04*x"));
    }else{
      ATH_MSG_ERROR("Unknown photon isolaiton WP: " << phWPname);
      delete wp;
      return StatusCode::FAILURE;
    }

    m_phWPs.push_back(wp);
    m_photonAccept.addCut(wp->name(), wp->name());

    // Return gracefully:
    return StatusCode::SUCCESS;    
  }
  
  
  StatusCode IsolationSelectionTool::addElectronWP(std::string elWPname)
  {
    auto wp = new IsolationWP(elWPname);
//     if(elWPname == "VeryLooseTrackOnly"){
    if(elWPname == "LooseTrackOnly"){
      addCutToWP(wp, m_elWPKey, xAOD::Iso::ptvarcone20, "99");
//     }else if(elWPname == "VeryLoose"){
    }else if(elWPname == "Loose"){
      addCutToWP(wp, m_elWPKey, xAOD::Iso::ptvarcone20, "99");
      addCutToWP(wp, m_elWPKey, xAOD::Iso::topoetcone20, "99");
//     }else if(elWPname == "Loose"){
    }else if(elWPname == "Tight"){
      addCutToWP(wp, m_elWPKey, xAOD::Iso::ptvarcone20, "99");
      addCutToWP(wp, m_elWPKey, xAOD::Iso::topoetcone20, "96");
//     }else if(elWPname == "Tight"){
//       addCutToWP(wp, m_elWPKey, xAOD::Iso::ptvarcone20, "94");
//       addCutToWP(wp, m_elWPKey, xAOD::Iso::topoetcone20, "89");
    }else if(elWPname == "Gradient"){
      addCutToWP(wp, m_elWPKey, xAOD::Iso::ptvarcone20, "0.1143*x+92.14");
      addCutToWP(wp, m_elWPKey, xAOD::Iso::topoetcone20, "0.1143*x+92.14");
    }else if(elWPname == "GradientLoose"){
      addCutToWP(wp, m_elWPKey, xAOD::Iso::ptvarcone20, "0.057*x+95.57");
      addCutToWP(wp, m_elWPKey, xAOD::Iso::topoetcone20, "0.057*x+95.57");
    }else{
      ATH_MSG_ERROR("Unknown electron isolaiton WP: " << elWPname);
      delete wp;
      return StatusCode::FAILURE;
    }

    m_elWPs.push_back(wp);
    m_electronAccept.addCut(wp->name(), wp->name());

    // Return gracefully:
    return StatusCode::SUCCESS;    
  }

  StatusCode IsolationSelectionTool::addUserDefinedWP(std::string WPname, xAOD::Type::ObjectType ObjType, std::vector< std::pair<xAOD::Iso::IsolationType, std::string> >& cuts, std::string key, IsoWPType type)
  {
    std::vector< IsolationWP* >* wps(nullptr);
    Root::TAccept* ac(nullptr);
    if(ObjType==xAOD::Type::Electron){
      if(key=="") key = m_elWPKey;
      wps = &m_elWPs;
      ac = &m_electronAccept;
    }else if(ObjType==xAOD::Type::Muon){
      if(key=="") key = m_muWPKey;
      wps = &m_muWPs;
      ac = &m_muonAccept;
    }else if(ObjType==xAOD::Type::Photon){
      if(key=="") key = m_phWPKey;
      wps = &m_phWPs;
      ac = &m_photonAccept;
    }else if(ObjType==xAOD::Type::Other){
      if(key=="") return StatusCode::FAILURE;
      wps = &m_objWPs;
      ac = &m_objAccept;
    }else{
      return StatusCode::FAILURE;
    }

    auto wp = new IsolationWP(WPname);
    if(type == Efficiency){
      for(auto& c: cuts) addCutToWP(wp, key, c.first, c.second);
    }
    else if(type == Cut){
      for(auto& c: cuts) wp->addCut(new IsolationConditionFormula(xAOD::Iso::toString(c.first), c.first, c.second));
    }else{
      ATH_MSG_ERROR("Unknown isolation WP type -- should not happen.");
      delete wp;
      return StatusCode::FAILURE;
    }

    wps->push_back(wp);
    ac->addCut(wp->name(), wp->name());

    return StatusCode::SUCCESS;
  }

  StatusCode IsolationSelectionTool::addWP(std::string WP, xAOD::Type::ObjectType ObjType){
    if(ObjType==xAOD::Type::Electron) {return addElectronWP(WP);}
    else if(ObjType==xAOD::Type::Muon){return addMuonWP(WP);}
    else if(ObjType==xAOD::Type::Photon){return addPhotonWP(WP);}
    else {return StatusCode::FAILURE;}

    return StatusCode::SUCCESS;
  }
  StatusCode IsolationSelectionTool::addWP(IsolationWP* wp, xAOD::Type::ObjectType ObjType){
    if(ObjType==xAOD::Type::Electron) {m_elWPs.push_back(wp); m_electronAccept.addCut(wp->name(), wp->name());}
    else if(ObjType==xAOD::Type::Muon){m_muWPs.push_back(wp); m_muonAccept.addCut(wp->name(), wp->name());}
    else if(ObjType==xAOD::Type::Photon){m_phWPs.push_back(wp); m_photonAccept.addCut(wp->name(), wp->name());}
    else if(ObjType==xAOD::Type::Other){m_objWPs.push_back(wp); m_objAccept.addCut(wp->name(), wp->name());}
    else {return StatusCode::FAILURE;}

    return StatusCode::SUCCESS;
  }

  const Root::TAccept& IsolationSelectionTool::accept(const xAOD::Photon& x) const
  {
    m_photonAccept.clear();
    for(auto i: m_phWPs){ 
      if(i->accept(x)){
        m_photonAccept.setCutResult(i->name(), true );
      }
    }    
    return m_photonAccept;
  }
  
  const Root::TAccept& IsolationSelectionTool::accept(const xAOD::Electron& x) const
  {
    m_electronAccept.clear();
    for(auto i: m_elWPs){
      if(i->accept(x)){
        m_electronAccept.setCutResult(i->name(), true);
      }
    }
    return m_electronAccept;
  }

  const Root::TAccept& IsolationSelectionTool::accept(const xAOD::Muon& x) const
  {
    m_muonAccept.clear();
    for(auto i: m_muWPs){
      if(i->accept(x)){
        m_muonAccept.setCutResult(i->name(), true);
      }
    }
    return m_muonAccept;
  }

  const Root::TAccept& IsolationSelectionTool::accept(const xAOD::IParticle& x) const
  {
    m_iparAccept->clear();
    for(auto i: *m_iparWPs){
      if(i->accept(x)){
        m_iparAccept->setCutResult(i->name(), true);
      }
    }
    return *m_iparAccept;
  }

  const Root::TAccept& IsolationSelectionTool::accept(const strObj& x) const
  {
    m_objAccept.clear();
    for(auto i : m_objWPs){ 
      if(i->accept(x)){
        m_objAccept.setCutResult(i->name(), true);
      }
    }
    return m_objAccept;
  }
  
  const Root::TAccept& IsolationSelectionTool::getPhotonTAccept() const
  {
    return m_photonAccept;    
  }
  
  const Root::TAccept& IsolationSelectionTool::getElectronTAccept() const
  {
    return m_electronAccept;    
  }
  
  const Root::TAccept& IsolationSelectionTool::getMuonTAccept() const
  {
    return m_muonAccept;    
  }
  const Root::TAccept& IsolationSelectionTool::getObjTAccept() const
  {
    return m_objAccept;    
  }
  
  TDirectory* IsolationSelectionTool::getTemporaryDirectory(void) const
  {
    //
    // Create a unique directory in memory to hold the histograms:
    //
    gROOT->cd();
    TDirectory* tempDir = 0;
    int counter = 0;
    while (not tempDir) {
      // First, let's find a directory name that doesn't exist yet:
      std::stringstream dirname;
      dirname << "IsolationSelectionTempDir_%i" << counter;
      if (gROOT->GetDirectory((dirname.str()).c_str())) {
        ++counter;
        continue;
      }
      // Let's try to make this directory:
      tempDir = gROOT->mkdir((dirname.str()).c_str());
      if (not tempDir) {
        //      ::Fatal("IsolationSelectionTool::getTemporaryDirectory",
        //      ERROR_MESSAGE("Temporary directory could not be created"));
        throw std::runtime_error("Temporary directory could not be created");
      }
    }
    
    return tempDir;
  }  
}
